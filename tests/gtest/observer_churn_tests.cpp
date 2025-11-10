#include <gtest/gtest.h>
#include <atomic>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct CountingObserverChurn : IObserver {
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override { events.fetch_add(1); }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, SubscribeUnsubscribeChurn) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  CountingObserverChurn ob;
  // Churn subscriptions while advancing time and emitting events via delay changes
  for (int i = 0; i < 10; ++i) {
    engine->Subscribe(ob);
    DelaySettings ds; ds.mode = (i % 2 == 0) ? DelayMode::Randomized : DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{1};
    ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
    (void)engine->Advance(std::chrono::milliseconds{0});
    engine->Unsubscribe(ob);
  }
  // Finally, subscribe and advance a bit to ensure we still get events
  engine->Subscribe(ob);
  (void)engine->Advance(std::chrono::milliseconds{10});
  EXPECT_GE(ob.events.load(), 1);
  engine->Unsubscribe(ob);
}

