// ObjectWindows Library
//
// 1998 Yura Bidus
//
//
#include <owlfx/core.h>
#pragma hdrstop

#ifdef _DLL
// The following symbol used to force inclusion of this module for _OWLDLL
#if defined(_X86_) || defined(_MAC)
extern "C" { int _afxForceSTDAFX; }
#else
extern "C" { int __afxForceSTDAFX; }
#endif
#endif
