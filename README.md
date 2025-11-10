## RailControl

Legacy 1994 OWL/Win32 railway control simulation, successfully modernized to Visual Studio 2022 and OWLNext 7.0.19 while preserving all original functionality.

### Overview

**RailControl** is a railway simulation game that demonstrates clean separation between simulation engine (RailCore) and user interface (RailUI):

- **Architecture**: Modular C++17 with interface-based design
- **Engine**: RailCore static library with comprehensive validation
- **UI**: OWL-based Windows application with legacy Win32 rendering
- **Data Format**: RCD (Railway Control Data) plain-text INI-style files
- **Test Coverage**: 90%+ enforced via CI (targeting 98%), 380+ test cases
- **Capacity**: 1000 track sections, 1000 routes, 500 trains, 50 platforms

**Key Features**:
- 30+ train status codes tracking complete lifecycle from arrival to departure
- Real-time collision detection and route validation
- Safety interlocking system preventing invalid operations
- Stage telemetry for detailed train progress visualization
- Command-line validation mode for automated testing
- SHA-256 layout identification for regression testing

### Requirements
- Windows 10/11 with **Visual Studio 2022 17.11+** (Desktop development with C++)
- OWLNext **7.0.19** sources vendored under `third_party/owlnext/`
- No Borland/BCC toolchain or BWCC runtime is required or supported

### Building with MSVC
You can either open the solution in the IDE or invoke MSBuild directly:

```
build.bat Debug                         # or Release
BUILD_GTEST=1 build.bat Debug           # optionally also build/run GoogleTest locally (restores NuGet)
STRICT=1 build.bat Debug                # fail the build when tests fail (console and gtest)
```

The script initialises the VS environment, builds RailControl, RailCore, and the console tests. Outputs land in `build\msvc\<Config>\` (and `railc.exe` is also copied to the repo root).

### Manual Testing
- Launch `railc.exe`, open dialogs, and verify menu actions. Optionally speed up console smoke tests by setting `RAILCORE_SMOKE_MODE=minimal`.
- Load layouts from `Game files/`: `FAST.RCD`, `KINGSX.RCD`, `QUEENST.RCD`, `WAVERLY.RCD`.
- Press `F1` to exercise the bundled WinHelp content (requires WinHlp32 on modern Windows).
- When stage telemetry is available (via the modern RailCore engine), the Arrivals/Departures panes append the active stage bucket and section pair alongside the status text.

### Project Structure

```
RailControl/
├── src/
│   ├── railui/          # UI layer (OWL-based Windows application)
│   │   ├── RAILC.CPP    # Main application and window management
│   │   ├── LAYOUT.CPP   # Core simulation canvas and rendering (4795 lines)
│   │   ├── ARRIVALS.CPP # Train arrivals window (8 concurrent trains)
│   │   ├── DEPARTUR.CPP # Train departures window
│   │   ├── PLATFORM.CPP # Platform occupancy display
│   │   ├── LOCOYARD.CPP # Locomotive yard management (16 slots)
│   │   ├── TIMETABL.CPP # Timetable and scheduling system
│   │   ├── ROUTES.CPP   # Route and selector management
│   │   └── [24+ UI source files]
│   └── railcore/        # Backend engine (simulation logic)
│       ├── engine_stub.cpp        # Engine implementation with state machine
│       ├── persistence/
│       │   ├── rcd_repository.cpp # RCD file loading/validation
│       │   └── rcd_id.cpp         # SHA-256 layout identification
│       └── [3 implementation files]
│
├── include/
│   ├── railcore/        # Public engine API headers
│   │   ├── engine.h           # IRailEngine interface (8 methods)
│   │   ├── types.h            # WorldState, Route, Loco, TimetableEntry
│   │   ├── observer.h         # IObserver pattern (3 notification channels)
│   │   ├── commands.h         # Command pattern (6 command types)
│   │   └── persistence/       # Repository interfaces
│   └── railui/          # UI component headers
│
├── build/msvc/          # Visual Studio 2022 build system
│   ├── RailControl.vcxproj    # Main GUI application
│   ├── RailCore.vcxproj       # Engine static library
│   ├── RailCoreTests.vcxproj  # Smoke tests (2 tests, <1 sec)
│   └── RailCoreGTest.vcxproj  # Comprehensive tests (380+ tests)
│
├── tests/
│   ├── railcore/        # Console smoke tests
│   └── gtest/           # 380+ GoogleTest test cases (163 files)
│
├── Game files/          # Sample railway layouts
│   ├── FAST.RCD         # Baseline test layout
│   ├── KINGSX.RCD       # King's Cross station
│   ├── QUEENST.RCD      # Queen Street station
│   └── WAVERLY.RCD      # Waverley station
│
├── RESOURCE/            # Windows resources
│   ├── RAILC.RC         # Resource definitions
│   └── [Bitmaps, icons, sounds]
│
├── HELP/                # WinHelp documentation
│   └── RAILC.HLP        # Help file (staged by build)
│
├── third_party/
│   └── owlnext/         # OWLNext 7.0.19 framework (vendored)
│
├── tools/               # Build and test utilities
│   ├── coverage.cmd               # OpenCppCoverage wrapper
│   ├── coverage_gate.ps1          # Single coverage gate (≥90%)
│   └── coverage_merge_gate.ps1    # Merged coverage gate
│
├── docs/                # Design documents and retrospectives
│
├── ARCHITECTURE.md      # Comprehensive architecture documentation
├── CLAUDE.md           # Claude Code guidance
├── AGENTS.md           # Contributor guide
└── build.cmd           # Main build script
```

**Component Statistics**:
- **RailUI**: 24 source files, ~12,000 LOC
- **RailCore**: 3 implementation files, 11 header files, ~890 LOC
- **Tests**: 382+ test cases (2 smoke + 380 comprehensive)
- **Build Time**: ~3 minutes (CI), ~60-100 seconds (local Debug)
- **Memory Footprint**: ~10 MB peak (including framework)

### Tests & Coverage

**Test Infrastructure**:
- **382+ Test Cases**: 2 console smoke tests + 380 comprehensive GoogleTest cases
- **163 Test Files**: Systematic coverage of validation, engine behavior, edge cases
- **90%+ Line Coverage**: Enforced via CI with OpenCppCoverage
- **Coverage Target**: 98% (Phase 1 complete: +31 tests targeting 93-94%)

**Coverage Gates** (CI enforced):
- Single-report coverage (RailCoreTests): ≥90% line coverage
- Merged coverage (RailCoreTests + RailCoreGTest): ≥90% line coverage

**Running Tests**:
```cmd
# Local test runner
tools/run_tests.ps1 -Config Debug -Coverage -MinimalSmoke

