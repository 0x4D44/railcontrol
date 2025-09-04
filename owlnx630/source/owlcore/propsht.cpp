//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TPropertyPage and TPropertySheet classes
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#if !defined(OWL_PROPSHT_H)
# include <owl/propsht.h>
#endif
#if !defined(OWL_TABCTRL_H)
# include <owl/tabctrl.h>
#endif
#if !defined(OWL_RESOURCE_H)
# include <owl/resource.h>
#endif
#if !defined(OWL_PROPSHT_RH)
# include <owl/propsht.rh>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif

#if !defined MAXPROPPAGES
#define MAXPROPPAGES 100
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;


//----------------------------------------------------------------------------
// TPropertySheet
//----------------------------------------------------------------------------

const uint PropSheetTimerID = 0xBACB;
const uint PropSheetTimeOut = 100;

DEFINE_RESPONSE_TABLE1(TPropertySheet, TWindow)
  EV_WM_TIMER,
END_RESPONSE_TABLE;

//
// Constructs a propertySheet object
//
TPropertySheet::TPropertySheet(TWindow* parent, LPCTSTR title,
                               uint startPage, bool isWizard,
                               uint32 flags, TModule* module)
:
  TWindow(parent, title, module), 
  SubClassSheet(false),
  WantTimer(false),
  TimerID(0),
  IsWizard(isWizard)
{
  // Initialize the structure representing the property sheet
  //
  memset(&HeaderInfo, 0, sizeof(HeaderInfo));
  HeaderInfo.dwSize = sizeof(PROPSHEETHEADER);
  
#if (_WIN32_IE > 0x0400)
//   if (TCommCtrl::Dll() != NULL && TCommCtrl::Dll()->GetVersion() < MkUint32((uint16)71, (uint16)4))
//   {
//     HeaderInfo.dwSize = PROPSHEETHEADER_V1_SIZE;
//   }
#endif 
  
  HeaderInfo.dwFlags= flags;
  if (IsWizard)
    HeaderInfo.dwFlags |= PSH_WIZARD;
  HeaderInfo.hInstance = *GetModule();
  HeaderInfo.pszCaption = title;
  HeaderInfo.nStartPage = startPage;

}

//
// Clean up resources used by PropertySheet object
//
TPropertySheet::~TPropertySheet()
{
}

//
//
bool
TPropertySheet::EnableTimer()
{
  // Set flag and let 'SetupWindow' create timer
  //
  WantTimer = true;
  if (!GetHandle())
    return true;

  // Set a timer if one's not enabled already
  //
  if(!TimerID)
    TimerID = SetTimer(PropSheetTimerID, PropSheetTimeOut);

  return TimerID != 0;
}

//
// Computes the size of the sheet, activates the designated 'startPage'
// and sets focus to the tab control...
//
void
TPropertySheet::SetupWindow()
{
  TWindow::SetupWindow();

  // if 'WantTimer' is enabled, start a timer
  //
  if (WantTimer)
    EnableTimer();
}

//
//
//
void
TPropertySheet::CleanupWindow()
{
  // Cleanup pending timer
  //
  if (TimerID && KillTimer(TimerID)) {
    TimerID = 0;
  }

  // Chain to base class' version
  //
  TWindow::CleanupWindow();
}

//
// Handler for the timer event.
// Closes the window.
//
void
TPropertySheet::EvTimer(uint timerId)
{
  if(timerId == TimerID)
    IdleAction(0);
}

//
// Updates the caption of the property sheet
//
void
TPropertySheet::SetCaption(LPCTSTR title)
{
  // Here we'll be a little flexible and allow a call to SetCaption
  // even before the underlying window element has been created...
  //
  if (GetHandle())
    SetTitle(title, 0);
  else {
    // TWindow's implementation will cache a copy of the string in the
    // data member 'Title'
    //
    TWindow::SetCaption(title);
    HeaderInfo.pszCaption = Title;
  }
}

//
// Executes modal propertysheet via call to 'Run' method
//
int
TPropertySheet::Execute()
{
  // Use CommCtrl via 'Run' helper method
  //
  return Run(true);
}

//
// Creates a modeless property sheet.
//
bool
TPropertySheet::Create()
{
  // Use CommCtrl via 'Run' helper method
  //
  return Run(false) != 0;
}

