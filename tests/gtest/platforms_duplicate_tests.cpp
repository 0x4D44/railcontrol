#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Platforms, DuplicatePlatformIdRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Duplicate an existing platform id: 1 with 8 coords
  content.insert(lineEnd+1, "1, 0,0,0,0,0,0,0,0\n");
  auto tmp = WriteTemp("gtest_dup_platforms.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupplat";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

