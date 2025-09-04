//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// This file defines the abstract TKeyHook class.  The intended usage of
// TKeyHook is to derive from it and provide an implementation of the
// OnKeystroke() method.
//
// Original code by Don Griffin; used with permission.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_KEYHOOK_H)
#define __OWLEXT_KEYHOOK_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
# include <owlext/core.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                      TKeyHook
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TKeyHook
{
    protected:
        TKeyHook ();
        ~TKeyHook ();

        void    ActivateKeyHook (bool = true);
        bool    IsKeyHookActive () const { return mActive; }

        //  Call the base class version if you want to let other objects see
        //  the event (and eventually call CallNextHookEx).  Return true to
        //  eat the keystroke, false to let it pass.
        virtual bool    OnKeystroke (uint vkey, uint32 flags);

    private:
        TKeyHook  * mNext;
        bool        mActive;
#if defined (BI_COMP_BORLANDC) && __BORLANDC__ <= 0x520
        static TKeyHook *           __thread sFirstHook;
        static HHOOK                __thread sHook;
#endif
        static LRESULT CALLBACK     KeyboardProc (int, WPARAM, LPARAM);
        static uint                 ActiveCount ();
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


#endif  // __OWLEXT_KEYHOOK_H

