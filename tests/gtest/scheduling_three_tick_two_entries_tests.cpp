#include <gtest/gtest.h>
#include <map>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

static int CountEventsFor(const std::vector<DomainEvent>& evs, DomainEventId id, uint32_t tt) {
  int c = 0;
  for (const auto& e : evs) {
    if (e.id == id && e.payload.index() == 1) {
      const auto& m = std::get<std::map<std::string,std::string>>(e.payload);
      auto it = m.find("timetableId"); if (it != m.end() && it->second == std::to_string(tt)) ++c;
    }
  }
  return c;
}

TEST(Scheduling, ThreeTickTwoEntriesArrivalThenDepartNoReemit) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2 timetable and 2 locos";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;

  // Tick 1: assign both + delay
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t1.result.delta);
  EXPECT_FALSE(t1.result.delta->globals.empty());

  // Tick 2 (~300ms): arrival for both; no globals
  auto t2 = engine->Advance(std::chrono::milliseconds{300});
  ASSERT_TRUE(t2.result.delta);
  EXPECT_EQ(t2.result.delta->globals.size(), 0u);
  EXPECT_GE(CountEventsFor(t2.result.events, DomainEventId::TrainArrived, tt1), 1);
  EXPECT_GE(CountEventsFor(t2.result.events, DomainEventId::TrainArrived, tt2), 1);

  // Tick 3 (~300ms): depart for both; no arrived re-emit; no globals
  auto t3 = engine->Advance(std::chrono::milliseconds{300});
  ASSERT_TRUE(t3.result.delta);
  EXPECT_EQ(t3.result.delta->globals.size(), 0u);
  EXPECT_GE(CountEventsFor(t3.result.events, DomainEventId::TrainDeparted, tt1), 1);
  EXPECT_GE(CountEventsFor(t3.result.events, DomainEventId::TrainDeparted, tt2), 1);
}