//
// Internal routine use allow each page to register the classes
// of it's children
//
void
TPropertyPage::RegisterPageChildObjects(TPropertyPage* page, void*)
{
  CHECK(page);
  page->RegisterChildObjects();
}

//
// Brings up modal/modeless propertysheet by invoking COMMCTRL's API.
//
int
TPropertySheet::Run(bool modal)
{
  // At this point our C++ parent object has (should have!) a valid HWND - 
  // hence update our Header structure
  //
  PRECONDITION(Parent->GetHandle());
  HeaderInfo.hwndParent = Parent->GetHandle();

  // We're about to use the Common Control Library's implementation
  // of PropertySheet - Check that it's available
  //
  CHECK(TCommCtrl::IsAvailable());

  HPROPSHEETPAGE pHandle[MAXPROPPAGES];

  // Have each page give us its handle
  //
  HeaderInfo.nPages = 0;
  HeaderInfo.phpage = pHandle;
  GetPageHandles();

  // Have each page register the window classes of it's children
  //
  ForEachPage(TActionPageFunc(TPropertyPage::RegisterPageChildObjects), 0);

  // Flag modal vs. modeless sheet
  //
  if (modal) {
    HeaderInfo.dwFlags &= ~PSH_MODELESS;
  }
  else {
    // Subclass the control so that this window will receive
    // button click notifications
    //
    EnableSubclass(true);
    HeaderInfo.dwFlags |= PSH_MODELESS;
  }

  // Display property sheet
  //
  return TCommCtrl::Dll()->PropertySheet(&HeaderInfo);
}

//
// Internal routine used to add the page handle of a 'TPropertyPage'
// pointer to the array of page handles stored in a PROPSHEETHEADER
// structure.
//
static void
fillPageInfo(TPropertyPage* page, PROPSHEETHEADER* pHeader)
{
  CHECK(page);
  CHECK(pHeader);
  CHECK(pHeader->phpage != 0);
  pHeader->phpage[pHeader->nPages] = page->CreatePropertyPage();
  pHeader->nPages++;
}

//
// Have each page of dialog hand over the page handle.
//
bool
TPropertySheet::GetPageHandles()
{
#if defined(__TRACE) || defined(__WARN)
  // Retrieve number of pages in this sheet's child list
  //
  int pageCount = GetPageCount();

  // A sheet must have a least one page
  //
  CHECK(pageCount);
  CHECK(pageCount <= MAXPROPPAGES);
#endif

  // Have each page hand over it's handle
  //
  ForEachPage(TActionPageFunc(fillPageInfo), &HeaderInfo);

#if defined(__TRACE) || defined(__WARN)
  // Double check count
  //
  CHECK(pageCount == (int)HeaderInfo.nPages);
#endif
  return true;
}

//
// Applies the specified 'action' function to each TPropertyPage
// child of the Sheet. 
// NOTE: The logic here traverses the TPropertySheet's ChildList. Therefore
//       we will miss any page that does not have an associated TPropertyPage
//       inserted in the sheet's ChildList.
void
TPropertySheet::ForEachPage(TActionPageFunc action, void* paramList)
{
  if (GetLastChild()) {
    TWindow*  curChild;
    TWindow*  nextChild = GetLastChild()->Next();
    TPropertyPage* curPage;
    do {
      curChild = nextChild;
      nextChild = nextChild->Next();
      curPage = TYPESAFE_DOWNCAST(curChild, TPropertyPage);
      if (curPage)
        action(curPage, paramList);
    } while (curChild != GetLastChild() && GetLastChild() != 0);
  }
}

//
// Applies the specified 'test' function to each 'TPropertyPage'
// of  the sheet and returns the first page which causes the
// 'test' function to return true. Returns '0' if no page meets
// the condition.
//
TPropertyPage*
TPropertySheet::FirstPageThat(TCondPageFunc test, void* paramList)
{
  if (GetLastChild()) {
    TWindow*  curChild;
    TWindow*  nextChild = GetLastChild()->Next();
    TPropertyPage* curPage;
    do {
      curChild = nextChild;
      nextChild = nextChild->Next();
      curPage = TYPESAFE_DOWNCAST(curChild, TPropertyPage);
      if (curPage) {
        if (test(curPage, paramList))
          return curPage;
      }
    } while (curChild != GetLastChild() && GetLastChild() != 0);
  }
  return 0;
}

