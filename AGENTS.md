# Contributor Guidelines

This document provides guidelines for contributors. For build commands and architecture details, see **[CLAUDE.md](CLAUDE.md)**.

## Quick Reference

- **Build**: `build.cmd Debug` or `build.cmd Release`
- **Test**: `BUILD_GTEST=1 build.cmd Debug` (runs all tests)
- **Single test**: `build\msvc\Debug\RailCoreGTest.exe --gtest_filter="TestName"`
- **Run**: `railc.exe` or load layouts via File menu from `Game files/`

## Coding Style

- **Indentation**: 2 spaces; preserve existing brace placement
- **Naming**: OWL/Windows style - `T` prefix for classes (e.g., `TMainWindow`), PascalCase methods, ALL_CAPS constants
- **Files**: Preserve historical casing (`.CPP`, `.H`) and CRLF line endings
- **Changes**: Keep minimal and focused; no auto-formatting of unrelated code

## Commit Guidelines

- Use imperative mood with scoped prefixes when helpful:
  - `core:` - RailCore engine/persistence changes
  - `ui:` - RailUI/OWL changes
  - `build:` - Build system changes
  - `docs:` - Documentation
  - `test:` - Test additions/fixes
- Example: `core: fix RCD parser handling of empty sections`

## Pull Request Guidelines

- Clear description with rationale
- Include repro/verify steps
- Screenshots for UI changes
- Link related issues
- Do not reformat unrelated files

## Testing Requirements

- All changes must pass existing tests: `STRICT=1 build.cmd Debug`
- New features require tests (target 90%+ coverage)
- Run `--rcd-validate` on sample layouts after RCD-related changes
