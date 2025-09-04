//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//
// Definition of class TMDIChild.
//----------------------------------------------------------------------------

#if !defined(OWL_MDICHILD_H)
#define OWL_MDICHILD_H

#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
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


class _OWLCLASS TMDIClient;

//
// class TMDIChild
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TMDIChild : virtual public TFrameWindow {
  public:
    TMDIChild(TMDIClient&     parent,
              LPCTSTR         title = 0,
              TWindow*        clientWnd = 0,
              bool            shrinkToClient = false,
              TModule*        module = 0);

    TMDIChild(HWND hWnd, TModule* module = 0);

   ~TMDIChild();

    // Override virtual methods defined by TWindow
    //
    bool       PreProcessMsg(MSG& msg);
    bool       ShowWindow(int cmdShow);
    bool       EnableWindow(bool enable);
    void       Destroy(int retVal = 0);

  protected:
    void       PerformCreate(int menuOrId);
    TResult    DefWindowProc(uint msg, TParam1 param1, TParam2 param2);
    void       EvMDIActivate(HWND hWndActivated, HWND hWndDeactivated);
    bool       EvNCActivate(bool active);
    void        EvMenuSelect(uint menuItemId, uint flags, HMENU hMenu);
    void       EvEnterIdle(uint source, HWND hWndDlg);

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TMDIChild(const TMDIChild&);
    TMDIChild& operator =(const TMDIChild&);

  DECLARE_RESPONSE_TABLE(TMDIChild);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TMDIChild, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TMDIChild, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TMDIChild );

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

typedef TMDIChild TMdiChild;


__OWL_END_NAMESPACE


#endif  // OWL_MDICHILD_H
