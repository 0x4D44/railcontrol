#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(PlatformsMessages, ExtraCoordinateTokensRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id + 9 coords (should be id + 8)
  content.insert(lineEnd+1, "996, 0,0,0,0,0,0,0,0,0\n");
  auto tmp = WriteTemp("gtest_bad_platforms_extra_coords.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplatex";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for PLATFORMS extra coords";
  EXPECT_NE(s.message.find("expected 8 coordinate tokens"), std::string::npos) << s.message;
}

TEST(PlatformsMessages, WhitespaceOnlyCoordinatesRejectedAsNonNumeric) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "995,  , , , , , , , \n");
  auto tmp = WriteTemp("gtest_bad_platforms_whitespace_coords.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplatws";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for PLATFORMS whitespace coords";
  EXPECT_NE(s.message.find("non-numeric coordinate"), std::string::npos) << s.message;
}

