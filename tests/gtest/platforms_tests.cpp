#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Platforms, LargeCoordinatesAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Insert platform with 8 large numeric coordinates
  content.insert(lineEnd+1, "998, 100000,200000,300000,400000,500000,600000,700000,800000\n");
  auto tmp = WriteTemp("gtest_ok_platforms_largecoords.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okplatlarge";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Platforms, NegativeCoordinateRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[PLATFORMS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Insert platform with a negative coordinate; parser treats '-' as invalid numeric
  content.insert(lineEnd+1, "997, -1,2,3,4,5,6,7,8\n");
  auto tmp = WriteTemp("gtest_bad_platforms_negative.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badplatneg";
  Status s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
}

