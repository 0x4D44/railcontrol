//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:38 $
//
// Implementation of classes TCreatedDC, and TIC
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);

//
//
//
TCreatedDC::TCreatedDC()
:
  TDC()
{
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TCreatedDC constructed @") << (void*)this);
}

//
//
//
TCreatedDC::TCreatedDC(LPCTSTR driver, LPCTSTR device,
                       LPCTSTR output, const DEVMODE * initData)
:
  TDC()
{
  Handle = ::CreateDC(driver, device, output, initData);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TCreatedDC constructed @") << (void*)this <<
    _T(" with driver ") << owl_string(driver).c_str() );
}

//
// Use an existing HDC. Delete it on destruction as requested
//
TCreatedDC::TCreatedDC(HDC handle, TAutoDelete autoDelete)
:
  TDC(handle, autoDelete)
{
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TCreatedDC constructed @") << (void*)this <<
    _T(" with handle ") << uint(handle));
}

//
// Destruct the TDC by deleting the HDC if ShouldDelete is true
//
TCreatedDC::~TCreatedDC()
{
  RestoreObjects();
  if (ShouldDelete && Handle)
    ::DeleteDC(STATIC_CAST(HDC,Handle));
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TCreatedDC destructed @") << (void*)this);
}

//
//
//
TIC::TIC(LPCTSTR driver, LPCTSTR device,
         LPCTSTR output, const DEVMODE * initData)
:
  TCreatedDC()
{
  Handle = STATIC_CAST(HANDLE,::CreateIC(driver, device, output, initData));
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TIC constructed @") << (void*)this <<
    " with driver " << owl_string(driver).c_str());
}

//
//
//
TIC::~TIC()
{
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TIC destructed @") << (void*)this);
}

__OWL_END_NAMESPACE
/* ========================================================================== */
