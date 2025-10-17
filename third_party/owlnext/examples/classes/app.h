#pragma once

#include <owl/applicat.h>
#include <owl/gdiobjec.h>
#include <vector>

class TClassesApp : public owl::TApplication
{
public:
  TClassesApp();
  ~TClassesApp();

protected:
  void InitMainWindow() override; // TApplication
  void InitInstance() override; // TApplication

private:
  std::vector<owl::TBitmap> MenuBitmaps;

  void AssignMenuBitmaps();
};

