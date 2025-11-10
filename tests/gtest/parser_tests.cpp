#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/persistence/rcd_repository.h"
#include "railcore/engine_factory.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Parser, GeneralStopBeforeStartRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[GENERAL]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "StartTime, 1000\nStopTime, 0930\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_general.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "gtest_badgen";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesMissingCommaRepaired) {
  std::ifstream in(DataFile("WAVERLY.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  // WAVERLY contains known lines with missing commas between tokens; ensure it loads
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("WAVERLY.RCD"); d.name = "WAVERLY";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Parser, SelectorTooFewFieldsRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 10\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_selector_fields.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badsel2";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SelectorNonNumericFieldRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Put non-numeric at x coordinate (token 2)
  content.insert(lineEnd+1, "999, xx, 10, 10, 10, 1, 0, UF\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_selector_nonnumeric.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselnum";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SelectorIdOutOfRangeRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // id 0 invalid
  content.insert(lineEnd+1, "0, 10, 10, 10, 10, 1, 0, UF\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_selector_id.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselid";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SelectorUpperBoundRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // id 1000 invalid (upper bound is 999)
  content.insert(lineEnd+1, "1000, 10, 10, 10, 10, 1, 0, UF\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_selector_upper.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselup";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SelectorNegativeNumericRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // negative numeric in token 2 should be rejected by TryParseInt
  content.insert(lineEnd+1, "999, -10, 10, 10, 10, 1, 0, UF\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_selector_negative.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badselneg";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SelectorDuplicateIdRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "1,0,0,0,0,1,0,UF\n");
  auto tmp = WriteTemp("gtest_dup_selector.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupsel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SectionsOutOfRangeRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SECTIONS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "1001, 0,0,0,0,0,0,0,0\n");
  auto tmp = WriteTemp("gtest_bad_section_range.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badsecrng";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, SectionsDuplicateIdRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[SECTIONS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // duplicate id 1
  content.insert(lineEnd+1, "1, 0,0,0,0,0,0,0,0\n");
  auto tmp = WriteTemp("gtest_dup_section.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupsec";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesUnknownSelectorRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 999, 999, 1, 2, 3, 4, 5, 6\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_selector.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badrtsel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesEncodedSectionsAccepted) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Add a valid route with encoded stage (1004 encodes secondary=1, primary=4) and valid section ids
  content.insert(lineEnd+1, "999, 1, 2, 1004, 5, 6, 7, 8, 9\n");
  auto tmp = std::filesystem::current_path() / "gtest_ok_route_encoded.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okroute";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Parser, RoutesMultipleEncodedStagesAccepted) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Two encoded stages: 1004 (sec=1, pri=4) and 2005 (sec=2, pri=5)
  content.insert(lineEnd+1, "998, 1, 2, 1004, 2005, 6, 7, 8, 9\n");
  auto tmp = std::filesystem::current_path() / "gtest_ok_route_multi_encoded.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okroute2";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Parser, RoutesUnknownSecondaryRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // 999041 encodes secondary=999 (unknown), primary=41 (likely exists)
  content.insert(lineEnd+1, "999, 1, 2, 1006, 12017, 23026, 30040, 999041, 48\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_secondary.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badroute2";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesUnknownFromSelectorRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Unknown from-selector 999, to-selector 1
  content.insert(lineEnd+1, "999, 999, 1, 1, 2, 3, 4, 5, 6\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_fromsel.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badfromsel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesUnknownToSelectorRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Known from-selector 1, unknown to-selector 999
  content.insert(lineEnd+1, "999, 1, 999, 1, 2, 3, 4, 5, 6\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_tosel.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtosel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesZeroSelectorsAccepted) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // from=0 and to=0 should be accepted per tolerant parser logic
  content.insert(lineEnd+1, "999, 0, 0, 1, 2, 3, 4, 5, 6\n");
  auto tmp = std::filesystem::current_path() / "gtest_ok_routes_zeroselectors.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okzeroselectors";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}


TEST(Parser, LocoYardInvalidStockRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[LOCOYARD]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "99, 10\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_locoyard.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badlyd";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, LocoYardInvalidOffsetRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOYARD]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // invalid refuel offset 99
  content.insert(lineEnd+1, "1, 99\n");
  auto tmp = WriteTemp("gtest_bad_locoyard_off.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badlyo";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}


TEST(Parser, TimetableMinuteBounds) {
  // 2359 valid, 2360 invalid
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string base = oss.str();
  // Valid 2359
  {
    std::string content = base;
    auto pos = content.find("[TIMETABLE]");
    ASSERT_NE(pos, std::string::npos);
    auto lineEnd = content.find('\n', pos);
    if (lineEnd == std::string::npos) lineEnd = content.size();
    content.insert(lineEnd+1, "450, T, T, 1, 2359, 0, 2359, 1, 1, 0, 0, 0\n");
    auto tmp = std::filesystem::current_path() / "gtest_ok_tt_2359.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "ok2359";
    Status s = engine->LoadLayout(d);
    EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
  }
  // Invalid 2360
  {
    std::string content = base;
    auto pos = content.find("[TIMETABLE]");
    ASSERT_NE(pos, std::string::npos);
    auto lineEnd = content.find('\n', pos);
    if (lineEnd == std::string::npos) lineEnd = content.size();
    content.insert(lineEnd+1, "451, T, T, 1, 2360, 0, 2360, 1, 1, 0, 0, 0\n");
    auto tmp = std::filesystem::current_path() / "gtest_bad_tt_2360.rcd";
    std::ofstream out(tmp, std::ios::binary); out << content; out.close();
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = tmp; d.name = "bad2360";
    Status s = engine->LoadLayout(d);
    EXPECT_NE(s.code, StatusCode::Ok);
  }
}

TEST(Parser, TimetableNextEntryUnknownRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[TIMETABLE]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "452, T, T, 1, 0700, 0, 0705, 1, 1, 0, 0, 999\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_tt_next.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badnext";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(ParserErrorMessages, RoutesUnknownSecondaryMessage) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 1, 2, 1006, 12017, 23026, 30040, 999041, 48\n");
  auto tmp = WriteTemp("gtest_bad_routes_secondary_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badroute2msg";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("unknown secondary section id"), std::string::npos);
}

TEST(ParserErrorMessages, MissingRequiredSectionsMessage) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  // Remove the [SELECTOR] header to trigger missing required sections
  auto pos = content.find("[SELECTOR]"); ASSERT_NE(pos, std::string::npos);
  content.replace(pos, std::string("[SELECTOR]").size(), "[SELX]");
  auto tmp = WriteTemp("gtest_missing_required_sections.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "missreq";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("Missing required sections"), std::string::npos);
}

TEST(ParserErrorMessages, DuplicateSectionsMessage) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  // Prepend an extra [SECTIONS] header to trigger duplicate sections
  std::string dup = std::string("[SECTIONS]\n1,1,1,1,1,1,1,1\n") + content;
  auto tmp = WriteTemp("gtest_duplicate_sections_msg.rcd", dup);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupmsg";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("Duplicate sections"), std::string::npos);
}

TEST(ParserErrorMessages, RoutesStageTokenRepairStillInvalid) {
  // Craft a routes line where missing commas lead to 7 stage tokens after whitespace split (10 total tokens),
  // which should be rejected with the exact 6 stage tokens error.
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=997, from=1, to=2, stage area has "1 2" and "6 7" which become 7 stage tokens after split
  content.insert(lineEnd+1, "997, 1, 2, 1 2, 3, 4, 5, 6 7\n");
  auto tmp = WriteTemp("gtest_bad_routes_repair_still_invalid.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badrepair";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("expected exactly 6 stage tokens"), std::string::npos);
}

TEST(ParserErrorMessages, RoutesStageTokenRepairTooFewTokens) {
  // After whitespace repair there are only 5 stage tokens; expect the 6-stage error.
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // id=996, from=1, to=2, stage area has "1 2" and a singletons to total 5 after repair
  content.insert(lineEnd+1, "996, 1, 2, 1 2, 3, 4, 5\n");
  auto tmp = WriteTemp("gtest_bad_routes_repair_toofew.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badrepairfew";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  EXPECT_NE(s.message.find("expected exactly 6 stage tokens"), std::string::npos);
}

TEST(Parser, PlatformsNonNumericRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[PLATFORMS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, xx, 0, 0, 0, 0, 0, 0, 0\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_platforms.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplat";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, PlatformsTooFewTokensRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[PLATFORMS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // only id + 7 coords (should be id + 8)
  content.insert(lineEnd+1, "999, 0,0,0,0,0,0,0\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_platforms_count.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplatcnt";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, OverlappingUnknownSectionRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[OVERLAPPING]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "999, 999, 998\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_overlap.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badovl";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesUnknownPrimaryRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Primary section id 999 (unknown), no secondary encoded
  content.insert(lineEnd+1, "995, 1, 2, 999, 1, 2, 3, 4, 5\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_primary.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badprim";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, OverlappingUnknownSectionRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[OVERLAPPING]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // reference unknown section 999 and 1000
  content.insert(lineEnd+1, "99, 999, 1000\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_overlap.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badoverl";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, GeneralMissingStartStopRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  // Replace GENERAL header with empty GENERAL block
  auto pos = content.find("[GENERAL]");
  ASSERT_NE(pos, std::string::npos);
  auto endLine = content.find('\n', pos); if (endLine == std::string::npos) endLine = content.size();
  auto nextHeader = content.find('[', endLine);
  content.erase(endLine+1, nextHeader - (endLine+1));
  auto tmp = std::filesystem::current_path() / "gtest_bad_general_missing.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badgenmiss";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, TimetableArrSelectorOutOfRangeRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "498, BadSel, BadSel, 55, 700, 0, 705, 3, 3, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_arrsel.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtsel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, TimetableDepMinutesOutOfRangeRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[TIMETABLE]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "499, T, T, 1, 0700, 0, 0760, 1, 1, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_tt_depmin.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "baddepmin";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, DuplicateRouteIdRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "1, 1, 2, 0, 0, 0, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_dup_route.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "duprte";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, DuplicatePlatformIdRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "1, 0,0,0,0,0,0,0,0\n");
  auto tmp = WriteTemp("gtest_dup_platform.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dupplat";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, DuplicateLocoIdRejected) {
  auto content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOS]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "1, 31, 999, 1\n");
  auto tmp = WriteTemp("gtest_dup_loco.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "duploco";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesTooFewStagesRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // Only id, from, to
  content.insert(lineEnd+1, "999, 1, 2\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_toofew.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "toofew";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, RoutesTooManyStagesRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[ROUTES]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  // id, from, to + 7 stage tokens (one extra)
  content.insert(lineEnd+1, "999, 1, 2, 1, 2, 3, 4, 5, 6, 7\n");
  auto tmp = std::filesystem::current_path() / "gtest_bad_routes_toomany.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "toomany";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, LocoYardDisabledAccepted) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[LOCOYARD]");
  ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos);
  if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "Disabled\n");
  auto tmp = std::filesystem::current_path() / "gtest_ok_locoyard_disabled.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "oklyd";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Parser, MissingSelectorSectionRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string content = oss.str();
  auto pos = content.find("[SELECTOR]");
  ASSERT_NE(pos, std::string::npos);
  content.replace(pos, std::string("[SELECTOR]").size(), "[SELX]");
  auto tmp = std::filesystem::current_path() / "gtest_missing_selector.rcd";
  std::ofstream out(tmp, std::ios::binary); out << content; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "missel";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(Parser, DuplicateSectionsHeaderRejected) {
  std::ifstream in(DataFile("FAST.RCD"), std::ios::binary);
  ASSERT_TRUE(in.good());
  std::ostringstream oss; oss << in.rdbuf();
  std::string dup = std::string("[SECTIONS]\n1,1,1,1,1,1,1,1\n") + oss.str();
  auto tmp = std::filesystem::current_path() / "gtest_dup_sections.rcd";
  std::ofstream out(tmp, std::ios::binary); out << dup; out.close();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "dup";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}
