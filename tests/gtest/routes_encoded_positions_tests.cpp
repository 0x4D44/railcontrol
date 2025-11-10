#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

static void GetTwoSections(uint32_t& primary, uint32_t& secondary) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_GE(snap.state->sections.size(), 2u);
  primary = snap.state->sections[0].id; secondary = snap.state->sections[1].id;
}

TEST(RoutesEncoded, EncodedAtFirstAndLastStagePositionsAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  uint32_t pri=0, sec=0; GetTwoSections(pri, sec);
  uint32_t enc = sec * 1000 + pri;
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Encoded at first stage (token 4) and last stage (token 9), others zero
  std::ostringstream ln; ln << "990, 0, 0, " << enc << ", 0, 0, 0, 0, " << enc << "\n";
  content.insert(lineEnd+1, ln.str());
  auto tmp = WriteTemp("gtest_ok_routes_encoded_positions.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okpos";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

