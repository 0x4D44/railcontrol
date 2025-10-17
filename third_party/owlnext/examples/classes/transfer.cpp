//
// OWLNext Dialog Data Transfer Demo
// This code demonstrates the dialog data transfer framework (DDT) in OWLNext.
// See "Dialog Data Transfer" on the OWLNext Wiki for more information.
// http://sourceforge.net/apps/mediawiki/owlnext/index.php?title=Dialog_Data_Transfer
//
// Copyright (c) 2010 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#if defined(__BORLANDC__)
# pragma option -w-ccc // Disable "Condition is always true/false".
# pragma option -w-inl // Disable warning "Functions containing 'statement' is not expanded inline".
#endif

#include "pch.h"

#include <owl/pch.h>

#include <owl/transfer.h>

#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>

#include <owl/combobox.h>
#include <owl/combobex.h>
#include <owl/checkbox.h>
#include <owl/checklst.h>
#include <owl/datetime.h>
#include <owl/edit.h>
#include <owl/hotkey.h>
#include <owl/ipaddres.h>
#include <owl/listbox.h>
#include <owl/memcbox.h>
#include <owl/monthcal.h>
#include <owl/radiobut.h>
#include <owl/scrollba.h>
#include <owl/slider.h>
#include <owl/static.h>

#include "client.h"
#include "transfer.rh"

using namespace owl;
using namespace std;

//
// Data type option constants (don't change)
//
#define FULL_DATA 0
#define SELECTED_INDEX 1
#define SELECTED_STRING 2
#define SELECTED_TIME 3
#define SELECTED_TIME_STRING 4
#define INTEGER_STATE 0
#define BOOLEAN_STATE 1
#define SCROLLBAR_POSITION 1
#define ADDRESS_OBJECT 0
#define INTEGER_ADDRESS 1

//
// Define the data types used to transfer data to/from the controls in this demo.
// Feel free to experiment with the options.
//
#define LISTBOX_DATA_OPTION  SELECTED_INDEX  // FULL_DATA | SELECTED_INDEX | SELECTED_STRING
#define EXACT_LISTBOX_STRINGS  1  // 0 | 1
#define CHECKBOX_DATA_OPTION  BOOLEAN_STATE  // INTEGER_STATE |  BOOLEAN_STATE
#define SCROLLBAR_DATA_OPTION  SCROLLBAR_POSITION  // FULL_DATA | SCROLLBAR_POSITION
#define DATETIMEPICKER_DATA_OPTION  SELECTED_TIME  // FULL_DATA | SELECTED_TIME | SELECTED_TIME_STRING
#define IPADDRESS_DATA_OPTION  INTEGER_ADDRESS  // ADDRESS_OBJECT | INTEGER_ADDRESS
#define MONTHCALENDAR_DATA_OPTION  SELECTED_TIME  // FULL_DATA | SELECTED_TIME

