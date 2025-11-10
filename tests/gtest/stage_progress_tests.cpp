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

TEST(StageProgress, EmitsStageAndProgressAcrossTicks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;
  // Assign and flush assignment delta
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});

  // 100ms -> stage 0, progress ~100
  auto t1 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_EQ(t1.status.code, StatusCode::Ok);
  std::string stage, prog;
  ASSERT_TRUE(FindField(t1.result.delta, tt, "stage", stage));
  EXPECT_EQ(stage, "0");
  ASSERT_TRUE(FindField(t1.result.delta, tt, "progressMs", prog));
  EXPECT_GE(std::stoll(prog), 100LL);

  // +150ms -> >=250ms triggers arrival; stage 1 and arrived flag are emitted
  auto t2 = engine->Advance(std::chrono::milliseconds{150});
  ASSERT_EQ(t2.status.code, StatusCode::Ok);
  std::string stage2; ASSERT_TRUE(FindField(t2.result.delta, tt, "stage", stage2)); EXPECT_EQ(stage2, "1");
  std::string arrived; ASSERT_TRUE(FindField(t2.result.delta, tt, "arrived", arrived)); EXPECT_EQ(arrived, "true");

  // +250ms -> >=500ms triggers depart; stage 2 and departed flag are emitted
  auto t3 = engine->Advance(std::chrono::milliseconds{250});
  ASSERT_EQ(t3.status.code, StatusCode::Ok);
  std::string stage3; ASSERT_TRUE(FindField(t3.result.delta, tt, "stage", stage3)); EXPECT_EQ(stage3, "2");
  std::string departed; ASSERT_TRUE(FindField(t3.result.delta, tt, "departed", departed)); EXPECT_EQ(departed, "true");
}

TEST(StageProgress, ZeroDtDoesNotEmitStageOrProgress) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);
  (void)engine->Advance(std::chrono::milliseconds{0});
  auto t1 = engine->Advance(std::chrono::milliseconds{0});
  if (t1.result.delta) {
    std::string stage, prog;
    EXPECT_FALSE(FindField(t1.result.delta, tt, "stage", stage));
    EXPECT_FALSE(FindField(t1.result.delta, tt, "progressMs", prog));
  }
}

