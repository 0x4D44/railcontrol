#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(StageIndex, MultiEntryMonotonicIndependently) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  auto snap = engine->GetSnapshot();
  ASSERT_GE(snap.state->timetable.size(), 2u);
  ASSERT_GE(snap.state->locos.size(), 2u);

  uint32_t tt1 = snap.state->timetable[0].id;
  uint32_t tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id;
  uint32_t l2 = snap.state->locos[1].id;

  LocoAssignment a1{tt1, l1, AssignmentAction::Assign};
  LocoAssignment a2{tt2, l2, AssignmentAction::Assign};
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a1}).code, StatusCode::Ok);
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, a2}).code, StatusCode::Ok);

  std::unordered_map<uint32_t,int> last;
  last[tt1] = -1; last[tt2] = -1;

  for (int i = 0; i < 4; ++i) {
    auto out = engine->Advance(std::chrono::milliseconds{100});
    ASSERT_EQ(out.status.code, StatusCode::Ok);
    ASSERT_TRUE(out.result.delta.has_value());
    for (auto tt : {tt1, tt2}) {
      int idx=-1;
      for (const auto& ed : out.result.delta->timetableEntries) {
        if (ed.id == tt) {
          auto it = ed.changedFields.find("stageIndex");
          if (it != ed.changedFields.end()) { idx = std::stoi(it->second); break; }
        }
      }
      ASSERT_GE(idx, last[tt]);
      last[tt] = idx;
    }
  }
}

