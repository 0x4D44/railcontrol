#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(DelayMode, ThresholdBoundariesAccepted) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // 0 minutes is valid
  DelaySettings ds0; ds0.mode = DelayMode::Randomized; ds0.threshold = std::chrono::minutes{0};
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds0}).code, StatusCode::Ok);
  // 1439 minutes is valid
  DelaySettings dsmax; dsmax.mode = DelayMode::MaintenanceOnly; dsmax.threshold = std::chrono::minutes{1439};
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, dsmax}).code, StatusCode::Ok);
}
