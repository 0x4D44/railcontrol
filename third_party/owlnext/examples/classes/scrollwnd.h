#pragma once
#include <owl\window.h>

class TScrollWindow : public owl::TWindow 
{
public:
  TScrollWindow(owl::TWindow* parent = 0, LPCTSTR title = 0);

  void SetupWindow();
  void Paint(owl::TDC& dc, bool, owl::TRect&);
};
