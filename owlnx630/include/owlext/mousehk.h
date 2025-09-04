//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// This file declares TMouseHook.  This class is used to encapsulate a
// WH_MOUSEHOOK Windows Hook.
//
// Original code by Don Griffin; used with permission.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_MOUSEHK_H)
#define __OWLEXT_MOUSEHK_H

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
//                                                                    TMouseHook
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TMouseHook
{
    protected:
        TMouseHook ();
        ~TMouseHook ();

        void    ActivateMouseHook (bool = true);
        bool    IsMouseHookActive () const { return mActive; }

        //  Call the base class version if you want to let other objects see
        //  the event (and eventually call CallNextHookEx).  Return true to
        //  eat the event, false to let it pass.
        virtual bool    OnMouseEvent (uint msg, MOUSEHOOKSTRUCT &);

    private:
        TMouseHook    * mNext;
        bool            mActive;

#if defined (BI_COMP_BORLANDC) && __BORLANDC__ <= 0x520
        static TMouseHook *         __thread sFirstHook;
        static HHOOK                __thread sHook;
#endif

        static LRESULT CALLBACK     MouseProc (int, WPARAM, LPARAM);
        static uint                 ActiveCount ();
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


#endif  // __OWLEXT_MOUSEHK_H

