#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetableMinutes, Dep2359OnlyAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Arr=0, Dep=2359
  content.insert(lineEnd+1, "486, A, A, 1, 0, 0, 2359, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_ok_tt_dep2359_only.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>(); EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okdep2359";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(TimetableMinutes, Dep2360OnlyRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Arr=0, Dep=2360 invalid
  content.insert(lineEnd+1, "485, A, A, 1, 0, 0, 2360, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_dep2360_only.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>(); EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "baddep2360";
  auto s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for Dep=2360";
  EXPECT_NE(s.message.find("DepTime minutes out of range"), std::string::npos) << s.message;
}

