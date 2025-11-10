#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Commands, SetDelayModeValidation) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Invalid threshold (>=1440) rejected
  DelaySettings bad; bad.mode = DelayMode::Randomized; bad.threshold = std::chrono::minutes{1440};
  auto s1 = engine->Command(CommandPayload{CommandId::SetDelayMode, bad});
  EXPECT_NE(s1.code, StatusCode::Ok);
  // Valid threshold accepted and reflected via globals delta on next tick
  DelaySettings ok; ok.mode = DelayMode::MaintenanceOnly; ok.threshold = std::chrono::minutes{2};
  auto s2 = engine->Command(CommandPayload{CommandId::SetDelayMode, ok});
  ASSERT_EQ(s2.code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  auto it = out.result.delta->globals.find("delay.thresholdMinutes");
  ASSERT_NE(it, out.result.delta->globals.end());
  EXPECT_EQ(it->second, std::to_string(ok.threshold.count()));
}

TEST(Commands, AssignReleaseUnknownIds) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Unknown timetable id
  LocoAssignment la; la.timetableId = 9999; la.locoId = 1; la.action = AssignmentAction::Assign;
  auto s1 = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  EXPECT_EQ(s1.code, StatusCode::NotFound);
  // Unknown loco id
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty());
  la.timetableId = snap.state->timetable.front().id; la.locoId = 9999; la.action = AssignmentAction::Assign;
  auto s2 = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  EXPECT_EQ(s2.code, StatusCode::NotFound);
  // Release with no assignment returns NotFound
  la.locoId = 0; la.action = AssignmentAction::Release;
  auto s3 = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
  EXPECT_EQ(s3.code, StatusCode::NotFound);
}
