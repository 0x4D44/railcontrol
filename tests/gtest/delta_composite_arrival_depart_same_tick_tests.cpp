#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static bool FindField(const std::shared_ptr<const WorldDelta>& delta, uint32_t tt, const char* key) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if (ed.id == tt) {
      if (ed.changedFields.find(key) != ed.changedFields.end()) return true;
    }
  }
  return false;
}

TEST(DeltaComposite, AssignDelayAndArriveDepartInOneTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Queue assignment and delay; then advance with a large dt so arrival+depart happen in same tick
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  // Expect globals delta (delay.*), assignment reflected earlier in state or as delta, and arrival/depart flags
  EXPECT_FALSE(out.result.delta->globals.empty());
  // Assignment may have been emitted as part of this tick if queued before first advance
  // Check that arrived and departed flags are present for the same tt
  EXPECT_TRUE(FindField(out.result.delta, tt, "arrived"));
  EXPECT_TRUE(FindField(out.result.delta, tt, "departed"));
}

