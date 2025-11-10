#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(StageIndex, NotEmittedOnZeroDtOnlyDeltalessTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  // Assign, then advance with zero dt should still emit assignment delta (and may carry stageIndex if present)
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  ASSERT_TRUE(t1.result.delta.has_value());

  // Next zero-dt with no state changes should not produce any delta at all
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(t2.status.code, StatusCode::Ok);
  EXPECT_FALSE(t2.result.delta.has_value());
}

