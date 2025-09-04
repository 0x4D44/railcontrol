//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Global data required for every module using OWL
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif

#if defined(__HPUX_SOURCE) || defined(_MSC_VER)
#include <owl/private/defs.h>
#include <owl/module.h>
#endif


 __OWL_USING_NAMESPACE

// Diagnostic group shared by LibMain/OwlMain modules
OWL_DIAGINFO;
//__OWL_BEGIN_NAMESPACE

DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlMain, true, 0);

__OWL_DECLCLASS(TDocTemplate);

//
// Global module pointer representing this executable instance
// provides the default instance for resources contructed within this module
//
#if defined(BI_COMP_BORLANDC) && !defined(BI_NEW_GMODULE)
__OWL TModule* Module  = 0;  // must be set to a TApplication* or a TModule*
#else

//# if defined(GCC_2953)
//__OWL TModule* Module  = 0;
//# else
__OWL TModule* gModule = 0;  // must be set to a TApplication* or a TModule*
//# endif /*GCC_2953*/

#endif /*BI_COMP_BORLAND && !BI_NEW_GMODULE*/
//
// Document templates contructed before doc manager get linked in here
//
__OWL TDocTemplate* DocTemplateStaticHead = 0;  // initialize to no templates

//
// This mask deems when certain features are available.
// See GetDocTemplateHead and GetModulePtr below.
//
const uint32 CompatibleVersionMask = 0xFFF00000;

//
// Exported access to global variables for DLLs that use OWL
//
//#if defined(BI_APP_DLL)
#if defined(BI_COMP_BORLANDC)
extern "C" { 
TDocTemplate** PASCAL __export GetDocTemplateHead(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &DocTemplateStaticHead : 0;
}
TModule** PASCAL __export 
GetModulePtr(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &gModule : 0;
}
}
#elif defined(BI_COMP_GNUC)
extern "C" { 
__export TDocTemplate** PASCAL GetDocTemplateHead(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &DocTemplateStaticHead : 0;
}
__export TModule** PASCAL 
GetModulePtr(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &gModule : 0;
}
}
#else

// EMF __export removed because it isn't needed in win32
/*__export*/ STDAPI_(TDocTemplate**) GetDocTemplateHead(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &DocTemplateStaticHead : 0;
}

/*__export*/ STDAPI_(TModule**) GetModulePtr(uint32 version)
{
  return (version&CompatibleVersionMask) == (OWLGetVersion()&CompatibleVersionMask) ? &gModule : 0;
}

#endif

//#endif

/* ========================================================================== */

