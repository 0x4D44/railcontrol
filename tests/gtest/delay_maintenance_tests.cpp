#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(DelayMode, MaintenanceThroughTrueDelta) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{3}; ds.maintenanceThrough = true;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  auto it = out.result.delta->globals.find("delay.maintenanceThrough");
  ASSERT_NE(it, out.result.delta->globals.end());
  EXPECT_EQ(it->second, std::string("true"));
}
