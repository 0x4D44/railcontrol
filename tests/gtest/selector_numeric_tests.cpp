#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Selector, NonNumericAtField6Rejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Tokens: id, f1, f2, f3, f4, f5, f6(non-numeric), f7, f8
  content.insert(lineEnd+1, "999, 1, 2, 3, 4, 5, xx, 7, UF\n");
  auto tmp = WriteTemp("gtest_bad_selector_field6.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badself6";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Selector, EighthTokenMayBeNonNumeric) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // First 7 numeric; token 8 arbitrary (UF)
  content.insert(lineEnd+1, "998, 10, 20, 30, 40, 50, 60, 70, UF\n");
  auto tmp = WriteTemp("gtest_ok_selector_token8.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "oksel8";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

