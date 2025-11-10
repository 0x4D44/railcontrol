#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetablePositive, NextEntryChainNonSequentialAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Non-sequential chain: 410 -> 420 -> 430 (ids not contiguous). All references exist.
  content.insert(lineEnd+1, "430, A, A, 1, 725, 0, 730, 1, 1, 0, 0, 0\n");
  content.insert(lineEnd+1, "420, A, A, 1, 715, 0, 720, 1, 1, 0, 0, 430\n");
  content.insert(lineEnd+1, "410, A, A, 1, 705, 0, 710, 1, 1, 0, 0, 420\n");
  auto tmp = WriteTemp("gtest_ok_tt_next_chain_nonseq.rcd", content);
  auto repo = std::make_shared<RcdLayoutRepository>(); EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "oknextns";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

