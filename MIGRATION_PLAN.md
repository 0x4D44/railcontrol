# Migration Plan: RailControl → OWLNext

## Goals
- Migrate legacy OWL app to OWLNext 6.30 with minimal code churn.
- Produce a working BC5 build first; then enable a modern toolchain (MSVC).

## Current State
- Sources in repo root; OWLNext 6.30 bundled under `owlnx630/`.
- BC5 build scripts and response files present; project links and runs against a locally built OWLNext static lib.

## Phases
1) Baseline and Inventory
   - Confirm headers/usages (`<owl/...>`, CTL3D), build scripts, and resource layout.
2) Build OWLNext (BC5)
   - Use `build_owln_bc5_short.bat` to set a minimal env and build a single static, MT, non‑Unicode lib.
   - Workarounds: short paths via `SUBST O:`, response files for TLIB, minimal environment (COMSPEC, SystemRoot set).
3) Rebuild RailControl Against Local OWLNext
   - Use `BccW32_local.cfg` and `link_local.rsp` to point to `./owlnx630/include` and `./owlnx630/lib`.
   - Verify the app links and runs; perform smoke tests.
4) Code Prep for Cross‑Compiler
   - Prefer `#include <owl/...>` headers (already consolidated in `CLASSDEF.H`).
   - Guard CTL3D direct calls or remove; prefer `TApplication::EnableCtl3d` when targeting BC5 only.
5) Build OWLNext with MSVC
   - Use `nmake -f vc.mak` for `owlcore`, `owlext`, `owlfx` (single variant initially).
6) Add MSVC Project for RailControl
   - Create `.vcxproj` or `nmake` makefile targeting the MSVC‑built OWLNext.
   - Link system libs: `user32`, `gdi32`, `comctl32`, `shell32`, `comdlg32`, `advapi32`, `version`.
7) Verify and Iterate
   - Build, run, and smoke test on modern Windows. Plan separate Help modernization (WinHelp → CHM/HTML) later.

## Build Tips (BC5)
- Trim environment (Windows 11 env blocks can break MAKE).
- Ensure: `COMSPEC=C:\Windows\System32\cmd.exe`, `SystemRoot=C:\Windows`.
- Use response files for long TLIB invocations (already integrated in `bc.mak`).

## Next Steps
- Optional: Unicode build, remove CTL3D, modernize Help.
- Add CI for MSVC build (artifact only).

