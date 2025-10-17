#pragma once
#include <coolprj/cooledit.h>
#include <owl/configfl.h>

class TCoolDemoEdit
  : public TCoolEditFile
{
public:

  TCoolDemoEdit(
    owl::TWindow* parent = nullptr,
    int id = 0,
    LPCTSTR text = nullptr,
    int x = 0, int y = 0, int w = 0, int h = 0,
    LPCTSTR fileName = nullptr,
    owl::TModule* module = nullptr) : TCoolEditFile{ parent, id, text, x, y, w, h, fileName, module },
    TextBuffer{ TCoolTextBuffer::clStyleDos },
    ConfigFile{ _T("cooldemo.ini") }
  {
    ModifyExStyle(0, WS_EX_CLIENTEDGE);
  }


  auto GetBuffer() -> TCoolTextBuffer* override
  {
    return &TextBuffer;
  }

protected:
  void SetupWindow() override;

  auto CanClose() -> bool override;

  void CmSelectAll()
  {
    SelectAll();
  }

  void CmSyntaxHighlighting();

    void CmEditClearBookmarks();
    void CmEditToggleBookmark();
    void CmEditGotoNextBookmark();
    void CmEditGotoPrevBookmark();
    void CmEditClearAllBookmarks();
    void CmEditReadonly();
    void CeEditReadonly(owl::TCommandEnabler& tce);
    void CmEditGotoRandBookmark(owl::uint id);
    void CmEditToggleRandBookmark(owl::uint id);
    void CmEditViewselection();
    void CeEditViewselection(owl::TCommandEnabler& tce);
    void CmEditSmartcursor();
    void CeEditSmartcursor(owl::TCommandEnabler& tce);
    void CmEditWhitespace();
    void CeEditWhitespace(owl::TCommandEnabler& tce);
    void CmEditViewcaret();
    void CeEditViewcaret(owl::TCommandEnabler& tce);
    void CeEditSelstream(owl::TCommandEnabler& tce);
    void CmEditSelline();
    void CeEditSelline(owl::TCommandEnabler& tce);
    void CmEditSelcolumn();
    void CeEditSelcolumn(owl::TCommandEnabler& tce);
    void CeLineindicator(owl::TCommandEnabler& tce);
    void CeDirtyIndicator(owl::TCommandEnabler& tce);
    void CeInsertIndicator(owl::TCommandEnabler& tce);
    void CeEditUndo(owl::TCommandEnabler& tce);
    void CeEditRedo(owl::TCommandEnabler& tce);
    void CmSearchSerbreakpoint();
    void CmSearchSetexecpoint();
    void CmSearchSeterror();
    void CmSearchInvalidbp();
    void CmEditViewlinenum();
    void CeEditViewlinenum(owl::TCommandEnabler& tce);

private:
  owl::TIniConfigFile ConfigFile;

  TCoolFileBuffer TextBuffer;

  DECLARE_RESPONSE_TABLE(TCoolDemoEdit);
};
