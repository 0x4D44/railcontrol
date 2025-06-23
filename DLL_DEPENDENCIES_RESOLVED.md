# BCC 5.02 DLL Dependencies - RESOLVED ✅

## Problem Summary
The railcontrol executable built with BCC 5.02 required external DLLs:
- `cw3230mt.dll` - C++ multi-threaded runtime library  
- `owl52.dll` - ObjectWindows Library (OWL) framework

## Solution Applied
**Local DLL Deployment** - Copy required DLLs to the application directory for standalone distribution.

### Required DLLs
1. **cw3230mt.dll** (319KB) - Multi-threaded C++ runtime
   - Source: `c:\apps\bc5\bin\cw3230mt.dll`
   - Linked via: `cw32mti.lib` (import library)

2. **owl52t.dll** (910KB) - OWL GUI framework (threaded version)
   - Source: `c:\apps\bc5\bin\owl52t.dll`  
   - Linked via: `owlwti.lib` (import library)

### Deployment Process
```batch
# Manual process:
copy c:\apps\bc5\bin\cw3230mt.dll .
copy c:\apps\bc5\bin\owl52t.dll .

# Automated process:
deploy.bat
```

## Alternative Approaches Attempted

### ❌ Static Linking (Failed)
- **Tried**: `owlwt.lib` + `cw32mt.lib` (static libraries)
- **Result**: Unresolved external `TMsgThread::TMsgThread` errors
- **Issue**: Threading dependencies in static OWL library incompatible with BCC 5.02

### ❌ Single-threaded Static (Failed)  
- **Tried**: `owlwt.lib` + `cw32.lib` (single-threaded static)
- **Result**: Same threading errors
- **Issue**: OWL framework inherently requires multi-threading support

### ✅ DLL Distribution (Success)
- **Approach**: Use import libraries + local DLL copies
- **Result**: Fully functional standalone executable
- **Benefits**: 
  - No compatibility issues
  - Reliable runtime behavior
  - Easy distribution (just copy folder)

## Files Added
- **deploy.bat** - Automated build and DLL deployment script
- **cw3230mt.dll** - C++ runtime (copied from BCC installation)
- **owl52t.dll** - OWL framework threaded version (copied from BCC installation)

## Final Status
✅ **railc.exe runs without errors**  
✅ **No system-wide DLL dependencies**  
✅ **Standalone distribution ready**  
✅ **Compatible with systems without BCC 5.02**

The application now runs successfully with all required dependencies included locally.
