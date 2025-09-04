//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TListBox and TListBoxData.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_LISTBOX_H)
# include <owl/listbox.h>
#endif
#include <stdlib.h>

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

// Let the compiler know that the following template instances will be defined elsewhere. 
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

#define MULTIPLESEL    (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)

//
// TListBoxData constructor
//
TListBoxData::TListBoxData()
{
  Strings = new TStringArray;     
  ItemDatas = new TUint32Array;   
  SelIndices = new TIntArray;  
}

//
// TListBoxData destructor
//
TListBoxData::~TListBoxData()
{
  delete Strings;
  delete ItemDatas;
  delete SelIndices;
}
//
void TListBoxData::Clear() 
{
  Strings->Flush();
  ItemDatas->Flush();
  ResetSelections();
}
//
int TListBoxData::GetSelCount() const 
{
  return SelIndices->Size();
}
//
void TListBoxData::ResetSelections() 
{
  SelIndices->Flush();
}

//
// Adds "str" to "Strings"
//
// If "isSelected" is true, marks it as selected
//
void
TListBoxData::AddString(LPCTSTR str, bool isSelected)
{
  Strings->Add(str);
  if (isSelected)
    Select(Strings->Size()-1);
}

//
// Adds str and associated item data to strings and datas
//
void
TListBoxData::AddStringItem(LPCTSTR str, uint32 itemData, bool isSelected)
{
  ItemDatas->Add(itemData);
  AddString(str, isSelected);
}

//
// Selects an item at a given index.
//
void
TListBoxData::Select(int index)
{
  if (index != LB_ERR && SelIndices->Find(index)==(int)NPOS)
    SelIndices->Add(index);
}

//
// Adds "str" to selection lists if it is in Strings
//
void
TListBoxData::SelectString(LPCTSTR str)
{
  for (uint i = 0; i < Strings->Size(); i++)
    if (_tcscmp((*Strings)[i].c_str(), str) == 0) {
      Select(i);
      break;
    }
}

//
// Returns the length of the string at the passed selection index
// excluding the terminating 0
//
int
TListBoxData::GetSelStringLength(int index) const
{
  if (index >= 0 && index < GetSelCount())
    return (*Strings)[(*SelIndices)[index]].length();
  return -1;
}

//
// Copies the string at the passed selection index into buffer
//
// bufferSize includes the terminating 0
//
void
TListBoxData::GetSelString(_TCHAR * buffer, int bufferSize, int index) const
{
  if (bufferSize > 0) {
    if (index < 0 || index >= GetSelCount())
      *buffer = 0;

    else {
      _tcsncpy(buffer, (*Strings)[(*SelIndices)[index]].c_str(), bufferSize-1);
      buffer[bufferSize - 1] = 0;
    }
  }
}

//
// copies the string at the passed index in SelStrings into str
//
void
TListBoxData::GetSelString(owl_string& str, int index) const
{
  if (index >= 0 && index < GetSelCount())
    str = (*Strings)[(*SelIndices)[index]];
  else
    str = _T("");
}

//----------------------------------------------------------------------------

//
// Constructor for TListBox
//
// Initializes its data fields using parameters passed and default values
//
// By default, a native listbox associated with the TListBox will:
//   - be visible upon creation
//   - have a border and a vertical scrollbar
//   - maintain entries in alphabetical order
//   - notify its parent when a selection is made
//
TListBox::TListBox(TWindow*   parent,
                   int        id,
                   int x, int y, int w, int h,
                   TModule*   module)
:
  TControl(parent, id, 0, x, y, w, h, module)
{
  Attr.Style |= LBS_STANDARD;
  Attr.ExStyle |= WS_EX_CLIENTEDGE;  // Creates 3d sunken inside edge
}

//
//
//
TListBox::TListBox(TWindow*   parent,
                   int        resourceId,
                   TModule*   module)
:
  TControl(parent, resourceId, module)
{
}

//
//
//
LPCTSTR
TListBox::GetClassName()
{
  return _T("LISTBOX");
}

