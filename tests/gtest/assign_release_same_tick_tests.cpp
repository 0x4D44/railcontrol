#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(AssignReleaseSameTick, EmitsReleaseDeltaAndNoPersistedAssignment) {
  EngineConfig cfg;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());

  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;

  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  la.action = AssignmentAction::Release;
  s = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok) << out.status.message;
  ASSERT_TRUE(out.result.delta) << "Expected delta for assign+release same tick";

  bool sawRelease = false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("assignedLocoId");
      if (it != ed.changedFields.end() && it->second.empty()) {
        sawRelease = true; break;
      }
    }
  }
  EXPECT_TRUE(sawRelease) << "Expected release delta after assign+release same tick";

  auto snap2 = engine->GetSnapshot();
  for (const auto& asn : snap2.state->assignments) {
    ASSERT_NE(asn.timetableId, tt) << "Assignment persisted unexpectedly after assign+release same tick";
  }
}

