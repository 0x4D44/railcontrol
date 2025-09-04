//----------------------------------------------------------------------------
//  ObjectWindows
// Copyright (c) 1995, 1997 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TSystem class.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_SYSTEM_H)
#include <owl/system.h>
#endif

__OWL_BEGIN_NAMESPACE


//
// Return an initialized version info object
//
TSystem::TVersionInfo& TSystem::GetVerInfo()
{
  static TVersionInfo versionInfo;
  return versionInfo;
}

//
// Return an initialized system info object
//
TSystem::TSystemInfo& TSystem::GetSystemInfo()
{
  static TSystemInfo systemInfo;
  return systemInfo;
}

//
// Return true if the system has exception suport built in and the runtime
// library uses it for stack unwinding support.
//
bool TSystem::SupportsExceptions()
{
  static bool exceptionOK = GetPlatformId() != Win32s;
  return exceptionOK;
}

//
// Return true if the system has thread suport built in and the runtime
// library uses it.
//
bool TSystem::SupportsThreads()
{
  static bool threadOK = GetPlatformId() != Win32s;
  return threadOK;
}

//
// Return true if the system has suport for per-instance data built in and the
// runtime library uses it.
//
bool TSystem::SupportsInstanceData()
{
  static bool instanceDataOK = GetPlatformId() != Win32s;
  return instanceDataOK;
}


//
// Get the windows version number in a 16bit unsigned int, with the bytes in
// the correct order: major in high byte, minor in low byte. Uses older
// GetVersion() API call.
//
uint TSystem::GetVersion() {
  static uint version = uint( ((::GetVersion()&0xFF) << 8) |
                              ((::GetVersion()&0xFF00) >> 8) );
  return version;
}

//
// Get the windows version flags number in a 16bit unsigned int. This is the
// high 16bits of the older GetVersion() API call.
//
uint TSystem::GetVersionFlags() {
  static uint versionFlags = uint(::GetVersion() >> 16);
  return versionFlags;
}

__OWL_END_NAMESPACE
//===============================================================================
