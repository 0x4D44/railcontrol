#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

 

TEST(EngineLifecycle, PauseAdvanceReset) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Pause, std::monostate{}}).code, StatusCode::Ok);
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::Pause, std::monostate{}}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{50});
  EXPECT_EQ(out.status.code, StatusCode::Ok);
  EXPECT_GE(out.result.clock.count(), 0);
  EXPECT_EQ(engine->Reset().code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  EXPECT_TRUE(snap.state->sections.empty());
}

TEST(EngineLifecycle, AdvanceNegativeDtRejected) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{-1});
  EXPECT_EQ(out.status.code, StatusCode::ValidationError);
}

TEST(EngineLifecycle, DelayModeValidation) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  DelaySettings bad; bad.mode = DelayMode::Randomized; bad.threshold = std::chrono::minutes{5000};
  EXPECT_NE(engine->Command(CommandPayload{CommandId::SetDelayMode, bad}).code, StatusCode::Ok);
  DelaySettings ok; ok.mode = DelayMode::MaintenanceOnly; ok.threshold = std::chrono::minutes{2};
  EXPECT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ok}).code, StatusCode::Ok);
}

struct CapturingRandom : IRandomProvider {
  uint32_t lastSeed{0};
  uint32_t Next() override { return 42; }
  void Seed(uint32_t s) override { lastSeed = s; }
};

TEST(EngineLifecycle, DeterministicSeedingWhenEnabled) {
  EngineConfig cfg; cfg.enableDeterministicSeeds = true;
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto rnd = std::make_shared<CapturingRandom>();
  auto engine = CreateEngine(cfg, repo, nullptr, rnd, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  EXPECT_EQ(rnd->lastSeed, 0xC0FFEEu);
}

TEST(EngineLifecycle, DeltaCompositionAssignThenDelay) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  auto s1 = engine->Command(CommandPayload{CommandId::AssignLoco, la});
  EXPECT_EQ(s1.code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  auto s2 = engine->Command(CommandPayload{CommandId::SetDelayMode, ds});
  EXPECT_EQ(s2.code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out.result.delta);
  // Expect assigned loco id and delay globals together
  bool sawAssign=false; for (const auto& ed : out.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (it!=ed.changedFields.end() && it->second==std::to_string(la.locoId)) { sawAssign=true; break; } }
  EXPECT_TRUE(sawAssign);
  auto gm = out.result.delta->globals;
  EXPECT_TRUE(gm.find("delay.mode") != gm.end());
  EXPECT_TRUE(gm.find("delay.threshold_min") != gm.end());
}

TEST(EngineLifecycle, DeltaReassignAcrossTicks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_GE(snap.state->locos.size(), 1u);
  ASSERT_GE(snap.state->timetable.size(), 1u);
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco1 = snap.state->locos.front().id;
  // Assign first
  LocoAssignment la; la.timetableId = tt; la.locoId = loco1; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto out1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out1.result.delta);
  bool sawAssign=false; for (const auto& ed : out1.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (it!=ed.changedFields.end() && it->second==std::to_string(loco1)) { sawAssign=true; break; } }
  EXPECT_TRUE(sawAssign);
  // Release
  la.action = AssignmentAction::Release;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, la}).code, StatusCode::Ok);
  auto out2 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(out2.result.delta);
  bool sawRelease=false; for (const auto& ed : out2.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (ed.id==tt && it!=ed.changedFields.end() && it->second.empty()) { sawRelease=true; break; } }
  EXPECT_TRUE(sawRelease);
  // Reassign different loco if available
  if (snap.state->locos.size() > 1) {
    uint32_t loco2 = snap.state->locos[1].id;
    LocoAssignment la2; la2.timetableId = tt; la2.locoId = loco2; la2.action = AssignmentAction::Assign;
    ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la2}).code, StatusCode::Ok);
    auto out3 = engine->Advance(std::chrono::milliseconds{0});
    ASSERT_TRUE(out3.result.delta);
    bool sawReassign=false; for (const auto& ed : out3.result.delta->timetableEntries) { auto it=ed.changedFields.find("assignedLocoId"); if (ed.id==tt && it!=ed.changedFields.end() && it->second==std::to_string(loco2)) { sawReassign=true; break; } }
    EXPECT_TRUE(sawReassign);
  }
}


TEST(EngineLifecycle, NoSeedingWhenDisabledOrMissingProvider) {
  // Disabled flag
  {
    EngineConfig cfg; cfg.enableDeterministicSeeds = false;
    auto repo = std::make_shared<RcdLayoutRepository>();
    auto rnd = std::make_shared<CapturingRandom>();
    auto engine = CreateEngine(cfg, repo, nullptr, rnd, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
    EXPECT_EQ(rnd->lastSeed, 0u);
  }
  // Missing provider
  {
    EngineConfig cfg; cfg.enableDeterministicSeeds = true;
    auto repo = std::make_shared<RcdLayoutRepository>();
    auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    auto s = engine->LoadLayout(d);
    EXPECT_EQ(s.code, StatusCode::Ok);
  }
}

TEST(EngineLifecycle, ReleaseLocoWrongIdNotFound) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = 0; la.action = AssignmentAction::Release;
  auto s = engine->Command(CommandPayload{CommandId::ReleaseLoco, la});
  EXPECT_EQ(s.code, StatusCode::NotFound);
}

TEST(EngineLifecycle, EngineConfigLimitsGuardLoad) {
  EngineConfig cfg; cfg.maxRoutes = 10; // FAST.RCD has > 10 routes
  auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d);
  EXPECT_NE(s.code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  EXPECT_TRUE(snap.state->sections.empty());
}
