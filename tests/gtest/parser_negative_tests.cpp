#include <gtest/gtest.h>
#include <string>
#include <filesystem>

#include "railcore/persistence/rcd_repository.h"
#include "railcore/services.h"
#include "railcore/status.h"

#include "test_utils.h"

using namespace RailCore;

static Status LoadFile(const char* name) {
  RcdLayoutRepository repo;
  LayoutDescriptor d; d.sourcePath = DataFile(name);
  WorldState ws;
  return repo.Load(d, ws);
}

static bool MsgHas(const Status& s, const char* needle) {
  return s.message.find(needle) != std::string::npos;
}

TEST(ParserNeg, General_StopTimeBeforeStart) {
  auto st = LoadFile("bad_general_order.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "GENERAL: StopTime must be greater than StartTime")) << st.message;
}

TEST(ParserNeg, Timetable_ArrTimeMinutesOutOfRange) {
  auto st = LoadFile("bad_tt_arrtime.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "ArrTime minutes out of range")) << st.message;
}

TEST(ParserNeg, Selector_TooFewFields) {
  auto st = LoadFile("bad_selector_fields.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Selector ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": too few fields")) << st.message;
}

TEST(ParserNeg, Selector_NonNumericField) {
  auto st = LoadFile("bad_selector_nonnumeric.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Selector ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": non-numeric field")) << st.message;
}

TEST(ParserNeg, Platforms_TokenCount) {
  auto st = LoadFile("bad_platforms.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Platform ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": expected 8 coordinate tokens")) << st.message;
}

TEST(ParserNeg, Routes_StageTokenCount) {
  auto st = LoadFile("bad_routes_tokens.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Route ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": expected exactly 6 stage tokens")) << st.message;
}

TEST(ParserNeg, Routes_UnknownSection) {
  auto st = LoadFile("bad_routes_section.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Route ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": unknown section id")) << st.message;
}

TEST(ParserNeg, Routes_UnknownFromSelector) {
  auto st = LoadFile("bad_routes_unknown_selector.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Route ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": unknown FromSelector")) << st.message;
}

TEST(ParserNeg, Routes_UnknownSecondarySection) {
  auto st = LoadFile("bad_routes_secondary.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "Route ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": unknown secondary section id")) << st.message;
}

TEST(ParserNeg, Timetable_TooFewFields) {
  auto st = LoadFile("bad_tt_fields.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "TIMETABLE ")) << st.message;
  EXPECT_TRUE(MsgHas(st, ": too few fields")) << st.message;
}

TEST(ParserNeg, Timetable_ArrSelectorOutOfRange) {
  auto st = LoadFile("bad_tt_arrsel.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "ArrSelector out of allowed range")) << st.message;
}

TEST(ParserNeg, Overlapping_UnknownSectionRef) {
  auto st = LoadFile("bad_overlap.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "OVERLAPPING pair references unknown section")) << st.message;
}

TEST(ParserNeg, Locoyard_Invalid) {
  auto st = LoadFile("bad_locoyard.rcd");
  EXPECT_NE(st.code, StatusCode::Ok);
  EXPECT_TRUE(MsgHas(st, "LOCOYARD:")) << st.message;
}
