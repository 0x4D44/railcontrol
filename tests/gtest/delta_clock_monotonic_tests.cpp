#include <gtest/gtest.h>
#include <chrono>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(DeltaClock, MonotonicAndMatchesDelta) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{10});
  auto t2 = engine->Advance(std::chrono::milliseconds{10});
  EXPECT_LT(t1.result.clock, t2.result.clock);
  if (t1.result.delta) EXPECT_EQ(t1.result.delta->clock, t1.result.clock);
  if (t2.result.delta) EXPECT_EQ(t2.result.delta->clock, t2.result.clock);
}

