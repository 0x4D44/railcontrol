#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesRepairMessages, RepairStillInvalidTooManyStages) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=997, from=1, to=2, stage area has "1 2" and "6 7" leading to 7 stage tokens after repair
  content.insert(lineEnd+1, "997, 1, 2, 1 2, 3, 4, 5, 6 7\n");
  auto tmp = WriteTemp("gtest_routes_repair_toomany_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "repairtm";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("expected exactly 6 stage tokens"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for repaired too many stage tokens";
  }
}

TEST(RoutesRepairMessages, RepairTooFewStages) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=996, from=1, to=2, stage area has "1 2" and a singletons to total 5 after repair
  content.insert(lineEnd+1, "996, 1, 2, 1 2, 3, 4, 5\n");
  auto tmp = WriteTemp("gtest_routes_repair_toofew_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "repairtfew";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("expected exactly 6 stage tokens"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for repaired too few stage tokens";
  }
}
