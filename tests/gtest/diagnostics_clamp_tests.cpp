#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

namespace {
struct DiagObserver : IObserver {
  int warnings{0};
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {}
  void OnDiagnostics(const DiagnosticsEvent& ev) override {
    if (ev.level == DiagnosticsLevel::Warning) ++warnings;
  }
};
}

TEST(DiagnosticsClamp, EmitsWarningWhenDtClamped) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  DiagObserver obs; engine->Subscribe(obs);
  // Advance with a large dt to force clamp to maxStep (1000ms)
  auto out = engine->Advance(std::chrono::milliseconds{5000});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  EXPECT_GE(obs.warnings, 1);
}

