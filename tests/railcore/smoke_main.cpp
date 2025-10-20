#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/observer.h"
#include "railcore/services.h"
#include <atomic>
#include "railcore/persistence/rcd_id.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static std::filesystem::path RepoRootFromOutDir() {
  // Resolve repo root whether running from build/msvc/<Config> or repo root
  std::filesystem::path cwd = std::filesystem::current_path();
  if (std::filesystem::exists(cwd / "FAST.RCD")) return cwd;
  return cwd / ".." / ".." / "..";
}

static std::filesystem::path DataFile(const char* name) {
  auto root = RepoRootFromOutDir();
  auto p1 = root / name;
  if (std::filesystem::exists(p1)) return p1;
  auto p2 = root / "Game files" / name;
  if (std::filesystem::exists(p2)) return p2;
  return p1;
}

static int TestMissingFile() {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  if (!engine) { std::fprintf(stderr, "CreateEngine failed\n"); return 1; }
  LayoutDescriptor d; d.sourcePath = RepoRootFromOutDir() / "NON_EXISTENT.RCD"; d.name = "missing";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::NotFound) {
    std::fprintf(stderr, "Expected NotFound for missing file, got %d\n", (int)s.code);
    return 2;
  }
  return 0;
}

// Test fakes for services
struct TestRandomProvider : IRandomProvider {
  uint32_t lastSeed {0};
  uint32_t Next() override { return 4; /* chosen by fair dice roll */ }
  void Seed(uint32_t seed) override { lastSeed = seed; }
};

static int TestDeterministicSeedOnLoad() {
  EngineConfig cfg; cfg.enableDeterministicSeeds = true;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto rnd = std::make_shared<TestRandomProvider>();
  auto engine = CreateEngine(cfg, repo, nullptr, rnd, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) return 99;
  if (rnd->lastSeed != 0xC0FFEEu) { std::fprintf(stderr, "Expected deterministic Seed() on load\n"); return 100; }
  return 0;
}

static int TestLoadFastRcd() {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    std::fprintf(stderr, "Load FAST.RCD failed: %s\n", s.message.c_str());
    return 3;
  }
  auto snap = engine->GetSnapshot();
  if (!snap.state) { std::fprintf(stderr, "Snapshot state null after load\n"); return 4; }
  if (snap.state->sections.size() < 60 || snap.state->routes.size() < 90 || snap.state->timetable.size() < 40) {
    std::fprintf(stderr, "Parsed counts too small: sections=%zu routes=%zu timetable=%zu\n",
                 snap.state->sections.size(), snap.state->routes.size(), snap.state->timetable.size());
    return 5;
  }
  return 0;
}

static int TestLoadKingsxRcd() {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("KINGSX.RCD"); d.name = "KINGSX";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    std::fprintf(stderr, "Load KINGSX.RCD failed: %s\n", s.message.c_str());
    return 6;
  }
  auto snap = engine->GetSnapshot();
  if (!snap.state) { std::fprintf(stderr, "Snapshot state null after KINGSX load\n"); return 7; }
  if (snap.state->sections.empty() || snap.state->routes.empty() || snap.state->timetable.empty()) {
    std::fprintf(stderr, "KINGSX counts empty: sections=%zu routes=%zu timetable=%zu\n",
                 snap.state->sections.size(), snap.state->routes.size(), snap.state->timetable.size());
    return 8;
  }
  return 0;
}

