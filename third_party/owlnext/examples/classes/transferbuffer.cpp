//
// OWLNext Transfer Buffer Demo
// This code demonstrates the safe transfer buffer mechanism in OWLNext.
// See "Safe Transfer Buffers" on the OWLNext Wiki for more information.
// http://sourceforge.net/apps/mediawiki/owlnext/index.php?title=Safe_Transfer_Buffers
//
// Copyright (c) 2010 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#if defined(__BORLANDC__)
# pragma option -w-ccc // Disable "Condition is always true/false".
# pragma option -w-inl // Disable warning "Functions containing 'statement' is not expanded inline".
#endif

#include "pch.h"
#include <owl/pch.h>

#include <owl/transferbuffer.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/propsht.h>

#include "client.h"
#include "transferbuffer.rh"

using namespace owl;
using namespace std;

//
// Safe transfer buffer
// Any alignment and packing is supported.
//
struct TSafeTransferBuffer
{
  _TCHAR Static[7];
  tstring StringStatic;
  _TCHAR Edit[7];
  tstring StringEdit;
  TListBoxData ListBox;
  TComboBoxData ComboBox;
  TComboBoxExData ComboBoxEx;
  WORD CheckBox;
  WORD RadioButton;
  WORD RadioButton2;
  TScrollBarData ScrollBar;
  TScrollBarData Slider;
  TCheckListData CheckList;
  TDateTimePickerData DateTimePicker;
  uint16 HotKey;
  TIPAddressBits IPAddress;
  _TCHAR MemComboBox[13];
  tstring StringMemComboBox;
  TMonthCalendarData MonthCalendar;
};


//
// OWLNext Transfer Buffer Demo 
//
class TTransferBufferDemo
  : public TEdit
{
public:

  TTransferBufferDemo(TWindow* parent, int id, LPCTSTR text, int x, int y, int w, int h,
    uint textLimit = 0, bool multiline = false, TModule* module = 0)
    : TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
  {

  }

protected:

  //
  // Command handlers
  //
  void CmSafeTransferBufferDialog();
  void CmSafeTransferBufferPropertySheet();

  void DisplayData(const TSafeTransferBuffer& data);

  DECLARE_RESPONSE_TABLE(TTransferBufferDemo);
};

//
// Response table for the application
//
DEFINE_RESPONSE_TABLE1(TTransferBufferDemo, TEdit)
EV_COMMAND(CM_SAFETRANSFERBUFFERDIALOG, CmSafeTransferBufferDialog),
EV_COMMAND(CM_SAFETRANSFERBUFFERPROPERTYSHEET, CmSafeTransferBufferPropertySheet),
END_RESPONSE_TABLE;

