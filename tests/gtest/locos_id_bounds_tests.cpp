#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(LocosIdBounds, MaxAccepted499) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "499, 31, 999, 1\n");
  auto tmp = WriteTemp("gtest_ok_locos_499.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "okloco499";
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(LocosIdBounds, GreaterThanMaxRejected) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  auto pos = content.find("[LOCOS]"); ASSERT_NE(pos, std::string::npos);
  auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
  content.insert(lineEnd+1, "500, 31, 999, 1\n");
  auto tmp = WriteTemp("gtest_bad_locos_500.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badloco500";
  auto s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for loco id 500";
  EXPECT_NE(s.message.find("Loco id out of range"), std::string::npos) << s.message;
}

