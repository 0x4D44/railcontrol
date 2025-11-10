#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

 

TEST(Delta, AssignThenReleaseSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  la.action = AssignmentAction::Release;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, la}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  bool sawRelease=false; for (const auto& ed : out.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (ed.id==tt && it!=ed.changedFields.end() && it->second.empty()) { sawRelease=true; break; } }
  EXPECT_TRUE(sawRelease);
  auto snap2 = engine->GetSnapshot();
  for (const auto& asn : snap2.state->assignments) { ASSERT_NE(asn.timetableId, tt) << "Assignment persisted unexpectedly"; }
}

TEST(Delta, MultipleAssignmentsOneTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) {
    GTEST_SKIP() << "Not enough timetable/locos for multi-assign test";
  }
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;
  LocoAssignment a1; a1.timetableId = tt1; a1.locoId = l1; a1.action = AssignmentAction::Assign;
  LocoAssignment a2; a2.timetableId = tt2; a2.locoId = l2; a2.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a1}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a2}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  int seen = 0; for (const auto& ed : out.result.delta->timetableEntries) {
    auto it = ed.changedFields.find("assignedLocoId");
    if (it != ed.changedFields.end()) {
      if ((ed.id == tt1 && it->second == std::to_string(l1)) || (ed.id == tt2 && it->second == std::to_string(l2))) ++seen;
    }
  }
  EXPECT_EQ(seen, 2);
}

TEST(Delta, TwoTimetableAssignmentsOneTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) {
    GTEST_SKIP() << "Not enough entities";
  }
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;
  LocoAssignment a1{tt1, l1, AssignmentAction::Assign};
  LocoAssignment a2{tt2, l2, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a1}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a2}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  int seen=0; for (const auto& ed : out.result.delta->timetableEntries) {
    auto it = ed.changedFields.find("assignedLocoId");
    if (it != ed.changedFields.end()) {
      if ((ed.id == tt1 && it->second == std::to_string(l1)) || (ed.id == tt2 && it->second == std::to_string(l2))) ++seen;
    }
  }
  EXPECT_EQ(seen, 2);
}

TEST(Delta, MultiTickTwoAssignmentsThenReleaseOne) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) {
    GTEST_SKIP() << "Not enough entities";
  }
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t1.result.delta);
  int seen=0; for (const auto& ed : t1.result.delta->timetableEntries) {
    auto it = ed.changedFields.find("assignedLocoId");
    if (it!=ed.changedFields.end() && ((ed.id==tt1 && it->second==std::to_string(l1)) || (ed.id==tt2 && it->second==std::to_string(l2)))) ++seen;
  }
  EXPECT_EQ(seen, 2);
  // Tick 2: release one
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt2, 0, AssignmentAction::Release}}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t2.result.delta);
  bool sawRelease=false;
  for (const auto& ed : t2.result.delta->timetableEntries) {
    auto it = ed.changedFields.find("assignedLocoId");
    if (ed.id==tt2 && it!=ed.changedFields.end() && it->second.empty()) sawRelease=true;
  }
  EXPECT_TRUE(sawRelease);
}

TEST(Delta, CompositeTwoAssignOneReleaseAndDelayInOneTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) {
    GTEST_SKIP() << "Not enough entities";
  }
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;
  // Queue up: assign both, then release first, and change delay settings — all before a single Advance.
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt1, 0, AssignmentAction::Release}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  bool sawTt1Cleared=false, sawTt2Assigned=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    auto it = ed.changedFields.find("assignedLocoId");
    if (ed.id==tt1 && it!=ed.changedFields.end() && it->second.empty()) sawTt1Cleared=true;
    if (ed.id==tt2 && it!=ed.changedFields.end() && it->second==std::to_string(l2)) sawTt2Assigned=true;
  }
  EXPECT_TRUE(sawTt1Cleared);
  EXPECT_TRUE(sawTt2Assigned);
  auto& g = out.result.delta->globals;
  EXPECT_EQ(g["delay.mode"], std::string("MaintenanceOnly"));
  EXPECT_EQ(g["delay.threshold_min"], std::to_string(ds.threshold.count()));
}

