//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of class TControlBar.
//----------------------------------------------------------------------------

#if !defined(OWL_CONTROLB_H)
#define OWL_CONTROLB_H

#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
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
// class TControlBar
// ~~~~~ ~~~~~~~~~~~
// Implements a control bar which provides mnemonic access for its button
// gadgets. By default positions itself at the top of the window
//
class _OWLCLASS TControlBar : public TGadgetWindow {
  public:
    TControlBar(TWindow*        parent = 0,
                TTileDirection  direction= Horizontal,
                TFont*          font = new TGadgetWindowFont,
                TModule*        module = 0);
   ~TControlBar();

  DECLARE_CASTABLE;
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_CONTROLB_H
