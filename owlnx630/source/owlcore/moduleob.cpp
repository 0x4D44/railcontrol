//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:49 $
//
// Implementation of obsolete functions in class TModule.
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#if !defined(OWL_MDI_H)
# include <owl/mdi.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if defined(OWL1_COMPAT)
//
//
//
TWindow*
TModule::MakeWindow(TWindow* win)
{
  if (win->Create())
    return win;

  win->Destroy();
  delete win;
  return 0;
}

//
//
//
int
TModule::ExecDialog(TDialog* dialog)
{
  int ret = dialog->Execute();
  delete dialog;
  return ret;
}

//
//
//
HWND
TModule::GetClientHandle(HWND hWnd)
{
  if (hWnd) {
    HWND  curWindow = GetWindow(hWnd, GW_CHILD);

    if (curWindow) {
      do {
        TCHAR  className[10];

        ::GetClassName(curWindow, className, 10);

        if (_tcsnicmp(className, _T("MDICLIENT"), 10) == 0)
          return curWindow;

      } while ((curWindow = GetWindow(curWindow, GW_HWNDNEXT)) != 0);
    }
  }

  return 0;
}

//
//
//
TWindow*
TModule::GetParentObject(HWND hWndParent)
{
  TWindow*  window = GetWindowPtr(hWndParent);
  HWND      clientHandle;

  if (window)
    return window;

  if ((clientHandle = GetClientHandle(hWndParent)) != 0)
    return new TMDIFrame(hWndParent, clientHandle);

  return new TWindow(hWndParent);
}

#endif // #if defined(OWL1_COMPAT)

__OWL_END_NAMESPACE
/* ========================================================================== */

