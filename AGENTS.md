# Repository Guidelines

## Project Structure & Module Organization
- Source: C++ and headers in the repo root (e.g., `RAILC.CPP`, `LAYOUT.CPP`, `CLASSDEF.H`).
- Resources: `RESOURCE/` for icons, bitmaps, `.RC`, and compiled `railc.res`.
- Help: `HELP/` contains WinHelp sources and assets.
- Data: sample layouts `*.RCD` in the root (`FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`).
- Build config: `railc.mak`, `BccW32.cfg`, `link*.rsp`, and `.bat` scripts.
- Output: `railc.exe` and `.obj` files in the root.

## Build, Test, and Development Commands
- Full build: `build.bat` — compiles, builds resources, links. Requires Borland C++ 5.02 at `C:\Apps\BC5` and OWLNext 6.30 at `C:\Apps\owlnx630`.
- Make build: `make -f railc.mak` — uses `BccW32.cfg` and `link.rsp`.
- Resources only: `brc32 -R -FO RESOURCE\railc.res RESOURCE\railc.rc`.
- Run: execute `railc.exe` (produced in repo root).

## Coding Style & Naming Conventions
- Indentation: 2 spaces; preserve existing brace placement and spacing.
- Naming: OWL/Windows style — classes prefixed `T` (e.g., `TMainWindow`), PascalCase methods, ALL_CAPS constants.
- Files: preserve historical casing (`.CPP`, `.H`) and CRLF line endings.
- No auto‑formatter; keep changes minimal and focused.

## Testing Guidelines
- No automated tests. Perform manual smoke tests:
  - Launch `railc.exe`, open major dialogs, verify painting and menu actions.
  - Load sample layouts (`FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`).
  - Press `F1` to confirm Help loads.

## Commit & Pull Request Guidelines
- Commits: imperative mood; scoped prefixes when helpful (`build:`, `ui:`, `core:`, `docs:`).
  - Example: `core: modernize GetClassName signature (LPSTR -> LPCTSTR)`
- PRs: clear description, rationale, repro/verify steps, and screenshots for UI changes; link related issues.
- Do not reformat unrelated files or change toolchain paths in shared configs.

## Security & Configuration Tips
- Toolchain paths are set in `BccW32.cfg` and `link*.rsp`. Prefer adding variants (e.g., `BccW32_local.cfg`) rather than editing shared files.
- Ensure `PATH` includes `C:\Apps\BC5\bin` for `bcc32`, `tlink32`, and `brc32`.
- Keep generated outputs confined to the root; avoid committing user‑specific configs.

