#include "pch.h"
#pragma hdrstop

#include "cooldemoedit.h"

#include <coolprj/clrpropdlg.h>
#include <coolprj/lang.h>

#include <owl/editfile.rh>
#include <coolprj/cooledit.rh>

#include "resource.h"

DEFINE_RESPONSE_TABLE1(TCoolDemoEdit, TCoolEditFile)
EV_COMMAND(CM_SELECTALL, CmSelectAll),
EV_COMMAND(CM_SYNTAXHIGHLIGHTING, CmSyntaxHighlighting),
EV_COMMAND(CM_EDITCLEAR_BOOKMARKS, CmEditClearBookmarks),
EV_COMMAND(CM_EDITTOGGLE_BOOKMARK, CmEditToggleBookmark),
EV_COMMAND(CM_EDITGOTO_NEXT_BOOKMARK, CmEditGotoNextBookmark),
EV_COMMAND(CM_EDITGOTO_PREV_BOOKMARK, CmEditGotoPrevBookmark),
EV_COMMAND(CM_EDITCLEAR_ALL_BOOKMARKS, CmEditClearAllBookmarks),
EV_COMMAND(CM_EDITREADONLY, CmEditReadonly),
EV_COMMAND_ENABLE(CM_EDITREADONLY, CeEditReadonly),

EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK0, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK1, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK2, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK3, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK4, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK5, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK6, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK7, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK8, CmEditGotoRandBookmark),
EV_COMMAND_AND_ID(CM_EDITGO_BOOKMARK9, CmEditGotoRandBookmark),

EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK0, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK1, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK2, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK3, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK4, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK5, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK6, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK7, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK8, CmEditToggleRandBookmark),
EV_COMMAND_AND_ID(CM_EDITTOGGLE_BOOKMARK9, CmEditToggleRandBookmark),
EV_COMMAND(CM_EDITVIEWSELECTION, CmEditViewselection),
EV_COMMAND_ENABLE(CM_EDITVIEWSELECTION, CeEditViewselection),
EV_COMMAND(CM_EDITSMARTCURSOR, CmEditSmartcursor),
EV_COMMAND_ENABLE(CM_EDITSMARTCURSOR, CeEditSmartcursor),
EV_COMMAND(CM_EDITWHITESPACE, CmEditWhitespace),
EV_COMMAND_ENABLE(CM_EDITWHITESPACE, CeEditWhitespace),
EV_COMMAND(CM_EDITVIEWCARET, CmEditViewcaret),
EV_COMMAND_ENABLE(CM_EDITVIEWCARET, CeEditViewcaret),
EV_COMMAND_ENABLE(CM_EDITSELSTREAM, CeEditSelstream),
EV_COMMAND(CM_EDITSELLINE, CmEditSelline),
EV_COMMAND_ENABLE(CM_EDITSELLINE, CeEditSelline),
EV_COMMAND(CM_EDITSELCOLUMN, CmEditSelcolumn),
EV_COMMAND_ENABLE(CM_EDITSELCOLUMN, CeEditSelcolumn),
EV_COMMAND(CM_EDITVIEWLINENUM, CmEditViewlinenum),
EV_COMMAND_ENABLE(CM_EDITVIEWLINENUM, CeEditViewlinenum),
EV_COMMAND_ENABLE(ID_LINEINDICATOR, CeLineindicator),
EV_COMMAND_ENABLE(ID_DIRTYINDICATOR, CeDirtyIndicator),
EV_COMMAND_ENABLE(ID_INSERTINDICATOR, CeInsertIndicator),
EV_COMMAND_ENABLE(CM_EDITUNDO, CeEditUndo),
EV_COMMAND_ENABLE(CM_EDITREDO, CeEditRedo),
EV_COMMAND(CM_SEARCH_SETBREAKPOINT, CmSearchSerbreakpoint),
EV_COMMAND(CM_SEARCH_SETEXECUTIONPOINT, CmSearchSetexecpoint),
EV_COMMAND(CM_SEARCH_SETERRORPOINT, CmSearchSeterror),
EV_COMMAND(CM_SEARCH_SETINVALIDBREAKPOINT, CmSearchInvalidbp),
END_RESPONSE_TABLE;

using namespace owl;

