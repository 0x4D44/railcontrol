# RailControl Codebase Review - 2025-10-20

## Scope & Approach
- Reviewed first-party C++ sources in the repository root (e.g., `RAILC.CPP`, `LAYOUT.CPP`, UI dialogs, and utility headers).
- Excluded `third_party/` and `owlnx630/` sources from findings except for contextual awareness while tracing call paths.
- Read each module manually, following layout initialisation, UI flows, resource handling, and diagnostic scaffolding.

## Architecture Overview
- **Application Shell (`RAILC.*`, dialogs)** - `TManager` bootstraps the app, `TMainWindow` orchestrates child windows, persists configuration via INI, and surfaces splash/start/finish/about/config dialogs.
- **Simulation Core (`LAYOUT.*`, `TIMETABL.*`, `LOCOS.*`, `PLATDATA.*`, `SECTION.*`, `OVLPDATA.*`, `ROUTES.*`)** - `TLayout` drives timetable state, parses `.RCD` data, manages selectors/routes, updates on timer ticks, and accumulates performance metrics.
- **Operational Views (`ARRIVALS.*`, `DEPARTUR.*`, `PLATFORM.*`, `LOCOYARD.*`, `STATBAR.*`, `TOOLBAR.*`, `TOOLBUTT.*`, `SELECTOR.*`)** - child windows render train movements and statuses, tool/selector interactions, and rely on layout-owned data.
- **Infrastructure (`general.h`, `handleguard.h`, `ownership.h`, `uihelpers.h`, `DEBUGMEMORYGUARD.*`)** - shared constants, safe buffer helpers, RAII wrappers for Windows handles, managed-pointer arrays, and optional allocation tracing.

## Key Findings
| Severity | Location | Issue | Impact | Recommendation |
| --- | --- | --- | --- | --- |
| High | `TOOLBUTT.CPP:137` | Registers toolbar-button window class with `CS_HREDRAW || CS_VREDRAW`; logical OR collapses to `1` instead of combining flags. | Horizontal invalidations do not trigger repaint, so buttons can render stale artefacts after width changes or parent resizing. | Switch to bitwise OR (`CS_HREDRAW | CS_VREDRAW`) and retest toolbar layout/resizing scenarios. |
| Medium | `ABOUT.CPP:41` | `TAbout::CmOk` deletes bitmaps, but there is no matching handler for `IDCANCEL`/`WM_DESTROY`. | Closing the dialog via Esc, Alt+F4, or system menu leaks the two bitmaps and the GDI object count grows across sessions. | Release `HLoco1`/`HLoco2` in a destructor or override `CmCancel`/`EvDestroy` to mirror the cleanup path. |
| Medium | `CONFIGUR.CPP:186` | Configuration dialog still calls legacy `WinHelp`. | `WinHelp` is absent on modern Windows; the call fails silently so users see no guidance despite the help button being enabled. | Route help requests through the existing `ShowHelpPlaceholder` helper (as the main window already does) or provide updated documentation. |
| Medium | `RAILC.CPP:1385` | Toolbar command handlers assume `ToolbarHan` is always non-null. Diagnostic builds (`DIAG_SKIP_TOOLBAR`) set the pointer to `NULL`, and creation failures would do the same. | Invoking File->New or other toolbar updates in those modes dereferences `nullptr`, crashing before diagnostics can run. | Guard all `ToolbarHan->...` access (and similar `StatbarHan` writes) or keep the diagnostic skip macros in sync with the runtime paths. |
| Medium | `LAYOUT.CPP:4414` | `PTrackLoco` capacity overflow is only logged (`fprintf`) without enforcing limits. | When more than the expected light-loco slots are needed the simulation continues with inconsistent state (locos remain untracked) with no user-visible warning. | Convert to a managed container or at least clamp/evict with an explicit UI/trace alert to keep state coherent. |
| Low | `LAYOUT.CPP:2084` | `.RCD` parser re-reads the file section by section and uses `atoi` without range/error checks. | Data issues collapse to zero defaults, making broken layouts hard to diagnose; repeated reopen costs I/O and complicates future edits. | Replace with a single-pass parser that records line numbers, uses `strtol`/validation, and reports detailed errors. |
| Low | `STATBAR.CPP:146` | `SetText` assumes incoming `LPARAM` is a valid `char*`. | Any stray message (for example automation or a malformed plug-in) with a null pointer will crash the UI thread. | Treat `lp == 0` as an empty string and optionally assert/log unexpected call sites. |
| Low | `RAILC.CPP:773` | Toolbar bitmap load result is unchecked before use. | Missing or renamed resources lead to blank toolbar renders with no diagnostics, complicating field support. | Check the handle, fall back to disabling the toolbar, and log via `StartupLog`/trace when resources are absent. |