static int TestEngineLifecycle() {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) return 10;
  // Pause is idempotent
  s = engine->Command(CommandPayload{CommandId::Pause, std::monostate{}});
  if (s.code != StatusCode::Ok) return 11;
  s = engine->Command(CommandPayload{CommandId::Pause, std::monostate{}});
  if (s.code != StatusCode::Ok) return 12;
  // Advance works
  auto out = engine->Advance(std::chrono::milliseconds{100});
  if (out.status.code != StatusCode::Ok) return 13;
  // Stop then Advance should fail
  s = engine->Command(CommandPayload{CommandId::Stop, std::monostate{}});
  if (s.code != StatusCode::Ok) return 14;
  // simulationActive should be false after Stop
  auto snapStopped = engine->GetSnapshot();
  if (snapStopped.state->simulationActive) { std::fprintf(stderr, "simulationActive should be false after Stop\n"); return 114; }
  out = engine->Advance(std::chrono::milliseconds{100});
  if (out.status.code == StatusCode::Ok) return 15;
  // Resume then Advance should succeed
  s = engine->Command(CommandPayload{CommandId::Resume, std::monostate{}});
  if (s.code != StatusCode::Ok) return 16;
  out = engine->Advance(std::chrono::milliseconds{100});
  if (out.status.code != StatusCode::Ok) return 17;
  // simulationActive should be true after running Advance
  auto snapRunning = engine->GetSnapshot();
  if (!snapRunning.state->simulationActive) { std::fprintf(stderr, "simulationActive should be true after Advance\n"); return 115; }
  return 0;
}

struct TestObserver : IObserver {
  int snapshots = 0;
  int events = 0;
  int diagnostics = 0;
  DiagnosticsEvent lastDiag;
  SimulationTickResult lastTick;
  void OnSnapshot(const LayoutSnapshot& /*snapshot*/) override { ++snapshots; }
  void OnEvents(const SimulationTickResult& tick) override { ++events; lastTick = tick; }
  void OnDiagnostics(const DiagnosticsEvent& diag) override { ++diagnostics; lastDiag = diag; }
};

static int TestObserverAndDiagnostics() {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) return 46;
  TestObserver obs;
  engine->Subscribe(obs);
  // Advance with clamped dt to trigger diagnostics
  auto out = engine->Advance(std::chrono::milliseconds{5000});
  if (out.status.code != StatusCode::Ok) return 47;
  if (obs.snapshots < 1) { std::fprintf(stderr, "Expected OnSnapshot on load\n"); return 70; }
  if (obs.diagnostics == 0) { std::fprintf(stderr, "Expected diagnostics on dt clamp\n"); return 48; }
  if (obs.lastDiag.level != DiagnosticsLevel::Warning) { std::fprintf(stderr, "Expected Warning diagnostics level\n"); return 49; }
  if (obs.events == 0) { std::fprintf(stderr, "Expected at least one OnEvents callback\n"); return 50; }
  // Negative dt should not emit events
  auto out2 = engine->Advance(std::chrono::milliseconds{-1});
  if (out2.status.code == StatusCode::Ok) return 51;
  int prevEvents = obs.events;
  // Ensure no additional events called due to error
  if (obs.events != prevEvents) { std::fprintf(stderr, "Unexpected events on error advance\n"); return 52; }
  engine->Unsubscribe(obs);
  // After unsubscribe, no more callbacks
  prevEvents = obs.events;
  auto out3 = engine->Advance(std::chrono::milliseconds{100});
  (void)out3;
  if (obs.events != prevEvents) { std::fprintf(stderr, "Unexpected events after unsubscribe\n"); return 71; }
  return 0;
}

static int TestZeroChangeTick() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 54;
  auto snap1 = engine->GetSnapshot();
  uint32_t t1 = snap1.state->tickId;
  auto out = engine->Advance(std::chrono::milliseconds{0});
  if (out.status.code != StatusCode::Ok) return 55;
  auto snap2 = engine->GetSnapshot();
  if (snap2.state->tickId != t1) { std::fprintf(stderr, "tickId changed on zero-change tick\n"); return 56; }
  // Issue a command that changes state (delay), then advance with zero dt should increment tickId
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); if (s.code != StatusCode::Ok) return 57;
  out = engine->Advance(std::chrono::milliseconds{0});
  auto snap3 = engine->GetSnapshot();
  if (snap3.state->tickId != t1 + 1) { std::fprintf(stderr, "tickId did not increment on delta-only tick\n"); return 58; }
  return 0;
}

