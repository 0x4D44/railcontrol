#include <gtest/gtest.h>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Commands, UnknownCommandRejected) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  CommandPayload cp; cp.id = static_cast<CommandId>(999); cp.data = std::monostate{};
  auto s = engine->Command(cp);
  EXPECT_EQ(s.code, StatusCode::InvalidCommand);
}

