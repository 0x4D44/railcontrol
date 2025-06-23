# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a legacy railway station control simulator written in C++ using Borland OWL (ObjectWindows Library) for Windows 3.1/Windows 95. The application simulates real-time train operations, allowing users to control train movements, routing, and timetabling at railway stations.

## Build Commands

**Primary Build System**: Borland C++ 4.5 with OWL framework
- **Build**: `make -f railc.mak` (requires Borland C++ 4.5 in PATH)
- **Environment Setup**: `set path=c:\apps\bc4\bin;%path%`
- **Output**: `railc.exe` (Windows executable)

**Debug Build**: Uncomment debug options in `railc.mak`:
```makefile
TLINK32 = TLink32 -v
BCC32   = Bcc32 +BccW32.cfg -v
```

## Architecture Overview

### Core Application Structure
- **TManager**: Main application class (inherits from TApplication)
- **TMainWindow**: Primary window managing the entire simulation
- **TLayout**: Core simulation engine and track display
- **Multiple specialized windows**: TArrivals, TDepartur, TPlatform, TLocoyard

### Data Architecture
- **RCD files**: Railway Control Data format using INI-like sections
- **22-state train lifecycle**: Comprehensive state machine for train movements
- **Route system**: Interactive selector-based route control with conflict detection
- **Timetabling**: Real-time schedule following with delay calculations

### Key File Relationships
- `classdef.h`: Master include file with all dependencies
- `railc.h/.cpp`: Main application entry point
- `layout.h/.cpp`: Core simulation engine
- `general.h`: Status codes, constants, and shared definitions
- `section.h/.cpp`: Track section geometry and state
- `timetabl.h/.cpp`: Train scheduling and locomotive assignment

## Data File Format (RCD)

The application uses RCD files with INI-like structure:
```ini
[SELECTOR]     ; Interactive route control buttons
[SECTIONS]     ; Track geometry (4-point polygons)
[TIMETABLE]    ; Train schedules and routing
[ROUTES]       ; Valid route combinations
[PLATFORMS]    ; Platform configurations
[LOCOS]        ; Available locomotive fleet
```

## Train State System

The application implements a 22-state train lifecycle:
- **States 1-5**: Approach and arrival preparation
- **States 6-11**: Arrival sequence (A-F)
- **State 12**: In platform
- **States 13-15**: Departure preparation
- **States 16-22**: Departure sequence (A-F)
- **State 30**: Special twin unit operations

## Code Conventions

### Legacy C++ Patterns
- **Hungarian notation**: Extensive use (e.g., `PMainWindow`, `LPSTR`)
- **Friend classes**: Inter-window communication via friend relationships
- **Manual resource management**: GDI objects (brushes, pens, fonts)
- **Fixed-size arrays**: Hardcoded limits throughout (1000 sections, 500 timetable entries)

### OWL Framework Specifics
- **Message response tables**: `DECLARE_RESPONSE_TABLE` macros
- **Window class hierarchy**: All windows inherit from TWindow/TFrameWindow
- **Event handling**: Method naming convention (e.g., `EvSize`, `EvMove`)

## Key Constants and Limits

```cpp
#define NOTOOLBUTTONS  1      // Number of toolbar buttons
#define NOSELECTORS    17     // Number of route selectors
#define INIFILENAME    "RAILC.INI"
#define HELPFILENAME   "RAILC.HLP"
#define APPNAME        "Rail Control"
```

## Working with the Simulation

### Understanding Train Operations
1. **Timetable Entries**: Each train has arrival/departure times, platform assignments, locomotive requirements
2. **Route Setting**: Users click selectors to set routes; system validates conflicts
3. **Locomotive Management**: Automatic assignment based on train class requirements
4. **Delay Tracking**: Real-time delay calculations for performance monitoring

### Modifying Simulation Logic
- **Train behavior**: Modify state transitions in layout engine
- **Route logic**: Update route validation in selector handling
- **Display updates**: Understand friend class relationships for UI updates
- **New features**: Consider impact on 22-state train lifecycle

## Development Notes

### Legacy Considerations
- **16-bit origins**: Originally designed for Windows 3.1
- **Borland-specific**: Uses Borland C++ extensions and OWL framework
- **Fixed memory model**: Assumes specific memory layout and limits
- **No STL**: Uses manual memory management and C-style arrays

### Debugging Support
- **Conditional compilation**: `#ifdef MDDEBUG` for debug builds
- **Trace arrays**: `lDbgStates[90]` for state tracking
- **Resource files**: Includes bitmap and sound resources

## Testing Approach

**No automated testing framework** - this is a legacy GUI application requiring manual testing:
- Test with various RCD files (FAST.RCD, KINGSX.RCD, QUEENST.RCD, WAVERLY.RCD)
- Verify train state transitions through complete lifecycle
- Test route conflict detection with overlapping routes
- Validate timetable parsing and locomotive assignment

## File Organization

- **Main source**: `*.cpp` and `*.h` files in root directory
- **Resources**: `RESOURCE/` directory (bitmaps, sounds, RC files)
- **Help system**: `HELP/` directory with compiled help files
- **Backup files**: `*.BAK` files (can be ignored)
- **Compiled objects**: `*.obj` files (build artifacts)
- **Legacy backup**: `OWLBACK/` directory contains older versions