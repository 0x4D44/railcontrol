#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(DeltaGlobals, EmitOnceAcrossTicks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Set delay -> expect globals on next tick
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{10});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  ASSERT_TRUE(t1.result.delta);
  EXPECT_FALSE(t1.result.delta->globals.empty());
  // Next tick with dt>0 and no new changes: globals should not re-emit
  auto t2 = engine->Advance(std::chrono::milliseconds{10});
  if (t2.result.delta) {
    EXPECT_TRUE(t2.result.delta->globals.empty());
  }
}

