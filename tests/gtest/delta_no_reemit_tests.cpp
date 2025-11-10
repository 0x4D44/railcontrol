#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Delta, SchedulingFlagsDoNotReemitNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Advance to trigger arrival and capture delta
  auto t1 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_TRUE(t1.result.delta);
  bool sawArr=false; for (const auto& ed : t1.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("arrived"); if (it!=ed.changedFields.end() && it->second=="true") { sawArr=true; break; } } }
  EXPECT_TRUE(sawArr);
  // Next tick with zero dt should not re-emit arrived flag
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  if (t2.result.delta) {
    bool sawArr2=false; for (const auto& ed : t2.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("arrived"); if (it!=ed.changedFields.end() && it->second=="true") { sawArr2=true; break; } } }
    EXPECT_FALSE(sawArr2);
  }
}

