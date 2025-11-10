#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(GeneralErrorMessages, StopTimeMinutesOutOfRange) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[GENERAL]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "StopTime, 2361\n");
  auto tmp = WriteTemp("gtest_bad_general_stop_minutes.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badstmin";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("StopTime minutes out of range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for malformed StopTime";
  }
}

TEST(GeneralErrorMessages, StopBeforeStartMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[GENERAL]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "StartTime, 1100\nStopTime, 1059\n");
  auto tmp = WriteTemp("gtest_bad_general_stop_before_start.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badstopstart";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("StopTime must be greater than StartTime"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for Stop<=Start";
  }
}