//
// OWLNext Dialog Data Transfer Demo application
//
class TTransferDemo
  : public TEdit
{
public:

  TTransferDemo(TWindow* parent, int id, LPCTSTR text, int x, int y, int w, int h,
    uint textLimit = 0, bool multiline = false, TModule* module = 0)
    : TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
  {

  }

  typedef tstring TStaticDataType; // You can use int, float etc. if your compiler supports template overloads.
  typedef tstring TEditDataType;

#if LISTBOX_DATA_OPTION == FULL_DATA

  typedef TListBoxData TListBoxDataType;
  typedef TCheckListData TCheckListDataType;
  typedef TComboBoxData TComboBoxDataType;
  typedef TComboBoxExData TComboBoxExDataType;
  typedef TComboBoxData TMemComboBoxDataType;

#elif LISTBOX_DATA_OPTION == SELECTED_INDEX

  typedef int TListBoxDataType;
  typedef int TCheckListDataType;
  typedef int TComboBoxDataType;
  typedef int TComboBoxExDataType;
  typedef int TMemComboBoxDataType;

#elif LISTBOX_DATA_OPTION == SELECTED_STRING

  typedef tstring TListBoxDataType;
  typedef tstring TCheckListDataType;
  typedef tstring TComboBoxDataType;
  typedef tstring TComboBoxExDataType;
  typedef tstring TMemComboBoxDataType;

#endif

#if CHECKBOX_DATA_OPTION == BOOLEAN_STATE

  typedef bool TCheckBoxDataType;

#elif CHECKBOX_DATA_OPTION == INTEGER_STATE

  typedef UINT TCheckBoxDataType;

#endif

  typedef int TRadioButtonDataType; // Index from radio buttons (assumes the radiobutton is the first in a group).

#if SCROLLBAR_DATA_OPTION == FULL_DATA

  typedef TScrollBarData TScrollBarDataType;
  typedef TScrollBarData TSliderDataType;

#elif SCROLLBAR_DATA_OPTION == SCROLLBAR_POSITION

  typedef int TScrollBarDataType;
  typedef int TSliderDataType;

#endif

  typedef uint16 THotKeyDataType;

#if IPADDRESS_DATA_OPTION == ADDRESS_OBJECT

  typedef TIPAddressBits TIPAddressDataType;

#elif IPADDRESS_DATA_OPTION == INTEGER_ADDRESS

  typedef uint32 TIPAddressDataType;

#endif

#if DATETIMEPICKER_DATA_OPTION == FULL_DATA

  typedef TDateTimePickerData TDateTimePickerDataType;

#elif DATETIMEPICKER_DATA_OPTION == SELECTED_TIME

  typedef TSystemTime TDateTimePickerDataType;

#elif DATETIMEPICKER_DATA_OPTION == SELECTED_TIME_STRING

  typedef tstring TDateTimePickerDataType;

#endif

#if MONTHCALENDAR_DATA_OPTION == FULL_DATA

  typedef TMonthCalendarData TMonthCalendarDataType;

#elif MONTHCALENDAR_DATA_OPTION == SELECTED_TIME

  typedef TSystemTime TMonthCalendarDataType;

#endif

  struct TData
  {
    TStaticDataType StaticData;
    TEditDataType EditData;
    TListBoxDataType ListBoxData;
    TCheckListDataType CheckListData;
    TComboBoxDataType ComboBoxData;
    TComboBoxExDataType ComboBoxExData;
    TMemComboBoxDataType MemComboBoxData;
    TCheckBoxDataType CheckBoxData;
    TRadioButtonDataType RadioButtonData;
    TScrollBarDataType ScrollBarData;
    TSliderDataType SliderData;
    THotKeyDataType HotKeyData;
    TIPAddressDataType IPAddressData;
    TDateTimePickerDataType DateTimePickerData;
    TMonthCalendarDataType MonthCalendarData;
  };

protected:

  //
  // Command handlers
  //
  void CmTransferDialog1();
  void CmTransferDialog2();
  void CmTransferDialog3();

  void DisplayData(const TData& data);

  DECLARE_RESPONSE_TABLE(TTransferDemo);
};

//
// Response table for the application
//
DEFINE_RESPONSE_TABLE1(TTransferDemo, TEdit)
EV_COMMAND(CM_TRANSFERDIALOG1, CmTransferDialog1),
EV_COMMAND(CM_TRANSFERDIALOG2, CmTransferDialog2),
EV_COMMAND(CM_TRANSFERDIALOG3, CmTransferDialog3),
END_RESPONSE_TABLE;