//
// Internal callback used to count the number of pages within
// a Property Sheet.
//
static void
countPages(TPropertyPage* /*page*/, int* pCount)
{
  (*pCount)++;
}

//
// Retrieves the number of pages within a particular sheet
//
int
TPropertySheet::GetPageCount() const
{
  int pageCount = 0;
  CONST_CAST(TPropertySheet*,
             this)->ForEachPage(TActionPageFunc(countPages), &pageCount);
  return pageCount;
}

//
// Updates the 'HWINDOW' data member of the PropertySheet Object.
// NOTE: This method is called from the InitHandle method of a
//       page of the sheet.
//
void
TPropertySheet::InitHandle(HWND sheetHandle)
{
  PRECONDITION(GetHandle()==0);
  PRECONDITION(::IsWindow(sheetHandle));
  SetHandle(sheetHandle);

  // When using the system's underlying implementation of PropertySheet
  // should we subclass the Sheet or should be it be treated as a black
  // box? Ideally the Sheet is this abstract container and we're only
  // concerned with our pages [dialogs]. However, there are scenarios where
  // we might want to subclass it. For example, if the sheet is used as a
  // client of a framewindow and 'ShrinkToClient' is enabled, we'll need to
  // detect when the sheet is resized (i.e. receiving WM_SIZE messages) to
  // allow the frame to adjust.
  //
  if (SubClassSheet) {
    SubclassWindowFunction();
    GetHWndState(true);
  }

  // Here we 'fake' a SetupWindow call - The typical OWL run-through
  // (i.e. SetupWindow invoked off WM_CREATE) fails in this case since
  // the sheet is created indirectly.
  //
  SetupWindow();
}

//
// Adds a new page to the end of the PropertySheet.
// NOTE: The 'pg' must have been created via a call to
//       'TPropertyPage::CreatePropertyPage' before
//       invoking the 'AddPage' method.
// NOTE: The property sheet is not resized to fit the new page. The
//       new page should be no larger that the largest page already
//       in the property sheet.
//
void
TPropertySheet::AddPage(TPropertyPage& pg)
{
  // Update pointer to parent object
  //
  if (pg.GetParentO() != this)
    pg.SetParent(this);

  // Have page create itself it necessary
  //
  pg.CreatePropertyPage();
  CHECK(HPROPSHEETPAGE(pg));

  // Inform sheet about new page
  //
  CHECK(HWND(*this));
  SendMessage(PSM_ADDPAGE, 0, TParam2(HPROPSHEETPAGE(pg)));
}


//
// Simulates the choice of the Apply button, indicating that one or more
// pages have changed and the changes need to be validated or recorded.
// The property sheet sends the PSN_KILLACTIVE notification message to the
// current page. If the current page returns FALSE, the propertysheet
// sends the PSN_APPLY notification message to all pages.
// Returns true if all pages successfully applied the changes or false
// otherwise.
//
bool
TPropertySheet::Apply()
{
  CHECK(HWND(*this));
  return SendMessage(PSM_APPLY) != 0;
}

//
// Disables the 'Cancel' button and changes the text of the 'OK' button
// to 'Close'. You must invoke this method after applying a change that
// cannot be canceled.
//
void
TPropertySheet::CancelToClose()
{
  CHECK(HWND(*this));
  SendMessage(PSM_CANCELTOCLOSE);
}

//
// Informs the sheet that information in a sheet has changed. The
// sheet enables the 'Apply' button.
//
void
TPropertySheet::PageChanged(const TPropertyPage& pg)
{
  PRECONDITION(HPROPSHEETPAGE(pg));
  SendMessage(PSM_CHANGED, TParam1(pg.GetHandle()));
}

//
// Retrieves the handle to the window of the current page of the sheet.
//
HWND
TPropertySheet::GetCurrentPage() const
{
  return HWND(CONST_CAST(TPropertySheet*,
                         this)->SendMessage(PSM_GETCURRENTPAGEHWND));
}

//
// Retrieves the handle to a tab control of a property sheet.
//
HWND
TPropertySheet::GetTabControl() const
{
  CHECK(GetHandle());
  return HWND(CONST_CAST(TPropertySheet*,
                         this)->SendMessage(PSM_GETTABCONTROL));
}

