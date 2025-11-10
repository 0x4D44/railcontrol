#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

static void GetTwoSections(uint32_t& a, uint32_t& b) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_GE(snap.state->sections.size(), 2u);
  a = snap.state->sections[0].id; b = snap.state->sections[1].id;
}

TEST(RoutesEncoded, TwoEncodedStagesAccepted) {
  std::string base = ReadAll(DataFile("FAST.RCD"));
  uint32_t primary1=0, secondary1=0; GetTwoSections(primary1, secondary1);
  // Also pick two more sections if present; else reuse the same pair
  uint32_t primary2 = primary1, secondary2 = secondary1;
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
    auto snap = engine->GetSnapshot(); if (snap.state->sections.size() >= 4u) { primary2 = snap.state->sections[2].id; secondary2 = snap.state->sections[3].id; }
  }
  uint32_t enc1 = secondary1 * 1000 + primary1;
  uint32_t enc2 = secondary2 * 1000 + primary2;
  auto pos = base.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = base.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = base.size();
  std::ostringstream ln; ln << "995, 0, 0, " << enc1 << ", 0, " << enc2 << ", 0, 0, 0\n";
  base.insert(lineEnd+1, ln.str());
  auto tmp = WriteTemp("gtest_ok_routes_two_encoded.rcd", base);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "ok2enc";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(RoutesEncoded, FromToZeroAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "994, 0, 0, 0, 0, 0, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_ok_routes_fromto_zero.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okft0";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

