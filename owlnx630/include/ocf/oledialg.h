//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.15 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TOleDialog, a TDialog that can host OLE controls.
//----------------------------------------------------------------------------

#ifdef _MSC_VER 
#   pragma once
#endif

#if !defined(OCF_OLEDIALG_H)
#define OCF_OLEDIALG_H

// Class name of stub control within a dialog which TOleDialog replaces with
// an OCX at runtime. The caption of the control contains the ProgID of the
// OCX to be created by TOleDialog.
//
#if !defined(OCX_STUB_CLASS)
# define OCX_STUB_CLASS _T("OCX")
#endif

#if !defined(RC_INVOKED)

#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#if !defined(OCF_OLEWINDO_H)
# include <ocf/olewindo.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OCF_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TOleDialog
// ~~~~~ ~~~~~~~~~~
class _OCFCLASS TOleDialog : public TOleWindow, public __OWL TDialog {
  public:
    TOleDialog(__OWL TWindow* parent, __OWL TResId resId, __OWL TModule* module = 0);
   ~TOleDialog();

    void      SetupWindow();
    bool      IdleAction(long idleCount);
    bool      PreProcessMsg(MSG& msg);

  protected:
    bool      EvOcViewSetSiteRect(__OWL TRect * rect);

  protected:
    HWND      CheckChild(HWND);
    void      LoadControl(HWND hControl);
    void      LoadControl(__OWL TString ProgId, const __OWL TRect& position);

  DECLARE_RESPONSE_TABLE(TOleDialog);
};

//
// class TRegisterOcxWnd
// ~~~~~ ~~~~~~~~~~~~~~~
class _OCFCLASS TRegisterOcxWnd {
  public:
    TRegisterOcxWnd(HINSTANCE);
   ~TRegisterOcxWnd();

  protected:
    HINSTANCE HAppInst;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


__OCF_END_NAMESPACE


#endif  // !RC_INVOKED
#endif  // OCF_OLEDIALG_H
