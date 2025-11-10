#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/persistence/rcd_id.h"
#include "railcore/persistence/rcd_repository.h"
#include "railcore/engine_factory.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

static std::filesystem::path RepoRoot() {
  auto cwd = std::filesystem::current_path();
  if (std::filesystem::exists(cwd / "Game files" / "FAST.RCD")) return cwd;
  return cwd / ".." / ".." / "..";
}

static std::filesystem::path DataFile(const char* name) {
  auto root = RepoRoot();
  if (std::filesystem::exists(root / name)) return root / name;
  return root / "Game files" / name;
}

TEST(Id, CanonicalizationStability) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  std::string canon = CanonicalizeRcdContent(content);
  auto id1 = ComputeRcdIdFromContent(canon);
  std::string modified;
  modified.reserve(content.size() + content.size()/10);
  for (char c : content) { modified.push_back(c); if (c=='\n') { modified.push_back(' '); modified.push_back(' '); } }
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(modified));
  EXPECT_EQ(id1, id2); EXPECT_EQ(id1.size(), 64u);
  std::string crlf; for (char c : content) { if (c=='\n') { crlf.push_back('\r'); crlf.push_back('\n'); } else crlf.push_back(c); }
  auto id3 = ComputeRcdIdFromContent(CanonicalizeRcdContent(crlf));
  EXPECT_EQ(id1, id3);
}

TEST(Id, EngineLayoutIdMatches) {
  auto canon = CanonicalizeRcdContent(ReadAll(DataFile("FAST.RCD")));
  auto id1 = ComputeRcdIdFromContent(canon);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  EXPECT_EQ(engine->GetLayoutId(), id1);
}

TEST(Id, CanonicalizationTabsAndLeadingTrimmed) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto id1 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content));
  // Insert tabs and leading spaces at the start of lines; canonicalization should trim
  std::string withTabs;
  withTabs.reserve(content.size()*2);
  bool startOfLine = true;
  for (char c : content) {
    if (startOfLine) { withTabs.push_back('\t'); withTabs.push_back(' '); }
    withTabs.push_back(c);
    startOfLine = (c == '\n');
  }
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(withTabs));
  EXPECT_EQ(id1, id2); EXPECT_EQ(id1.size(), 64u);
}

TEST(Id, TrailingEmptyLinesIgnored) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto id1 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content));
  // Append a few blank lines
  std::string withTrailing = content + "\n\n\n";
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(withTrailing));
  EXPECT_EQ(id1, id2);
}

TEST(Id, DifferentLayoutsProduceDifferentIds) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d1; d1.sourcePath = DataFile("FAST.RCD"); d1.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d1).code, StatusCode::Ok);
  auto id1 = engine->GetLayoutId();
  LayoutDescriptor d2; d2.sourcePath = DataFile("KINGSX.RCD"); d2.name = "KINGSX";
  ASSERT_EQ(engine->LoadLayout(d2).code, StatusCode::Ok);
  auto id2 = engine->GetLayoutId();
  EXPECT_FALSE(id1.empty());
  EXPECT_FALSE(id2.empty());
  EXPECT_NE(id1, id2);
}
