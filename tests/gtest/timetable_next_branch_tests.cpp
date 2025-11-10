#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetablePositive, NextEntryBranchTargetsExistingIdsAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Build a small set: 600->610, 610->620; and 605->620 (branch)
  content.insert(lineEnd+1, "620, A, A, 1, 830, 0, 835, 1, 1, 0, 0, 0\n");
  content.insert(lineEnd+1, "610, A, A, 1, 820, 0, 825, 1, 1, 0, 0, 620\n");
  content.insert(lineEnd+1, "600, A, A, 1, 810, 0, 815, 1, 1, 0, 0, 610\n");
  content.insert(lineEnd+1, "605, A, A, 1, 812, 0, 817, 1, 1, 0, 0, 620\n");
  auto tmp = WriteTemp("gtest_ok_tt_next_branch.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okttbr";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

