//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Dispatch functions (crackers) to crack a Windows message and pass control
// to a member function via a pointer (pmf).
//----------------------------------------------------------------------------

#if !defined(OWL_DISPATCH_H)
#define OWL_DISPATCH_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//__OWL_BEGIN_NAMESPACE

//
// class GENERIC
// ~~~~~ ~~~~~~~
// Generic class for casting pointer to objects and pointer to member functions
// Class is not actually defined or implemented anywhere
//

//DLN Look into this more, it might be cause of other portability problems
// Replaced by Val Ovechkin 3:27 PM 5/26/98
//class _CALLCNVN GENERIC;   // Conditionally define to correct calling model
#if defined(__SUNPRO_CC) || defined(__HPUX_SOURCE)
class GENERIC { };
#else
class _CALLCNVN GENERIC;
#endif


#if defined(BI_COMP_MSC)
# pragma pointers_to_members(full_generality, virtual_inheritance)
#endif

//
// typedef TAnyPMF
// ~~~~~~~ ~~~~~~~
// generic pointer to member function
//
typedef void (GENERIC::*TAnyPMF)();

//
// typedef TAnyDispatcher
// ~~~~~~~ ~~~~~~~~~~~~~~
// all message dispatcher functions take four parameters:
// - reference to an object
// - pointer to member function (signature varies according to the cracking
//   that the function performs)
// - wParam
// - lParam
//
typedef _OWLFUNC(int32) (*TAnyDispatcher)(GENERIC&, TAnyPMF, uint, int32);

//
// LEGEND: in order to keep dispatcher names from getting too long, the
//         following abbreviations are used. The names are based on the data
//         sizes passed & returned, & which param they come from.
//
// - v   (void return)
// - i   (int)
// - U   (uint)
// - H   (HANDLE) (requires special cracking, uint size)
// - I32 (int32)
// - POINT (TPoint&) (TPoint object constructed)
// - POINTER (void*) (model ambient size)
//
// Possible future cracker encoding
//  Which param:
//  - 1  wParam
//  - 2  lParam
//    How cracked (default to no processing, size of param):
//    - U Ambient size uint (squashed lParam in 16bit land)
//    - L Low 16bit word
//    - H high 16bit word
//
// Custom message crackers are named based on the message they crack
//

//----------------------------------------------------------------------------

//
// passes lParam as an int32 and returns an int result
//
_OWLFUNC(int32)
i_LPARAM_Dispatch(GENERIC& genericxx,
                  int     (GENERIC::*pmf)(int32),
                  uint     wParam,
                  int32    lParam);

//
// passes wParam as a uint and returns an int result
//
_OWLFUNC(int32)
i_WPARAM_Dispatch(GENERIC& genericxx,
                  int     (GENERIC::*pmf)(uint),
                  uint     wParam,
                  int32    lParam);

//
// passes wParam as a uint and returns a bool result
//
_OWLFUNC(int32)
B_WPARAM_Dispatch(GENERIC& genericxx,
                  bool     (GENERIC::*pmf)(uint),
                  uint     wParam,
                  int32    lParam);

//
// passes wParam as a bool and returns a bool result
//
_OWLFUNC(int32)
B_B_Dispatch(GENERIC& genericxx,
             bool     (GENERIC::*pmf)(bool),
             uint     wParam,
             int32    lParam);

//----------------------------------------------------------------------------

//
// passes nothing and returns an int32 result
//
_OWLFUNC(int32)
I32_Dispatch(GENERIC& genericxx,
             uint32  (GENERIC::*pmf)(),
             uint,
             int32);

//
// passes lParam as an int32 and returns an int32 result
//
_OWLFUNC(int32)
I32_LPARAM_Dispatch(GENERIC& genericxx,
                    int32   (GENERIC::*pmf)(int32),
                    uint,
                    int32    lParam);

//
// passes lParam as a uint and returns an int32 result
//
_OWLFUNC(int32)
I32_U_Dispatch(GENERIC& genericxx,
               int32   (GENERIC::*pmf)(uint),
               uint,
               int32    lParam);

//
// passes wParam as a uint and lParam as an int32 and returns an int32 result
//
_OWLFUNC(int32)
I32_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                           int32   (GENERIC::*pmf)(uint, int32),
                           uint     wParam,
                           int32    lParam);

