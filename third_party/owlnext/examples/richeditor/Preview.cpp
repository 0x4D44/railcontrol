#include <owl/pch.h>

#include "Preview.h"
#include "App.rh"
#include <owl/buttonga.h>
#include <owl/textgadg.h>
#include <owl/prevwin.rh>
#include <owl/editfile.rh>

using namespace owl;

DEFINE_RESPONSE_TABLE1(TPreview, TDecoratedFrame)
  EV_COMMAND_ENABLE(IDB_PREVIOUS, PPR_PreviousEnable),
  EV_COMMAND_ENABLE(IDB_NEXT, PPR_NextEnable),
  EV_COMMAND(IDB_PREVIOUS, PPR_Previous),
  EV_COMMAND(IDB_NEXT, PPR_Next),
  EV_COMMAND(IDB_ONEUP, PPR_OneUp),
  EV_COMMAND_ENABLE(IDB_TWOUP, PPR_TwoUpEnable),
  EV_COMMAND(IDB_TWOUP, PPR_TwoUp),
  EV_COMMAND(IDB_DONE, PPR_Done),
  EV_COMMAND(CM_FILEPRINT, CmPrint),
  EV_COMMAND_ENABLE(CM_FILEPRINT, CmPrintEnable),
  EV_WM_CLOSE,
END_RESPONSE_TABLE;

TPreview::TPreview(
  TWindow* parentWindow, 
  TPrinter* printer,
  TWindow* currWindow, 
  LPCTSTR title,
  TLayoutWindow* client
  )
  : TDecoratedFrame(parentWindow, title, client)
{
  CurrWindow = currWindow;
  Printer = printer;
  Client = client;
  Page1 = 0;
  Page2 = 0;
  FromPage = 1;
  ToPage = 1;

  TPrintDialog::TData& data = Printer->GetSetup();
  PrnDC = new TPrintDC{data.GetDeviceName(), data.GetDevMode()};

  PrintExtent = new TSize(PrnDC->GetDeviceCaps(HORZRES), PrnDC->GetDeviceCaps(VERTRES));
  Printout = new TTextPrintout(Printer, _T("Print Preview"), currWindow, true);

  SetBkgndColor(TColor::SysAppWorkspace);
  
  // Create default toolbar New and associate toolbar buttons with commands.
  //
  PreviewSpeedBar = new TControlBar(this);
  PreviewSpeedBar->Insert(*new TButtonGadget(IDB_PREVIOUS, IDB_PREVIOUS, TButtonGadget::Command, true));
  PreviewSpeedBar->Insert(*new TButtonGadget(IDB_NEXT, IDB_NEXT, TButtonGadget::Command, true));
  PreviewSpeedBar->Insert(*new TSeparatorGadget(6));
  PreviewSpeedBar->Insert(*new TButtonGadget(IDB_ONEUP, IDB_ONEUP, TButtonGadget::Exclusive, true, TButtonGadget::Down));
  PreviewSpeedBar->Insert(*new TButtonGadget(IDB_TWOUP, IDB_TWOUP, TButtonGadget::Exclusive, true));
  PreviewSpeedBar->Insert(*new TSeparatorGadget(12));
  PreviewSpeedBar->Insert(*new TTextGadget(IDB_CURRPAGE, TGadget::Recessed, TTextGadget::Left, 10, _T("Page 1")));
  PreviewSpeedBar->Insert(*new TSeparatorGadget(20));
  PreviewSpeedBar->Insert(*new TButtonGadget(CM_FILEPRINT, CM_FILEPRINT, TButtonGadget::Command, true));
  PreviewSpeedBar->Insert(*new TSeparatorGadget(20));
  PreviewSpeedBar->Insert(*new TButtonGadget(IDB_DONE, IDB_DONE, TButtonGadget::Command, true));
  Insert(*PreviewSpeedBar, TDecoratedFrame::Top);

  Attr.Style &= ~WS_VISIBLE;
  SetAcceleratorTable(IDM_PRINTPREVIEW);
}

TPreview::~TPreview()
{
  delete Page1;
  delete Page2;
  delete PrnDC;
  delete PrintExtent;
  delete Printout;
}

void TPreview::SetupWindow()
{
  TDecoratedFrame::SetupWindow();

  // Set the caption of the preview window based on that of the Main Window.
  //
  TAPointer<_TCHAR> captionText(new _TCHAR[256]);
  GetApplication()->GetMainWindow()->GetWindowText(captionText, 256);
  _tcscat(captionText, _T(" (Preview)"));
  SetCaption(captionText);

  // Set the icons of the preview window.
  //
  SetIcon(GetApplication(), IDI_MDIAPPLICATION);
  SetIconSm(GetApplication(), IDI_MDIAPPLICATION);

  TPrintDialog::TData& data = Printer->GetSetup();
  Page1 = new TPreviewPage(Client, *Printout, *PrnDC, *PrintExtent, 1);
  Page1->SetPageNumber(1);
  data.MaxPage = 1;

  Page2 = 0;

  TLayoutMetrics metrics1;
  metrics1.X.Set(lmLeft, lmRightOf, lmParent, lmLeft, 15);
  metrics1.Y.Set(lmTop, lmBelow, lmParent, lmTop, 15);

  // Determine major axis of preview page, have that follow parent size.
  // Make minor axis a percentage (aspect ratio) of the page's major axis
  //
  TRect r = Client->GetClientRect();
  long ratio;
  if (PrintExtent->cx > PrintExtent->cy)
    ratio = ((long)PrintExtent->cy * 100) / PrintExtent->cx;
  else
    ratio = ((long)PrintExtent->cx * 100) / PrintExtent->cy;

  bool xMajor = ((r.Width() * ratio) / 100) > r.Height();
  if (xMajor)
  {
    metrics1.Height.Set(lmBottom, lmAbove, lmParent, lmBottom, 15);
    metrics1.Width.PercentOf(Page1, (int)((long)PrintExtent->cx * 95 / PrintExtent->cy), lmHeight);
  }
  else
  {
    metrics1.Height.PercentOf(Page1, (int)((long)PrintExtent->cy * 95 / PrintExtent->cx), lmWidth);
    metrics1.Width.Set(lmRight, lmLeftOf, lmParent, lmRight, 15);
  }

  Page1->Create();

  Client->SetChildLayoutMetrics(*Page1, metrics1);
  Client->Layout();
}

