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
    if (ed.id == tt) {
      auto it = ed.changedFields.find(key);
      if (it != ed.changedFields.end()) { out = it->second; return true; }
    }
  }
  return false;
}

// After depart has been emitted, a zero-dt advance must not re-emit
// stage/progress fields.
TEST(StageProgress, ZeroDtAfterDepartDoesNotEmitStageOrProgress) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Assign and flush assignment delta
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});

  // Advance beyond depart threshold so we have departed
  auto t1 = engine->Advance(std::chrono::milliseconds{600});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  // Sanity: departed flag present
  bool sawDepart=false; if (t1.result.delta) {
    for (const auto& ed : t1.result.delta->timetableEntries) {
      if (ed.id==tt) { auto it=ed.changedFields.find("departed"); if (it!=ed.changedFields.end() && it->second=="true") { sawDepart=true; break; } }
    }
  }
  ASSERT_TRUE(sawDepart) << "Expected depart before zero-dt check";

  // Zero-dt should not emit stage/progress again
  auto t2 = engine->Advance(std::chrono::milliseconds{0});
  if (t2.result.delta) {
    std::string stage, prog;
    EXPECT_FALSE(FindField(t2.result.delta, tt, "stage", stage));
    EXPECT_FALSE(FindField(t2.result.delta, tt, "progressMs", prog));
  }
}

