#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// Helper to generate minimal valid RCD with specified entity counts
static std::string GenerateRcdLayout(size_t numSections, size_t numRoutes, size_t numLocos, size_t numTimetable) {
  std::ostringstream oss;

  // [GENERAL]
  oss << "[GENERAL]\n";
  oss << "Name, TestLayout\n";
  oss << "StartTime, 800\n";
  oss << "StopTime, 1700\n";
  oss << "\n";

  // [SELECTOR] - need at least 1
  oss << "[SELECTOR]\n";
  oss << "1, 10, 10, 30, 30, 1, 0, UF\n";
  oss << "\n";

  // [SECTIONS]
  oss << "[SECTIONS]\n";
  for (size_t i = 1; i <= numSections; ++i) {
    int x = static_cast<int>((i % 10) * 50);
    int y = static_cast<int>((i / 10) * 50);
    oss << i << ", " << x << ", " << y << ", " << (x+50) << ", " << y << ", "
        << (x+50) << ", " << (y+10) << ", " << x << ", " << (y+10) << "\n";
  }
  oss << "\n";

  // [OVERLAPPING]
  oss << "[OVERLAPPING]\n";
  oss << "1, 1, 1\n"; // minimal valid entry
  oss << "\n";

  // [PLATFORMS]
  oss << "[PLATFORMS]\n";
  oss << "1, 10, 10, 60, 10, 60, 20, 10, 20\n";
  oss << "\n";

  // [ROUTES]
  oss << "[ROUTES]\n";
  for (size_t i = 1; i <= numRoutes; ++i) {
    // Route: ID, FromSel, ToSel, Stage1, Stage2, Stage3, Stage4, Stage5, Stage6
    size_t sec = std::min(i, numSections); // reference valid section
    oss << i << ", 1, 1, " << sec << ", 0, 0, 0, 0, 0\n";
  }
  oss << "\n";

  // [LOCOS]
  oss << "[LOCOS]\n";
  for (size_t i = 1; i <= numLocos; ++i) {
    oss << i << ", Stock" << i << ", 1, 100\n";
  }
  oss << "\n";

  // [LOCOYARD]
  oss << "[LOCOYARD]\n";
  oss << "1, 5\n";
  oss << "\n";

  // [TIMETABLE]
  oss << "[TIMETABLE]\n";
  for (size_t i = 1; i <= numTimetable; ++i) {
    // TT: ID, Name, Loco, ArrSel, ArrTime, DepSel, DepTime, Platform, StopMinutes, DelayMinutes, RouteId, NextEntry
    size_t locoId = ((i-1) % numLocos) + 1;
    int arrTime = 800 + static_cast<int>((i % 100) * 5);
    int depTime = arrTime + 10;
    oss << i << ", Train" << i << ", " << locoId << ", 1, " << arrTime
        << ", 1, " << depTime << ", 1, 5, 0, 0, 0\n";
  }
  oss << "\n";

  return oss.str();
}

TEST(EngineLimits, MaxTrainsExceeded) {
  EngineConfig cfg;
  cfg.maxActiveTrains = 10; // Set low limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with 15 locos (exceeds limit of 10)
  std::string rcdContent = GenerateRcdLayout(5, 5, 15, 5);
  auto tmpPath = WriteTemp("engine_limits_max_trains.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "ExceedsTrains";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::ValidationError);
  EXPECT_NE(s.message.find("Active trains exceed configured max"), std::string::npos);
}

TEST(EngineLimits, MaxTrainsAtLimit) {
  EngineConfig cfg;
  cfg.maxActiveTrains = 10; // Set limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with exactly 10 locos (at limit - should succeed)
  std::string rcdContent = GenerateRcdLayout(5, 5, 10, 5);
  auto tmpPath = WriteTemp("engine_limits_trains_at_limit.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "TrainsAtLimit";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(EngineLimits, MaxSectionsExceeded) {
  EngineConfig cfg;
  cfg.maxSections = 20; // Set low limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with 25 sections (exceeds limit of 20)
  std::string rcdContent = GenerateRcdLayout(25, 5, 5, 5);
  auto tmpPath = WriteTemp("engine_limits_max_sections.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "ExceedsSections";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::ValidationError);
  EXPECT_NE(s.message.find("Sections exceed configured max"), std::string::npos);
}

TEST(EngineLimits, MaxSectionsAtLimit) {
  EngineConfig cfg;
  cfg.maxSections = 20; // Set limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with exactly 20 sections (at limit - should succeed)
  std::string rcdContent = GenerateRcdLayout(20, 5, 5, 5);
  auto tmpPath = WriteTemp("engine_limits_sections_at_limit.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "SectionsAtLimit";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(EngineLimits, MaxRoutesExceeded) {
  EngineConfig cfg;
  cfg.maxRoutes = 15; // Set low limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with 20 routes (exceeds limit of 15)
  std::string rcdContent = GenerateRcdLayout(20, 20, 5, 5);
  auto tmpPath = WriteTemp("engine_limits_max_routes.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "ExceedsRoutes";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::ValidationError);
  EXPECT_NE(s.message.find("Routes exceed configured max"), std::string::npos);
}

TEST(EngineLimits, MaxRoutesAtLimit) {
  EngineConfig cfg;
  cfg.maxRoutes = 15; // Set limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with exactly 15 routes (at limit - should succeed)
  std::string rcdContent = GenerateRcdLayout(15, 15, 5, 5);
  auto tmpPath = WriteTemp("engine_limits_routes_at_limit.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "RoutesAtLimit";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(EngineLimits, MaxTimetableEntriesExceeded) {
  EngineConfig cfg;
  cfg.maxTimetableEntries = 30; // Set low limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with 35 timetable entries (exceeds limit of 30)
  std::string rcdContent = GenerateRcdLayout(10, 5, 10, 35);
  auto tmpPath = WriteTemp("engine_limits_max_timetable.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "ExceedsTimetable";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::ValidationError);
  EXPECT_NE(s.message.find("Timetable entries exceed configured max"), std::string::npos);
}

TEST(EngineLimits, MaxTimetableEntriesAtLimit) {
  EngineConfig cfg;
  cfg.maxTimetableEntries = 30; // Set limit
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);

  // Create layout with exactly 30 timetable entries (at limit - should succeed)
  std::string rcdContent = GenerateRcdLayout(10, 5, 10, 30);
  auto tmpPath = WriteTemp("engine_limits_timetable_at_limit.rcd", rcdContent);

  LayoutDescriptor desc;
  desc.sourcePath = tmpPath;
  desc.name = "TimetableAtLimit";

  Status s = engine->LoadLayout(desc);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}
