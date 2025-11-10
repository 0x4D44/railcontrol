#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(DelayMode, NegativeThresholdRejected) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{-1};
  auto s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(DelayMode, Threshold1440Rejected) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1440};
  auto s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
  EXPECT_NE(s.code, StatusCode::Ok);
}
