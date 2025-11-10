#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesEncoded, PrimaryOnlyEncodedAccepted) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  // Choose an existing section id as primary; secondary=0 encoded results in the same primary value (<1000)
  EngineConfig cfg0; auto repo0 = std::make_shared<RcdLayoutRepository>(); auto engine0 = CreateEngine(cfg0, repo0, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d0; d0.sourcePath = DataFile("FAST.RCD"); d0.name = "FAST";
  ASSERT_EQ(engine0->LoadLayout(d0).code, StatusCode::Ok);
  auto snap = engine0->GetSnapshot(); ASSERT_FALSE(snap.state->sections.empty());
  uint32_t primary = snap.state->sections.front().id;
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  std::ostringstream ln; ln << "992, 0, 0, " << primary << ", 0, 0, 0, 0, 0\n";
  content.insert(lineEnd+1, ln.str());
  auto tmp = WriteTemp("gtest_ok_routes_primary_only.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okprim";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

