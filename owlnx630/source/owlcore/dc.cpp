//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Implementation of class TDC
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);        // General GDI diagnostic group

//
//
//
void
TDC::Init()
{
  OrgBrush = 0;
  OrgPen = 0;
  OrgFont = 0;
  OrgPalette = 0;
  OrgBitmap = 0;
  OrgTextBrush = 0;
}

//
//
//
TDC::TDC(HDC handle)
:
  TGdiBase(handle, NoAutoDelete)
{
  Init();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TDC constructed @") << (void*)this <<
    _T(" from handle") << uint(handle));
}

//
// Following two ctors are for use by derived classes only
//
TDC::TDC()
{
  Init();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TDC constructed @") << (void*)this);
}

//
//
//
TDC::TDC(HDC handle, TAutoDelete autoDelete)
:
  TGdiBase(handle, autoDelete)
{
  Init();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TDC constructed @") << (void*)this <<
    _T(" from handle") << uint(handle));
}

//
// Default dtor does not delete Handle
//
TDC::~TDC()
{
  RestoreObjects();
  TRACEX(OwlGDI, OWL_CDLEVEL, _T("TDC destructed @") << (void*)this);
}

//
//
//
HDC
TDC::GetAttributeHDC() const
{
  return HDC(Handle);
}

//
//
//
void
TDC::SelectObject(const TPen& pen)
{
  TRACEX(OwlGDI, 1, _T("TDC::SelectPen @") << (void*)this <<
    _T(" pen @") << (void*)&pen);
  HPEN oldPen = (HPEN)::SelectObject(GetHDC(), pen);
  if (oldPen) {
    TGdiObject::RefInc(pen);
    if (uint(oldPen) > 1)
      if (!OrgPen)
        OrgPen = oldPen;
      else
        TGdiObject::RefDec(oldPen, false);
  }
}

//
//
//
void
TDC::SelectObject(const TBrush& brush)
{
  TRACEX(OwlGDI, 1, _T("TDC::SelectBrush @") << (void*)this <<
    _T(" brush @") << (void*)&brush);
  HBRUSH oldBrush = (HBRUSH)::SelectObject(GetHDC(), brush);
  if (oldBrush) {
    TGdiObject::RefInc(brush);
    if (uint(oldBrush) > 1)
      if (!OrgBrush)
        OrgBrush = oldBrush;
      else
        TGdiObject::RefDec(oldBrush, false);
  }
}

//
//
//
void
TDC::SelectObject(const TFont& font)
{
  TRACEX(OwlGDI, 1, _T("TDC::SelectFont @") << (void*)this <<
    _T(" font @") << (void*)&font);
  HFONT oldFont = (HFONT)::SelectObject(GetHDC(), font);
  if (oldFont) {
    TGdiObject::RefInc(font);
    if (uint(oldFont) > 1)
      if (!OrgFont)
        OrgFont = oldFont;
      else
        TGdiObject::RefDec(oldFont, false);
  }
}

//
//
//
void
TDC::SelectObject(const TPalette& palette, bool forceBackground)
{
  TRACEX(OwlGDI, 1, _T("TDC::SelectPalette @") << (void*)this <<
    _T(" palette @") << (void*)&palette);
  HPALETTE oldPalette = ::SelectPalette(GetHDC(), palette, forceBackground);
  if (oldPalette) {
    TGdiObject::RefInc(palette);
    if (uint(oldPalette) > 1)
      if (!OrgPalette)
        OrgPalette = oldPalette;
      else
        TGdiObject::RefDec(oldPalette, false);
  }
}

//
//
//
void
TDC::SelectObject(const TBitmap& bitmap)
{
  TRACEX(OwlGDI, 1, _T("TDC::SelectBitmap @") << (void*)this <<
    _T(" bitmap @") << (void*)&bitmap);
  HBITMAP oldBitmap = (HBITMAP)::SelectObject(GetHDC(), bitmap);
  if (oldBitmap) {
    TGdiObject::RefInc(bitmap);
    if (uint(oldBitmap) > 1)
      if (!OrgBitmap)
        OrgBitmap = oldBitmap;
      else
        TGdiObject::RefDec(oldBitmap, false);
  }
}

