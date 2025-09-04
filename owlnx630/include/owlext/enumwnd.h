//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//
// This file declares the TEnumWindows class.  It is encapsulates the
// EnumTaskWindows (16-bit) or EnumThreadWindows (32-bit) APIs.  It can
// also handle EnumChildWindows to visit every window for a task/thread.
//
// Original code by Don Griffin; used with permission.
//
//-------------------------------------------------------------------
#if !defined (__OWLEXT_ENUMWND_H)
#define __OWLEXT_ENUMWND_H

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

class OWLEXTCLASS TEnumWindows
{
    public:
        enum TEnumMethod { emTopLevelWindows, emAllWindows };

        void EnumWindows (TEnumMethod);

    protected:
        TEnumWindows ();

        virtual bool OnEnumWindow (HWND) = 0;

    private:
        bool                    mDoingChildWindows;

        bool                    WndEnumMethod   (HWND hWnd);
        static BOOL CALLBACK    WndEnumProc     (HWND hWnd, LPARAM lParam);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif  // __OWLEXT_ENUMWND_H

