#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(WorldDeltaClock, MatchesTickClockAndClearsNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Issue a delay change to produce a globals delta on next tick
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  ASSERT_TRUE(t1.result.delta);
  EXPECT_EQ(t1.result.delta->clock, t1.result.clock);
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_FALSE(t2.result.delta);
}

