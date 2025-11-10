2025-10-21 â€” SHA-256 IDs, build hardening, and tests

- Core IDs
  - Switched layout ID computation from 64-bit FNV-1a to SHA-256 using Windows CNG (BCrypt).
    - Code: src/railcore/persistence/rcd_id.cpp; header updated at include/railcore/persistence/rcd_id.h.
    - Output format is 64 lowercase hex chars (schemaTag + "\n" + canonical content).
    - Updated comment in src/railcore/persistence/rcd_repository.cpp to reflect SHA-256.
  - Linking: added Bcrypt.lib to RailControl, RailCoreTests, and RailCoreGTest project link settings.

- Build reliability
  - Introduced Directory.Build.props to disable NuGet asset resolution for native projects without PackageReferences (avoids intermittent Microsoft.NuGet.targets "Sequence contains no elements").
    - Scoped to all projects except RailCoreGTest to allow GoogleTest NuGet in CI.

- Tests
  - Adjusted smoke test to accept 64-char IDs (tests/railcore/smoke_main.cpp).
  - Fixed forward declaration for TestAssignThenReleaseSameTick to resolve use-before-definition.
  - Added more GoogleTest coverage:
    - Observer: SnapshotOnSubscribe delivers an initial snapshot on subscribe (tests/gtest/observer_tests.cpp).
    - Parser: OVERLAPPING unknown section rejection (tests/gtest/parser_tests.cpp).

- Validation
  - Verified local builds via build.bat Debug: RailControl app, RailCore lib, and RailCoreTests all build and run.
  - Ran tools/build_tests_only.cmd Debug to confirm console tests pass cleanly.

- Next up
  - Continue migrating smoke scenarios to GoogleTest suites.
  - Extend parser boundary tests and world-delta ordering tests.
  - Optional: merge console + gtest coverage reports and make gtests fatal in CI after stability.

- CI/coverage
  - Added soft coverage gate step in CI to parse `coverage.xml` and warn when line-rate for `src/railcore` falls below 80%.
    - Files: .github/workflows/msvc.yml (new step), tools/coverage_gate.ps1 (new script).
  - Added docs/coverage/baseline.md with instructions and initial baseline note.

- Added ROUTES error message tests (token repair still invalid), and multi-tick delta tests covering assign ? delay ? release across ticks.

- Added composite delta test: two assigns + one release + delay in a single tick; verified order-agnostic presence in WorldDelta.

- GTest build stability
  - Added `nuget.config` pinned to nuget.org to stabilize PackageReference restore for GoogleTest on local machines.
  - Updated `build/msvc/RailCoreGTest.vcxproj` with a `PackageReference` to `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn` (1.10.0.3).
 - README updated to document `BUILD_GTEST=1` usage and that NuGet access is required locally.

- More GoogleTests
  - Positive parser cases:
    - GENERAL StopTime at 23:59 accepted.
    - TIMETABLE with minute 59 accepted via `ok_tt_2359.rcd`.
    - ROUTES with encoded secondary section id accepted (constructed using existing section ids from FAST.RCD).
  - Files: `tests/gtest/parser_positive_tests.cpp`, project includes updated.

- Scheduling slice
  - Implemented minimal arrival (200ms) and depart (500ms) thresholds with corresponding DomainEvents and WorldDelta flags (`arrived`, `departed`).
  - Tests: `tests/gtest/scheduling_tests.cpp` covers arrival, depart, suppression after release, no duplicates, and multi-assignment behavior.
  - Added reassign scheduling test: `tests/gtest/scheduling_reassign_tests.cpp`.

- Test gating & CI
  - `build.cmd` now honors `STRICT=1` to fail the build when tests fail locally.
  - CI workflow updated to make GTest build fatal (removed continue-on-error/try-catch).

- More delta/selector/route tests
  - Delta clears next tick and combined scheduling+globals in the same tick.
  - Route selectors cross-check (positive; asserts message if fixture lacks those selectors).

- Scheduling tests: Added reassign reset and no-duplicate-event cases; added selector-cross route test.

- Parser tests: Added PLATFORMS large/negative coords, SELECTOR field position checks, ROUTES primary-only encoded case.
  - Also added duplicate platform/route id rejections and mixed encoded+zero stage positive case.

- Scheduling tests: Added departed non-reemit and interleaving arrivals/depatures across entries.

- Coverage: Raised soft merged and single coverage gates to 85% in CI.

- Coverage: Merged gate now supports -Hard; CI merged gate is hard at 85%. Added selector/routes/platforms edge tests and non-reemit departed tests.

- Coverage gates: Single-report (console) gate now hard at 85%; merged gate already hard. Added selector field7 numeric test.

- Added delay mode bounds tests, general minutes error message test, and scheduling release-after-arrival suppression test.

- Added LOCOYARD invalid code/offset tests and delay mode/general message tests.

- Added message-specific tests for GENERAL/PLATFORMS/SELECTOR/TIMETABLE errors (minutes out of range, expected tokens, too few fields, ArrSelector and NextEntry messages).

- Added repository/selector/platforms/overlapping message-specific tests; broadened error-message coverage.

- CI: console smoke now runs in minimal mode (RAILCORE_SMOKE_MODE=minimal) to reduce runtime; coverage runs remain full.

- Coverage: Raised hard gates (single + merged) to 88%.

- Coverage: Raised hard gates (single + merged) to 90% after expanding tests.

2025-10-21 â€” Smoke test outdir routing

- Added environment-variable-controlled outdir for console smoke temporary files.
  - If `RAILCORE_TEST_OUTDIR` is set, the smoke binary changes its working directory to that path at startup; otherwise it uses the executable directory. This keeps ad hoc runs from polluting the repo root.
  - The build script already runs tests from `build/msvc/<Config>`, so CI/local runs were clean; this change also covers manual invocations.
  - Docs updated (README) to mention `RAILCORE_TEST_OUTDIR`.

2025-10-21 â€” Test working-directory fix, HLD/doc updates

- Test execution working dir
  - Updated `build.cmd` to `pushd` into the output directory before running `RailCoreTests.exe` and `RailCoreGTest.exe`. This keeps any temporary files created by tests out of the repo root and inside `build/msvc/<Config>/`.
  - Updated `tools/coverage.cmd` to switch the working directory to the test binaryâ€™s folder before invoking OpenCppCoverage, ensuring coverage runs also write artifacts in the output directory.

- HLD alignment
  - Edited `docs/2025.10.17 - ui-backend split hld.md` to replace references to `build_msvc.bat` with `build.bat`/`build.cmd`, aligning with the actual MSVC toolchain and scripts.

- README cleanup (pending)
  - Noted an encoding/formatting glitch in the â€œTests & Coverageâ€ section (literal `\n` sequences). Will clean up in a focused doc pass to avoid noisy diffs elsewhere.
