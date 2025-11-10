#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteMapping, ReassignKeepsRouteMappingForTimetable) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_GE(snap.state->timetable.size(), 1u);
  ASSERT_GE(snap.state->locos.size(), 2u);

  // Choose a TT with a matching route
  uint32_t tt = 0; for (const auto& t : snap.state->timetable) {
    if (t.arrSelector == 0) continue;
    for (const auto& r : snap.state->routes) { if (r.fromSelector == t.arrSelector) { tt = t.id; break; } }
    if (tt != 0) break;
  }
  if (tt == 0) GTEST_SKIP() << "No timetable with matching route; skipping.";

  // Assign first loco
  uint32_t l1 = snap.state->locos[0].id;
  LocoAssignment la{tt, l1, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_TRUE(t1.result.delta.has_value());
  std::optional<uint32_t> routeId1;
  for (const auto& ed : t1.result.delta->timetableEntries) if (ed.id == tt) { auto it = ed.changedFields.find("routeId"); if (it!=ed.changedFields.end()) routeId1 = static_cast<uint32_t>(std::stoul(it->second)); }
  ASSERT_TRUE(routeId1.has_value());

  // Reassign to second loco and advance; expect same routeId mapping persists
  uint32_t l2 = snap.state->locos[1].id;
  LocoAssignment la2{tt, l2, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la2}).code, StatusCode::Ok);
  auto t2 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_TRUE(t2.result.delta.has_value());
  std::optional<uint32_t> routeId2;
  for (const auto& ed : t2.result.delta->timetableEntries) if (ed.id == tt) { auto it = ed.changedFields.find("routeId"); if (it!=ed.changedFields.end()) routeId2 = static_cast<uint32_t>(std::stoul(it->second)); }
  ASSERT_TRUE(routeId2.has_value());
  EXPECT_EQ(*routeId1, *routeId2);
}