void TCoolDemoEdit::SetupWindow()
{
  TCoolEditFile::SetupWindow();
  if (GetFileName())
  {
    TextBuffer.RestoreSyntaxDescr(ConfigFile);
    SetSyntaxParser(ChooseSyntaxParser(this, GetFileName()));
  }
  else
  {
    FileNew();
  }
}

auto TCoolDemoEdit::CanClose() -> bool
{
  if (!GetBuffer()->IsDirty())
    return true;

  auto s = tostringstream{};
  const auto f = GetFileName() ? GetFileName() : GetModule()->LoadString(IDS_UNTITLEDFILE);
  s << _T("Save changes to \"") << f << "\"?";
  const auto c = MessageBox(s.str(), GetApplication()->GetName(), MB_YESNOCANCEL);
  switch (c)
  {
  case IDYES:
    return FileSave();

  case IDNO:
    return true;

  case IDCANCEL:
  default:
    return false;
  }
}

void TCoolDemoEdit::CmSyntaxHighlighting()
{
  auto data = TEditPropDlgXfer{ TPSheetFunctor{} };
  auto& config = ConfigFile;
  data.LoadData(&config);

  auto sheet = TPropertySheet{ this, _T("Syntax Highlighting"), 0, false, PSH_NOCONTEXTHELP };
  auto page = TEditPropDlg{ &sheet, data };
  if (sheet.Execute() == IDOK)
  {
    data.SaveData(config);
    TextBuffer.RestoreSyntaxDescr(config);
    Invalidate();
  }
}