//
// Transfers state information for a TListBox
//
// Transfers the items and selection of the list to or from a transfer
// buffer if tdSetData or tdGetData, respectively, is passed as the
// direction
//
// Buffer should point to a TListBoxData which points to the data to be
// transferred
//
// Transfer returns the size of TListBoxData
//
// To retrieve the size without transferring data, pass tdSizeData as the
// direction
//
uint
TListBox::Transfer(void* buffer, TTransferDirection direction)
{
  long           style = GetStyle();
  TListBoxData*  listBoxData = (TListBoxData*)buffer;

  if (direction == tdGetData) {

    // First, clear out Strings array and fill with contents of list box
    //
    listBoxData->Clear();

    // Pre-calculate max string length so that one big buffer can be used
    //
    int  count = GetCount();
    int  maxStrLen = 0;
    int  i;
    for (i = 0; i < count; i++) {
      int  strLen = GetStringLen(i);
      if (strLen > maxStrLen)
        maxStrLen = strLen;
    }

    // Get each string and item data in the listbox & add to listboxdata
    //
    TAPointer<TCHAR> tmpStr = new TCHAR[maxStrLen+1];
    for (i = 0; i < GetCount(); i++) {
      GetString(tmpStr, i);
      listBoxData->AddStringItem(tmpStr, GetItemData(i), false);
    }

    // Update transfer data with new selected item(s)
    //
    listBoxData->ResetSelections();

    if (!(style & MULTIPLESEL)) {
      // Single selection
      //
      listBoxData->Select(GetSelIndex());
    }
    else {
      // Multiple selection
      //
      int  selCount = GetSelCount();
      if (selCount > 0) {
        int*  selections = new int[selCount];

        GetSelIndexes(selections, selCount);

        // Select each item by index
        //
        for (int selIndex = 0; selIndex < selCount; selIndex++)
          listBoxData->Select(selections[selIndex]);

        delete[] selections;
      }
    }
  }
  else if (direction == tdSetData) {
    ClearList();

    // Add each string, item data and selections in listBoxData to list box
    //
    const int noSelection = -1;
    uint  selCount = listBoxData->GetSelCount();  // for multi selection
    int  selIndex = noSelection;                 // for single selection
    for (uint i = 0; i < listBoxData->GetStrings().Size(); i++) {
      // Index may be different from i when the listbox is sorted.
      //
      int index = AddString(listBoxData->GetStrings()[i].c_str());
      if(i < listBoxData->GetItemDatas().Size())
        SetItemData(index, listBoxData->GetItemDatas()[i]);
      if (style & MULTIPLESEL) {
        for (uint j = 0; j < selCount; j++)
          if (listBoxData->GetSelIndices()[j] == (int)i) {
            SetSel(index, true);
            break;
          }
      }
      else {
        if (selCount && (uint)listBoxData->GetSelIndices()[0] == i)
          selIndex = index;
        else
          // Inserted something before item and the item to select has been
          // pushed further down in the list.
          //
          if (selIndex != noSelection && index <= selIndex)
            selIndex++;
      }
    }
    if (selIndex != noSelection && !(style & MULTIPLESEL))
      SetSelIndex(selIndex);
  }

  return sizeof(TListBoxData);
}

//
// Returns the index of the first string in the associated listbox
// which is the same as the passed string
//
// Searches for a match beginning at the passed indexStart
//
// If a match is not found after the last string has been compared,
// the search continues from the beginning of the list until a match
// is found or until the list has been completely traversed
//
// Searches from beginning of list when -1 is passed as the index
//
// Returns the index of the selected string; a negative value is returned
// if an error occurs
//
// For single or multiple-selection list boxes
//
int
TListBox::FindExactString(LPCTSTR findStr, int indexStart) const
{
  bool  found = false;
  int   firstMatch = indexStart = FindString(findStr, indexStart);
  do {
    if (indexStart > -1) {
      TAPointer<TCHAR> tmpStr = new TCHAR[GetStringLen(indexStart) + 1];

      GetString(tmpStr, indexStart);

      if (_tcscmp(tmpStr, findStr) == 0)
        found = true;
      else
        indexStart = FindString(findStr, indexStart);
    }
  } while (!found && indexStart != firstMatch);

  return found ? indexStart : -1;
}

//
// For use with single-selection list boxes (and combo boxes)
//
// Retrieves the text of the string which is selected in the associated
// listbox
//
// Returns the number of characters copied; -1 is returned if no string
// is selected or this is a multiple-selection list box
//
// Since the Windows function is not passed a size parameter, we have to
// allocate a string to hold the largest string (gotten from a query), and
// copy a part of it
//
int
TListBox::GetSelString(LPTSTR str, int maxChars) const
{
  int  index = GetSelIndex();

  if (index > -1) {
    int  length = GetStringLen(index);

    if (maxChars >= length)
      return GetString(str, index);
    else if(length > 0){
      TAPointer<TCHAR> tmpStr = new TCHAR[length + 1];
      GetString(tmpStr, index);
      _tcsncpy(str, tmpStr, maxChars);
      return maxChars;
    }
  }
  return -1;
}

//
// Returns the number of selected items in the list box. For
// multiple-selection list boxes only
//
int
TListBox::GetSelCount() const
{
  if (!(GetStyle() & MULTIPLESEL))
    return GetSelIndex() < 0 ? 0 : 1;

  // Multiple-selection list box
  //
  return (int)CONST_CAST(TListBox*,this)->SendMessage(LB_GETSELCOUNT);
}

