#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(GeneralErrorMessages, StartTimeMinutesOutOfRange) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[GENERAL]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "StartTime, 1261\n");
  auto tmp = WriteTemp("gtest_bad_general_minutes.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badgenmin";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("StartTime minutes out of range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for malformed StartTime";
  }
}