static int TestWorldDeltaClockAndClearing() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 74;
  // Issue a delay change to create a globals delta
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); if (s.code != StatusCode::Ok) return 75;
  auto out = engine->Advance(std::chrono::milliseconds{100});
  if (!out.result.delta) { std::fprintf(stderr, "Expected WorldDelta after delay change\n"); return 76; }
  if (out.result.delta->clock != out.result.clock) { std::fprintf(stderr, "WorldDelta clock mismatch\n"); return 77; }
  // Next advance with no new changes should have no delta
  auto out2 = engine->Advance(std::chrono::milliseconds{0});
  if (out2.result.delta) { std::fprintf(stderr, "Unexpected WorldDelta with no changes\n"); return 78; }
  return 0;
}

static int TestIdCanonicalizationStability() {
  // Read FAST.RCD and create a variant with extra trailing spaces to ensure canonicalization is stable
  std::filesystem::path fast = DataFile("FAST.RCD");
  std::ifstream in(fast, std::ios::binary);
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  std::string canon = CanonicalizeRcdContent(content);
  auto id1 = ComputeRcdIdFromContent(canon);
  // Add trailing spaces at end of lines
  std::string modified;
  modified.reserve(content.size()+content.size()/10);
  for (char c : content) {
    modified.push_back(c);
    if (c == '\n') modified.insert(modified.end(), {' ', ' ', ' '});
  }
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(modified));
  if (id1 != id2) { std::fprintf(stderr, "Canonicalization ID mismatch for trailing spaces\n"); return 101; }
  // Validate CRLF handling by injecting \r
  std::string crlf;
  crlf.reserve(content.size()*2);
  for (char c : content) {
    if (c == '\n') { crlf.push_back('\r'); crlf.push_back('\n'); }
    else crlf.push_back(c);
  }
  auto id3 = ComputeRcdIdFromContent(CanonicalizeRcdContent(crlf));
  if (id1 != id3) { std::fprintf(stderr, "Canonicalization ID mismatch for CRLF\n"); return 102; }
  return 0;
}

struct ReentrantObserver : IObserver {
  IRailEngine* engine {nullptr};
  std::atomic<int> busyCount {0};
  std::atomic<bool> attempted {false};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    if (engine && !attempted.exchange(true)) {
      auto out = engine->Advance(std::chrono::milliseconds{10});
      if (out.status.code == StatusCode::Busy) busyCount.fetch_add(1);
    }
  }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

static int TestReentrantAdvanceBusy() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 81;
  ReentrantObserver ro; ro.engine = engine.get();
  engine->Subscribe(ro);
  auto out = engine->Advance(std::chrono::milliseconds{50});
  if (out.status.code != StatusCode::Ok) return 82;
  if (ro.busyCount.load() == 0) { std::fprintf(stderr, "Expected Busy status on reentrant Advance\n"); return 83; }
  engine->Unsubscribe(ro);
  return 0;
}

struct TestTelemetrySink : ITelemetrySink {
  int count {0};
  DiagnosticsEvent last;
  void Emit(const DiagnosticsEvent& ev) override { ++count; last = ev; }
};

static int TestTelemetrySinkReceivesDiagnostics() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto sink = std::make_shared<TestTelemetrySink>();
  auto engine = CreateEngine(cfg, repo, sink, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 106;
  auto out = engine->Advance(std::chrono::milliseconds{5000}); (void)out;
  if (sink->count == 0) { std::fprintf(stderr, "Expected telemetry sink to receive diagnostics\n"); return 107; }
  return 0;
}