void TCoolDemoEdit::CmEditClearBookmarks()
{
  ClearBookmarks();
}
//
void TCoolDemoEdit::CmEditToggleBookmark()
{
  ToggleRandBookmark();
}
//
void TCoolDemoEdit::CmEditGotoNextBookmark()
{
  NextRandBookmark();
}
//
void TCoolDemoEdit::CmEditGotoPrevBookmark()
{
  PrevRandBookmark();
}
//
void TCoolDemoEdit::CmEditClearAllBookmarks()
{
  ClearRandBookmarks();
}
//
void TCoolDemoEdit::CmEditReadonly()
{
  EnableReadOnly(!IsReadOnly());
}
//
void TCoolDemoEdit::CeEditReadonly(TCommandEnabler& tce)
{
  tce.SetCheck(IsReadOnly());
}
//
void TCoolDemoEdit::CmEditGotoRandBookmark(uint id)
{
  int bookmarkId = id - CM_EDITGO_BOOKMARK0;
  GoBookmark(bookmarkId);
}
//
void TCoolDemoEdit::CmEditToggleRandBookmark(uint id)
{
  int bookmarkId = id - CM_EDITTOGGLE_BOOKMARK0;
  ToggleBookmark(bookmarkId);
}
//
//
void TCoolDemoEdit::CmEditViewselection()
{
  EnableShowInactiveSel(IsShowInactiveSel() ? false : true);
}
//
void TCoolDemoEdit::CeEditViewselection(TCommandEnabler& tce)
{
  tce.SetCheck(IsShowInactiveSel());
}
//
void TCoolDemoEdit::CmEditSmartcursor()
{
  EnableSmartCursor(IsSmartCursor() ? false : true);
}
//
void TCoolDemoEdit::CeEditSmartcursor(TCommandEnabler& tce)
{
  tce.SetCheck(IsSmartCursor());
}
//
void TCoolDemoEdit::CmEditWhitespace()
{
  EnableTabs(IsTabsVisible() ? false : true);
}
//
void TCoolDemoEdit::CeEditWhitespace(TCommandEnabler& tce)
{
  tce.SetCheck(IsTabsVisible());
}
//
void TCoolDemoEdit::CmEditViewcaret()
{
  EnableTabs(IsTabsVisible() ? false : true);
}
//
void TCoolDemoEdit::CeEditViewcaret(TCommandEnabler& tce)
{
  tce.SetCheck(IsTabsVisible());
}
//
void TCoolDemoEdit::CmEditViewlinenum()
{
  ShowLineNumbers(IsLineNumbers() ? false : true);
}
//
void TCoolDemoEdit::CeEditViewlinenum(TCommandEnabler& tce)
{
  tce.SetCheck(IsLineNumbers());
}
//
void TCoolDemoEdit::CeEditSelstream(TCommandEnabler& tce)
{
  tce.SetCheck(IsStreamSelMode());
}
//
void TCoolDemoEdit::CmEditSelline()
{
  SetSelMode(stLineSelection);
}
//
void TCoolDemoEdit::CeEditSelline(TCommandEnabler& tce)
{
  tce.SetCheck(IsLineSelMode());
}
//
void TCoolDemoEdit::CmEditSelcolumn()
{
  SetSelMode(stColumnSelection);
}
//
void TCoolDemoEdit::CeEditSelcolumn(TCommandEnabler& tce)
{
  tce.SetCheck(IsColumnSelMode());
}
//
void TCoolDemoEdit::CeLineindicator(TCommandEnabler& tce)
{
  VERIFY_TEXTPOS(CursorPos);
  _TCHAR buffer[40];
  // VVV m_ptCursorPos.x + 1 ???
  wsprintf(buffer, _T("Ln %d, Col %d"), CursorPos.row + 1, CursorPos.col + 1);
  tce.Enable(true);
  tce.SetText(buffer);
}
//
void TCoolDemoEdit::CeDirtyIndicator(TCommandEnabler& tce)
{
  VERIFY_TEXTPOS(CursorPos);
  //tce.Enable(true);
  tce.SetCheck(GetBuffer()->IsDirty() ? 1 : 0);
}
//
void TCoolDemoEdit::CeInsertIndicator(TCommandEnabler& tce)
{
  tce.Enable(IsOverType());
  tce.SetText(IsOverType() ? _T("OVR") : _T("INS"));
}
/////////////////////////////////////////////////////////////////////////////
void TCoolDemoEdit::CeEditUndo(TCommandEnabler& tce)
{
  PRECONDITION(GetBuffer());
  //can also set text
  TCoolTextBuffer* buffer = GetBuffer();
  _TCHAR buff[MAX_PATH];
  bool retval = buffer->GetUndoDescription(buff, MAX_PATH, GetApplication());
  if (!retval)
    _tcscpy_s(buff, MAX_PATH, _T("Can't Undo"));
  _tcscat_s(buff, MAX_PATH, _T("\tAlt+BkSp"));
  tce.SetText(buff);
  tce.Enable(buffer->CanUndo());
}
//
void TCoolDemoEdit::CeEditRedo(TCommandEnabler& tce)
{
  PRECONDITION(GetBuffer());

  //can also set text
  TCoolTextBuffer* buffer = GetBuffer();

  _TCHAR buff[MAX_PATH];
  bool retval = buffer->GetRedoDescription(buff, MAX_PATH, GetApplication());
  if (retval)
    tce.SetText(buff);
  else
    tce.SetText(_T("Can't Redo"));
  tce.Enable(buffer->CanRedo());
}
//
void TCoolDemoEdit::CmSearchSerbreakpoint()
{
  TCoolTextBuffer* buffer = GetBuffer();
  bool is_set = (buffer->GetLineFlags(GetCursorPos().row) & TCoolTextBuffer::lfBreakPoint);
  buffer->SetLineFlag(GetCursorPos().row, TCoolTextBuffer::lfBreakPoint, !is_set, false);
}
//
void TCoolDemoEdit::CmSearchSetexecpoint()
{
  TCoolTextBuffer* buffer = GetBuffer();
  bool is_set = buffer->GetLineFlags(GetCursorPos().row) & TCoolTextBuffer::lfExecution;
  buffer->SetLineFlag(GetCursorPos().row, TCoolTextBuffer::lfExecution, !is_set, false);
}
//
void TCoolDemoEdit::CmSearchSeterror()
{
  TCoolTextBuffer* buffer = GetBuffer();
  bool is_set = buffer->GetLineFlags(GetCursorPos().row) & TCoolTextBuffer::lfCompError;
  buffer->SetLineFlag(GetCursorPos().row, TCoolTextBuffer::lfCompError, !is_set, false);
}
//
void TCoolDemoEdit::CmSearchInvalidbp()
{
  TCoolTextBuffer* buffer = GetBuffer();
  bool is_set = buffer->GetLineFlags(GetCursorPos().row) & TCoolTextBuffer::lfInvalidBPoint;
  buffer->SetLineFlag(GetCursorPos().row, TCoolTextBuffer::lfInvalidBPoint, !is_set, false);
}
//
