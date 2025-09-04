//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of OWL message dispatcher functions
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DISPATCH_H)
# include <owl/dispatch.h>
#endif

//__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(BI_NO_NAMESPACE)
using OWL::TPoint;
using OWL::TRect;
#endif

//----------------------------------------------------------------------------

_OWLFUNC(int32)
i_LPARAM_Dispatch(GENERIC& genericxx,
                  int     (GENERIC::*pmf)(int32),
                  uint,
                  int32    param2)
{
  return (genericxx.*pmf)(param2);
}

_OWLFUNC(int32)
i_WPARAM_Dispatch(GENERIC& genericxx,
                  int     (GENERIC::*pmf)(uint),
                  uint     param1,
                  int32)
{
  return (genericxx.*pmf)(param1);
}

_OWLFUNC(int32)
B_WPARAM_Dispatch(GENERIC& genericxx,
                  bool     (GENERIC::*pmf)(uint),
                  uint     param1,
                  int32)
{
  return (genericxx.*pmf)(param1);
}

_OWLFUNC(int32)
B_B_Dispatch(GENERIC& genericxx,
             bool     (GENERIC::*pmf)(bool),
             uint     param1,
             int32)
{
  return (genericxx.*pmf)((bool)param1);
}

//----------------------------------------------------------------------------

_OWLFUNC(int32)
I32_Dispatch(GENERIC& genericxx,
             uint32  (GENERIC::*pmf)(),
             uint,
             int32)
{
  return (genericxx.*pmf)();
}

_OWLFUNC(int32)
I32_LPARAM_Dispatch(GENERIC& genericxx,
                    int32  (GENERIC::*pmf)(int32),
                    uint,
                    int32    param2)
{
  return (genericxx.*pmf)(param2);
}

//
// Passes param2 as a uint and returns an int32 result
//
_OWLFUNC(int32)
I32_U_Dispatch(GENERIC& genericxx,
               int32   (GENERIC::*pmf)(uint),
               uint,
               int32    param2)
{
  return (genericxx.*pmf)((uint)param2);
}

//
// Passes param2 as an int and returns bool
//
_OWLFUNC(int32)
B_I2_Dispatch(GENERIC& genericxx,
               bool  (GENERIC::*pmf)(int),
               uint,
               int32    param2)
{
  return (genericxx.*pmf)((int)param2);
}

_OWLFUNC(int32)
I32_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                           int32   (GENERIC::*pmf)(uint, int32),
                           uint     param1,
                           int32    param2)
{
  return (genericxx.*pmf)(param1, param2);
}

//----------------------------------------------------------------------------

_OWLFUNC(int32)
U_Dispatch(GENERIC& genericxx,
           uint  (GENERIC::*pmf)(),
           uint,
           int32)
{
  return (genericxx.*pmf)();
}

_OWLFUNC(int32)
U32_Dispatch(GENERIC& genericxx,
             uint32  (GENERIC::*pmf)(),
             uint,
             int32)
{
  return (genericxx.*pmf)();
}

_OWLFUNC(int32)
B_Dispatch(GENERIC& genericxx,
           bool(GENERIC::*pmf)(),
           uint,
           int32)
{
  return (genericxx.*pmf)();
}

_OWLFUNC(int32)
B_LPARAM_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(int32),
                  uint,
                  int32    param2)
{
  return (genericxx.*pmf)(param2) != false;
}

_OWLFUNC(int32)
U_LPARAM_Dispatch(GENERIC& genericxx,
                  uint    (GENERIC::*pmf)(int32),
                  uint,
                  int32    param2)
{
  return (genericxx.*pmf)(param2);
}

_OWLFUNC(int32)
U_POINT_Dispatch(GENERIC& genericxx,
                 uint    (GENERIC::*pmf)(TPoint&),
                 uint,
                 int32    param2)
{
  TPoint p(param2);
  return (genericxx.*pmf)(p);
}

