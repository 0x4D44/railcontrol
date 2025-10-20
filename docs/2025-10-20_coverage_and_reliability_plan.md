# RailCore Reliability + Code Coverage Plan (2025-10-20)

This plan fixes current test stability problems (reentrancy/abort dialog), introduces coverage tooling, migrates to GoogleTest, and adds targeted tests to raise and sustain coverage for `src/railcore`.

## Stage 0 — Immediate Stabilization

- Suppress Windows debug pop‑ups in the test harness so failures print to stderr and return non‑zero exit codes.
- Keep lightweight progress prints in the current smoke harness to pinpoint failures while we harden runtime.

## Stage 1 — Reentrancy Fix (root cause)

- Problem: `EngineStub::Advance` holds the mutex while invoking observer callbacks. A test observer calls `Advance` inside `OnEvents`, causing reentrancy on the same mutex → deadlock/abort.
- Fixes in `src/railcore/engine_stub.cpp`:
  - Compute the `SimulationTickResult` under lock; copy `observers_` to a local vector while holding the lock.
  - Set `inProgress_ = true`, release the lock, then invoke `OnEvents` on the copied list; reacquire the lock, set `inProgress_ = false`, return.
  - Ensure `Subscribe`/`NotifySnapshot` capture state and observers under lock, then invoke `OnSnapshot` outside the lock.
  - Never mutate the observed container during callbacks; use copies to avoid iterator invalidation if observers unsubscribe in callbacks.
- Success criteria:
  - `TestReentrantAdvanceBusy` passes (Busy observed exactly once).
  - No hangs; no MSVC abort dialogs; all tests complete.

## Stage 2 — Verify, Baseline, and Coverage Measurement

- Rebuild and run the smoke tests; confirm clean console exits.
- Add coverage tooling:
  - Use OpenCppCoverage to instrument `build\\msvc\\Debug\\RailCoreTests.exe`.
  - Filter sources to `src\\railcore`; exclude `tests`, `third_party`, and `src\\railui`.
  - Export Cobertura XML to `build\\msvc\\Debug\\coverage.xml`.
- Add `tools/coverage.cmd` and a `COVERAGE=1` switch in `build.cmd` that runs the coverage pass.
- Record the first baseline in `docs/coverage/baseline.md`.

## Stage 3 — Adopt GoogleTest (structured tests)

- Add a `RailCoreGTest` project (Win32, Debug/Release) using the Microsoft GoogleTest NuGet.
- Migrate existing smoke assertions into suites:
  - ParserTests, EngineLifecycleTests, ObserverTests, IdCanonicalizationTests, LimitsAndValidationTests, ReentrancyTests.
- Keep the console smoke app temporarily; remove it after parity.
- Run via VS Test Explorer and `vstest.console` in CI.

## Stage 4 — Targeted Tests to Raise Coverage

Focus module-by-module with explicit boundary and negative cases.

### rcd_repository (parser)

- GENERAL: malformed `key=value`; invalid keys ignored; `StartTime`/`StopTime` bounds; `StopTime <= StartTime` invalid.
- SELECTOR: min/max ID; numeric field bounds; token count boundary (≥8 valid, 7 invalid); non-numeric field rejections.
- ROUTES: exact token count (9 total); selector existence; section existence; encoded `secondary*1000 + primary` validation; negative/zero handling.
- LOCOYARD: multiple valid lines; `Disabled` accepted; invalid stock codes and offsets rejected; mixing `Disabled` and entries.
- TIMETABLE: minute 59 vs 60; optionality of Arr/Dep; ArrSelector 1..49; `NextEntry` references existence.
- Performance: synthetic large layout to sanity-check parsing time.

### engine_stub (runtime)

- Commands: Pause/Resume/Stop idempotency; invalid payloads; limit enforcement leaves no retained state.
- Advance: negative dt error; dt clamp emits Warning diagnostics; `WorldDelta` composition for multiple changes; zero-change tick behavior.
- Observer: snapshot-on-subscribe; unsubscribe during callback; reentrancy returns Busy.
- RNG: deterministic seeding only when configured and provider present.

### rcd_id (canonicalization/IDs)

- CR/LF vs CRLF vs trailing whitespace; tabs; schema tag influence; empty-file handling.

Coverage targets (by module):

- Parser lines ≥ 85%, branches ≥ 70%.
- Engine lines ≥ 80%, branches ≥ 65%.
- ID helpers lines ≥ 90%.

## Stage 5 — CI Coverage Reporting + Gates

- Update `.github/workflows/msvc.yml`:
  - Install OpenCppCoverage (Chocolatey) and run `build.cmd Debug COVERAGE=1`.
  - Upload `coverage.xml` as an artifact; optional PR comment with a coverage bot.
- Gates:
  - Soft gate initially (warn when `src\\railcore` lines < 80%).
  - Convert to hard gate after stabilization.

## Stage 6 — Planned Functional Work With Tests

- Replace FNV‑1a with SHA‑256 for layout IDs (Windows BCrypt or internal implementation). Keep canonicalization unchanged; extend ID tests accordingly.
- Introduce initial scheduling behavior behind a feature flag and add unit tests for state transitions and deltas.

## Stage 7 — UI Boundary Confidence (no GUI)

- Add a minimal UI adapter surface that consumes `WorldDelta`/events without OWL dependencies.
- Unit-test adapter logic headlessly and keep UI code excluded from coverage.

## Ownership and Guardrails

- Coverage scope: `src/railcore` only; exclude `src/railui` and `third_party` from metrics.
- Keep MSVC toolchain; do not introduce g++ or Borland. Respect existing line endings and style in source files.

## Acceptance Criteria

- Tests run headless (no dialogs) and report via console and GoogleTest.
- Reentrancy test passes; no deadlocks; no aborts.
- Cobertura XML produced locally and in CI.
- Thresholds met: `src/railcore` lines ≥ 80% (phase 1), parser ≥ 85%, ID ≥ 90%.
- CI warns (then fails) on coverage regressions under thresholds.