//
// Search dialog using safe transfer buffer
//
class TSafeTransferBufferDialog
  : public TDialog,
  virtual public TTransferBufferWindow<TSafeTransferBuffer>
{
public:

  //
  // Control representations
  //
  TStatic& Static;
  TStatic& StringStatic;
  TEdit& Edit;
  TEdit& StringEdit;
  TListBox& ListBox;
  TComboBox& ComboBox;
  TComboBoxEx& ComboBoxEx;
  TCheckBox& CheckBox;
  TRadioButton& RadioButton;
  TRadioButton& RadioButton2;
  TScrollBar& ScrollBar;
  TSlider& Slider;
  TCheckList& CheckList;
  TDateTimePicker& DateTimePicker;
  THotKey& HotKey;
  TIPAddress& IPAddress;
  TMemComboBox& MemComboBox;
  TMemComboBox& StringMemComboBox;
  TMonthCalendar& MonthCalendar;

  //
  // Constructor
  // Note that TTransferBuffer is a typedef provided by the base class;
  // it is defined as the same type as the template argument to TTransferBufferWindow.
  //
  TSafeTransferBufferDialog(TWindow* parent, TTransferBuffer& b)
    : TDialog(parent, IDD_SAFETRANSFERBUFFERDIALOG),
    Static(Bind<TStatic>(&TTransferBuffer::Static, IDC_STATIC1)),
    StringStatic(Bind<TStatic>(&TTransferBuffer::StringStatic, IDC_STRINGSTATIC)),
    Edit(Bind<TEdit>(&TTransferBuffer::Edit, IDC_EDIT)),
    StringEdit(Bind<TEdit>(&TTransferBuffer::StringEdit, IDC_STRINGEDIT)),
    ListBox(Bind<TListBox>(&TTransferBuffer::ListBox, IDC_LISTBOX)),
    ComboBox(Bind<TComboBox>(&TTransferBuffer::ComboBox, IDC_COMBOBOX)),
    ComboBoxEx(Bind<TComboBoxEx>(&TTransferBuffer::ComboBoxEx, IDC_COMBOBOXEX)),
    CheckBox(Bind<TCheckBox>(&TTransferBuffer::CheckBox, IDC_CHECKBOX)),
    RadioButton(Bind<TRadioButton>(&TTransferBuffer::RadioButton, IDC_RADIOBUTTON)),
    RadioButton2(Bind<TRadioButton>(&TTransferBuffer::RadioButton2, IDC_RADIOBUTTON2)),
    ScrollBar(Bind<TScrollBar>(&TTransferBuffer::ScrollBar, IDC_SCROLLBAR)),
    Slider(Bind<TSlider>(&TTransferBuffer::Slider, IDC_SLIDER, 0)),
    CheckList(Bind<TCheckList>(&TTransferBuffer::CheckList, IDC_CHECKLIST)),
    DateTimePicker(Bind<TDateTimePicker>(&TTransferBuffer::DateTimePicker, IDC_DATETIMEPICKER)),
    HotKey(Bind<THotKey>(&TTransferBuffer::HotKey, IDC_HOTKEY)),
    IPAddress(Bind<TIPAddress>(&TTransferBuffer::IPAddress, IDC_IPADDRESS)),
    MemComboBox(Bind<TMemComboBox>(&TTransferBuffer::MemComboBox, IDC_MEMCOMBOBOX, _T("MemComboBox"))),
    StringMemComboBox(Bind<TMemComboBox>(&TTransferBuffer::StringMemComboBox, IDC_STRINGMEMCOMBOBOX, _T("StringMemComboBox"))),
    MonthCalendar(Bind<TMonthCalendar>(&TTransferBuffer::MonthCalendar, IDC_MONTHCALENDAR))
  {
    SetTransferBuffer(&b); // type-safe overload
  }

protected:

  //
  // Puts some data in all controls.
  //
  void CmSetControls()
  {
    Static.SetText(_T("Static"));
    StringStatic.SetText(_T("StringStatic"));
    Edit.SetText(_T("Edit"));
    StringEdit.SetText(_T("StringEdit"));
    ListBox.ClearList();
    ListBox.AddString(_T("ListBox1"));
    ListBox.AddString(_T("ListBox2"));
    ListBox.AddString(_T("ListBox3"));
    ComboBox.ClearList();
    ComboBox.AddString(_T("ComboBox1"));
    ComboBox.AddString(_T("ComboBox2"));
    ComboBox.AddString(_T("ComboBox3"));
    ComboBoxEx.ClearList();
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx1")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx2")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx3")));
    CheckBox.Check();
    RadioButton.Check();
    RadioButton2.Uncheck();
    ScrollBar.SetRange(0, 100);
    ScrollBar.SetPosition(50);
    Slider.SetRange(0, 100);
    Slider.SetPosition(50);
    CheckList.ClearList();
    CheckList.AddItem(new TCheckListItem(_T("CheckList1")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList2")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList3")));
    //CheckList.AddString(_T("CheckList4")); // NB! Will crash transfer later!
    DateTimePicker.SetTime(TSystemTime(2000, 1, 1));
    HotKey.SetHotKey(VK_F1, VK_SHIFT);
    IPAddress.SetAddress(TIPAddressBits(1, 2, 3, 4));
    MemComboBox.ClearList();
    MemComboBox.AddString(_T("MemComboBox1"));
    MemComboBox.AddString(_T("MemComboBox2"));
    MemComboBox.AddString(_T("MemComboBox3"));
    StringMemComboBox.ClearList();
    StringMemComboBox.AddString(_T("StringMemComboBox1"));
    StringMemComboBox.AddString(_T("StringMemComboBox2"));
    StringMemComboBox.AddString(_T("StringMemComboBox3"));
    MonthCalendar.SetCurSel(TSystemTime(2000, 1, 1));
  }

  //
  // Clears the data from all the controls.
  //
  void CmClearControls()
  {
    Static.Clear();
    StringStatic.Clear();
    Edit.Clear();
    StringEdit.Clear();
    ListBox.ClearList();
    ComboBox.ClearList();
    ComboBox.Clear();
    ComboBoxEx.ClearList();
    ComboBoxEx.Clear();
    CheckBox.Uncheck();
    RadioButton.Uncheck();
    RadioButton2.Uncheck();
    ScrollBar.SetRange(0, 0);
    Slider.SetRange(0, 0);
    CheckList.ClearList();
    DateTimePicker.SetTime(TSystemTime::LocalTime());
    HotKey.SetHotKey(0);
    IPAddress.Clear();
    MemComboBox.ClearList();
    StringMemComboBox.ClearList();
    MonthCalendar.SetCurSel(TSystemTime::LocalTime());
  }

  //
  // Triggers a safety check exception.
  // Changing the text limit violates the transfer buffer contract for controls that use character 
  // arrays for transfer. The window will throw an exception on the next transfer.
  // Applications should use a standard string class as a field type instead of a character array.
  //
  void CmChangeTextLimit()
  {
    switch (rand() % 3)
    {
    case 0: Static.SetTextLimit(666); break;
    case 1: Edit.SetTextLimit(666); break;
    case 2: MemComboBox.SetTextLimit(666); break;
    }
  }

  DECLARE_RESPONSE_TABLE(TSafeTransferBufferDialog);
};