//----------------------------------------------------------------------------

//
// passes no arguments and returns a uint result
//
_OWLFUNC(int32)
U_Dispatch(GENERIC& genericxx,
           uint    (GENERIC::*pmf)(),
           uint     wParam,
           int32    lParam);
//
// passes lParam as an int32 and returns a bool result
//
_OWLFUNC(int32)
B_LPARAM_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(int32),
                  uint     wParam,
                  int32    lParam);

//
// passes no arguments and returns a bool result
//
_OWLFUNC(int32)
B_Dispatch(GENERIC& genericxx,
           bool    (GENERIC::*pmf)(),
           uint     wParam,
           int32    lParam);

//
// passes lParam as an int32 and returns a uint result
//
_OWLFUNC(int32)
U_LPARAM_Dispatch(GENERIC& genericxx,
                  uint    (GENERIC::*pmf)(int32),
                  uint     wParam,
                  int32    param2);

//
// passes nothing and returns a uint32 result
//
_OWLFUNC(int32)
U32_Dispatch(GENERIC& genericxx,
             uint32  (GENERIC::*pmf)(),
             uint,
             int32);

//
// passes lParam as a TPoint& and returns a uint result
//
_OWLFUNC(int32)
U_POINT_Dispatch(GENERIC& genericxx,
                 uint    (GENERIC::*pmf)(__OWL TPoint&),
                 uint,
                 int32    lParam);

//
// passes lParam as a void* and returns a uint result
//
_OWLFUNC(int32)
U_POINTER_Dispatch(GENERIC& genericxx,
                   uint    (GENERIC::*pmf)(void*),
                   uint,
                   int32    lParam);

//
// passes lParam as a void* and returns a bool result
//
_OWLFUNC(int32)
B_POINTER_Dispatch(GENERIC& genericxx,
                   bool    (GENERIC::*pmf)(void*),
                   uint,
                   int32    lParam);

//
// passes wParam as a uint and returns a uint result
//
_OWLFUNC(int32)
U_U_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(uint),
             uint    param1,
             int32);

//
// passes wParam as a bool and returns a bool result
//
_OWLFUNC(int32)
B_B_Dispatch(GENERIC& genericxx,
             bool    (GENERIC::*pmf)(bool),
             uint    param1,
             int32);

//
// Passes param2 as an int and returns bool
//
_OWLFUNC(int32)
B_I2_Dispatch(GENERIC& genericxx,
              bool  (GENERIC::*pmf)(int),
              uint,
              int32    param2);

//
// passes wParam as a bool and returns a uint result
//
_OWLFUNC(int32)
U_B_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(bool),
             uint     param1,
             int32);

//
// passes wParam as bool, lParam as a uint and returns a bool result
//
_OWLFUNC(int32)
B_B_U_Dispatch(GENERIC& genericxx,
             bool    (GENERIC::*pmf)(bool, uint),
             uint     wParam,
             int32    lParam);

//
// passes wParam as a uint, lParam as a uint and returns a uint
//
_OWLFUNC(int32)
U_U_U_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(uint, uint),
             uint,
             int32    lParam);

//
// passes wParam as a bool, lParam as a uint and returns a uint
//
_OWLFUNC(int32)
U_B_U_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(bool, uint),
             uint,
             int32    lParam);

//
// passes wParam as a uint, lParam as a uint and returns a bool
//
_OWLFUNC(int32)
B_U_U_Dispatch(GENERIC& genericxx,
             bool    (GENERIC::*pmf)(uint, uint),
             uint,
             int32    lParam);

//
// passes wParam as a uint, lParam as a  pointer to a rect, copying to a
// temp rect & back, if needed for near data models only.  returns uint
//
_OWLFUNC(int32)
U_U_RECT_Dispatch(GENERIC& genericxx,
                  uint    (GENERIC::*pmf)(uint, __OWL TRect&),
                  uint     param1,
                  int32    param2);

//
// passes wParam as a uint, lParam as a  pointer to a rect, copying to a
// temp rect & back, if needed for near data models only.  returns bool
//
_OWLFUNC(int32)
B_U_RECT_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(uint, __OWL TRect&),
                  uint     param1,
                  int32    param2);

