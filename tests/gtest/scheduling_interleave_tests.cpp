#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Scheduling, InterleavedArrivalsAndDeparts) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2 timetable and 2 locos";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Stagger tt2 by one tick
  auto t0 = engine->Advance(std::chrono::milliseconds{50}); (void)t0;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Advance to cross arrival for tt1; tt2 not yet
  auto t1 = engine->Advance(std::chrono::milliseconds{170});
  int arrived1=0, arrived2=0; for (const auto& ev : t1.result.events) {
    if (ev.id == DomainEventId::TrainArrived) {
      if (ev.payload.index() == 1) {
        auto& m = std::get<std::map<std::string,std::string>>(ev.payload);
        if (m.find("timetableId")!=m.end()) {
          if (m["timetableId"] == std::to_string(tt1)) arrived1++;
          if (m["timetableId"] == std::to_string(tt2)) arrived2++;
        }
      }
    }
  }
  EXPECT_GE(arrived1, 1);
  EXPECT_EQ(arrived2, 0);
  // Advance more to cross arrival for tt2 and depart for tt1
  auto t2 = engine->Advance(std::chrono::milliseconds{350});
  int dep1=0, arr2=0; for (const auto& ev : t2.result.events) {
    if (ev.id == DomainEventId::TrainDeparted) {
      if (ev.payload.index() == 1) {
        auto& m = std::get<std::map<std::string,std::string>>(ev.payload);
        if (m.find("timetableId")!=m.end() && m["timetableId"] == std::to_string(tt1)) dep1++;
      }
    }
    if (ev.id == DomainEventId::TrainArrived) {
      if (ev.payload.index() == 1) {
        auto& m = std::get<std::map<std::string,std::string>>(ev.payload);
        if (m.find("timetableId")!=m.end() && m["timetableId"] == std::to_string(tt2)) arr2++;
      }
    }
  }
  EXPECT_GE(dep1, 1);
  EXPECT_GE(arr2, 1);
}
