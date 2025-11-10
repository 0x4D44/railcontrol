#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static bool HasAssigned(const std::shared_ptr<const WorldDelta>& delta, uint32_t tt, uint32_t loco) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("assignedLocoId");
      if (it != ed.changedFields.end() && it->second == std::to_string(loco)) return true;
    }
  }
  return false;
}
static bool HasReleased(const std::shared_ptr<const WorldDelta>& delta, uint32_t tt) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if ( ed.id == tt ) {
      auto it = ed.changedFields.find("assignedLocoId");
      if (it != ed.changedFields.end() && it->second.empty()) return true;
    }
  }
  return false;
}

TEST(DeltaComposite, AssignOneReleaseAnotherAndDelaySameTick) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP() << "Need at least 2 timetable and 2 locos";
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;

  // Pre-assign tt2 so we can release it in the composite tick
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});

  // Now queue: assign tt1, release tt2, and set delay; then advance
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, LocoAssignment{tt2, l2, AssignmentAction::Release}}).code, StatusCode::Ok);
  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{2};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);
  EXPECT_TRUE(HasAssigned(out.result.delta, tt1, l1));
  EXPECT_TRUE(HasReleased(out.result.delta, tt2));
  EXPECT_FALSE(out.result.delta->globals.empty());
}

