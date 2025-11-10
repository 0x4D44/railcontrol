#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <algorithm>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "test_utils.h"

using namespace RailCore;

TEST(SchedulingMultiEntry, TwoAssignmentsProgressWithRouteContextSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  auto snap = engine->GetSnapshot();
  ASSERT_GE(snap.state->timetable.size(), 2u);
  ASSERT_GE(snap.state->locos.size(), 2u);
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;

  LocoAssignment a1{tt1, l1, AssignmentAction::Assign};
  LocoAssignment a2{tt2, l2, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a1}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a2}).code, StatusCode::Ok);

  // Advance beyond depart threshold so both have stageIndex and route-context fields
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());

  bool saw1 = false, saw2 = false;
  auto hasAll = [](const EntityDelta& ed) {
    return ed.changedFields.find("stageIndex") != ed.changedFields.end() &&
           ed.changedFields.find("routeId") != ed.changedFields.end() &&
           ed.changedFields.find("stagePrimary") != ed.changedFields.end() &&
           ed.changedFields.find("stageSecondary") != ed.changedFields.end();
  };
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt1) { EXPECT_TRUE(hasAll(ed)); saw1 = true; }
    if (ed.id == tt2) { EXPECT_TRUE(hasAll(ed)); saw2 = true; }
  }
  EXPECT_TRUE(saw1);
  EXPECT_TRUE(saw2);
}