//
// passes wParam.lo as a uint and wParam.hi as uint, lParam as a  pointer to a point, 
// copying to a temp rect & back, if needed for near data models only.  returns bool
//
_OWLFUNC(int32)
B_U_U_POINT_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(uint, uint, __OWL TPoint&),
                  uint     param1,
                  int32    param2);

//
// passes wParam.lo as a uint and wParam.hi as int, lParam as a  pointer to a point, 
// copying to a temp rect & back, if needed for near data models only.  returns bool
//
_OWLFUNC(int32)
B_U_i_POINT_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(uint, int, __OWL TPoint&),
                  uint     param1,
                  int32    param2);

//
// passes wParam as a uint, lParam.lo as a uint, and lParam.hi as a uint
//
_OWLFUNC(int32)
U_U_U_U_Dispatch(GENERIC& genericxx,
                 uint    (GENERIC::*pmf)(uint, uint, uint),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a uint, lParam.lo as a uint, and lParam.hi as a uint
// and returns a bool
_OWLFUNC(int32)
B_U_U_U_Dispatch(GENERIC& genericxx,
                 bool    (GENERIC::*pmf)(uint, uint, uint),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a uint and lParam as an int32 and returns a uint result
//
_OWLFUNC(int32)
U_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         uint    (GENERIC::*pmf)(uint, int32),
                         uint     wParam,
                         int32    lParam);

//
// passes wParam as a uint and lParam as an int32 and returns a bool result
//
_OWLFUNC(int32)
B_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         uint    (GENERIC::*pmf)(uint, int32),
                         uint     wParam,
                         int32    lParam);

//
// passes wParam as a bool and lParam as an int32 and returns a uint result
//
_OWLFUNC(int32)
U_B_LPARAM_Dispatch(GENERIC& genericxx,
                    uint    (GENERIC::*pmf)(bool, int32),
                    uint     wParam,
                    int32    lParam);

//----------------------------------------------------------------------------

//
// passes nothing and always returns 0
//
_OWLFUNC(int32)
v_Dispatch(GENERIC& genericxx,
           void    (GENERIC::*pmf)(),
           uint,
           int32);

//
// passes lParam as an int32 and always returns 0
//
_OWLFUNC(int32)
v_LPARAM_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(int32),
                  uint,
                  int32    lParam);

//
// passes lParam as a TPoint& and always returns 0
//
_OWLFUNC(int32)
v_POINT_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(__OWL TPoint&),
                 uint,
                 int32    lParam);

//
// passes lParam as a void* and always returns 0
//
_OWLFUNC(int32)
v_POINTER_Dispatch(GENERIC& genericxx,
                   void    (GENERIC::*pmf)(void*),
                   uint,
                   int32    lParam);

//
// passes lParam as a uint and always returns 0
//
_OWLFUNC(int32)
v_U_Dispatch(GENERIC& genericxx,
             void    (GENERIC::*pmf)(uint),
             uint,
             int32    lParam);

//
// passes wParam as a uint and lParam as a TPoint& and always returns 0
//
_OWLFUNC(int32)
v_U_POINT_Dispatch(GENERIC& genericxx,
                   void    (GENERIC::*pmf)(uint, __OWL TPoint&),
                   uint     wParam,
                   int32    lParam);

//
// passes wParam as a uint and lParam as a uint and always returns 0
//
_OWLFUNC(int32)
v_U_U_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(uint, uint),
               uint     wParam,
               int32    lParam);

//
// passes wParam as a uint and lParam as a bool and always returns 0
//
_OWLFUNC(int32)
v_U_B_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(uint, bool),
               uint     wParam,
               int32    lParam);

//
// passes wParam as a bool and lParam as a uint and always returns 0
//
_OWLFUNC(int32)
v_B_U_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(bool, uint),
               uint     wParam,
               int32    lParam);

//
// passes wParam as a uint, lParam.lo as a uint, and lParam.hi as a uint and
// always returns 0
//
_OWLFUNC(int32)
v_U_U_U_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, uint),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a uint, lParam.lo as a int, and lParam.hi as a int and
// always returns 0
//
_OWLFUNC(int32)
v_U_i_i_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, int32, int32),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a bool, lParam.lo as a uint, and lParam.hi as a uint and
// always returns 0
//
_OWLFUNC(int32)
v_B_U_U_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(bool, uint, uint),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a uint, lParam.lo as a uint, and lParam.hi as a bool and
// always returns 0
//
_OWLFUNC(int32)
v_U_U_B_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, bool),
                 uint     wParam,
                 int32    lParam);