//
// Test dialog using the dialog data transfer framework
// This dialog uses control instances for all the controls.
//
class TTransferDemoDialog1
  : public TTransferDialog<TTransferDemo::TData, TDialog>
{
public:

  //
  // Control instances
  //
  TStatic Static;
  TEdit Edit;
  TListBox ListBox;
  TCheckList CheckList;
  TComboBox ComboBox;
  TComboBoxEx ComboBoxEx;
  TMemComboBox MemComboBox;
  TCheckBox CheckBox;
  TRadioButton RadioButton;
  TScrollBar ScrollBar;
  TSlider Slider;
  THotKey HotKey;
  TIPAddress IPAddress;
  TDateTimePicker DateTimePicker;
  TMonthCalendar MonthCalendar;

  //
  // Constructor
  //
  TTransferDemoDialog1(TWindow* parent, TTransferDemo::TData& data)
    : TTransferDialog<TTransferDemo::TData, TDialog>(parent, IDD_TRANSFERDEMODIALOG, data),
    Static(this, IDC_STATIC1),
    Edit(this, IDC_EDIT),
    ListBox(this, IDC_LISTBOX),
    CheckList(this, IDC_CHECKLIST),
    ComboBox(this, IDC_COMBOBOX),
    ComboBoxEx(this, IDC_COMBOBOXEX),
    MemComboBox(this, IDC_MEMCOMBOBOX, _T("MemComboBox")),
    CheckBox(this, IDC_CHECKBOX),
    RadioButton(this, IDC_RADIOBUTTON),
    ScrollBar(this, IDC_SCROLLBAR),
    Slider(this, IDC_SLIDER, 0),
    HotKey(this, IDC_HOTKEY),
    IPAddress(this, IDC_IPADDRESS),
    DateTimePicker(this, IDC_DATETIMEPICKER),
    MonthCalendar(this, IDC_MONTHCALENDAR)
  {}

protected:

  //
  // TTransferDialog override
  //
  virtual void DoTransferData(const TTransferInfo& i, TTransferDemo::TData& a)
  {
    TransferStaticData(i, Static, a.StaticData);
    TransferEditData(i, Edit, a.EditData);

#if LISTBOX_DATA_OPTION == FULL_DATA

    TransferListBoxData(i, ListBox, a.ListBoxData);
    TransferCheckListData(i, CheckList, a.CheckListData);
    TransferComboBoxData(i, ComboBox, a.ComboBoxData);
    TransferComboBoxExData(i, ComboBoxEx, a.ComboBoxExData);
    TransferComboBoxData(i, MemComboBox, a.MemComboBoxData);

#elif LISTBOX_DATA_OPTION == SELECTED_INDEX

    TransferListBoxData(i, ListBox, a.ListBoxData);
    TransferListBoxData(i, CheckList, a.CheckListData);
    TransferComboBoxData(i, ComboBox, a.ComboBoxData);
    TransferComboBoxData(i, ComboBoxEx, a.ComboBoxExData);
    TransferComboBoxData(i, MemComboBox, a.MemComboBoxData);

#elif LISTBOX_DATA_OPTION == SELECTED_STRING

    TransferListBoxData(i, ListBox, a.ListBoxData, EXACT_LISTBOX_STRINGS == 1);
    TransferListBoxData(i, CheckList, a.CheckListData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, ComboBox, a.ComboBoxData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, ComboBoxEx, a.ComboBoxExData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, MemComboBox, a.MemComboBoxData, EXACT_LISTBOX_STRINGS == 1);

#endif

#if CHECKBOX_DATA_OPTION == BOOLEAN_STATE || CHECKBOX_DATA_OPTION == INTEGER_STATE

    TransferCheckBoxData(i, CheckBox, a.CheckBoxData);

#endif

    TransferRadioButtonData(i, RadioButton, a.RadioButtonData);
    TransferScrollBarData(i, ScrollBar, a.ScrollBarData);
    TransferSliderData(i, Slider, a.SliderData);
    TransferHotKeyData(i, HotKey, a.HotKeyData);
    TransferIPAddressData(i, IPAddress, a.IPAddressData);
    TransferDateTimePickerData(i, DateTimePicker, a.DateTimePickerData);
    TransferMonthCalendarData(i, MonthCalendar, a.MonthCalendarData);
  }

  //
  // TWindow override
  // Sets up some data to select from, if the transfer doesn't include full data for list boxes.
  //
  void SetupWindow()
  {
    TTransferDialog<TTransferDemo::TData, TDialog>::SetupWindow();

#if LISTBOX_DATA_OPTION != FULL_DATA

    ListBox.AddString(_T("ListBox1"));
    ListBox.AddString(_T("ListBox2"));
    ListBox.AddString(_T("ListBox3"));
    CheckList.AddItem(new TCheckListItem(_T("CheckList1")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList2")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList3")));
    ComboBox.AddString(_T("ComboBox1"));
    ComboBox.AddString(_T("ComboBox2"));
    ComboBox.AddString(_T("ComboBox3"));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx1")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx2")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx3")));

