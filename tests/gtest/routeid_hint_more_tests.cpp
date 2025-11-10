#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <unordered_set>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteHint, AbsentWhenNoMatchingRoute) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  std::unordered_set<uint32_t> froms; for (const auto& r : snap.state->routes) froms.insert(r.fromSelector);

  // Find a timetable entry whose ArrSelector has no matching route fromSelector
  uint32_t tt = 0; uint32_t arrSel = 0;
  for (const auto& t : snap.state->timetable) {
    if (t.arrSelector > 0 && froms.find(t.arrSelector) == froms.end()) { tt = t.id; arrSel = t.arrSelector; break; }
  }
  if (tt == 0) GTEST_SKIP() << "No timetable with unmatched ArrSelector; skipping.";
  ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la{tt, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  bool sawRouteId=false;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      if (ed.changedFields.find("routeId") != ed.changedFields.end()) sawRouteId = true;
    }
  }
  ASSERT_FALSE(sawRouteId);
}

TEST(RouteHint, PersistsAcrossTicksForAssignment) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();

  // Select a timetable with a matching route
  uint32_t tt = 0; uint32_t arrSel = 0; std::optional<uint32_t> routeId;
  for (const auto& t : snap.state->timetable) {
    for (const auto& r : snap.state->routes) {
      if (t.arrSelector > 0 && r.fromSelector == t.arrSelector) { tt = t.id; arrSel = t.arrSelector; routeId = r.id; break; }
    }
    if (tt != 0) break;
  }
  if (tt == 0) GTEST_SKIP() << "No timetable with matching route; skipping.";
  ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la{tt, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  // Multiple dt>0 ticks should keep emitting routeId
  for (int i = 0; i < 3; ++i) {
    auto out = engine->Advance(std::chrono::milliseconds{100});
    ASSERT_EQ(out.status.code, StatusCode::Ok);
    ASSERT_TRUE(out.result.delta.has_value());
    bool saw=false;
    for (const auto& ed : out.result.delta->timetableEntries) {
      if (ed.id == tt) {
        auto it = ed.changedFields.find("routeId");
        if (it != ed.changedFields.end()) { saw = true; break; }
      }
    }
    ASSERT_TRUE(saw);
  }
}

TEST(RouteHint, NotEmittedOnZeroDtAssignmentDelta) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la{snap.state->timetable.front().id, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == la.timetableId) {
      ASSERT_TRUE(ed.changedFields.find("assignedLocoId") != ed.changedFields.end());
      ASSERT_TRUE(ed.changedFields.find("routeId") == ed.changedFields.end());
    }
  }
}

