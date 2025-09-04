//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TComboBox & TComboBoxData.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif

#include <owl/pch.h>

#if !defined(OWL_COMBOBOX_H)
# include <owl/combobox.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlControl);

#if !defined(SECTION) || SECTION == 1

//
// Constructor for a TComboBoxData object
//
TComboBoxData::TComboBoxData()
{
  Strings = new TStringArray;
  ItemDatas = new TDwordArray;
  SelIndex = 0;
}

//
// Destructor for TComboBoxData
//
TComboBoxData::~TComboBoxData()
{
  delete Strings;
  delete ItemDatas;
}

//30.05.2007 - Submitted by Frank Rast:    
//TComboBoxData needs a copy constructor because the
//default copy constructor does not deep copy the
//protected data of this class. For the same reason a
//assignment operator is needed.
TComboBoxData::TComboBoxData(const TComboBoxData& tCBD)
{
  Strings = new TStringArray;
  ItemDatas = new TDwordArray;
  SelIndex = 0;
	*this = tCBD;
}

TComboBoxData &TComboBoxData::operator=(const TComboBoxData& tCBD)
{
	*Strings = *tCBD.Strings;
	*ItemDatas = *tCBD.ItemDatas;
	Selection = tCBD.Selection;
  SelIndex = tCBD.SelIndex;
	return *this;
}


#endif
#if !defined(SECTION) || SECTION == 2

//
//
void TComboBoxData::Clear() 
{
  Strings->Flush();
  ItemDatas->Flush();
  ResetSelections();
}

#endif
#if !defined(SECTION) || SECTION == 3

//
// Adds a given string to the "Strings" array and copies it into
// "Selection" if "isSelected" is true
//
void
TComboBoxData::AddString(LPCTSTR str, bool isSelected)
{
  Strings->Add(str); // add to end
  if (isSelected)
    Select(Strings->Size()-1);
}

#endif
#if !defined(SECTION) || SECTION == 4

//
// Adds a given string and uint32 item to the "Strings" and "ItemDatas"
// array and copies the string into "Selection" if "isSelected" is true
//
void
TComboBoxData::AddStringItem(LPCTSTR str, uint32 itemData, bool isSelected)
{
  ItemDatas->Add(itemData);
  AddString(str, isSelected);
}

#endif
#if !defined(SECTION) || SECTION == 5

//
// Selects an item at a given index.
//
void
TComboBoxData::Select(int index)
{
  if (index != CB_ERR) {
    SelIndex = index;
    if (index < (int)Strings->Size())
      Selection = (*Strings)[index];
  }
}

#endif
#if !defined(SECTION) || SECTION == 6

//
// Selects "str", marking the matching String entry (if any) as selected
//
void
TComboBoxData::SelectString(LPCTSTR str)
{
  int numStrings = Strings->Size();
  SelIndex = CB_ERR;
  for (int i = 0; i < numStrings; i++)
    if (_tcscmp((*Strings)[i].c_str(), str) == 0) {
      SelIndex = i;
      break;
    }
  if (Selection != str)
    Selection = str;
}

#endif
#if !defined(SECTION) || SECTION == 7

//
// Returns the length of the selection string excluding the terminating 0
//
int
TComboBoxData::GetSelStringLength() const
{
  return Selection.length();
}

#endif
#if !defined(SECTION) || SECTION == 8

//
// Copies the selected string into Buffer. BufferSize includes the terminating 0
//
void
TComboBoxData::GetSelString(_TCHAR * buffer, int bufferSize) const
{
  if (bufferSize > 0) {
    _tcsncpy(buffer, Selection.c_str(), bufferSize-1);
    buffer[bufferSize - 1] = 0;
  }
}

#endif
#if !defined(SECTION) || SECTION == 9

//----------------------------------------------------------------------------

//
// Constructors for a TComboBox object
//
// By default, an MS-Windows combobox associated with the TComboBox will have
// a vertical scrollbar and will maintain its entries in alphabetical order
//
TComboBox::TComboBox(TWindow*        parent,
                     int             id,
                     int x, int y, int w, int h,
                     uint32          style,
                     uint            textLimit,
                     TModule*        module)