#endif

  }

  //
  // Puts some data in all the controls.
  //
  void CmSetControls()
  {
    Static.SetText(_T("Static"));
    Edit.SetText(_T("Edit"));
    ListBox.ClearList();
    ListBox.AddString(_T("ListBox1"));
    ListBox.AddString(_T("ListBox2"));
    ListBox.AddString(_T("ListBox3"));
    CheckList.ClearList();
    CheckList.AddItem(new TCheckListItem(_T("CheckList1")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList2")));
    CheckList.AddItem(new TCheckListItem(_T("CheckList3")));
    ComboBox.ClearList();
    ComboBox.AddString(_T("ComboBox1"));
    ComboBox.AddString(_T("ComboBox2"));
    ComboBox.AddString(_T("ComboBox3"));
    ComboBoxEx.ClearList();
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx1")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx2")));
    ComboBoxEx.InsertItem(TComboBoxExItem(_T("ComboBoxEx3")));
    MemComboBox.ClearList();
    MemComboBox.AddString(_T("MemComboBox1"));
    MemComboBox.AddString(_T("MemComboBox2"));
    MemComboBox.AddString(_T("MemComboBox3"));
    CheckBox.Check();
    SetSelectedRadioButtonIndex(RadioButton, 1);
    ScrollBar.SetRange(0, 100);
    ScrollBar.SetPosition(50);
    Slider.SetRange(0, 100);
    Slider.SetPosition(50);
    HotKey.SetHotKey(VK_F1, VK_SHIFT);
    IPAddress.SetAddress(TIPAddressBits(1, 2, 3, 4));
    DateTimePicker.SetTime(TSystemTime(2000, 1, 1));
    MonthCalendar.SetCurSel(TSystemTime(2000, 1, 1));
  }

  //
  // Clears the data from all the controls.
  //
  void CmClearControls()
  {
    Static.Clear();
    Edit.Clear();
    ListBox.ClearList();
    CheckList.ClearList();
    ComboBox.ClearList();
    ComboBox.Clear();
    ComboBoxEx.ClearList();
    ComboBoxEx.Clear();
    MemComboBox.ClearList();
    CheckBox.Uncheck();
    SetSelectedRadioButtonIndex(RadioButton, 0);
    ScrollBar.SetRange(0, 0);
    Slider.SetRange(0, 0);
    HotKey.SetHotKey(0);
    IPAddress.Clear();
    DateTimePicker.SetTime(TSystemTime::LocalTime());
    MonthCalendar.SetCurSel(TSystemTime::LocalTime());
  }

  DECLARE_RESPONSE_TABLE(TTransferDemoDialog1);
};

//
// Response table for the dialog
//
DEFINE_RESPONSE_TABLE1(TTransferDemoDialog1, TDialog)
EV_COMMAND(IDC_SETCONTROLS, CmSetControls),
EV_COMMAND(IDC_CLEARCONTROLS, CmClearControls),
END_RESPONSE_TABLE;

//
// Tests dialog data transfer.
// A static buffer is used to be able to observe the data over several executions of the dialog.
//
void TTransferDemo::CmTransferDialog1()
{
  static TTransferDemo::TData data;
  TTransferDemoDialog1 dlg(this, data);
  dlg.SetCaption(_T("Transfer Dialog 1 (using overloads for control instances)"));
  if (dlg.Execute() == IDOK)
  {
    TRACE(_T("IDOK!"));
    DisplayData(data);
  }
}

