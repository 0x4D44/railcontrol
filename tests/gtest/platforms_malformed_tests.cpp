#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Platforms, MalformedSeparatorsRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Use semicolons to break comma-based parsing and cause token count mismatch
  content.insert(lineEnd+1, "999; 1;2;3;4;5;6;7;8\n");
  auto tmp = WriteTemp("gtest_bad_platforms_semi.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplatsemi";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