:
  TListBox(parent, id, x, y, w, h, module),
  TextLimit(textLimit)
{
  Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP |
               CBS_SORT | CBS_AUTOHSCROLL | WS_VSCROLL | style;
  TRACEX(OwlControl, OWL_CDLEVEL, _T("TComboBox constructed @") << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 10

//
//
//
TComboBox::TComboBox(TWindow*   parent,
                     int        resourceId,
                     uint       textLimit,
                     TModule*   module)
:
  TListBox(parent, resourceId, module),
  TextLimit(textLimit)
{
  TRACEX(OwlControl, OWL_CDLEVEL, _T("TComboBox constructed from resource @") << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 11

//
//
//
TComboBox::~TComboBox()
{
  TRACEX(OwlControl, OWL_CDLEVEL, _T("TComboBox destructed @") << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 12

//
// Sets and selects the contents of the associated edit control to the
// supplied string
//
void
TComboBox::SetText(LPCTSTR str)
{
  // If str is 0, then use empty str
  //
  if (!str)
    str = _T("");

  // If not in listbox, then set the edit/static portion
  //
  if (SetSelStringExact(str, -1) < 0)//DLN was SetSelString, which would use partial matches 
    {
    SetWindowText(str);
    SetEditSel(0, ::_tcslen(str));
  }
}

#endif
#if !defined(SECTION) || SECTION == 13

//
// Sets the text length limit member and associated control
//
void
TComboBox::SetTextLimit(uint textlimit)
{
  TextLimit = textlimit;
  if (GetHandle() && TextLimit != 0)
    SendMessage(CB_LIMITTEXT, TextLimit-1);
}

#endif
#if !defined(SECTION) || SECTION == 14

//
// Returns, in the supplied reference parameters, the starting and
// ending positions of the text selected in the associated edit control
//
// Returns CB_ERR is the combo box has no edit control
//
int
TComboBox::GetEditSel(int& startPos, int& endPos)
{
  TResult  retValue = SendMessage(CB_GETEDITSEL);

  startPos = LoUint16(retValue);
  endPos   = HiUint16(retValue);

  return (int)retValue;
}

#endif
#if !defined(SECTION) || SECTION == 15

//
// Shows or hides the drop-down list
//
void
TComboBox::ShowList(bool show)
{
  if ((GetWindowLong(GWL_STYLE) & CBS_DROPDOWN) == CBS_DROPDOWN)
    SendMessage(CB_SHOWDROPDOWN, show);
}

#endif
#if !defined(SECTION) || SECTION == 16

//
void 
TComboBox::GetDroppedControlRect(TRect& rect) const 
{
  PRECONDITION(GetHandle());
  CONST_CAST(TComboBox*,this)->
    SendMessage(CB_GETDROPPEDCONTROLRECT, 0, TParam2((TRect *)&rect));

  // BUG suggested by Luigi Bianchi
  int vertRes = TDesktopDC().GetDeviceCaps(VERTRES);
  if(rect.Bottom() >= vertRes) 
    rect.Offset(0, -rect.Height());
}

#endif
#if !defined(SECTION) || SECTION == 17

static void
DoAddStringToCB(owl_string& str, void* comboBox)
{
  ((TListBox*)comboBox)->AddString(str.c_str());
}

//
// Transfers the items and selection of the combo box to or from a transfer
// buffer if tdSetData or tdGetData, respectively, is passed as the
// direction
//
// Buffer should point to a TComboBoxData which points to the data to be
// transferred
//
// Transfer returns the size of TComboBoxData
//
// To retrieve the size without transferring data, pass tdSizeData as the
// direction
//
uint
TComboBox::Transfer(void* buffer, TTransferDirection direction)
{
  TComboBoxData* comboBoxData = (TComboBoxData*)buffer;

  if (direction == tdGetData) {
    // Clear out Strings array and fill with contents of list box part
    // Prescan for longest string to allow a single temp allocation
    //
    comboBoxData->Clear();

    int  count = GetCount();
    int  maxStringLen = 0;
    int  i;
    for (i = 0; i < count; i++) {
      int  stringLen = GetStringLen(i);
      if (stringLen > maxStringLen)
        maxStringLen = stringLen;
    }
    {
      TAPointer<TCHAR> tmpString = new TCHAR[maxStringLen+1];
      for (i = 0; i < count; i++) {
        GetString(tmpString, i);
        comboBoxData->AddStringItem(tmpString, GetItemData(i), false);
      }
    }

    // Get the sel string from the list by index, or if no index from the
    // edit box
    //
    int selIndex = GetSelIndex();
    if (selIndex >= 0) {
      int  stringLen = GetStringLen(selIndex);
      if (stringLen > 0) {
        TAPointer<TCHAR> str = new TCHAR[stringLen+1];
        GetString(str, selIndex);
        comboBoxData->SelectString(str);
      }
      else
        comboBoxData->SelectString(_T(""));
    }
    else {
      int  stringLen = GetWindowTextLength();
      if (stringLen > 0) {
        TAPointer<TCHAR> str = new TCHAR[stringLen+1];
        GetWindowText(str, stringLen+1);
        comboBoxData->SelectString(str);
      }
      else
        comboBoxData->SelectString(_T(""));
    }
  }
  else if (direction == tdSetData) {
    ClearList();
    // Frank Rast: Set the ItemData for the correct index. Previous code did not handle different item order
		for (unsigned i = 0; i < comboBoxData->GetStrings().GetItemsInContainer(); i++)
		{	int index = AddString(comboBoxData->GetStrings()[i].c_str());
			if(i < comboBoxData->GetItemDatas().GetItemsInContainer())
				SetItemData(index, comboBoxData->GetItemDatas()[i]);
		}

    SetWindowText(comboBoxData->GetSelection().c_str());

    if (comboBoxData->GetSelIndex() >= 0)
      SetSelStringExact(comboBoxData->GetSelection().c_str(), -1); //DLN was SetSelString, which would use partial matches 
  }

  return sizeof(TComboBoxData);
}

#endif
#if !defined(SECTION) || SECTION == 18

//
// Returns name of predefined Windows combobox class
//
LPCTSTR
TComboBox::GetClassName()
{
  return _T("COMBOBOX");
}

//
// Limits the amount of text that the user can enter in the combo box's
// edit control to the value of TextLimit minus 1
//
// Creates plain TWindow aliases for the children in the combo box so that
// TWindow can handle kill focus messages for focus support.
//
void
TComboBox::SetupWindow()
{
  TRACEX(OwlControl, 1, _T("TComboBox::SetupWindow() @ ") << (void*)this);

  TListBox::SetupWindow();

  SetTextLimit(TextLimit);

  THandle hWnd = ::GetWindow(GetHandle(), GW_CHILD);
  while (hWnd) {
    if (!GetWindowPtr(hWnd))
      (new TWindow(hWnd))->SetParent(this);
    hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
  }
}

//
// Cleanup aliases created in SetupWindow
//
void
TComboBox::CleanupWindow()
{
  HWND hWnd = ::GetWindow(GetHandle(), GW_CHILD);
  while (hWnd) {
    TWindow* wnd = GetWindowPtr(hWnd);
    delete wnd;
    hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
  }

  // call base class
  //
  TListBox::CleanupWindow();

  TRACEX(OwlControl, 1, _T("TComboBox::CleanupWindow() @ ") << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 19

IMPLEMENT_STREAMABLE1(TComboBox, TListBox);

#if !defined(BI_NO_OBJ_STREAMING)

//
// Reads an instance of TComboBox from the supplied ipstream
//
void*
TComboBox::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TListBox*)GetObject(), is);
  is >> GetObject()->TextLimit;
  return GetObject();
}

//
// Writes the TComboBox to the supplied opstream
//
void
TComboBox::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TListBox*)GetObject(), os);
  os << GetObject()->TextLimit;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE


