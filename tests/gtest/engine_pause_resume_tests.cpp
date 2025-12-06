#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(EnginePauseResume, IdempotentCommands) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Pause twice is Ok
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Pause, std::monostate{}}).code, StatusCode::Ok);
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Pause, std::monostate{}}).code, StatusCode::Ok);
  // Resume twice is Ok
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Resume, std::monostate{}}).code, StatusCode::Ok);
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Resume, std::monostate{}}).code, StatusCode::Ok);
}

TEST(EnginePauseResume, AdvanceWhilePausedNoOp) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  auto snapBefore = engine->GetSnapshot();
  ASSERT_TRUE(snapBefore.state);
  auto pauseStatus = engine->Command(CommandPayload{CommandId::Pause, std::monostate{}});
  EXPECT_EQ(pauseStatus.code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{50});
  EXPECT_EQ(out.status.code, StatusCode::Ok);
  EXPECT_FALSE(out.result.delta);
  EXPECT_EQ(out.result.clock, snapBefore.state->clock);

  auto snapAfter = engine->GetSnapshot();
  EXPECT_EQ(snapAfter.state->clock, snapBefore.state->clock);
  EXPECT_EQ(snapAfter.state->tickId, snapBefore.state->tickId);
}
