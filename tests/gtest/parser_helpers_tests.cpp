#include <gtest/gtest.h>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// Tests for internal parser helper function behavior via RCD parsing
// Targets: ParseFirstIntToken, TryParseInt, SplitCSV edge cases

TEST(ParserHelpers, GeneralKeyValueWithEqualsFormat) {
  // Test GENERAL section parsing with Key=Value format (vs CSV)
  std::string rcd = R"([GENERAL]
Name=TestLayout
StartTime=900
StopTime=1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_equals.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "EqualsFormat";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, GeneralKeyValueWithCommaFormat) {
  // Test GENERAL section parsing with "Key, Value" CSV format
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_comma.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "CommaFormat";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, CSVSplitTrailingComma) {
  // Test CSV split with trailing comma (should create empty token)
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF,

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_trailing_comma.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "TrailingComma";
  Status s = engine->LoadLayout(desc);
  // Should still parse successfully (extra empty field is ignored)
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, LeadingPlusSignIntegers) {
  // Test TryParseInt with leading + sign
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, +900
StopTime, +1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
+1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, +1, +1

[PLATFORMS]
+1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
+1, 1, 1, +1, 0, 0, 0, 0, 0

[LOCOS]
+1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
+1, Train1, +1, 1, +900, 1, +910, +1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_plus_sign.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "PlusSign";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, EmptyCSVTokens) {
  // Test CSV split with consecutive commas (empty tokens)
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, , , , ,

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_empty_tokens.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "EmptyTokens";
  Status s = engine->LoadLayout(desc);
  // Empty stage tokens are valid (treated as 0)
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, AllWhitespaceTokens) {
  // Test tokens with only whitespace (should trim to empty)
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1,   ,   ,   ,   ,

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_whitespace_tokens.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "WhitespaceTokens";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(ParserHelpers, SingleTokenNoComma) {
  // Test ParseFirstIntToken with no comma (entire line is token)
  std::string rcd = R"([GENERAL]
Name, TestLayout
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("parser_helpers_no_comma.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "NoComma";
  Status s = engine->LoadLayout(desc);
  // Should fail - sections need full coordinates
  EXPECT_NE(s.code, StatusCode::Ok);
}

TEST(ParserHelpers, MixedTabsAndSpacesWhitespace) {
  // Test whitespace trimming with mixed tabs and spaces
  std::string rcd = "[GENERAL]\n"
                    "Name,\t TestLayout  \n"
                    " \tStartTime,  900\t\n"
                    "\tStopTime, 1800  \t\n"
                    "\n"
                    "[SELECTOR]\n"
                    " \t1,  10, 10, 30, 30, 1, 0, UF\t \n"
                    "\n"
                    "[SECTIONS]\n"
                    "\t 1, 10, 10, 60, 10, 60, 20, 10, 20  \n"
                    "\n"
                    "[OVERLAPPING]\n"
                    "1, 1, 1\n"
                    "\n"
                    "[PLATFORMS]\n"
                    "1, 10, 10, 60, 10, 60, 20, 10, 20\n"
                    "\n"
                    "[ROUTES]\n"
                    "1, 1, 1, 1, 0, 0, 0, 0, 0\n"
                    "\n"
                    "[LOCOS]\n"
                    "1, Stock, 1, 100\n"
                    "\n"
                    "[LOCOYARD]\n"
                    "1, 5\n"
                    "\n"
                    "[TIMETABLE]\n"
                    "1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0\n";

  auto tmpPath = WriteTemp("parser_helpers_mixed_whitespace.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "MixedWhitespace";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}
