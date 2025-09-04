//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Class definition for implementation of Mail enabling.
//----------------------------------------------------------------------------

#if !defined(OWL_MAILER_H)
#define OWL_MAILER_H

#if !defined(OWL_MODULE_H)
# include <owl/module.h>
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
// class TMailer
// ~~~~~ ~~~~~~~
class _OWLCLASS TMailer : public TModule {
  public:
    TMailer();

    bool  IsMAPIAvailable() const;
    void  SendDocuments(TWindow* owner, LPCTSTR paths,
                        LPCTSTR names = 0, bool asynchWork = false);

    TModuleProc5<uint32,uint32,LPSTR,LPSTR,LPSTR,uint32> MAPISendDocuments;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_MAILER_H
