#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// After an assignment, a zero-dt tick emits an assignment delta but should not include stageIndex.
TEST(StageIndex, NotIncludedOnZeroDtAssignmentDelta) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  // Zero-dt advance yields assignment delta; should not include stageIndex.
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  bool sawAssign=false, sawStageIdx=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == la.timetableId) {
      if (ed.changedFields.find("assignedLocoId") != ed.changedFields.end()) sawAssign = true;
      if (ed.changedFields.find("stageIndex") != ed.changedFields.end()) sawStageIdx = true;
    }
  }
  ASSERT_TRUE(sawAssign);
  ASSERT_FALSE(sawStageIdx);
}

