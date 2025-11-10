#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/services.h"

using namespace RailCore;

namespace {

struct TestRandomProvider : IRandomProvider {
  uint32_t lastSeed {0};
  uint32_t Next() override { return 4; }
  void Seed(uint32_t seed) override { lastSeed = seed; }
};

} // namespace

TEST(EngineSeedTests, DeterministicSeedOnLoadWhenEnabled) {
  EngineConfig cfg; cfg.enableDeterministicSeeds = true;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto rnd  = std::make_shared<TestRandomProvider>();
  auto eng  = CreateEngine(cfg, repo, nullptr, rnd, nullptr, nullptr);

  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = eng->LoadLayout(d);
  ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  EXPECT_EQ(rnd->lastSeed, 0xC0FFEEu);
}

