#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static bool FindField(const std::shared_ptr<const WorldDelta>& delta, uint32_t tt, const char* key, std::string& out) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if ( ed.id == tt ) {
      auto it = ed.changedFields.find(key);
      if (it != ed.changedFields.end()) { out = it->second; return true; }
    }
  }
  return false;
}

TEST(StageReassign, ResetStageAndProgressAfterReassign) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Assign and advance to arrival
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});
  (void)engine->Advance(std::chrono::milliseconds{250});

  // Reassign (to same or second loco if available) should reset stage to 0
  uint32_t loco2 = loco;
  if (snap.state->locos.size() > 1) loco2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Zero-dt to flush assignment delta, then a small dt to emit stage/progress
  (void)engine->Advance(std::chrono::milliseconds{0});
  auto t = engine->Advance(std::chrono::milliseconds{50});
  std::string stage; ASSERT_TRUE(FindField(t.result.delta, tt, "stage", stage));
  EXPECT_EQ(stage, std::string("0"));
}

