#pragma once
#include <owl/window.h>
#include <owl/dibitmap.h>
#include <owl/clipview.h>

//
// class TBmpViewWindow
// ~~~~~ ~~~~~~~~~~~~~~
// A bitmap displaying window derived from TClipboardViewer to
// facilitate receiving of clipboard change notifications. Could mix it in if
// an additional base was desired.
//
class TBmpViewWindow : virtual public owl::TWindow,
  public owl::TClipboardViewer {
public:
  bool      ScreenPalettized;  // True if the screen is a palettized device
  owl::TPalette* DefPalette;        // Palette to use for DIBs w/o color tables
  owl::TBrush*   BkgndBrush;        // Window background brush
  long      Rop;               // Bitmap -> screen raster op

  bool      Fit;               // Stretch bitmap to fit?
  bool      AutoClipView;      // Clipboard auto-display mode
  bool      PaintFromDDB;      // Paint DDB to screen, vs. DIB
  bool      PaintIcon;         // Paint icon from DIB, vs. from resource

  owl::tstring    FileName;
  owl::TDib*     Dib;
  owl::TBitmap*  Bitmap;
  HBITMAP   MemHandle;
  owl::TPalette* Palette;
  int       PixelWidth;
  int       PixelHeight;
  owl::uint16    Colors;

  TBmpViewWindow();
  ~TBmpViewWindow();

protected:
  void      CmFileOpen();
  void      CmRead1();
  void      CmRead2();
  void      CmReadBad();
  void      CmMemHandle();
  void      CmMemHandleRefresh();
  void      CmCopy();
  void      CmPaste();
  void      CmFit();
  void      CmAutoClipView();
  void      CmPaintFromDDB();
  void      CmPaintIcon();
  void      CeCopy(owl::TCommandEnabler& ce);
  void      CePaste(owl::TCommandEnabler& ce);
  void      CeFit(owl::TCommandEnabler& ce);
  void      CeAutoClipView(owl::TCommandEnabler& ce);
  void      CePaintFromDDB(owl::TCommandEnabler& ce);
  void      CePaintIcon(owl::TCommandEnabler& ce);

  void      Paint(owl::TDC&, bool erase, owl::TRect&);
  void      EvSize(owl::uint sizeType, const owl::TSize&);

  void      EvPaletteChanged(HWND hWndPalChg);
  bool      EvQueryNewPalette();
  void      EvDrawClipboard();
  void      EvDestroy();

  bool      UpdatePalette(bool alwaysRepaint);
  void      AdjustScroller();
  void      SetCaption(LPCTSTR);
  void      SetupFromDib(owl::TDib* dib);
  bool      LoadBitmapFile(LPCTSTR);
  bool      LoadBitmapResource(owl::uint16 resId);
  bool      LoadMemHandle(HBITMAP hBitmap);

  DECLARE_RESPONSE_TABLE(TBmpViewWindow);
};
