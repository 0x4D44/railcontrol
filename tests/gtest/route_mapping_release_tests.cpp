#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteMapping, ReleaseClearsRouteMappingAndReassignRecomputes) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());

  uint32_t tt = 0; for (const auto& t : snap.state->timetable) { if (t.arrSelector != 0) { tt = t.id; break; } }
  if (tt == 0) GTEST_SKIP() << "No timetable with non-zero ArrSelector; skipping.";
  uint32_t loco = snap.state->locos.front().id;

  // Assign and advance to get routeId/stage fields
  LocoAssignment la{tt, loco, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);

  // Release and advance — no timetable delta for this TT should appear (mapping cleared)
  LocoAssignment rel{tt, loco, AssignmentAction::Release};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::ReleaseLoco, rel}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(t2.status.code, StatusCode::Ok);
  if (t2.result.delta.has_value()) {
    for (const auto& ed : t2.result.delta->timetableEntries) { ASSERT_NE(ed.id, tt) << "Unexpected delta for released TT"; }
  }

  // Reassign and advance — routeId should be emitted again
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto t3 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(t3.status.code, StatusCode::Ok);
  ASSERT_TRUE(t3.result.delta.has_value());
  bool sawRoute=false;
  for (const auto& ed : t3.result.delta->timetableEntries) {
    if (ed.id == tt) { sawRoute = (ed.changedFields.find("routeId") != ed.changedFields.end()); }
  }
  ASSERT_TRUE(sawRoute);
}

