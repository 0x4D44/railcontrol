# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RailControl is a Windows railway simulation application built with C++ and the OWL (ObjectWindows Library) framework. The project was successfully migrated from OWL 5.2 to OWLNext 6.30 in 2025, maintaining full backward compatibility while enabling modern framework features.

## Build Commands

### Primary Build Method
- **Streamlined Build**: `build.bat` - **RECOMMENDED** - Single command builds entire application
  - Compiles all source files in optimized batches
  - Builds Windows resources
  - Links executable with proper libraries
  - Provides detailed progress reporting
  - Handles errors gracefully

### Alternative Build Methods  
- **Production Build**: `build_final.bat` - Legacy build with detailed status output
- **Simple Build**: `build_simple.bat` - Legacy clean build with error checking
- **Make Build**: `make -f railc.mak` - Traditional makefile approach
- **Manual Build**: Individual compilation with `bcc32 +BccW32_simple.cfg -c [source].cpp`

### Build Requirements
- **Compiler**: Borland C++ 5.02 at `C:\Apps\BC5`
- **Framework**: OWLNext 6.30 at `C:\Apps\owlnx630`
- **Path Setup**: `set path=c:\apps\bc5\bin;%path%`

### Build Configurations
- `BccW32_simple.cfg` - Simplified configuration for avoiding command line length issues
- `BccW32_noautolink.cfg` - Configuration without auto-linking
- `link_clean.rsp`, `link_fixed.rsp` - Linker response files with different library configurations

## Architecture Overview

### Core Application Structure
- **Main Application**: `TManager` class extending `TApplication`
- **Main Window**: `TMainWindow` class extending `TFrameWindow` - central UI controller
- **Railway Simulation**: Multi-window system with arrivals, departures, platforms, and locomotive yard

### Key Components
- **Layout System**: `TLayout` class handles railway track visualization and train movement
- **Train Management**: Comprehensive system tracking train states (30+ status codes from "Due" to "Departed")
- **UI Components**: Toolbar, status bar, multiple child windows for different operational views
- **Resource Management**: Graphics (BMP), sounds (WAV), help system (HLP), and Windows resources

### Business Logic Modules
- **ARRIVALS.CPP/H**: Train arrival handling and display
- **DEPARTUR.CPP/H**: Train departure management 
- **PLATFORM.CPP/H**: Platform operations and train-platform interactions
- **LOCOYARD.CPP/H**: Locomotive yard operations, maintenance, refueling
- **LAYOUT.CPP/H**: Core railway layout rendering and train movement simulation
- **TIMETABL.CPP/H**: Timetable management and scheduling

### Railway Data
- **Stock Types**: 22 different locomotive/rolling stock types (HST, EMU, DMU, freight, etc.)
- **Railway Layouts**: Multiple pre-defined layouts (FAST.RCD, KINGSX.RCD, QUEENST.RCD, WAVERLY.RCD)
- **Train States**: 30+ status codes tracking complete lifecycle from arrival to departure

## Development Patterns

### OWL Framework Usage
- Uses OWLNext 6.30 with comprehensive compatibility modes:
  - `OWL1_COMPAT` - Legacy API support
  - `OWL2_COMPAT` - OWL 2.x compatibility  
  - `OWL5_COMPAT` - OWL 5.x compatibility
- Message handling via `DECLARE_RESPONSE_TABLE` macros
- Friend class relationships for inter-component communication

### Code Conventions
- Hungarian notation for variable naming (`PStatbar`, `HBRUSH`, etc.)
- Extensive use of Windows API types (`HWND`, `HDC`, `HBITMAP`)
- Debug tracing system with `TRC_*` macros (controlled by `MDDEBUG`)
- Resource management with explicit handle tracking

### File Organization  
- **Headers**: `.H` files contain class definitions and constants
- **Implementation**: `.CPP` files contain method implementations
- **Resources**: `RESOURCE/` directory contains UI resources, bitmaps, sounds
- **Help**: `HELP/` directory contains help files and documentation
- **Data**: `.RCD` files contain railway layout definitions

## Migration Status

This codebase represents a successful legacy modernization:
- **Framework Migration**: OWL 5.2 → OWLNext 6.30 completed with zero compilation errors
- **Toolchain**: Maintained Borland C++ 5.02 for stability
- **Compatibility**: Full backward compatibility preserved via compatibility modes
- **Future Path**: Positioned for further migration to modern compilers/frameworks

The migration demonstrates effective legacy code modernization while preserving complete business functionality.