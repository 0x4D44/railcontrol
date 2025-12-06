#include <algorithm>
#include <mutex>
#include <map>
#include <set>
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
    std::unique_lock<std::mutex> lock(mu_);
    if (!repo_) return Status{StatusCode::InternalError, "No layout repository"};
    WorldState ws;
    LayoutDescriptor ld = desc;
    Status s = repo_->Load(ld, ws);
    if (s.code != StatusCode::Ok) return s;
    if (config_.enableDeterministicSeeds && random_) {
      random_->Seed(0xC0FFEEu);
    }
    if (ws.locos.size() > config_.maxActiveTrains)
      return Status{StatusCode::ValidationError, "Active trains exceed configured max"};
    if (ws.sections.size() > config_.maxSections)
      return Status{StatusCode::ValidationError, "Sections exceed configured max"};
    if (ws.routes.size() > config_.maxRoutes)
      return Status{StatusCode::ValidationError, "Routes exceed configured max"};
    if (ws.timetable.size() > config_.maxTimetableEntries)
      return Status{StatusCode::ValidationError, "Timetable entries exceed configured max"};

    state_ = std::make_shared<WorldState>(std::move(ws));
    engineState_ = EngineState::Paused;
    state_->simulationActive = false;
    lastTick_ = std::chrono::milliseconds{0};
    layoutId_ = ld.id;
    lock.unlock();
    NotifySnapshot();
    return Ok();
  }

  AdvanceOutcome Advance(std::chrono::milliseconds dt) override {
    std::unique_lock<std::mutex> lock(mu_);
    AdvanceOutcome out{Status{StatusCode::Ok, {}}, {}};
    if (!state_) { out.status = Status{StatusCode::InvalidCommand, "No layout loaded"}; return out; }
    if (engineState_ == EngineState::Stopped) { out.status = Status{StatusCode::InvalidCommand, "Engine stopped"}; return out; }
    if (engineState_ == EngineState::Paused) {
      state_->simulationActive = false;
      out.result.clock = state_->clock;
      out.result.wallTime = std::chrono::milliseconds{0};
      return out;
    }
    if (inProgress_) { out.status = Status{StatusCode::Busy, "Advance in progress"}; return out; }
    if (dt < std::chrono::milliseconds{0}) { out.status = Status{StatusCode::ValidationError, "Negative dt"}; return out; }

    bool emitClampDiag = false;
    if (dt > maxStep()) { dt = maxStep(); emitClampDiag = true; }

    inProgress_ = true;
    engineState_ = EngineState::Running;
    state_->simulationActive = true;

    auto newClock = state_ ? (state_->clock + dt) : dt;
    const std::chrono::milliseconds kArriveAfter{200};
    const std::chrono::milliseconds kDepartAfter{500};

    if (state_) {
      for (const auto& kv : assignments_) {
        uint32_t tt = kv.first;
        bool isArrived = arrived_.count(tt) != 0;
        bool isDeparted = departed_.count(tt) != 0;
        auto it = assignmentStartClock_.find(tt);
        if (it != assignmentStartClock_.end()) {
          auto elapsed = newClock - it->second;
          if (!isArrived && elapsed >= kArriveAfter) {
            DomainEvent ev; ev.id = DomainEventId::TrainArrived;
            std::map<std::string,std::string> m; m["timetableId"] = std::to_string(tt); m["locoId"] = std::to_string(kv.second);
            ev.payload = m; pendingEvents_.push_back(std::move(ev));
            EntityDelta ed; ed.id = tt; ed.changedFields["arrived"] = "true";
            pendingTimetableExtraDeltas_.push_back(std::move(ed));
            arrived_.insert(tt); isArrived = true;
          }
          if (isArrived && !isDeparted && elapsed >= kDepartAfter) {
            DomainEvent ev; ev.id = DomainEventId::TrainDeparted;
            std::map<std::string,std::string> m; m["timetableId"] = std::to_string(tt); m["locoId"] = std::to_string(kv.second);
            ev.payload = m; pendingEvents_.push_back(std::move(ev));
            EntityDelta ed; ed.id = tt; ed.changedFields["departed"] = "true";
            pendingTimetableExtraDeltas_.push_back(std::move(ed));
            departed_.insert(tt); isDeparted = true;
          }
          if (dt > std::chrono::milliseconds{0}) {
            int stage = 0; if (isDeparted) stage = 2; else if (isArrived) stage = 1; else stage = 0;
            EntityDelta ed; ed.id = tt;
            ed.changedFields["stage"] = std::to_string(stage);
            ed.changedFields["progressMs"] = std::to_string(static_cast<long long>(elapsed.count()));
            {
              int idx = 0;
              std::chrono::milliseconds slice{0};
              if (elapsed > std::chrono::milliseconds{0}) {
                slice = kDepartAfter / 6;
                if (slice.count() > 0) {
                  idx = static_cast<int>(std::min<int64_t>(5, elapsed / slice));
                  if (idx < 0) idx = 0;
                }
              }
              ed.changedFields["stageIndex"] = std::to_string(idx);
              if (slice.count() > 0) {
                auto bucketElapsed = elapsed - (slice * idx);
                if (bucketElapsed < std::chrono::milliseconds{0}) bucketElapsed = std::chrono::milliseconds{0};
                ed.changedFields["stageBucketElapsedMs"] = std::to_string(static_cast<long long>(bucketElapsed.count()));
              }
              auto itMap = assignmentRoute_.find(tt);
              if (itMap != assignmentRoute_.end() && itMap->second != 0) {
                ed.changedFields["routeId"] = std::to_string(itMap->second);
                // Provide current stage primary/secondary for the mapped route.
                // If later stages are zeros, fall back to the most recent non-zero stage <= idx.
                for (const auto& r : state_->routes) {
                  if (r.id == itMap->second) {
                    int eff = idx;
                    while (eff > 0 && r.stages[eff].primary == 0 && r.stages[eff].secondary == 0) { --eff; }
                    const auto& st = r.stages[eff];
                    ed.changedFields["stagePrimary"] = std::to_string(st.primary);
                    ed.changedFields["stageSecondary"] = std::to_string(st.secondary);
                    break;
                  }
                }
              }
            }
            pendingTimetableExtraDeltas_.push_back(std::move(ed));
          }
        }
      }
    }

    bool willEmitDelta = !pendingAssignmentDelta_.empty() || !pendingGlobalsDelta_.empty() || !pendingTimetableExtraDeltas_.empty();
    bool changed = (dt > std::chrono::milliseconds{0}) || willEmitDelta;
    if (changed && state_) { state_->tickId += 1; }
    if (state_) state_->clock = newClock;
    lastTick_ = dt;
    out.result.clock = state_->clock;
    out.result.wallTime = dt;
    out.result.events.swap(pendingEvents_);
    if (!pendingAssignmentDelta_.empty() || !pendingGlobalsDelta_.empty() || !pendingTimetableExtraDeltas_.empty()) {
      auto wd = std::make_shared<WorldDelta>();
      wd->clock = state_->clock;
      for (const auto& kv : pendingAssignmentDelta_) {
        EntityDelta ed; ed.id = kv.first;
        if (kv.second >= 0) ed.changedFields["assignedLocoId"] = std::to_string(kv.second); else ed.changedFields["assignedLocoId"] = "";
        wd->timetableEntries.push_back(std::move(ed));
      }
      pendingAssignmentDelta_.clear();
      for (auto& ed : pendingTimetableExtraDeltas_) { wd->timetableEntries.push_back(std::move(ed)); }
      pendingTimetableExtraDeltas_.clear();
      if (!pendingGlobalsDelta_.empty()) { wd->globals = pendingGlobalsDelta_; pendingGlobalsDelta_.clear(); }
      out.result.delta = wd;
    }

    std::vector<IObserver*> observersCopy = observers_;
    SimulationTickResult tickCopy = out.result;
    lock.unlock();
    if (emitClampDiag) { EmitDiagnostics(DiagnosticsLevel::Warning, "Advance dt clamped to maxStep"); }
    for (auto* o : observersCopy) { try { o->OnEvents(tickCopy); } catch (...) { /* observer exception ignored to ensure all observers notified */ } }
    lock.lock();
    inProgress_ = false;
    return out;
  }

  Status Reset() override {
    std::lock_guard<std::mutex> lock(mu_);
    state_.reset(); engineState_ = EngineState::Idle; lastTick_ = std::chrono::milliseconds{0};
    return Ok();
  }

  Status Command(const CommandPayload& cmd) override {
    std::lock_guard<std::mutex> lock(mu_);
    switch (cmd.id) {
      case CommandId::Pause:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        if (engineState_ == EngineState::Paused) return Ok();
        engineState_ = EngineState::Paused; return Ok();
      case CommandId::Resume:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        if (engineState_ == EngineState::Running) return Ok();
        engineState_ = EngineState::Running; return Ok();
      case CommandId::Stop:
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        engineState_ = EngineState::Stopped; if (state_) state_->simulationActive = false; return Ok();
      case CommandId::SetDelayMode:
      {
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        const DelaySettings* ds = std::get_if<DelaySettings>(&cmd.data);
        if (!ds) return Status{StatusCode::InvalidCommand, "SetDelayMode missing payload"};
        auto mins = ds->threshold.count(); if (mins < 0 || mins >= 1440) return Status{StatusCode::ValidationError, "Delay threshold out of range"};
        currentDelay_ = *ds; if (state_) state_->currentDelay = currentDelay_;
        DomainEvent ev; ev.id = DomainEventId::DelayChanged; std::map<std::string,std::string> m;
        m["mode"] = (ds->mode == DelayMode::None ? "None" : (ds->mode == DelayMode::Randomized ? "Randomized" : "MaintenanceOnly"));
        m["thresholdMinutes"] = std::to_string(ds->threshold.count());
        m["maintenanceThrough"] = ds->maintenanceThrough ? "true" : "false";
        ev.payload = m; pendingEvents_.push_back(std::move(ev));
        pendingGlobalsDelta_["delay.mode"] = m["mode"];
        pendingGlobalsDelta_["delay.thresholdMinutes"] = m["thresholdMinutes"];
        pendingGlobalsDelta_["delay.threshold_min"] = m["thresholdMinutes"];
        pendingGlobalsDelta_["delay.maintenanceThrough"] = m["maintenanceThrough"];
        return Ok();
      }
      case CommandId::AssignLoco:
      {
        if (!state_) return Status{StatusCode::InvalidCommand, "No layout loaded"};
        const LocoAssignment* la = std::get_if<LocoAssignment>(&cmd.data);
        if (!la) return Status{StatusCode::InvalidCommand, "AssignLoco missing payload"};
        bool ttFound=false, locoFound=false;
        for (const auto& tt : state_->timetable) if (tt.id == la->timetableId) { ttFound = true; break; }
        for (const auto& l : state_->locos) if (l.id == la->locoId) { locoFound = true; break; }
        if (!ttFound) return Status{StatusCode::NotFound, "Unknown timetable id"};
        if (!locoFound) return Status{StatusCode::NotFound, "Unknown loco id"};
        assignments_[la->timetableId] = la->locoId;
        assignmentStartClock_[la->timetableId] = state_->clock;
        {
          uint32_t arrSel = 0;
          for (const auto& tt : state_->timetable) if (tt.id == la->timetableId) { arrSel = tt.arrSelector; break; }
          uint32_t chosenRouteId = 0;
          if (arrSel > 0) {
            for (const auto& r : state_->routes) {
              if (r.fromSelector == arrSel) {
                if (chosenRouteId == 0 || r.id < chosenRouteId) chosenRouteId = r.id;
              }
            }
          }
          if (chosenRouteId != 0) assignmentRoute_[la->timetableId] = chosenRouteId; else assignmentRoute_.erase(la->timetableId);
        }
        arrived_.erase(la->timetableId); departed_.erase(la->timetableId);
        pendingAssignmentDelta_[la->timetableId] = static_cast<int>(la->locoId);
        bool updated=false; for (auto& asn : state_->assignments) { if (asn.timetableId == la->timetableId) { asn.locoId = la->locoId; updated=true; break; } }
        if (!updated) state_->assignments.push_back({la->timetableId, la->locoId});
        DomainEvent ev; ev.id = DomainEventId::LocoAssigned; std::map<std::string,std::string> m; m["timetableId"] = std::to_string(la->timetableId); m["locoId"] = std::to_string(la->locoId); ev.payload = m; pendingEvents_.push_back(std::move(ev));
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
        assignmentStartClock_.erase(la->timetableId);
        assignmentRoute_.erase(la->timetableId);
        arrived_.erase(la->timetableId);
        departed_.erase(la->timetableId);
        pendingAssignmentDelta_[la->timetableId] = -1;
        for (auto iter = state_->assignments.begin(); iter != state_->assignments.end(); ) {
          if (iter->timetableId == la->timetableId && (la->locoId == 0 || iter->locoId == la->locoId)) iter = state_->assignments.erase(iter); else ++iter;
        }
        DomainEvent ev; ev.id = DomainEventId::LocoReleased; std::map<std::string,std::string> m; m["timetableId"] = std::to_string(la->timetableId); if (la->locoId != 0) m["locoId"] = std::to_string(la->locoId); ev.payload = m; pendingEvents_.push_back(std::move(ev));
        return Ok();
      }
      default:
        return Status{StatusCode::InvalidCommand, "Unknown command"};
    }
  }

  LayoutSnapshot GetSnapshot() const override {
    std::lock_guard<std::mutex> lock(mu_);
    LayoutSnapshot snap;
    if (state_) {
      snap.state = std::make_shared<WorldState>(*state_);
    } else {
      snap.state = std::make_shared<WorldState>();
    }
    snap.snapshotClock = snap.state->clock;
    return snap;
  }

  std::string GetLayoutId() const override {
    std::lock_guard<std::mutex> lock(mu_); return layoutId_;
  }

  void Subscribe(IObserver& obs) override {
    LayoutSnapshot snap; {
      std::lock_guard<std::mutex> lock(mu_);
      observers_.push_back(&obs);
      if (state_) { snap.state = state_; snap.snapshotClock = state_->clock; }
    }
    if (snap.state) { try { obs.OnSnapshot(snap); } catch (...) { /* observer exception ignored */ } }
  }

  void Unsubscribe(IObserver& obs) override {
    std::lock_guard<std::mutex> lock(mu_);
    observers_.erase(std::remove(observers_.begin(), observers_.end(), &obs), observers_.end());
  }

