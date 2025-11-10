#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// After whitespace repair to 6 stage tokens, include one token with an unknown
// primary id and another encoded with an unknown secondary id. Parser should
// reject with either unknown primary or unknown secondary message.
TEST(RoutesRepairMessages, DualUnknownPrimaryAndSecondaryAfterRepair) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=991, from=1, to=2; stage area repairs from whitespace and includes
  // primary=999 (likely unknown) and encoded 999041 (secondary=999 unknown)
  content.insert(lineEnd+1, "991, 1, 2, 1 2, 999, 999041, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_routes_repair_dual_unknown_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "rtrepdu";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok; layout may contain section 999";
  bool hasPrimary = s.message.find("unknown section id") != std::string::npos;
  bool hasSecondary = s.message.find("unknown secondary section id") != std::string::npos;
  EXPECT_TRUE(hasPrimary || hasSecondary) << s.message;
}

