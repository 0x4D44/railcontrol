// Based on https://www.codeproject.com/articles/177603/win-editable-treeview-and-listview-merged-as-one
// Not all functionality is finished yet

#pragma once

#include <owl/layoutwi.h>
#include <owl/treeviewctrl.h>
#include <owl/colmnhdr.h>

class TTreeList : public owl::TLayoutWindow
{
public:
  explicit TTreeList(owl::TWindow* parent, LPCTSTR title = 0, owl::TModule* module = 0);
  explicit TTreeList(owl::TWindow* parent, const owl::tstring& title, owl::TModule* module = 0);
  ~TTreeList();

protected:
  void SetupWindow();

  int NmCustomDraw(owl::TNmCustomDraw& nm);

  void NmReleasedCapture();

private:
  void Init();

  int ItemPostPaint(owl::TNmCustomDraw& nm);

  owl::TTreeViewCtrl* tree;
  owl::TColumnHeader* header;

  DECLARE_RESPONSE_TABLE(TTreeList);
};

