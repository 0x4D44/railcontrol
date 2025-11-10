#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RepositoryMessages, LayoutFileNotFoundMessage) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = RepoRoot() / "no_such.rcd"; d.name = "missing";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::NotFound);
  EXPECT_NE(s.message.find("Layout file not found"), std::string::npos);
}

TEST(RepositoryMessages, EmptyLayoutFileMessage) {
  // Create an empty temp file
  std::filesystem::path tmp = std::filesystem::current_path() / "gtest_empty.rcd";
  {
    std::ofstream out(tmp, std::ios::binary); /* empty */
  }
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "empty";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("Layout file is empty"), std::string::npos);
}
