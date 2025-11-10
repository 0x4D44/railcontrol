#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Scheduling, ReleaseAfterArrivalSuppressesDepart) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Trigger arrival (~200ms) then release before depart threshold (~500ms)
  (void)engine->Advance(std::chrono::milliseconds{250});
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt, 0, AssignmentAction::Release}}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{300});
  bool sawDepart=false; for (const auto& ev : t2.result.events) if (ev.id == DomainEventId::TrainDeparted) sawDepart=true;
  EXPECT_FALSE(sawDepart);
}
