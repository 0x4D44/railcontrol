#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "test_utils.h"

using namespace RailCore;

static std::optional<size_t> FindFirstEventIndex(const std::vector<DomainEvent>& evs, DomainEventId id, uint32_t tt) {
  for (size_t i = 0; i < evs.size(); ++i) {
    if (evs[i].id == id) {
      auto mp = std::get<std::map<std::string,std::string>>(evs[i].payload);
      auto it = mp.find("timetableId");
      if (it != mp.end() && std::stoul(it->second) == tt) return i;
    }
  }
  return std::nullopt;
}

TEST(SchedulingMultiEntry, ArrivalBeforeDeparturePerEntry) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  auto snap = engine->GetSnapshot();
  ASSERT_GE(snap.state->timetable.size(), 2u);
  ASSERT_GE(snap.state->locos.size(), 2u);
  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;

  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  const auto& evs = out.result.events;
  // Assert each entry has both Arrival and Departure, and Arrival index < Departure index for that entry
  auto a1 = FindFirstEventIndex(evs, DomainEventId::TrainArrived, tt1);
  auto d1 = FindFirstEventIndex(evs, DomainEventId::TrainDeparted, tt1);
  auto a2 = FindFirstEventIndex(evs, DomainEventId::TrainArrived, tt2);
  auto d2 = FindFirstEventIndex(evs, DomainEventId::TrainDeparted, tt2);
  ASSERT_TRUE(a1.has_value()); ASSERT_TRUE(d1.has_value());
  ASSERT_TRUE(a2.has_value()); ASSERT_TRUE(d2.has_value());
  EXPECT_LT(*a1, *d1);
  EXPECT_LT(*a2, *d2);
}

