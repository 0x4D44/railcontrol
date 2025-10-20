#include <algorithm>
#include <mutex>
#include <map>
#include <variant>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/services.h"
#include "railcore/types.h"

namespace RailCore {

namespace {

class EngineStub : public IRailEngine {
public:
  EngineStub(const EngineConfig& cfg,
             std::shared_ptr<ILayoutRepository> repo,
             std::shared_ptr<ITelemetrySink> telemetry,
             std::shared_ptr<IRandomProvider> random,
             std::shared_ptr<IClockService> clock,
             std::shared_ptr<IPersistenceService> persistence)
    : config_(cfg), repo_(std::move(repo)), telemetry_(std::move(telemetry)),
      random_(std::move(random)), clock_(std::move(clock)), persistence_(std::move(persistence)) {}

  Status LoadLayout(const LayoutDescriptor& desc) override {
    std::lock_guard<std::mutex> lock(mu_);
    if (!repo_) return Status{StatusCode::InternalError, "No layout repository"};
    WorldState ws;
    LayoutDescriptor ld = desc; // make a mutable copy to receive layout id
    Status s = repo_->Load(ld, ws);
    if (s.code != StatusCode::Ok) return s;
    // Deterministic RNG when configured
    if (config_.enableDeterministicSeeds && random_) {
      random_->Seed(0xC0FFEEu);
    }
    // Enforce EngineConfig bounds
    if (ws.locos.size() > config_.maxActiveTrains) {
      return Status{StatusCode::ValidationError, "Active trains exceed configured max"};
    }
    if (ws.sections.size() > config_.maxSections) {
      return Status{StatusCode::ValidationError, "Sections exceed configured max"};
    }
    if (ws.routes.size() > config_.maxRoutes) {
      return Status{StatusCode::ValidationError, "Routes exceed configured max"};
    }
    if (ws.timetable.size() > config_.maxTimetableEntries) {
      return Status{StatusCode::ValidationError, "Timetable entries exceed configured max"};
    }
    state_ = std::make_shared<WorldState>(std::move(ws));
    engineState_ = EngineState::Paused;
    state_->simulationActive = false;
    lastTick_ = std::chrono::milliseconds{0};
    layoutId_ = ld.id;
    NotifySnapshot();
    return Ok();
  }

  AdvanceOutcome Advance(std::chrono::milliseconds dt) override {
    std::lock_guard<std::mutex> lock(mu_);
    AdvanceOutcome out{Status{StatusCode::Ok, {}}, {}};
    if (!state_) {
      out.status = Status{StatusCode::InvalidCommand, "No layout loaded"};
      return out;
    }
    if (engineState_ == EngineState::Stopped) {
      out.status = Status{StatusCode::InvalidCommand, "Engine stopped"};
      return out;
    }
    if (inProgress_) {
      out.status = Status{StatusCode::Busy, "Advance in progress"};
      return out;
    }
    if (dt < std::chrono::milliseconds{0}) {
      out.status = Status{StatusCode::ValidationError, "Negative dt"};
      return out;
    }
    if (dt > maxStep()) {
      // clamp and optionally warn via telemetry
      dt = maxStep();
      EmitDiagnostics(DiagnosticsLevel::Warning, "Advance dt clamped to maxStep");
    }
    inProgress_ = true;
    engineState_ = EngineState::Running;
    state_->simulationActive = true;
    // Determine whether this tick represents a state change
    bool willEmitDelta = !pendingAssignmentDelta_.empty() || !pendingGlobalsDelta_.empty();
    bool changed = (dt > std::chrono::milliseconds{0}) || willEmitDelta;
    if (changed) {
      state_->tickId += 1;
    }
    state_->clock += dt;
    lastTick_ = dt;
    out.result.clock = state_->clock;
    out.result.wallTime = dt;
    // Flush pending domain events on tick
    out.result.events.swap(pendingEvents_);
    // Emit WorldDelta for assignment and global changes
    if (!pendingAssignmentDelta_.empty() || !pendingGlobalsDelta_.empty()) {
      auto wd = std::make_shared<WorldDelta>();
      wd->clock = state_->clock;
      for (const auto& kv : pendingAssignmentDelta_) {
        EntityDelta ed; ed.id = kv.first;
        if (kv.second >= 0) {
          ed.changedFields["assignedLocoId"] = std::to_string(kv.second);
        } else {
          ed.changedFields["assignedLocoId"] = ""; // cleared
        }
        wd->timetableEntries.push_back(std::move(ed));
      }
      pendingAssignmentDelta_.clear();
      if (!pendingGlobalsDelta_.empty()) {
        wd->globals = pendingGlobalsDelta_;
        pendingGlobalsDelta_.clear();
      }
      out.result.delta = wd;
    }
    NotifyEvents(out.result);
    inProgress_ = false;
    return out;
  }

