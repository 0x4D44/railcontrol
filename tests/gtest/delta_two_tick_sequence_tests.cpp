#include <gtest/gtest.h>
#include <string>
#include "delta_helpers.h"
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(DeltaSequence, AssignAndDelayThenArrivalNextTickNoGlobalReemit) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Tick 1: queue assignment and delay; zero-dt advance flushes assignment + globals
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  ASSERT_TRUE(t1.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(t1.result.delta, tt, "assignedLocoId", std::to_string(loco).c_str()));
  EXPECT_TRUE(DeltaGlobalsHave(t1.result.delta, {"delay.mode","delay.thresholdMinutes"}));

  // Tick 2: dt large enough to emit arrival only; globals should not re-emit
  auto t2 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_EQ(t2.status.code, StatusCode::Ok);
  ASSERT_TRUE(t2.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(t2.result.delta, tt, "arrived"));
  EXPECT_FALSE(DeltaGlobalsHave(t2.result.delta, {"delay.mode","delay.thresholdMinutes"}));
}

