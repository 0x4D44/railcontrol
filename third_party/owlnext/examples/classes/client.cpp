#include "pch.h"
#include "client.h"

#include "thmsdlg.h"
#include "checklstdlg.h"
#include "monthcaldlg.h"
#include "sliderdlg.h"
#include "urllinkdlg.h"
#include "glyphbtndlg.h"
#include "employeedlg.h"
#include "gridpicdlg.h"
#include "ledclockdlg.h"
#include "static.h"

#include "scrollwnd.h"
#include "treeviewwnd.h"
#include "listviewwnd.h"
#include "bmpviewwnd.h"

#include "copydatawnd.h"
#include "cpuinfownd.h"
#include "exceptiontransport.h"

#include "tabctrl.h"

#include "layoutwnd.h"

#include "resizabledialogs1.h"

#include "treelist.h"
#include "draglistwnd.h"

#include "progressdlg.h"

#include "resource.h"
#include "tabctrl.rh"
#include "tabbed.rh"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TClassesMDIClient, TMDIClient)
  EV_COMMAND(CM_CONTROLS, CmControls),
  EV_COMMAND(CM_TCHECKLIST, CmTCheckList),
  EV_COMMAND(CM_TMONTHCALENDAR, CmTMonthCalendar),
  EV_COMMAND(CM_TSLIDER, CmTSlider),
  EV_COMMAND(CM_TURLLINK, CmTUrlLink),
  EV_COMMAND(CM_TGLYPHBUTTON, CmTGlyphButton),
  EV_COMMAND(CM_TVALIDATE, CmTValidate),
  EV_COMMAND(CM_TNOTETAB, CmTNoteTab),
  EV_COMMAND(CM_TGRIDPICKER, CmTGridPicker),
  EV_COMMAND(CM_TLEDCLOCK, CmTLedClock),
  EV_COMMAND(CM_TSTATIC, CmTStatic),
  EV_COMMAND(CM_TPROGRESSDLG, CmTProgressDlg),

  EV_COMMAND(CM_TSCROLLER, CmTScroller),
  EV_COMMAND(CM_TTREEVIEW, CmTTreeView),
  EV_COMMAND(CM_TLISTVIEW, CmTListView),
  EV_COMMAND(CM_TBITMAPVIEW, CmTBitmapView),
  EV_COMMAND(CM_TTABCONTROL, CmTTabControl),
  EV_COMMAND(CM_TTABBEDWINDOW, CmTTabbedWindow),
  EV_COMMAND(CM_OPENWINDOW, CmOpenTTabbedWindow),
  EV_COMMAND(CM_TLAYOUTWINDOW, CmTLayoutWindow),
  EV_COMMAND(CM_TTREELIST, CmTTreeList),
  EV_COMMAND(CM_TDRAGLIST, CmTDragList),

  EV_COMMAND(CM_COPYDATA, CmCopyData),
  EV_COMMAND(CM_CPUINFO, CmCpuInfo),
  EV_COMMAND(CM_EXCEPTIONTRANSPORT, CmExceptionTransport),
  EV_COMMAND(CM_OPENDIALOG, CmOpenDialog),

  EV_COMMAND(CM_DIALOGDATATRANSFER, CmDialogDataTransfer),
  EV_COMMAND(CM_TRANSFER, CmTransfer),
  EV_COMMAND(CM_TRANSFERBUFFER, CmTransferBuffer),

  EV_COMMAND(CM_TOGGLECAPTION, CmToggleCaption),
  EV_COMMAND_ENABLE(CM_TOGGLECAPTION, CeToggleCaption),

  EV_COMMAND(CM_RESIZABLEDIALOG1, CmResizableDialog1),
  EV_COMMAND(CM_REGISTRY, CmRegistry),
END_RESPONSE_TABLE;


TClassesMDIClient::TClassesMDIClient()
{
}


TClassesMDIClient::~TClassesMDIClient()
{
}

void TClassesMDIClient::CmControls()
{
  TThemesDialog dialog(this);
  dialog.Execute();
}

void TClassesMDIClient::CmTCheckList()
{
  TCheckListXDialog dialog(this);
  dialog.Execute();
}

void TClassesMDIClient::CmTMonthCalendar()
{
  TSystemTime date(2011, 4, 6);

  TCalendarDialog dialog(this, date);
  if (dialog.Execute() == IDOK)
  {
    _TCHAR buf[200];
    wsprintf(buf, _T("Year: %d, Month: %d, Day: %d"), date.GetYear(), date.GetMonth(), date.GetDay());
    MessageBox(buf, _T("Selected date"), MB_OK);
  }
}

void TClassesMDIClient::CmTSlider()
{
  TSliderDialog dialog(this);
  dialog.Execute();
}

void TClassesMDIClient::CmTUrlLink()
{
  TUrlLinkDialog dialog(this);
  dialog.Execute();
}

void TClassesMDIClient::CmTGlyphButton()
{
  TGlyphButtonDialog dialog(this);
  dialog.Execute();
}

