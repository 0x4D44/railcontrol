//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Default implementation of DLlEntryPoint()/LibMain() for user DLLs
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif

__OWL_USING_NAMESPACE

// link owl libraries
//#include <owl/private/owllink.h>

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlMain);

// MSC code in module.cpp
#if !defined(_BUILDOWLDLL) && !defined(_OWLDLL)
#if defined(BI_COMP_BORLANDC)
// NOTE: We use priority 31 to come just before/after ctr/dtr of global
//       objects (which are assigned a priorority of 32)
//
long TlsAddRefs();
long TlsRelease();

static void __initOWL()
{
  TlsAddRefs();
}
#pragma startup __initOWL 31

//
static void __termOWL()
{
  TlsRelease();
}
#pragma exit __termOWL 31
#endif // BI_COMP_BORLANDC
#endif // if !_BUILDOWLDLL

#if defined(BI_COMP_MSC)
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID)
#else
int WINAPI DllEntryPoint(HINSTANCE hInstance, uint32 reason, LPVOID)
#endif
{
  TRACEX(OwlMain, 0, "DllEntryPoint(" << hInstance << ", " << reason << ", " <<
         ") called");

  switch (reason) {
    case DLL_PROCESS_ATTACH:

      // If you provide your own version of DLL EntryPoint, make sure
      // you call OwlInitUserDLL to allow OWL to initialize '::Module'
      //
      return OWLInitUserDLL(hInstance, _T("")) ? 1 : 0;
  }
  return 1;
}

