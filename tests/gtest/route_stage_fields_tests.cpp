#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteStageFields, PresentAndMatchRouteForStageIndex) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();

  // Find a timetable with matching route (fromSelector == ArrSelector)
  uint32_t tt = 0; uint32_t arrSel = 0; std::optional<uint32_t> routeId;
  for (const auto& t : snap.state->timetable) {
    if (t.arrSelector == 0) continue;
    for (const auto& r : snap.state->routes) {
      if (r.fromSelector == t.arrSelector) { tt = t.id; arrSel = t.arrSelector; routeId = r.id; break; }
    }
    if (tt != 0) break;
  }
  if (tt == 0) GTEST_SKIP() << "No timetable with matching route; skipping.";
  ASSERT_FALSE(snap.state->locos.empty());

  // Assign and advance 100ms
  LocoAssignment la{tt, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());

  // Extract fields from delta for this timetable
  int idx=-1; std::optional<uint32_t> hintedRouteId; std::optional<uint32_t> stp, sts;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find("stageIndex"); if (it != ed.changedFields.end()) idx = std::stoi(it->second);
      auto ir = ed.changedFields.find("routeId"); if (ir != ed.changedFields.end()) hintedRouteId = static_cast<uint32_t>(std::stoul(ir->second));
      auto ip = ed.changedFields.find("stagePrimary"); if (ip != ed.changedFields.end()) stp = static_cast<uint32_t>(std::stoul(ip->second));
      auto is = ed.changedFields.find("stageSecondary"); if (is != ed.changedFields.end()) sts = static_cast<uint32_t>(std::stoul(is->second));
      break;
    }
  }
  ASSERT_GE(idx, 0);
  ASSERT_TRUE(hintedRouteId.has_value());
  ASSERT_TRUE(stp.has_value());
  ASSERT_TRUE(sts.has_value());

  // Validate stagePrimary/Secondary match snapshot route stages[idx]
  auto snap2 = engine->GetSnapshot(); bool ok=false;
  for (const auto& r : snap2.state->routes) {
    if (r.id == *hintedRouteId) {
      EXPECT_EQ(r.fromSelector, arrSel);
      EXPECT_EQ(r.stages[idx].primary, *stp);
      EXPECT_EQ(r.stages[idx].secondary, *sts);
      ok = true; break;
    }
  }
  ASSERT_TRUE(ok);
}

TEST(RouteStageFields, NotEmittedOnZeroDtAssignmentDelta) {
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
      ASSERT_TRUE(ed.changedFields.find("stagePrimary") == ed.changedFields.end());
      ASSERT_TRUE(ed.changedFields.find("stageSecondary") == ed.changedFields.end());
    }
  }
}