void TPreview::SpeedBarState()
{
  // Update the page count.
  //
  TTextGadget* cpGadget = dynamic_cast<TTextGadget*>(PreviewSpeedBar->GadgetWithId(IDB_CURRPAGE));
  if (cpGadget)
  {
    TAPointer<_TCHAR> buffer(new _TCHAR[32]);

    if (Page2 && FromPage != ToPage)
      wsprintf(buffer, _T("Page %d - %d"), FromPage, ToPage);
    else
      wsprintf(buffer, _T("Page %d"), FromPage);
    cpGadget->SetText(buffer);
  }
}

void TPreview::PPR_PreviousEnable(TCommandEnabler& tce)
{
  // Only have previous on if we're not at the first page.
  //
  tce.Enable(FromPage != 1);
}

void TPreview::PPR_NextEnable(TCommandEnabler& tce)
{
  // Only have next on if we're not at the last page.
  //
  TPrintDialog::TData& printerData = Printer->GetSetup();
  tce.Enable(ToPage != printerData.MaxPage);
}

void TPreview::PPR_Previous()
{
  TPrintDialog::TData& printerData = Printer->GetSetup();
  if (FromPage > printerData.MinPage)
  {
    FromPage--;
    ToPage--;
    Page1->SetPageNumber(FromPage);
    if (Page2)
      Page2->SetPageNumber(ToPage);
  }
  SpeedBarState();
}

void TPreview::PPR_Next()
{
  TPrintDialog::TData& printerData = Printer->GetSetup();
  if (ToPage < printerData.MaxPage)
  {
    FromPage++;
    ToPage++;
    Page1->SetPageNumber(FromPage);
    if (Page2)
      Page2->SetPageNumber(ToPage);
  }
  SpeedBarState();
}

void TPreview::PPR_OneUp()
{
  if (Page2)
  {
    Client->RemoveChildLayoutMetrics(*Page2);
    delete Page2;
    Page2 = 0;
    Client->Layout();
    ToPage = FromPage;
    SpeedBarState();
  }
}

void TPreview::PPR_TwoUpEnable(TCommandEnabler& tce)
{
  // Two up is only available for portrait mode.
  //
  tce.Enable(PrintExtent->cx <= PrintExtent->cy);
}

void TPreview::PPR_TwoUp()
{
  if (!Page2)
  {
    Page2 = new TPreviewPage(Client, *Printout, *PrnDC, *PrintExtent, PageNumber + 1);
    Page2->Create();

    TLayoutMetrics metrics2;
    metrics2.X.Set(lmLeft, lmRightOf, Page1, lmRight, 30);
    metrics2.Y.SameAs(Page1, lmTop);

    // Assume portrait.
    //
    metrics2.Width.SameAs(Page1, lmWidth);
    metrics2.Height.SameAs(Page1, lmBottom);
    Client->SetChildLayoutMetrics(*Page2, metrics2);
    Client->Layout();

    // Page 2 is the next page.  If the next page is outside of our range then
    // set the first page back one and the 2nd page is the current page. If the
    // document is only 1 page long then the 2nd page is empty.
    //
    TPrintDialog::TData& printerData = Printer->GetSetup();
    if (FromPage == printerData.MaxPage)
    {
      if (FromPage > 1)
      {
        FromPage--;
        ToPage = FromPage + 1;
        Page1->SetPageNumber(FromPage);
        Page2->SetPageNumber(ToPage);
      }
      else
        Page2->SetPageNumber(0);
    }
    else
    {
      ToPage = FromPage + 1;
      Page2->SetPageNumber(ToPage);
    }
    SpeedBarState();
  }
}

void TPreview::PPR_Done()
{
  // Don't call the base class EvClose; we do not want TPreview to be destructed.
  //
  GetApplication()->EndModal(IDCANCEL);
}

void TPreview::EvClose()
{
  // Don't call the base class EvClose; we do not want TPreview to be destructed.
  //
  GetApplication()->EndModal(IDCANCEL);
}

void TPreview::CmPrint()
{
  TWindow* client = GetApplication()->GetMainWindow()->GetClientWindow();

  if (client)
    client->SendMessage(WM_COMMAND, CM_FILEPRINT, 0);
}

void TPreview::CmPrintEnable(TCommandEnabler& tce)
{
  tce.Enable(true);
}
