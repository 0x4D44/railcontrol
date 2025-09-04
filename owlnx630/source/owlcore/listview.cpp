//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-12-16 09:10:46 $
//
// Implementation of class TListView
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_LISTVIEW_H)
# include <owl/listview.h>
#endif
#if !defined(OWL_INPUTDIA_H)
# include <owl/inputdia.h>
#endif
#include <owl/listview.rh>
#include <owl/docview.rh>
#include <owl/edit.rh>
#include <stdio.h>

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlDocView);        // General Doc/View diagnostic group

// Let the compiler know that the following template instances will be defined elsewhere.
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

const TCHAR VirtualLastLineStr[] = _T("---");  // Last virtual line appended to list

DEFINE_RESPONSE_TABLE1(TListView, TListBox)
  EV_COMMAND(CM_EDITUNDO,   CmEditUndo),
  EV_COMMAND(CM_EDITCUT,    CmEditCut),
  EV_COMMAND(CM_EDITCOPY,   CmEditCopy),
  EV_COMMAND(CM_EDITPASTE,  CmEditPaste),
  EV_COMMAND(CM_EDITCLEAR,  CmEditClear),
  EV_COMMAND(CM_EDITDELETE, CmEditDelete),
  EV_COMMAND(CM_EDITADD,    CmEditAdd),
  EV_COMMAND(CM_EDITEDIT,   CmEditItem),
  EV_WM_GETDLGCODE,
  EV_NOTIFY_AT_CHILD(LBN_DBLCLK, CmEditItem),
  EV_NOTIFY_AT_CHILD(LBN_SELCHANGE, CmSelChange),
  EV_VN_DOCCLOSED,
  EV_VN_ISWINDOW,
  EV_VN_ISDIRTY,
  EV_VN_COMMIT,
  EV_VN_REVERT,
END_RESPONSE_TABLE;

//
//
//
TListView::TListView(TDocument& doc, TWindow* parent)
:
  TListBox(parent, GetNextViewId(), 0,0,0,0),
  TView(doc),
  DirtyFlag(false),
  Origin(0),
  MaxWidth(0)
{
  Attr.Style &= ~(LBS_SORT);
  Attr.Style |= (WS_HSCROLL | LBS_NOINTEGRALHEIGHT);
  Attr.AccelTable = IDA_LISTVIEW;
  if (::FindResource(*GetModule(), TResId(IDM_LISTVIEW), RT_MENU))
    SetViewMenu(new TMenuDescr(IDM_LISTVIEW, 0,1,0,0,0,1, GetModule()));
}

//
// Add a string into the view.
// Return the index at which the string is added.
//
int
TListView::AddString(LPCTSTR str)
{
  long style = GetWindowLong(GWL_STYLE);
  if (!(style & LBS_SORT)) {
    int itemsInListBox = GetCount();
    if (itemsInListBox > 0) {
      // before the end of list marker
      return InsertString(str, itemsInListBox-1);
    }
  }
  return TListBox::AddString(str);
}

//
//
//
void
TListView::SetExtent(LPCTSTR str)
{
  int len = ::_tcslen(str);
  if (len == 0)
    return;

  TClientDC  clientDC(*this);
  TSize extent = clientDC.GetTextExtent(str, len);
  extent.cx += 2; // room for focus rectangle

  if (extent.cx > MaxWidth)
    SetHorizontalExtent(MaxWidth = extent.cx);
}

//
//
//
bool
TListView::VnDocClosed(int omode)
{
  if (DirtyFlag == 2 || !(omode & ofWrite))  // make sure someone else's write
    return false;
  int top = GetTopIndex();
  int sel = GetSelIndex();
  LoadData(top, sel);
  return true;
}

//
//
//
bool
TListView::LoadData(int top, int sel)
{
  CmEditClear();    // Clear list & remove virtual last line temporarily
  DeleteString(0);

  long style = GetWindowLong(GWL_STYLE);
  if (!(style & LBS_SORT))
    TListBox::AddString(VirtualLastLineStr);     // Append virtual last line

  DirtyFlag = false;

  _tistream* inStream;
  if ((inStream = Doc->InStream(ios::in)) == 0) {
    Doc->PostError(IDS_UNABLEOPEN, MB_OK);
    return false;
  }
  bool status = false;
  for (;;) {
    _TCHAR buf[100+1];
    inStream->getline(buf, COUNTOF(buf)-1);
    if (!inStream->gcount() && !inStream->good()) {
      status = ToBool(inStream->eof());
      break;
    }
    AddString(buf);
    SetExtent(buf);
  }
  SetTopIndex(top);
  SetSelIndex(sel);
  delete inStream;   // close file in case process switch
  if (!status)
    Doc->PostError(IDS_READERROR, MB_OK);
  return status;
}

//
//
//
bool
TListView::Create()
{
  TRY {
    TListBox::Create();   // throws exception TWindow::TXWindow
  }
  CATCH( (TXOwl& ) {
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    return true;   // cannot return false - throws another exception
  })
  if (Doc->GetDocPath() == 0) {
    CmEditClear();         // perform any clearing initialization
    return true;           // new file, no data to display
  }
  if (!LoadData(0, 0))
    NotOK();
  return true;
}