private:
  std::chrono::milliseconds maxStep() const { return std::chrono::milliseconds{1000}; }

  void NotifySnapshot() {
    std::vector<IObserver*> observersCopy; LayoutSnapshot snap; {
      std::lock_guard<std::mutex> lock(mu_);
      if (!state_) return; snap.state = state_; snap.snapshotClock = state_->clock; observersCopy = observers_;
    }
    for (auto* o : observersCopy) { try { o->OnSnapshot(snap); } catch (...) { /* observer exception ignored to ensure all observers notified */ } }
  }

  void EmitDiagnostics(DiagnosticsLevel level, const std::string& message) {
    DiagnosticsEvent ev; ev.level = level; ev.message = message; ev.timestamp = std::chrono::system_clock::now();
    std::vector<IObserver*> observersCopy; { std::lock_guard<std::mutex> lock(mu_); observersCopy = observers_; }
    for (auto* o : observersCopy) { try { o->OnDiagnostics(ev); } catch (...) { /* observer exception ignored to ensure all observers notified */ } }
    if (telemetry_) { try { telemetry_->Emit(ev); } catch (...) { /* telemetry exception ignored */ } }
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
  std::vector<EntityDelta> pendingTimetableExtraDeltas_;
  std::map<uint32_t, SimulationClock> assignmentStartClock_;
  std::map<uint32_t, uint32_t> assignmentRoute_;
  std::set<uint32_t> arrived_;
  std::set<uint32_t> departed_;
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
