#pragma once
#include <owl/layoutwi.h>

class TLayoutDialog;

struct TChildInfo {
  owl::TWindow*       Child;
  owl::TLayoutMetrics LayoutMetrics;
};

const int MaxChildren = 5;

class TLayoutDemoWindow : public owl::TLayoutWindow
{
public:
  TLayoutDemoWindow(owl::TWindow* parent);


protected:
  void SetupWindow();

private:
  void CmLayout();
  void CmReLayout();

private:
  TChildInfo  ChildInfo[MaxChildren + 1];

  TLayoutDialog* LayoutDialog;

  DECLARE_RESPONSE_TABLE(TLayoutDemoWindow);

};

