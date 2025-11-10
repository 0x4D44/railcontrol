#include <gtest/gtest.h>
#include <string>
#include "delta_helpers.h"
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(DeltaComposite, OrderAgnosticPresenceAcrossMultipleEntriesAndGlobals) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2+2";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  EXPECT_TRUE(DeltaHasEntryField(out.result.delta, tt1, "assignedLocoId", std::to_string(l1).c_str()));
  EXPECT_TRUE(DeltaHasEntryField(out.result.delta, tt2, "assignedLocoId", std::to_string(l2).c_str()));
  EXPECT_TRUE(DeltaGlobalsHave(out.result.delta, {"delay.mode","delay.thresholdMinutes"}));
}

