#include <gtest/gtest.h>
#include <string>
#include "delta_helpers.h"
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(DeltaSequence, AssignDelayThenArrivalThenDepartNoRedundantEmits) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Tick 1: assign + delay -> expect assignment + globals
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t1.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(t1.result.delta, tt, "assignedLocoId", std::to_string(loco).c_str()));
  EXPECT_TRUE(DeltaGlobalsHave(t1.result.delta, {"delay.mode","delay.thresholdMinutes","delay.maintenanceThrough"}));

  // Tick 2: arrival only (no globals)
  auto t2 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_TRUE(t2.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(t2.result.delta, tt, "arrived"));
  EXPECT_FALSE(DeltaGlobalsHave(t2.result.delta, {"delay.mode","delay.thresholdMinutes"}));

  // Tick 3: depart only (no arrived re-emit, no globals)
  auto t3 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_TRUE(t3.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(t3.result.delta, tt, "departed"));
  EXPECT_FALSE(DeltaHasEntryField(t3.result.delta, tt, "arrived"));
  EXPECT_FALSE(DeltaGlobalsHave(t3.result.delta, {"delay.mode","delay.thresholdMinutes"}));
}

