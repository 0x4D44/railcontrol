#include <gtest/gtest.h>
#include <atomic>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct SnapshotCounting : IObserver {
  std::atomic<int> snapshots{0};
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override { snapshots.fetch_add(1); }
  void OnEvents(const SimulationTickResult&) override { events.fetch_add(1); }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, ResubscribeYieldsSnapshotAndEvents) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  SnapshotCounting ob;
  engine->Subscribe(ob);
  // Trigger one event batch
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});
  int snaps = ob.snapshots.load();
  // Unsubscribe and resubscribe
  engine->Unsubscribe(ob);
  engine->Subscribe(ob);
  // On resubscribe should get another snapshot
  EXPECT_GE(ob.snapshots.load(), snaps + 1);
  // And further events still delivered
  (void)engine->Advance(std::chrono::milliseconds{10});
  EXPECT_GE(ob.events.load(), 1);
}

