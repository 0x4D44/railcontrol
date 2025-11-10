#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Samples, LoadAllSampleLayouts) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  ASSERT_TRUE(engine);

  const char* files[] = { "FAST.RCD", "KINGSX.RCD", "QUEENST.RCD", "WAVERLY.RCD" };
  for (const char* f : files) {
    LayoutDescriptor d; d.sourcePath = DataFile(f); d.name = f;
    Status s = engine->LoadLayout(d);
    EXPECT_EQ(s.code, StatusCode::Ok) << f << ": " << s.message;
  }
}