//
// Passes a message to a property sheet dialog box and indicates
// whether the dialog processed the message.
// Returns true if the message was processed or false otherwise.
//
bool
TPropertySheet::IsDialogMessage(MSG& msg)
{
  CHECK(GetHandle());
  return SendMessage(PSM_ISDIALOGMESSAGE, 0, TParam2(&msg)) != 0;
}

//
// Simulates the choice of a property sheet button.
// The button parameter can be one of the following:
//
//    PSBTN_APPLYNOW      Apply Now button.
//    PSBTN_BACK          Back button.
//    PSBTN_CANCEL        Cancel button.
//    PSBTN_FINISH        Finish button.
//    PSBTN_HELP          Help button.
//    PSBTN_NEXT          Next button.
//    PSBTN_OK            OK button
//
void
TPropertySheet::PressButton(int button)
{
  CHECK(GetHandle());
  SendMessage(PSM_PRESSBUTTON, TParam1(button));
}


//
// Forwards the 'PSM_QUERYSIBLINGS' message to each page in the
// property sheet. If a page returns a nonzeroe value, the property
// sheet does not send the message to subsequent pages.
// Returns the nonzero value from a page in the property sheet, or
// zero if no page returns a nonzero value.
//
int
TPropertySheet::QuerySiblings(TParam1 p1, TParam2 p2)
{
  CHECK(GetHandle());
  return (int)SendMessage(PSM_QUERYSIBLINGS, p1, p2);
}

//
// Indicates that the system needs to be restarted for the changes
// to take effect. You should invoke this method only in response to
// the PSN_APPLY or PSN_KILLACTIVE notifications.
// NOTE: It's your responsibility to reboot the system [via ExitWindowEx
//       for example].
// NOTE: Invoking this method causes the TPropertySheet::Execute method
//       to return ID_PSREBOOTSYSTEM.
//
void
TPropertySheet::RebootSystem()
{
  CHECK(GetHandle());
  SendMessage(PSM_REBOOTSYSTEM);
}

//
// Removes the specified page from the property sheet
//
void
TPropertySheet::RemovePage(TPropertyPage& pg)
{
  PRECONDITION(HPROPSHEETPAGE(pg));
  CHECK(GetHandle());
  SendMessage(PSM_REMOVEPAGE, 0, TParam2(HPROPSHEETPAGE(pg)));
  //
  // Should we actually invoke 'DestroyPropertySheetPage' for
  // Pages which are added then removed from the PropertySheet??
}

//
// Removes the page at the specified index from the property sheet
//
void
TPropertySheet::RemovePage(int pgIndex)
{
  CHECK(GetHandle());
  SendMessage(PSM_REMOVEPAGE, pgIndex);
  //
  // Should we actually invoke 'DestroyPropertySheetPage' for
  // Pages which are added then removed from the PropertySheet??
}

//
// Indicates that the system needs to be restarted for the changes
// to take effect. You should invoke this method only in response to
// the PSN_APPLY or PSN_KILLACTIVE notifications.
// NOTE: It's your responsibility to reboot the system [via ExitWindowEx
//       for example].
// NOTE: Invoking this method causes the TPropertySheet::Execute method
//       to return ID_PSRESTARTWINDOWS.
//
void
TPropertySheet::RestartWindows()
{
  CHECK(GetHandle());
  SendMessage(PSM_RESTARTWINDOWS);
}

//
// Activates the specified page in the property sheet
// Returns true if successful or false otherwise.
// NOTE: The page that's loosing activation receives a PSN_KILLACTIVE
//       notification while the window that's gaining activation receives
//       a PSN_SETACTIVE notification.
//
bool
TPropertySheet::SelectPage(TPropertyPage& pg)
{
  PRECONDITION(GetHandle());
  PRECONDITION(HPROPSHEETPAGE(pg));
  return SendMessage(PSM_SETCURSEL, 0, TParam2(HPROPSHEETPAGE(pg))) != 0;
}

//
// Activates the page at the specified index in the property sheet.
// Returns true if successful or false otherwise.
// NOTE: The page that's loosing activation receives a PSN_KILLACTIVE
//       notification while the window that's gaining activation receives
//       a PSN_SETACTIVE notification.
//
bool
TPropertySheet::SelectPage(int pgIndex)
{
  PRECONDITION(GetHandle());
  return SendMessage(PSM_SETCURSEL, TParam1(pgIndex)) != 0;
}

