#include <gtest/gtest.h>
#include "test_utils.h"

#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/types.h"
#include "railcore/observer.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

TEST(DeltaComposite, AssignAndDelayChangeSameTickProduceBothDeltas) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto snap = engine->GetSnapshot(); ASSERT_FALSE(snap.state->timetable.empty()); ASSERT_FALSE(snap.state->locos.empty());
  const uint32_t tt = snap.state->timetable.front().id; const uint32_t loco = snap.state->locos.front().id;

  // Queue both an assignment and delay change before advancing time
  LocoAssignment la; la.timetableId = tt; la.locoId = loco; la.action = AssignmentAction::Assign;
  s = engine->Command(CommandPayload{CommandId::AssignLoco, la}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{2}; ds.maintenanceThrough = false;
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;

  auto out = engine->Advance(std::chrono::milliseconds{0});
  ASSERT_EQ(out.status.code, StatusCode::Ok);
  ASSERT_TRUE(out.result.delta);

  bool sawAssign=false; for (const auto& ed : out.result.delta->timetableEntries) {
    if (ed.id==tt) { auto it=ed.changedFields.find("assignedLocoId"); if (it!=ed.changedFields.end() && it->second==std::to_string(loco)) { sawAssign=true; break; } }
  }
  EXPECT_TRUE(sawAssign);
  EXPECT_FALSE(out.result.delta->globals.empty());
}

namespace {
struct UnsubscribingObserver : IObserver {
  std::shared_ptr<IRailEngine> engine;
  int events{0};
  explicit UnsubscribingObserver(std::shared_ptr<IRailEngine> e) : engine(std::move(e)) {}
  void OnSnapshot(const LayoutSnapshot&) override {}
  void OnEvents(const SimulationTickResult&) override {
    ++events;
    engine->Unsubscribe(*this);
  }
  void OnDiagnostics(const DiagnosticsEvent&) override {}
};
}

TEST(ObserverFlow, UnsubscribeDuringCallbackStopsFurtherCallbacks) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  auto s = engine->LoadLayout(d); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto obs = std::make_unique<UnsubscribingObserver>(engine);
  engine->Subscribe(*obs);
  // Trigger at least one event via a delay change and an advance
  DelaySettings ds; ds.mode = DelayMode::Randomized; ds.threshold = std::chrono::minutes{1};
  s = engine->Command(CommandPayload{CommandId::SetDelayMode, ds}); ASSERT_EQ(s.code, StatusCode::Ok) << s.message;
  auto out = engine->Advance(std::chrono::milliseconds{0}); (void)out;
  const int firstCount = obs->events;
  // Further advances should not invoke observer again
  (void)engine->Advance(std::chrono::milliseconds{100});
  EXPECT_EQ(obs->events, firstCount);
}