TEST(Delta, ClockMatchesAndClearing) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Issue a delay change to create a globals delta
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_TRUE(out.result.delta);
  EXPECT_EQ(out.result.delta->clock, out.result.clock);
  // Next advance with no new changes should have no delta
  auto out2 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_FALSE(out2.result.delta);
}

TEST(Delta, ZeroChangeTickIdBehavior) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap1 = engine->GetSnapshot();
  uint32_t t1 = snap1.state->tickId;
  // No changes, zero dt => tickId stays same
  auto out = engine->Advance(std::chrono::milliseconds{0});
  (void)out;
  auto snap2 = engine->GetSnapshot();
  EXPECT_EQ(snap2.state->tickId, t1);
  // Make a change (delay) and advance with zero dt => tickId increments
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out2 = engine->Advance(std::chrono::milliseconds{0});
  (void)out2;
  auto snap3 = engine->GetSnapshot();
  EXPECT_EQ(snap3.state->tickId, t1 + 1);
}

TEST(Delta, CompositeAssignReleaseAndDelayInOneTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  // Assign, then release, and also change delay settings before advancing
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  la.action = AssignmentAction::Release;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, la}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{3};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  // Expect a cleared assignment for the timetable id
  bool sawCleared=false; for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("assignedLocoId");
      if (it != ed.changedFields.end() && it->second.empty()) { sawCleared = true; break; }
    }
  }
  EXPECT_TRUE(sawCleared);
  // Expect globals contain delay settings
  auto& g = out.result.delta->globals;
  EXPECT_EQ(g.find("delay.mode")->second, std::string("MaintenanceOnly"));
  EXPECT_EQ(g.find("delay.threshold_min")->second, std::to_string(ds.threshold.count()));
  EXPECT_EQ(g.find("delay.maintenanceThrough")->second, std::string("false"));
}

TEST(Delta, CompositeClearsNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  la.action = AssignmentAction::Release;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, la}).code, StatusCode::Ok);
  // First advance yields delta
  auto out1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out1.result.delta);
  // Second advance with no new changes yields no delta
  auto out2 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_FALSE(out2.result.delta);
}

TEST(Delta, MultiTickAssignThenDelayThenRelease) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  // Tick 1: assign
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t1.result.delta);
  bool sawAssign=false; for (const auto& ed : t1.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (ed.id==tt && it!=ed.changedFields.end() && it->second==std::to_string(loco)) { sawAssign=true; break; } }
  EXPECT_TRUE(sawAssign);
  // Tick 2: delay settings
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t2.result.delta);
  auto it = t2.result.delta->globals.find("delay.mode"); ASSERT_NE(it, t2.result.delta->globals.end());
  EXPECT_EQ(it->second, std::string("Randomized"));
  // Tick 3: release
  la.action = AssignmentAction::Release;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, la}).code, StatusCode::Ok);
  auto t3 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t3.result.delta);
  bool sawRelease=false; for (const auto& ed : t3.result.delta->timetableEntries) { auto jt=ed.changedFields.find("assignedLocoId"); if (ed.id==tt && jt!=ed.changedFields.end() && jt->second.empty()) { sawRelease=true; break; } }
  EXPECT_TRUE(sawRelease);
}

TEST(Delta, GlobalsAndAssignmentInSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  // Mutate assignment and delay globals before tick
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{3};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  // Validate both assignment change and globals present
  bool sawAssign=false; for (const auto& ed : out.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (it!=ed.changedFields.end()) { sawAssign=true; break; } }
  EXPECT_TRUE(sawAssign);
  auto itMode = out.result.delta->globals.find("delay.mode");
  auto itThresh = out.result.delta->globals.find("delay.threshold_min");
  EXPECT_TRUE(itMode != out.result.delta->globals.end());
  EXPECT_TRUE(itThresh != out.result.delta->globals.end());
}