//
// Activates the page with the specified resource identifier.
// Returns true if successful or false otherwise.
// NOTE: The page that's loosing activation receives a PSN_KILLACTIVE
//       notification while the window that's gaining activation receives
//       a PSN_SETACTIVE notification.
//
bool
TPropertySheet::SelectPage(TResId pgRes)
{
  CHECK(GetHandle());
  return SendMessage(PSM_SETCURSELID, 0, TParam2((_TCHAR *)pgRes));
}

//
// Sets the text for the 'Finish' button in a Wizard property sheet.
// NOTE: The button is enabled while the 'Next' and 'Back' buttons
//       are hidden.
//
void
TPropertySheet::SetFinishText(LPCTSTR txt)
{
  CHECK(GetHandle());
  SendMessage(PSM_SETFINISHTEXT, 0, TParam2(txt));
}

//
// Sets the title of a property sheet.
// If 'style' parameter is the PSH_PROPTITLE value, the prefix
// "Properties of" is included with the specified title ('txt')
// parameter.
//
void
TPropertySheet::SetTitle(LPCTSTR txt, uint32 style)
{
  CHECK(GetHandle());
  SendMessage(PSM_SETTITLE, TParam1(style), TParam2(txt));
}

//
// Enables the 'Back', 'Next' or 'Finish' button in a wizard
// property sheet. The 'flags' parameter can be a combination of
// the following values:
//
//          PSWIZB_BACK                 Back button
//          PSWIZB_NEXT                 Next button
//          PSWIZB_FINISH               Finish button
//
void
TPropertySheet::SetWizButtons(uint32 flags)
{
  CHECK(GetHandle());
  SendMessage(PSM_SETWIZBUTTONS, 0, TParam2(flags));
}

//
// Informs the sheet that the information in the specified page
// has reverted to the previously saved state. The sheet disables
// the 'Apply' button if no other pages have registered changes
// with the property sheet.
//
void
TPropertySheet::PageUnchanged(TPropertyPage& pg)
{
  PRECONDITION(HPROPSHEETPAGE(pg));
  CHECK(GetHandle());
  SendMessage(PSM_UNCHANGED, TParam1(pg.GetHandle()));
}

//
//
//
bool
TPropertySheet::PreProcessMsg(MSG& msg) {

  // If current page = 0, then it's time to close the property sheet.
  // 
  HWND page = GetCurrentPage();
  if (!page) {
    CloseWindow();
    return false;
  } 
  else {
    return TWindow::PreProcessMsg(msg);
  }
}


// ---------------------------------------------------------------------------
//  TPropertyPage
// ---------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TPropertyPage, TDialog)
  EV_WM_CLOSE,
  EV_PSN_SETACTIVE(SetActive),
  EV_PSN_KILLACTIVE(KillActive),
  EV_PSN_APPLY(Apply),
  EV_PSN_RESET(Reset),
  EV_PSN_HELP(Help),
  EV_PSN_WIZBACK(WizBack),
  EV_PSN_WIZFINISH(WizFinish),
  EV_PSN_WIZNEXT(WizNext),
  EV_PSN_QUERYCANCEL(QueryCancel),
END_RESPONSE_TABLE;

//
// Constructor for TPropertyPage
//
//
TPropertyPage::TPropertyPage(TPropertySheet* parent, TResId resId,
                             LPCTSTR title, TResId iconRes,
                             TModule* module)

:
  TDialog(parent, resId, module), 
  HPropPage(0)
{
  // Initialize the PROPSHEETPAGE structure
  // NOTE: We're storing the 'this' pointer in the application-defined
  //       section of the PROPSHEETPAGE structure...
  //
  memset(&PageInfo, 0, sizeof(PageInfo));
  PageInfo.dwSize = sizeof(PROPSHEETPAGE);
  PageInfo.dwFlags= PSP_DEFAULT;
  PageInfo.pszTemplate = resId;
  PageInfo.hInstance = *GetModule();

  if (title)
    SetTitle(title);
  if (iconRes)
    SetIcon(iconRes);

  PageInfo.dwFlags |= PSP_USECALLBACK;
  PageInfo.pfnCallback = PropCallback;
  PageInfo.pfnDlgProc = PropDlgProc;
  PageInfo.lParam = LPARAM(this);

  // When OWL is built with the NATIVECTRL_ALWAYS option or in 32-bit the
  // Common Control library MUST be available....
  //
  CHECK(TCommCtrl::IsAvailable());
}

