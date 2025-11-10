#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/persistence/rcd_repository.h"
#include "railcore/engine_factory.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Repository, FallbackToSiblingGameFiles) {
  // Point to repo root FAST.RCD (does not exist), repository should fallback to sibling "Game files/FAST.RCD".
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  auto root = RepoRoot();
  LayoutDescriptor d; d.sourcePath = root / "FAST.RCD"; d.name = "FAST";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

