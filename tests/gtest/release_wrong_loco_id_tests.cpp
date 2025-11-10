#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Assignments, ReleaseWithWrongLocoIdReturnsNotFound) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());

  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  // Try to release the same timetable with a different loco id -> NotFound
  LocoAssignment wrong; wrong.timetableId = la.timetableId; wrong.locoId = la.locoId + 12345; wrong.action = AssignmentAction::Release;
  auto s = engine->Command(CommandPayload{CommandId::ReleaseLoco, wrong});
  EXPECT_EQ(s.code, StatusCode::NotFound);
}