//!BB TDialog should probably support LPCDLGTEMPLATE in 32-bit to
//    allow proper support of the following constructor...
//
//
//
TPropertyPage::TPropertyPage(TPropertySheet* parent, 
                             const PROPSHEETPAGE& pgInfo,
                             TModule* module)
:
  TDialog(parent, *pgInfo.pResource, NoAutoDelete, module),
  HPropPage(0)
{

  // Initialize the PROPSHEETPAGE structure
  // NOTE: We're storing the 'this' pointer in the application-defined
  //       section of the PROPSHEETPAGE structure...
  PageInfo = pgInfo;
  PageInfo.dwSize = sizeof(PROPSHEETPAGE);
  PageInfo.hInstance = *GetModule();
  PageInfo.dwFlags |= PSP_USECALLBACK;
  PageInfo.pfnCallback = PropCallback;
  PageInfo.pfnDlgProc = PropDlgProc;
  PageInfo.lParam = LPARAM(this);

  // When OWL is built with the NATIVECTRL_ALWAYS option, the
  // Common Control library MUST be available....
  //
  CHECK(TCommCtrl::IsAvailable());

}



//
// Destructor of TPropertyPage - Clean up allocated buffers used when
// ObjectWindows provides implementation of property pages.
//
TPropertyPage::~TPropertyPage()
{}

//
// Specifies flags to be used in creating the property page.  These
// are the flags that belong in PROPSHEETPAGE.dwFlags.  If used, this 
// method should be called immediately after the TPropertyPage is 
// constructed.
//
void
TPropertyPage::SetFlags(uint32 flags)
{
  PageInfo.dwFlags = flags;
}

//
// Specifies the icon to be used for this page
// NOTE: This routine must be invoked before the page is created.
//
void
TPropertyPage::SetIcon(const TIcon& icon)
{
  PageInfo.hIcon = icon;
  PageInfo.dwFlags &= ~PSH_USEICONID;
  PageInfo.dwFlags |=  PSH_USEHICON;
}

//
// Specifies the icon to be used for this page
// NOTE: This routine must be invoked before the page is created.
//
void
TPropertyPage::SetIcon(TResId iconResId)
{
  PageInfo.pszIcon = iconResId;
  PageInfo.dwFlags &= ~PSH_USEHICON;
  PageInfo.dwFlags |=  PSH_USEICONID;
}

//
// Sets the caption of this page.
// NOTE: This routine must be invoked before the page is created.
//
void
TPropertyPage::SetTitle(LPCTSTR title)
{
  // Let TWindow make a copy of the title to 'Title'
  // Then point to the 'duped' copy...
  //
  SetCaption(title);
  PageInfo.pszTitle = Title;
  PageInfo.dwFlags |= PSP_USETITLE;
}

//
// Sets the caption of this page.
// NOTE: This routine must be invoked before the page is created.
//
void
TPropertyPage::SetTitle(int txtResId)
{
  PageInfo.pszTitle = MAKEINTRESOURCE(txtResId);
  PageInfo.dwFlags &= ~PSP_USETITLE;
}

//
// WM_NOTIFY handler: Scans for property sheet notifications to 'patch'
// the 'idFrom' member to the predefined 'PropPageID'.
// NOTE: This is necessary since WM_NOTIFY subdispatching relies on the
//       id of the sender.
//
TResult
TPropertyPage::EvNotify(uint id, TNotify& notifyInfo)
{
  if (notifyInfo.code <= (uint)PSN_FIRST && notifyInfo.code >= (uint)PSN_LAST){
    // Property sheet notifications require special handling since the
    // concept of ctlId is non-existent. We patch it to the default
    // PageID expected by the ObjectWindows Property Page dispatchers
    //
    notifyInfo.idFrom = PropPageID;
    id = PropPageID;

    // Also make sure we don't reflect the message back to what looks like
    // the 'child' sender but is really the sheet. We achieve this by
    // NULLing out the HWND of the sender.
    //
    notifyInfo.hwndFrom = 0;
  }
  return TDialog::EvNotify(id, notifyInfo);
}

