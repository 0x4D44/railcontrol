#pragma once
#include <owl/edit.h>

class TCpuInfoWindow : public owl::TEdit
{
public:
  TCpuInfoWindow();

protected:
  owl::TFont Font;

  void SetupWindow() override;
};