## Module Notes
- **Application & Shell (`RAILC.*`, `STARTUP.*`, `START.*`, `FINISH.*`, `ABOUT.*`, `CONFIGUR.*`)**
  - `TMainWindow` owns the child windows, loads INI state, and wires menu/toolbar commands; instrumentation macros (`DIAG_*`) selectively skip parts of setup.
  - Startup logging is stubbed (`StartupLog` no-ops); re-enabling it would strengthen diagnostics in the field.
  - Dialog code leans on OWL defaults; introducing RAII (for example smart wrappers for bitmaps) would simplify cleanup paths.
- **Simulation Core (`LAYOUT.*`, related models)**
  - `TLayout` drives nearly all gameplay state with fixed-size arrays and 1-based indices; the diagnostics helpers (`LogLayoutEvent`, `OpenDiagnosticsLog`) are helpful but should be compiled out of production if overhead becomes noticeable.
  - File ingestion (`ReadDataFile`) could be split into smaller helpers with stronger validation to cut down on repetition and subtle failure modes.
- **Operational Views & Controls (`ARRIVALS.*`, `DEPARTUR.*`, `PLATFORM.*`, `LOCOYARD.*`, `STATBAR.*`, `TOOLBAR.*`, `TOOLBUTT.*`, `SELECTOR.*`)**
  - Rendering paths rely on GDI with manual colour selection; RAII guards (`TDcGuard`, `TReleaseDcGuard`) are used consistently.
  - Tool and selector logic still share mutable arrays (`ToolButtData`, selector state flags); migrating to per-instance structs would clarify ownership and reduce errors.
- **Infrastructure & Utilities (`general.h`, `handleguard.h`, `ownership.h`, `uihelpers.h`, `DEBUGMEMORYGUARD.*`)**
  - `TBuffer`, `CopyBuffer`, and `FormatBuffer` centralise safe string handling and are worth keeping as the default approach.
  - `THandleGuard` / `TManagedArray` provide RAII for handles and heap objects; ensure new allocations flow through them.
  - `DebugMemoryGuard` overrides global `new/delete`, emitting dumps and leak logs; keep initialisation guarded so release builds stay lean.

## Cross-Cutting Observations
- Manual memory management and 1-based indexing are pervasive; favour `std::vector`/`std::array` and RAII in new work to reduce fragility.
- Error handling typically surfaces message boxes; supplement with structured logging (using the existing log directory) so failures can be diagnosed headless.
- Diagnostic macros toggle significant behaviour; keeping runtime code tolerant of those flags prevents crashes when instrumentation is enabled.

## Testing & Tooling Opportunities
- No automated coverage exists for layout parsing or UI flows. A harness that loads each sample `.RCD` under Debug with `DebugMemoryGuard` enabled would catch regressions early.
- Consider lightweight screenshot or text-dump comparisons for `ARRIVALS`/`DEPARTUR` windows to detect rendering regressions.
- Expand diagnostics logging to capture resource-load failures (for example missing toolbar bitmap) to simplify support.

## Suggested Next Steps
1. Patch the high/medium findings (toolbar class flags, `TAbout` cleanup, help routing, toolbar pointer guards, `PTrackLoco` handling).
2. Add resource-load checks and pointer null-guards around shared UI components before running instrumentation-heavy builds.
3. Plan a follow-up refactor of `ReadDataFile` into a structured parser with explicit validation and error reporting.
4. Document the memory guard workflow and ensure release builds either disable it or expose clear opt-in controls.