//
// Handle case of Escape from Edit control with ES_MULTILINE style
//
void TPropertyPage::EvClose()
{
  _TCHAR szClass[10];
  HWND hWnd = ::GetFocus();
  if(hWnd && IsChild(hWnd) && (::GetWindowLong(hWnd, GWL_STYLE)&ES_MULTILINE) &&
    ::GetClassName(hWnd, szClass, 10) && lstrcmpi(szClass, _T("EDIT")) == 0){
    GetParentO()->PostMessage(WM_CLOSE, 0, 0);
  }
  TDialog::EvClose();
}

//
// This callback is the default 'Dialog box procedure' of each page of our
// property sheet....
//
INT_PTR CALLBACK OWL_EXPORT16
TPropertyPage::PropDlgProc(HWND hDlg, uint msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg) {
    case  WM_INITDIALOG: {
        // Attach C++ object with it's underlying handle if necessary
        //
        LPPROPSHEETPAGE pageInfo = REINTERPRET_CAST(LPPROPSHEETPAGE,lParam);
        InitHandle(hDlg, pageInfo);
      }
      break;

    case WM_NOTIFY: {
        TNotify& notifyInfo= *(REINTERPRET_CAST(TNotify*, lParam));
        if (notifyInfo.code<=(uint)PSN_FIRST && notifyInfo.code>=(uint)PSN_LAST){
          // Property sheet notifications require special handling
          // since the concept of ctlId is non-existent. We patch it
          // to the default PageID expected by the ObjectWindows
          // Property Page dispatchers
          //
          notifyInfo.idFrom = PropPageID;
          wParam = PropPageID;
        }
      }
      break;

    default:
      break;
  }
  return TDialog::StdDlgProc(hDlg, msg, wParam, lParam);
}

//
// As with TDialog, most of the page's events are dispatch
// directly from 'StdWndProc'. ALthough the Sheet has each
// page's DialogProc, the notifications are not (don't seem
// to be) funelled directly to the dialogProc
//
bool
TPropertyPage::DialogFunction(uint msg, TParam1 p1, TParam2 p2)
{
  switch(msg) {
    case WM_NOTIFY: {
        NMHDR& nmhdr = *(REINTERPRET_CAST(NMHDR *, p2));
        if (nmhdr.code >= (uint)PSN_LAST && nmhdr.code <= (uint)PSN_FIRST) {
          CHECK(p1 != 0);
          CHECK(nmhdr.idFrom != 0);
        }
      }
      break;

    default:
      break;
  }
  return TDialog::DialogFunction(msg, p1, p2);
}

// 'CopyPageInfo' is called by the 'Sheet' object requesting
// the page to fill out a 'PROPSHEETPAGE' structure which
// describes the attribute of the page.
//
void
TPropertyPage::CopyPageInfo(PROPSHEETPAGE& pgInfo) const
{
  pgInfo = PageInfo;
}

//
// 'CreatePropertyPage' is called by the 'Sheet' object requesting
// the page to return a handle used to represent this dialog
// when it's inserted into the 'Sheet'
//
HPROPSHEETPAGE
TPropertyPage::CreatePropertyPage()
{
  if (!HPropPage) 
    HPropPage = TCommCtrl::Dll()->CreatePropertySheetPage(&PageInfo);

  return HPropPage;
}

//
// Destroys the page represented by this object.
//
bool
TPropertyPage::DestroyPropertyPage()
{
  PRECONDITION(HPropPage);
  if (TCommCtrl::Dll()->DestroyPropertySheetPage(HPropPage)) {
    HPropPage= 0;
    return true;
  }
  return false;
}

//
// Creates the page
//
bool
TPropertyPage::Create()
{
  // When using the system's implementation if PropertyDialogs, the page
  // is actually created behind the scene when the PropertySheet is
  // created. The callbacks specified by ObjectWindows [PropDlgProc &
  // PropCallback] will update the TPropertyPage's HWindow data member.
  //
  // Therefore, our 'Create' method simply checks that the handle was
  // indeed initialized and happily returns true.
  //
  CHECK(GetHandle() != 0);
  return true;
}

