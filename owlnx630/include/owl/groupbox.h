//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of class TGroupBox.  This defines the basic behavior for all
// group boxes.
//----------------------------------------------------------------------------

#if !defined(OWL_GROUPBOX_H)
#define OWL_GROUPBOX_H

#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
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
// class TGroupBox
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TGroupBox : public TControl {
  public:
    TGroupBox(TWindow*        parent,
              int             id,
              LPCTSTR         text,
              int X, int Y, int W, int H,
              TModule*        module = 0);

    TGroupBox(TWindow* parent, int resourceId, TModule* module = 0);

    // Returns the length of the control's text
    //
    int   GetTextLen();

    // Fills the given string with the text of the control.  Returns the
    // number of characters copied.
    // Sets the contents of the associated static text control to the given
    // string
    //
    int   GetText(LPTSTR str, int maxChars);
    void  SetText(LPCTSTR str);

    virtual void SelectionChanged(int controlId);

    bool   GetNotifyParent() const;
    void   SetNotifyParent(bool notifyparent);

  protected:
    LPCTSTR    GetClassName();

  public_data:
    bool  NotifyParent;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TGroupBox(const TGroupBox&);
    TGroupBox& operator =(const TGroupBox&);

  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TGroupBox, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TGroupBox, 1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TGroupBox );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
inline int TGroupBox::GetTextLen() {
  return ::GetWindowTextLength(GetHandle());
}

//
inline int TGroupBox::GetText(LPTSTR str, int maxChars) {
  return ::GetWindowText(GetHandle(), str, maxChars);
}

//
inline void TGroupBox::SetText(LPCTSTR str) {
  ::SetWindowText(GetHandle(), str);
}

//
// Return the flag that indicates whether the parent is notified or not.
//
inline bool TGroupBox::GetNotifyParent() const
{
  return NotifyParent;
}

//
// Set the flag that indicates whether the parent is notified or not.
//
inline void TGroupBox::SetNotifyParent(bool notifyparent)
{
  NotifyParent = notifyparent;
}

__OWL_END_NAMESPACE


#endif  // OWL_GROUPBOX_H
