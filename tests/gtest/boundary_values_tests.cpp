#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// Tests for boundary values (min/max valid IDs)
// Ensures parser handles edge cases at ID range limits

TEST(BoundaryValues, SectionIdMax999) {
  // Maximum valid section ID is 999
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
999, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 999, 999

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 999, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_section_999.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "SectionMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, SectionIdMin1) {
  // Minimum valid section ID is 1
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
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

  auto tmpPath = WriteTemp("boundary_section_1.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "SectionMin";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, PlatformIdMax999) {
  // Maximum valid platform ID is 999
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
999, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 999, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_platform_999.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "PlatformMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, SelectorIdMax999) {
  // Maximum valid selector ID is 999
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF
999, 20, 20, 30, 30, 1, 0, DF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 1, 999, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_selector_999.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "SelectorMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, RouteIdMax999) {
  // Maximum valid route ID is 999
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
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
999, 1, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_route_999.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "RouteMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, LocoIdMax499) {
  // Maximum valid loco ID is 499
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
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
499, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 499, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_loco_499.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "LocoMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, TimetableIdMax499) {
  // Maximum valid timetable ID is 499
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
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
499, Train499, 1, 1, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_timetable_499.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "TimetableMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, SelectorArrSelectorMax49) {
  // Maximum valid ArrSelector for timetable is 49 (input selectors only)
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 900
StopTime, 1800

[SELECTOR]
1, 10, 10, 30, 30, 1, 0, UF
49, 20, 20, 30, 30, 1, 0, DF

[SECTIONS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[OVERLAPPING]
1, 1, 1

[PLATFORMS]
1, 10, 10, 60, 10, 60, 20, 10, 20

[ROUTES]
1, 49, 1, 1, 0, 0, 0, 0, 0

[LOCOS]
1, Stock, 1, 100

[LOCOYARD]
1, 5

[TIMETABLE]
1, Train1, 1, 49, 900, 1, 910, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_arrselector_49.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "ArrSelectorMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, StartTimeZero) {
  // Test StartTime at minimum boundary (0 = 00:00)
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 0
StopTime, 1200

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
1, Train1, 1, 1, 100, 1, 110, 1, 5, 0, 0, 0
)";

  auto tmpPath = WriteTemp("boundary_starttime_0.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "StartTimeZero";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(BoundaryValues, StopTime2359) {
  // Test StopTime at maximum valid time (23:59)
  std::string rcd = R"([GENERAL]
Name, BoundaryTest
StartTime, 800
StopTime, 2359

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

  auto tmpPath = WriteTemp("boundary_stoptime_2359.rcd", rcd);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor desc; desc.sourcePath = tmpPath; desc.name = "StopTimeMax";
  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}
