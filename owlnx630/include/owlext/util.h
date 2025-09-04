//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// OWLEXT's utility functions and macros
//
// Original code by Don Griffin; used with permission.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_UTIL_H)
#define __OWLEXT_UTIL_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

OWLEXT_BEGIN_NAMESPACE


// These are in OwlExt.cpp:
//
void    ActivateWindow (__OWL TWindow* tWindow);
void    CenterScreenRectOnRect (__OWL TRect* rc, __OWL TRect* rcOver);
void    CenterWindowOver (HWND, HWND);
void    FastFillRect (HDC dc, int x, int y, int cx, int cy, __OWL TColor& color);
void    FastFrameRect3D (HDC hdc, const __OWL TRect& r, __OWL TColor& clrLT, __OWL TColor& clrRB);
LPTSTR  FormatTime (LPTSTR szTime, uint bufSize, bool showSeconds = true);
void    GetTheTime (__OWL TSystemTime& sysTime);
uint16  GetTrueOsVer ();
void    UpdateCursor ();

// MFC style function, that draw the frame of a 3d rect with given colors
void Draw3dRect(__OWL TDC& dc, const __OWL TRect& rect,
  __OWL TColor clrTopLeft, __OWL TColor clrBottomRight);
void Draw3dRect(__OWL TDC& dc, int x, int y, int cx, int cy,
  __OWL TColor clrTopLeft, __OWL TColor clrBottomRight);

// MFC style function, that draws a filled rect. Is this faster???
void FillSolidRect(__OWL TDC& dc, int x, int y, int cx, int cy, __OWL TColor clr);
void FillMaskRect(__OWL TDC& dc, __OWL TRect rect);
void DrawDisabledButton(__OWL TDC& dc, const __OWL TRect& rc);

owl_string GetHintText(__OWL TWindow* win, uint id, __OWL THintText hintType = __OWL htTooltip);

//  Inliners:
//
inline void   CenterWindow (HWND hWnd)
                { CenterWindowOver (hWnd, GetParent(hWnd)); }
inline void   CenterWindow (__OWL TWindow *pWnd)
                { CenterWindow (pWnd->GetHandle()); }
inline void   CenterWindowOver (__OWL TWindow *pWndOver, __OWL TWindow *pWndUnder)
                { CenterWindowOver (pWndOver->GetHandle(), pWndUnder->GetHandle()); }

inline void   FastFillRect (HDC dc, const __OWL TPoint &topLeft, const __OWL TSize &s,
                            __OWL TColor c)
                { FastFillRect (dc, topLeft.x, topLeft.y, s.cx, s.cy, c); }
inline void   FastFillRect (HDC dc, const __OWL TRect &r, __OWL TColor c)
                { FastFillRect (dc, r.left, r.top, r.Width(), r.Height(), c); }

inline uint16 GetOsVer () { return GetTrueOsVer(); }
inline uint   GetOsVerMajor() { return HIBYTE(GetOsVer()); }
inline uint   GetOsVerMinor() { return LOBYTE(GetOsVer()); }

inline bool   IsKeyPressed (int vkey) { return GetKeyState (vkey) < 0; }

// more accurately "HasWin95UI":
inline bool   IsWin95 () { return GetOsVerMajor() >= 4; }

inline uint16 SwapBytes (uint16 w)  { return uint16 ((w << 8) | (w >> 8)); }

////////////////////////////////////////////////////////////////////////////
//  C++ helper functions GetStockBrush, GetStockFont and GetStockPen:
//

// In case WINDOWSX.H was included:
#undef GetStockBrush
#undef GetStockPen
#undef GetStockFont
#undef SelectBrush
#undef SelectPen
#undef SelectFont

enum TStockBrush
{
    sbBlackBrush    = BLACK_BRUSH,
    sbDkGrayBrush   = DKGRAY_BRUSH,
    sbGrayBrush     = GRAY_BRUSH,
    sbHollowBrush   = HOLLOW_BRUSH,
    sbLtGrayBrush   = LTGRAY_BRUSH,
    sbNullBrush     = NULL_BRUSH,
    sbWhiteBrush    = WHITE_BRUSH
};

inline HBRUSH GetStockBrush (TStockBrush sb)
    { return (HBRUSH) GetStockObject (sb); }


enum TStockPen
{
    spWhitePen  = WHITE_PEN,
    spBlackPen  = BLACK_PEN,
    spNullPen   = NULL_PEN
};

inline HPEN GetStockPen (TStockPen sp)
    { return (HPEN) GetStockObject (sp); }


enum TStockFont
{
    sfAnsiFixedFont     = ANSI_FIXED_FONT,
    sfAnsiVarFont       = ANSI_VAR_FONT,
    sfDefaultGuiFont    = DEFAULT_GUI_FONT,
    sfDeviceDefaultFont = DEVICE_DEFAULT_FONT,
    sfOemFixedFont      = OEM_FIXED_FONT,
    sfSystemFont        = SYSTEM_FONT,
    sfSystemFixedFont   = SYSTEM_FIXED_FONT
};

inline HFONT  GetStockFont (TStockFont sf)
    { return (HFONT) GetStockObject (sf); }

inline HPALETTE GetStockPalette ()
    { return (HPALETTE) GetStockObject (DEFAULT_PALETTE); }

inline HBRUSH SelectBrush (HDC dc, HBRUSH hbr)
    { return (HBRUSH) ::SelectObject (dc, hbr); }

inline HPEN SelectPen (HDC dc, HPEN pen)
    { return (HPEN) ::SelectObject (dc, pen); }

inline HFONT SelectFont (HDC dc, HFONT font)
    { return (HFONT) ::SelectObject (dc, font); }



OWLEXT_END_NAMESPACE


#endif