//
// Response table for the dialog.
//
DEFINE_RESPONSE_TABLE1(TSafeTransferBufferDialog, TDialog)
EV_COMMAND(IDC_SETCONTROLS, CmSetControls),
EV_COMMAND(IDC_CLEARCONTROLS, CmClearControls),
EV_COMMAND(IDC_CHANGETEXTLIMIT, CmChangeTextLimit),
END_RESPONSE_TABLE;

//
// Search property page using safe transfer buffer
//
class TSafeTransferBufferPage
  : public TPropertyPage,
  virtual public TTransferBufferWindow<TSafeTransferBuffer>
{
public:

  //
  // Control representations
  //
  TStatic& Static;
  TStatic& StringStatic;
  TEdit& Edit;
  TEdit& StringEdit;
  TListBox& ListBox;
  TComboBox& ComboBox;
  TComboBoxEx& ComboBoxEx;
  TCheckBox& CheckBox;
  TRadioButton& RadioButton;
  TRadioButton& RadioButton2;
  TScrollBar& ScrollBar;
  TSlider& Slider;
  TCheckList& CheckList;
  TDateTimePicker& DateTimePicker;
  THotKey& HotKey;
  TIPAddress& IPAddress;
  TMemComboBox& MemComboBox;
  TMemComboBox& StringMemComboBox;
  TMonthCalendar& MonthCalendar;

  //
  // Constructor
  // Note that TTransferBuffer is a typedef provided by the base class;
  // it is defined as the same type as the template argument to TTransferBufferWindow.
  //
  TSafeTransferBufferPage(TPropertySheet* parent, TTransferBuffer& b)
    : TPropertyPage(parent, IDD_SAFETRANSFERBUFFERPAGE),
    Static(Bind<TStatic>(&TTransferBuffer::Static, IDC_STATIC1)),
    StringStatic(Bind<TStatic>(&TTransferBuffer::StringStatic, IDC_STRINGSTATIC)),
    Edit(Bind<TEdit>(&TTransferBuffer::Edit, IDC_EDIT)),
    StringEdit(Bind<TEdit>(&TTransferBuffer::StringEdit, IDC_STRINGEDIT)),
    ListBox(Bind<TListBox>(&TTransferBuffer::ListBox, IDC_LISTBOX)),
    ComboBox(Bind<TComboBox>(&TTransferBuffer::ComboBox, IDC_COMBOBOX)),
    ComboBoxEx(Bind<TComboBoxEx>(&TTransferBuffer::ComboBoxEx, IDC_COMBOBOXEX)),
    CheckBox(Bind<TCheckBox>(&TTransferBuffer::CheckBox, IDC_CHECKBOX)),
    RadioButton(Bind<TRadioButton>(&TTransferBuffer::RadioButton, IDC_RADIOBUTTON)),
    RadioButton2(Bind<TRadioButton>(&TTransferBuffer::RadioButton2, IDC_RADIOBUTTON2)),
    ScrollBar(Bind<TScrollBar>(&TTransferBuffer::ScrollBar, IDC_SCROLLBAR)),
    Slider(Bind<TSlider>(&TTransferBuffer::Slider, IDC_SLIDER, 0)),
    CheckList(Bind<TCheckList>(&TTransferBuffer::CheckList, IDC_CHECKLIST)),
    DateTimePicker(Bind<TDateTimePicker>(&TTransferBuffer::DateTimePicker, IDC_DATETIMEPICKER)),
    HotKey(Bind<THotKey>(&TTransferBuffer::HotKey, IDC_HOTKEY)),
    IPAddress(Bind<TIPAddress>(&TTransferBuffer::IPAddress, IDC_IPADDRESS)),
    MemComboBox(Bind<TMemComboBox>(&TTransferBuffer::MemComboBox, IDC_MEMCOMBOBOX, _T("MemComboBox"))),
    StringMemComboBox(Bind<TMemComboBox>(&TTransferBuffer::StringMemComboBox, IDC_STRINGMEMCOMBOBOX, _T("StringMemComboBox"))),
    MonthCalendar(Bind<TMonthCalendar>(&TTransferBuffer::MonthCalendar, IDC_MONTHCALENDAR))
  {
    SetTransferBuffer(&b); // type-safe overload
  }

protected:

  //
  // Puts some data in all controls.
  //
  void CmSetControls()
  {
    Static.SetText(_T("Static"));
    StringStatic.SetText(_T("StringStatic"));
    Edit.SetText(_T("Edit"));
    StringEdit.SetText(_T("StringEdit"));
    ListBox.ClearList();
    ListBox.AddString(_T("ListBox1"));
    ListBox.AddString(_T("ListBox2"));
    ListBox.AddString(_T("ListBox3"));
    ComboBox.ClearList();
    ComboBox.AddString(_T("ComboBox1"));
    ComboBox.AddString(_T("ComboBox2"));
    ComboBox.AddString(_T("ComboBox3"));
    ComboBoxEx.ClearList();
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx1")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx2")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx3")));
    CheckBox.Check();
    RadioButton.Check();
    RadioButton2.Uncheck();
    ScrollBar.SetRange(0, 100);
    ScrollBar.SetPosition(50);
    Slider.SetRange(0, 100);
    Slider.SetPosition(50);
    CheckList.ClearList();
    CheckList.AddItem(new TCheckListItem(_T("CheckList1")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList2")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList3")));
    //CheckList.AddString(_T("CheckList4")); // NB! Will crash transfer later!
    DateTimePicker.SetTime(TSystemTime(2000, 1, 1));
    HotKey.SetHotKey(VK_F1, VK_SHIFT);
    IPAddress.SetAddress(TIPAddressBits(1, 2, 3, 4));
    MemComboBox.ClearList();
    MemComboBox.AddString(_T("MemComboBox1"));
    MemComboBox.AddString(_T("MemComboBox2"));
    MemComboBox.AddString(_T("MemComboBox3"));
    StringMemComboBox.ClearList();
    StringMemComboBox.AddString(_T("StringMemComboBox1"));
    StringMemComboBox.AddString(_T("StringMemComboBox2"));
    StringMemComboBox.AddString(_T("StringMemComboBox3"));
    MonthCalendar.SetCurSel(TSystemTime(2000, 1, 1));

    GetSheet()->PageChanged(*this);
  }

  //
  // Clears the data from all the controls.
  //
  void CmClearControls()
  {
    Static.Clear();
    StringStatic.Clear();
    Edit.Clear();
    StringEdit.Clear();
    ListBox.ClearList();
    ComboBox.ClearList();
    ComboBox.Clear();
    ComboBoxEx.ClearList();
    ComboBoxEx.Clear();
    CheckBox.Uncheck();
    RadioButton.Uncheck();
    RadioButton2.Uncheck();
    ScrollBar.SetRange(0, 0);
    Slider.SetRange(0, 0);
    CheckList.ClearList();
    DateTimePicker.SetTime(TSystemTime::LocalTime());
    HotKey.SetHotKey(0);
    IPAddress.Clear();
    MemComboBox.ClearList();
    StringMemComboBox.ClearList();
    MonthCalendar.SetCurSel(TSystemTime::LocalTime());

    GetSheet()->PageChanged(*this);
  }

  //
  // Triggers a safety check exception.
  // Changing the text limit violates the transfer buffer contract for controls that use character 
  // arrays for transfer. The window will throw an exception on the next transfer.
  // Applications should use a standard string class as a field type instead of a character array.
  //
  void CmChangeTextLimit()
  {
    switch (rand() % 3)
    {
    case 0: Static.SetTextLimit(666); break;
    case 1: Edit.SetTextLimit(666); break;
    case 2: MemComboBox.SetTextLimit(666); break;
    }

    GetSheet()->PageChanged(*this);
  }

  DECLARE_RESPONSE_TABLE(TSafeTransferBufferPage);
};

