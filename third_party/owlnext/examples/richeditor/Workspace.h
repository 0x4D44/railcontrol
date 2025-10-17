#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <owl\mdi.h>

class TWorkspace
  : public owl::TMDIClient
{
public:

  TWorkspace(owl::TModule* module = 0);

  void OpenFile(LPCTSTR fileName = 0);

protected:

  virtual void SetupWindow(); // override

private:

  void EvDropFiles(owl::TDropInfo);

  DECLARE_RESPONSE_TABLE(TWorkspace);
};

#endif
