//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2010-02-24 08:16:55 $
//
// Definition of TSystem, a system information provider class
//----------------------------------------------------------------------------

#if !defined(OWL_SYSTEM_H)
#define OWL_SYSTEM_H

#if !defined(OWL_WSYSINC_H)
# include <owl/wsysinc.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// Define Win95 platform constant if not defined in system headers
//
#if !defined(VER_PLATFORM_WIN32_WINDOWS)
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

//
//@class TSystem
// ~~~~~ ~~~~~~~
class _OWLCLASS TSystem {
  public:

    // High level capability support queries
    //
    static bool SupportsExceptions();
    static bool SupportsThreads();
    static bool SupportsInstanceData();

#if defined(OWL5_COMPAT)
    static bool Has3dUI();
    static bool HasSmallCaption();
    static bool HasSmallIcon();
#endif

    // More specific platform type queries
    //
    static bool IsNT();
    static bool IsWin95();
    static bool IsWin32s();

    enum TPlatformId {
      Win32s       = VER_PLATFORM_WIN32s,
      Win32Windows = VER_PLATFORM_WIN32_WINDOWS,
      Win32NT      = VER_PLATFORM_WIN32_NT,
      Win16        = -1,
    };
    static uint GetPlatformId();

    // Version information
    //
    static uint GetVersion();
    static uint GetVersionFlags();

    static uint GetMajorVersion();
    static uint GetMinorVersion();
    static uint GetBuildNumber();

    static const _TCHAR* GetVersionStr();

    // Hardware information
    //
    enum TProcessor {
      Intel386     = PROCESSOR_INTEL_386,
      Intel486     = PROCESSOR_INTEL_486,
      IntelPentium = PROCESSOR_INTEL_PENTIUM,
      MipsR4000    = PROCESSOR_MIPS_R4000,
      Alpha21064   = PROCESSOR_ALPHA_21064,
      Ppc601       = -1,
      Ppc603       = -1,
      Ppc604       = -1,
      Ppc620       = -1,
    };
    static TProcessor GetProcessorType();

  protected:

    // Class that encapsulates the OSVERSIONINFO struct and the GetVersionEx()
    // Win32 call.
    //
    class TVersionInfo : private OSVERSIONINFO /*_OSVERSIONINFOA*/ {
      public:
        TVersionInfo();
        uint GetMajorVersion() const;
        uint GetMinorVersion() const;
        uint GetBuildNumber() const;
        uint GetPlatformId() const;
        const _TCHAR* GetVersionStr() const;
    };

    // Return an initialized version info object
    //
    static TVersionInfo& GetVerInfo();

    // Class that encapsulates the _SYSTEM_INFO struct and the GetSystemInfo()
    // Win32 call.
    //
    class TSystemInfo : private _SYSTEM_INFO {
      public:
        TSystemInfo();
        TProcessor GetProcessorType() const;
    };

    // Return an initialized system info object
    //
    static TSystemInfo& GetSystemInfo();
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inlines
//

#if defined(OWL5_COMPAT)
//
// Does this OS support tiny caption (via the WS_EX_TOOLWINDOW style)
//
inline bool TSystem::HasSmallCaption()
{
  return Has3dUI();
}

//
// Does this OS support small window icons
//
inline bool TSystem::HasSmallIcon()
{
  return Has3dUI();
}

inline bool TSystem::Has3dUI()
{
  return true;
}
#endif


//
inline bool TSystem::IsNT() {
  return GetPlatformId() == Win32NT;
}

//
inline bool TSystem::IsWin95() {
  return GetPlatformId() == Win32Windows;
}

//
inline bool TSystem::IsWin32s() {
  return GetPlatformId() == Win32s;
}

//
inline uint TSystem::GetPlatformId() {
  return GetVerInfo().GetPlatformId();
}

//
inline TSystem::TProcessor TSystem::GetProcessorType() {
  return GetSystemInfo().GetProcessorType();
}

//
inline uint TSystem::GetMajorVersion() {
  return GetVerInfo().GetMajorVersion();
}

//
inline uint TSystem::GetMinorVersion() {
  return GetVerInfo().GetMinorVersion();
}

//
inline uint TSystem::GetBuildNumber() {
  return GetVerInfo().GetBuildNumber();
}

//
inline const _TCHAR* TSystem::GetVersionStr() {
  return GetVerInfo().GetVersionStr();
}

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSystem::TSystemInfo::TSystemInfo() {
  // no struct size to fill in...
  ::GetSystemInfo(this);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
inline TSystem::TProcessor TSystem::TSystemInfo::GetProcessorType() const {
  return (TProcessor)dwProcessorType;
}

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSystem::TVersionInfo::TVersionInfo() {
  dwOSVersionInfoSize = sizeof *this;
  ::GetVersionEx(this);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
inline uint TSystem::TVersionInfo::GetMajorVersion() const {
  return dwMajorVersion;
}

//
inline uint TSystem::TVersionInfo::GetMinorVersion() const {
  return dwMinorVersion;
}

//
inline uint TSystem::TVersionInfo::GetBuildNumber() const {
  return dwBuildNumber;
}

//
inline uint TSystem::TVersionInfo::GetPlatformId() const {
  return dwPlatformId;
}

//
inline const _TCHAR* TSystem::TVersionInfo::GetVersionStr() const {
  return szCSDVersion;
}


__OWL_END_NAMESPACE


#endif  // OWL_SYSTEM_H
