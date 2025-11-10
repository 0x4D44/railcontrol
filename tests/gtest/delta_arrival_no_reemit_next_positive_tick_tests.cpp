#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(Delta, ArrivalDoesNotReemitOnNextPositiveTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Trigger arrival (~200-250ms)
  auto t1 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_TRUE(t1.result.delta);
  bool sawArr=false; for (const auto& ed : t1.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("arrived"); if (it!=ed.changedFields.end() && it->second=="true") { sawArr=true; break; } } }
  ASSERT_TRUE(sawArr);
  // Next positive dt not enough to depart; arrived should not re-emit
  auto t2 = engine->Advance(std::chrono::milliseconds{100});
  if (t2.result.delta) {
    bool sawArr2=false; for (const auto& ed : t2.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("arrived"); if (it!=ed.changedFields.end() && it->second=="true") { sawArr2=true; break; } } }
    EXPECT_FALSE(sawArr2);
  }
}