//
// passes wParam as a uint and always returns 0
//
_OWLFUNC(int32)
v_WPARAM_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(uint),
                  uint     wParam,
                  int32    lParam);

//
// passes wParam as an HWND and lParam as a pointer and always returns true
// for WM_COPYDATA
//
_OWLFUNC(int32)
v_HWND_PCOPYDATASTRUCT_Dispatch(GENERIC& genericxx,
                         void    (GENERIC::*pmf)(HWND, COPYDATASTRUCT*),
                         uint     wParam,
                         int32    lParam);

//
// passes wParam as a bool and always returns 0
//
_OWLFUNC(int32)
v_B_Dispatch(GENERIC& genericxx,
             void    (GENERIC::*pmf)(bool),
             uint     wParam,
             int32    lParam);

//
// passes wParam and lParam as bools and always returns 0
//
_OWLFUNC(int32)
v_B_B_Dispatch(GENERIC& genericxx,
             void    (GENERIC::*pmf)(bool, bool),
             uint     wParam,
             int32    lParam);

//
// passes wParam as a uint and lParam as an int32 and always returns 0
//
_OWLFUNC(int32)
v_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         void    (GENERIC::*pmf)(uint, int32),
                         uint     wParam,
                         int32    lParam);


//----------------------------------------------------------------------------
// Semi-custom crackers

//
// passes a uint, Handle, and uint and returns an int result
// 32-bit: wParam.lo, lParam,    wParam.hi
// 16-bit: wParam,    lParam.lo, lParam.hi
//
_OWLFUNC(int32)
i_U_W_U_Dispatch(GENERIC& genericxx,
                 int     (GENERIC::*pmf)(uint, uint, uint),
                 uint     wParam,
                 int32    lParam);

//
// passes two uints and a HANDLE and always returns 0
// 32-bit passes: wParam.lo, wParam.hi, lParam
// 16-bit passes: wParam,    lParam.lo, lParam.hi (same as v_U_U_U)
//
_OWLFUNC(int32)
v_U_U_W_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, uint),
                 uint     wParam,
                 int32    lParam);

//----------------------------------------------------------------------------
// message-specific crackers

//
// cracker for WM_ACTIVATE
// passes a uint, a bool, and an HWND and always returns 0
//
_OWLFUNC(int32)
v_Activate_Dispatch(GENERIC& genericxx,
                    void    (GENERIC::*pmf)(uint, bool, uint),
                    uint     wParam,
                    int32    lParam);

//
// cracker for WM_MDIACTIVATE
// passes two HWNDs and always returns 0
//
_OWLFUNC(int32)
v_MdiActivate_Dispatch(GENERIC& genericxx,
                       void    (GENERIC::*pmf)(uint, uint),
                       uint     wParam,
                       int32    lParam);


//
// cracker for WM_PARENTNOTIFY
// passes two uints and an HWND and returns a 32bit result
//
_OWLFUNC(int32)
I32_MenuChar_Dispatch(GENERIC& genericxx,
                      int32   (GENERIC::*pmf)(uint, uint, uint),
                      uint     wParam,
                      int32    lParam);

//
// cracker for WM_PARENTNOTIFY
// passes three uints and always returns 0
//
_OWLFUNC(int32)
v_ParentNotify_Dispatch(GENERIC& genericxx,
                        void    (GENERIC::*pmf)(uint, uint, uint),
                        uint     wParam,
                        int32    lParam);


//__OWL_END_NAMESPACE

//----------------------------------------------------------------------------
// Aliases for compatibility

#define HBRUSH_HDC_W_U_Dispatch  U_U_U_U_Dispatch
#define LRESULT_U_U_W_Dispatch   I32_MenuChar_Dispatch
#define LRESULT_WPARAM_LPARAM_Dispatch I32_WPARAM_LPARAM_Dispatch
#define v_U_B_W_Dispatch         v_Activate_Dispatch
#define v_W_W_Dispatch           v_MdiActivate_Dispatch



#endif  // OWL_DISPATCH_H
