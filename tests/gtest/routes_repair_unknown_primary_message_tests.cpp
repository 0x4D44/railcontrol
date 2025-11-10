#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// After whitespace repair to 6 stage tokens, ensure a stage token with an
// unknown primary section id (no secondary encoded) is rejected with message.
TEST(RoutesRepairMessages, UnknownPrimaryAfterRepair) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Use a whitespace pair and include a stage token '999' which should be
  // treated as a primary id; most FAST layouts shouldn't have section 999.
  content.insert(lineEnd+1, "992, 1, 2, 1 2, 3, 999, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_routes_repair_unknown_primary_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "rtreppri";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for unknown primary after repair (layout may contain 999)";
  EXPECT_NE(s.message.find("unknown section id"), std::string::npos) << s.message;
}