# Build with coverage
set BUILD_GTEST=1
set COVERAGE=1
build.cmd Debug

# Check coverage gates
tools\coverage_gate.ps1 -CoberturaXml build\msvc\Debug\coverage.xml -MinLineRate 0.90
tools\coverage_merge_gate.ps1 -CoberturaXmlA build\msvc\Debug\coverage.xml -CoberturaXmlB build\msvc\Debug\coverage_gtest.xml -MinLineRate 0.90
```

**Test Organization**:
- **Console Smoke Tests**: `build/msvc/RailCoreTests.vcxproj` (minimal mode default)
  - Set `RAILCORE_SMOKE_MODE=minimal` for quick tests
  - Define `RAILCORE_ENABLE_FULL_SMOKE` for legacy full suite
  - Set `RAILCORE_TEST_OUTDIR` to control temp file location
- **GoogleTest Suites**: `build/msvc/RailCoreGTest.vcxproj`
  - Build locally with `BUILD_GTEST=1` (requires NuGet)
  - Runs automatically in CI
  - Output: `build/msvc/Debug/gtest-results.xml`
- **Coverage Analysis**: OpenCppCoverage via `tools/coverage.cmd`
  - Generates Cobertura XML format
  - Excludes UI layer (focuses on RailCore backend)

**Recent Coverage Improvements** (Phase 1):
- ✅ Config limit enforcement tests (8 tests)
- ✅ Telemetry emission tests (3 tests)
- ✅ Parser helper edge cases (9 tests)
- ✅ Boundary value tests (11 tests)
- 📈 Coverage gain: +3-4% (90% → 93-94%)

See `wrk_docs/2025.01.06 - CC - Coverage Analysis Report.md` for detailed coverage analysis and improvement plan.

### Command-line Validation
- You can validate one or more `.RCD` files from the command line without launching the UI:
  - Single file: `build\msvc\Debug\railc.exe --rcd-validate "Game files\FAST.RCD"` → prints `Valid` and exits `0`.
  - Bad file: `build\msvc\Debug\railc.exe --rcd-validate bad_tt_arrsel.rcd` → prints a one-line error and exits `1`.
  - Wildcard/dir: `build\msvc\Debug\railc.exe --rcd-validate "Game files\*.RCD"` or `--rcd-validate "Game files"`.
- Notes:
  - Accepts `--rcd-validate`, `-rcd-validate`, or `/rcd-validate` (case-insensitive).
  - For GUI subsystem behavior, best-effort console attach is used; output may be invisible if launched from Explorer. Exit codes are authoritative.
  - Append `--print-id` to include the layout SHA-256 identifier in success lines (single file: `Valid (id=...)`; multiple files: `Valid: <path> (id=...)`).
  - Mirrored logging is env-var only: set `RCD_CLI_LOG=path` to duplicate output for automation (tests/CI rely on it); there is no CLI flag for logging.

### Architecture

See **[ARCHITECTURE.md](ARCHITECTURE.md)** for comprehensive architecture documentation including:

- **System Architecture Overview** - Component diagrams, dependency graphs
- **Core Components Deep Dive** - RailCore engine, RailUI layer, state machines
- **Data Flow Diagrams** - Startup, game session, route selection
- **RCD File Format Specification** - Complete format details with validation rules
- **Build & Test Infrastructure** - Build system, dual-layer testing, coverage gates
- **Design Patterns** - 8 patterns identified: Observer, Command, State, RAII, etc.
- **Performance Characteristics** - Memory footprint, rendering costs, build times
- **Extension Points** - Examples of custom repositories, telemetry, observers

**Key Architectural Documents**:
- `ARCHITECTURE.md` - Master architecture document (700+ lines)
- `CLAUDE.md` - Claude Code integration guide
- `AGENTS.md` - Contributor expectations (style, build/test, PR process)
- `docs/modern_toolchain_migration_plan.md` - Modernization roadmap
- `docs/` - Debugging journals and design retrospectives

### RCD File Format

Railway Control Data (RCD) files use a plain-text INI-style format with 9 required sections:

```ini
[GENERAL]       # StartTime, StopTime (simulation window)
[SECTIONS]      # Track sections (ID, quad coordinates)
[OVERLAPPING]   # Conflict zones (section pairs)
[PLATFORMS]     # Station platforms (ID, coordinates)
[SELECTOR]      # Signal/point controls (ID, type, label)
[ROUTES]        # Train paths (ID, from, to, 6 clearing stages)
[LOCOS]         # Locomotive roster (ID, class, number)
[LOCOYARD]      # Yard configuration (enabled or disabled)
[TIMETABLE]     # Train schedules (ID, times, routes, delays)
```

**Features**:
- CSV-style data format within sections
- Comprehensive validation with detailed error messages
- Cross-reference integrity checking
- SHA-256 content identification (64-char hex, whitespace-insensitive)
- Stage encoding: `primary + (secondary * 1000)` for overlapping sections

**ID Ranges**:
- Sections/Routes: 1-999 (must be unique)
- Locomotives/Timetables: 1-499
- Arrival Selectors: 1-49 (input selectors only)

### CI/CD Pipeline

The GitHub Actions workflow (`.github/workflows/msvc.yml`) provides comprehensive validation:

**Build Matrix**: Debug + Release configurations
**Validation Steps**:
1. Build OWLNext dependencies
2. Build RailControl, RailCore, RailCoreTests
3. Lint memory guard usage
4. Run console smoke tests (Debug only)
5. Command-line RCD validation (good/bad/wildcard)
6. Install OpenCppCoverage
7. Run coverage analysis (RailCoreTests)
8. Build and run RailCoreGTest suite
9. Run coverage analysis (RailCoreGTest)
10. Enforce 90% coverage gates (hard fail)

**Artifacts**:
- RCD CLI validation logs
- Cobertura coverage XML (single + merged)
- GTest results XML

### Additional Documentation







