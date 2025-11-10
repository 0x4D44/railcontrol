#include <gtest/gtest.h>
#include <string>
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

TEST(Scheduling, TwoEntriesArriveAndDepartInSameTickWithGlobals) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2+2";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{3};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);

  // Single large dt: should produce arrival/depart for both and a globals delta
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  EXPECT_FALSE(out.result.delta->globals.empty());
  EXPECT_GE(CountEventsFor(out.result.events, DomainEventId::TrainArrived, tt1), 1);
  EXPECT_GE(CountEventsFor(out.result.events, DomainEventId::TrainArrived, tt2), 1);
  EXPECT_GE(CountEventsFor(out.result.events, DomainEventId::TrainDeparted, tt1), 1);
  EXPECT_GE(CountEventsFor(out.result.events, DomainEventId::TrainDeparted, tt2), 1);
}