  Status Reset() override {
    std::lock_guard<std::mutex> lock(mu_);
    state_.reset();
    engineState_ = EngineState::Idle;
    lastTick_ = std::chrono::milliseconds{0};
    return Ok();
  }

  Status Command(const CommandPayload& cmd) override {
    std::lock_guard<std::mutex> lock(mu_);
    switch (cmd.id) {
      case CommandId::Pause:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        if (engineState_ == EngineState::Paused) return Status{StatusCode::Ok, "no-op"};
        engineState_ = EngineState::Paused;
        return Ok();
      case CommandId::Resume:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        if (engineState_ == EngineState::Running) return Status{StatusCode::Ok, "no-op"};
        engineState_ = EngineState::Running;
        return Ok();
      case CommandId::Stop:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        engineState_ = EngineState::Stopped;
        if (state_) state_->simulationActive = false;
        return Ok();
      case CommandId::SetDelayMode:
      {
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        const DelaySettings* ds = std::get_if<DelaySettings>(&cmd.data);
        if (!ds) return Status{StatusCode::InvalidCommand, "SetDelayMode missing payload"};
        // Basic validation: threshold minutes in [0, 1440)
        auto mins = ds->threshold.count();
        if (mins < 0 || mins >= 1440) return Status{StatusCode::ValidationError, "Delay threshold out of range"};
        currentDelay_ = *ds; // store for future ticks
        if (state_) state_->currentDelay = currentDelay_;
        // Emit a DelayChanged event and a delta for globals on next tick
        DomainEvent ev; ev.id = DomainEventId::DelayChanged;
        std::map<std::string,std::string> m;
        m["mode"] = (ds->mode == DelayMode::None ? "None" : (ds->mode == DelayMode::Randomized ? "Randomized" : "MaintenanceOnly"));
        m["threshold_min"] = std::to_string(ds->threshold.count());
        m["maintenanceThrough"] = ds->maintenanceThrough ? "true" : "false";
        ev.payload = m;
        pendingEvents_.push_back(std::move(ev));
        pendingGlobalsDelta_["delay.mode"] = m["mode"];
        pendingGlobalsDelta_["delay.threshold_min"] = m["threshold_min"];
        pendingGlobalsDelta_["delay.maintenanceThrough"] = m["maintenanceThrough"];
        return Ok();
      }
      case CommandId::AssignLoco:
      {
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        const LocoAssignment* la = std::get_if<LocoAssignment>(&cmd.data);
        if (!la) return Status{StatusCode::InvalidCommand, "AssignLoco missing payload"};
        bool ttFound = false, locoFound = false;
        for (const auto& tt : state_->timetable) { if (tt.id == la->timetableId) { ttFound = true; break; } }
        for (const auto& l : state_->locos) { if (l.id == la->locoId) { locoFound = true; break; } }
        if (!ttFound) return Status{StatusCode::NotFound, "Unknown timetable id"};
        if (!locoFound) return Status{StatusCode::NotFound, "Unknown loco id"};
        // Record assignment in internal map and reflect in world state
        assignments_[la->timetableId] = la->locoId;
        pendingAssignmentDelta_[la->timetableId] = static_cast<int>(la->locoId);
        bool updated = false;
        for (auto& asn : state_->assignments) {
          if (asn.timetableId == la->timetableId) { asn.locoId = la->locoId; updated = true; break; }
        }
        if (!updated) state_->assignments.push_back({la->timetableId, la->locoId});
        // Emit a LocoAssigned event on next tick
        DomainEvent ev; ev.id = DomainEventId::LocoAssigned;
        std::map<std::string,std::string> m;
        m["timetableId"] = std::to_string(la->timetableId);
        m["locoId"] = std::to_string(la->locoId);
        ev.payload = m;
        pendingEvents_.push_back(std::move(ev));
        return Ok();
      }
      case CommandId::ReleaseLoco:
      {
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        const LocoAssignment* la = std::get_if<LocoAssignment>(&cmd.data);
        if (!la) return Status{StatusCode::InvalidCommand, "ReleaseLoco missing payload"};
        auto it = assignments_.find(la->timetableId);
        if (it == assignments_.end()) return Status{StatusCode::NotFound, "No assignment for timetable"};
        if (la->locoId != 0 && it->second != la->locoId) return Status{StatusCode::NotFound, "Different loco assigned"};
        assignments_.erase(it);
        pendingAssignmentDelta_[la->timetableId] = -1;
        // Reflect in world state vector
        for (auto iter = state_->assignments.begin(); iter != state_->assignments.end(); ++iter) {
          if (iter->timetableId == la->timetableId && (la->locoId == 0 || iter->locoId == la->locoId)) {
            state_->assignments.erase(iter);
            break;
          }
        }
        DomainEvent ev; ev.id = DomainEventId::LocoReleased;
        std::map<std::string,std::string> m;
        m["timetableId"] = std::to_string(la->timetableId);
        if (la->locoId != 0) m["locoId"] = std::to_string(la->locoId);
        ev.payload = m;
        pendingEvents_.push_back(std::move(ev));
        return Ok();
      }
      default:
        return Status{StatusCode::InvalidCommand, "Unknown command"};
    }
  }

