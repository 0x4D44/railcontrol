#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(EngineStop, AdvanceAfterStopRejectedThenResumeWorks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::Stop, std::monostate{}}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{10});
  EXPECT_EQ(out.status.code, StatusCode::InvalidCommand);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::Resume, std::monostate{}}).code, StatusCode::Ok);
  auto out2 = engine->Advance(std::chrono::milliseconds{10});
  EXPECT_EQ(out2.status.code, StatusCode::Ok);
}
