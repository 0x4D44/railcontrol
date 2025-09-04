//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of print preview classes
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_PREVIEW_H)
# include <owl/preview.h>
#endif
#if !defined(OWL_PRINTER_H)
# include <owl/printer.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif 
#if !defined(OWL_PRINTDIA_H)
# include <owl/printdia.h>
#endif
#include <math.h>

//
// Some inlines to provide platform independence since we drop directly to API
// calls to implement some of TPrintPreviewDC.
//
  inline bool SetWindowExt(HDC hdc, int w, int h) {return ::SetWindowExtEx(hdc, w, h, 0);}
  inline bool SetWindowOrg(HDC hdc, int w, int h) {return ::SetWindowOrgEx(hdc, w, h, 0);}
  inline bool SetViewportExt(HDC hdc, int w, int h) {return ::SetViewportExtEx(hdc, w, h, 0);}
  inline bool SetViewportOrg(HDC hdc, int x, int y) {return ::SetViewportOrgEx(hdc, x, y, 0);}

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Construct a basic Preview DC
//
TPreviewDCBase::TPreviewDCBase(TDC&      screen,
                               TPrintDC& printdc)
:
  TPrintDC(screen),
  PrnDC(printdc)
{}

//
//
//
HDC
TPreviewDCBase::GetAttributeHDC() const
{
  return PrnDC;
}

//
// Construct a Print Preview Device context
//
TPrintPreviewDC::TPrintPreviewDC(TDC&         screen,
                                 TPrintDC&    printdc,
                                 const TRect& client,
                                 const TRect& clip)
:
  TPreviewDCBase(screen, printdc),
  PrnFont(0),
  CurrentPreviewFont(0)
{
  //
  // Set the initial mode & extents for the screen dc
  //
  ::SetMapMode(GetHDC(), MM_ANISOTROPIC);
  ::SetWindowExt(GetHDC(), client.Width(), client.Height());
  ::SetViewportExt(GetHDC(), client.Width(), client.Height());

  //
  // Static call to virtual method, but some mapping must be done now.
  //
  ReScale();

  //
  // Assume clip rect is in device points - DPs are same in new viewport
  //
  SelectClipRgn(clip);
  PrnFont = (HFONT)PrnDC.GetCurrentObject(OBJ_FONT);
  SyncFont();
}

//
//
//
TPrintPreviewDC::~TPrintPreviewDC()
{
  // cleanup screen dc
  //
  ::DeleteObject(::SelectObject(GetHDC(), ::GetStockObject(SYSTEM_FONT)));
  delete CurrentPreviewFont;
}

//
// Intercept setting of the printer font, making & keeping a copy of it and
// calling SyncFont if needed to recreate the preview font.
//
void
TPrintPreviewDC::SelectObject(const TFont& newFont)
{
  if ((HFONT)newFont) {
    LOGFONT   lf;
    newFont.GetObject(lf);

    TFont*  oldPreviewF = CurrentPreviewFont;
    CurrentPreviewFont = new TFont(&lf);

    PrnDC.SelectObject(*CurrentPreviewFont);
    delete oldPreviewF;

    if ((HFONT)(*CurrentPreviewFont) != PrnFont) {
      PrnFont = (HFONT)(*CurrentPreviewFont);
      SyncFont();
    }
  }
}

//
//
//
void
TPrintPreviewDC::SelectStockObject(int index)
{
  PrnDC.SelectStockObject(index);
  switch (index) {
    case ANSI_FIXED_FONT:
    case ANSI_VAR_FONT:
    case DEVICE_DEFAULT_FONT:
    case OEM_FIXED_FONT:
    case SYSTEM_FONT:
    case SYSTEM_FIXED_FONT: {
      HFONT stockFont = (HFONT)GetStockObject(index);
      if (stockFont != PrnFont) {
        PrnFont = stockFont;
        SyncFont();
      }
      break;
    }
    default:
      TPrintDC::SelectStockObject(index);
  }
}

//
//
//
void
TPrintPreviewDC::RestoreFont()
{
  PrnDC.RestoreFont();
  PrnFont = (HFONT)PrnDC.GetCurrentObject(OBJ_FONT);
  TPrintDC::RestoreFont();
  SyncFont();
}

//
//
//
int
TPrintPreviewDC::GetDeviceCaps(int index) const
{
  switch (index) {
    case CLIPCAPS:
    case RASTERCAPS:
    case CURVECAPS:
    case LINECAPS:
    case POLYGONALCAPS:
    case TEXTCAPS:      // report capabilities supported on both devices
      return PrnDC.GetDeviceCaps(index) & TPrintDC::GetDeviceCaps(index);

      // otherwise, report printer caps and let GDI sort out differences
    default:
      return PrnDC.GetDeviceCaps(index);
  }
}