  LayoutSnapshot GetSnapshot() const override {
    std::lock_guard<std::mutex> lock(mu_);
    LayoutSnapshot snap;
    snap.state = state_ ? state_ : std::make_shared<WorldState>();
    snap.snapshotClock = snap.state->clock;
    return snap;
  }

  std::string GetLayoutId() const override {
    std::lock_guard<std::mutex> lock(mu_);
    return layoutId_;
  }

  void Subscribe(IObserver& obs) override {
    std::lock_guard<std::mutex> lock(mu_);
    observers_.push_back(&obs);
    if (state_) {
      LayoutSnapshot snap; snap.state = state_; snap.snapshotClock = state_->clock;
      obs.OnSnapshot(snap);
    }
  }

  void Unsubscribe(IObserver& obs) override {
    std::lock_guard<std::mutex> lock(mu_);
    observers_.erase(std::remove(observers_.begin(), observers_.end(), &obs), observers_.end());
  }

private:
  std::chrono::milliseconds maxStep() const { return std::chrono::milliseconds{1000}; }

  void NotifySnapshot() {
    if (!state_) return;
    LayoutSnapshot snap;
    snap.state = state_;
    snap.snapshotClock = state_->clock;
    for (auto* o : observers_) {
      o->OnSnapshot(snap);
    }
  }

  void NotifyEvents(const SimulationTickResult& tick) {
    for (auto* o : observers_) {
      o->OnEvents(tick);
    }
  }

  void EmitDiagnostics(DiagnosticsLevel level, const std::string& message) {
    DiagnosticsEvent ev; ev.level = level; ev.message = message; ev.timestamp = std::chrono::system_clock::now();
    for (auto* o : observers_) { o->OnDiagnostics(ev); }
    if (telemetry_) telemetry_->Emit(ev);
  }

  enum class EngineState { Idle, Paused, Running, Stopped };

  EngineConfig config_{};
  std::shared_ptr<ILayoutRepository> repo_;
  std::shared_ptr<ITelemetrySink> telemetry_;
  std::shared_ptr<IRandomProvider> random_;
  std::shared_ptr<IClockService> clock_;
  std::shared_ptr<IPersistenceService> persistence_;

  mutable std::mutex mu_;
  std::shared_ptr<WorldState> state_;
  EngineState engineState_ {EngineState::Idle};
  bool inProgress_ {false};
  std::chrono::milliseconds lastTick_ {0};
  std::vector<IObserver*> observers_;
  std::string layoutId_;
  DelaySettings currentDelay_{};
  std::vector<DomainEvent> pendingEvents_;
  std::map<uint32_t,uint32_t> assignments_;
  std::map<uint32_t,int> pendingAssignmentDelta_;
  std::map<std::string,std::string> pendingGlobalsDelta_;
};

} // namespace

std::shared_ptr<IRailEngine> CreateEngine(
  const EngineConfig& config,
  std::shared_ptr<ILayoutRepository> repo,
  std::shared_ptr<ITelemetrySink> telemetry,
  std::shared_ptr<IRandomProvider> random,
  std::shared_ptr<IClockService> clock,
  std::shared_ptr<IPersistenceService> persistence) {
  return std::make_shared<EngineStub>(config, std::move(repo), std::move(telemetry), std::move(random), std::move(clock), std::move(persistence));
}

} // namespace RailCore
