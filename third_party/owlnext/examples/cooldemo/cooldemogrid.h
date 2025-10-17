#pragma once
#include <coolprj/coolgrid.h>


class TCoolDemoGrid
  : public TCoolGrid
{
public:

  TCoolDemoGrid(
    owl::TWindow* parent = nullptr,
    int id = 0,
    LPCTSTR text = nullptr,
    int x = 0, int y = 0, int w = 0, int h = 0,
    owl::TModule* module = nullptr);


protected:
  void SetupWindow() override
  {
    TCoolGrid::SetupWindow();
  }

  auto CanClose() -> bool override
  {
    return true;
  }

private:

  DECLARE_RESPONSE_TABLE(TCoolDemoGrid);
};


