#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// Verify that encoded stage tokens (secondary*1000 + primary) are decoded into
// Route::Stage {primary, secondary} in the parsed world state.
TEST(RoutesParsed, EncodedSecondaryStored) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Two encoded tokens at first two stages: (sec=7, pri=3) and (sec=9, pri=2)
  // Remaining stages are zeros.
  content.insert(lineEnd+1, "48002, 0, 0, 7003, 9002, 0, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_routes_encoded_storage.rcd", content);

  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "encoded";
  Status s = engine->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot();
  const Route* found = nullptr;
  for (const auto& r : snap.state->routes) { if (r.id == 48002u) { found = &r; break; } }
  ASSERT_NE(found, nullptr);
  ASSERT_EQ(found->stages[0].primary, 3u);
  ASSERT_EQ(found->stages[0].secondary, 7u);
  ASSERT_EQ(found->stages[1].primary, 2u);
  ASSERT_EQ(found->stages[1].secondary, 9u);
  for (int i = 2; i < 6; ++i) {
    EXPECT_EQ(found->stages[i].primary, 0u);
    EXPECT_EQ(found->stages[i].secondary, 0u);
  }
}

