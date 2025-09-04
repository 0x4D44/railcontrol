//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TIcon, a GDI Icon object encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_SHELLITM_H)
# include <owl/shellitm.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);        // General GDI diagnostic group

//
//
//
TIcon::TIcon(HICON handle, TAutoDelete autoDelete)
:
  TGdiBase(handle, autoDelete)
{
}

//
//
//
TIcon::TIcon(HINSTANCE, const TIcon& icon)
{
  Handle = ::CopyIcon(icon);
  CheckValid();
}

//
//
//
TIcon::TIcon(HINSTANCE instance, TResId resId)
{
  Handle = ::LoadIcon(instance, resId);
  ShouldDelete = (instance != 0);
  CheckValid();
}

//
//
//
TIcon::TIcon(HINSTANCE instance, LPCTSTR fileName, int index)
{
  //!TH #if defined(BI_PLAT_WIN32) && WINVER >= 0x400
  //!CQ   // could use win4 call...
  //!TH #else
  Handle = TShell::ExtractIcon(instance, (LPTSTR)fileName, index);

  //!TH #endif
  if ((int)Handle == 1)
    Handle = 0;
  CheckValid();
}

//
//
//
TIcon::TIcon(HINSTANCE instance, const TSize& size, int planes, int bitsPixel,
             const void * andBits, const void * xorBits)
{
  Handle = ::CreateIcon(instance, size.cx, size.cy,
                        uint8(planes), uint8(bitsPixel),
                        (const uint8 *)andBits, (const uint8 *)xorBits);
  CheckValid();
}

//
//
//
TIcon::TIcon(const void* resBits, uint32 resSize)
{
  Handle = CreateIconFromResource((PBYTE)resBits, resSize, true, 0x00030000);
  CheckValid();
}

//
//
//
TIcon::TIcon(const ICONINFO* iconInfo)
{
  //IconInfo->fIcon = true;  // assume the user setup the struct OK
  Handle = CreateIconIndirect((PICONINFO)iconInfo);
  CheckValid();
}

//
//
//
TIcon::~TIcon()
{
  if (ShouldDelete && Handle)
    ::DestroyIcon((HICON)Handle);
}

__OWL_END_NAMESPACE
/* ========================================================================== */

