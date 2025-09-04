//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:33 $
//
// Definition of class TControl.  This defines the basic behavior of all
// controls.
//----------------------------------------------------------------------------

#if !defined(OWL_CONTROL_H)
#define OWL_CONTROL_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
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
// enum TUseNative
// ~~~~ ~~~~~~~~~~
// Settings to suggest how a control should obtain its implementation, OWL or
// Native. nuNever..nuAlways. Control can be later queried back per
// instance to determine the choice made by looking at the nuUsing bit.
//
enum TNativeUse {
              // Suggested native control use for class
  nuNever,    // Instance should never use native implementation
  nuAvoid,    // Avoid if possible, unless options require native
  nuDontCare, // Don't care--control uses whatever is best
  nuAttempt,  // Attempt to use, unless options not supported
  nuAlways,   // Always use when available
  nuSuggestion = 0xFF,

  nuUsing = 0x8000, // Instance is using native
};

//
// class TControl
// ~~~~~ ~~~~~~~~
class _OWLCLASS TControl : virtual public TWindow {
  public:
    TControl(TWindow*        parent,
             int             id,
             LPCTSTR          title,
             int x, int y, int w, int h,
             TModule*        module = 0);

    TControl(TWindow* parent, int resourceId, TModule* module = 0);
   ~TControl();

    TNativeUse GetNativeUse() const;

  protected:

    // Constructor to alias non-OWL control
    //
    TControl(HWND hWnd, TModule* module = 0);

    // These methods are called for owner-draw controls (buttons, list boxes,
    // and combo boxes)
    //
    virtual int   CompareItem(COMPAREITEMSTRUCT & compareInfo);
    virtual void  DeleteItem(DELETEITEMSTRUCT & deleteInfo);
    virtual void  MeasureItem(MEASUREITEMSTRUCT & measureInfo);
    virtual void  DrawItem(DRAWITEMSTRUCT & drawInfo);

    // Default behavior for DrawItem is to call one of the following based on
    // the draw type:
    //
    virtual void  ODADrawEntire(DRAWITEMSTRUCT & drawInfo);
    virtual void  ODAFocus(DRAWITEMSTRUCT & drawInfo);
    virtual void  ODASelect(DRAWITEMSTRUCT & drawInfo);

    // Message response functions
    //
    void          EvPaint();
    TResult       EvCompareItem(uint ctrlId, COMPAREITEMSTRUCT & comp);
    void          EvDeleteItem(uint ctrlId, DELETEITEMSTRUCT & del);
    void          EvDrawItem(uint ctrlId, DRAWITEMSTRUCT & draw);
    void          EvMeasureItem(uint ctrlId, MEASUREITEMSTRUCT & meas);

  protected:
    TNativeUse    NativeUse;  // Using a native control implementation

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TControl(const TControl&);
    TControl& operator =(const TControl&);

  DECLARE_RESPONSE_TABLE(TControl);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TControl, 2);
  DECLARE_STREAMABLE_OWL(NS_OWL::TControl, 2);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TControl );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline Implementations
//

//
// Return if OWL is using the native common control or not.
//
inline TNativeUse TControl::GetNativeUse() const
{
  return NativeUse;
}


__OWL_END_NAMESPACE


#endif  // OWL_CONTROL_H
