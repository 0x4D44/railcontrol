#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(SchedulingProgression, ArrivesThenDepartsOverTwoTicks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());

  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  // Tick 1: 300ms -> should include arrival only
  (void)engine->Advance(std::chrono::milliseconds{0}); // consume assign delta
  auto out1 = engine->Advance(std::chrono::milliseconds{300});
  ASSERT_EQ(out1.status.code, StatusCode::Ok);
  ASSERT_TRUE(out1.result.delta);
  bool sawArr=false, sawDep=false;
  for (const auto& ed : out1.result.delta->timetableEntries) {
    if (ed.id==tt) {
      sawArr = (ed.changedFields.count("arrived") && ed.changedFields.at("arrived")=="true") || sawArr;
      sawDep = (ed.changedFields.count("departed") && ed.changedFields.at("departed")=="true") || sawDep;
    }
  }
  EXPECT_TRUE(sawArr);
  EXPECT_FALSE(sawDep);

  // Tick 2: +300ms -> should include departure
  auto out2 = engine->Advance(std::chrono::milliseconds{300});
  ASSERT_EQ(out2.status.code, StatusCode::Ok);
  ASSERT_TRUE(out2.result.delta);
  bool sawDep2=false;
  for (const auto& ed : out2.result.delta->timetableEntries) {
    if (ed.id==tt && ed.changedFields.count("departed") && ed.changedFields.at("departed")=="true") { sawDep2=true; break; }
  }
  EXPECT_TRUE(sawDep2);
}

TEST(SchedulingProgression, MultiEntriesArriveAndDepartSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_GE(snap.state->timetable.size(), 2u); ASSERT_GE(snap.state->locos.size(), 2u);

  const uint32_t tt1 = snap.state->timetable[0].id; const uint32_t loco1 = snap.state->locos[0].id;
  const uint32_t tt2 = snap.state->timetable[1].id; const uint32_t loco2 = snap.state->locos[1].id;

  s = engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, loco1, AssignmentAction::Assign}}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, loco2, AssignmentAction::Assign}}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  (void)engine->Advance(std::chrono::milliseconds{0});
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  bool t1Arr=false,t1Dep=false,t2Arr=false,t2Dep=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id==tt1) { t1Arr = t1Arr || (ed.changedFields.count("arrived") && ed.changedFields.at("arrived")=="true"); t1Dep = t1Dep || (ed.changedFields.count("departed") && ed.changedFields.at("departed")=="true"); }
    if (ed.id==tt2) { t2Arr = t2Arr || (ed.changedFields.count("arrived") && ed.changedFields.at("arrived")=="true"); t2Dep = t2Dep || (ed.changedFields.count("departed") && ed.changedFields.at("departed")=="true"); }
  }
  EXPECT_TRUE(t1Arr && t1Dep);
  EXPECT_TRUE(t2Arr && t2Dep);
}

