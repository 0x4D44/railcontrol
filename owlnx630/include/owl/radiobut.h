//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//
// Definition of class TRadioButton.  This defines the basic behavior for all
// radio buttons.
//----------------------------------------------------------------------------

#if !defined(OWL_RADIOBUT_H)
#define OWL_RADIOBUT_H

#if !defined(OWL_CHECKBOX_H)
# include <owl/checkbox.h>
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
// class TRadioButton
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TRadioButton : public TCheckBox {
  public:
    TRadioButton(TWindow*        parent,
                 int             id,
                 LPCTSTR         title,
                 int x, int y, int w, int h,
                 TGroupBox*      group = 0,
                 TModule*        module = 0);

    TRadioButton(TWindow*   parent,
                 int        resourceId,
                 TGroupBox* group = 0,
                 TModule*   module = 0);

  protected:
    // Child id notification handled at the child
    //
    void       BNClicked();  // BN_CLICKED
    LPCTSTR    GetClassName();

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TRadioButton(const TRadioButton&);
    TRadioButton& operator=(const TRadioButton&);
      
  DECLARE_RESPONSE_TABLE(TRadioButton);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TRadioButton, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TRadioButton, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TRadioButton );

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_RADIOBUT_H
