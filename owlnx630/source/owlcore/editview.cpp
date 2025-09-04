//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TEditView
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_EDITVIEW_H)
# include <owl/editview.h>
#endif
#if !defined(OWL_DOCVIEW_RH)
# include <owl/docview.rh>
#endif
#if !defined(OWL_EDITVIEW_RH)
# include <owl/editview.rh>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlDocView);        // General Doc/View diagnostic group

#if !defined(SECTION) || SECTION == 1

//
// TEditView response table
//
DEFINE_RESPONSE_TABLE1(TEditView, TEditSearch)
  EV_VN_DOCCLOSED,
  EV_VN_ISWINDOW,
  EV_VN_ISDIRTY,
  EV_VN_COMMIT,
  EV_VN_REVERT,
  EV_WM_NCDESTROY,
END_RESPONSE_TABLE;

//
//
//
TEditView::TEditView(TDocument& doc, TWindow* parent)
:
  TEditSearch(parent, GetNextViewId(), (LPCTSTR)0),
  TView(doc),
  Origin(0)
{
  Attr.AccelTable = IDA_EDITVIEW;
  if (::FindResource(*GetModule(), TResId(IDM_EDITVIEW), RT_MENU))
    SetViewMenu(new TMenuDescr(IDM_EDITVIEW, 0,2,0,0,0,1, GetModule()));
}

//
//
//
void
TEditView::EvNCDestroy()
{
  TEditSearch::EvNCDestroy();// call TWindow::EvNCDestroy, this may be deleted
}

//
//
//
TEditView::~TEditView()
{
}

//
// Does a given HWND belong to this view? Yes if it is us, or a child of us
//
bool
TEditView::VnIsWindow(HWND hWnd)
{
  return hWnd == TWindow::GetHandle() || IsChild(hWnd);
}

//
//
//
bool
TEditView::VnDocClosed(int omode)
{
  if (VnIsDirty() || !(omode & ofWrite))  // make sure someone else's write
    return false;

  int top = GetFirstVisibleLine();
  uint selbeg;
  uint selend;
  TEdit::GetSelection(selbeg, selend);
  TEdit::Clear();
  LoadData();
  Scroll(0, top);
  TEdit::SetSelection(selbeg, selend);

  return true;
}

//
//
//
bool
TEditView::LoadData()
{
  _tistream* inStream;
  if ((inStream = Doc->InStream(ios::in | ios::binary)) == 0) {
    Doc->PostError(IDS_UNABLEOPEN, MB_OK);
    return false;
  }
  inStream->seekg(0L, ios::end);
  unsigned long total = inStream->tellg();
  inStream->seekg(0L, ios::beg);

  // Buffer Limit
  uint MaxEditBuf = 32000;
  if (TSystem::IsNT()) {
    MaxEditBuf = 2000000;
  }

  uint count = (total > MaxEditBuf) ? MaxEditBuf : (uint)total;
  LPTSTR buf = LockBuffer(count + 1);
  if (!buf) {
    delete inStream;
//    TXOutOfMemory::Raise();
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    return false;
  }

  uint len;
  inStream->read(buf, len = count);

  bool status = (inStream->gcount() == (int)len);
  buf[count] = 0;    // 0 terminate buffer
  UnlockBuffer(buf, true);
  delete inStream;   // close file in case process switch
  if (!status)
    Doc->PostError(IDS_READERROR, MB_OK);

  return status;
}

//
//
//
bool
TEditView::Create()
{
  TRY {
    TEditSearch::Create();   // throws exception TWindow::TXWindow
  }
  CATCH( (TXOwl&) {
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    NotOK();
    return true;   // cannot return false - throws another exception
  })
  if (Doc->GetDocPath() == 0) {
    return true;           // new file, no data to display
  }
  if (!LoadData())
    NotOK();
  return true;
}

//
//
//
void
TEditView::PerformCreate(int menuOrId)
{
  TEdit::PerformCreate(menuOrId);
}

//
//
//
bool
TEditView::VnCommit(bool force)
{
  if (!force && !(VnIsDirty()))
    return true;

  _tostream* outStream;
  if ((outStream = Doc->OutStream(ios::out | ios::binary)) == 0) {
    Doc->PostError(IDS_UNABLEOPEN, MB_OK);
    return false;
  }
  outStream->seekp(Origin);

  bool status = false;
  LPTSTR buf = LockBuffer();
  if (buf) {
    uint count = ::_tcslen(buf);
    outStream->write(buf, count);
    status = ToBool(outStream->good());
    UnlockBuffer(buf);
    ClearModify();   // reset edit control
  }
  delete outStream;
  if (!status)
    Doc->PostError(IDS_WRITEERROR, MB_OK);

  return status;
}

//
//
//
bool
TEditView::VnRevert(bool clear)
{
  TEdit::Clear();
  ClearModify();   // reset edit control
  return clear ? true : LoadData();
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE2(TEditView, TEditSearch, TView);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TEditView::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TEditSearch*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  is >> GetObject()->Origin;
  return GetObject();
}

//
//
//
void
TEditView::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TEditSearch*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
  os << GetObject()->Origin;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE

