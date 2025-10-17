#pragma once
#include <owl/decmdifr.h>

class TClassesMDIFrame : public owl::TDecoratedMDIFrame
{
public:
  TClassesMDIFrame(owl::TWindow* parent, LPCTSTR title);
  virtual ~TClassesMDIFrame();

  DECLARE_RESPONSE_TABLE(TClassesMDIFrame);
};

