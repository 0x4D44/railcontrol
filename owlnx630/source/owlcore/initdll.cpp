//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2008-05-13 09:56:34 $
//
// Default OwlMain to satisfy LibMain in DLLs that aren't components
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
 __OWL_USING_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlMain);

#ifdef BI_NAMESPACE
  using OWL::TModule;
  using OWL::TApplication;
#endif
// Routine which initializes an OWL User DLL..
// NOTE: Must be explicitly invoked if your DLL provides it's own
//       LibMain, DllEntryPoint (or DllMain).
//
bool
OWLInitUserDLL(HINSTANCE hInstance, LPTSTR cmdLine)
{
  // If no global ::Module ptr exists yet, provide a default now
  //
  if (!gModule) { 
    _TCHAR buffer[MAX_PATH];
    GetModuleFileName(hInstance, buffer, MAX_PATH);
    static TModule module(buffer, hInstance);
    gModule = &module;
  }

  // Store away information about this instance
  //
  TApplication::SetWinMainParams(hInstance, 0, 
                cmdLine?cmdLine:_T(""), SW_SHOWNORMAL);

  TRY {
    int retVal = OwlMain(0, 0);
    TRACEX(OwlMain, 0, _T("DllEntryPoint() returns ") << retVal);
    return retVal == 0;
  }
  CATCH( (XBASE& x) {return NS_OWL::HandleGlobalException(x, 0) == 0;} )
}

/* ========================================================================== */

