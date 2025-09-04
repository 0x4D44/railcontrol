//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2010-05-17 07:12:34 $
//
// Implementation of TPrintDC
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif

//
// Supply missing function definition and macros for newer Win32 function
//
  extern "C" {
    int  WINAPI DeviceCapabilitiesExA(LPCSTR, LPCSTR, LPCSTR, WORD,
                                      LPSTR, CONST DEVMODEA*);
    int  WINAPI DeviceCapabilitiesExW(LPCWSTR, LPCWSTR, LPCWSTR, WORD,
                                      LPWSTR, CONST DEVMODEW*);
    #if defined(UNICODE)
      #define DeviceCapabilitiesEx  DeviceCapabilitiesExW
    #else
      #define DeviceCapabilitiesEx  DeviceCapabilitiesExA
    #endif // !UNICODE
  }

  #define PROC_DEVICECAPABILITIES MAKEINTRESOURCE(91)

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
//
//
class TPrinterDriverLibrary : public TModule {
  public:
    TPrinterDriverLibrary(LPCTSTR drvName);

    TModuleProc5<int,LPCTSTR,LPCTSTR,WORD,LPTSTR,const DEVMODE*> DeviceCapabilities;
};

//
//
//
TPrinterDriverLibrary::TPrinterDriverLibrary(LPCTSTR drvName)
:
  TModule(drvName, true, true, false), // shouldLoad, mustLoad and !addToList
  DeviceCapabilities(*this,(LPCSTR)PROC_DEVICECAPABILITIES)
{
}

//
//
//
TPrintDC::TPrintDC(HDC handle, TAutoDelete autoDelete)
:
  TCreatedDC(handle, autoDelete)
{
  memset(&DocInfo, 0, sizeof(DocInfo));
  DocInfo.cbSize = sizeof(DocInfo);
  DocInfo.lpszOutput = _T("");
  DocInfo.lpszDatatype = _T("");
}

//
//
//
TPrintDC::TPrintDC(LPCTSTR driver, LPCTSTR device,
                   LPCTSTR output, const DEVMODE * initData)
:
  TCreatedDC(driver, device, output, initData)
{
  memset(&DocInfo, 0, sizeof(DocInfo));
  DocInfo.cbSize = sizeof(DocInfo);
  DocInfo.lpszOutput = _T("");
  DocInfo.lpszDatatype = _T("");
}

//
//
//
uint32
TPrintDC::DeviceCapabilities(LPCTSTR driver,
                             LPCTSTR device,
                             LPCTSTR port,
                             int     capability,
                             LPTSTR  output,
                             LPDEVMODE devmode)
{
  // Hand call DeviceCapabilities due to Win32s missing function!
  //
  uint32 caps = 0;

#if !defined(WINELIB)
  // Try the Win32 DeviceCapabilitiesEx function
  //
  caps = ::DeviceCapabilitiesEx(driver, device, port, (uint16)capability, output, devmode);
  if (GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
    return caps;
#endif

  // Locate & call the DeviceCapabilities function within the printer driver
  // itself.
  //
  try {
    TPrinterDriverLibrary driverLib(driver);
    caps = driverLib.DeviceCapabilities(device, port, (uint16)capability, output, devmode);
  }
  catch (...) {
  }
  return caps;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
