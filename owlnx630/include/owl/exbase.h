//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:14 $
//
//   Base exception support for framework exceptions
//----------------------------------------------------------------------------

#if !defined(OWL_EXBASE_H)
#define OWL_EXBASE_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//----------------------------------------------------------------------------
// Exception support comes in two levels.
//
//  1) No emulation provided, any compiler support disabled. Throwing causes
//     an abort. (DISABLE_EXCEPTIONS defined)
//
//  2) Compiler support exceptions (DISABLE_EXCEPTIONS not defined)
//
// BI_NO_EXCEPTIONS comes from owl/compiler.h
// DISABLE_EXCEPTIONS comes from the outside
//
#if defined(DISABLE_EXCEPTIONS)  // (level 1 -- exceptions disabled)
# define TRY
# define CATCH(arg_and_code)
# define ENDCATCH
# define THROW(expr) abort()
# define RETHROW

#else                            // (level 2 -- compiler support required)
# include <owl/private/except.h>

# define TRY try
# define CATCH(arg_and_code)  catch arg_and_code
# define ENDCATCH
# define THROW(expr) throw expr
# define RETHROW throw
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//----------------------------------------------------------------------------
//
// Derived exception class that supports cloning, rethrowing & instance
// counting
//

class _OWLCLASS TXBase : public XBASE {
  public:
    TXBase(const owl_string& msg);
    TXBase(const TXBase& src);
#if defined(BI_STD_EXCEPTIONS)
    virtual ~TXBase() THROW_SPEC0;
#else
    virtual ~TXBase();
#endif

#if defined(BI_STD_EXCEPTIONS)
    const owl_string & why() const { return str; }
#endif

#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
    const char * _OWLENTRY what() const THROW_SPEC0;
    TXBase& _RTLENTRY operator=(const TXBase _FAR &src);
#endif
    virtual TXBase* Clone();
    virtual void Throw();

    static void Raise(const owl_string& msg);

  protected:
    static int InstanceCount;
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
    owl_string str;
#endif
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE


#endif  // OWL_EXBASE_H
