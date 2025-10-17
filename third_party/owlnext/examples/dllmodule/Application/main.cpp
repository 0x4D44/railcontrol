//
// Module Sample; application
// This file demonstrates use of a sample DLL that relies on the OWLNext framework.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#include "../Module/module.h"
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <memory>
#include "main.rh"
#include "../Module/module.rh"

using namespace owl;

class TMainDialog
  : public TDialog
{
public:

  TMainDialog::TMainDialog()
    : TDialog(nullptr, IDD_MAIN)
  {}

protected:

  virtual void SetupWindow()
  {
    // Load a string from the application module.
    //
    auto a = GetModule()->LoadString(IDS_MAIN);
    SetDlgItemText(IDC_MAIN, a);

    // Load a string from the sample DLL module.
    //
    auto s = TSampleModule::GetInstance().LoadString(IDS_MODULE);
    SetDlgItemText(IDC_MODULE, s);
  }

  //
  // Demonstrate various use cases for the sample module dialogs.
  //
  void BnButtonClicked()
  {
    static auto c = 0;
    switch (c++)
    {
      case 0: // Use TSampleModule::TDialog1; simple creation and use.
      {
        try
        {
          TSampleModule::TDialog1(this).Execute();
        }
        catch (...)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
        }
        break;
      }

      case 1: // Use TSampleModule::TDialog2; controlled construction and destruction.
      {
        try
        {
          auto& m = TSampleModule::GetInstance();
          using TDialog2 = TSampleModule::TDialog2;
          auto deleter = [&m](TDialog2* p) { if (p) m.DeleteDialog2(p); };
          using TDialog2Ptr = std::unique_ptr<TDialog2, decltype(deleter)>;
          auto p = TDialog2Ptr{m.CreateDialog2(this), deleter}; CHECK(p);
          p->Execute();
        }
        catch (...)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
        }
        break;
      }

      case 2: // Use the sample module's C interface to TSampleModule::TDialog2.
      {
        SampleModule_TDialogHandle h = SampleModule_CreateDialog(GetHandle());
        if (!h)
        {
          MessageBox(_T("Module dialog creation failed."), _T("Error"), MB_ICONERROR | MB_OK);
          return;
        }
        int r = SampleModule_ExecuteDialog(h);
        if (r == -1)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
          SampleModule_DeleteDialog(h);
          return;
        }
        SampleModule_DeleteDialog(h);
        break;
      }

      default:
      {
        MessageBox(_T("All test cases have been run; starting over."));
        c = 0;
        break;
      }

    } // switch
  }

  DECLARE_RESPONSE_TABLE(TMainDialog);
};

DEFINE_RESPONSE_TABLE1(TMainDialog, TDialog)
  EV_BN_CLICKED(IDC_OPEN_MODULE_DIALOG, BnButtonClicked),
END_RESPONSE_TABLE;

class TMyDlgApp
  : public TApplication
{
public:

  TMyDlgApp()
    : TApplication(_T("DLL Module Sample"))
  {}

protected:

  void InitMainWindow() override
  {
    TWindow* client = new TMainDialog;
    MainWindow = new TFrameWindow(0, GetName(), client, true);
    MainWindow->SetIcon(this, IDI_MAIN);
    MainWindow->ModifyStyle(WS_MAXIMIZEBOX | WS_SIZEBOX, 0);
  }

};

auto OwlMain(int, char* []) -> int
{
  return TMyDlgApp().Run();
}
