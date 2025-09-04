//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Definition TMenuGadget class
//----------------------------------------------------------------------------

#if !defined(OWL_MENUGADGET_H)
#define OWL_MENUGADGET_H

#if !defined(OWL_TEXTGADG_H)
# include <owl/textgadg.h>
#endif
#if !defined(OWL_MENU_H)
# include <owl/menu.h>
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
// class TMenuGadget
// ~~~~~ ~~~~~~~~~~~
class _OWLCLASS TMenuGadget : public TTextGadget {
  public:
    TMenuGadget(TMenu& menu, TWindow* window, int id = 0,
                TBorderStyle borderStyle = TGadget::ButtonUp,
                LPTSTR text = 0, TFont* font = new TGadgetWindowFont);
   ~TMenuGadget();

    // Override from TGadget
    //
    virtual void LButtonDown(uint modKeys, TPoint& p);
    // Added support for Flat Style
    //
    virtual void PaintBorder(TDC& dc);
    virtual void GetDesiredSize(TSize& size);

    //
    // Added support for Hot text
    virtual TColor GetEnabledColor() const;

  private:
    TPopupMenu* PopupMenu;
    TWindow*    CmdTarget;
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_MENUGADGET_H