//
// Test dialog using the dialog data transfer framework
// This dialog uses the transfer function overloads for child identifiers (integers).
//
class TTransferDemoDialog2
  : public TTransferDemoDialog1
{
public:

  //
  // Constructor
  //
  TTransferDemoDialog2(TWindow* parent, TTransferDemo::TData& data)
    : TTransferDemoDialog1(parent, data)
  {}

protected:

  //
  // TTransferDialog override
  //
  virtual void DoTransferData(const TTransferInfo& i, TTransferDemo::TData& a)
  {
    TransferStaticData(i, IDC_STATIC1, a.StaticData);
    TransferEditData(i, IDC_EDIT, a.EditData);

#if LISTBOX_DATA_OPTION == FULL_DATA

    TransferListBoxData(i, IDC_LISTBOX, a.ListBoxData);
    TransferCheckListData(i, IDC_CHECKLIST, a.CheckListData);
    TransferComboBoxData(i, IDC_COMBOBOX, a.ComboBoxData);
    TransferComboBoxExData(i, IDC_COMBOBOXEX, a.ComboBoxExData);
    TransferComboBoxData(i, IDC_MEMCOMBOBOX, a.MemComboBoxData);

#elif LISTBOX_DATA_OPTION == SELECTED_INDEX

    TransferListBoxData(i, IDC_LISTBOX, a.ListBoxData);
    TransferListBoxData(i, IDC_CHECKLIST, a.CheckListData);
    TransferComboBoxData(i, IDC_COMBOBOX, a.ComboBoxData);
    TransferComboBoxData(i, IDC_COMBOBOXEX, a.ComboBoxExData);
    TransferComboBoxData(i, IDC_MEMCOMBOBOX, a.MemComboBoxData);

#elif LISTBOX_DATA_OPTION == SELECTED_STRING

    TransferListBoxData(i, IDC_LISTBOX, a.ListBoxData, EXACT_LISTBOX_STRINGS == 1);
    TransferListBoxData(i, IDC_CHECKLIST, a.CheckListData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, IDC_COMBOBOX, a.ComboBoxData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, IDC_COMBOBOXEX, a.ComboBoxExData, EXACT_LISTBOX_STRINGS == 1);
    TransferComboBoxData(i, IDC_MEMCOMBOBOX, a.MemComboBoxData, EXACT_LISTBOX_STRINGS == 1);

#endif

#if CHECKBOX_DATA_OPTION == BOOLEAN_STATE || CHECKBOX_DATA_OPTION == INTEGER_STATE

    TransferCheckBoxData(i, IDC_CHECKBOX, a.CheckBoxData);

#endif

    TransferRadioButtonData(i, IDC_RADIOBUTTON, a.RadioButtonData);
    TransferScrollBarData(i, IDC_SCROLLBAR, a.ScrollBarData);
    TransferSliderData(i, IDC_SLIDER, a.SliderData);
    TransferHotKeyData(i, IDC_HOTKEY, a.HotKeyData);
    TransferIPAddressData(i, IDC_IPADDRESS, a.IPAddressData);
    TransferDateTimePickerData(i, IDC_DATETIMEPICKER, a.DateTimePickerData);
    TransferMonthCalendarData(i, IDC_MONTHCALENDAR, a.MonthCalendarData);
  }

};

//
// Tests dialog data transfer.
// A static buffer is used to be able to observe the data over several executions of the dialog.
//
void TTransferDemo::CmTransferDialog2()
{
  static TTransferDemo::TData data;
  TTransferDemoDialog2 dlg(this, data);
  dlg.SetCaption(_T("Transfer Dialog 2 (using overloads for child identifiers)"));
  if (dlg.Execute() == IDOK)
  {
    TRACE(_T("IDOK!"));
    DisplayData(data);
  }
}

