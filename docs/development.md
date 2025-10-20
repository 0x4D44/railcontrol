# RailControl Development Notes

## Memory Guard Diagnostics

- Guard code compiles only in MSVC builds. Borland artefacts remain unchanged.
- The `[Diagnostics] EnableMemoryGuards` INI flag (default `1` in Debug, `0` in Release) controls whether the guard hooks run.
- Guard logs live in `%LOCALAPPDATA%\RailControl\Logs\memory_guard.log` with rotation (5 files × 2 MB).
- GitHub Actions runs `tools\lint_memory_guard.bat` to ensure the guard header compiles cleanly when `_MSC_VER` is undefined, catching regressions in gating.

### Enabling or Disabling PageHeap/Application Verifier

1. Build the desired configuration via `build_msvc.bat Debug` (or `Release`).
2. Run `tools\enable_memory_guards.bat enable Debug` to enable PageHeap + Application Verifier.  
   Use `disable` to remove the hooks or `status` to query current settings.
3. Launch `build\msvc\Debug\railc_msvc.exe`. Guards log startup/shutdown events automatically.

The script requires `gflags.exe` and `appverif.exe` on `PATH` (part of the Windows SDK Debugging Tools).

### Collecting Logs and Minidumps

- After a crash, execute `pwsh -File tools\collect_minidump.ps1` (optionally pass an output path).
- The script gathers `memory_guard.log*`, `ptrack_debug.log*`, and `*.dmp` files from `%LOCALAPPDATA%\RailControl\Logs` and `%LOCALAPPDATA%\RailControl\Dumps`, packaging them into a timestamped zip.

### Manual Overrides

- To opt out of guards without removing tooling, set `EnableMemoryGuards=0` under the `[Diagnostics]` section in `railc.ini`.
- Delete the key to revert to the default behaviour (Debug builds auto-enable, Release stays off).

## Runtime Guard Controls

- `DebugMemoryGuard::Enable/Disable/SetEnabled` can be invoked from future diagnostics hooks to toggle guards without restarting. Configuration bootstrap still honours `[Diagnostics] EnableMemoryGuards`.
- `DebugMemoryGuard::GetStatistics()` reports outstanding allocations, peak usage, and handler state for dashboards or debugger visualisations.
- Guard allocation logs land in `%LOCALAPPDATA%\RailControl\Logs\memory_guard.log` with rolling rotation; minidumps write to `%LOCALAPPDATA%\RailControl\Dumps`.
- The MSVC build scripts copy Windows SDK `dbghelp.dll` beside `railc_msvc.exe`, ensuring `MiniDumpWriteDump` is available on machines without matching debugger installations.

## Ownership Wrappers

- `TManagedArray<T, N>` replaces raw pointer grids for timetables and locomotives under MSVC, providing unique ownership with a Borland-friendly fallback.
- Slots work like legacy pointers (`array[i]->Method()`), but lifetime is automatic; call `Reset(index)` to clear an entry and `Emplace(index, args...)` to construct in place.
- Legacy consumers comparing `array[i] == 0` continue to compile; prefer `Reset`/`Get` for new code to keep ownership rules explicit.

## Handle Wrappers

- `DrawRaisedPanel` and `FillRectOpaque` (see `uihelpers.h`) provide reusable framing/backfill helpers for arrivals, departures, platform, and yard overlays.
- `DrawHeaderLabels` formats the column headers for those panes, driving consistent offsets without hand-coded `TextOut` blocks.
- `THandleGuard` and `TSharedHandle` (see `handleguard.h`) wrap GDI/UI handles and issue the matching destroy call automatically. Use the provided aliases (`TBrushGuard`, `TPenGuard`, `TFontGuard`, `TBitmapGuard`, `TDcGuard`, `TReleaseDcGuard`) when creating new resources in MSVC builds.
- Existing window classes (`TMainWindow`, `TLayout`, `TStartup`, `TToolbar`, `TToolbutton`, `TStatbar`, `TLocoyard`, `TPlatform`, `TSelector`) rely on these guards instead of manual `DeleteObject`/`DeleteDC`, reducing leak risk during exceptional paths.

## Buffer Helpers

- `TBuffer<N>` provides a fixed-capacity ANSI buffer that auto-initialises to an empty string and offers `.Data()`/`operator[]` access for legacy APIs.
- `CopyBuffer` and `FormatBuffer` mirror the old `CopyString` helpers but always take explicit capacities; the legacy wrapper remains only for compatibility and is marked deprecated in MSVC builds.
- `ReadDataFile()` uses `TBuffer` instances for `szInput`, `szComment`, and `StartText` parsing, with `ReadDataLine()` ensuring buffers are cleared before reuse.
