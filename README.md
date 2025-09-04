## RailControl

Legacy 1994 OWL/Win32 railway control simulation, updated to build with OWLNext 6.30 and Borland C++ 5.02.

### Requirements
- Windows (32-bit build environment)
- Borland C++ 5.02 at `C:\Apps\BC5` (bin on PATH)
- OWLNext 6.30 source bundled in `owlnx630/`

### Quick Start (Local OWLNext)
1) Build OWLNext (static lib) with the helper:
   - In cmd.exe: run `build_owln_bc5_short.bat` (sets a minimal env, builds OWLNext, and aliases the lib).
2) Build RailControl:
   - Run `build_local.bat` (compiles sources, builds resources, links).
3) Run:
   - `railc.exe` (created in the repo root).

### Alternative (Classic Make)
- Ensure `C:\Apps\owlnx630` has prebuilt OWL libs and run `make -f railc.mak`.

### Manual Testing
- Launch `railc.exe` and open major dialogs; verify painting and menus.
- Load layouts: `FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`.
- Press `F1` for Help (WinHelp may not be available on modern Windows).

### Project Structure
- Sources: `*.CPP`, `*.H` in repo root.
- Resources: `RESOURCE/` (`railc.rc`, icons/bitmaps, compiled `railc.res`).
- Help: `HELP/` (WinHelp sources/assets).
- OWLNext: `owlnx630/` (include, source, examples).
- Build scripts/config: `build*.bat`, `railc.mak`, `BccW32*.cfg`, `link*.rsp`.

### Migration Plan
See `MIGRATION_PLAN.md` for the staged upgrade to OWLNext and a modern toolchain, including BC5 build workarounds and next steps.

### Contributor Guide
See `AGENTS.md` for repository guidelines (structure, style, build/test, PRs).

