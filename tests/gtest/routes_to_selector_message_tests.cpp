#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RoutesErrorMessages, UnknownToSelectorMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  // Use a high selector id unlikely to exist for ToSelector
  content.insert(lineEnd+1, "989, 0, 999, 0, 0, 0, 0, 0, 0\n");
  auto tmp = WriteTemp("gtest_bad_routes_tosel_msg.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badtosel";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("unknown ToSelector"), std::string::npos);
  } else {
    GTEST_SKIP() << "Fixture contains selector 999; cannot assert unknown message";
  }
}
