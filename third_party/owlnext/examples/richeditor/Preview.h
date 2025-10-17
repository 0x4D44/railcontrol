#ifndef PREVIEW_H
#define PREVIEW_H

#include "TextPrintout.h"
#include <owl/decframe.h>
#include <owl/controlb.h>
#include <owl/preview.h>

class TPreview
  : public owl::TDecoratedFrame
{
public:

  TPreview(
    owl::TWindow* parentWindow, 
    owl::TPrinter* printer, 
    owl::TWindow* currWindow, 
    LPCTSTR title, 
    owl::TLayoutWindow* client);

  virtual ~TPreview();

protected:

  virtual void SetupWindow(); // override

  void EvClose();

private:

  owl::TLayoutWindow* Client;
  int PageNumber, FromPage, ToPage;
  owl::TWindow* CurrWindow;
  owl::TControlBar* PreviewSpeedBar;
  owl::TPreviewPage* Page1;
  owl::TPreviewPage* Page2;
  owl::TPrinter* Printer;
  owl::TPrintDC* PrnDC;
  owl::TSize* PrintExtent;
  TTextPrintout* Printout;

  void SpeedBarState();
  void PPR_PreviousEnable(owl::TCommandEnabler& tce);
  void PPR_NextEnable(owl::TCommandEnabler& tce);
  void PPR_Previous();
  void PPR_Next();
  void PPR_OneUp();
  void PPR_TwoUpEnable(owl::TCommandEnabler& tce);
  void PPR_TwoUp();
  void PPR_Done();
  void CmPrintEnable(owl::TCommandEnabler& tce);
  void CmPrint();

  DECLARE_RESPONSE_TABLE(TPreview);
};

#endif
