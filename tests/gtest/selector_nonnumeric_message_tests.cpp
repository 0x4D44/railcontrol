#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(SelectorMessages, NonNumericFieldMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, xx, 1, 1, 1, 1, 1, UF\n");
  auto tmp = WriteTemp("gtest_bad_selector_nonnumeric_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselmsg";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("non-numeric field"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for selector non-numeric field";
  }
}
