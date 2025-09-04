//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:32 $
//
// Definition of class TCheckBox. This defines the basic behavior for all check
// boxes.
//----------------------------------------------------------------------------

#if !defined(OWL_CHECKBOX_H)
#define OWL_CHECKBOX_H

#if !defined(OWL_BUTTON_H)
# include <owl/button.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

class _OWLCLASS TGroupBox;

//
// Checkbox flags indicating check state
//
enum {
  BF_CHECKED   = 0x01,
  BF_GRAYED    = 0x02,
  BF_UNCHECKED = 0x00,
};

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TCheckBox
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TCheckBox : public TButton {
  public:
    TCheckBox(TWindow*        parent,
              int             id,
              LPCTSTR          title,
              int x, int y, int w, int h,
              TGroupBox*      group = 0,
              TModule*        module = 0);

    TCheckBox(TWindow*   parent,
              int        resourceId,
              TGroupBox* group = 0,
              TModule*   module = 0);
   ~TCheckBox();

    void       Check();
    void       Uncheck();
    void       Toggle();

    uint       GetCheck() const;
    void       SetCheck(uint check);

    TGroupBox* GetGroup() const;
    void       SetGroup(TGroupBox* group);

    // Override TWindow virtual member functions
    //
    uint       Transfer(void* buffer, TTransferDirection direction);

  protected:
    //  Override TButton's processing so drawable check boxes and radio
    //  buttons work properly
    //
    uint       EvGetDlgCode(MSG *);

    // Child id notification
    //
    void       BNClicked();  // BN_CLICKED

    LPCTSTR     GetClassName();

  public_data:
    TGroupBox* Group;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TCheckBox(const TCheckBox&);
    TCheckBox& operator =(const TCheckBox&);

  DECLARE_RESPONSE_TABLE(TCheckBox);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TCheckBox, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TCheckBox, 1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TCheckBox );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
inline void TCheckBox::Check() {
  SetCheck(BF_CHECKED);
}

//
inline void TCheckBox::Uncheck() {
  SetCheck(BF_UNCHECKED);
}

//
inline TGroupBox* TCheckBox::GetGroup() const {
  return Group;
}

//
inline void TCheckBox::SetGroup(TGroupBox* group) {
  Group = group;
}

//
inline uint TCheckBox::EvGetDlgCode(MSG *) {
  return (uint)DefaultProcessing();
}

//
inline uint TCheckBox::GetCheck() const {
  PRECONDITION(GetHandle());
  return (uint)CONST_CAST(TCheckBox*, this)->SendMessage(BM_GETCHECK);
}


__OWL_END_NAMESPACE


#endif  // OWL_CHECKBOX_H
