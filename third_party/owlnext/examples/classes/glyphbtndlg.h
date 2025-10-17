#pragma once
#include <owl/dialog.h>

class TGlyphButtonDialog :
  public owl::TDialog
{
public:
  TGlyphButtonDialog(TWindow* parent);
  virtual ~TGlyphButtonDialog();

protected:
  void SetupWindow() override;
};