_OWLFUNC(int32)
U_POINTER_Dispatch(GENERIC& genericxx,
                   uint    (GENERIC::*pmf)(void*),
                   uint,
                   int32    param2)
{
  return (genericxx.*pmf)((void*)param2);
}

_OWLFUNC(int32)
B_POINTER_Dispatch(GENERIC& genericxx,
                   bool    (GENERIC::*pmf)(void*),
                   uint,
                   int32    param2)
{
  return (genericxx.*pmf)((void*)param2);
}

_OWLFUNC(int32)
U_U_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(uint),
             uint     param1,
             int32)
{
  return (genericxx.*pmf)((uint)param1);
}

_OWLFUNC(int32)
U_B_Dispatch(GENERIC& genericxx,
             uint    (GENERIC::*pmf)(bool),
             uint     param1,
             int32)
{
  return (genericxx.*pmf)((bool)param1);
}

_OWLFUNC(int32)
B_B_U_Dispatch(GENERIC& genericxx,
             bool    (GENERIC::*pmf)(bool, uint),
             uint     param1,
             int32    param2)
{
  return (genericxx.*pmf)((bool)param1, (uint)param2);
}

_OWLFUNC(int32)
U_U_RECT_Dispatch(GENERIC& genericxx,
                  uint    (GENERIC::*pmf)(uint, TRect&),
                  uint     param1,
                  int32    param2)
{
  return (genericxx.*pmf)(param1, *(TRect*)param2);
}

_OWLFUNC(int32)
B_U_RECT_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(uint, TRect&),
                  uint     param1,
                  int32    param2)
{
  return (genericxx.*pmf)(param1, *(TRect*)param2);
}

//
// passes wParam.lo as a uint and wParam.hi as uint, lParam as a  pointer to a point,
// copying to a temp rect & back, if needed for near data models only.  returns bool
//
_OWLFUNC(int32)
B_U_U_POINT_Dispatch(GENERIC& genericxx,
                  bool    (GENERIC::*pmf)(uint, uint, TPoint&),
                  uint     param1,
                  int32    param2)
{
  TPoint p(param2);
  return (genericxx.*pmf)(LoUint16(param1), HiUint16(param1), p);
}

//
// passes wParam.lo as a uint and wParam.hi as int, lParam as a  pointer to a point,
// copying to a temp rect & back, if needed for near data models only.  returns bool
//
// WM_MOUSEWHEEL
_OWLFUNC(int32)
B_U_i_POINT_Dispatch(GENERIC& genericxx,
                     bool     (GENERIC::*pmf)(uint, int, __OWL TPoint&),
                     uint     param1,
                     int32    param2)
{
  TPoint p(LoInt16(param2), HiInt16(param2));  
  return (genericxx.*pmf)(LoUint16(param1), HiInt16(param1), p);
  //return 0;
}


//
_OWLFUNC(int32)
U_U_U_U_Dispatch(GENERIC& genericxx,
                 uint    (GENERIC::*pmf)(uint, uint, uint),
                 uint     param1,
                 int32    param2)
{
  return (genericxx.*pmf)(param1, LoUint16(param2), HiUint16(param2));
}

_OWLFUNC(int32)
U_U_U_Dispatch(GENERIC& genericxx,
               uint    (GENERIC::*pmf)(uint, uint),
               uint     wParam,
               int32    lParam)
{
  return (genericxx.*pmf)(wParam, uint(lParam));
}

_OWLFUNC(int32)
U_B_U_Dispatch(GENERIC& genericxx,
               uint    (GENERIC::*pmf)(bool, uint),
               uint     wParam,
               int32    lParam)
{
  return (genericxx.*pmf)((bool)wParam, uint(lParam));
}

_OWLFUNC(int32)
B_U_U_Dispatch(GENERIC& genericxx,
               bool    (GENERIC::*pmf)(uint, uint),
               uint     wParam,
               int32    lParam)
{
  return (genericxx.*pmf)(wParam, (uint)lParam); //cast for 16-bit compatability
}

