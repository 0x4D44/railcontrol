//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Class TControlGadget definition. TControlGadget is a gadget that takes a
// TWindow and allows it to be inserted into a gadget window. In effect it is
// an adapter that allows arbitrary controls to be used like gadgets in a tool
// bar, status bar, etc.
//----------------------------------------------------------------------------

#if !defined(OWL_CONTROLG_H)
#define OWL_CONTROLG_H

#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
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
// class TControlGadget
// ~~~~~ ~~~~~~~~~~~~~~
class _OWLCLASS TControlGadget : public TGadget {
  public:
    TControlGadget(TWindow& control, TBorderStyle = None);
   ~TControlGadget();

  protected:
    void           Created();
    void           Inserted();
    void           Removed();

    void           InvalidateRect(const TRect& rect, bool erase = true);
    void           Update();  // Paint now if possible

    void           SetBounds(const TRect& rect);

    void           GetDesiredSize(TSize& size);

  public:
    TWindow*       GetControl() const;
    void           SetControl(TWindow* control);

  protected_data:
    TWindow*       Control;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TControlGadget(const TControlGadget&);
    TControlGadget& operator =(const TControlGadget&);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline Implementations
//

//
// Return the control that is simulating a gadget.
//
inline TWindow* TControlGadget::GetControl() const {
  return Control;
}

//
// Set the control that is simulating a gadget.
//
inline void TControlGadget::SetControl(TWindow* control) {
  Control = control;
}

__OWL_END_NAMESPACE


#endif  // OWL_CONTROLG_H
