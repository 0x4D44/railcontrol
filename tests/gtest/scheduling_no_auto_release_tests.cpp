#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Scheduling, NoAutoReleaseAfterDepart) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;

  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Consume assign delta
  (void)engine->Advance(std::chrono::milliseconds{0});
  // Advance to trigger arrival and departure across ticks
  (void)engine->Advance(std::chrono::milliseconds{300});
  (void)engine->Advance(std::chrono::milliseconds{300});

  // Assignment should still be present until explicitly released
  auto snap2 = engine->GetSnapshot();
  bool found=false; for (const auto& asn : snap2.state->assignments) { if (asn.timetableId == tt && asn.locoId == loco) { found=true; break; } }
  EXPECT_TRUE(found) << "Assignment auto-released unexpectedly after depart";

  // Explicit release should remove the assignment and emit an event on the next tick
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt, 0, AssignmentAction::Release}}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{100});
  bool sawReleased=false; for (const auto& ev : out.result.events) if (ev.id == DomainEventId::LocoReleased) sawReleased=true;
  EXPECT_TRUE(sawReleased);

  auto snap3 = engine->GetSnapshot();
  bool stillThere=false; for (const auto& asn : snap3.state->assignments) { if (asn.timetableId == tt) { stillThere=true; break; } }
  EXPECT_FALSE(stillThere) << "Assignment still present after explicit release";
}

