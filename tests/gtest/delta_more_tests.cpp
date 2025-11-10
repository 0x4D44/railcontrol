#include <gtest/gtest.h>
#include <filesystem>
#include <map>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Delta, ClearsOnNextTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  // Make a change that will produce a globals delta
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{1};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_TRUE(t1.result.delta);
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  EXPECT_FALSE(t2.result.delta);
}

TEST(Delta, SchedulingAndGlobalsInSameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  // Queue delay change; then advance to exceed arrival threshold so both show up in same tick
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_TRUE(out.result.delta);
  // Verify globals present
  auto& g = out.result.delta->globals;
  EXPECT_EQ(g["delay.mode"], std::string("Randomized"));
  // Verify timetable arrived flag exists for the assigned timetable id
  bool sawArr=false; for (const auto& ed : out.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("arrived"); if (it!=ed.changedFields.end() && it->second=="true") { sawArr=true; break; } } }
  EXPECT_TRUE(sawArr);
}

