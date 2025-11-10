#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(General, UnknownKeysIgnored) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[GENERAL]"); ASSERT_NE(pos, std::string::npos);
  auto next = content.find('\n', pos); if (next == std::string::npos) next = content.size();
  // Inject an unknown key and a known key to preserve validity
  content.insert(next+1, "FooBar=123\nStartTime=0700\nStopTime=0800\n");
  auto tmp = WriteTemp("gtest_ok_general_unknown_keys.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "genunk";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

