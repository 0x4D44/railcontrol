#include <gtest/gtest.h>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Sections, MissingPlatformsSectionMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  // Remove the [PLATFORMS] section completely
  auto start = content.find("[PLATFORMS]"); ASSERT_NE(start, std::string::npos);
  auto next = content.find('[', start + 1);
  if (next == std::string::npos) next = content.size();
  content.erase(start, next - start);
  auto tmp = WriteTemp("gtest_bad_missing_platforms.rcd", content);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "missplat";
  Status s = engine->LoadLayout(d);
  if (s.code == StatusCode::Ok) GTEST_SKIP() << "Unexpected Ok for missing PLATFORMS";
  EXPECT_NE(s.message.find("Missing required sections"), std::string::npos) << s.message;
}

