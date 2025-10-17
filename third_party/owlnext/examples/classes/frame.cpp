#include "pch.h"

# include <owl/monthcal.h>

#include "frame.h"
#include "client.h"
#include "resource.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TClassesMDIFrame, TDecoratedMDIFrame)
END_RESPONSE_TABLE;

TClassesMDIFrame::TClassesMDIFrame(TWindow* parent, LPCTSTR title)
  : TDecoratedMDIFrame(title, TResId(0), std::make_unique<TClassesMDIClient>()) // Always call the base class constructor.
{
}


TClassesMDIFrame::~TClassesMDIFrame()
{
}

