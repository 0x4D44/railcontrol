#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(CommandsValidation, MissingPayloadsRejected) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Missing payload for SetDelayMode
  auto s1 = engine->Command(CommandPayload{CommandId::SetDelayMode, std::monostate{}});
  EXPECT_NE(s1.code, StatusCode::Ok);
  // Missing payload for AssignLoco
  auto s2 = engine->Command(CommandPayload{CommandId::AssignLoco, std::monostate{}});
  EXPECT_NE(s2.code, StatusCode::Ok);
}
