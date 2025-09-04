//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// This file declares TMessageHook.  This class is used to encapsulate a
// WH_GETMESSAGE Windows Hook.
//
// Original code by Don Griffin; used with permission.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_MSGHOOK_H)
#define __OWLEXT_MSGHOOK_H

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

class OWLEXTCLASS TMessageHook{
    protected:
        TMessageHook ();
        ~TMessageHook ();

        void    ActivateMsgHook (bool = true);
        bool    IsMsgHookActive () const { return mActive; }

        //  Call the base class version if you want to let other objects see
        //  the event (and eventually call CallNextHookEx).
        virtual void    OnGetMsg (MSG &);   // PM_REMOVE
        virtual void    OnPeekMsg (MSG &);  // PM_NOREMOVE

    private:
        TMessageHook  * mNext;
        bool            mActive;

#if defined (BI_COMP_BORLANDC) && __BORLANDC__ <= 0x520
        static TMessageHook *       __thread sFirstHook;
        static HHOOK                __thread sHook;
#endif

        static LRESULT CALLBACK     GetMsgProc (int, WPARAM, LPARAM);
        static uint                 ActiveCount ();
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif  // __OWLEXT_MSGHOOK_H