//
// Response table for the dialog.
//
DEFINE_RESPONSE_TABLE1(TSafeTransferBufferPage, TPropertyPage)
EV_COMMAND(IDC_SETCONTROLS, CmSetControls),
EV_COMMAND(IDC_CLEARCONTROLS, CmClearControls),
EV_COMMAND(IDC_CHANGETEXTLIMIT, CmChangeTextLimit),
END_RESPONSE_TABLE;

//
// Tests safe transfer buffers.
// A static buffer is used to be able to observe the data over several executions of the dialog.
//
void TTransferBufferDemo::CmSafeTransferBufferDialog()
{
  static TSafeTransferBufferDialog::TTransferBuffer b;
  TSafeTransferBufferDialog dlg(this, b);
  dlg.SetCaption(_T("Safe Transfer Buffer Dialog Demo"));

  if (dlg.Execute() == IDOK)
  {
    TRACE(_T("IDOK!"));
    DisplayData(b);
  }
}

//
// Tests safe transfer buffers.
// A static buffer is used to be able to observe the data over several executions of the dialog.
//
void TTransferBufferDemo::CmSafeTransferBufferPropertySheet()
{
  static TSafeTransferBufferPage::TTransferBuffer b;
  TPropertySheet sheet(this, _T("Safe Transfer Buffer Dialog Demo"), 0, false);
  TSafeTransferBufferPage page(&sheet, b);

  if (sheet.Execute() == IDOK)
  {
    TRACE(_T("IDOK!"));
    DisplayData(b);
  }
}

