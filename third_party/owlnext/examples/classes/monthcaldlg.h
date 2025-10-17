//
// OWLNext TMonthCalendar Example
// This example demonstrates the use of TMonthCalendar in OWLNext.
//
// Copyright (c) 1998 by Bidus Yura, All Rights Reserved
// Copyright (c) 2015 Vidar Hasfjord
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).
//

#pragma once

#include <owl/transfer.h>
#include <owl/monthcal.h>

#include "resource.h"

//
// Calendar dialog
//
class TCalendarDialog
  : public owl::TTransferDialog<owl::TSystemTime>
{
public:

  TCalendarDialog(owl::TWindow* parent, owl::TSystemTime& d);

  void CloseWindow(int retValue) override;

private:

  owl::TMonthCalendar Calendar{ this, IDC_MONTHCAL };

  void DoTransferData(const owl::TTransferInfo& i, owl::TSystemTime& d) override;

  void EvMcnSelect(owl::TNmSelChange&);

  DECLARE_RESPONSE_TABLE(TCalendarDialog);
};