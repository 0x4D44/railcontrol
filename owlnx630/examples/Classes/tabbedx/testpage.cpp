////////////////////////////////////////////////////////////////////////////////
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/layoutwi.h> //must include for <owl/tabbed.h>
#include <owl/tabbed.h>

#include "testpage.rh"
#include <stdio.h>

class TPagedWindow: public TTabbedWindow {
	private:
		int PageNum;
	public:
		TPagedWindow();
		void CmAddPage();

	DECLARE_RESPONSE_TABLE(TPagedWindow);
};
//
DEFINE_RESPONSE_TABLE1(TPagedWindow, TTabbedWindow)
	EV_COMMAND(CM_ADDPAGE, CmAddPage),
END_RESPONSE_TABLE;
//
TPagedWindow::TPagedWindow()
{
	PageNum = 1;

	TWindow* ptrPage1 = new TWindow(this);
	TWindow* ptrPage2 = new TWindow(this);

	Add(*ptrPage1, _T("First"));
	Add(*ptrPage2, _T("Second"));

	ptrPage1->SetBkgndColor(TColor::LtYellow);
	ptrPage1->ModifyExStyle(0,WS_EX_CLIENTEDGE);
	ptrPage2->SetBkgndColor(TColor::LtGreen);
	ptrPage2->ModifyExStyle(0,WS_EX_CLIENTEDGE);

	GetTabControl()->SetSel(0);
}
//
void TPagedWindow::CmAddPage()
{
	 _TCHAR PageName[15];
	 wsprintf(PageName, _T("New Page %d"), PageNum++);
	 TWindow* wnd = new TWindow(this,PageName);
	 wnd->ModifyExStyle(0, WS_EX_CLIENTEDGE);
	 wnd->SetBkgndColor(TColor::LtBlue);

	 AddPage(*wnd);
}

//
class TestApp : public TApplication {
	public:
		TestApp() : TApplication(_T("TabWindow Tester")){}
		void InitMainWindow();
};
//
void
TestApp::InitMainWindow()
{
	TFrameWindow* frame = new TFrameWindow(0, _T("TabWindow Tester"), new TPagedWindow);
	frame->AssignMenu(IDM_MENU1);
	SetMainWindow(frame);
}

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
	return TestApp().Run();
}
//////////////////
