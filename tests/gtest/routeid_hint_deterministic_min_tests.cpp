#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <unordered_set>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(RouteHint, ChoosesSmallestMatchingRouteId) {
  // Load layout and find a timetable ArrSelector that currently has no matching route
  {
    EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
    ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
    auto snap = engine->GetSnapshot();
    std::unordered_set<uint32_t> froms; for (const auto& r : snap.state->routes) froms.insert(r.fromSelector);
    uint32_t tt = 0; uint32_t arrSel = 0;
    for (const auto& t : snap.state->timetable) {
      if (t.arrSelector > 0 && froms.find(t.arrSelector) == froms.end()) { tt = t.id; arrSel = t.arrSelector; break; }
    }
    if (tt == 0) GTEST_SKIP() << "No unmatched ArrSelector found in sample layout; skipping deterministic test.";

    // Build a modified layout with two routes having fromSelector==arrSel: choose min id
    std::string content = ReadAll(DataFile("FAST.RCD"));
    auto pos = content.find("[ROUTES]"); ASSERT_NE(pos, std::string::npos);
    auto lineEnd = content.find('\n', pos); if (lineEnd == std::string::npos) lineEnd = content.size();
    std::ostringstream r1; r1 << "49998, " << arrSel << ", 0, 0, 0, 0, 0, 0, 0\n";
    std::ostringstream r2; r2 << "49999, " << arrSel << ", 0, 0, 0, 0, 0, 0, 0\n";
    content.insert(lineEnd+1, r2.str()); // insert higher id first
    content.insert(lineEnd+1, r1.str()); // then lower id so both present
    auto tmp = WriteTemp("gtest_routes_deterministic_min.rcd", content);

    EngineConfig cfg2; auto repo2 = std::make_shared<RcdLayoutRepository>(); auto engine2 = CreateEngine(cfg2, repo2, nullptr, nullptr, nullptr, nullptr);
    LayoutDescriptor d2; d2.sourcePath = tmp; d2.name = "detmin";
    ASSERT_EQ(engine2->LoadLayout(d2).code, StatusCode::Ok);
    auto snap2 = engine2->GetSnapshot(); ASSERT_FALSE(snap2.state->locos.empty());
    LocoAssignment la{tt, snap2.state->locos.front().id, AssignmentAction::Assign};
    ASSERT_EQ(engine2->Command(CommandPayload{CommandId::AssignLoco, la}).code, StatusCode::Ok);
    auto out = engine2->Advance(std::chrono::milliseconds{100});
    ASSERT_EQ(out.status.code, StatusCode::Ok);
    ASSERT_TRUE(out.result.delta.has_value());
    std::optional<uint32_t> hinted;
    for (const auto& ed : out.result.delta->timetableEntries) {
      if (ed.id == tt) {
        auto it = ed.changedFields.find("routeId");
        if (it != ed.changedFields.end()) hinted = static_cast<uint32_t>(std::stoul(it->second));
      }
    }
    ASSERT_TRUE(hinted.has_value());
    EXPECT_EQ(*hinted, 49998u);
  }
}

