#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Deltas, DelayGlobalsKeysPresent) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{3}; ds.maintenanceThrough = true;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta.has_value());
  const auto& g = out.result.delta->globals;
  // Expect standardized keys
  ASSERT_TRUE(g.find("delay.mode") != g.end());
  ASSERT_TRUE(g.find("delay.thresholdMinutes") != g.end());
  ASSERT_TRUE(g.find("delay.maintenanceThrough") != g.end());
}