_OWLFUNC(int32)
B_U_U_U_Dispatch(GENERIC& genericxx,
                 bool    (GENERIC::*pmf)(uint, uint, uint),
                 uint     param1,
                 int32    param2)
{
  return (genericxx.*pmf)(param1, LoUint16(param2), HiUint16(param2));
}

_OWLFUNC(int32)
U_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         uint    (GENERIC::*pmf)(uint, int32),
                         uint     param1,
                         int32    param2)
{
  return (genericxx.*pmf)(param1, param2);
}

_OWLFUNC(int32)
B_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         bool    (GENERIC::*pmf)(uint, int32),
                         uint     param1,
                         int32    param2)
{
  return (genericxx.*pmf)(param1, param2);
}

_OWLFUNC(int32)
U_B_LPARAM_Dispatch(GENERIC& genericxx,
                    uint    (GENERIC::*pmf)(bool, int32),
                    uint     param1,
                    int32    param2)
{
  return (genericxx.*pmf)((bool)param1, param2);
}

//----------------------------------------------------------------------------

_OWLFUNC(int32)
v_Dispatch(GENERIC& genericxx,
           void    (GENERIC::*pmf)(),
           uint,
           int32)
{
  (genericxx.*pmf)();
  return 0;
}

_OWLFUNC(int32)
v_LPARAM_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(int32),
                  uint,
                  int32    param2)
{
  (genericxx.*pmf)(param2);
  return 0;
}

_OWLFUNC(int32)
v_POINT_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(TPoint&),
                 uint,
                 int32    param2)
{
  TPoint p(param2);
  (genericxx.*pmf)(p);

  return 0;
}

_OWLFUNC(int32)
v_POINTER_Dispatch(GENERIC& genericxx,
                   void    (GENERIC::*pmf)(void*),
                   uint,
                   int32    param2)
{
  (genericxx.*pmf)((void*)param2);
  return 0;
}

_OWLFUNC(int32)
v_U_Dispatch(GENERIC& genericxx,
             void    (GENERIC::*pmf)(uint),
             uint,
             int32    param2)
{
  (genericxx.*pmf)((uint)param2);
  return 0;
}

_OWLFUNC(int32)
v_U_POINT_Dispatch(GENERIC& genericxx,
                   void    (GENERIC::*pmf)(uint, TPoint&),
                   uint     param1,
                   int32    param2)
{
  TPoint p(param2);
  (genericxx.*pmf)(param1, p);

  return 0;
}

_OWLFUNC(int32)
v_U_U_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(uint, uint),
               uint     param1,
               int32    param2)
{
  (genericxx.*pmf)(param1, (uint)param2);
  return 0;
}

_OWLFUNC(int32)
v_U_B_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(uint, bool),
               uint     param1,
               int32    param2)
{
  (genericxx.*pmf)(param1, (bool)param2);
  return 0;
}

_OWLFUNC(int32)
v_B_U_Dispatch(GENERIC& genericxx,
               void    (GENERIC::*pmf)(bool, uint),
               uint     param1,
               int32    param2)
{
  (genericxx.*pmf)((bool)param1, (uint)param2);
  return 0;
}

_OWLFUNC(int32)
v_U_U_U_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, uint),
                 uint     param1,
                 int32    param2)
{
  (genericxx.*pmf)(param1, LoUint16(param2), HiUint16(param2));
  return 0;
}

_OWLFUNC(int32)
v_U_i_i_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, int32, int32),
                 uint     param1,
                 int32    param2)
{
  (genericxx.*pmf)(param1, LoInt16(param2), HiInt16(param2));
  return 0;
}

_OWLFUNC(int32)
v_B_U_U_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(bool, uint, uint),
                 uint     param1,
                 int32    param2)
{
  (genericxx.*pmf)((bool)param1, LoUint16(param2), HiUint16(param2));
  return 0;
}

