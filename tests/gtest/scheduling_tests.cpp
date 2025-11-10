#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Scheduling, TrainArrivedAfterThreshold) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la{tt, loco, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  // First short advance: should not arrive yet
  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  bool sawArrive = false;
  for (const auto& ev : t1.result.events) if (ev.id == DomainEventId::TrainArrived) sawArrive = true;
  EXPECT_FALSE(sawArrive);
  // Second advance pushes over internal threshold (~200ms total)
  auto t2 = engine->Advance(std::chrono::milliseconds{200});
  sawArrive = false;
  for (const auto& ev : t2.result.events) if (ev.id == DomainEventId::TrainArrived) sawArrive = true;
  EXPECT_TRUE(sawArrive);
  // Delta should contain an 'arrived' flag for the timetable id
  ASSERT_TRUE(t2.result.delta);
  bool sawDelta=false;
  for (const auto& ed : t2.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("arrived");
      if (it != ed.changedFields.end() && it->second == "true") { sawDelta = true; break; }
    }
  }
  EXPECT_TRUE(sawDelta);
}

TEST(Scheduling, TrainDepartedAfterArrival) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{250}); // trigger arrival
  auto t2 = engine->Advance(std::chrono::milliseconds{300}); // exceed depart threshold
  bool sawDepart=false; for (const auto& ev : t2.result.events) if (ev.id == DomainEventId::TrainDeparted) sawDepart=true;
  EXPECT_TRUE(sawDepart);
  ASSERT_TRUE(t2.result.delta);
  bool sawDepDelta=false; for (const auto& ed : t2.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("departed"); if (it!=ed.changedFields.end() && it->second=="true") { sawDepDelta=true; break; } } }
  EXPECT_TRUE(sawDepDelta);
}

TEST(Scheduling, ReleaseBeforeDepartSuppressesDepart) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{100}); // not yet arrived
  // Release before depart threshold
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt, 0, AssignmentAction::Release}}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{600});
  bool sawDepart=false; for (const auto& ev : t2.result.events) if (ev.id == DomainEventId::TrainDeparted) sawDepart=true;
  EXPECT_FALSE(sawDepart);
}

TEST(Scheduling, NoDuplicateArrivalOrDepartEvents) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{250});
  (void)engine->Advance(std::chrono::milliseconds{0}); // arrival should have happened already; zero dt tick should not re-emit
  auto t3 = engine->Advance(std::chrono::milliseconds{10});
  int arriveCount = 0; for (const auto& ev : t3.result.events) if (ev.id == DomainEventId::TrainArrived) ++arriveCount;
  EXPECT_EQ(arriveCount, 0);
  auto t4 = engine->Advance(std::chrono::milliseconds{400}); // depart should occur once
  int departCount = 0; for (const auto& ev : t4.result.events) if (ev.id == DomainEventId::TrainDeparted) ++departCount;
  EXPECT_EQ(departCount, 1);
  auto t5 = engine->Advance(std::chrono::milliseconds{100});
  departCount = 0; for (const auto& ev : t5.result.events) if (ev.id == DomainEventId::TrainDeparted) ++departCount;
  EXPECT_EQ(departCount, 0);
}

TEST(Scheduling, MultipleAssignmentsArriveThenDepart) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2 timetable and 2 locos";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Advance beyond arrival threshold
  auto t1 = engine->Advance(std::chrono::milliseconds{250});
  int arrive=0; for (const auto& ev : t1.result.events) if (ev.id == DomainEventId::TrainArrived) ++arrive;
  EXPECT_GE(arrive, 1);
  // Advance beyond depart threshold
  auto t2 = engine->Advance(std::chrono::milliseconds{300});
  int depart=0; for (const auto& ev : t2.result.events) if (ev.id == DomainEventId::TrainDeparted) ++depart;
  EXPECT_GE(depart, 1);
}