void TTransferBufferDemo::DisplayData(const TSafeTransferBuffer& data)
{
  tostringstream text;
  text << _T("Static: ") << data.Static << _T("\r\n");
  text << _T("StringStatic: ") << data.StringStatic << _T("\r\n");
  text << _T("Edit: ") << data.Edit << _T("\r\n");
  text << _T("StringEdit: ") << data.StringEdit << _T("\r\n");
  text << _T("ListBox: ") << data.ListBox.GetSelString() << _T("\r\n");
  text << _T("CheckList: ") << data.CheckList.GetSelCount() << _T("\r\n");
  text << _T("ComboBox: ") << data.ComboBox.GetSelString() << _T("\r\n");
  text << _T("ComboBoxEx: ") << data.ComboBoxEx.GetSelString() << _T("\r\n");
  text << _T("MemComboBox: ") << data.MemComboBox << _T("\r\n");
  text << _T("StringMemComboBox: ") << data.StringMemComboBox << _T("\r\n");
  text << _T("CheckBox: ") << data.CheckBox << _T("\r\n");
  text << _T("RadioButton: ") << data.RadioButton << _T("\r\n");
  text << _T("ScrollBar: ") << data.ScrollBar.Position << _T("\r\n");
  text << _T("Slider: ") << data.Slider.Position << _T("\r\n");
  text << _T("HotKey: ") << data.HotKey << _T("\r\n");
  text << _T("IPAddress: ") << data.IPAddress.Address << _T("\r\n");
  text << _T("DateTimePicker: ") << data.DateTimePicker.DateTime.GetYear() << _T("-")
    << data.DateTimePicker.DateTime.GetMonth() << _T("-") << data.DateTimePicker.DateTime.GetDay() << _T("\r\n");
  text << _T("MonthCalendar: ") << data.MonthCalendar.Date1.GetYear() << _T("-")
    << data.MonthCalendar.Date1.GetMonth() << _T("-") << data.MonthCalendar.Date1.GetDay() << _T("\r\n");

  SetText(text.str());


}



void TClassesMDIClient::CmTransferBuffer()
{
  _TCHAR text[] = _T("Transfer Buffer Demo\r\n")
    _T("This code demonstrates the safe transfer buffer mechanism in OWLNext..");

  TMDIChild* child = new TMDIChild(*this, _T("Dialog Data Transfer"), new TTransferBufferDemo(0, 0, text, 0, 0, 0, 0, 0, true));
  child->SetMenuDescr(TMenuDescr(IDM_TRANSFERBUFFER, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);

}