//
//
//
void
TDC::SelectStockObject(int index)
{
  PRECONDITION(::GetStockObject(index));
  TRACEX(OwlGDI, 1, _T("TDC::SelectStockObject @") << (void*)this <<
    _T(" index ") << index);
  HANDLE oldObj = ::SelectObject(GetHDC(), ::GetStockObject(index));
  if (uint(oldObj) > 1)
    TGdiObject::RefDec(oldObj, false);
}

//
//
//
void
TDC::RestorePen()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestorePen @") << (void*)this);
  if (OrgPen) {
    TGdiObject::RefDec(::SelectObject(GetHDC(), OrgPen), false);
    OrgPen = 0;
  }
}

//
//
//
void
TDC::RestoreBrush()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestoreBrush @") << (void*)this);
  if (OrgBrush) {
    TGdiObject::RefDec(::SelectObject(GetHDC(), OrgBrush), false);
    OrgBrush = 0;
  }
}

//
//
//
void
TDC::RestoreFont()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestoreFont @") << (void*)this);
  if (OrgFont) {
    TGdiObject::RefDec(::SelectObject(GetHDC(), OrgFont), false);
    OrgFont = 0;
  }
}

//
//
//
void
TDC::RestorePalette()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestorePalette @") << (void*)this);
  if (OrgPalette) {
    TGdiObject::RefDec(::SelectPalette(GetHDC(), OrgPalette, false), false);
    OrgPalette = 0;
  }
}

//
//
//
void
TDC::RestoreBitmap()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestorePen @") << (void*)this);
  if (OrgBitmap) {
    TGdiObject::RefDec(::SelectObject(GetHDC(), OrgBitmap), false);
    OrgBitmap = 0;
  }
}

//
//
//
void
TDC::RestoreTextBrush()
{
  TRACEX(OwlGDI, 1, _T("TDC::RestoreTextBrush @") << (void*)this);
  if (OrgTextBrush) {
    TGdiObject::RefDec(::SelectObject(GetHDC(), OrgTextBrush), false);
    OrgTextBrush = 0;
  }
}

//
//
//
void
TDC::RestoreObjects()
{
  if (Handle) {
    RestorePen();
    RestoreBrush();
    RestoreFont();
    RestorePalette();
    RestoreBitmap();
    RestoreTextBrush();
  }
}

//
// Subset of Win32 GetCurrentObject , just a straight
// call for normal win32
//
HANDLE
TDC::GetCurrentObject(uint objectType) const
{
    return ::GetCurrentObject(GetHDC(), objectType);
}

extern "C" bool  PASCAL FastWindowFrame(HDC, LPRECT, int xWidth, int yWidth, long rop);

//!CQ add ROP arg to allow this to be used by docking drag frame painting

//
//
//
void
TDC::OWLFastWindowFrame(TBrush& brush, TRect& r, int xThick, int yThick, uint32 rop)
{
  SelectObject(brush);

    int  width = r.Width() - xThick;
    int  height = r.Height() - yThick;

    PatBlt(r.left,        r.top, xThick, height, rop);  // left
    PatBlt(r.left+xThick, r.top, width, yThick, rop);   // top
    PatBlt(r.left, r.top+height, width, yThick, rop);  // bottom
    PatBlt(r.left+width,  r.top+yThick, xThick, height, rop);  // right

  RestoreBrush();
}

//
//
//
int
TDC::SaveDC() const
{
  return ::SaveDC(GetHDC());
}

//
//
//
bool
TDC::RestoreDC(int savedIndex)
{
  return ::RestoreDC(GetHDC(), savedIndex);
}

//
//
//
int
TDC::GetDeviceCaps(int index) const
{
  return ::GetDeviceCaps(GetAttributeHDC(), index);
}

//
//
//
bool
TDC::ResetDC(DEVMODE & devMode)
{
  return ::ResetDC(GetHDC(), &devMode) != 0;
}

