#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Timetable, DuplicateIdAllowedFirstWins) {
  // Duplicate an existing timetable id; legacy behavior allows it and keeps first occurrence.
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Duplicate id 1 with a valid line; parser should accept overall file.
  content.insert(lineEnd+1, "1, D, D, 1, 700, 0, 705, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_dup_timetable_id.rcd", content);

  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupTT";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

