#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "test_utils.h"

using namespace RailCore;

static std::string MinimalRcdWithTrailingZeroStages() {
  // Build a minimal valid RCD with one selector, a few sections, one route whose
  // last stages are zeros, one loco, locoyard disabled, and one timetable entry
  // referencing selector 1.
  std::string s;
  s += "[SELECTOR]\n";
  s += "1, 0, 0, 1, 1, 1, 0, UF\n";
  s += "\n[SECTIONS]\n";
  s += "1, 0,0, 10,0, 10,10, 0,10\n";
  s += "2, 20,0, 30,0, 30,10, 20,10\n";
  s += "\n[OVERLAPPING]\n";
  s += "1, 1, 1\n";
  s += "\n[PLATFORMS]\n";
  s += "1, 0,0, 1,0, 1,1, 0,1\n";
  s += "\n[ROUTES]\n";
  // id=1, fromSelector=1, toSelector=1, stages: (1,0),(1,0),(2,0),(0,0),(0,0),(0,0)
  s += "1, 1, 1, 1, 1, 2, 0, 0, 0\n";
  s += "\n[LOCOS]\n";
  s += "1, 0, 0, 1\n";
  s += "\n[LOCOYARD]\n";
  s += "DISABLED\n";
  s += "\n[TIMETABLE]\n";
  // id, Name1, Name2, ArrSelector(=1), ArrTime, col6, DepTime, col8, col9, col10, NextId
  s += "1, A,B, 1, 600, 0, 700, 0, 0, 0, 0\n";
  s += "\n[GENERAL]\n";
  s += "StartTime= 0600\n";
  s += "StopTime= 1200\n";
  return s;
}

TEST(RouteStageFallback, FallsBackToLastNonZero) {
  auto path = WriteTemp("tmp_fallback.rcd", MinimalRcdWithTrailingZeroStages());
  auto repo = std::make_shared<RcdLayoutRepository>();
  EngineConfig cfg; auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = path; d.name = "tmp";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id;
  uint32_t loco = snap.state->locos.front().id;
  LocoAssignment la{tt, loco, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);

  // Advance enough time to reach stageIndex near/max (simulate > 500ms)
  auto out = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta.has_value());
  bool saw = false; uint32_t sp = 0, ss = 0; int idx = -1;
  for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id == tt) {
      auto itI = ed.changedFields.find("stageIndex");
      if (itI != ed.changedFields.end()) idx = std::stoi(itI->second);
      auto itP = ed.changedFields.find("stagePrimary");
      auto itS = ed.changedFields.find("stageSecondary");
      if (itP != ed.changedFields.end() && itS != ed.changedFields.end()) {
        sp = static_cast<uint32_t>(std::stoul(itP->second));
        ss = static_cast<uint32_t>(std::stoul(itS->second));
        saw = true;
      }
    }
  }
  ASSERT_TRUE(saw);
  // Expect fallback to most recent non-zero stage (which is the 3rd stage: primary=2 secondary=0)
  EXPECT_EQ(sp, 2u);
  EXPECT_EQ(ss, 0u);
  EXPECT_GE(idx, 3); // we advanced enough to be beyond the first non-zero stages
}

