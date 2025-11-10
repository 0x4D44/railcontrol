#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesRepair, ThreeEncodedTokensAfterWhitespaceAccepted) {
  // Fetch two sections; reuse encoded tokens as needed
  EngineConfig cfg0; auto repo0 = std::make_shared<RcdLayoutRepository>(); auto engine0 = CreateEngine(cfg0, repo0, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d0; d0.sourcePath = DataFile("FAST.RCD"); d0.name = "FAST";
  ASSERT_EQ(engine0->LoadLayout(d0).code, StatusCode::Ok);
  auto snap = engine0->GetSnapshot(); ASSERT_GE(snap.state->sections.size(), 2u);
  uint32_t pri = snap.state->sections[0].id; uint32_t sec = snap.state->sections[1].id;
  uint32_t enc = sec * 1000 + pri;

  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // After repair, tokens: enc, 0, enc, 0, enc, 0
  std::ostringstream ln; ln << "986, 0, 0, " << enc << ", 0, " << enc << ", 0, " << enc << ", 0\n";
  content.insert(lineEnd+1, ln.str());
  auto tmp = WriteTemp("gtest_ok_routes_repair_three_encoded.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okre3";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