//
// Test dialog using the dialog data transfer framework
// This dialog uses the DDX-like versions of the transfer functions.
//
class TTransferDemoDialog3
  : public TTransferDemoDialog2
{
public:

  //
  // Constructor
  //
  TTransferDemoDialog3(TWindow* parent, TTransferDemo::TData& data)
    : TTransferDemoDialog2(parent, data)
  {}

protected:

  //
  // TTransferDialog override
  //
  virtual void DoTransferData(const TTransferInfo& i, TTransferDemo::TData& a)
  {
    DDX_Text(i, IDC_STATIC1, a.StaticData);
    DDX_Text(i, IDC_EDIT, a.EditData);

#if LISTBOX_DATA_OPTION == FULL_DATA

    // There is no DDX support for full data, so we cheat and use Transfer functions here.
    //
    TransferListBoxData(i, IDC_LISTBOX, a.ListBoxData);
    TransferCheckListData(i, IDC_CHECKLIST, a.CheckListData);
    TransferComboBoxData(i, IDC_COMBOBOX, a.ComboBoxData);
    TransferComboBoxExData(i, IDC_COMBOBOXEX, a.ComboBoxExData);

#elif LISTBOX_DATA_OPTION == SELECTED_INDEX

    DDX_LBIndex(i, IDC_LISTBOX, a.ListBoxData);
    DDX_LBIndex(i, IDC_CHECKLIST, a.CheckListData);
    DDX_CBIndex(i, IDC_COMBOBOX, a.ComboBoxData);
    DDX_CBIndex(i, IDC_COMBOBOXEX, a.ComboBoxExData);

#elif LISTBOX_DATA_OPTION == SELECTED_STRING && EXACT_LISTBOX_STRINGS

    DDX_LBStringExact(i, IDC_LISTBOX, a.ListBoxData);
    DDX_LBStringExact(i, IDC_CHECKLIST, a.CheckListData);
    DDX_CBStringExact(i, IDC_COMBOBOX, a.ComboBoxData);
    DDX_CBStringExact(i, IDC_COMBOBOXEX, a.ComboBoxExData);
    DDX_CBStringExact(i, IDC_MEMCOMBOBOX, a.MemComboBoxData);

#elif LISTBOX_DATA_OPTION == SELECTED_STRING && !EXACT_LISTBOX_STRINGS

    DDX_LBString(i, IDC_LISTBOX, a.ListBoxData);
    DDX_LBString(i, IDC_CHECKLIST, a.CheckListData);
    DDX_CBString(i, IDC_COMBOBOX, a.ComboBoxData);
    DDX_CBString(i, IDC_COMBOBOXEX, a.ComboBoxExData);
    DDX_CBString(i, IDC_MEMCOMBOBOX, a.MemComboBoxData);

#endif

#if CHECKBOX_DATA_OPTION == BOOLEAN_STATE

    // This shows how awkward boolean transfer is with DDX.
    //
    UINT state = a.CheckBoxData ? BST_CHECKED : BST_UNCHECKED;
    DDX_Check(i, IDC_CHECKBOX, state);
    a.CheckBoxData = (state == BST_CHECKED);

#elif CHECKBOX_DATA_OPTION == INTEGER_STATE

    DDX_Check(i, IDC_CHECKBOX, a.CheckBoxData);

#endif

    DDX_Radio(i, IDC_RADIOBUTTON, a.RadioButtonData);

#if SCROLLBAR_DATA_OPTION == FULL_DATA

    // There is no DDX support for full data, so we cheat and use Transfer functions here.
    //
    TransferScrollBarData(i, IDC_SCROLLBAR, a.ScrollBarData);
    TransferSliderData(i, IDC_SLIDER, a.SliderData);

#elif SCROLLBAR_DATA_OPTION == SCROLLBAR_POSITION

    DDX_Scroll(i, IDC_SCROLLBAR, a.ScrollBarData);
    DDX_Slider(i, IDC_SLIDER, a.SliderData);

#endif

    DDX_HotKey(i, IDC_HOTKEY, a.HotKeyData);

#if IPADDRESS_DATA_OPTION == ADDRESS_OBJECT

    uint32 address = a.IPAddressData;
    DDX_IPAddress(i, IDC_IPADDRESS, address);
    a.IPAddressData = address;

#elif IPADDRESS_DATA_OPTION == INTEGER_ADDRESS

    DDX_IPAddress(i, IDC_IPADDRESS, a.IPAddressData);

#endif

#if DATETIMEPICKER_DATA_OPTION == FULL_DATA

    // There is no DDX support for full data, so we cheat and use Transfer functions here.
    //
    TransferDateTimePickerData(i, IDC_DATETIMEPICKER, a.DateTimePickerData);

#elif DATETIMEPICKER_DATA_OPTION == SELECTED_TIME || DATETIMEPICKER_DATA_OPTION == SELECTED_TIME_STRING

    DDX_DateTimeCtrl(i, IDC_DATETIMEPICKER, a.DateTimePickerData);

#endif

#if MONTHCALENDAR_DATA_OPTION == FULL_DATA

    // There is no DDX support for full data, so we cheat and use Transfer functions here.
    //
    TransferMonthCalendarData(i, IDC_DATETIMEPICKER, a.MonthCalendarData);

#elif MONTHCALENDAR_DATA_OPTION == SELECTED_TIME

    DDX_MonthCalCtrl(i, IDC_MONTHCALENDAR, a.MonthCalendarData);

#endif

  }

};

