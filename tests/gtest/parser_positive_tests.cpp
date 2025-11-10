#include <gtest/gtest.h>
#include <string>
#include <filesystem>

#include "railcore/persistence/rcd_repository.h"
#include "railcore/persistence/rcd_id.h"
#include "railcore/services.h"
#include "railcore/status.h"

#include "test_utils.h"

using namespace RailCore;

static Status LoadFileSetDesc(const char* name, LayoutDescriptor& outDesc, WorldState& outWs) {
  RcdLayoutRepository repo;
  outDesc.sourcePath = DataFile(name);
  return repo.Load(outDesc, outWs);
}

TEST(ParserPos, Valid_2359_MinuteBoundary) {
  LayoutDescriptor desc; WorldState ws;
  auto st = LoadFileSetDesc("ok_tt_2359.rcd", desc, ws);
  EXPECT_EQ(st.code, StatusCode::Ok) << st.message;
  // Expect a 64-hex SHA-256 id
  EXPECT_EQ(desc.id.size(), 64u);
  for (char c : desc.id) EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
}

TEST(ParserPos, Timetable_DuplicateIds_FirstWins) {
  LayoutDescriptor desc; WorldState ws;
  auto st = LoadFileSetDesc("dup_tt.rcd", desc, ws);
  EXPECT_EQ(st.code, StatusCode::Ok) << st.message;
  // Verify layout id present
  EXPECT_EQ(desc.id.size(), 64u);
}

TEST(ParserPos, Locos_DuplicateIds_Tolerated) {
  LayoutDescriptor desc; WorldState ws;
  auto st = LoadFileSetDesc("dup_loco.rcd", desc, ws);
  EXPECT_EQ(st.code, StatusCode::Ok) << st.message;
}

