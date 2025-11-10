#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteStageFields, ProgressionUpdatesStageForSameAssignment) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();

  // Find a timetable with matching route
  uint32_t tt = 0; std::optional<uint32_t> routeId;
  for (const auto& t : snap.state->timetable) {
    if (t.arrSelector == 0) continue;
    for (const auto& r : snap.state->routes) {
      if (r.fromSelector == t.arrSelector) { tt = t.id; routeId = r.id; break; }
    }
    if (tt != 0) break;
  }
  if (tt == 0) GTEST_SKIP() << "No timetable with matching route; skipping.";
  ASSERT_FALSE(snap.state->locos.empty());

  LocoAssignment la{tt, snap.state->locos.front().id, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  auto tick1 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(tick1.status.code, StatusCode::Ok);
  ASSERT_TRUE(tick1.result.delta.has_value());
  int idx1=-1; std::optional<uint32_t> p1, s1;
  for (const auto& ed : tick1.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto itI = ed.changedFields.find("stageIndex"); if (itI!=ed.changedFields.end()) idx1 = std::stoi(itI->second);
      auto itP = ed.changedFields.find("stagePrimary"); if (itP!=ed.changedFields.end()) p1 = static_cast<uint32_t>(std::stoul(itP->second));
      auto itS = ed.changedFields.find("stageSecondary"); if (itS!=ed.changedFields.end()) s1 = static_cast<uint32_t>(std::stoul(itS->second));
      break;
    }
  }
  ASSERT_GE(idx1, 0);
  ASSERT_TRUE(p1.has_value());
  ASSERT_TRUE(s1.has_value());

  auto tick2 = engine->Advance(std::chrono::milliseconds{200});
  ASSERT_EQ(tick2.status.code, StatusCode::Ok);
  ASSERT_TRUE(tick2.result.delta.has_value());
  int idx2=-1; std::optional<uint32_t> p2, s2;
  for (const auto& ed : tick2.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto itI = ed.changedFields.find("stageIndex"); if (itI!=ed.changedFields.end()) idx2 = std::stoi(itI->second);
      auto itP = ed.changedFields.find("stagePrimary"); if (itP!=ed.changedFields.end()) p2 = static_cast<uint32_t>(std::stoul(itP->second));
      auto itS = ed.changedFields.find("stageSecondary"); if (itS!=ed.changedFields.end()) s2 = static_cast<uint32_t>(std::stoul(itS->second));
      break;
    }
  }
  ASSERT_GE(idx2, idx1);
  // If index advanced, expect stagePrimary/Secondary to be allowed to change; if not, they may remain same.
  if (idx2 > idx1) {
    // Only assert that at least one of primary/secondary differs or index differs to indicate progression
    EXPECT_TRUE((p2 && s2) && ((*p2 != *p1) || (*s2 != *s1)));
  }
}