//
// Tests dialog data transfer.
// A static buffer is used to be able to observe the data over several executions of the dialog.
//
void TTransferDemo::CmTransferDialog3()
{
  static TTransferDemo::TData data;
  TTransferDemoDialog3 dlg(this, data);
  dlg.SetCaption(_T("Transfer Dialog 3 (using DDX-like functions)"));
  if (dlg.Execute() == IDOK)
  {
    TRACE(_T("IDOK!"));
    DisplayData(data);
  }
}


void TTransferDemo::DisplayData(const TData& data)
{
  tostringstream text;
  text << _T("StaticData: ") << data.StaticData << _T("\r\n");
  text << _T("EditData: ") << data.EditData << _T("\r\n");
  text << _T("ListBoxData: ") << data.ListBoxData << _T("\r\n");
  text << _T("CheckListData: ") << data.CheckListData << _T("\r\n");
  text << _T("ComboBoxData: ") << data.ComboBoxData << _T("\r\n");
  text << _T("ComboBoxExData: ") << data.ComboBoxExData << _T("\r\n");
  text << _T("MemComboBoxData: ") << data.MemComboBoxData << _T("\r\n");
  text << _T("CheckBoxData: ") << data.CheckBoxData << _T("\r\n");
  text << _T("RadioButtonData: ") << data.RadioButtonData << _T("\r\n");
  text << _T("ScrollBarData: ") << data.ScrollBarData << _T("\r\n");
  text << _T("SliderData: ") << data.SliderData << _T("\r\n");
  text << _T("HotKeyData: ") << data.HotKeyData << _T("\r\n");
  text << _T("IPAddressData: ") << data.IPAddressData << _T("\r\n");
  text << _T("DateTimePickerData: ") << data.DateTimePickerData.GetYear() << _T("-")
    << data.DateTimePickerData.GetMonth() << _T("-") << data.DateTimePickerData.GetDay() << _T("\r\n");
  text << _T("MonthCalendarData: ") << data.MonthCalendarData.GetYear() << _T("-")
    << data.MonthCalendarData.GetMonth() << _T("-") << data.MonthCalendarData.GetDay() << _T("\r\n");

  SetText(text.str());
}


void TClassesMDIClient::CmTransfer()
{
  _TCHAR text[] = _T("Dialog Data Transfer\r\n")
    _T("This code demonstrates the dialog data transfer framework (DDT) in OWLNext.");

  TMDIChild* child = new TMDIChild(*this, _T("Dialog Data Transfer"), new TTransferDemo(0, 0, text, 0, 0, 0, 0, 0, true));
  child->SetMenuDescr(TMenuDescr(IDM_TRANSFER, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);

}

