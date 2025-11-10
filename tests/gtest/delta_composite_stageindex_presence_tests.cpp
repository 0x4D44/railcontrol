#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Deltas, AssignAndDelayEmitWithStageIndexOnDt) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  // Assign first loco to first timetable entry
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  // Change delay mode too
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2}; ds.maintenanceThrough = false;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);

  // Advance with dt>0 so stageIndex is eligible
  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  // Expect both a timetable assignment delta and globals
  bool sawAssign=false, sawStageIdx=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("assignedLocoId");
      if (it != ed.changedFields.end()) sawAssign = true;
      auto it2 = ed.changedFields.find("stageIndex");
      if (it2 != ed.changedFields.end()) sawStageIdx = true;
    }
  }
  ASSERT_TRUE(sawAssign);
  ASSERT_TRUE(sawStageIdx);
  EXPECT_TRUE(out.result.delta->globals.find("delay.mode") != out.result.delta->globals.end());
}