//
// Retrieves the text of the strings which are selected in the
// associated listbox
//
// Returns the number of items put into Strings.  -1 is returned if this is
// not a multiple-selection list box
//
// Since the Windows function is not passed a size parameter, we have to
// allocate a string to hold the largest string (gotten from a query), and
// copy a part of it
//
// Only for use with multiple-selection list boxes
//
int
TListBox::GetSelStrings(LPTSTR* strs, int maxCount, int maxChars) const
{
  if (!(GetStyle() & MULTIPLESEL))
    return -1;

  int i = GetSelCount();

  if (i < maxCount)
    maxCount = i;

  if (maxCount > 0) {
    int*  selections = new int[maxCount];

    GetSelIndexes(selections, maxCount);

    for (int selIndex = 0; selIndex < maxCount; selIndex++) {
      int  tmpStrLen = GetStringLen(selections[selIndex]);

      if (maxChars >= tmpStrLen)
        GetString(strs[selIndex], selections[selIndex]);

      else if(tmpStrLen > 0) {
        TAPointer<TCHAR> tmpStr = new TCHAR[tmpStrLen+1];
        GetString(tmpStr, selections[selIndex]);
        _tcsncpy(strs[selIndex], tmpStr, maxChars);
      }
    }
    delete[] selections;
  }
  return maxCount;
}

//
// Selects the first string in the associated listbox following the
// passed index which begins with the passed string
//
// Searches for a match beginning at the passed Index. if a match is not
// found after the last string has been compared, the search continues
// from the beginning of the list until a match is found or until the list
// has been completely traversed
//
// Searches from beginning of list when -1 is passed as the index
//
// Returns the index of the selected string. a negative value is returned
// if an error occurs
//
// Only for single-selection list boxes
//
int
TListBox::SetSelString(LPCTSTR findStr, int indexStart)
{
  if (!(GetStyle() & MULTIPLESEL))
    return (int)SendMessage(LB_SELECTSTRING, indexStart, TParam2(findStr));
  return -1;
}

//
// Selects the strings in the associated list box which begin with
// the passed prefixes
//
// For each string the search begins at the beginning of the list
// and continues until a match is found or until the list has been
// completely traversed
//
// If ShouldSet is true, the matched strings are selected and highlighted;
// otherwise the highlight is removed from the matched strings and they
// are no longer selected
//
// Returns the number of strings successfully selected or deselected
//
// If NumSelections is less than zero, all strings are selected or deselected
// and a negative value is returned on failure
//
// Only for multiple-selection list boxes (-1 is returned if this is not
// a multiple-selection list box)
//
int
TListBox::SetSelStrings(LPCTSTR* strs, int numSelections, bool shouldSet)
{
  if (!(GetStyle() & MULTIPLESEL))
    return -1;

  if (numSelections < 0)
    return SetSel(-1, shouldSet);

  int  successes = 0;
  for (int i = 0; i < numSelections; i++) {
    int  selIndex;
    if ((selIndex = FindString(strs[i], -1)) > -1)
      if (SetSel(selIndex, shouldSet) > -1)
        successes++;
  }
  return successes;
}

//
// Returns the index of the selected string in the associated listbox
//
// A negative value is returned if no string is selected or this
// is a multiple-selection list box
//
// Only for single-selection list boxes
//
int
TListBox::GetSelIndex() const
{
  if (!(GetStyle() & MULTIPLESEL))
    return (int)CONST_CAST(TListBox*,this)->SendMessage(LB_GETCURSEL);
  return -1;
}

//
// Fills indexes with the indexes of up to maxCount selected strings
//
// Returns number of items put in the array (-1 for single-selection
// list boxes)
//
int
TListBox::GetSelIndexes(int* indexes, int maxCount) const
{
  if (!(GetStyle() & MULTIPLESEL))
    return -1;
  return (int)CONST_CAST(TListBox*,this)->SendMessage(LB_GETSELITEMS,
                                                        maxCount,
                                                        TParam2(indexes));
}

// Selects the string at passed index in the associated listbox and
// forces the string into view
//
// Clears selection when -1 is passed as the index. a negative value is
// returned if an error occurs
//
// Only for single-selection list boxes
//
int
TListBox::SetSelIndex(int index)
{
  if (!(GetStyle() & MULTIPLESEL))
    return (int)SendMessage(LB_SETCURSEL, index);
  return -1;
}

//
// Selects/deselects the strings in the associated list box at the
// passed indexes
//
// If ShouldSet is true, the indexed strings are selected and highlighted;
// otherwise the highlight is removed and they are no longer selected
//
// Returns the number of strings successfully selected or deselected(-1
// if not a multiple-selection list box)
//
// If NumSelections is less than zero, all strings are selected or deselected
// and a negative value is returned on failure
//
// only for multiple-selection list boxes
//
int
TListBox::SetSelIndexes(int* indexes, int numSelections, bool shouldSet)
{
  int  successes = 0;

  if (!(GetStyle() & MULTIPLESEL))
    return -1;  // including if it's a combobox

  if (numSelections < 0)
    return (int)SendMessage(LB_SETSEL, shouldSet, -1);

  else {
    for (int i = 0; i < numSelections; i++)
      if ((int)SendMessage(LB_SETSEL, shouldSet, indexes[i]) > -1)
        successes++;
  }
  return successes;
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TListBox, TControl);

#if !defined(BI_NO_OBJ_STREAMING)

//
// Reads an instance of TListBox from the supplied ipstream
//
void*
TListBox::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TControl*)GetObject(), is);
  return GetObject();
}

//
// Writes the TListBox to the supplied opstream
//
void
TListBox::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TControl*)GetObject(), os);
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

