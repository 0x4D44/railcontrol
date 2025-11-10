# RCD Command-Line Validation — High-Level Design

## Goals
- Add a headless validation mode to `railc.exe` invoked as `railc --rcd-validate`.
- Validate one or more `.RCD` layout files and print either clear error messages or a clear "Valid" message.
- Return non-zero exit codes for validation or usage errors to enable automation.
- Avoid launching any UI or showing dialogs; run cleanly in consoles and CI, and never pop modal error boxes.

## Non-Goals (v1)
- No JSON output or machine-readable schema (may be added later).
- No automatic repair or mutation of `.RCD` files.
- No dependency on external runners; implemented inside the existing `railc.exe` binary.

## User Experience
- Invocation (single file):
  - `railc --rcd-validate "Game files\FAST.RCD"`
  - Output: `Valid`
  - Exit code: `0`.
- Invocation (single file invalid):
  - Output example (single-line, greppable):
    - `Invalid: Game files\FAST.RCD: TIMETABLE 42: ArrSelector out of allowed range (1..49)`
  - Exit code: `1`.
- Invocation (multiple paths):
  - `railc --rcd-validate path\a.rcd path\b\` (files and/or directories)
  - Prints a per-item verdict (one line per file). Final exit is non-zero if any invalid.
  - If a directory contains zero `.RCD` files (non-recursive in v1), print `Invalid: <dir>: no .RCD files found` and contribute to non-zero exit.
  - Wildcards must be quoted to avoid shell interference: `railc --rcd-validate "Game files\\*.RCD"`.
- Usage error (no paths provided):
  - Prints usage and returns `2`.

## CLI Options (v1)
- `--rcd-validate <path> [<path> ...]`
  - Each `<path>` may be a file or a directory.
  - If a directory is specified, all `*.RCD` files under it are validated (non-recursive in v1).
  - Case-insensitive switch; accept any of: `--rcd-validate`, `-rcd-validate`, or `/rcd-validate`.
  - The equals form (e.g., `--rcd-validate=path`) is not supported in v1.
  - Optional `--print-id` flag mirrors the stable layout SHA-256 (`desc.id`) in success lines, aiding support fingerprinting without impacting exit codes.
  - If no `<path>` follows `--rcd-validate`, usage is shown and exit code `2` is returned.
- Usage text (printed on usage error):
  - `Usage: railc --rcd-validate <file-or-directory> [more paths]`
  - `Validates .RCD files without launching the UI.`
- Future (not implemented in v1; reserved):
  - `--recursive` to traverse subdirectories when a directory is passed.
  - `--quiet` to suppress per-file success lines when multiple inputs are provided.
  - `--json` to emit machine-readable results (JSON Lines).
  - `--id` to also print the computed layout SHA-256 id for valid files.

## Output and Exit Codes
- Success (all files valid):
  - Single file: prints `Valid`.
  - Multiple files: prints `Valid: <path>` per file.
  - Exit: `0`.
- Validation error(s):
  - Prints a single line per file: `Invalid: <path>: <reason>`.
  - Exit: `1` if any invalid, not found, or directory with no `.RCD` files.
- Not found / not a file (treated as validation failure in v1):
  - `Invalid: <path>: not found`
  - Exit contributes to `1`.
- Usage error (missing or bad arguments): `2`.
- Unexpected internal error: `3`.
  - Prints `Internal error: <message>` to stderr if available.
  - Exit precedence: usage error (2) takes precedence only when no paths are provided; otherwise individual file/dir results determine `0` vs `1`.

## Architecture
- Implement a CLI pathway inside the GUI binary entrypoint before any UI initialization.
- Use existing backend parsing/validation implemented in `RailCore`:
  - `RailCore::RcdLayoutRepository::Load(LayoutDescriptor&, WorldState&)` — performs parsing and returns a `Status` with details.
  - `rcd_repository` already provides detailed, user-friendly validation messages and a fallback to sibling `Game files`.
- Console I/O for a GUI subsystem executable (Windows GUI subsystem has no console by default):
  - First, call `AttachConsole(ATTACH_PARENT_PROCESS)`.
  - If attach succeeds, reopen standard streams to the console (e.g., `CONOUT$`/`CONIN$`) so `std::(w)cout`/`std::(w)cerr` reliably reach the console.
  - If attach fails because there is no parent console, do not allocate a console by default in v1 (prevents a console window flash when launched from Explorer). Still run and return exit codes; stdout/stderr writes are best-effort.
  - When running from a real console (the primary expected usage), output appears normally.
  - Ensure no UI dialogs are shown; set `SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX)` in CLI mode. In Debug, suppress CRT dialog boxes via `_set_abort_behavior(0, _WRITE_ABORT_MSG|_CALL_REPORTFAULT)` and `_CrtSetReportMode`/`_CrtSetReportFile`.
  - Set `std::ios::sync_with_stdio(true)` and flush outputs (`std::endl`) to ensure prompt emission in CI.

## Components and Changes
- UI entry (`src/railui/RAILC.CPP`):
  - Update `int OwlMain(int argc, tchar* argv[])` (or `wWinMain`/`WinMain` wrapper as applicable) to parse arguments at the very start.
  - If `--rcd-validate` is present, dispatch to a new CLI function and return its exit code without initializing memory guards, constructing `TApplication`, or creating any windows.
  - Keep existing behavior unchanged when the flag is not present.
- New module: `src/railui/cli_validate.cpp` (and header if needed):
  - `int RunRcdValidationCli(int argc, wchar_t** argv)` (wide APIs to avoid codepage issues).
  - Responsibilities:
    - Try to attach to an existing console; do not allocate a new one in v1.
    - Parse subsequent arguments as paths; if none, print usage and return `2`.
    - Expand directories to `*.RCD` files (non-recursive v1). Accept wildcards by manually expanding patterns (FindFirstFileW/FindNextFileW) because GUI subsystem apps do not get wildcard expansion.
    - Apply deterministic ordering: sort expanded file lists case-insensitively by full path before validation.
    - For each file:
      - Build `RailCore::LayoutDescriptor` with `sourcePath` as `std::filesystem::path{std::wstring}`.
      - Call `RailCore::RcdLayoutRepository::Load(desc, worldState)`.
      - If ok: print `Valid` (single-file) or `Valid: <path>` (multi-file).
      - If not ok: print `Invalid: <path>: <status.message>`.
    - Aggregate exit code: `1` if any invalid or not found, else `0`.
  - Ensure no exceptions escape; catch-all sets exit `3` with a succinct message written to stderr if available.
- Backend reuse:
  - No backend changes are required for v1; the parser already yields detailed errors.

## Argument Parsing Details
- The `--rcd-validate` switch terminates option parsing and consumes following tokens as paths until end of argv or next `--` (future extension).
- Accept `--rcd-validate`, `-rcd-validate`, and `/rcd-validate` (case-insensitive).
- Paths are accepted as absolute or relative. The repository’s built-in fallback will also try sibling `Game files` when a bare filename is provided.
- For directory inputs: enumerate files matching `*.RCD` (case-insensitive) in that directory only (v1).
- For wildcard inputs: when argv contains `*.RCD`, expand manually via `FindFirstFileW`/`FindNextFileW` since GUI subsystem apps do not get shell globbing.
- Do not support `--rcd-validate=path` in v1; require paths as separate arguments after the switch.
 - File extension matching is case-insensitive (`.RCD` or `.rcd`).
 - Long paths: rely on system long-path settings; v1 does not add `\\?\` prefixes explicitly.

## Console Behavior and Diagnostics
- Suppress OS error UI in CLI mode using `SetErrorMode` and CRT handlers (Debug) to avoid modal dialogs.
- Do not initialize OWL/GUI subsystems or show message boxes.
- Write validation messages to stdout. Write usage and unexpected internal errors to stderr.
- If no console is attached (Explorer launch), we still return accurate exit codes; output may be invisible in v1 (documented). A future `--log <file>` could capture output explicitly.
- Implementation note: for CI and automation, an environment variable `RCD_CLI_LOG` (path) mirrors each output line to a file, aiding scenarios where GUI stdout is not visible. Logging remains environment-variable-only in v1; no CLI flag is planned until a future iteration.

## Build and Packaging
- Source additions:
  - `src/railui/cli_validate.cpp` compiled into `RailControl`.
- Project changes:
  - Add the new file to `build/msvc/RailControl.vcxproj`.
  - No separate binary; the feature ships within `railc.exe`.
- CI:
  - Add a simple CI step that runs `railc --rcd-validate "Game files\FAST.RCD"` to sanity check the mode and ensures non-zero exit on a known bad file (e.g., `bad_tt_arrsel.rcd`).
  - Use quoted paths to avoid issues with spaces in directory names.
  - Add an additional wildcard sanity: `railc --rcd-validate "Game files\\*.RCD"` (expects at least one `Valid:` line and exit `0`).

## Testing Strategy
- Unit/component (GoogleTest):
  - Factor the per-path validation into a pure helper (no console calls) returning a verdict and message; cover file-not-found and valid/invalid cases using the repo's sample `.RCD` files.
- Manual/console:
  - From repo root: `build\msvc\Debug\railc.exe --rcd-validate "Game files\FAST.RCD"` → `Valid`.
  - Bad files in repo root (e.g., `bad_tt_arrsel.rcd`) should print a precise validation error and exit `1`.
  - Directory mode: `railc --rcd-validate "Game files"` prints a line per file; ensure mixed results propagate exit `1`.
- Edge cases:
  - Non-existent path → `Invalid: <path>: not found` with exit `1`.
  - No args after switch → usage and exit `2`.
  - Directory with zero `.RCD` files → `Invalid: <dir>: no .RCD files found` with exit `1`.
  - Launched from Explorer (no parent console) → output may be invisible; exit code still correct (documented). No console allocation in v1.
  - Deterministic ordering: wildcard/directory results appear sorted case-insensitively by full path.

## Risks and Mitigations
- GUI subsystem stdout/stderr are not attached by default:
  - Attach to parent console when present; do not allocate console in v1 to avoid flashing a window when launched from Explorer. Document the behavior; add `--log` in a future iteration if needed.
- Regression risk to UI startup:
  - Gate all CLI logic behind an early check in `OwlMain`; skip memory guards and all UI paths.
- CRT/OS modal dialogs in error paths:
  - Set `SetErrorMode` and CRT handlers in CLI mode to ensure headless operation.
- Wildcard and directory expansion ambiguity:
  - Implement explicit expansion using `FindFirstFileW/FindNextFileW`; for directories, explicitly search `*.RCD` case-insensitively. Report `no .RCD files found` for empty dirs.
- Localization:
  - Messages are English-only in v1; reuse parser messages verbatim for clarity.
 - Long path behavior:
  - Rely on OS long path support (`LongPathsEnabled`). If needed later, add explicit `\\?\` handling as an enhancement.

## Implementation Plan (Short)
1) Add `cli_validate.cpp` with `RunRcdValidationCli` implementing parsing, console attach (best-effort), and validation loop.
2) Modify `OwlMain` to detect `--rcd-validate` and dispatch immediately (before memory guards and any UI initialization).
3) Wire up the new file in `RailControl.vcxproj`.
4) Quick smoke in Debug from a console; validate good and bad `.RCD` samples.
5) Add a CI step to sanity-check the CLI mode on both a good and a bad file.

## Open Decisions (tracked)
- Printing of normalized vs input paths:
  - Decision: print the input path as provided for clarity; do not canonicalize in v1.
- UTF-8 vs wide console output:
  - Decision: use `std::wcout`/`std::wcerr`; if attached to a console, consider `WriteConsoleW` internally for best fidelity. Messages contain ASCII by design.
- Deduplication of paths:
  - Decision: do not deduplicate; validate in provided order.