//
//
//
TColor
TDC::SetBkColor(const TColor& color)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetBkColor(GetHDC(), color);
  return ::SetBkColor(GetAttributeHDC(), color);
}

//
//
//
TColor
TDC::SetTextColor(const TColor& color)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetTextColor(GetHDC(), color);
  return ::SetTextColor(GetAttributeHDC(), color);
}

//
//
//
int
TDC::SetMapMode(int mode)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetMapMode(GetHDC(), mode);
  return ::SetMapMode(GetAttributeHDC(), mode);
}

//
//
//
bool
TDC::SetViewportOrg(const TPoint& point, TPoint * oldOrg)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetViewportOrgEx(GetHDC(), point.x, point.y, oldOrg);
  return ::SetViewportOrgEx(GetAttributeHDC(), point.x, point.y, oldOrg);
}

//
//
//
bool
TDC::OffsetViewportOrg(const TPoint& delta, TPoint * oldOrg)
{
  if (GetHDC() != GetAttributeHDC())
    ::OffsetViewportOrgEx(GetHDC(), delta.x, delta.y, oldOrg);
  return ::OffsetViewportOrgEx(GetAttributeHDC(), delta.x, delta.y, oldOrg);
}

//
//
//
bool
TDC::SetViewportExt(const TSize& extent, TSize * oldExtent)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetViewportExtEx(GetHDC(), extent.cx, extent.cy, oldExtent);
  return ::SetViewportExtEx(GetAttributeHDC(), extent.cx, extent.cy, oldExtent);
}

//
//
//
bool
TDC::ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom,
                      TSize * oldExtent)
{
  if (GetHDC() != GetAttributeHDC())
    ::ScaleViewportExtEx(GetHDC(), xNum, xDenom, yNum, yDenom, oldExtent);
  return ::ScaleViewportExtEx(GetAttributeHDC(), xNum, xDenom, yNum, yDenom, oldExtent);
}

//
//
//
bool
TDC::SetWindowOrg(const TPoint& point, TPoint * oldOrg)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetWindowOrgEx(GetHDC(), point.x, point.y, oldOrg);
  return ::SetWindowOrgEx(GetAttributeHDC(), point.x, point.y, oldOrg);
}

//
//
//
bool
TDC::OffsetWindowOrg(const TPoint& delta, TPoint * oldOrg)
{
  if (GetHDC() != GetAttributeHDC())
    ::OffsetWindowOrgEx(GetHDC(), delta.x, delta.y, oldOrg);
  return ::OffsetWindowOrgEx(GetAttributeHDC(), delta.x, delta.y, oldOrg);
}

//
//
//
bool
TDC::SetWindowExt(const TSize& extent, TSize * oldExtent)
{
  if (GetHDC() != GetAttributeHDC())
    ::SetWindowExtEx(GetHDC(), extent.cx, extent.cy, oldExtent);
  return ::SetWindowExtEx(GetAttributeHDC(), extent.cx, extent.cy, oldExtent);
}

//
//
//
bool
TDC::ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom, TSize * oldExtent)
{
  if (GetHDC() != GetAttributeHDC())
    ::ScaleWindowExtEx(GetHDC(), xNum, xDenom, yNum, yDenom, oldExtent);
  return ::ScaleWindowExtEx(GetAttributeHDC(), xNum, xDenom, yNum, yDenom, oldExtent);
}

//
//
//
bool
TDC::TextOut(int x, int y, LPCTSTR str, int count)
{
  return ::TextOut(GetHDC(), x, y, str, count>=0 ? count : ::_tcslen(str));
}

//
//
//
bool
TDC::ExtTextOut(int x, int y, uint16 options, const TRect* rect,
                LPCTSTR str, int count, const int * dx)
{
  PRECONDITION(count == 0 || str != 0);
  return ::ExtTextOut(GetHDC(), x, y, options, rect, str,
                      count>=0 ? count : ::_tcslen(str), (int *)dx);
                                                      // API typecast
}