//
//
//
bool
TListView::VnCommit(bool force)
{
  if (!force && !DirtyFlag)
    return true;

  _tostream* outStream = Doc->OutStream(ios::out);
  if (outStream == 0) {
    Doc->PostError(IDS_UNABLEOPEN, MB_OK);
    return false;
  }
  outStream->seekp(Origin);
  int count = GetCount();
  for (int index = 0; index < count-1; index++) {  // don't write last virtual line
    int len = GetStringLen(index);
    TAPointer<_TCHAR> buf = new _TCHAR[len+1];
    GetString((_TCHAR*)buf, index);
    *outStream << (_TCHAR*)buf << _T('\n');
    //delete buf;
  }
  DirtyFlag = 2;           // to detect our own close notification

  bool status = ToBool(outStream->good());
  delete outStream;
  DirtyFlag = false;
  if (!status)
    Doc->PostError(IDS_WRITEERROR, MB_OK);

  return status;
}

//
//
//
bool
TListView::VnRevert(bool clear)
{
  if (!clear && Doc->GetDocPath() != 0)
    return LoadData(0,0);
  CmEditClear();
  DirtyFlag = false;
  return true;
}

//
//
//
uint
TListView::EvGetDlgCode(MSG *)
{
  uint retVal = (uint)DefaultProcessing();
  retVal |= DLGC_WANTCHARS;
  return retVal;
}

//
//
//
void
TListView::CmEditUndo()
{
#if BI_MSG_LANGUAGE == 0x0411
  MessageBox("‚±‚Ì‹@”\‚ÍŽÀ‘•‚³‚ê‚Ä‚¢‚Ü‚¹‚ñ", "Œ³‚É–ß‚·", MB_OK);
#else
  MessageBox(_T("Feature not implemented"), _T("Undo"), MB_OK);  
#endif
}

//
//
//
void
TListView::CmEditCut()
{
  CmEditCopy();
  CmEditDelete();
}

//
//
//
void
TListView::CmEditCopy()
{
  int index = GetSelIndex();
  int count = GetCount();
  if (count <= 1 || index >= count)
    return;

  TClipboard cb(*this, false);
  if (cb.EmptyClipboard()) {
    int len = GetStringLen(index);
    HANDLE cbhdl = ::GlobalAlloc(GHND,len+0+1);
    LPTSTR buf = (LPTSTR)::GlobalLock(cbhdl);
    GetString(buf, index);
    ::GlobalUnlock(cbhdl);
#if defined(UNICODE)
    cb.SetClipboardData(CF_UNICODETEXT, cbhdl);
#else
    cb.SetClipboardData(CF_TEXT, cbhdl);
#endif
  }
}

//
//
//
void
TListView::CmEditPaste()
{
  int index = GetSelIndex();
  if (index < 0)
    index = 0;

  TClipboard cb(*this, false);
  if (!cb)
    return;   // clipboard open by another program

#if defined(UNICODE)
  HANDLE cbhdl = cb.GetClipboardData(CF_UNICODETEXT);
#else
  HANDLE cbhdl = cb.GetClipboardData(CF_TEXT);
#endif
  if (cbhdl) {
    LPTSTR text = (LPTSTR)::GlobalLock(cbhdl);
    InsertString(text, index);
    SetSelIndex(index+1);
    DirtyFlag = true;
    ::GlobalUnlock(cbhdl);
  }
}

//
//
//
void
TListView::CmEditDelete()
{
  int count = GetCount();
  int index = GetSelIndex();
  if (count <= 1 || index >= count-1)
    return;

  DeleteString(index);
  SetSelIndex(index);
  DirtyFlag = true;
}

//
//
//
void
TListView::CmEditClear()
{
  int count = GetCount();
  if (count == 1)
    return;
  if (count) {
    ClearList();
    DirtyFlag = true;
    SetHorizontalExtent(MaxWidth = 0);
  }
  long style = GetWindowLong(GWL_STYLE);
  if (!(style & LBS_SORT))
    TListBox::AddString(VirtualLastLineStr);
}

static int linePrompt(TWindow* parent, int index, UINT id,
                      LPTSTR buf, int buflen)
{
  TCHAR msg[41];
  _stprintf(msg, parent->GetModule()->LoadString(IDS_LISTNUM).c_str(), index);
  return TInputDialog(parent, msg,
                      parent->GetModule()->LoadString(id).c_str(),
                      buf, buflen).Execute();
}

//
//
//
void
TListView::CmEditAdd()
{
  TCHAR inputText[101];
  *inputText = 0;

  int index = GetSelIndex();
  if (index < 0)
    index = 0;

  if (linePrompt(this,index+1,CM_EDITADD,inputText,COUNTOF(inputText)) == IDOK) {
    InsertString(inputText, index);
    SetSelIndex(index+1);
    SetExtent(inputText);
    DirtyFlag = true;
  }
}

//
//
//
void
TListView::CmEditItem()
{
  int index = GetSelIndex();
  int count = GetCount();

  if (index == count-1) {
    CmEditAdd();
    return;
  }

  if (index < 0 || index >= count-1)
    return;

  TCHAR inputText[101];
  GetSelString(inputText, COUNTOF(inputText)-1);

  if (linePrompt(this,index+1,CM_EDITEDIT,inputText,COUNTOF(inputText))==IDOK) {
    DeleteString(index);
    InsertString(inputText, index);
    SetExtent(inputText);
    SetSelIndex(index);
    DirtyFlag = true;
  }
}

//
// To prevent interpreting as unprocessed accelerator
//
void
TListView::CmSelChange()
{
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE2(TListView, TListBox, TView);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TListView::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TListBox*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  is >> GetObject()->Origin;
  return GetObject();
}

//
//
//
void
TListView::Streamer::Write(opstream &os) const
{
  WriteBaseObject((TListBox*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
  os << GetObject()->Origin;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