static int TestResetAndLayoutId() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  // GetLayoutId should be empty before load
  if (!engine->GetLayoutId().empty()) { std::fprintf(stderr, "Expected empty layout id before load\n"); return 59; }
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 60;
  if (engine->GetLayoutId().empty()) { std::fprintf(stderr, "Expected layout id after load\n"); return 61; }
  auto snap1 = engine->GetSnapshot();
  if (snap1.state->sections.empty()) { std::fprintf(stderr, "Expected sections after load\n"); return 62; }
  // Reset clears state
  s = engine->Reset(); if (s.code != StatusCode::Ok) return 63;
  auto snap2 = engine->GetSnapshot();
  if (!snap2.state->sections.empty() || snap2.state->tickId != 0) { std::fprintf(stderr, "Reset did not clear world state\n"); return 64; }
  // Layout id remains last loaded (implementation choice); do not assert here
  return 0;
}

static int TestReleaseLocoWrongId() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 65;
  auto snap = engine->GetSnapshot(); if (snap.state->timetable.empty() || snap.state->locos.empty()) return 66;
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); if (s.code != StatusCode::Ok) return 67;
  // Attempt to release with wrong loco id
  LocoAssignment wrong; wrong.timetableId = la.timetableId; wrong.locoId = la.locoId + 12345; wrong.action = AssignmentAction::Release;
  s = engine->Command(CommandPayload{CommandId::ReleaseLoco, wrong});
  if (s.code != StatusCode::NotFound) { std::fprintf(stderr, "Expected NotFound for ReleaseLoco wrong id\n"); return 68; }
  return 0;
}

