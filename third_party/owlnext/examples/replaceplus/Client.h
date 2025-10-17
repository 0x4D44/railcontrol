#ifndef CLIENT_H
#define CLIENT_H

#include "Replacer.h"
#include <owl/transfer.h>

class TReplaceClient 
  : public owl::TTransferDialog<TReplaceData>
{
public:

  TReplaceClient(owl::TWindow* parent, TReplaceData&, const owl::tstring& regKey);

protected:

  virtual void SetupWindow(); // override
  virtual void DoTransferData(const owl::TTransferInfo&, TReplaceData&); // override

private:

  //
  // Event handlers
  //
  void BnBrowseClicked();
  void BnActionClicked();
  void BnReplaceClicked();
  void BnHelpClicked();

  DECLARE_RESPONSE_TABLE(TReplaceClient);
};

#endif
