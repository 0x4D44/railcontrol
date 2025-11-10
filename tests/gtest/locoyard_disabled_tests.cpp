#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Locoyard, DisabledOnlyAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOYARD]"); ASSERT_NE(pos, std::string::npos);
  auto next = content.find('\n', pos); if (next == std::string::npos) next = content.size();
  // Replace LOCOYARD section header with DISABLED line as the first entry
  content.insert(next+1, "DISABLED\n");
  auto tmp = WriteTemp("gtest_ok_locoyard_disabled_only.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "lydis";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Locoyard, DisabledAndEntriesAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOYARD]"); ASSERT_NE(pos, std::string::npos);
  auto next = content.find('\n', pos); if (next == std::string::npos) next = content.size();
  content.insert(next+1, "DISABLED\n1, 10\n");
  auto tmp = WriteTemp("gtest_ok_locoyard_disabled_and_entries.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "lydis2";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

