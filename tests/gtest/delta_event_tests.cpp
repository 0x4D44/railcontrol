#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static bool HasEvent(const std::vector<DomainEvent>& evs, DomainEventId id) {
  for (const auto& e : evs) if (e.id == id) return true; return false;
}

TEST(DeltaEvents, AssignEmitsDeltaAndEvent) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  bool sawAssign=false; for (const auto& ed : out.result.delta->timetableEntries) { if (ed.id==tt) { auto it=ed.changedFields.find("assignedLocoId"); if (it!=ed.changedFields.end() && it->second==std::to_string(loco)) { sawAssign=true; break; } } }
  EXPECT_TRUE(sawAssign);
  EXPECT_TRUE(HasEvent(out.result.events, DomainEventId::LocoAssigned));
}

TEST(DeltaEvents, DelayChangeEmitsGlobalDeltaAndEvent) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{5}; ds.maintenanceThrough = true;
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  auto itM = out.result.delta->globals.find("delay.mode");
  auto itT = out.result.delta->globals.find("delay.thresholdMinutes");
  auto itX = out.result.delta->globals.find("delay.maintenanceThrough");
  EXPECT_TRUE(itM != out.result.delta->globals.end());
  EXPECT_TRUE(itT != out.result.delta->globals.end());
  EXPECT_TRUE(itX != out.result.delta->globals.end());
  EXPECT_TRUE(HasEvent(out.result.events, DomainEventId::DelayChanged));
}

TEST(DeltaEvents, ArrivalAndDepartureFlagsAndEvents) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  (void)engine->Advance(std::chrono::milliseconds{0}); // consume assign delta
  // Advance long enough to cross both arrival and departure thresholds in one tick
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  bool sawArr=false, sawDep=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id==tt) {
      auto ia = ed.changedFields.find("arrived"); if (ia!=ed.changedFields.end() && ia->second=="true") sawArr=true;
      auto idp= ed.changedFields.find("departed"); if (idp!=ed.changedFields.end() && idp->second=="true") sawDep=true;
    }
  }
  EXPECT_TRUE(sawArr);
  EXPECT_TRUE(sawDep);
  EXPECT_TRUE(HasEvent(out.result.events, DomainEventId::TrainArrived));
  EXPECT_TRUE(HasEvent(out.result.events, DomainEventId::TrainDeparted));
}

