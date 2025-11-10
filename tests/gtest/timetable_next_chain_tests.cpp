#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetablePositive, NextEntryChainThreeAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Add three entries 480 -> 481 -> 482
  content.insert(lineEnd+1, "480, A, A, 1, 700, 0, 705, 1, 1, 0, 0, 481\n");
  content.insert(lineEnd+1, "481, A, A, 1, 710, 0, 715, 1, 1, 0, 0, 482\n");
  content.insert(lineEnd+1, "482, A, A, 1, 720, 0, 725, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_ok_tt_next_chain.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>();
  EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "oknext3";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

