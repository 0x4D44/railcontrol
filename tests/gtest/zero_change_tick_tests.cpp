#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(ZeroChangeTick, TickIdOnlyIncrementsOnDeltaOrTime) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap1 = engine->GetSnapshot();
  auto t1 = snap1.state->tickId;
  auto out1 = engine->Advance(std::chrono::milliseconds{0}); ASSERT_EQ(out1.status.code, StatusCode::Ok);
  auto snap2 = engine->GetSnapshot();
  EXPECT_EQ(snap2.state->tickId, t1) << "tickId changed on zero-change tick";
  // Issue a command that produces a delta; dt=0 should then increment tickId
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1}; ds.maintenanceThrough = false;
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto out2 = engine->Advance(std::chrono::milliseconds{0}); ASSERT_EQ(out2.status.code, StatusCode::Ok);
  auto snap3 = engine->GetSnapshot();
  EXPECT_EQ(snap3.state->tickId, t1 + 1) << "tickId did not increment on delta-only tick";
}

