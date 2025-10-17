#ifndef APP_H
#define APP_H

#include "Replacer.h"
#include <owl/applicat.h>

class TReplaceApp 
  : public owl::TApplication
{
public:

  TReplaceApp();
  ~TReplaceApp();

protected:

  virtual void InitMainWindow(); // override

private:

  TReplaceData Arguments;

  void CmReplace();

  DECLARE_RESPONSE_TABLE(TReplaceApp);
};

#endif
