# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RailControl is a Windows railway simulation application originally built in 1994. It has been successfully modernized to use Visual Studio 2022 and OWLNext 7.0.19 while preserving all original functionality. The codebase is split into RailCore (backend static library) and RailUI (OWL-based frontend).

## Build Commands

### Primary Build Method
**RECOMMENDED**: `build.cmd Debug` or `build.cmd Release`
- Automatically builds OWLNext dependencies if missing
- Builds RailControl.exe, RailCore.lib, and RailCoreTests.exe
- Runs console smoke tests in Debug mode
- Copies output to repo root for easy execution
- Environment variables:
  - `BUILD_GTEST=1` - Build GoogleTest suites (requires NuGet restore)
  - `COVERAGE=1` - Generate coverage.xml with OpenCppCoverage
  - `STRICT=1` - Fail build when tests fail

### Manual MSVC Build
Open `build\msvc\RailControl.sln` in Visual Studio 2022 or use MSBuild directly:
```
msbuild build\msvc\RailControl.vcxproj /p:Configuration=Debug /p:Platform=Win32
```

### Build Requirements
- **Visual Studio 2022 17.11+** with Desktop C++ workload (v143 toolset)
- **OWLNext 7.0.19** - vendored under `third_party/owlnext/` (auto-built by build.cmd)
- **Windows SDK 10.0**
- **OpenCppCoverage** - Optional, for coverage analysis (choco install opencppcoverage)

### Running the Application
- **GUI**: `railc.exe` (or `build\msvc\Debug\railc.exe`)
- **Load layouts**: Use File menu to load `.RCD` files from `Game files\` directory
- **Command-line validation**: `railc.exe --rcd-validate "Game files\FAST.RCD"` (exits 0 if valid, 1 if invalid)
  - Wildcard support: `--rcd-validate "Game files\*.RCD"`
  - Add `--print-id` to include SHA-256 layout identifiers
  - Set `RCD_CLI_LOG=path` env var to mirror output to file

## Testing

### Console Smoke Tests
- Project: `build\msvc\RailCoreTests.vcxproj`
- Runs automatically in Debug builds via `build.cmd`
- Environment variables:
  - `RAILCORE_SMOKE_MODE=minimal` - Run quick smoke tests (default in CI/build.cmd)
  - `RAILCORE_ENABLE_FULL_SMOKE` - Compile/run full legacy smoke suite
  - `RAILCORE_TEST_OUTDIR` - Directory for test output files

### GoogleTest Suites
- Project: `build\msvc\RailCoreGTest.vcxproj`
- Build with `BUILD_GTEST=1 build.cmd Debug`
- Runs in CI; optional locally
- Outputs: `build\msvc\Debug\gtest-results.xml`

### Running Individual Tests
```cmd
# Run all GoogleTest tests
build\msvc\Debug\RailCoreGTest.exe

# Run tests matching a pattern
build\msvc\Debug\RailCoreGTest.exe --gtest_filter="RcdRepository*"

# Run a single test
build\msvc\Debug\RailCoreGTest.exe --gtest_filter="RcdRepositoryTest.LoadFast"

# List all available tests
build\msvc\Debug\RailCoreGTest.exe --gtest_list_tests
```

### Coverage
- **Single report**: `COVERAGE=1 build.cmd Debug` → `build\msvc\Debug\coverage.xml`
- **Coverage gates**: CI enforces ≥90% line coverage (console tests alone + merged with GTest)
- **Local runners**: `tools\run_tests.ps1 -Config Debug -Coverage`
- **Gate scripts**: `tools\coverage_gate.ps1` and `tools\coverage_merge_gate.ps1`

### Test Data
- Valid layouts: `Game files\*.RCD` (FAST, KINGSX, QUEENST, WAVERLY)
- Invalid layouts: `bad_*.rcd` and `dup_*.rcd` files in repo root (for validation testing)

## Architecture Overview

See **[ARCHITECTURE.md](ARCHITECTURE.md)** for comprehensive details including component diagrams, state machines, and data flow.

### Module Structure
- **RailCore** (`src/railcore/`, `include/railcore/`) - Backend static library
  - `RcdRepository` - RCD file loading/validation
  - `RcdId` - SHA-256 layout identification
  - `IRailEngine` interface with stub implementation
- **RailUI** (`src/railui/`) - OWL-based GUI application
  - `TManager`/`TMainWindow` - Application framework
  - `TLayout` - Main simulation canvas (rendering, train movement)
  - Train view windows (arrivals, departures, platforms, locoyard)
- **Tests** (`tests/`) - Console smoke tests and GoogleTest suites
- **OWLNext 7.0.19** - Vendored under `third_party/owlnext/`

### RCD File Format
Plain-text INI-style format with 9 required sections: `[GENERAL]`, `[SECTIONS]`, `[OVERLAPPING]`, `[PLATFORMS]`, `[SELECTOR]`, `[ROUTES]`, `[LOCOS]`, `[LOCOYARD]`, `[TIMETABLE]`. See `docs/2025.10.17 - RCD file format guide.md` for complete specification.

## Development Patterns

### Code Conventions
- **Naming**: Hungarian notation (e.g., `PStatbar`, `HBRUSH`), OWL/Windows style with `T` prefix for classes
- **Files**: Uppercase extensions (`.H`, `.CPP`) preserved from legacy codebase
- **Indentation**: 2 spaces; preserve existing brace placement
- **Line endings**: CRLF (Windows standard)
- **Headers**: Public API under `include/`, implementation headers under `src/`

### OWLNext Framework
- Built against **OWLNext 7.0.19** with legacy compatibility modes
- Message handling via `DECLARE_RESPONSE_TABLE` macros
- Resource IDs defined in `RESOURCE\railc.h`

### Debugging
- **Debug tracing**: Use `TRC_*` macros (enabled when `MDDEBUG` is defined)
- **Memory guard**: `DEBUGMEMORYGUARD.CPP` available for memory debugging (opt-in, legacy)
- **Visual Studio**: Attach debugger to `railc.exe` for breakpoint debugging

## CI/CD

GitHub Actions workflow (`.github/workflows/msvc.yml`):
- Builds Debug and Release configurations in parallel
- Runs console smoke tests and GoogleTest suites (Debug only)
- Validates sample RCD files with `--rcd-validate`
- Enforces 90% coverage gates (hard fail if below threshold)

## Additional Documentation

- **[README.md](README.md)** - Project overview, structure, and quick start
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Comprehensive architecture (700+ lines)
- **`docs/`** - Design documents, migration plans, debugging journals