//
// This test program exercises the exception transport mechanism in OWLNext.
//
// 32-bit OWL 5 and OWLNext pre-6.33 relied on Windows Structured Exception Handling (SEH) to 
// transport unhandled exceptions across the Window API boundary between handler and message loop.
// This was non-standard, compiler-dependent and proved to be unreliable (see e.g. [bugs:#230]).
// Since 6.33, unhandled exceptions in event handlers are now suspended and rethrown in the message 
// loop, similar to the 16-bit implementation of OWL 5.
//
// See TWindow::ReceiveMessage, TApplication::SuspendThrow, ResumeThrow and MessageLoop.
//
// Copyright (c) 2012 Vidar Hasfjord
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).
//

#include "pch.h"
#include "exceptiontransport.h"

#include "resource.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TExceptionalBase, TWindow)
EV_COMMAND(IDC_TXBASE, CmTXBase),
EV_COMMAND(IDC_STDRUNTIMEERROR, CmStdRuntimeError),
EV_COMMAND(IDC_INTEXCEPTION, CmIntException),
EV_COMMAND(IDC_DIAGEXCEPTION, CmDiagException),
EV_COMMAND(IDC_ENDSESSION, CmEndSession),
END_RESPONSE_TABLE;

DEFINE_RESPONSE_TABLE2(TExceptionalDialog, TDialog, TExceptionalBase)
END_RESPONSE_TABLE;

TExceptionalDialog::TExceptionalDialog(TWindow* parent)
  : TExceptionalBase(parent),
  TDialog(parent, IDD_EXCEPTIONAL_DIALOG)
{}


DEFINE_RESPONSE_TABLE2(TExceptionalPage, TPropertyPage, TExceptionalBase)
END_RESPONSE_TABLE;

TExceptionalPage::TExceptionalPage(TPropertySheet* parent)
  : TExceptionalBase(parent),
  TPropertyPage(parent, IDD_EXCEPTIONAL_PAGE, _T("Exceptional page"))
{}


DEFINE_RESPONSE_TABLE2(TExceptionalClient, TDialog, TExceptionalBase)
EV_COMMAND(IDC_DIALOG, CmDialog),
EV_COMMAND(IDC_PROPERTYSHEET, CmPropertySheet),
END_RESPONSE_TABLE;

TExceptionalClient::TExceptionalClient(TWindow* parent)
  : TExceptionalBase(parent),
  TDialog(parent, IDD_EXCEPTIONAL_CLIENT)
{}

void TExceptionalClient::SetupWindow() // override
{
  TDialog::SetupWindow();
  CheckDlgButton(IDC_MODAL, BST_CHECKED);
}

void TExceptionalClient::CmDialog()
{
  bool shouldOpenModally = IsDlgButtonChecked(IDC_MODAL) == BST_CHECKED;
  if (shouldOpenModally)
  {
    TExceptionalDialog d(this);
    d.Execute();
  }
  else
  {
    TExceptionalDialog* d = new TExceptionalDialog(this);
    d->Create();
    d->ShowWindow(SW_SHOW);
  }
}

void TExceptionalClient::CmPropertySheet()
{
  tstring title = _T("Exceptional property sheet");
  bool shouldOpenModally = IsDlgButtonChecked(IDC_MODAL) == BST_CHECKED;
  if (shouldOpenModally)
  {
    TPropertySheet s(this, title);
    TExceptionalPage p(&s);
    s.Run(true);
  }
  else
  {
    TPropertySheet* s = new TPropertySheet(this, title);
    new TExceptionalPage(s);
    s->Run(false);
  }
}