//
// The pages of a propertysheet are created internally by windows...
// Consequently, we must attempt to grab and thunk the 'HWND' as
// early as possible. There are two basic opportunities to do so:
//
// (a) A Sheet can provide a callback which is called whenever a
//     page is created or released. Hence 'TPropertyPage::PropCallback'
//     invokes 'InitHandle'.
//
// (b) Each page provides a dialog-procedure callback. Hence,
//     'TPropertyPage::PropDlgProc' invokes 'InitHandle' upon receiving
//     a WM_INITDIALOG message.
//
void
TPropertyPage::InitHandle(HWND pageHandle, LPPROPSHEETPAGE ppsp)
{
  // First check that the lParam data member of the PROPSHEETPAGE
  // contains a 'this' pointer to an OWL TPropertyPage object
  // wrapping the dialog
  //
  TPropertyPage* page = REINTERPRET_CAST(TPropertyPage*, ppsp->lParam);

  if (page && pageHandle) {
    // Only proceed if the C++ object is not fully initialized
    //
    if (page->GetHandle()) {
      CHECK(page->GetHandle() == pageHandle);
      return;
    }

    // Proceed to initialize the handle of the page object.
    //
    page->SetHandle(pageHandle);

    // We can now retrieve the pointer to the sheet object
    // and initialize the latter if necessary.
    //
    TPropertySheet* sheet = page->GetSheet();
    if (sheet) {
      if (!sheet->GetHandle()) {
        HWND sheetHandle = ::GetParent(pageHandle);
        if (sheetHandle)
          sheet->InitHandle(sheetHandle);
      }
    }

    // Allow OWL to thunk the page window.
    //
    page->SubclassWindowFunction();

    // NOTE: Typically, we'd call 'GetHWndState', 'PerformDlgInit',
    //       'SetupWindow' and 'SetFlag(wfFullyCreated)' after
    //       thunking a window. However, TDialog's 'EvInitDialog'
    //       method [invoked via the PropDlgProc callback] will
    //       handle that.
  }
}

//
// Static callback invoked whenever a Property Page is created is
// destroyed.
//
uint CALLBACK OWL_EXPORT16
TPropertyPage::PropCallback(HWND hwnd, uint uMsg, LPPROPSHEETPAGE ppsp)
{
  switch (uMsg) {
    case PSPCB_CREATE: {

           // A Property Page was just created.. We'll attempt to thunk
           // the underlying 'HWND' if it was specified..
           //
           if (hwnd)
             InitHandle(hwnd, ppsp);
         }
         break;

    case PSPCB_RELEASE: {

            // A Property Page was just released... Currently, we
            // don't have any processing to do here....
            // Should we invoke a virtual method of TPropertyPage
            // from here - just in case a page needs to do something
            // when it's released or are the default ObjectWindows
            // mechanisms (CleanupWindow & Destructor) sufficient....
            //
         }
         break;

    default:
          break;
  }
  // The return is ignored for PSPCB_RELEASE (according to the doc).
  // A non-zero value allows the page to be created...
  //
  return 1;
}


// Default implementation of PropertySheet notifications.. Derived classes
// will override - most likely.
//

//
//
//
int
TPropertyPage::Apply(TNotify&)
{
  // Check if it's OK to close and attempt to retrieve data
  //
  if (CanClose()) {
    TransferData(tdGetData);
    return PSNRET_NOERROR;
  }

  // It's not OK to proceed - return focus to this page
  //
  return PSNRET_INVALID_NOCHANGEPAGE;
}

//
//
//
bool
TPropertyPage::KillActive(TNotify&)
{
  return false;
}

//
//
//
void
TPropertyPage::Help(TNotify&)
{
}

//
//
//
void
TPropertyPage::Reset(TNotify&)
{
}

//
//
//
int
TPropertyPage::SetActive(TNotify&)
{
  return 0;
}

//
//
//
int
TPropertyPage::WizBack(TNotify&)
{
  return 0;
}

//
//
//
bool
TPropertyPage::WizFinish(TNotify&)
{
  return false;
}

//
//
//
int
TPropertyPage::WizNext(TNotify&)
{
  return 0;
}

//
//
//
bool
TPropertyPage::QueryCancel(TNotify&)
{
  return false;
}


__OWL_END_NAMESPACE
/* ========================================================================== */

