#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(EngineBasics, GetLayoutIdBeforeAndAfterReset) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  // Before load: empty id
  EXPECT_TRUE(engine->GetLayoutId().empty());
  // After load: has id
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto idAfter = engine->GetLayoutId();
  EXPECT_FALSE(idAfter.empty());
  // Reset: we currently keep last layout id (implementation choice)
  s = engine->Reset();
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  EXPECT_EQ(engine->GetLayoutId(), idAfter);
}

TEST(EngineBasics, ReleaseLocoWrongIdReturnsNotFound) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  // Release with wrong loco id should be NotFound
  LocoAssignment wrong; wrong.timetableId = la.timetableId; wrong.locoId = la.locoId + 12345; wrong.action = AssignmentAction::Release;
  s = engine->Command(CommandPayload{CommandId::ReleaseLoco, wrong});
  EXPECT_EQ(s.code, StatusCode::NotFound);
}

