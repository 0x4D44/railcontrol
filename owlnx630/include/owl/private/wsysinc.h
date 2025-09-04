//----------------------------------------------------------------------------
// Object Windows Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Includes windowing system headers, with necessary macros defined
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_WSYSINC_H)
#define OWL_PRIVATE_WSYSINC_H
#define SERVICES_WSYSINC_H

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif

//
// For MS Windows, get windows.h and other standard windows headers
//
#if defined(BI_PLAT_MSW)
# if defined(_INC_WINDOWS)
    //
    // WINDOWS.H has already been included - Ensure that 
    // 1. STRICT is defined
    // 2. That the user did not request us to map BOOL to bool
    //
#   if !defined(STRICT)
#error if windows.h is included before services/wsysinc.h, STRICT must be defined
#   endif
# else
    //
    // We're going to pull in WINDOWS.H - define macros required for
    // framework libraries.
    //

#   if !defined(NOMINMAX)
#   define NOMINMAX
#   endif   // !defined(NOMINMAX)

#    if !defined(STRICT)
#      define STRICT
#   endif   // !defined(STRICT)

    // define Windows as Versin 5 to get additional data
#if !defined(_WIN32_WINNT)
#   define _WIN32_WINNT 0x0500
//#  define _WIN32_WINNT 0x0400
#endif


# pragma pack(push,8)

#  if defined(BI_NEED_COMMDLG_H)
//Jogy  commdlg.h is included in windows.h, we need to override it
#   define _INC_COMMDLG
# endif

# include <windows.h>

#  if defined(BI_NEED_COMMDLG_H)
//Jogy  the commdlg.h is included in windows.h, we need to override it with the updated one 
#   undef _INC_COMMDLG
# include <api_upd/commdlg.h>
# endif

# pragma pack(pop)

# endif

//
// Retrieve other Windows headers (before we proceed with BOOL-remapping)
//

// VER.H is currently mucking with definitions of UINT if LIB is not 
// defined.
//

// Grab shell extension header
//
//
// Get Windows OLE2 headers only if requested
//
# if defined(INC_OLE2)
#   include <owl/private/ole2inc.h>
# endif



//
// Map BOOL, TRUE and FALSE to bool, true and false respectively.
//
# if defined(BI_UNIQUE_BOOL) && defined(BI_REDEFINE_BOOL)
#   undef  BOOL       
#   define BOOL bool
#   undef  FALSE
#   define FALSE false
#   undef  TRUE
#   define TRUE true
# endif

//    extern HINSTANCE _hInstance;

// For OS/2, get os2.h
//
#endif

// Avoid mapping MessageBox to MessageBox[A/W]
#if defined(MessageBox)
#undef MessageBox
inline int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
#if defined(UNICODE)
  { return MessageBoxExW(hWnd, lpText, lpCaption, uType, 0); }
# else
  { return MessageBoxExA(hWnd, lpText, lpCaption, uType, 0); }
# endif
#endif

// Avoid mapping GetFileTitle to GetFileTitle[A/W]
#if defined(GetFileTitle)
#undef GetFileTitle
inline short GetFileTitle(LPCTSTR lpszFile, LPTSTR lpszTitle, WORD cbBuf)
#ifdef UNICODE
  { return ::GetFileTitleW(lpszFile, lpszTitle, cbBuf); }
#else
  { return ::GetFileTitleA(lpszFile, lpszTitle, cbBuf); }
#endif
#endif

// Avoid Win95 mapping DrawStatusText to DrawStatusText[A/W]
#if defined(DrawStatusText)
#undef DrawStatusText
inline void DrawStatusText(HDC hDC, 
                           LPRECT lprc, 
                           LPCTSTR szText,
                           UINT uFlags)
#ifdef UNICODE
  { ::DrawStatusTextW(hDC, lprc, szText, uFlags); }
#else
  { ::DrawStatusTextA(hDC, lprc, szText, uFlags); }
#endif
#endif

// Avoid Win95 mapping DrawState to DrawState[A/W]
#if defined(DrawState)
#undef DrawState
inline BOOL DrawState(HDC hdc, HBRUSH hbr, DRAWSTATEPROC lpOutputFunc,
  LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
#ifdef UNICODE
  { return ::DrawStateW(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
    fuFlags); }
#else
  { return ::DrawStateA(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
    fuFlags); }
#endif
#endif


#endif  // OWL_PRIVATE_WSYSINC_H

