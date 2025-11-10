#include <gtest/gtest.h>
#include <memory>

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "test_utils.h"

using namespace RailCore;

static int GetIntField(const WorldDelta& d, uint32_t id, const char* key, int defVal = -1) {
  for (const auto& ed : d.timetableEntries) {
    if (ed.id == id) {
      auto it = ed.changedFields.find(key);
      if (it != ed.changedFields.end()) return std::stoi(it->second);
    }
  }
  return defVal;
}

TEST(StageBucketElapsed, MonotonicWithinBucketThenWrapsOnAdvance) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  uint32_t tt = snap.state->timetable.front().id; uint32_t loco = snap.state->locos.front().id;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::AssignLoco, LocoAssignment{tt, loco, AssignmentAction::Assign}}).code, StatusCode::Ok);

  // Small advances within a bucket should increase stageBucketElapsedMs
  auto t1 = engine->Advance(std::chrono::milliseconds{20});
  ASSERT_TRUE(t1.result.delta.has_value());
  int b1 = GetIntField(*t1.result.delta, tt, "stageBucketElapsedMs");
  ASSERT_GE(b1, 0);

  auto t2 = engine->Advance(std::chrono::milliseconds{20});
  ASSERT_TRUE(t2.result.delta.has_value());
  int b2 = GetIntField(*t2.result.delta, tt, "stageBucketElapsedMs");
  ASSERT_GE(b2, 0);
  EXPECT_GT(b2, b1);

  // A big advance likely crosses into next bucket; the remainder should wrap and be <= previous remainder
  auto t3 = engine->Advance(std::chrono::milliseconds{100});
  ASSERT_TRUE(t3.result.delta.has_value());
  int b3 = GetIntField(*t3.result.delta, tt, "stageBucketElapsedMs");
  ASSERT_GE(b3, 0);
  EXPECT_LE(b3, b2);
}

