#include "pch.h"
#include "tabctrl.h"
#include "tabctrl.rh"

#include <owl/radiobut.h>
#include <owl/inputdia.h>

using namespace owl;

//
// Template to toggle bits on and off
//
template <class T1, class T2>
inline void BitsOnOff(T1& value, T2 bitOn, T2 bitOff)
{
  value |= bitOn;
  value &= ~bitOff;
}



DEFINE_RESPONSE_TABLE1(TTabControlWindow, TMDIChild)
EV_WM_SIZE,

EV_TCN_SELCHANGE(TabCtlID, TabSelChange),
EV_TCN_SELCHANGING(TabCtlID, TabSelChanging),
EV_TCN_KEYDOWN(TabCtlID, TabKeyDown),

EV_COMMAND(CM_NEWTAB, CmNewTab),
EV_COMMAND(CM_ADDITEM, CmAddItem),
EV_COMMAND(CM_REMOVEITEM, CmRemoveItem),
EV_COMMAND_ENABLE(CM_REMOVEITEM, CeRemoveItem),
EV_COMMAND(CM_DELETEALL, CmDeleteAll),
EV_COMMAND_ENABLE(CM_DELETEALL, CeDeleteAll),
EV_COMMAND(CM_CHANGEITEM, CmChangeItem),
EV_COMMAND_ENABLE(CM_CHANGEITEM, CeChangeItem),
EV_COMMAND(CM_INFO, CmInfo),
EV_COMMAND(CM_VETOSELCHANGING, CmVetoSelChanging),
EV_COMMAND_ENABLE(CM_VETOSELCHANGING, CeVetoSelChanging),
END_RESPONSE_TABLE;

TTabControlWindow::TTabControlWindow(TMDIClient& parent, LPCTSTR title, owl::TWindow* clientWnd, bool shrinkToClient, TModule* module)
  : TMDIChild(parent, title, clientWnd, shrinkToClient, module),
  VetoSelChanging(false),
  Tab(dynamic_cast<TTabControl*>(clientWnd)),
  Client(0)
{}

void
TTabControlWindow::AdjustTabClient()
{
  // Retrieve the window rectangle of the tab control.
  //
  TRect rect;
  Tab->GetWindowRect(rect);

  // NOTE: GetWindowRect returns screen coordinates. We'll need
  // to have the coordinates relative to the frame window,
  // the parent of both the tab and client window
  //
  ::MapWindowPoints(HWND_DESKTOP, *this, (LPPOINT)&rect, 2);

  // Ask the tab for it's 'client-area' based in it window location.
  //
  Tab->AdjustRect(false, rect);

  // Move the Client window.
  //
  if (Client && Client->GetHandle())
    Client->SetWindowPos(HWND_TOP, rect, SWP_SHOWWINDOW);
}

void
TTabControlWindow::EvSize(uint sizeType, const TSize& size)
{
  // Let frame resize the client
  //
  TMDIChild::EvSize(sizeType, size);

  // Adjust the edit control (which is kind of the client of the
  // tab control)...
  //
  AdjustTabClient();
}

void
TTabControlWindow::TabSelChange(TNotify& /*nm*/)
{
  // NOP
}

bool
TTabControlWindow::TabSelChanging(TNotify& /*nm*/)
{
  return VetoSelChanging ? 1 : 0;
}

void
TTabControlWindow::TabKeyDown(TTabKeyDown& /*tk*/)
{
  // NOP
}

//
// CmNewTab() creates a dialog which allows the user to select the type
// of TabControl to be created. It then proceeds to create the control
// and makes the new control the client of the application's frame window.
//
void
TTabControlWindow::CmNewTab()
{
  // Create/Initialize a structure to set and retrieve data.
  //
  struct
  {
    uint16 buttonStyle;
    uint16 defStyle;
    uint16 singleLine;
    uint16 multipleLine;
  }
  dlgData = { 0, 1, 1, 0 };

  // Create C++ objects representing the Dialog and its controls.
  //
  TDialog* dlg = new TDialog(this, IDD_NEWTAB);
  new TRadioButton(dlg, IDC_RADIOBUTTON1);
  new TRadioButton(dlg, IDC_RADIOBUTTON2);
  new TRadioButton(dlg, IDC_RADIOBUTTON3);
  new TRadioButton(dlg, IDC_RADIOBUTTON4);

  // Set the transfer buffer and execute the dialog.
  //
  dlg->SetTransferBuffer(&dlgData);
  if (dlg->Execute() == IDOK)
  {
    // Create a new Tab Control.
    //
    TTabControl* tab = new TTabControl(0, TabCtlID, 0, 0, 0, 0);

    // Convert selection into style bits.
    //
    if (dlgData.defStyle)
      BitsOnOff(tab->Attr.Style, TCS_TABS, TCS_BUTTONS);
    if (dlgData.buttonStyle)
      BitsOnOff(tab->Attr.Style, TCS_BUTTONS, TCS_TABS);
    if (dlgData.singleLine)
      BitsOnOff(tab->Attr.Style, TCS_SINGLELINE, TCS_MULTILINE);
    if (dlgData.multipleLine)
      BitsOnOff(tab->Attr.Style, TCS_MULTILINE, TCS_SINGLELINE);

    // Cleanup prior client window.
    //
    TWindow* oldClient = SetClientWindow(tab);
    Tab = tab;
    if (oldClient)
    {
      if (oldClient->GetHandle())
        oldClient->Destroy();
      delete oldClient;
    }
  }
}