void TClassesMDIClient::CmTValidate()
{
  static TEmployeeInformation data =
  {
    _T("Frank Foo"), // Name
    _T("123-45-6789"), // SocialSecurityNumber
    _T("123"), // EmployeeId
    _T("Sales"), // Department
    _T("15"), // SecurityClearance
    _T("1.2345"), // Coefficient
    true, // FullTime
    true, // Permanent
    true, // Exempt
    _T("999"), // CustomData
    _T(""), // Address
  };
  TEmployeeDlg dlg(this, data);
  if (dlg.Execute() != IDOK) return;

  tostringstream report;
  report << _T("Name ") << data.Name << _T('\n')
    << _T("SS#: ") << data.SocialSecurityNumber << _T('\n')
    << _T("Emp. ID: ") << data.EmployeeId << _T('\n')
    << _T("Dept: ") << data.Department << _T('\n')
    << _T("Security: ") << data.SecurityClearance << _T('\n')
    << _T("Coefficient: ") << data.Coefficient << _T('\n')
    << _T("State: ") << (data.FullTime ? _T("FullTime ") : _T("PartTime "))
    << (data.Permanent ? _T("Permanent ") : _T("Temporary "))
    << (data.Exempt ? _T("Exempt ") : _T("NonExempt ")) << _T('\n')
    << _T("Custom data: ") << data.CustomData << _T('\n')
    << _T("Address: ") << data.Address;
  MessageBox(report.str(), _T("Employee Information"));
}

void TClassesMDIClient::CmTScroller()
{
  TMDIChild* child = new TMDIChild(*this, _T("TScroller"), new TScrollWindow());
  child->SetMenuDescr(TMenuDescr(IDM_DUMMY, 0, 0, 0, 0, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTTreeView()
{
  TMDIChild* child = new TMDIChild(*this, _T("TTreeView"), new TTreeViewWindow(0));
  child->SetMenuDescr(TMenuDescr(IDM_TREEVIEWMENU, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTListView()
{
  TMDIChild* child = new TMDIChild(*this, _T("TListView"), new TListViewWindow(0));
  child->SetMenuDescr(TMenuDescr(IDM_DUMMY, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTBitmapView()
{
  TMDIChild* child = new TMDIChild(*this, _T("TBitmapView"), new TBmpViewWindow);
  child->SetMenuDescr(TMenuDescr(IDM_BMPVIEWMENU, 1, 0, 0, 1, 0, 0));
  child->SetIcon(GetModule(), TResId(IDI_BMPVIEWICON));
  child->SetIconSm(GetModule(), TResId(IDI_BMPVIEWICON));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmCopyData()
{
  TMDIChild* child = new TMDIChild(*this, _T("WM_COPYDATA"), new TCopyDataWindow(0));
  child->SetMenuDescr(TMenuDescr(IDM_COPYDATA, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmCpuInfo()
{
  TMDIChild* child = new TMDIChild(*this, _T("CPU Info"), new TCpuInfoWindow());
  child->SetMenuDescr(TMenuDescr(IDM_DUMMY, 0, 0, 0, 0, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmExceptionTransport()
{
  try
  {
    TExceptionalClient dialog(this);
    dialog.Execute();
  }
  catch (const std::exception& x)
  {
    TRACE(_T("Exiting due to unhandled std::exception: ") << x.what()); InUse(x);
    throw;
  }
  catch (const TXEndSession&)
  {
    TRACE(_T("Exiting due to WM_ENDSESSION."));
    throw;
  }
  catch (int x)
  {
    TRACE(_T("Exiting due to unhandled exception of type 'int': ") << x); InUse(x);
    throw;
  }
  catch (...)
  {
    TRACE(_T("Exiting due to unknown unhandled exception."));
    throw;
  }
}

void TClassesMDIClient::CmTTabControl()
{
  TTabControl* tab = new TTabControl(0, TabCtlID, 0, 0, 0, 0);

  TMDIChild* child = new TTabControlWindow(*this, _T("Tab Control"), tab);
  child->SetMenuDescr(TMenuDescr(1000, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTNoteTab()
{
  TNoteTabDlg(this, IDD_NOTETAB1).Execute();
}

void TClassesMDIClient::CmToggleCaption()
{
  TWindow* w = GetActiveMDIChild(); CHECK(w);
  w->ShowWindow(SW_HIDE);
  if (w->GetStyle() & WS_CAPTION)
    w->ModifyStyle(WS_CAPTION, 0);
  else
    w->ModifyStyle(0, WS_CAPTION);
  w->ShowWindow(SW_SHOW);
  w->SetFocus();
}

void TClassesMDIClient::CeToggleCaption(TCommandEnabler& c)
{
  c.Enable(GetActiveMDIChild() != NULL);
}

void TClassesMDIClient::CmTLayoutWindow()
{
  TMDIChild* child = new TMDIChild(*this, _T("TLayoutWindow"), new TLayoutDemoWindow(0));
  child->SetMenuDescr(TMenuDescr(IDM_LAYOUT, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmResizableDialog1()
{
  TSampleResizableDialog1 dlg(this);
  dlg.SetAppRegKeyName(_T("OWLNextClasses"));
  dlg.SetRegValuePrefix(_T("SampleResizableDialog1"));
  dlg.Execute();
}

void TClassesMDIClient::CmTTreeList()
{
  TMDIChild* child = new TMDIChild(*this, _T("TTreeList"), new TTreeList(0));
  child->SetMenuDescr(TMenuDescr(IDM_DUMMY, 0, 0, 0, 0, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTGridPicker()
{
  TGridPickerDialog dlg(this);
  dlg.Execute();
}

void TClassesMDIClient::CmTStatic()
{
  auto d = CreateStaticDemoDialog(this);
  d->Execute();
}

void TClassesMDIClient::CmTLedClock()
{
  TLedClockDialog dlg(this);
  dlg.Execute();
}

void TClassesMDIClient::CmTDragList()
{
  TMDIChild* child = new TMDIChild(*this, _T("TDragList"), new TDragListWindow());
  child->SetMenuDescr(TMenuDescr(IDM_DUMMY, 0, 0, 0, 0, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmTProgressDlg()
{
  ShowProgessDialog(this);
}