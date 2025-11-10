#include <gtest/gtest.h>
#include <atomic>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct FirstUnsubObserver : IObserver {
  IRailEngine* engine{nullptr};
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    events.fetch_add(1);
    if (engine) engine->Unsubscribe(*this);
  }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

struct CountingObserver2 : IObserver {
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override { events.fetch_add(1); }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, UnsubscribeOneKeepsOthersNotified) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  FirstUnsubObserver a; a.engine = engine.get();
  CountingObserver2 b;
  engine->Subscribe(a);
  engine->Subscribe(b);
  // Produce some events: set delay and advance twice
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0}); // a unsubscribes here
  int bAfter1 = b.events.load();
  (void)engine->Advance(std::chrono::milliseconds{10});
  EXPECT_GE(b.events.load(), bAfter1 + 1);
  EXPECT_GE(a.events.load(), 1);
}

