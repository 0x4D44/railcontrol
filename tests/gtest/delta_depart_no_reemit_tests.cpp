#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Delta, DepartedFlagDoesNotReemitNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Advance beyond depart threshold to trigger departed delta
  (void)engine->Advance(std::chrono::milliseconds{250});
  auto t2 = engine->Advance(std::chrono::milliseconds{300});
  ASSERT_TRUE(t2.result.delta);
  bool sawDepart=false; for (const auto& ed : t2.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("departed"); if (it!=ed.changedFields.end() && it->second=="true") { sawDepart=true; break; } } }
  EXPECT_TRUE(sawDepart);
  // Next tick should not re-emit departed flag
  auto t3 = engine->Advance(std::chrono::milliseconds{0});
  if (t3.result.delta) {
    bool sawDepart2=false; for (const auto& ed : t3.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("departed"); if (it!=ed.changedFields.end() && it->second=="true") { sawDepart2=true; break; } } }
    EXPECT_FALSE(sawDepart2);
  }
}