//
//
//
bool
TDC::TabbedTextOut(const TPoint& p, LPCTSTR str, int count,
                   int numPositions, const int * positions,
                   int tabOrigin, TSize& size) {
  size = (uint32)::TabbedTextOut(GetHDC(), p.x, p.y, str,
                                 count>=0 ? count : ::_tcslen(str),
                                 numPositions, (int *)positions,
                                 tabOrigin);
                                                // API typecast
  return true;
}

//
//
//
int
TDC::DrawText(LPCTSTR str, int count, const TRect& rect, uint16 format)
{
  return ::DrawText(GetHDC(), str, count,  // uses -1 to signify autocount
                    (RECT*)&rect, format);
                    // API typecast
}

//
//
//
int
TDC::DrawTextEx(LPTSTR str, int count, const TRect* rect, uint format,
                LPDRAWTEXTPARAMS params)
{
  if(params)
    params->cbSize = sizeof(DRAWTEXTPARAMS);
  return ::DrawTextEx(GetHDC(), str, count,  // uses -1 to signify autocount
                      (RECT*)rect, format, params);
                    // API typecast
}

//
//
//
bool  //JBC
TDC::DrawFrameControl(TRect lpRect, UINT nType, UINT nState)
{ 
  RECT rect;
  rect.left = lpRect.Left();
  rect.top = lpRect.Top();
  rect.right = lpRect.Right();
  rect.bottom = lpRect.Bottom();
  return ::DrawFrameControl((HDC)Handle, &rect, nType, nState); 
}

//
//
//
bool
TDC::GrayString(const TBrush& brush, GRAYSTRINGPROC outputFunc,
                LPCTSTR str, int count, const TRect& rect)
{
  return ::GrayString(GetHDC(), brush, outputFunc, (uint32)str,
                      count>=0 ? count : 0,  // uses 0 to signify autocount
                      rect.left, rect.top, rect.Width(), rect.Height());
}

// DLN (stripped from TUIHandle)
// Draws an edge using system ::DrawEdge if available, otherwise
// does it the hard way
bool
TDC::DrawEdge(const TRect& frame, uint edge, uint flags)
{
  static int hasDrawEdge = true;

  // Try once to see if the API call is available. If not, do ourselves.
  //
  if (hasDrawEdge) {
    if (::DrawEdge(*this, (LPRECT)&frame, edge, flags))
      return true;
    if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
      hasDrawEdge = false;
    else
      return false;
  }

  // ::DrawEdge is not available, do the drawing ourselves
  //
  TRect f(frame);  // working frame rectangle

  // If mono is set, draw a thin, flat, black (windowFrame) frame
  //
  if (flags & Mono) 
    {
    if (edge & EdgeOuter) 
      {
// KSM      PaintFrame(*this, f, flags, TColor::SysWindowFrame, TColor::SysWindowFrame);
      PaintFrame(f, flags, TColor::SysWindowFrame, TColor::SysWindowFrame);

      f.Inflate(-1,-1);
      }
    if (flags & Fill) 
      { //!CQ repeated code--nest else?
      TBrush brsh(TColor::SysWindow);
      SelectObject(brsh);
      PatBlt(f);
      RestoreBrush();
      }
    return true;
    }

  // If flat is set, draw a thin, flat, shadow frame
  //
  if (flags & Flat) 
    {
    if (edge & EdgeOuter) 
      {
      PaintFrame(f, flags, TColor::Sys3dShadow, TColor::Sys3dShadow);
      f.Inflate(-1,-1);
      }
    if (flags & Fill) 
      { //!CQ repeated code--nest else?
      TBrush brsh(TColor::Sys3dFace);
      SelectObject(brsh);
      PatBlt(f);
      RestoreBrush();
      }
    return true;
    }

  // Draw outer edge if indicated, adjusting rect afterwards
  //
  if (edge & EdgeOuter) {
    static TColor tlColors[] = {
      TColor::Sys3dLight,       // EdgeRaised
      TColor::Sys3dHilight,     // EdgeRaised + Soft
      TColor::Sys3dShadow,      // EdgeSunken
      TColor::Sys3dDkShadow,    // EdgeSunken + Soft
    };
    static TColor brColors[] = {
      TColor::Sys3dDkShadow,    // EdgeRaised
      TColor::Sys3dDkShadow,    // EdgeRaised + Soft
      TColor::Sys3dHilight,     // EdgeSunken
      TColor::Sys3dHilight,     // EdgeSunken + Soft
    };
    int ci = ((edge & SunkenOuter) ? 2 : 0) | ((flags & Soft) ? 1 : 0);
//KSM    PaintFrame(*this, f, flags, tlColors[ci], brColors[ci]);
    PaintFrame(f, flags, tlColors[ci], brColors[ci]);
    f.Inflate(-1,-1);
  }

  // Draw inner edge if indicated, adjusting rect afterwards
  //
  if (edge & EdgeInner) {
    static TColor tlColors[] = {
      TColor::Sys3dHilight,     // EdgeRaised
      TColor::Sys3dLight,       // EdgeRaised + Soft
      TColor::Sys3dDkShadow,    // EdgeSunken
      TColor::Sys3dShadow,      // EdgeSunken + Soft
    };
    static TColor brColors[] = {
      TColor::Sys3dShadow,      // EdgeRaised
      TColor::Sys3dShadow,      // EdgeRaised + Soft
      TColor::Sys3dLight,       // EdgeSunken
      TColor::Sys3dLight,       // EdgeSunken + Soft
    };
    int ci = ((edge & SunkenOuter) ? 2 : 0) | ((flags & Soft) ? 1 : 0);
//KSM    PaintFrame(*this, f, flags, tlColors[ci], brColors[ci]);
    PaintFrame(f, flags, tlColors[ci], brColors[ci]);
    f.Inflate(-1,-1);
  }

  // Fill interior if indicated
  //
  if (flags & Fill) {
    TBrush brsh(TColor::Sys3dFace);
    SelectObject(brsh);
    PatBlt(f);
    RestoreBrush();
  }

//!CQ not really usefull since frame is not returned
//  if (flags & Adjust)
//    frame = f;

  return true;
}

