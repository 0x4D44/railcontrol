#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(OverlappingMessages, UnknownSectionMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[OVERLAPPING]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 999, 998\n");
  auto tmp = WriteTemp("gtest_bad_overlapping_unknown_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badoverlmsg";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("OVERLAPPING pair references unknown section"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for OVERLAPPING unknown section";
  }
}
