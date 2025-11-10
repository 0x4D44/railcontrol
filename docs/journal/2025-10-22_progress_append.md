2025-10-22 — Progress Update

- Build state
  - Verified MSVC Debug build via `build.bat Debug`.
  - Targets green: `RailControl` (UI), `RailCore` (lib), `RailCoreTests` (console smoke).
  - Optional `RailCoreGTest` restore/build: fixed project XML corruption; local NuGet restore still fails with “Sequence contains no elements”. CI remains the source of truth for gtests.

- Fixes
  - Repaired `build/msvc/RailCoreGTest.vcxproj` malformed tail ItemGroup (stray literal `` `r`n `` content). Two test files are now declared correctly and the project closes cleanly.
  - Added a clear banner to console smoke when running in minimal mode so it’s obvious that only two checks run by default.

- What changed
  - File: `build/msvc/RailCoreGTest.vcxproj` — cleaned the final `<ItemGroup>` to include:
    - `tests/gtest/routes_repair_unknown_secondary_message_tests.cpp`
    - `tests/gtest/routes_repair_unknown_primary_message_tests.cpp`
  - File: `tests/railcore/smoke_main.cpp` — print a banner in minimal mode to direct devs to `RailCoreGTest` or CI coverage artifacts.

- Notes
  - Local GTest restore flakiness is expected on some machines; CI builds and runs them, enforcing coverage gates. Local workaround: rely on console smoke or run CI.

- Next
  - Continue migrating any remaining valuable console assertions into GoogleTests and prune disabled legacy console sections.
  - Expand scheduling tests alongside route/section-aware progression work.
  - (Optional) Investigate a local NuGet pin (TFM/package versions) to eliminate the “Sequence contains no elements” restore error.

- Added parser tests: selector field1/2/5 and min tokens; routes token count checks; ID canonicalization ignores trailing blank lines.
- Added ID tests: trailing spaces per line stable; 64-char lowercase hex format asserted.
- Added ROUTES from/to selector tests: zero accepted, unknown ids rejected.
- Added delta test asserting standardized delay globals keys in WorldDelta.
- Added ROUTES ambiguous whitespace repair tests: too-many/too-few stage tokens after repair must error with message.
- Fixed commands test to use standardized key delay.thresholdMinutes.
- Migrated legacy smoke cases to GTest: duplicate TIMETABLE id allowed; Release with wrong loco id returns NotFound.
- Added DelayChanged event payload test: validates mode/thresholdMinutes/maintenanceThrough.
- Added ROUTES encoded storage test: verifies primary/secondary decoded into Route::stages[].
- Added ROUTES multi-entry storage test: verifies independent from/to and stage arrays across multiple routes.
- Added StageIndex tests: monotonic growth across ticks; no delta on zero-dt when no changes.
- Added StageIndex final value test: reaches max bucket by 600ms (arrival+departure thresholds).
- Added composite delta test: assignment + delay on dt>0 yields timetable delta with stageIndex and globals keys.
- Added StageIndex tests: zero-dt assignment delta excludes stageIndex; multi-entry monotonicity validated.
- Added routeId hint test: when a route exists with fromSelector==ArrSelector, dt>0 tick includes routeId matching such a route.
- Added route stage fields in delta (stagePrimary/Secondary) when routeId hint present; added tests for presence and matching to snapshot route stages.
- Engine: route mapping determinism improved (smallest matching routeId). Added test to verify selection when multiple matching routes are injected.
- Added progression test: stagePrimary/Secondary reflect stageIndex progression across ticks for mapped route.
- Added route mapping tests: reassign preserves mapping; release clears mapping and reassign recomputes.
- Added multi-entry route stage field presence test and absence test when no mapping exists.
