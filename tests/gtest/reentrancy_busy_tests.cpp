#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

namespace {
struct ReentrantObserver : IObserver {
  std::shared_ptr<IRailEngine> engine;
  StatusCode lastReentrant{StatusCode::Ok};
  explicit ReentrantObserver(std::shared_ptr<IRailEngine> e) : engine(std::move(e)) {}
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    auto out = engine->Advance(std::chrono::milliseconds{1});
    lastReentrant = out.status.code;
  }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};
}

TEST(Reentrancy, AdvanceReturnsBusyDuringCallback) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto obs = std::make_unique<ReentrantObserver>(engine);
  engine->Subscribe(*obs);
  auto out = engine->Advance(std::chrono::milliseconds{10});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  EXPECT_EQ(obs->lastReentrant, StatusCode::Busy);
}

