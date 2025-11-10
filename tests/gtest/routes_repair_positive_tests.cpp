#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesRepair, RepairsWhitespaceToSixStagesAndAccepts) {
  // Obtain two valid section ids from FAST.RCD
  EngineConfig cfg0; auto repo0 = std::make_shared<RcdLayoutRepository>(); auto engine0 = CreateEngine(cfg0, repo0, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d0; d0.sourcePath = DataFile("FAST.RCD"); d0.name = "FAST";
  ASSERT_EQ(engine0->LoadLayout(d0).code, StatusCode::Ok);
  auto snap = engine0->GetSnapshot(); ASSERT_GE(snap.state->sections.size(), 2u);
  uint32_t s1 = snap.state->sections[0].id; uint32_t s2 = snap.state->sections[1].id;

  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  std::ostringstream ln;
  // After repair, stage area becomes: s1, s2, 0, 0, 0, 0  (six tokens)
  ln << "988, 0, 0, " << s1 << " " << s2 << ", 0, 0, 0, 0\n";
  content.insert(lineEnd+1, ln.str());
  auto tmp = WriteTemp("gtest_ok_routes_repair_positive.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okrepair";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

