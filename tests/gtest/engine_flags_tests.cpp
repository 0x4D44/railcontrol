#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(EngineLifecycleFlags, SimulationActiveTransitions) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  EXPECT_FALSE(snap.state->simulationActive);
  // Advance makes it running/active
  auto out = engine->Advance(std::chrono::milliseconds{50}); (void)out;
  auto snap2 = engine->GetSnapshot();
  EXPECT_TRUE(snap2.state->simulationActive);
  // Stop clears active
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::Stop, std::monostate{}}).code, StatusCode::Ok);
  auto snap3 = engine->GetSnapshot();
  EXPECT_FALSE(snap3.state->simulationActive);
}

TEST(EngineLifecycleFlags, ResetKeepsLastLayoutId) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  std::string idBefore = engine->GetLayoutId();
  ASSERT_FALSE(idBefore.empty());
  ASSERT_EQ(engine->Reset().code, StatusCode::Ok);
  std::string idAfter = engine->GetLayoutId();
  EXPECT_EQ(idBefore, idAfter);
  auto snap = engine->GetSnapshot();
  EXPECT_TRUE(snap.state->sections.empty());
}