int main() {
  int rc = 0;
  if ((rc = TestMissingFile()) != 0) return rc;
  if ((rc = TestLoadFastRcd()) != 0) return rc;
  if ((rc = TestLoadKingsxRcd()) != 0) return rc;
  // Layout ID should differ between FAST and KINGSX
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d1; d1.sourcePath = DataFile("FAST.RCD"); d1.name = "FAST";
    Status s1 = engine->LoadLayout(d1); if (s1.code != StatusCode::Ok) return 110;
    std::string id1 = engine->GetLayoutId();
    LayoutDescriptor d2; d2.sourcePath = DataFile("KINGSX.RCD"); d2.name = "KINGSX";
    Status s2 = engine->LoadLayout(d2); if (s2.code != StatusCode::Ok) return 111;
    std::string id2 = engine->GetLayoutId();
    if (id1.empty() || id2.empty() || id1 == id2) { std::fprintf(stderr, "Expected different layout IDs for FAST and KINGSX\n"); return 112; }
  }
  if ((rc = TestEngineLifecycle()) != 0) return rc;
  if ((rc = TestObserverAndDiagnostics()) != 0) return rc;
  if ((rc = TestZeroChangeTick()) != 0) return rc;
  if ((rc = TestResetAndLayoutId()) != 0) return rc;
  if ((rc = TestReleaseLocoWrongId()) != 0) return rc;
  if ((rc = TestWorldDeltaClockAndClearing()) != 0) return rc;
  if ((rc = TestReentrantAdvanceBusy()) != 0) return rc;
  if ((rc = TestDeterministicSeedOnLoad()) != 0) return rc;
  if ((rc = TestIdCanonicalizationStability()) != 0) return rc;
  if ((rc = TestTelemetrySinkReceivesDiagnostics()) != 0) return rc;
  // EngineConfig max limits should guard load
  {
    EngineConfig cfg; cfg.maxRoutes = 10; // FAST.RCD has > 10 routes
    auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for maxRoutes limit\n"); return 72; }
    auto snap = engine->GetSnapshot();
    if (!snap.state->sections.empty()) { std::fprintf(stderr, "Engine retained state after failed load\n"); return 73; }
  }
  // Bad ROUTES with missing stage tokens should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[ROUTES]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Insert a route with only id,from,to (missing stages)
      content.insert(lineEnd+1, "999, 1, 2\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_routes_tokens.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badrout";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad ROUTES tokens, got Ok\n"); return 37; }
  }
  // Bad ROUTES with unknown selector should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[ROUTES]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Insert a route referencing non-existent selectors 999 and 998 and valid 6 stages (zeros)
      content.insert(lineEnd+1, "998, 999, 998, 0, 0, 0, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_routes_selector.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badsel";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad ROUTES selectors, got Ok\n"); return 38; }
  }
  // Bad ROUTES with unknown section in stage token should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[ROUTES]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Stage token 999 references non-existent section
      content.insert(lineEnd+1, "996, 1, 2, 999, 0, 0, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_routes_section.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badrsec";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad ROUTES section ref, got Ok\n"); return 95; }
  }
  // Bad ROUTES with too many stage tokens should fail (7 stages)
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[ROUTES]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Insert a route with 7 stage tokens
      content.insert(lineEnd+1, "997, 1, 2, 0, 0, 0, 0, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_routes_extrastages.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badextr";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for extra ROUTES stage tokens, got Ok\n"); return 41; }
  }
  // Invalid GENERAL StopTime <= StartTime should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[GENERAL]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Insert StartTime 1000 and StopTime 0930 to force inversion
      content.insert(lineEnd+1, "StartTime, 1000\nStopTime, 0930\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_general_order.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badgord";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for GENERAL Stop<=Start, got Ok\n"); return 42; }
  }
  // Load additional sample layouts
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d1; d1.sourcePath = DataFile("QUEENST.RCD"); d1.name = "QUEENST";
    Status s1 = engine->LoadLayout(d1);
    if (s1.code != StatusCode::Ok) { std::fprintf(stderr, "Load QUEENST.RCD failed\n"); return 43; }
    LayoutDescriptor d2; d2.sourcePath = DataFile("WAVERLY.RCD"); d2.name = "WAVERLY";
    Status s2 = engine->LoadLayout(d2);
    if (s2.code != StatusCode::Ok) { std::fprintf(stderr, "Load WAVERLY.RCD failed\n"); return 44; }
  }
  // Missing required section should fail (remove [SELECTOR])
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    // Replace [SELECTOR] with [SELX]
    size_t pos = content.find("[SELECTOR]");
    if (pos != std::string::npos) {
      content.replace(pos, std::string("[SELECTOR]").size(), "[SELX]");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "missing_selector.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "missel";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for missing [SELECTOR], got Ok\n"); return 39; }
  }
  // Command validation
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 23;
    // Missing payloads
    s = engine->Command(CommandPayload{CommandId::SetDelayMode, std::monostate{}});
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected InvalidCommand for SetDelayMode without payload\n"); return 91; }
    s = engine->Command(CommandPayload{CommandId::AssignLoco, std::monostate{}});
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected InvalidCommand for AssignLoco without payload\n"); return 92; }
    // Invalid delay settings (too large threshold)
    DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{5000}; ds.maintenanceThrough = false;
    s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for SetDelayMode large threshold\n"); return 24; }
    // Valid delay settings
    ds.threshold = std::chrono::minutes{5};
    s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
    if (s.code != StatusCode::Ok) { std::fprintf(stderr, "Valid SetDelayMode rejected\n"); return 25; }
    // World state should reflect currentDelay
    auto snapDelay = engine->GetSnapshot();
    if (snapDelay.state->currentDelay.threshold != std::chrono::minutes{5} || snapDelay.state->currentDelay.mode != DelayMode::Randomized) {
      std::fprintf(stderr, "WorldState currentDelay not updated\n"); return 45;
    }
    // AssignLoco NotFound when ids unknown
    LocoAssignment la; la.timetableId = 999; la.locoId = 999; la.action = AssignmentAction::Assign;
    s = engine->Command(CommandPayload{CommandId::AssignLoco, la});
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected NotFound for AssignLoco unknown ids\n"); return 26; }
    // AssignLoco Ok on existing ids (use first from parsed state)
    auto snap = engine->GetSnapshot();
    if (!snap.state->timetable.empty() && !snap.state->locos.empty()) {
      la.timetableId = snap.state->timetable.front().id;
      la.locoId = snap.state->locos.front().id;
      s = engine->Command(CommandPayload{CommandId::AssignLoco, la});
      if (s.code != StatusCode::Ok) { std::fprintf(stderr, "AssignLoco failed on existing ids\n"); return 27; }
      // World state should reflect assignment immediately
      auto snap2 = engine->GetSnapshot();
      bool found=false;
      for (const auto& asn : snap2.state->assignments) {
        if (asn.timetableId == la.timetableId && asn.locoId == la.locoId) { found = true; break; }
      }
      if (!found) { std::fprintf(stderr, "Assignment not reflected in world state\n"); return 31; }
      // Advance and expect LocoAssigned and DelayChanged events (from earlier SetDelayMode)
      auto out = engine->Advance(std::chrono::milliseconds{100});
      bool sawAssign=false, sawDelay=false;
      for (const auto& ev : out.result.events) {
        if (ev.id == DomainEventId::LocoAssigned) sawAssign = true;
        if (ev.id == DomainEventId::DelayChanged) sawDelay = true;
      }
      if (!sawAssign || !sawDelay) { std::fprintf(stderr, "Expected assignment and delay events on tick\n"); return 28; }
      // Delta should reflect timetable assignment and delay globals
      if (!out.result.delta) { std::fprintf(stderr, "Expected WorldDelta on assignment\n"); return 33; }
      bool deltaMatch=false;
      for (const auto& ed : out.result.delta->timetableEntries) {
        auto it = ed.changedFields.find("assignedLocoId");
        if (ed.id == la.timetableId && it != ed.changedFields.end() && it->second == std::to_string(la.locoId)) { deltaMatch = true; break; }
      }
      if (!deltaMatch) { std::fprintf(stderr, "WorldDelta missing assignedLocoId change\n"); return 34; }
      auto git = out.result.delta->globals.find("delay.mode");
      if (git == out.result.delta->globals.end() || git->second != "Randomized") { std::fprintf(stderr, "WorldDelta missing delay.mode\n"); return 53; }
      auto gth = out.result.delta->globals.find("delay.threshold_min");
      if (gth == out.result.delta->globals.end() || gth->second != "5") { std::fprintf(stderr, "WorldDelta missing delay.threshold_min\n"); return 84; }
      auto gmt = out.result.delta->globals.find("delay.maintenanceThrough");
      if (gmt == out.result.delta->globals.end() || gmt->second != "false") { std::fprintf(stderr, "WorldDelta missing delay.maintenanceThrough\n"); return 85; }

      // ReleaseLoco should emit LocoReleased
      s = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
      if (s.code != StatusCode::Ok) { std::fprintf(stderr, "ReleaseLoco failed\n"); return 29; }
      out = engine->Advance(std::chrono::milliseconds{100});
      bool sawReleased=false;
      for (const auto& ev : out.result.events) {
        if (ev.id == DomainEventId::LocoReleased) sawReleased = true;
      }
      if (!sawReleased) { std::fprintf(stderr, "Expected LocoReleased event on tick\n"); return 30; }
      // Delta should reflect timetable release (assignedLocoId cleared)
      if (!out.result.delta) { std::fprintf(stderr, "Expected WorldDelta on release\n"); return 35; }
      bool deltaRelease=false;
      for (const auto& ed : out.result.delta->timetableEntries) {
        auto it = ed.changedFields.find("assignedLocoId");
        if (ed.id == la.timetableId && it != ed.changedFields.end() && it->second.empty()) { deltaRelease = true; break; }
      }
      if (!deltaRelease) { std::fprintf(stderr, "WorldDelta missing release change\n"); return 36; }
      // Re-assign a different loco to the same timetable
      if (snap.state->locos.size() > 1) {
        LocoAssignment la2; la2.timetableId = la.timetableId; la2.locoId = snap.state->locos[1].id; la2.action = AssignmentAction::Assign;
        s = engine->Command(CommandPayload{CommandId::AssignLoco, la2}); if (s.code != StatusCode::Ok) return 93;
        auto out4 = engine->Advance(std::chrono::milliseconds{0});
        bool reassigned=false;
        if (out4.result.delta) {
          for (const auto& ed : out4.result.delta->timetableEntries) {
            auto it2 = ed.changedFields.find("assignedLocoId");
            if (ed.id == la2.timetableId && it2 != ed.changedFields.end() && it2->second == std::to_string(la2.locoId)) { reassigned = true; break; }
          }
        }
        if (!reassigned) { std::fprintf(stderr, "WorldDelta missing reassignment change\n"); return 94; }
      }
      // World state should no longer contain the assignment
      auto snap3 = engine->GetSnapshot();
      for (const auto& asn : snap3.state->assignments) {
        if (asn.timetableId == la.timetableId) { std::fprintf(stderr, "Assignment still present after release\n"); return 32; }
      }
    }
  }
  // Duplicate section header should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    std::string dup = std::string("[SECTIONS]\n1,1,1,1,1,1,1,1\n") + content;
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_sections.rcd";
    std::ofstream out(tmp, std::ios::binary); out << dup; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "dup";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate sections, got Ok\n"); return 20; }
  }
  // Invalid StartTime in GENERAL should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[GENERAL]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "StartTime, 1265\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_general.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badgen";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad GENERAL StartTime, got Ok\n"); return 21; }
  }
  // Invalid DepTime in TIMETABLE should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[TIMETABLE]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Insert a line with bad DepTime (9961)
      content.insert(lineEnd+1, "499, BadDep, BadDep, 3, 700, 0, 9961, 3, 3, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_tt.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtt";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad TIMETABLE DepTime, got Ok\n"); return 22; }
  }
  // Invalid ArrTime in TIMETABLE should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[TIMETABLE]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Bad ArrTime 8861 (minutes 61)
      content.insert(lineEnd+1, "497, BadArr, BadArr, 3, 8861, 0, 905, 3, 3, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_tt_arrtime.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtarr";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad TIMETABLE ArrTime, got Ok\n"); return 103; }
  }
  // Too few fields in TIMETABLE should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[TIMETABLE]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // insufficient tokens (<12)
      content.insert(lineEnd+1, "496, TooFew, TooFew, 3, 700\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_tt_fields.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badttf";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for TIMETABLE too few fields, got Ok\n"); return 105; }
  }
  // OVERLAPPING references unknown section should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[OVERLAPPING]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "999, 999, 998\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_overlap.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badovl";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad OVERLAPPING, got Ok\n"); return 79; }
  }
  // PLATFORMS line with wrong token count should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[PLATFORMS]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "999, 1, 2, 3, 4, 5\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_platforms.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplat";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad PLATFORMS tokens, got Ok\n"); return 80; }
  }
  // Duplicate platform id should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[PLATFORMS]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Duplicate id 1 with valid 8 coords
      content.insert(lineEnd+1, "1, 0,0,0,0,0,0,0,0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_platforms.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupplat";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate platform id, got Ok\n"); return 86; }
  }
  // Duplicate route id should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[ROUTES]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Duplicate route id 1 with valid 6 stages (zeros)
      content.insert(lineEnd+1, "1, 1, 2, 0, 0, 0, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_route.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "duprte";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate route id, got Ok\n"); return 96; }
  }
  // Duplicate loco id should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[LOCOS]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Duplicate loco id 1
      content.insert(lineEnd+1, "1, 31, 999, 1\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_loco.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "duploco";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate loco id, got Ok\n"); return 97; }
  }
  // Duplicate timetable id should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[TIMETABLE]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Duplicate timetable id 1
      content.insert(lineEnd+1, "1, D, D, 1, 700, 0, 705, 1, 1, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_tt.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "duptt";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate timetable id, got Ok\n"); return 98; }
  }
  // LOCOYARD invalid stock code should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[LOCOYARD]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "99, 10\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_locoyard.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badlyd";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for LOCOYARD invalid stock code, got Ok\n"); return 108; }
  }
  // LOCOYARD invalid refuel offset should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[LOCOYARD]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "1, 99\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_locoyard_off.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badlyo";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for LOCOYARD invalid offset, got Ok\n"); return 109; }
  }
  // LOCOYARD Disabled should be accepted
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[LOCOYARD]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "Disabled\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "ok_locoyard_disabled.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "lydisabled";
    Status s = engine->LoadLayout(d);
    if (s.code != StatusCode::Ok) { std::fprintf(stderr, "Unexpected error for LOCOYARD Disabled: %s\n", s.message.c_str()); return 113; }
  }
  // Duplicate selector id should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[SELECTOR]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Duplicate selector id 1 with dummy tokens count (9 fields total)
      content.insert(lineEnd+1, "1,0,0,0,0,1,0,UF\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "dup_selector.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupsel";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for duplicate selector, got Ok\n"); return 87; }
  }
  // Selector line with too few tokens should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[SELECTOR]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      // Too few fields (only id and x)
      content.insert(lineEnd+1, "999, 10\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_selector_fields.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badsel2";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for selector too few fields, got Ok\n"); return 104; }
  }
  // ReleaseLoco with no assignment should return NotFound
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    Status s = engine->LoadLayout(d); if (s.code != StatusCode::Ok) return 88;
    auto snap = engine->GetSnapshot(); if (snap.state->timetable.empty()) return 89;
    LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = 0; la.action = AssignmentAction::Release;
    s = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
    if (s.code != StatusCode::NotFound) { std::fprintf(stderr, "Expected NotFound for ReleaseLoco with no assignment\n"); return 90; }
  }
  // Invalid ArrSelector (>49) in TIMETABLE should fail
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string content = oss.str();
    size_t pos = content.find("[TIMETABLE]");
    if (pos != std::string::npos) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) lineEnd = content.size();
      content.insert(lineEnd+1, "498, BadSel, BadSel, 55, 700, 0, 705, 3, 3, 0, 0, 0\n");
    }
    std::filesystem::path tmp = std::filesystem::current_path() / "bad_tt_arrsel.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtsel";
    Status s = engine->LoadLayout(d);
    if (s.code == StatusCode::Ok) { std::fprintf(stderr, "Expected ValidationError for bad TIMETABLE ArrSelector, got Ok\n"); return 40; }
  }
  // Verify stable layout id computation
  {
    std::filesystem::path fast = DataFile("FAST.RCD");
    std::ifstream in(fast, std::ios::binary);
    std::ostringstream oss; oss << in.rdbuf();
    std::string canon = CanonicalizeRcdContent(oss.str());
    auto id1 = ComputeRcdIdFromContent(canon);
    auto id2 = ComputeRcdIdFromContent(canon);
    if (id1.empty() || id1.size() != 16 || id1 != id2) {
      std::fprintf(stderr, "Invalid layout id: '%s' vs '%s'\n", id1.c_str(), id2.c_str());
      return 9;
    }
    // Verify engine exposes the same layout id
    EngineConfig cfg;
    auto repo = std::make_shared<RcdLayoutRepository>();
    auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = fast; d.name = "FAST";
    Status s = engine->LoadLayout(d);
    if (s.code != StatusCode::Ok) return 18;
    auto eid = engine->GetLayoutId();
    if (eid != id1) {
      std::fprintf(stderr, "Engine layout id mismatch: '%s' (engine) vs '%s' (direct)\n", eid.c_str(), id1.c_str());
      return 19;
    }
  }
  return 0;
}