//
//
//
inline int
GlyphHeight(TEXTMETRIC& tm)
{
  return tm.tmHeight < 0
            ? tm.tmHeight
            : -(tm.tmHeight - tm.tmInternalLeading);
}

//
// SyncFont performs a simple font match attempt, with a retry option if
// the GDI selected match is too different from the selected printer font.
// In print preview, matching the size of the characters is more important
// than matching their appearance.  In most cases, the print preview will
// barely be legible anyway.  Size is most important because you don't
// want character size differences to change the line breaks or page
// breaks of the on-screen document from what they would be on the
// printed page.  This effect is minimized in this TPrintPreviewDC object,
// since info reports such as GetTextMetrics and GetTextExtent are always
// reported from the printer dc using the real font.  Internal calculations
// should be the same for preview as for printing, but the output accuracy
// will depend upon the accuracy of font selection.
//
// It is also possible to take over control of the text output functions
// through this DC object - the TextOut and other text methods are virtual.
// You can place each character on the preview screen yourself, if you
// desire more precision in character placement than GDI's font matching
// can provide.  That's a lot of work, and a lot of code, and isn't
// necessary to meet the needs of most applications.
//
// SyncFont is virtual so that you may substitute your own font matching
// algorythm with more font matching heuristics.
//
void
TPrintPreviewDC::SyncFont()
{
  //
  // set screen font to match current printer font.
  //
  LOGFONT lf;
  ::GetObject(PrnFont, sizeof(lf), &lf);

  TEXTMETRIC tm;
  PrnDC.GetTextMetrics(tm);

  lf.lfHeight         = GlyphHeight(tm);
  lf.lfWidth          = tm.tmAveCharWidth;
  lf.lfWeight         = tm.tmWeight;
  lf.lfItalic         = tm.tmItalic;
  lf.lfUnderline      = tm.tmUnderlined;
  lf.lfStrikeOut      = tm.tmStruckOut;
  lf.lfCharSet        = tm.tmCharSet;
  lf.lfOutPrecision   = OUT_TT_PRECIS;
  lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
  lf.lfQuality        = DRAFT_QUALITY;

  // Keep just the pitch (low 2 bits). Ignore the family
  //
  lf.lfPitchAndFamily = uint8((tm.tmPitchAndFamily & 0x0003) | FF_DONTCARE);
  PrnDC.GetTextFace(sizeof(lf.lfFaceName)/sizeof(lf.lfFaceName[0]), lf.lfFaceName);

  ::DeleteObject(::SelectObject(GetHDC(), ::CreateFontIndirect(&lf)));

  //
  // if height isn't right, relax the font pitch and facename requirements
  //
  GetTextMetrics(tm);
  if (abs(abs(static_cast<int>(lf.lfHeight)) - abs(GlyphHeight(tm))) > 2) {
    if (lf.lfPitchAndFamily & FF_DECORATIVE)
      lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DECORATIVE;
    else
      lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    lf.lfFaceName[0] = 0;
    ::DeleteObject(::SelectObject(GetHDC(), CreateFontIndirect(&lf)));
  }
}

//
// Return the size of the given printer device in device units.
//
static TSize 
GetPageSizeInPixels(TPrintDC& d)
{
  return TSize(d.GetDeviceCaps(HORZRES), d.GetDeviceCaps(VERTRES));
}
  
//
// Set the mapping between logical units and device units.
// It is assumed that the viewport extents of the screen DC
// are already set and represent the full previewed page. 
// The window extents are set to the page size in logical units
// as defined by the printer DC.
//
void
TPrintPreviewDC::ReScale()
{
  // Get the extents of the screen viewport in device units (pixels).
  // This should represent the whole previewed page on the screen.

  TSize ve; ::GetViewportExtEx(GetHDC(), &ve);

  // Calculate the size of the previewed page in logical units.

  TSize page = GetPageSizeInPixels(PrnDC);
  TSize pve = PrnDC.GetViewportExt();
  TSize pwe = PrnDC.GetWindowExt();
  TSize we(MulDiv(page.cx, pwe.cx, pve.cx), MulDiv(page.cy, pwe.cy, pve.cy));

  // Set the mapping mode and scale.

  ::SetMapMode(GetHDC(), MM_ANISOTROPIC);
  ::SetWindowExtEx(GetHDC(), we.cx, we.cy, 0);
  ::SetViewportExtEx(GetHDC(), ve.cx, ve.cy, 0);

  // Set the origin for logical units.

  ReOrg();
}

