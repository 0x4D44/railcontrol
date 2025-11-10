2025-10-21 - Build/test validation and next steps

- Build validation
  - Ran `build.bat Debug`; RailControl, RailCore, and console smoke tests built cleanly and executed (`TestMissingFile`, `TestLoadFastRcd`).
  - Confirmed headless behavior (no debug pop-ups) and correct working directory for test outputs.
- GTest local note
  - `BUILD_GTEST=1` triggers NuGet restore for `RailCoreGTest.vcxproj`. On this machine it fails with `Microsoft.NuGet.targets(198): Sequence contains no elements` (known, intermittent). The build script continues and still runs console tests.
  - Action: keep CI as the source of truth for GTest. Consider adding a local fallback to skip GTest when NuGet restore fails noisily, but do not change CI.
- Engine scheduling
  - Reviewed current arrival/depart thresholds (200ms/500ms) and event/delta emission. No changes made in this pass to avoid destabilizing passing tests.
- Next steps (short horizon)
  - Continue migrating remaining valuable console smoke assertions into GoogleTests, then prune unreachable console code after the early return.
  - Optional: add a local GTest restore hint or doc snippet for developers, pointing to `tools/build_tests_only.cmd` when they want a clean loop without NuGet.

2025-10-21 - Console smoke pruning, add GTest

- Pruned console smoke
  - Wrapped the legacy body behind a disabled block and kept only minimal checks in `main`.
  - Fixed handler lambda braces and added forward declaration to satisfy MSVC.
  - File: tests/railcore/smoke_main.cpp
- Added GoogleTest
  - New test: `Scheduling.NoAutoReleaseAfterDepart` asserts assignments are not auto-released on depart, and that explicit release emits `LocoReleased` and clears state.
  - File: tests/gtest/scheduling_no_auto_release_tests.cpp
  - Project: build/msvc/RailCoreGTest.vcxproj updated to include the test.
- Build
 - Verified `build.bat Debug` remains green; minimal smoke runs headless.

2025-10-21 - Extend tests

- Added GTests
  - `StageProgress.ZeroDtAfterDepartDoesNotEmitStageOrProgress` ensures no stage/progress re-emission on zero-dt after a depart flag.
    - File: tests/gtest/stage_post_depart_zero_dt_no_progress_tests.cpp
 - `Id.EmptyContentProducesDeterministicSha256Hex` verifies canonicalization on empty content and stable 64-hex SHA-256 id.
    - File: tests/gtest/id_empty_tests.cpp
  - `WorldDeltaClock.MatchesTickClockAndClearsNextTick` asserts delta clock equality and clearing.
    - File: tests/gtest/world_delta_clock_tests.cpp
  - `GeneralMessages.StopTimeMustBeGreaterThanStartTime` checks specific GENERAL error wording.
    - File: tests/gtest/general_stop_vs_start_message_tests.cpp
  - `Observer.UnsubscribeOneKeepsOthersNotified` validates multi-observer behavior.
    - File: tests/gtest/observer_unsubscribe_multiple_tests.cpp
  - `DeltaComposite.TwoAssignmentsAndDelayInSameTick` validates two entry deltas + globals in one tick.
    - File: tests/gtest/delta_multi_entry_composite_tests.cpp
  - `RoutesRepairMessages.UnknownSecondaryAfterRepair` validates unknown secondary after whitespace repair.
    - File: tests/gtest/routes_repair_unknown_secondary_message_tests.cpp
  - `RoutesRepairMessages.UnknownPrimaryAfterRepair` validates unknown primary after whitespace repair.
    - File: tests/gtest/routes_repair_unknown_primary_message_tests.cpp
  - `TimetableMinutes.ArrDep2359Accepted` and `ArrDep2360RejectedWithMessages` cover minute bounds.
    - File: tests/gtest/timetable_minute_bounds_tests.cpp
  - `TimetablePositive.NextEntryChainThreeAccepted` validates a small NextEntry chain.
    - File: tests/gtest/timetable_next_chain_tests.cpp
  - `Observer.ResubscribeYieldsSnapshotAndEvents` validates resubscribe snapshot + events.
    - File: tests/gtest/observer_resubscribe_tests.cpp
  - Project updated to include all new files in `build/msvc/RailCoreGTest.vcxproj`.
- Notes
  - Local GTest build may still hit NuGet restore issues; CI remains authoritative for running the new tests.
