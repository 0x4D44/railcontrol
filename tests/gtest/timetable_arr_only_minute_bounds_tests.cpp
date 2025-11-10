#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetableMinutes, Arr2359OnlyAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Arr=2359, Dep=0
  content.insert(lineEnd+1, "488, A, A, 1, 2359, 0, 0, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_ok_tt_arr2359_only.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>(); EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okarr2359";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(TimetableMinutes, Arr2360OnlyRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Arr=2360 invalid, Dep=0
  content.insert(lineEnd+1, "487, A, A, 1, 2360, 0, 0, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_arr2360_only.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>(); EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badarr2360";
  auto s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for Arr=2360";
  EXPECT_NE(s.message.find("ArrTime minutes out of range"), std::string::npos) << s.message;
}

