//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of WinMain for user exes
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
# include <owl/lclstrng.h>
#endif
#if !defined(MAX_PATH)
#  include <dos.h>
#endif

// link owl libraries
//#include <owl/private/owllink.h>


__OWL_USING_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlMain);

//
// Default WinMain calls OwlMain, after setting params into TApplication
//
int 
_tmain(int argc, _TCHAR* argv[])
{
  TRACEX(OwlMain, 0, _T("main() called"));

  HINSTANCE hInstance = ::GetModuleHandle(NULL);
  // If no global ::Module ptr exists yet, provide a default now
  //
  if (!gModule) {
    _TCHAR buffer[MAX_PATH];
    GetModuleFileName(hInstance, buffer,MAX_PATH);
    static NS_OWL::TModule module(buffer, hInstance);
    gModule = &module;
  }

  LPCTSTR cmdLine = GetCommandLine();
  NS_OWL::TApplication::SetWinMainParams(hInstance, 0, 
                        cmdLine?cmdLine:_T(""), SW_SHOWNORMAL);

  TRY {
    int retVal = OwlMain(argc, argv);
    TRACEX(OwlMain, 0, _T("WinMain() returns ") << retVal);
    return retVal;
  }
  CATCH( (NS_OWL::TXEndSession&) {return 0;} )
  CATCH( (XBASE& x) {return NS_OWL::HandleGlobalException(x, 0);} )
}

/* ========================================================================== */

