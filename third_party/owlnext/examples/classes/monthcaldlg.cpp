//
// OWLNext TMonthCalendar Example
// This example demonstrates the use of TMonthCalendar in OWLNext.
//
// Copyright (c) 1998 by Bidus Yura, All Rights Reserved
// Copyright (c) 2015 Vidar Hasfjord
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).
//

#include "pch.h"
#include "monthcaldlg.h"

#include "resource.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TCalendarDialog, TTransferDialog)
EV_MCN_SELECT(IDC_MONTHCAL, EvMcnSelect),
END_RESPONSE_TABLE;

TCalendarDialog::TCalendarDialog(TWindow* parent, TSystemTime& d)
  : TTransferDialog{ parent, IDD_MONTHCALDIALOG, d }
{}

void TCalendarDialog::CloseWindow(int retValue)
{
  if (retValue == IDOK)
    TransferData(tdGetData); // Ensure data is transferred, even for a modeless dialog.
  TTransferDialog::CloseWindow(retValue);
}

void TCalendarDialog::DoTransferData(const TTransferInfo& i, TSystemTime& d)
{
  TransferMonthCalendarData(i, Calendar, d);
}

void TCalendarDialog::EvMcnSelect(TNmSelChange&)
{
  CmOk();
}