// DLN ripped from TUIHandle
// Paint a 2-color single pixel thick frame, bevel corners get their own color
//
/*void
TDC::PaintFrame(const TRect& fr, uint flags, const TColor& tlColor, const TColor& brColor)
{
  if (flags & (Top | Left)) {
    TBrush brsh(tlColor);
    dc.SelectObject(brsh);
    if (flags & Top)
      dc.PatBlt(fr.left, fr.top, fr.Width()-1, 1);
    if (flags & Left)
      dc.PatBlt(fr.left, fr.top+1, 1, fr.Height()-2);
    dc.RestoreBrush();
  }

  if (flags & (Bottom | Right)) {
    TBrush brsh(brColor);
    dc.SelectObject(brsh);
    if (flags & Bottom)
      dc.PatBlt(fr.left, fr.bottom-1, fr.Width(), 1);
    if (flags & Right)
      dc.PatBlt(fr.right-1, fr.top, 1, fr.Height()-1);
    dc.RestoreBrush();
  }
}
*/
void
TDC::PaintFrame(const TRect& fr, uint flags, const TColor& tlColor, const TColor& brColor)
{
  if (flags & (Top | Left)) {
    TBrush brsh(tlColor);
    SelectObject(brsh);
    if (flags & Top)
      PatBlt(fr.left, fr.top, fr.Width()-1, 1);
    if (flags & Left)
      PatBlt(fr.left, fr.top+1, 1, fr.Height()-2);
    RestoreBrush();
  }

  if (flags & (Bottom | Right)) {
    TBrush brsh(brColor);
    SelectObject(brsh);
    if (flags & Bottom)
      PatBlt(fr.left, fr.bottom-1, fr.Width(), 1);
    if (flags & Right)
      PatBlt(fr.right-1, fr.top, 1, fr.Height()-1);
    RestoreBrush();
  }
}
__OWL_END_NAMESPACE
/* ========================================================================== */
