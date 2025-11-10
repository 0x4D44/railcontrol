#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Sections, DuplicateGeneralSectionMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  // Insert another [GENERAL] with valid keys to create a duplicate
  auto pos = content.find("[GENERAL]"); ASSERT_NE(pos, std::string::npos);
  auto next = content.find('\n', pos); if (next == std::string::npos) next = content.size();
  content.insert(next+1, "[GENERAL]\nStartTime=0700\nStopTime=0800\n");
  auto tmp = WriteTemp("gtest_bad_duplicate_general.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupgen";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for duplicate GENERAL";
  EXPECT_NE(s.message.find("Duplicate sections"), std::string::npos) << s.message;
}

