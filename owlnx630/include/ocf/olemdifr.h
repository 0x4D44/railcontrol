//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.15 $
//$Author: jogybl $
//$Date: 2007-11-20 09:10:27 $
//
//----------------------------------------------------------------------------

#if !defined(OCF_OLEMDIFR_H)
#define OCF_OLEMDIFR_H

#if !defined(OCF_OLEFRAME_H)
# include <ocf/oleframe.h>
#endif
#if !defined(OWL_MDI_H)
# include <owl/mdi.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OCF_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TOleMDIFrame
// ~~~~~ ~~~~~~~~~~~~
// Decorated MDI frame that supports OLE 2 using OCF
//
class _OCFCLASS TOleMDIFrame : public TOleFrame, public __OWL TMDIFrame {
  public:
    TOleMDIFrame(LPCTSTR			title,
                 __OWL TResId       menuResId,
                 __OWL TMDIClient&  clientWnd = *new __OWL TMDIClient,
                 bool					trackMenuSelection = false,
                 __OWL TModule*			module = 0);

   ~TOleMDIFrame();

  protected:
    __OWL TResult    DefWindowProc(uint message, __OWL TParam1 param1, __OWL TParam2 param2);
    bool       EvOcAppInsMenus(TOcMenuDescr & sharedMenu);
    void       EvActivateApp(bool active, HTASK hTask);

  DECLARE_RESPONSE_TABLE(TOleMDIFrame);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

typedef TOleMDIFrame TOleMdiFrame;


__OCF_END_NAMESPACE


#endif  // OWL_OLEMDIFR_H
