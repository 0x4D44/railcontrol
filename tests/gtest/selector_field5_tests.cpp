#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Selector, NonNumericAtField5Rejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id, f1..f4 numeric, f5 non-numeric, f6..f7 numeric
  content.insert(lineEnd+1, "999, 1, 2, 3, 4, xx, 6, 7, UF\n");
  auto tmp = WriteTemp("gtest_bad_selector_field5.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badself5";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

