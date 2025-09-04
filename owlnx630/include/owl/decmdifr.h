//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of TDecoratedMDIFrame class
//----------------------------------------------------------------------------

#if !defined(OWL_DECMDIFR_H)
#define OWL_DECMDIFR_H

#if !defined(OWL_MDI_H)
# include <owl/mdi.h>
#endif
#if !defined(OWL_DECFRAME_H)
# include <owl/decframe.h>
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
// class TDecoratedMDIFrame
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// MDI frame that supports decoration child windows
//
class _OWLCLASS TDecoratedMDIFrame : public TMDIFrame,
                                     public TDecoratedFrame {
  public:
    TDecoratedMDIFrame(LPCTSTR          title,
                       TResId          menuResId,
                       TMDIClient&     clientWnd = *new TMDIClient,
                       bool            trackMenuSelection = false,
                       TModule*        module = 0);

    TResult  DefWindowProc(uint message, TParam1 wParam, TParam2 lParam);

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TDecoratedMDIFrame(const TDecoratedMDIFrame&);
    TDecoratedMDIFrame& operator =(const TDecoratedMDIFrame&);

  DECLARE_RESPONSE_TABLE(TDecoratedMDIFrame);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TDecoratedMDIFrame, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TDecoratedMDIFrame, 1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TDecoratedMDIFrame );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

typedef TDecoratedMDIFrame TDecoratedMdiFrame;


__OWL_END_NAMESPACE


#endif  // OWL_DECMDIFR_H

