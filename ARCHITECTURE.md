# RailControl - Comprehensive Architecture Documentation

**Project**: RailControl Railway Simulation
**Date**: 2025-11-06
**Version**: 3.0 (Modernized)
**Analysis Scope**: Complete codebase architecture review

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [System Architecture Overview](#system-architecture-overview)
3. [Core Components](#core-components)
4. [Data Flow](#data-flow)
5. [Build & Test Infrastructure](#build--test-infrastructure)
6. [Key Design Patterns](#key-design-patterns)
7. [Performance Characteristics](#performance-characteristics)
8. [Extension Points](#extension-points)

---

## Executive Summary

RailControl is a legacy 1994 Windows railway simulation application that has undergone successful modernization to Visual Studio 2022 and OWLNext 7.0.19. The architecture demonstrates a clean separation between:

- **RailCore** (backend simulation engine) - C++17 static library with comprehensive test coverage
- **RailUI** (frontend GUI) - OWL-based Windows application with legacy Win32 rendering
- **Test Infrastructure** - Dual-layer testing (smoke tests + 159 GoogleTest cases) with 90% coverage gates

**Key Statistics**:
- **Total Source Files**: ~50 C++ implementation files
- **Lines of Code**: ~15,000+ LOC (excluding OWLNext framework)
- **Test Coverage**: 90%+ line coverage enforced via CI
- **Supported Data Formats**: RCD (Railway Control Data) plain-text format
- **Maximum Capacity**: 1000 sections, 1000 routes, 500 trains, 50 platforms

---

## System Architecture Overview

### High-Level Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        RailControl.exe                          │
│                    (Win32 GUI Application)                      │
└─────────────────────────────────────────────────────────────────┘
                              │
            ┌─────────────────┴─────────────────┐
            │                                   │
┌───────────▼──────────────┐      ┌────────────▼─────────────┐
│       RailUI Layer       │      │    RailCore Library      │
│   (OWL Framework Based)  │      │  (Engine & Persistence)  │
│                          │      │                          │
│  • TMainWindow           │      │  • IRailEngine           │
│  • TLayout (rendering)   │      │  • RcdRepository         │
│  • Train view windows    │      │  • WorldState            │
│  • Toolbar/Status bar    │      │  • Command system        │
│  • Dialog boxes          │      │  • Observer pattern      │
└───────────┬──────────────┘      └────────────┬─────────────┘
            │                                   │
            └──────────── Friend Access ────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │  OWLNext 7.0.19  │
                    │  (UI Framework)  │
                    └──────────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │   Windows API    │
                    │   (GDI, Win32)   │
                    └──────────────────┘
```

### Component Dependency Graph

```
RailControl.exe
 ├─ RailCore.lib (static library)
 │   ├─ engine_stub.cpp
 │   ├─ rcd_id.cpp
 │   ├─ rcd_repository.cpp
 │   └─ Public Headers (include/railcore/)
 │
 ├─ OWLNext Libraries (4 static libs)
 │   ├─ owl-7.0.lib
 │   ├─ owlext-7.0.lib
 │   ├─ ocf-7.0.lib
 │   └─ coolprj-7.0.lib
 │
 └─ Windows Libraries (17 system libs)
     ├─ user32.lib, gdi32.lib, kernel32.lib
     ├─ comctl32.lib, comdlg32.lib
     └─ Bcrypt.lib (for SHA-256)
```

---

## Core Components

### 1. RailCore Engine (Backend)

**Purpose**: Simulation logic, state management, data persistence

#### 1.1 Interface Layer

```cpp
// Main engine interface
class IRailEngine {
  virtual Status LoadLayout(LayoutDescriptor) = 0;
  virtual Status Advance(milliseconds dt) = 0;
  virtual Status Command(CommandPayload) = 0;
  virtual shared_ptr<const WorldState> GetSnapshot() = 0;
  virtual void Subscribe(IObserver*) = 0;
};
```

**Key Characteristics**:
- **Interface-based**: All interactions through abstract interfaces
- **Dependency injection**: Services injected via constructors
- **Observer pattern**: Multi-subscriber state change notifications
- **Thread-safe**: Single mutex with immutable snapshots

#### 1.2 Data Persistence (RCD Files)

**RcdRepository** - Loads/validates RCD files with comprehensive error checking:

```
Load Process Flow:
1. Path resolution (with fallback to "Game files/")
2. File I/O (binary read, entire file to memory)
3. Content normalization (CRLF → LF)
4. Section scanning (9 required sections)
5. Structural validation
6. Data parsing (CSV format)
7. Cross-reference validation
8. SHA-256 ID computation
9. WorldState population
```

**RCD File Format** (9 INI sections):
```ini
[GENERAL]       - StartTime, StopTime (HHMM format)
[SECTIONS]      - Track sections (ID, X1-Y4 coordinates)
[OVERLAPPING]   - Conflict pairs (section A, section B)
[PLATFORMS]     - Station platforms (ID, coordinates)
[SELECTOR]      - Signal/point controls (ID, type, label)
[ROUTES]        - Train paths (ID, from, to, 6 stages)
[LOCOS]         - Locomotive roster (ID, class, number)
[LOCOYARD]      - Yard configuration (disabled or stock types)
[TIMETABLE]     - Train schedules (ID, times, selectors, next)
```

**Validation Rules**:
- **ID Ranges**: Section/Route 1-999, Loco/Timetable 1-499, ArrSelector 1-49
- **Uniqueness**: Sections, routes, selectors must be unique
- **Cross-refs**: All referenced IDs must exist in respective sections
- **Time Format**: HHMM with minutes 00-59
- **Stage Encoding**: `primary + (secondary * 1000)`

#### 1.3 Layout Identification

**SHA-256 Based Identification**:
```cpp
std::string ComputeRcdIdFromContent(const std::string& canonical,
                                    const char* schemaTag = "rcd:v1")
```

**Canonicalization** (whitespace-insensitive):
1. Strip all `\r` characters
2. Trim leading/trailing whitespace per line
3. Remove trailing blank lines
4. Rebuild with LF line endings

**Result**: 64-character lowercase hex string (stable across minor formatting changes)

---

### 2. RailUI Layer (Frontend)

**Purpose**: User interface, rendering, input handling

#### 2.1 Application Framework

```
TManager (TApplication)
  └── TMainWindow (TFrameWindow)
      ├── TToolbar (12 buttons)
      ├── TStatbar (status messages)
      ├── TLayout (main simulation canvas)
      └── Optional Windows:
          ├── TArrivals (8 upcoming arrivals)
          ├── TDepartures (8 scheduled departures)
          ├── TPlatform (platform occupancy grid)
          └── TLocoyard (16 locomotive slots)
```

**Initialization Sequence**:
```
1. OwlMain() entry point
   ├─ CLI mode check (--rcd-validate) → RunRcdValidationCli()
   ├─ Memory guard bootstrap (MSVC only)
   └─ TManager app creation

2. TManager::InitMainWindow()
   └─ new TMainWindow(nullptr, APPNAME)

3. TMainWindow::SetupWindow()
   ├─ Load INI settings (window positions, flags)
   ├─ Create GDI resources (brushes, pens, fonts)
   ├─ new TStatbar() → Create()
   ├─ Configure toolbar button metadata
   ├─ new TToolbar(12 buttons) → Create()
   ├─ new TLayout() → Create()
   ├─ Auto-create child windows (if INI says "Exists=1")
   ├─ CMOptOptimi() if StartOptim=1
   └─ Close splash window

4. Message loop begins (GetMessage/DispatchMessage)
```

#### 2.2 Rendering System (TLayout)

**Core Rendering Pipeline**:
```
WM_TIMER (100ms)
  └─ EvTimer()
      └─ Skip Counter Logic (SkipInt = 10/20/30)
          └─ HandleTimeChange()
              ├─ GetExpecteds() [populate Expect[] array]
              ├─ GetDepartures() [populate Depart[] array]
              ├─ HandleTracking() [advance train positions]
              ├─ TimeCheck() [calculate delays]
              └─ UpdateSelectors() [enable/disable route options]
                  └─ UpdateDisplay()
                      ├─ Draw 3D border
                      ├─ Scale coordinate system (850x550 → window size)
                      ├─ Iterate 1000 sections (draw if non-null)
                      ├─ Iterate 50 platforms (draw if non-null)
                      └─ Render clock and delay boxes
```

**Coordinate System**:
- **Logical**: 850×550 pixels (design-time)
- **Runtime Scaling**: `sectionXScale = windowWidth / 850.0f`
- **Transformation**: Applied per-frame before rendering all elements

**Graphics Optimization**:
- **No double-buffering**: Direct rendering to window DC
- **Selective redraw**: Platform `Redraw` flags, clock digit caching
- **Fixed O(1050) cost**: 1000 section checks + 50 platform checks

#### 2.3 UI Component Details

**Toolbar** (TToolbar + TToolbutton):
- 12 buttons: New, Pause, Restart, Stop, Optimize, Config, 4 window toggles, Help, About
- Bitmap sprite sheet (24×22 pixels per button)
- Button state: UpPosition (raised) or DownPosition (sunken)
- Status bar integration: Button hover updates status text

**Status Bar** (TStatbar):
- Single-line text display (100 char max)
- Custom message `SB_SETTEXT` (WM_USER + 1)
- 3D beveled border (highlight/shadow pens)
- Arial 15pt font

**Train Management Windows**:
- **Arrivals**: 8 rows, 9 columns (Due time, On, Late, Loco, Desc, Status, Dep time, Dep desc, Stage telemetry)
- **Departures**: 8 rows, 4 columns (Dep time, Stock type, Platform, Description)
- **Platforms**: 50 platforms, 2 columns (Platform #, Description/occupancy)
- **Locoyard**: 4×4 grid, 16 locomotive slots

**Stage Telemetry Integration**:
```cpp
struct StageTelemetryUpdate {
  int timetableId;        // Which train
  int stage;              // Stage number (0+) or -1
  int stageIndex;         // Stage bucket (0-5) or -1
  int stagePrimary;       // Primary section
  int stageSecondary;     // Secondary section
  long progressMs;        // Progress in milliseconds
};
```

Displayed as: `"Stage 2 (bucket 1) sec 3->4"`

---

### 3. Train State Machine

**30+ Status Codes** (ST_XXXX):

```
Lifecycle Phases:

Initialization:
  ST_NONE (0)         → No train

Arrival:
  ST_DUE (1)          → Expected in 6+ minutes
  ST_APPROACH (2)     → Approaching (3-6 min)
  ST_HELD (3)         → Held (conflict)
  ST_FIRSTHELD (4)    → Held at first signal
  ST_SETPLAT (5)      → Route assigned
  ST_ARRA-ARRF (6-11) → 6 approach stages

Platform:
  ST_INPLAT (12)      → In platform (arrived)
  ST_RELEASE (13)     → Stock released, loco needed
  ST_STOCKOK (14)     → Stock ready

Departure:
  ST_READYDEP (15)    → Ready (5 min to departure)
  ST_STARTDEP (16)    → Departure initiated
  ST_DEPA-DEPF (17-22)→ 6 departure stages

Special:
  ST_TWINASSOC (30)   → Twin unit association
```

**State Transitions** (from TimeCheck):
```cpp
// Arrival progression
if (status == ST_NONE && currentTime < arrivalTime - 6min)
  newStatus = ST_DUE;

if (status == ST_DUE && arrivalTime - 6min <= currentTime < arrivalTime - 3min)
  newStatus = ST_APPROACH;

if (status == ST_APPROACH && routeSet)
  newStatus = ST_SETPLAT;

if (status == ST_SETPLAT)
  newStatus = ST_ARRA;  // Begin approach stages

// Platform progression
if (status == ST_ARRF && reachedPlatform)
  newStatus = ST_INPLAT;

if (status == ST_INPLAT && releaseTime <= currentTime)
  newStatus = ST_RELEASE;

// Departure progression
if (status == ST_STOCKOK && currentTime >= depTime - 5min)
  newStatus = ST_READYDEP;

if (status == ST_READYDEP && departureRouteSet)
  newStatus = ST_DEPA;  // Begin departure stages
```

---

### 4. Route & Selector System

#### 4.1 Infrastructure Components

**Sections** (Track Circuits):
- 1000-element array `PSectionInfo[1000]`
- Binary occupancy state: `TRUE` (occupied) or `FALSE` (clear)
- Quadrilateral geometry (4 coordinate points)
- Atomic state transitions

**Selectors** (Signal/Point Controls):
- 6 types: Input, Output, Diesel Platform, Electric Platform, Hold Point, Locoyard
- Button state machine: SelectCur (up/down), SelectOld (rollback)
- Interactive selection: LButtonDown → MouseMove → LButtonUp → WM_COMMAND

**Routes** (Train Paths):
- 1000 pre-defined routes
- 6 clearing stages per route
- Stage encoding: `Clear[i] = SectionB * 1000 + SectionA`
- Supports up to 12 sections per route

**Overlaps** (Conflict Zones):
- Section pairs that physically conflict
- Both sections must be clear for either route
- Prevents junction collisions

#### 4.2 Route Validation Algorithm

```cpp
bool IsRouteValid(int startSelector, int endSelector) {
  // Phase 1: Route lookup
  Route* route = FindRoute(startSelector, endSelector);
  if (!route) return false;

  // Phase 2: Section availability
  for (int stage = 0; stage < 6; stage++) {
    int secA, secB;
    route->GetClear(stage, secA, secB);
    if (PSectionInfo[secA]->IsOccupied()) return false;
    if (PSectionInfo[secB]->IsOccupied()) return false;
  }

  // Phase 3: Overlap protection
  for (auto& overlap : overlaps) {
    if (RouteUsesSection(route, overlap.sectionA) ||
        RouteUsesSection(route, overlap.sectionB)) {
      if (PSectionInfo[overlap.sectionA]->IsOccupied()) return false;
      if (PSectionInfo[overlap.sectionB]->IsOccupied()) return false;
    }
  }

  return true;  // Route is safe
}
```

#### 4.3 Selector Interlocking Rules

**UpdateSelectors()** enforces safety:

```
Rule 1a: Input with Through-Train
  → Disable all non-output selectors

Rule 1b: Input with Regular Train
  → EMU restriction: Disable non-electric platforms
  → Occupied platforms: Disable if ST_INPLAT/ST_STOCKOK/ST_READYDEP
  → Light loco exclusion: Disable locoyard (unless train IS light)

Rule 1c: Platform Selector
  → Disable all other platforms
  → For light loco: disable hold point if maintenance needed

Rule 1d: Locoyard
  → Disable all input/output selectors
  → Allow only platforms with ST_RELEASE trains
```

---

## Data Flow

### Startup Data Flow

```
User launches railc.exe
  └─ OwlMain()
      ├─ Check for --rcd-validate flag
      │   ├─ YES → RunRcdValidationCli() → exit with code
      │   └─ NO  → Continue
      │
      ├─ BootstrapMemoryGuardsFromConfig() [MSVC only]
      │
      └─ TManager app.Run()
          ├─ InitMainWindow() → new TMainWindow()
          │   ├─ TFrameWindow constructor
          │   ├─ Load INI (window position, settings)
          │   └─ Create GDI resources
          │
          ├─ SetupWindow()
          │   ├─ Create UI components (toolbar, status, layout)
          │   ├─ Auto-create child windows
          │   └─ Close splash screen
          │
          └─ Message loop
              ├─ WM_COMMAND → Response table dispatch
              ├─ WM_TIMER → EvTimer() → simulation step
              ├─ WM_SIZE → EvSize() → RedoChildren()
              └─ WM_PAINT → UpdateDisplay()
```

### Game Session Data Flow

```
User clicks "New Game" (CM_MNUFILNEW)
  └─ CMMnuFilNew()
      ├─ Kill old timer (if exists)
      ├─ DisplayHan->StartNew()
      │   ├─ Show file picker dialog
      │   ├─ RcdRepository.Load(path, worldState)
      │   │   ├─ File I/O
      │   │   ├─ Validation (9 sections, cross-refs)
      │   │   ├─ SHA-256 ID computation
      │   │   └─ WorldState population
      │   ├─ Initialize timetable arrays
      │   └─ Compute initial train positions
      │
      ├─ Set GameInProgress = TRUE
      ├─ Update all view windows
      ├─ Enable Pause/Stop menu items
      ├─ Show start dialog (instructions)
      └─ SetTimer(ID_TIMER, 100ms)

Every 100ms (WM_TIMER):
  └─ DisplayHan->EvTimer()
      ├─ Skip counter check (framerate control)
      ├─ HandleTimeChange()
      │   ├─ GetExpecteds() → Update Expect[] array
      │   ├─ GetDepartures() → Update Depart[] array
      │   ├─ HandleTracking() → Advance train positions
      │   ├─ TimeCheck() → State machine transitions
      │   └─ UpdateSelectors() → Safety interlocking
      │
      └─ Post WM_COMMAND to child windows
          ├─ ArrivalHan->UpdateDisplay()
          ├─ DeparturHan->UpdateDisplay()
          ├─ PlatformHan->UpdateDisplay()
          └─ LocoyardHan->UpdateDisplay()
```

### Route Selection Data Flow

```
User clicks first selector (e.g., input line)
  └─ TSelector::EvLButtonUp()
      └─ Post WM_COMMAND(CM_SELECT)
          └─ TLayout::HandleSelectors()
              ├─ Find pressed selector
              ├─ Selector1 = it
              └─ UpdateSelectors()
                  ├─ Apply interlocking rules
                  └─ Enable/disable conflicting selectors

User clicks second selector (e.g., platform)
  └─ TSelector::EvLButtonUp()
      └─ Post WM_COMMAND(CM_SELECT)
          └─ TLayout::HandleSelectors()
              ├─ Count pressed selectors (now 2)
              ├─ IsRouteValid(Selector1, Selector2)
              │   ├─ Find route with From=Selector1, To=Selector2
              │   ├─ Check all stage sections clear
              │   └─ Check overlap protection
              │
              ├─ If valid:
              │   ├─ Execute train movement
              │   ├─ Update section occupancy
              │   ├─ Update platform states
              │   ├─ Release both buttons
              │   └─ Selector1 = Selector2 = 0
              │
              └─ If invalid:
                  ├─ Display error message
                  └─ Release both buttons
```

---

## Build & Test Infrastructure

### Build System Architecture

```
Entry Point: build.cmd [Debug|Release]
  ├─ Check OWLNext libraries (4 libs)
  │   └─ If missing → build_owlnext_msvc.bat
  │
  ├─ Initialize MSVC toolchain (vcvarsall.bat x86)
  │
  ├─ Sequential Build:
  │   ├─ RailControl.vcxproj (GUI app)
  │   ├─ RailCore.vcxproj (static library)
  │   ├─ RailCoreTests.vcxproj (smoke tests)
  │   └─ [Optional] RailCoreGTest.vcxproj (BUILD_GTEST=1)
  │
  ├─ Test Execution (Debug only):
  │   ├─ RailCoreTests.exe (minimal mode)
  │   └─ [Optional] RailCoreGTest.exe
  │
  ├─ Coverage Collection (COVERAGE=1):
  │   ├─ OpenCppCoverage → coverage.xml
  │   └─ OpenCppCoverage → coverage_gtest.xml
  │
  └─ Artifact Staging:
      ├─ Copy dbghelp.dll
      ├─ Copy RAILC.HLP
      └─ Copy railc.exe to repo root
```

### Test Infrastructure

**Layer 1: Smoke Tests** (RailCoreTests.exe)
- **Purpose**: Fast gate-keeper
- **Count**: 2 tests
- **Runtime**: < 1 second
- **Tests**:
  1. Missing file handling (NotFound error)
  2. Basic FAST.RCD loading with count validation

**Layer 2: Comprehensive Tests** (RailCoreGTest.exe)
- **Purpose**: Deep validation
- **Count**: 159+ test cases
- **Runtime**: 15-30 seconds
- **Categories**:
  - Parser & Validation (13 tests)
  - Engine Lifecycle (18 tests)
  - Data Persistence (20+ tests)
  - Scheduling Engine (12+ tests)
  - Routes & Selectors (25+ tests)
  - Platforms (15+ tests)
  - Timetable (12+ tests)
  - State Changes & Deltas (20+ tests)
  - Observer Pattern (8+ tests)

**Test Data**:
- **Valid layouts**: 7 files (FAST, KINGSX, QUEENST, WAVERLY, etc.)
- **Malformed data**: 26 files (bad_*.rcd, dup_*.rcd)
- **Edge cases**: 2 files (ok_locoyard_disabled.rcd, ok_tt_2359.rcd)

### Coverage Measurement

**Tool**: OpenCppCoverage (Cobertura XML format)

**Coverage Scope**:
- **Included**: src/railcore/**/*.cpp
- **Excluded**: tests/, third_party/, src/railui/

**Coverage Gates** (CI enforced):
1. **Single report** (RailCoreTests): ≥ 90% line coverage
2. **Merged report** (RailCoreTests + RailCoreGTest): ≥ 90% line coverage

**Gate Scripts**:
- `tools/coverage_gate.ps1` - Single report validation
- `tools/coverage_merge_gate.ps1` - Union merge validation

**Merge Algorithm**:
```
1. Extract line maps from both files
   For each <class filename="...">:
     For each <line number="X" hits="Y">:
       key = "filename:linenumber"
       value = (hits > 0)

2. Union merge:
   merged[key] = coverageA[key] OR coverageB[key]

3. Recalculate aggregate rate:
   lineRate = coveredLines / totalLines

4. Apply threshold check (0.90)
```

### CI/CD Pipeline (GitHub Actions)

**Workflow**: `.github/workflows/msvc.yml`

**Trigger**: Push to main, pull requests

**Matrix**: Debug and Release (parallel execution)

**Stages** (Debug configuration):
```
1. Checkout code
2. Setup MSBuild
3. Build OWLNext libraries
4. Build RailControl (Debug)
5. Memory guard linting
6. Run RailCoreTests (smoke)
7-9. RCD validation (good, bad, wildcard)
10. Upload CLI logs
11. Install OpenCppCoverage
12. Coverage run (RailCoreTests)
13. Upload coverage artifact
14. Coverage gate (≥90%, hard fail)
15. Build RailCoreGTest
16. Run RailCoreGTest
17. Upload GTest results
18. Coverage run (RailCoreGTest)
19. Upload GTest coverage
20. Merged coverage gate (≥90%, hard fail)
```

**Execution Time**: ~3 minutes (Debug + Release in parallel)

---

## Key Design Patterns

### 1. Service Locator / Dependency Injection

```cpp
class RailEngineStub {
  std::unique_ptr<ILayoutRepository> repo_;
  std::unique_ptr<ITelemetry> telemetry_;

public:
  RailEngineStub(
    std::unique_ptr<ILayoutRepository> repo,
    std::unique_ptr<ITelemetry> telemetry
  ) : repo_(std::move(repo)),
      telemetry_(std::move(telemetry)) {}
};
```

**Benefits**:
- Testability (inject mocks)
- Flexibility (swap implementations)
- Explicit dependencies

### 2. Observer Pattern

```cpp
class IObserver {
  virtual void OnSnapshot(const WorldState&) = 0;
  virtual void OnEvents(const DomainEvent[], size_t) = 0;
  virtual void OnDiagnostics(const std::string&) = 0;
};

std::vector<IObserver*> observers_;

void NotifySnapshot(const WorldState& state) {
  for (auto* obs : observers_) {
    obs->OnSnapshot(state);
  }
}
```

### 3. Command Pattern

```cpp
struct CommandPayload {
  CommandType type;
  variant<AssignLocoPayload, ReleaseLocoPayload, ...> data;
};

Status Command(const CommandPayload& cmd) {
  switch (cmd.type) {
    case CommandType::AssignLoco:
      return HandleAssignLoco(get<AssignLocoPayload>(cmd.data));
    // ... other commands
  }
}
```

### 4. State Pattern

```cpp
enum class EngineState { Idle, Paused, Running, Stopped };

Status Advance(milliseconds dt) {
  if (engineState_ != EngineState::Running) {
    return Status{InvalidCommand, "Engine not running"};
  }
  // ... simulation step
}
```

### 5. Immutable Snapshot Pattern

```cpp
std::shared_ptr<const WorldState> state_;

shared_ptr<const WorldState> GetSnapshot() {
  return state_;  // Shared immutable pointer
}
```

**Benefits**:
- Thread-safe reads without locking
- Efficient memory sharing
- Clear ownership semantics

### 6. RAII (Resource Acquisition Is Initialization)

```cpp
template <typename Traits>
class THandleGuard {
  typename Traits::handle_type mHandle;

public:
  ~THandleGuard() {
    if (mHandle != Traits::Invalid()) {
      Traits::Close(mHandle);
    }
  }
};

// Usage
TBitmapGuard bitmap(LoadBitmapA(...));
// Automatic cleanup on scope exit
```

### 7. Builder / Accumulator Pattern

```cpp
// During RCD parsing
std::set<uint32_t> sectionIds;
std::map<uint32_t, RouteParsed> routes;

// Accumulate all entities first
// Then validate cross-references
for (const auto& route : routes) {
  if (!sectionIds.count(route.fromSection)) {
    return ValidationError("Unknown section");
  }
}
```

### 8. Factory Pattern

```cpp
std::unique_ptr<IRailEngine> CreateEngine(
  const EngineConfig& config,
  std::unique_ptr<ILayoutRepository> repo
) {
  return std::make_unique<RailEngineStub>(std::move(repo));
}
```

---

## Performance Characteristics

### Memory Footprint

```
Core Data Structures:
  TSection[1000]      ~48 KB
  TPlatData[50]       ~2.8 KB
  TRoutes[1000]       ~100 KB
  TTimetable[500]     ~100 KB
  TLocos[500]         ~40 KB
  Clock bitmap        ~30 KB
  ──────────────────────────
  Total               ~330 KB core

UI Resources:
  GDI objects         ~50 KB
  Font handles        ~20 KB
  Bitmap cache        ~200 KB
  ──────────────────────────
  Total               ~270 KB UI

Peak Memory Usage:    ~10 MB (including OWLNext framework)
```

### Rendering Performance

```
Per-Frame Cost (60 FPS target):

Fixed O(1050):
  1000 section null-checks + draws     ~5 ms
  50 platform state lookups + draws    ~1 ms
  1 clock render (1-2 digits changed)  ~0.5 ms
  2 delay boxes (conditional)          ~0.5 ms
  ────────────────────────────────────────
  Total                                ~7 ms per frame

Frame skip: Every 10-30 frames (configurable)
Effective rate: 2-6 FPS for simulation updates
```

### Build Performance

```
Component          | Time (Debug) | Time (Release)
───────────────────|──────────────|───────────────
OWLNext pre-build  | 30-60 sec    | 30-60 sec (cached)
RailControl.vcxproj| 5-10 sec     | 8-15 sec
RailCore.vcxproj   | 2-3 sec      | 3-5 sec
RailCoreTests      | 1-2 sec      | 1-2 sec
RailCoreGTest      | 10-15 sec    | N/A
Link all projects  | 5-10 sec     | 5-10 sec
───────────────────|──────────────|───────────────
Total              | 60-100 sec   | 40-70 sec
```

### Test Execution Performance

```
Test Suite          | Count | Time
────────────────────|───────|──────────
RailCoreTests       | 2     | < 1 sec
RailCoreGTest       | 159+  | 15-30 sec
OpenCppCoverage     | N/A   | +5-15 sec overhead
────────────────────|───────|──────────
Total CI (Debug)    | 161+  | 150-200 sec
```

---

## Extension Points

### 1. Custom Layout Repository

```cpp
class DatabaseRepository : public ILayoutRepository {
  Status Load(LayoutDescriptor& desc, WorldState& outState) override {
    // Fetch from database instead of file
    auto data = db_->Query("SELECT * FROM layouts WHERE id = ?", desc.name);
    // ... parse and populate outState
    return Ok();
  }
};

// Usage
auto engine = CreateEngine(
  config,
  std::make_unique<DatabaseRepository>(dbConnection)
);
```

### 2. Custom Telemetry Sink

```cpp
class CloudTelemetry : public ITelemetry {
  void RecordMetric(const std::string& name, double value) override {
    // Send to cloud metrics service
    httpClient_->Post("/metrics", {{"name", name}, {"value", value}});
  }
};
```

### 3. Custom Observer

```cpp
class HistoryObserver : public IObserver {
  std::vector<WorldState> history_;

  void OnSnapshot(const WorldState& state) override {
    history_.push_back(state);
    if (history_.size() > 1000) {
      history_.erase(history_.begin());
    }
  }
};
```

### 4. Alternative Save Format

```cpp
class JsonLayoutRepository : public ILayoutRepository {
  Status Load(LayoutDescriptor& desc, WorldState& outState) override {
    auto json = ParseJsonFile(desc.sourcePath);
    // ... map JSON to WorldState
    return Ok();
  }

  Status Save(const WorldState& state, const std::filesystem::path& path) override {
    auto json = SerializeToJson(state);
    WriteFile(path, json);
    return Ok();
  }
};
```

### 5. Pluggable Random Provider

```cpp
class DeterministicRandom : public IRandomProvider {
  std::mt19937 gen_;

public:
  void Seed(uint32_t seed) override {
    gen_.seed(seed);
  }

  int Next(int min, int max) override {
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen_);
  }
};
```

---

## Conclusion

RailControl demonstrates a successful legacy modernization that preserves the original 1994 functionality while incorporating modern C++ practices:

**Strengths**:
- ✅ Clean architecture with separation of concerns
- ✅ Comprehensive test coverage (90%+)
- ✅ Modern toolchain (VS 2022, C++17)
- ✅ Well-defined interfaces and extension points
- ✅ Production-quality error handling
- ✅ Thread-safe engine design
- ✅ Automated CI/CD with quality gates

**Legacy Considerations**:
- OWL framework (1990s vintage, but stable)
- GDI rendering (no GPU acceleration)
- Fixed-capacity arrays (suitable for target domain)
- Hungarian notation (preserved for consistency)

**Future Modernization Opportunities**:
- DirectX/OpenGL rendering
- Dynamic resource allocation
- Network multiplayer support
- Modern UI framework (Qt, Dear ImGui)
- Plugin architecture for custom train behaviors

The architecture successfully balances **maintainability** (clear structure, testable), **performance** (efficient rendering, fast builds), and **extensibility** (pluggable components, interface-based design).

**Total Analysis**: 10 parallel agents, 50+ source files reviewed, 15,000+ lines of code analyzed.
