# RCD CLI Validation — Implementation Plan

## Overview
This plan implements `railc --rcd-validate` as a headless validation mode inside the existing GUI binary, per the HLD. It emphasizes minimal surface-area changes, deterministic behavior, and CI coverage.

## Stages

1) Early CLI Dispatch in UI Entry
- Files: `src/railui/RAILC.CPP`
- Tasks:
  - At the very beginning of `OwlMain` (or the WinMain wrapper), scan `argv` for `--rcd-validate`/`-rcd-validate`/`/rcd-validate` (case-insensitive).
  - If present, bypass all UI and memory-guard setup; call `RunRcdValidationCli(argc, argvW)` and return its code.
- Exit criteria:
  - Launching with the flag returns immediately without constructing any windows.
  - Normal UI path is unaffected when flag is absent.

2) CLI Runner Implementation
- Files: `src/railui/cli_validate.cpp` (new), optional header `src/railui/cli_validate.h`.
- Tasks:
  - Implement `int RunRcdValidationCli(int argc, wchar_t** argv)`:
  - Set `SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX)`.
  - Debug-only: suppress CRT pop-ups.
  - Attempt `AttachConsole(ATTACH_PARENT_PROCESS)`; if successful, reopen stdio to `CONIN$`/`CONOUT$`, call `std::ios::sync_with_stdio(true)`.
  - Parse paths after the switch; recognise auxiliary flags like `--print-id`; on no paths, print usage to stderr and return `2`.
    - Expand wildcards with `FindFirstFileW/FindNextFileW`.
    - For directories, enumerate `*.RCD` non-recursively (case-insensitive); if none, print `Invalid: <dir>: no .RCD files found` and mark failure.
    - Apply deterministic ordering: sort expanded file lists case-insensitively by full path before validation.
    - For each file, use `RailCore::RcdLayoutRepository::Load(...)` and print `Valid` / `Valid: <path>` or `Invalid: <path>: <reason>`.
    - Aggregate exit: `0` if all valid; `1` if any invalid/not found/empty-dir; `3` on unexpected exception with a concise message to stderr.
    - Factor a pure helper (no console APIs) for validating a single path to enable unit tests.
- Exit criteria:
  - Works from a console with correct stdout/stderr behavior; does not show any UI.

3) Project Wiring
- Files: `build/msvc/RailControl.vcxproj`
- Tasks:
  - Add `src/railui/cli_validate.cpp` to the project; ensure Unicode/wide APIs are available.
  - No additional libraries required beyond existing ones.
- Exit criteria:
  - Debug build succeeds; `railc.exe` size increases minimally.

4) Local Smoke Tests
- Tasks:
  - From repo root:
    - `build\\msvc\\Debug\\railc.exe --rcd-validate "Game files\\FAST.RCD"` → `Valid`.
    - `build\\msvc\\Debug\\railc.exe --rcd-validate bad_tt_arrsel.rcd` → one-line error; exit `1`.
    - `build\\msvc\\Debug\\railc.exe --rcd-validate "Game files"` → lines per file; mixed results produce exit `1`.
    - `build\\msvc\\Debug\\railc.exe --rcd-validate "Game files\\*.RCD"` → sorted `Valid:` lines; exit `0`.
- Exit criteria:
  - Outputs match HLD and exit codes are correct.

5) CI Integration
- Files: `.github/workflows/msvc.yml`
- Tasks:
  - After building Debug, add two steps:
    - Run `railc --rcd-validate "Game files\\FAST.RCD"` and expect success (exit `0`).
    - Run `railc --rcd-validate bad_tt_arrsel.rcd` and expect failure (exit `1`).
    - Run `railc --rcd-validate "Game files\\*.RCD"` and expect success (exit `0`).
- Exit criteria:
  - CI job fails on unexpected exits; logs show one-line messages.

6) Unit Tests (Optional, Recommended)
- Files: `tests/gtest/cli_validate_tests.cpp` (new), `build/msvc/RailCoreGTest.vcxproj`
- Tasks:
  - Add tests for the pure helper function covering: valid file, invalid file, not found, directory with zero `.RCD` files, wildcard expansion logic with a temp directory, and deterministic sort order.
  - Keep these tests independent of console APIs.
- Exit criteria:
  - Tests pass in CI; coverage includes helper logic branches.

7) Documentation Update
- Files: `README.md`
- Tasks:
  - Add a “Command-line” section with usage and examples.
  - Note GUI subsystem console attach behavior and that output may be invisible if launched from Explorer (exit codes still correct).
- Exit criteria:
  - Docs reflect current behavior and examples are accurate.

8) Post-Implementation Cleanup
- Tasks:
  - Quick pass to ensure no remaining references to alternative runners for this feature.
  - Confirm no regressions in UI startup path.
- Exit criteria:
  - Clean builds; normal app behavior unchanged without the flag.

## Risks & Mitigations (Implementation Focus)
- Wildcard expansion in GUI subsystem:
  - Mitigate via explicit `FindFirstFileW` usage; add unit coverage for helper.
- Console attach edge cases:
  - Reopen stdio on success; still rely on exit codes if attach fails.
- Path encoding:
  - Use wide strings end-to-end; print with `std::wcout` or `WriteConsoleW` when attached.
