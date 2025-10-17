# Decision Log – Build System for Modern Toolchain

- **Date**: 17 Oct 2025
- **Participants**: Build Engineering (acting maintainer)
- **Context**: Stage 1 of the modern toolchain migration requires establishing a new build configuration for MSVC/OWLNext v7.x.

## Decision
Adopt **native Visual Studio solution/project files** as the primary build artifacts for the MSVC toolchain migration. CMake is deferred for a later phase if cross-platform builds become a requirement.

## Rationale
- Aligns with the existing Windows-only target audience and reduces ramp-up effort.
- Simplifies integration with Visual Studio IDE debugging and traditional `.vcxproj`-based build customization (resource compilation, response files).
- Minimizes up-front tooling changes, allowing focus on compiler/OWLNext porting work.

## Consequences
- Maintain `build/msvc/` with `.sln/.vcxproj` files generated or edited directly.
- GitHub Actions pipeline can invoke `msbuild` or `devenv` without additional tooling.
- If future cross-platform needs emerge, introduce CMake or Meson in a separate effort and keep Visual Studio projects in sync or auto-generated.

## Follow-up Actions
1. Generate initial `RailControl.sln` / `RailControl.vcxproj` once Visual Studio 2022 is installed (Stage 1 task).
2. Document usage instructions in `build/msvc/README.md` and update `README.md`.
3. Revisit decision after port completion if maintaining multiple project formats becomes burdensome.
