#ifndef ABOUT_H
#define ABOUT_H

#include <owl/dialog.h>

class TAboutDlg 
  : public owl::TDialog
{
public:

  TAboutDlg(owl::TWindow* parent, owl::TModule* module = 0);

protected:

  virtual void SetupWindow() override;

};

#endif
