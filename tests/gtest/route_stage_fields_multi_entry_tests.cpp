#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <vector>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteStageFields, PresentForMultipleAssignments) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();

  // Collect up to two timetable entries that have matching routes
  std::vector<uint32_t> tts;
  for (const auto& t : snap.state->timetable) {
    if (t.arrSelector == 0) continue;
    bool matched=false; for (const auto& r : snap.state->routes) if (r.fromSelector == t.arrSelector) { matched=true; break; }
    if (matched) tts.push_back(t.id);
    if (tts.size() == 2) break;
  }
  if (tts.size() < 2) GTEST_SKIP() << "Fewer than two timetable entries with matching routes; skipping.";
  ASSERT_GE(snap.state->locos.size(), 2u);

  // Assign two entries
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tts[0], snap.state->locos[0].id, AssignmentAction::Assign}}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tts[1], snap.state->locos[1].id, AssignmentAction::Assign}}).code, StatusCode::Ok);

  auto out = engine->Advance(std::chrono::milliseconds{150});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());

  int found=0;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tts[0] || ed.id == tts[1]) {
      auto itR = ed.changedFields.find("routeId");
      auto itP = ed.changedFields.find("stagePrimary");
      auto itS = ed.changedFields.find("stageSecondary");
      auto itI = ed.changedFields.find("stageIndex");
      if (itR != ed.changedFields.end() && itP != ed.changedFields.end() && itS != ed.changedFields.end() && itI != ed.changedFields.end()) {
        ++found;
      }
    }
  }
  EXPECT_EQ(found, 2);
}

