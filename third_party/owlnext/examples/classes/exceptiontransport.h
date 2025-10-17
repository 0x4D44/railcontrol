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

#pragma once

#include <owl/dialog.h>
#include <owl/propsht.h>


class TExceptionalBase
  : public virtual owl::TWindow
{
public:

  TExceptionalBase(owl::TWindow* parent)
    : owl::TWindow(parent)
  {}

protected:

  void CmTXBase() { throw owl::TXBase(_T("TXBase")); }
  void CmStdRuntimeError() { throw std::runtime_error("std::runtime_error"); }
  void CmIntException() { throw 42; }
  void CmDiagException() { PRECONDITIONX(false, _T("Assertion violation!")); }

  //
  // Aborts the program by posting a WM_ENDSESSION message.
  //
  // Before OWLNext 6.32.5, TWindow::EvEndSession would generate a TXEndSession exception. Now 
  // TWindow::EvEndSession does nothing (only logs a message that the message was not handled), and 
  // the WM_ENDSESSION message has special handling in the message loop, causing the TXEndSession 
  // exception to be thrown there (thus needing no exception transport). See [bugs:#231].
  //
  void CmEndSession() { PostMessage(WM_ENDSESSION, TRUE); }

  DECLARE_RESPONSE_TABLE(TExceptionalBase);
};

class TExceptionalDialog
  : public TExceptionalBase,
  public owl::TDialog
{
public:

  TExceptionalDialog(owl::TWindow* parent);

  DECLARE_RESPONSE_TABLE(TExceptionalDialog);
};

class TExceptionalPage
  : public TExceptionalBase,
  public owl::TPropertyPage
{
public:

  TExceptionalPage(owl::TPropertySheet* parent);
 
  DECLARE_RESPONSE_TABLE(TExceptionalPage);
};

class TExceptionalClient
  : public TExceptionalBase,
  public owl::TDialog
{
public:

  TExceptionalClient(owl::TWindow* parent);

protected:

  virtual void SetupWindow(); // override

  void CmDialog();

  void CmPropertySheet();

  DECLARE_RESPONSE_TABLE(TExceptionalClient);
};
