#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(EventOrder, ArrivalBeforeDepartureInSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());

  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  // Consume assignment tick, then advance sufficiently to trigger both arrival and departure
  (void)engine->Advance(std::chrono::milliseconds{0});
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);

  int idxArr = -1, idxDep = -1;
  for (size_t i = 0; i < out.result.events.size(); ++i) {
    if (out.result.events[i].id == DomainEventId::TrainArrived && idxArr == -1) idxArr = static_cast<int>(i);
    if (out.result.events[i].id == DomainEventId::TrainDeparted && idxDep == -1) idxDep = static_cast<int>(i);
  }
  ASSERT_GE(idxArr, 0);
  ASSERT_GE(idxDep, 0);
  EXPECT_LT(idxArr, idxDep);
}

