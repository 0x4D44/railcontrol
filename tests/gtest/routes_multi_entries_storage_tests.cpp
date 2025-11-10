#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesParsed, MultipleRoutesStoredIndependently) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Two routes: one encoded at first stage, one primary-only sequence
  content.insert(lineEnd+1, "48010, 5, 6, 9002, 0, 0, 0, 0, 0\n");
  content.insert(lineEnd+1, "48009, 1, 2, 1, 2, 3, 4, 5, 6\n");
  auto tmp = WriteTemp("gtest_routes_multi_entries_storage.rcd", content);

  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "multi";
  Status s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot();

  const Route* r1 = nullptr; const Route* r2 = nullptr;
  for (const auto& r : snap.state->routes) {
    if (r.id == 48009u) r1 = &r; else if (r.id == 48010u) r2 = &r;
  }
  ASSERT_NE(r1, nullptr);
  ASSERT_NE(r2, nullptr);
  EXPECT_EQ(r1->fromSelector, 1u); EXPECT_EQ(r1->toSelector, 2u);
  for (int i = 0; i < 6; ++i) { EXPECT_EQ(r1->stages[i].primary, static_cast<uint32_t>(i+1)); EXPECT_EQ(r1->stages[i].secondary, 0u); }
  EXPECT_EQ(r2->fromSelector, 5u); EXPECT_EQ(r2->toSelector, 6u);
  EXPECT_EQ(r2->stages[0].primary, 2u); EXPECT_EQ(r2->stages[0].secondary, 9u);
  for (int i = 1; i < 6; ++i) { EXPECT_EQ(r2->stages[i].primary, 0u); EXPECT_EQ(r2->stages[i].secondary, 0u); }
}

