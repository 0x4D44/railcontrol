//
// OWLNext WM_COPYDATA Demo
// This code demonstrates sending and receiving the WM_COPYDATA message.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#pragma once
#include <owl\listbox.h>

class TCopyDataWindow :
  public owl::TListBox
{
public:
  TCopyDataWindow(owl::TWindow* parent);
  virtual ~TCopyDataWindow();

protected:
  void SetupWindow();

  void CmCopyData();

#if defined(OWL5_COMPAT)

    void EvCopyData(HWND sender, PCOPYDATASTRUCT p)
  {
    EvCopyData(sender, *p);
  }

#endif

    bool EvCopyData(HWND sender, const COPYDATASTRUCT& c);

DECLARE_RESPONSE_TABLE(TCopyDataWindow);
};

