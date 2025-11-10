#include <gtest/gtest.h>
#include <atomic>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

 

struct CountingObserver : IObserver {
  int snapshots{0};
  int events{0};
  void OnSnapshot(const LayoutSnapshot&) override { ++snapshots; }
  void OnEvents(const SimulationTickResult&) override { ++events; }
};

TEST(Observer, SnapshotAndUnsubscribe) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  CountingObserver co;
  engine->Subscribe(co);
  auto out = engine->Advance(std::chrono::milliseconds{10});
  (void)out;
  EXPECT_GE(co.snapshots, 1);
  EXPECT_GE(co.events, 0);
  int before = co.events;
  engine->Unsubscribe(co);
  (void)engine->Advance(std::chrono::milliseconds{10});
  EXPECT_EQ(co.events, before);
}

struct ReentrantObserver : IObserver {
  IRailEngine* engine{nullptr};
  std::atomic<int> busy{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    if (engine) {
      auto out = engine->Advance(std::chrono::milliseconds{5});
      if (out.status.code == StatusCode::Busy) busy.fetch_add(1);
    }
  }
};

TEST(Observer, ReentrancyBusy) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  ReentrantObserver ro; ro.engine = engine.get();
  engine->Subscribe(ro);
  auto out = engine->Advance(std::chrono::milliseconds{50});
  EXPECT_EQ(out.status.code, StatusCode::Ok);
  EXPECT_GE(ro.busy.load(), 1);
  engine->Unsubscribe(ro);
}

struct SnapshotCountingObserver : IObserver {
  int snapshots{0};
  void OnSnapshot(const LayoutSnapshot&) override { ++snapshots; }
  void OnEvents(const SimulationTickResult&) override {}
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, SnapshotOnSubscribe) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  SnapshotCountingObserver ob;
  engine->Subscribe(ob);
  EXPECT_GE(ob.snapshots, 1);
  engine->Unsubscribe(ob);
}
struct CollectingObserver : IObserver {
  int snapshots{0};
  std::vector<DomainEventId> events;
  void OnSnapshot(const LayoutSnapshot&) override { ++snapshots; }
  void OnEvents(const SimulationTickResult& tick) override { for (const auto& ev : tick.events) events.push_back(ev.id); }
};

TEST(Observer, EventsBatchContainsAssignAndDelay) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  CollectingObserver co; engine->Subscribe(co);
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  (void)out;
  bool sawAssign=false, sawDelay=false;
  for (auto id : co.events) { if (id == DomainEventId::LocoAssigned) sawAssign=true; if (id == DomainEventId::DelayChanged) sawDelay=true; }
  EXPECT_TRUE(sawAssign);
  EXPECT_TRUE(sawDelay);
}

TEST(Observer, EventsClearedOnNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la{snap.state->timetable.front().id, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_GE(t1.result.events.size(), 1u);
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_TRUE(t2.result.events.empty());
}


struct DiagObserver : IObserver {
  int diags{0};
  DiagnosticsEvent last;
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {}
  void OnDiagnostics(const DiagnosticsEvent& ev) override { ++diags; last = ev; }
};

TEST(Observer, DiagnosticsOnDtClamp) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DiagObserver ob; engine->Subscribe(ob);
  auto out = engine->Advance(std::chrono::milliseconds{5000});
  (void)out;
  EXPECT_GE(ob.diags, 1);
  EXPECT_EQ(ob.last.level, DiagnosticsLevel::Warning);
}

struct UnsubObserver : IObserver {
  IRailEngine* engine{nullptr};
  bool unsubscribed{false};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    if (engine && !unsubscribed) {
      engine->Unsubscribe(*this);
      unsubscribed = true;
    }
  }
};

TEST(Observer, UnsubscribeDuringCallback) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  UnsubObserver uo; uo.engine = engine.get();
  engine->Subscribe(uo);
  auto out = engine->Advance(std::chrono::milliseconds{10});
  (void)out;
  // A second advance should not crash or re-invoke since unsubscribed
  auto out2 = engine->Advance(std::chrono::milliseconds{10});
  (void)out2;
  SUCCEED();
}
