//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_COMPAT_H)
#define OWL_COMPAT_H


#if !defined(BI_OLDNAMES)
# define BI_OLDNAMES
#endif

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// struct TMessage
// ~~~~~~ ~~~~~~~~
struct TMessage {
  HWND  Receiver;
  uint16  Message;

  union {
    uint16  WParam;
#if defined(__GNUC__) //JJH gcc doesn' like struct tags inside unions
    struct /*tagWP*/ {
#else
    struct tagWP {
#endif
      uint8 Lo;
      uint8 Hi;
    } WP;
  };

  union {
    uint32  LParam;

#if defined(__GNUC__)
    struct /*tagLP*/ {
#else
    struct tagLP {
#endif
      uint16 Lo;
      uint16 Hi;
    } LP;
  };

  long  Result;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

class _OWLCLASS TWindow;
_OWLFUNC(TMessage) __GetTMessageWin(TWindow* win);
#define __GetTMessage() __GetTMessageWin(this)

//
// Constants for application messages
//
#define WM_FIRST        0x0000  // 0x0000- 0x7FFF window messages
#define WM_INTERNAL     0x7F00  // 0x7F00- 0x7FFF reserved for internal use
#define ID_FIRST        0x8000  // 0x8000- 0x8FFF child id messages
#define ID_INTERNAL     0x8F00  // 0x8F00- 0x8FFF reserved for internal use
#define NF_FIRST        0x9000  // 0x9000- 0x9FFF notification messages
#define NF_INTERNAL     0x9F00  // 0x9F00- 0x9FFF reserved for internal use
#define CM_FIRST        0xA000  // 0xA000- 0xFFFF command messages
#define CM_INTERNAL     0xFF00  // 0xFF00- 0xFFFF reserved for internal use
#define WM_COUNT        0x8000  // num of window msgs
#define ID_COUNT        0x1000  // num of child id msgs
#define NF_COUNT        0x1000  // num of notification msgs
#define CM_COUNT        0x6000  // num of command msgs

#define WM_RESERVED             WM_INTERNAL - WM_FIRST

#define ID_RESERVED             ID_INTERNAL - ID_FIRST
#define ID_FIRSTMDICHILD        ID_RESERVED + 1
#define ID_MDICLIENT            ID_RESERVED + 2

#define CM_RESERVED             CM_INTERNAL - CM_FIRST

//
// Error conditions
//
#define EM_INVALIDCHILD         -1
#define EM_INVALIDCLIENT        -2
#define EM_INVALIDMAINWINDOW    -3
#define EM_INVALIDMODULE        -4
#define EM_INVALIDWINDOW        -5
#define EM_OUTOFMEMORY          -6

typedef int _FAR & Rint;
typedef int _FAR * Pint;
typedef void _FAR * Pvoid;
typedef void _FAR * _FAR & RPvoid;
typedef char _FAR * Pchar;
typedef const char _FAR * PCchar;
typedef const void _FAR * PCvoid;


__OWL_END_NAMESPACE


#endif  // OWL_COMPAT_H
