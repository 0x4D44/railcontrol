//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//
// Definition of class TTimeGadget
//----------------------------------------------------------------------------

#if !defined(OWL_TIMEGADG_H)
#define OWL_TIMEGADG_H

#if !defined(OWL_TEXTGADG_H)
# include <owl/textgadg.h>
#endif
#if !defined(OWL_TIME_H)
# include <owl/time.h>
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
// class TTimeGadget
// ~~~~~ ~~~~~~~~~~~
// A gadget for displaying a time and/or date message.
//
class _OWLCLASS TTimeGadget : public TTextGadget {
  public:

    typedef void _CALLCNVN (*TGetTimeFunc)(owl_string&);

    TTimeGadget(TGetTimeFunc timeFunc = &TTimeGadget::GetSystemTime,
                int id = 0,
                TBorderStyle = Recessed, 
                TAlign = Center,
                uint numChars = 12, 
                LPCTSTR text = 0,
                TFont* font = 0 /*new TGadgetWindowFont*/);

    // Override from TGadget
    //
    void  Created();
    bool  IdleAction(long count);

    static void GetTTime(owl_string&);
    static void GetSystemTime(owl_string&);
    static void GetLocalTime(owl_string&);

  private:
    TGetTimeFunc TimeFunc;
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_TIMEGADG_H
