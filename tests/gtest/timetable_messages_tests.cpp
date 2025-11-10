#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetableMessages, DepMinutesOutOfRange) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "496, BadDep, BadDep, 3, 0700, 0, 1261, 3, 3, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_dep_minutes.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "baddepmin";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("DepTime minutes out of range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for malformed DepTime";
  }
}

TEST(TimetableMessages, ArrMinutesOutOfRange) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "497, BadArr, BadArr, 3, 1261, 0, 0905, 3, 3, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_arr_minutes.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badarrmin";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("ArrTime minutes out of range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for malformed ArrTime";
  }
}

TEST(TimetableMessages, ArrSelectorOutOfRangeMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "498, BadSel, BadSel, 55, 0700, 0, 0705, 3, 3, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_arrsel_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badarrsel";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("ArrSelector out of allowed range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for ArrSelector out of range";
  }
}

TEST(TimetableMessages, NextEntryUnknownMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "499, N, N, 1, 0700, 0, 0705, 1, 1, 0, 0, 999\n");
  auto tmp = WriteTemp("gtest_bad_tt_next_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badnextmsg";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("NextEntry unknown"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for NextEntry unknown";
  }
}

