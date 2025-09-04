//----------------------------------------------------------------------------
//  ObjectWindows
//  Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//   This header file declare the TTaceWindow class.
//----------------------------------------------------------------------------

#if !defined(__OWL_TRACEWND_H)
#define __OWL_TRACEWND_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if defined(__HPUX_SOURCE)
#include <owl/framewin.h>
#endif


__OWL_BEGIN_NAMESPACE
// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


class _OWLCLASS TPaneSplitter;
class _OWLCLASS TImageList;
 
class TTraceEdit;
class TGroupList;
class TModuleListBox;

struct TTraceWindowHook: public TDiagBaseHook {
  virtual void Output(TDiagBase* group, LPCTSTR str);
};


//
// class TTraceWindow
// ~~~~~ ~~~~~~~~~~~~
//
class TTraceWindow : public TFrameWindow {
  public:
    TTraceWindow ();
    ~TTraceWindow ();

    virtual bool    CanClose ();
    virtual bool    IdleAction (long);
    virtual void    SetupWindow ();
    virtual void    CleanupWindow();
    void            UpdateTraceText ();

    static TTraceWindow*  TraceWindow () { return sTraceWnd; }

  protected:
    static TTraceWindow* sTraceWnd;

    TTraceWindowHook  TraceHook;
    TDiagBaseHook*    OldHook;

    TPaneSplitter*  Panels;
    
    TTraceEdit*      Edit;
    TGroupList*      TraceGroups;
    TModuleListBox*  TraceModules;
    bool            TraceDirty;
    bool            Active;
    owl_string      TraceDir;
    owl_string      TraceText;

    void            AddModules      (TModule*  module);
    owl_string      FormatGroup     (TDiagBase* group);
    owl_string      FormatLink      (TModule* group);

    void            CmSave          ();
    void            CmTop           ();
    void            CeTop            (TCommandEnabler &);
    void            CmSetLevel      ();
    void            CeSetLevel      (TCommandEnabler &);

    void            SaveWindowState ();

    void            OutputMessage(TDiagBase* group, LPCTSTR msg);

    void            EvModuleChange  ();
    void            EvActivate(uint active, bool minimized, HWND hWndOther);

    friend struct TTraceWindowHook;

    DECLARE_RESPONSE_TABLE (TTraceWindow);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

#endif  // __OWL_TRACEWND_H

