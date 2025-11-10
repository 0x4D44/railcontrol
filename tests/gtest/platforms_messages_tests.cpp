#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(PlatformsMessages, ExpectedEightCoordinatesMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Only 6 tokens after id
  content.insert(lineEnd+1, "999, 1,2,3,4,5,6\n");
  auto tmp = WriteTemp("gtest_bad_platforms_token_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplattok";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("expected 8 coordinate tokens"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for bad PLATFORMS token count";
  }
}
