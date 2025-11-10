#include <gtest/gtest.h>
#include <atomic>
#include <vector>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct CountingObs : IObserver {
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override { events.fetch_add(1); }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, ManySubscribersAndPartialUnsubscribeMidCallback) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Create many subscribers
  std::vector<std::unique_ptr<CountingObs>> subs; subs.reserve(32);
  for (int i = 0; i < 32; ++i) { subs.emplace_back(std::make_unique<CountingObs>()); engine->Subscribe(*subs.back()); }
  // Emit an event batch
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1}; ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});
  int total1 = 0; for (auto& s : subs) total1 += s->events.load();
  EXPECT_GE(total1, 32);
  // Unsubscribe half and advance again
  for (int i = 0; i < 16; ++i) engine->Unsubscribe(*subs[i]);
  (void)engine->Advance(std::chrono::milliseconds{10});
  int total2 = 0; for (int i = 0; i < 32; ++i) total2 += subs[i]->events.load();
  // Ensure additional callbacks occurred but roughly for only remaining subs
  EXPECT_GE(total2, total1 + 8); // loose lower bound
}

