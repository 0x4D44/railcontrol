#pragma once

#if !defined(UNICODE)

#include <coolprj\hexdocument.h>
#include <coolprj\hexedit.h>

class THexViewer : public THexEditView
{
public:
  THexViewer(owl::TDocument& doc, owl::TWindow* parent = 0);

  ~THexViewer();

  THexBuffer* GetBuffer()
  {
    return buffer;
  }

  bool Create();

private:
  THexBuffer* buffer;
};

#endif