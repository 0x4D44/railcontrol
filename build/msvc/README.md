# MSVC Build Scaffolding (Stage 1)

This directory will host Visual Studio solution/project files (or equivalent build definitions) for the modern toolchain migration.

## Planned Contents
- `RailControl.sln` – top-level solution referencing the application project (to be generated).
- `RailControl.vcxproj` – MSVC project configured for Win32, static runtime, and OWLNext v7 static libraries.
- `build_owlnext_msvc.bat` – helper that calls `vcvarsall x86` and runs `nmake -f vc.mak COMPAT=5` for `owlcore`, `owlext`, `ocf`, and `coolprj`.
- `Toolchain.props` / `.targets` – shared compiler/linker settings (pending).
- Optional CMake preset files if we later introduce automated project generation.

## Current Status
- Visual Studio 2022 Community is installed; OWLNext 7.0.19 libraries built via `build_owlnext_msvc.bat`.
- Decision recorded in `docs/decisions/2025-10-17-build-system.md`: native Visual Studio projects first, CMake optional later.

## TODO
- Generate solution/project files with complete source/resource lists.
- Integrate MSVC build into GitHub Actions (`.github/workflows/msvc.yml`).
