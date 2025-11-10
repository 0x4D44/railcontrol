#include <gtest/gtest.h>
#include <memory>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(TimetableParsed, ArrSelectorStored) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto snap = engine->GetSnapshot();
  ASSERT_FALSE(snap.state->timetable.empty());
  // FAST.RCD should have ArrSelector for first entry in [1..49]; we only assert it's non-zero to avoid binding to a specific value.
  EXPECT_GT(snap.state->timetable.front().arrSelector, 0u);
  EXPECT_LE(snap.state->timetable.front().arrSelector, 49u);
}

