#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteHint, EmitsRouteIdWhenMatchingRouteExists) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t arrSel = snap.state->timetable.front().arrSelector;

  // Skip test if there is no route whose fromSelector matches this ArrSelector
  bool hasMatchingRoute = false; std::optional<uint32_t> anyRouteId;
  for (const auto& r : snap.state->routes) { if (r.fromSelector == arrSel) { hasMatchingRoute = true; anyRouteId = r.id; break; } }
  if (!hasMatchingRoute) GTEST_SKIP() << "No matching route for ArrSelector; skipping.";

  ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = tt; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());

  // Find routeId hint for this timetable entry and verify it maps to a route whose fromSelector equals arrSelector
  std::optional<uint32_t> hintedRouteId;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("routeId");
      if (it != ed.changedFields.end()) { hintedRouteId = static_cast<uint32_t>(std::stoul(it->second)); break; }
    }
  }
  ASSERT_TRUE(hintedRouteId.has_value());
  bool ok = false;
  for (const auto& r : engine->GetSnapshot().state->routes) { if (r.id == *hintedRouteId) { ok = (r.fromSelector == arrSel); break; } }
  ASSERT_TRUE(ok);
}

