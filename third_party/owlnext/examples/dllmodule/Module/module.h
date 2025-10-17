//
// Module Sample; module header
// This file defines the interface of a sample DLL that relies on the OWLNext framework.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#ifndef MODULE_H
#define MODULE_H

//
// To build the module as a DLL, define MODULE_BUILD_DLL.
// To use the DLL from client code, define MODULE_USE_DLL.
// For static linking, neither should be defined.
//
#if defined(MODULE_BUILD_DLL)
# define MODULE_API __declspec(dllexport)
#elif defined (MODULE_USE_DLL)
# define MODULE_API __declspec(dllimport)
#else
# define MODULE_API
#endif

#include <owl/module.h>
#include <owl/dialog.h>

//
// OWL encapsulation of the module
// The module is implemented as a singleton, which is initialized in DllMain.
//
class MODULE_API TSampleModule
  : public owl::TModule
{
public:

  static auto GetInstance() -> TSampleModule&;

  // 
  // This is an example of a dialog that has its resources stored within the module.
  // The constructor sets the internal Module pointer to point to the TSampleModule instance.
  // 
  class MODULE_API TDialog1
    : public owl::TDialog
  {
  public:

    TDialog1(TWindow* parent);

  protected:

    void SetupWindow() override;

  };

  //
  // This is an example of a class that has object construction and destruction controlled by the
  // module, e.g. for heap control. The constructor and destructor is made private, and the client
  // has to use dedicated functions exported by the module to create and delete objects.
  //
  class MODULE_API TDialog2
    : public TDialog1
  {
  private:

    TDialog2(TWindow* parent);
    ~TDialog2() override;

    friend class TSampleModule;

  };

  auto CreateDialog2(owl::TWindow* parent) -> TDialog2*;
  void DeleteDialog2(TDialog2*);

private:

  TSampleModule(THandle);
  ~TSampleModule() override;

  friend auto WINAPI DllMain(HINSTANCE, DWORD, LPVOID) -> BOOL;

};

//
// This section demonstrates a C-compatible interface to the TSampleModule::TDialog1 class.
// By using a C API for your module you can fully isolate the use of OWLNext in the implementation
// of the module and allow any application, using OWLNext or not, to use it without conflicts.
//
// In a real scenario this section would be put in its own header file that only would include 
// "windows.h" for the definition of HWND, thus achieving a complete decoupling of the API from the
// implementation dependency on OWLNext.
//
// We use the "SampleModule_" prefix here on all names to avoid name clashes, since C does not
// support namespaces.
//
extern "C" 
{
  struct SampleModule_TDialog;
  typedef SampleModule_TDialog* SampleModule_TDialogHandle;

  MODULE_API SampleModule_TDialogHandle SampleModule_CreateDialog(HWND);
  MODULE_API int SampleModule_ExecuteDialog(SampleModule_TDialogHandle);
  MODULE_API void SampleModule_DeleteDialog(SampleModule_TDialogHandle);
}

#endif
