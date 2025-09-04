//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Definition of mode-tracking text gadget class TModeGadget.
//----------------------------------------------------------------------------

#if !defined(OWL_MODEGAD_H)
#define OWL_MODEGAD_H

#if !defined(OWL_TEXTGADG_H)
# include <owl/textgadg.h>
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
// class TModeGadget
// ~~~~~ ~~~~~~~~~~~
class _OWLCLASS TModeGadget : public TTextGadget {
  public:
    TModeGadget(int vkKey, LPCTSTR text, int id = 0,
                TBorderStyle = Recessed, TAlign = Center,
                TFont* font = 0 /*new TGadgetWindowFont*/);

    // Override from TGadget
    //
    bool IdleAction(long count);

  private:
    int VkKey;
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif // OWL_MODEGAD_H
