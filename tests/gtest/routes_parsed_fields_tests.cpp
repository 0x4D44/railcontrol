#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesParsed, StoresFromToAndStagePairs) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Use simple stage token pattern: 1,2,3,4,5,6 (no secondary)
  content.insert(lineEnd+1, "48001, 11, 22, 1, 2, 3, 4, 5, 6\n");
  auto tmp = WriteTemp("gtest_routes_parsed_fields.rcd", content);

  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "parsed";
  Status s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot();
  const Route* found = nullptr;
  for (const auto& r : snap.state->routes) { if (r.id == 48001u) { found = &r; break; } }
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->fromSelector, 11u);
  EXPECT_EQ(found->toSelector, 22u);
  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(found->stages[i].secondary, 0u);
    EXPECT_EQ(found->stages[i].primary, static_cast<uint32_t>(i+1));
  }
}

