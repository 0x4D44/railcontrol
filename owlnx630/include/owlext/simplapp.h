//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software. All rights reserved.
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
// TSimpleApp
//-------------------------------------------------------------------
#ifndef __OWLEXT_SIMPLAPP_H
#define __OWLEXT_SIMPLAPP_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                    TSimpleApp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TSimpleApp : public __OWL TApplication
{
    // Object lifetime methods
    //
public:
    TSimpleApp(__OWL TFrameWindow* fwndw, LPCTSTR name = 0);

    // OWL mutators
    //
protected:
    virtual void InitMainWindow();
    virtual bool IdleAction(long idleCount);

    // Internal data
    //
private:
    __OWL TFrameWindow* m_window;
    static __OWL TAppDictionary s_appDictionary;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


#endif

