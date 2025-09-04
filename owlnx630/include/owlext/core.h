//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// OWLEXT's "base" includes; portions by Don Griffin and used with permission.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_CORE_H)
#define __OWLEXT_CORE_H

// FIRST of all include OWL core definitions
// Grab the "core" OWL classes that we use over and over again
//
#if !defined (OWL_OWLCORE_H)
# define OEMRESOURCE            // needed to load scrollbar bitmaps
# include <owl/owlcore.h>
#endif

// Grab the macro magic for DLLs
//
#if !defined (__OWLEXT_DEFS_H)
# include <owlext/defs.h>
#endif

// Grab the ANSI-C/C++ headers we'll need over and over again; some of the "core"
// OWLEXT classes may depend on this stuff, so we must include it before those
// class header files
//
#if !defined (__STRING_H)
# include <string.h>
#endif
#if !defined(BI_STD_RTL)
#	if !defined (__IOSTREAM_H)
#		include <iostream.h>
#	endif
#	if !defined (__FSTREAM_H)
#		include <fstream.h>
#	endif
#endif

// Some utility classes; may be used in the core classes, so include before the
// core class header files
//
#if !defined (__OWLEXT_RCPTR_H)
# include <owlext/rcptr.h>
#endif
#if !defined (__OWL_PROPERTY_H)
#define __OWLEXT_PROPERTY_H
# include <owl/property.h>
#endif

// ASSERT/VERIFY and TRACE stuff:
//
// Note--many of Don Griffin's classes made EXTENSIVE use of the macro system
// found in this header (and associated CPP file, which is included in the CD
// but not as part of this project); I stripped them out primarily so that all
// classes would use the OWL facilities, as well as to prevent confusion on the
// part of readers trying to wade through the source code. Look for future
// versions of OWLEXT to use a "unified" version of OWL's and Don's macro system.
// However, this file also includes several useful macros (such as VALIDPTR)
// that aren't already covered by OWL's macros, so I include this header. (Some
// may be wondering how I can include the header and not the .CPP file--
// remember that if a function is declared, not defined but never *used*, it's
// not any kind of error, just more work for the compiler.)
//
#ifndef __OWLEXT_DEBUGEX_H
# include <owlext/debugex.h>
#endif



//  16/32 Compatibility Types and Macros
//
/*
#if defined(BI_PLAT_WIN32)
  #define  IF16(X)
  #define  IF32(X)     X
#else
  #define  IF16(X)     X
  #define  IF32(X)
#endif
#define IF16E32(X,Y)    IF16(X) IF32(Y)

//  Macro Helpers:
//
#ifndef ARRAYCOUNT
  #define ARRAYCOUNT(A)       (sizeof(A) / sizeof(A[0]))
#endif
*/

#define OBJZERO(object)     memset (&object, 0, sizeof(object))

////////////////////////////////////////////////////////////////////////////
//  These macros are used for declaring signature templates for OWL dispatch
//  tables.  The signatures are used to ensure that methods provided to the
//  macros are of the proper form (ie, signature).  Eg:
//
//      DECLARE_DISPSIGNATURE1 (void, v_EnterPage_Sig, TEnterPageNotify &)
//
//  This creates a signature verification template called v_EnterPage_Sig
//  that is used like this:
//
//      #define  EV_TABN_ENTERPAGE(id,method)
//                  { nmEnterPage, id, (TAnyDispatcher) ::v_LPARAM_Dispatch,
//                    (TMyPMF) v_EnterPage_Sig (&TMyClass::method) }
//
//  All this will simply guarantee that "method" is of the form:
//
//          void Method (TEnterPageNotify &);
//
//  The number on the end of DECLARE_DISPSIGNATUREx is either 0, 1, 2, 3 or
//  4, depending on the number of arguments the method takes.
//
#define DECLARE_DISPSIGNATURE0(R,Name) \
    template <class T> \
    inline R _CALLCNVN (T::*Name(R _CALLCNVN (T::*pmf)()))() \
        { return pmf; }

#define DECLARE_DISPSIGNATURE1(R,Name,A) \
    template <class T> \
    inline R _CALLCNVN (T::*Name(R _CALLCNVN (T::*pmf)(A)))(A) \
        { return pmf; }

#define DECLARE_DISPSIGNATURE2(R,Name,A,B) \
    template <class T> \
    inline R _CALLCNVN (T::*Name(R _CALLCNVN (T::*pmf)(A,B)))(A,B) \
        { return pmf; }

#define DECLARE_DISPSIGNATURE3(R,Name,A,B,C) \
    template <class T> \
    inline R _CALLCNVN (T::*Name(R _CALLCNVN (T::*pmf)(A,B,C)))(A,B,C) \
        { return pmf; }

#define DECLARE_DISPSIGNATURE4(R,Name,A,B,C,D) \
    template <class T> \
    inline R _CALLCNVN (T::*Name(R _CALLCNVN (T::*pmf)(A,B,C,D)))(A,B,C,D) \
        { return pmf; }

#endif

