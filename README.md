## RailControl

Legacy 1994 OWL/Win32 railway control simulation, now maintained as a Visual Studio project targeting modern Windows with OWLNext 7.x.

### Requirements
- Windows with Visual Studio 2022 (Desktop development with C++)
- OWLNext 7.x sources vendored under `third_party/owlnext/`

### Building with MSVC
You can either open the solution in the IDE or invoke MSBuild directly:

```
build_msvc.bat Debug    # or Release
```

The script loads the VS environment and builds `build\msvc\RailControl.vcxproj`. Outputs land in `build\msvc\<Config>\railc_msvc.exe`.

### Manual Testing
- Launch `railc_msvc.exe`, open dialogs, and verify menu actions.
- Load layouts: `FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`.
- Press `F1` to exercise the bundled WinHelp content (requires WinHlp32 on modern Windows).

### Project Structure
- Sources: `*.CPP`, `*.H` in repo root.
- Resources: `RESOURCE/` (`railc.rc`, bitmaps/icons).
- Help assets: `HELP/`.
- OWLNext 7.x: `third_party/owlnext/`.
- MSVC build system: `build/msvc/`.

### Additional Documentation
- Modern toolchain status: `docs/modern_toolchain_migration_plan.md`.
- Debugging journal and retrospectives live under `docs/`.

### Contributor Guide
See `AGENTS.md` for repository expectations (style, build/test, PR process).

