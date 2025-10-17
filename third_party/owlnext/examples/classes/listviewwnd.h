#pragma once
#include <owl/listviewctrl.h>
#include <owl/tooltip.h>

#include "resource.h"

class TListViewWindow
  : public owl::TWindow
{
public:

  TListViewWindow(owl::TWindow* parent);
  ~TListViewWindow();

protected:

  //
  // TWindow overrides
  //

  virtual void SetupWindow(); // override

private:

  enum
  {
    ListId = 42,
    ListStyle = LVS_REPORT | LVS_SHOWSELALWAYS,
    ListStyleEx = /*LVS_EX_CHECKBOXES | */LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP
  };

  owl::TListViewCtrl List;

  owl::TIPtrArray<owl::TToolInfo*> headerTooltips;

  owl::TColor oddLineColor, evenLineColor;

  void EvSetFocus(HWND lostFocus);
  void EvSize(owl::uint sizeType, const owl::TSize& size);
  int NmCustomDraw(owl::TNmCustomDraw& nm);
  void LvnGetInfoTip(owl::TLvGetInfoTip&);
  owl::TResult EvNotify(owl::uint id, owl::TNotify & notifyInfo);

  void UpdateTooltips();

  DECLARE_RESPONSE_TABLE(TListViewWindow);
};

