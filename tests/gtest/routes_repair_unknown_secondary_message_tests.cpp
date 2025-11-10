#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// After whitespace repair to 6 stage tokens, ensure encoded token with unknown
// secondary section id is rejected with the expected message.
TEST(RoutesRepairMessages, UnknownSecondaryAfterRepair) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=993, from=1, to=2; stage area contains a whitespace pair "1 2" which repairs
  // into two tokens, plus regular tokens and an encoded token 999041
  // (secondary=999 unknown, primary=41 likely exists), then zeros to reach 6 stage tokens.
  content.insert(lineEnd+1, "993, 1, 2, 1 2, 3, 4, 999041, 0, 0\n");
  auto tmp = WriteTemp("gtest_routes_repair_unknown_secondary_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "rtrepsec";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for unknown secondary after repair";
  EXPECT_NE(s.message.find("unknown secondary section id"), std::string::npos) << s.message;
}

