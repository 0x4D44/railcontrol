//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2009-08-05 10:32:03 $                                                    //
//
// Implementation of TFont, an encapsulation of the GDI Font object
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif
#if !defined(DEFAULT_GUI_FONT)      // May not be defined by 16-bit headers
# define  DEFAULT_GUI_FONT  17
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);          // General GDI diagnostic group
DIAG_DECLARE_GROUP(OwlGDIOrphan);    // Oprhan control tracing group

//
// Constructors
//
TFont::TFont(HFONT handle, TAutoDelete autoDelete)
:
  TGdiObject(handle, autoDelete)
{
  if (ShouldDelete)
    RefAdd(Handle, Font);
}

//
//
//
TFont::TFont(LPCTSTR facename,
             int height, int width, int escapement, int orientation,
             int weight,
             uint8 pitchAndFamily,
             uint8 italic, uint8 underline, uint8 strikeout, uint8 charSet,
             uint8 outputPrecision, uint8 clipPrecision, uint8 quality)
{
  Handle = ::CreateFont(height, width, escapement, orientation, weight,
                        italic, underline, strikeout, charSet,
                        outputPrecision, clipPrecision, quality,
                        pitchAndFamily, facename);
  WARNX(OwlGDI, !Handle, 0, _T("Cannot create TFont (") << TResId(facename) <<
        _T(" ") << height << _T("pt)"));
  CheckValid();
  RefAdd(Handle, Font);
}

//
//
//
TFont::TFont(int height, int width, int escapement, int orientation,
             int weight,
             uint8 italic, uint8 underline, uint8 strikeout,
             uint8 charSet,
             uint8 outputPrecision,
             uint8 clipPrecision,
             uint8 quality,
             uint8 pitchAndFamily,
             LPCTSTR facename)
{
  Handle = ::CreateFont(height, width, escapement, orientation, weight,
                        italic, underline, strikeout, charSet,
                        outputPrecision, clipPrecision, quality,
                        pitchAndFamily, facename);
  WARNX(OwlGDI, !Handle, 0, _T("Cannot create TFont (") << TResId(facename) <<
        _T(" ") << height << _T("pt)"));
  CheckValid();
  RefAdd(Handle, Font);
}

//
//
//
TFont::TFont(const LOGFONT * logFont)
{
  PRECONDITION(logFont);
  Handle = ::CreateFontIndirect((LPLOGFONT)logFont);  // API typecast
  WARNX(OwlGDI, !Handle, 0, _T("Cannot create TFont from logfont @") <<
        hex << uint32(LPVOID(logFont)));
  CheckValid();
  RefAdd(Handle, Font);
}

//
//
//
TFont::TFont(const TFont& src)
{
#if !defined(NO_GDI_SHARE_HANDLES)
  Handle = src.Handle;
  RefAdd(Handle, Font);
#else
  LOGFONT logFont;
  src.GetObject(logFont);
  Handle = ::CreateFontIndirect(&logFont);
  WARNX(OwlGDI, !Handle, 0, _T("Cannot create TFont from TFont @") <<
        hex << uint32(LPVOID(&src)));
  CheckValid();
  RefAdd(Handle, Font);
#endif
}

//
// Retrieves information about this font when it is selected in the
// specified Device Context.
//
void
TFont::GetTextMetrics(TEXTMETRIC& tm, TDC& dc) const
{
  dc.SelectObject(*this);
  dc.GetTextMetrics(tm);
  dc.RestoreFont();
}

//
// Returns information about this font when it is selected in the
// specified Device Context.
//
TEXTMETRIC
TFont::GetTextMetrics(TDC& dc) const
{
  TEXTMETRIC tm;
  GetTextMetrics(tm, dc);
  return tm;
}

//
// Retrieves information about this font when it is selected in
// a Screen Device Context.
//
void
TFont::GetTextMetrics(TEXTMETRIC& tm) const
{
  TScreenDC dc;
  GetTextMetrics(tm, dc);
}

//
// Returns information about this font when it is selected in
// a Screen Device Context.
//
TEXTMETRIC
TFont::GetTextMetrics() const
{
  TEXTMETRIC  tm;
  TScreenDC   dc;
  GetTextMetrics(tm, dc);
  return tm;
}

//
// Return the extent of a given string using this particular font.
//
TSize
TFont::GetTextExtent(LPCTSTR text) const
{
  TScreenDC dc;
  return GetTextExtent(dc, text);
}

//
// Return the extent of a given string using this particular font in a
// particular DC.
//
TSize
TFont::GetTextExtent(TDC& dc, LPCTSTR text) const
{
  dc.SelectObject(*this);
  TSize size = dc.GetTextExtent(text, ::_tcslen(text));
  dc.RestoreFont();
  return size;
}

//
//  Return the font's name
//
owl_string
TFont::GetFaceName() const
{
  LOGFONT lf;
  GetObject(lf);
  return owl_string(lf.lfFaceName);
}

//
//  Return the pitch and family flags for this logical font
//
uint8
TFont::GetPitchAndFamily() const
{
  LOGFONT lf;
  GetObject(lf);
  return lf.lfPitchAndFamily;
}

//
// Create a wrapper for the default GUI font.
//
TDefaultGUIFont::TDefaultGUIFont()
:
  TFont(HFONT(::GetStockObject(DEFAULT_GUI_FONT)))
{
}

__OWL_END_NAMESPACE
/* ========================================================================== */

