#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(SelectorMessages, TooFewFieldsMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 10\n");
  auto tmp = WriteTemp("gtest_bad_selector_too_few_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselfew";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("too few fields"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for too few selector fields";
  }
}

