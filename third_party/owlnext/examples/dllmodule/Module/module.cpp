//
// Module Sample; module implementation
// This file implements a sample DLL that relies on the OWLNext framework.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#include "module.h"
#include <memory>

#include "module.rh"

using namespace owl;

namespace
{
  auto ModuleInstance = static_cast<TSampleModule*>(nullptr);
}

auto TSampleModule::GetInstance() -> TSampleModule&
{
  PRECONDITION(ModuleInstance);
  return *ModuleInstance;
}

TSampleModule::TDialog1::TDialog1(TWindow* parent)
  : TDialog(parent, IDD_SAMPLE, ModuleInstance)
{}

void TSampleModule::TDialog1::SetupWindow()
{
  TDialog::SetupWindow();

#if 1

  // Load a string from this module.
  //
  auto s = GetModule()->LoadString(IDS_MODULE);
  SetDlgItemText(IDC_STATIC1, s);

#elif 0 // Quicker way to do the same thing:

  // Load a string from this module (using the TWindow::LoadString shorthand).
  //
  auto s = LoadString(IDS_MODULE);
  SetDlgItemText(IDC_STATIC1, s);

#elif 0 // Even quicker:

  // Load a string from this module (using the SetDlgItemText overload).
  //
  SetDlgItemText(IDC_STATIC1, IDS_MODULE);

#endif

}

TSampleModule::TDialog2::TDialog2(TWindow* parent)
  : TDialog1(parent)
{}

TSampleModule::TDialog2::~TDialog2()
{}

auto TSampleModule::CreateDialog2(TWindow* parent) -> TSampleModule::TDialog2*
{return new TDialog2(parent);}

void TSampleModule::DeleteDialog2(TDialog2* p)
{delete p;}

//
// OWL maintains an internal list of modules used to search for resources. Whether the module is
// added to the list is controlled by the third parameter of the TModule constructor.
// Here we choose not to be part of the list, since we do not want client code to inadvertently
// look up a resource in our module. We will control resource loading from this module explicitly.
//
TSampleModule::TSampleModule(THandle handle)
  : TModule(_T("Sample"), handle, false) // addToList = false
{}

TSampleModule::~TSampleModule()
{}

//
// Implementation of the C interface to the module
// No exceptions can be allowed to escape these functions, so we need full exception handling.
//
extern "C" 
{

  SampleModule_TDialogHandle SampleModule_CreateDialog(HWND parent)
  {
    try
    {
      auto m = &TSampleModule::GetInstance();
      auto parentAlias = std::make_unique<TWindow>(parent, m);
      auto w = new TSampleModule::TDialog1(parentAlias.get());
      parentAlias.release(); // Will be freed in SampleModule_DeleteDialog.
      return static_cast<SampleModule_TDialogHandle>(static_cast<void*>(w));
    }
    catch (...)
    {
      return nullptr;
    }
  }

  int SampleModule_ExecuteDialog(SampleModule_TDialogHandle h)
  {
    try
    {
      auto w = static_cast<TSampleModule::TDialog2*>(static_cast<void*>(h));
      return w->Execute();
    }
    catch (...)
    {
      return -1;
    }
  }

  void SampleModule_DeleteDialog(SampleModule_TDialogHandle h)
  {
    try
    {
      auto w = static_cast<TSampleModule::TDialog1*>(static_cast<void*>(h));
      auto parentAlias = std::unique_ptr<TWindow>(w->GetParent()); // Take ownership.
      delete w;
    }
    catch (...)
    {}
  }

}

//
// DLL entry point; manages the lifetime of the module singleton.
//
auto WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID) -> BOOL
{
  try
  {
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
      ModuleInstance = new TSampleModule(handle);
      break;

    case DLL_PROCESS_DETACH:
      delete ModuleInstance;
      ModuleInstance = nullptr;
      break;
    }
    return TRUE;
  }
  catch (...)
  {
    return FALSE;
  }
}
