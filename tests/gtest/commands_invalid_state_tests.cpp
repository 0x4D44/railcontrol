#include <gtest/gtest.h>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"

using namespace RailCore;

TEST(CommandsInvalidState, BeforeLoadInvalidCommands) {
  EngineConfig cfg; auto engine = CreateEngine(cfg, nullptr, nullptr, nullptr, nullptr, nullptr);
  ASSERT_TRUE(engine);
  auto s1 = engine->Command(CommandPayload{CommandId::Pause, std::monostate{}});
  EXPECT_EQ(s1.code, StatusCode::InvalidCommand);
  auto s2 = engine->Command(CommandPayload{CommandId::Resume, std::monostate{}});
  EXPECT_EQ(s2.code, StatusCode::InvalidCommand);
  auto s3 = engine->Command(CommandPayload{CommandId::Stop, std::monostate{}});
  EXPECT_EQ(s3.code, StatusCode::InvalidCommand);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  auto s4 = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
  EXPECT_EQ(s4.code, StatusCode::InvalidCommand);
  LocoAssignment la{1,1,AssignmentAction::Assign};
  auto s5 = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  EXPECT_EQ(s5.code, StatusCode::InvalidCommand);
  auto s6 = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
  EXPECT_EQ(s6.code, StatusCode::InvalidCommand);
}
