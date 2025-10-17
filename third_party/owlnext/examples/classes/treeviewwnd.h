#pragma once
#include <owl/treeviewctrl.h>
#include <owl/imagelst.h>

#include "resource.h"

class TTreeViewWindow
  : public owl::TWindow
{
public:

  TTreeViewWindow(owl::TWindow* parent);

protected:

  //
  // TWindow overrides
  //

  virtual void SetupWindow(); // override

private:

  enum
  {
    TreeId = 42,
    TreeStyle = TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT
  };

  owl::TTreeViewCtrl Tree;
  owl::TImageList Icons;

  //
  // Event handlers
  //

  void CmSetState();
  void CmGetState();
  void CmSortDescending();

  void EvSetFocus(HWND lostFocus);
  void EvSize(owl::uint sizeType, const owl::TSize& size);

  bool TvnItemExpanding(owl::TTvNotify& i);

  DECLARE_RESPONSE_TABLE(TTreeViewWindow);
};

