#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "railcore/persistence/rcd_repository.h"
#include "railcore/engine_factory.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Repository, FallbackToSiblingGameFiles) {
  // Point to repo root FAST.RCD (does not exist), repository should fallback to sibling "Game files/FAST.RCD".
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  auto root = RepoRoot();
  LayoutDescriptor d; d.sourcePath = root / "FAST.RCD"; d.name = "FAST";
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

TEST(Repository, RejectsNonNumericRouteStageTokens) {
  const std::string rcd = R"( [GENERAL]
StartTime=0100
StopTime=0200
[SECTIONS]
1,0,0,0,0
[OVERLAPPING]
1,1,1
[PLATFORMS]
1,1,1,1,1,1,1,1,1
[SELECTOR]
1,1,1,1,1,1,1,1
[ROUTES]
1,1,1,abc,0,0,0,0,0
[LOCOS]
1
[LOCOYARD]
DISABLED
[TIMETABLE]
1,0,0,1,0100,0,0200,0,0,0,0,0
)";

  auto path = WriteTemp("non_numeric_route.rcd", rcd);
  RcdLayoutRepository repo;
  LayoutDescriptor desc; desc.sourcePath = path;
  WorldState ws;
  auto status = repo.Load(desc, ws);
  EXPECT_EQ(status.code, StatusCode::ValidationError);
}

TEST(Repository, RejectsGeneralHoursOutOfRange) {
  const std::string rcd = R"( [GENERAL]
StartTime=2500
StopTime=2600
[SECTIONS]
1,0,0,0,0
[OVERLAPPING]
1,1,1
[PLATFORMS]
1,1,1,1,1,1,1,1,1
[SELECTOR]
1,1,1,1,1,1,1,1
[ROUTES]
1,1,1,100,0,0,0,0,0
[LOCOS]
1
[LOCOYARD]
DISABLED
[TIMETABLE]
1,0,0,1,0100,0,0200,0,0,0,0,0
)";

  auto path = WriteTemp("bad_hours.rcd", rcd);
  RcdLayoutRepository repo;
  LayoutDescriptor desc; desc.sourcePath = path;
  WorldState ws;
  auto status = repo.Load(desc, ws);
  EXPECT_EQ(status.code, StatusCode::ValidationError);
}