_OWLFUNC(int32)
v_U_U_B_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, bool),
                 uint     param1,
                 int32    param2)
{
  (genericxx.*pmf)(param1, LoUint16(param2), (bool)HiUint16(param2));
  return 0;
}

_OWLFUNC(int32)
v_WPARAM_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(uint),
                  uint     param1,
                  int32)
{
  (genericxx.*pmf)(param1);
  return 0;
}

_OWLFUNC(int32)
v_B_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(bool),
                  uint     param1,
                  int32)
{
  (genericxx.*pmf)((bool)param1);
  return 0;
}

_OWLFUNC(int32)
v_B_B_Dispatch(GENERIC& genericxx,
                  void    (GENERIC::*pmf)(bool, bool),
                  uint     param1,
                  int32    param2)
{
  (genericxx.*pmf)((bool)param1, (bool)param2);
  return 0;
}

_OWLFUNC(int32)
v_HWND_PCOPYDATASTRUCT_Dispatch(GENERIC& genericxx,
                         void    (GENERIC::*pmf)(HWND, COPYDATASTRUCT*),
                         uint     param1,
                         int32    param2)
{
  (genericxx.*pmf)((HWND)param1, (COPYDATASTRUCT*)param2);
  return true;
}

_OWLFUNC(int32)
v_WPARAM_LPARAM_Dispatch(GENERIC& genericxx,
                         void    (GENERIC::*pmf)(uint, int32),
                         uint     param1,
                         int32    param2)
{
  (genericxx.*pmf)(param1, param2);
  return 0;
}

//----------------------------------------------------------------------------
//
// Semi-custom message crackers
//
_OWLFUNC(int32)
i_U_W_U_Dispatch(GENERIC& genericxx,
                 int     (GENERIC::*pmf)(uint, uint, uint),
                 uint     param1,
                 int32    param2)
{
  return (genericxx.*pmf)(LoUint16(param1), param2, HiUint16(param1));
}

_OWLFUNC(int32)
v_U_U_W_Dispatch(GENERIC& genericxx,
                 void    (GENERIC::*pmf)(uint, uint, uint),
                 uint     param1,
                 int32    param2)
{
  (genericxx.*pmf)(LoUint16(param1), HiUint16(param1), param2);

  return 0;
}

//----------------------------------------------------------------------------
//
// Custom message crackers
//

_OWLFUNC(int32)
v_Activate_Dispatch(GENERIC& genericxx,
                    void    (GENERIC::*pmf)(uint, bool, uint),
                    uint     param1,
                    int32    param2)
{
  (genericxx.*pmf)(LoUint16(param1), (bool)HiUint16(param1), param2);

  return 0;
}

_OWLFUNC(int32)
v_MdiActivate_Dispatch(GENERIC& genericxx,
                       void    (GENERIC::*pmf)(uint, uint),
                       uint     param1,
                       int32    param2)
{
  (genericxx.*pmf)(param2, param1);

  return 0;
}

_OWLFUNC(int32)
I32_MenuChar_Dispatch(GENERIC& genericxx,
                      int32   (GENERIC::*pmf)(uint, uint, uint),
                      uint     param1,
                      int32    param2)
{
  return (genericxx.*pmf)(LoUint16(param1), HiUint16(param1), param2);
}

_OWLFUNC(int32)
v_ParentNotify_Dispatch(GENERIC& genericxx,
                        void    (GENERIC::*pmf)(uint, uint, uint),
                        uint     param1,
                        int32    param2)
{
  if (LoUint16(param1) >= WM_MOUSEFIRST && LoUint16(param1) <= WM_MOUSELAST)
    (genericxx.*pmf)(param1, LoUint16(param2), HiUint16(param2));
  else
    (genericxx.*pmf)(LoUint16(param1), param2, HiUint16(param1));

  return 0;
}

//__OWL_END_NAMESPACE
//==============================================================================
