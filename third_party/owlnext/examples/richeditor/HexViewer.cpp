#include <owl/pch.h>

#if !defined(UNICODE)

#include "HexViewer.h"


THexViewer::THexViewer(owl::TDocument& doc, owl::TWindow* parent) 
  : THexEditView(doc, parent)
{
}

THexViewer::~THexViewer()
{
}

bool THexViewer::Create()
{
  if (Doc->GetDocPath() == 0) {
    buffer = new THexFileBuffer(); // new file, no data to display
    return THexEditView::Create();
  }

  THexDocument* hexDoc = dynamic_cast<THexDocument*>(Doc);
  if (hexDoc != 0)
  {
    if (hexDoc->Open(0, Doc->GetDocPath()))
    {
      buffer = &hexDoc->GetHexBuffer();
    }
    else
    {
      DWORD err = ::GetLastError();
    }
  }
  else
  {
    buffer = new THexFileBuffer();
  }

  return THexEditView::Create();
}

#endif