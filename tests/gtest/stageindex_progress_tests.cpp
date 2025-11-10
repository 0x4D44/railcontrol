#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

// Validate that optional stageIndex (0..5) is present on dt>0 and monotonic up to departure threshold.
TEST(StageIndex, MonotonicUntilDepart) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  // Assign first loco to first timetable entry
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  LocoAssignment la; la.timetableId = snap.state->timetable.front().id; la.locoId = snap.state->locos.front().id; la.action = AssignmentAction::Assign;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  int lastIdx = -1;
  // Advance in 100ms slices up to 600ms; expect non-decreasing stageIndex until depart
  for (int i = 0; i < 6; ++i) {
    auto out = engine->Advance(std::chrono::milliseconds{100});
    ASSERT_EQ(out.status.code, StatusCode::Ok);
    ASSERT_TRUE(out.result.delta.has_value());
    bool found=false; int idx=-1;
    for (const auto& ed : out.result.delta->timetableEntries) {
      if (ed.id == la.timetableId) {
        auto it = ed.changedFields.find("stageIndex");
        if (it != ed.changedFields.end()) { idx = std::stoi(it->second); found=true; break; }
      }
    }
    ASSERT_TRUE(found);
    ASSERT_GE(idx, lastIdx);
    lastIdx = idx;
  }
}

