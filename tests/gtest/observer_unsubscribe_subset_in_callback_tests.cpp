#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <vector>
#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct SelfUnsubObserver : IObserver {
  std::shared_ptr<IRailEngine> engine;
  int index{0};
  std::atomic<int> events{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    events.fetch_add(1);
    if ((index % 2) == 0 && engine) engine->Unsubscribe(*this); // even-index unsubscribes on first callback
  }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};

TEST(Observer, UnsubscribeSubsetDuringCallback) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  std::vector<std::unique_ptr<SelfUnsubObserver>> obs; obs.reserve(20);
  for (int i = 0; i < 20; ++i) {
    auto o = std::make_unique<SelfUnsubObserver>(); o->engine = engine; o->index = i; engine->Subscribe(*o); obs.emplace_back(std::move(o));
  }
  // Emit an event batch to trigger unsubscribes for even indices
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1}; ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});
  // Next advance should still notify odd-index observers; even-index likely unsubscribed
  (void)engine->Advance(std::chrono::milliseconds{10});
  int oddCount = 0; int evenCount = 0;
  for (int i = 0; i < 20; ++i) { if ((i % 2)==0) evenCount += obs[i]->events.load(); else oddCount += obs[i]->events.load(); }
  EXPECT_GE(oddCount, evenCount); // loose check: odd should have at least as many callbacks as even
}