//
// This function prompts the user for a label and creates a new tab item.
//
void
TTabControlWindow::CmAddItem()
{
  TInputDialog dlg(this, _T("New Item"), _T("Enter Item Label"));
  if (dlg.Execute() == IDOK)
  {
    if (!dlg.GetBuffer()[0]) return;

    // Add a new item to the tabcontrol.
    //
    TTabItem tabItem(dlg.GetBuffer());
    Tab->Add(tabItem);

    // Adjust Client in case the Tab items got rearranged.
    //
    AdjustTabClient();
  }
}

void
TTabControlWindow::CeRemoveItem(TCommandEnabler& ce)
{
  ce.Enable((Tab && Tab->GetSel() != -1) ? true : false);
}


//
// This methods offers the user a list of the tab items and removes
// the selected one (if one is selected via the picklist dialog).
//
void
TTabControlWindow::CmRemoveItem()
{
  TInputDialog dlg(this, _T("Remove Item"), _T("Enter Item Index"));
  if (dlg.Execute() == IDOK && dlg.GetCaption()[0])
  {
    int index = _ttoi(dlg.GetCaption());

    // Remove item from tabcontrol.
    //
    if (index < Tab->GetCount() && index >= 0)
    {
      Tab->Delete(index);

      // Adjust Client in case the Tab items got rearranged.
      //
      AdjustTabClient();
    }
    else
      MessageBox(_T("Invalid index specified"), _T("INFO"));
  }
}

void
TTabControlWindow::CmChangeItem()
{
  int index = Tab->GetSel();
  if (index == -1) return;

  tchar label[80] = {};
  TTabItem item(*Tab, index, TCIF_TEXT, COUNTOF(label), label);

  tostringstream s;
  s << "New label of (" << label << ")";
  TInputDialog dlg(this, _T("Change Item"), s.str(), label, COUNTOF(label));
  if (dlg.Execute() == IDOK && label[0])
  {
    Tab->SetItem(index, item);

    // Adjust Client in case the Tab items got rearranged.
    //
    AdjustTabClient();
  }
}

void
TTabControlWindow::CeChangeItem(TCommandEnabler& ce)
{
  ce.Enable((Tab && Tab->GetSel() != -1) ? true : false);
}

void
TTabControlWindow::CmDeleteAll()
{
  Tab->DeleteAll();

  // Adjust Client in case the Tab items got rearranged.
  //
  AdjustTabClient();
}

void
TTabControlWindow::CeDeleteAll(TCommandEnabler& ce)
{
  ce.Enable((Tab && Tab->GetCount()) ? true : false);
}

void
TTabControlWindow::CmVetoSelChanging()
{
  VetoSelChanging = !VetoSelChanging;
}

void
TTabControlWindow::CeVetoSelChanging(TCommandEnabler& ce)
{
  ce.SetCheck(VetoSelChanging ? TCommandEnabler::Checked :
    TCommandEnabler::Unchecked);
}

void TTabControlWindow::CmInfo()
{
  tostringstream s;
  s << "Count: " << Tab->GetCount()
    << ", RowCount: " << Tab->GetRowCount()
    << ", Sel: " << Tab->GetSel()
    << ", ImageList: 0x" << std::hex << static_cast<void*>(Tab->GetImageList());
  MessageBox(s.str(), _T("INFO"));
}



DEFINE_RESPONSE_TABLE1(TNoteTabDlg, TDialog)
EV_TCN_SELCHANGE(IDC_NOTETAB, TabSelChange),
END_RESPONSE_TABLE;

tchar *Tabs[] = { _T("Title"), _T("Ship To"), _T("Order Notes") };
int  TitleIDs[] = { IDC_RADIOBUTTON1, IDC_RADIOBUTTON2, IDC_RADIOBUTTON3 };
int  ShipIDs[] = { IDC_STATIC1, IDC_EDITNAME, IDC_STATIC2, IDC_EDITADDR };
int  OrderIDs[] = { IDC_STATIC3, IDC_EDITNOTES };

//
//
//
TNoteTabDlg::TNoteTabDlg(TWindow* parent, TResId id)
  :TDialog(parent, id)
{
  NoteTab = new TNoteTab(this, IDC_NOTETAB);
  NoteTab->SetWindowFace(true);
}

//
//
//
void
TNoteTabDlg::SetupWindow()
{
  TDialog::SetupWindow();
  for (int i = 0; i<sizeof(Tabs) / sizeof(Tabs[0]); i++)
    NoteTab->Add(Tabs[i]);
  NoteTab->SetSel(0);
}

//
//
//
void
TNoteTabDlg::TabSelChange(TNotify far&)
{
  int index = NoteTab->GetSel(), i;
  for (i = 0; i<COUNTOF(TitleIDs); i++) {
    ::ShowWindow(GetDlgItem(TitleIDs[i]), index == 0 ? SW_SHOW : SW_HIDE);
    ::EnableWindow(GetDlgItem(TitleIDs[i]), index == 0 ? TRUE : FALSE);
  }
  for (i = 0; i<COUNTOF(ShipIDs); i++) {
    ::ShowWindow(GetDlgItem(ShipIDs[i]), index == 1 ? SW_SHOW : SW_HIDE);
    ::EnableWindow(GetDlgItem(ShipIDs[i]), index == 1 ? TRUE : FALSE);
  }
  for (i = 0; i<COUNTOF(OrderIDs); i++) {
    ::ShowWindow(GetDlgItem(OrderIDs[i]), index == 2 ? SW_SHOW : SW_HIDE);
    ::EnableWindow(GetDlgItem(OrderIDs[i]), index == 2 ? TRUE : FALSE);
  }
}