//
// Set the mapping of the logical origin to device units.
//
void
TPrintPreviewDC::ReOrg()
{
  // Get the viewport origin of the printer DC and transform it into 
  // screen device units. It is assumed that the viewport extents of
  // the screen DC represent the whole previewed page.

  TPoint pvo = PrnDC.GetViewportOrg();
  TSize page = GetPageSizeInPixels(PrnDC);
  TSize ve; ::GetViewportExtEx(GetHDC(), &ve); // screen extents
  TPoint vo(MulDiv(pvo.x, ve.cx, page.cx), MulDiv(pvo.y, ve.cy, page.cy));

  // Use the same logical origin as the printout. 

  TPoint wo = PrnDC.GetWindowOrg();

  // Set the origins.

  ::SetWindowOrgEx(GetHDC(), wo.x, wo.y, 0);
  ::SetViewportOrgEx(GetHDC(), vo.x, vo.y, 0);
}

//
//
//
TColor
TPrintPreviewDC::SetBkColor(const TColor& color)
{
  TColor result = PrnDC.SetBkColor(color);
  ::SetBkColor(GetHDC(), PrnDC.GetBkColor());
  return result;
}

//
//
//
TColor
TPrintPreviewDC::SetTextColor(const TColor& color)
{
  TColor result = PrnDC.SetTextColor(color);
  ::SetTextColor(GetHDC(), PrnDC.GetTextColor());
  return result;
}

//
//
//
int
TPrintPreviewDC::SetMapMode(int mode)
{
  int result = PrnDC.SetMapMode(mode);
  ReScale();
  return result;
}

//
//
//
bool
TPrintPreviewDC::SetViewportOrg(const TPoint& origin, TPoint * oldOrg)
{
  bool result = PrnDC.SetViewportOrg(origin, oldOrg);
  ReOrg();
  return result;
}

//
//
//
bool
TPrintPreviewDC::OffsetViewportOrg(const TPoint& delta, TPoint * oldOrg)
{
  bool result = PrnDC.OffsetViewportOrg(delta, oldOrg);
  ReOrg();
  return result;
}

//
//
//
bool
TPrintPreviewDC::SetViewportExt(const TSize& extent, TSize * oldExtent)
{
  bool result = PrnDC.SetViewportExt(extent, oldExtent);
  ReScale();
  return result;
}

bool
TPrintPreviewDC::ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom,
                                  TSize * oldExtent)
{
  bool result = PrnDC.ScaleViewportExt( xNum, xDenom, yNum, yDenom, oldExtent);
  ReScale();
  return result;
}

bool
TPrintPreviewDC::SetWindowExt(const TSize& extent, TSize * oldExtent)
{
  bool result = PrnDC.SetWindowExt(extent, oldExtent);
  ReScale();
  return result;
}

//
//
//
bool
TPrintPreviewDC::ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom,
                                TSize * oldExtent)
{
  bool result = PrnDC.ScaleWindowExt(xNum, xDenom, yNum, yDenom, oldExtent);
  ReScale();
  return result;
}


DEFINE_RESPONSE_TABLE1(TPreviewPage, TWindow)
  EV_WM_SIZE,
END_RESPONSE_TABLE;

IMPLEMENT_CASTABLE(TPreviewPage);

//
//
//
TPreviewPage::TPreviewPage(TWindow* parent, TPrintout& printout,
                           TPrintDC& prndc, TSize& printExtent, int pagenum)
:
  TWindow(parent),
  PageNum(pagenum),
  PrintExtent(printExtent),
  Printout(printout),
  PrintDC(prndc)
{
  Attr.Style = WS_CHILD | WS_BORDER | WS_VISIBLE;
  SetBkgndColor(TColor::White);
}

//
//
//
void
TPreviewPage::SetPageNumber(int newNum)
{
  PageNum = newNum;
  if (GetHandle())
    Invalidate();
}

//
// Using a TPrintPreviewDC, 'print' the current page (PageNum) of Printout
// onto the window DC provided,
//
void
TPreviewPage::Paint(TDC& dc, bool, TRect& clip)
{
  TPrintPreviewDC pdc(dc, PrintDC, GetClientRect(), clip);
  Printout.SetPrintParams(&pdc, PrintExtent);

  if (Printout.HasPage(PageNum)) {
    Printout.BeginPrinting();
    Printout.BeginDocument(PageNum, PageNum, pfBoth);

    // Change clip rect into the shared logical coordinate space, & print
    //
    pdc.SDPtoLP(clip);
    Printout.PrintPage(PageNum, clip, pfBoth);

    Printout.EndDocument();
    Printout.EndPrinting();
  }
  else
    dc.PatBlt(0, 0, Attr.W, Attr.H, WHITENESS);
}

//
// Always repaint whole window if size is changed
//
void
TPreviewPage::EvSize(uint sizeType, TSize& size)
{
  Invalidate();
  TWindow::EvSize(sizeType, size);
}

__OWL_END_NAMESPACE
/* ========================================================================== */

