//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Bidus Yura, All Rights Reserved
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_MONTHCAL_H)
# include <owl/monthcal.h>
#endif

#include <owl/dialog.h>
#include <owl/framewin.h>

#include "monthcal.rh"
//
// Class TSampleApp
// ~~~~~ ~~~~~~~~~~
class TSampleApp : public TApplication {
  public:
    void    InitMainWindow();
};

//
// Class TClientWindow
// ~~~~~ ~~~~~~~~~~~~~
class TClientWindow : public TDialog {
  public:
    TClientWindow(TWindow* parent = 0);
};

TMonthCalendarData Data(TSystemTime(2011,4,6));

TClientWindow::TClientWindow(TWindow* parent)
:
 TDialog(parent, IDD_CLIENTDLG, 0)
{
	new TMonthCalendar(this,IDC_MONTHCAL);
  SetTransferBuffer(&Data);
}

void
TSampleApp::InitMainWindow()
{
  SetMainWindow(new TFrameWindow(0, _T("Month Calendar"),
                                 new TClientWindow(), true));
  GetMainWindow()->ModifyStyle(WS_THICKFRAME,0);
}

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
 return TSampleApp().Run();
}
