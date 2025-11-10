#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Id, DifferentLayoutsProduceDifferentIds_GTest) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d1; d1.sourcePath = DataFile("FAST.RCD"); d1.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d1).code, StatusCode::Ok);
  std::string id1 = engine->GetLayoutId();
  LayoutDescriptor d2; d2.sourcePath = DataFile("KINGSX.RCD"); d2.name = "KINGSX";
  ASSERT_EQ(engine->LoadLayout(d2).code, StatusCode::Ok);
  std::string id2 = engine->GetLayoutId();
  ASSERT_FALSE(id1.empty()); ASSERT_FALSE(id2.empty());
  EXPECT_NE(id1, id2);
}
