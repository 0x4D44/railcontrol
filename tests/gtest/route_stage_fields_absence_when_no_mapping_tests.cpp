#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <unordered_set>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteStageFields, AbsentWhenNoRouteMapping) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  std::unordered_set<uint32_t> froms; for (const auto& r : snap.state->routes) froms.insert(r.fromSelector);

  // Find a timetable whose ArrSelector has no matching route fromSelector
  uint32_t tt = 0; for (const auto& t : snap.state->timetable) { if (t.arrSelector>0 && froms.find(t.arrSelector)==froms.end()) { tt = t.id; break; } }
  if (tt == 0) GTEST_SKIP() << "No unmatched ArrSelector found; skipping.";
  ASSERT_FALSE(snap.state->locos.empty());

  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, snap.state->locos.front().id, AssignmentAction::Assign}}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      ASSERT_TRUE(ed.changedFields.find("routeId") == ed.changedFields.end());
      ASSERT_TRUE(ed.changedFields.find("stagePrimary") == ed.changedFields.end());
      ASSERT_TRUE(ed.changedFields.find("stageSecondary") == ed.changedFields.end());
    }
  }
}

