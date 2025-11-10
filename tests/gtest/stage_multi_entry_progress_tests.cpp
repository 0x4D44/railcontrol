#include <gtest/gtest.h>
#include <string>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static bool FindField(const std::shared_ptr<const WorldDelta>& delta, uint32_t tt, const char* key, std::string& out) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if ( ed.id == tt ) {
      auto it = ed.changedFields.find(key);
      if (it != ed.changedFields.end()) { out = it->second; return true; }
    }
  }
  return false;
}

TEST(StageProgressMulti, IndependentProgressMonotonic) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); if (snap.state->timetable.size() < 2 || snap.state->locos.size() < 2) GTEST_SKIP();
  uint32_t tt1 = snap.state->timetable[0].id, tt2 = snap.state->timetable[1].id;
  uint32_t l1 = snap.state->locos[0].id, l2 = snap.state->locos[1].id;

  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt1, l1, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{50}); // start tt1 earlier
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt2, l2, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0}); // flush assign

  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  std::string p1a, p2a; ASSERT_TRUE(FindField(t1.result.delta, tt1, "progressMs", p1a)); ASSERT_TRUE(FindField(t1.result.delta, tt2, "progressMs", p2a));
  long long tt1p1 = std::stoll(p1a), tt2p1 = std::stoll(p2a);

  auto t2 = engine->Advance(std::chrono::milliseconds{120});
  std::string p1b, p2b; ASSERT_TRUE(FindField(t2.result.delta, tt1, "progressMs", p1b)); ASSERT_TRUE(FindField(t2.result.delta, tt2, "progressMs", p2b));
  long long tt1p2 = std::stoll(p1b), tt2p2 = std::stoll(p2b);

  EXPECT_GE(tt1p2, tt1p1);
  EXPECT_GE(tt2p2, tt2p1);
}

