# RailControl Remediation Plan - 2025-10-20

## Stage 1 ? Critical Stability Fixes (Week 1)
- **Toolbar repaint bug** (`TOOLBUTT.CPP:137`)
  - Replace logical OR with bitwise OR for `CS_HREDRAW`/`CS_VREDRAW`.
  - Regression check: resize main window, toggle toolbar visibility, and confirm buttons repaint correctly.
- **About dialog resource leak** (`ABOUT.CPP:41`)
  - Move bitmap cleanup to a destructor or pair with `CmCancel`/`EvDestroy`.
  - Smoke test: open/close About via OK, Esc, Alt+F4; monitor GDI object count.
- **Toolbar null-guarding** (`RAILC.CPP:1385` and related handlers)
  - Add defensive checks around `ToolbarHan`/`StatbarHan` before use; ensure diagnostic `DIAG_SKIP_TOOLBAR` builds run without crashes.
  - Test: run with diagnostics enabled and exercise File?New/Pause/Stop flows.

## Stage 2 ? User Experience & Help Flow (Week 2)
- **Configuration help pathway** (`CONFIGUR.CPP:186`)
  - Replace `WinHelp` call with `ShowHelpPlaceholder` or updated documentation link.
  - Verify Help button shows the placeholder message in modern Windows builds.
- **Resource load resiliency** (`RAILC.CPP:773`)
  - Check toolbar bitmap load results, log failures, and disable toolbar gracefully when assets are missing.
  - Test: temporarily rename bitmap resource to trigger fallback and confirm logging/UI behaviour.

## Stage 3 ? Simulation Robustness (Weeks 3?4)
- **`PTrackLoco` overflow handling** (`LAYOUT.CPP:4414`)
  - Introduce capacity management (e.g., `std::array`/`std::vector` plus explicit exhaustion handling) and surface warnings in UI/logs.
  - Test with stress layout to force >10 light loco movements; confirm state remains consistent.
- **Status bar message safety** (`STATBAR.CPP:146`)
  - Treat null `LPARAM` as empty string and log unexpected callers.
  - Test via automated message injection or instrumentation.
- **`.RCD` parser hardening** (`LAYOUT.CPP:2084` and surrounding code)
  - Refactor into single-pass parser with `strtol` validation and line-numbered error reporting.
  - Regression: load all shipped `.RCD` files; ensure detailed error surfaces on synthetic malformed input.

## Stage 4 ? Diagnostics & Tooling Enhancements (Week 5)
- Re-enable or document `StartupLog` support to aid field diagnostics.
- Extend diagnostics logging (using existing log directory) to capture resource-load and parser failures.
- Outline automated smoke harness: load sample layouts under Debug with `DebugMemoryGuard` enabled and capture screenshots/text dumps for `ARRIVALS`/`DEPARTUR` windows.

## Stage 5 ? Follow-up & Cleanup (Week 6)
- Review new logging/tests for noise; adjust thresholds.
- Update documentation/README with new help paths, diagnostic toggles, and testing checklist.
- Close outstanding warnings (`ARRIVALS.CPP:138`, `DEPARTUR.CPP:139`) by removing or using `TextString` locals.
