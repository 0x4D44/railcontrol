//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:20 $
//
// Reliable include for standard C++ exception classes
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_EXCEPT_H)
# define OWL_PRIVATE_EXCEPT_H

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif

#if defined(BI_COMP_WATCOM)
#pragma read_only_file
#endif

//
// Provide throw specifier macros that allow conditionally disabling the use
// of throw specifiers based on the macro, BI_NO_THROWSPEC.
// This can be turned on for compilers that don't support it, or for users
// that dont want it.
//
# if defined(BI_NO_THROWSPEC)
#  define THROW_SPEC0
#  define THROW_SPEC1(x)
#  define THROW_SPEC2(x,x2)
#  define THROW_SPEC3(x,x2,x3)
#  define THROW_SPEC4(x,x2,x3,x4)
# else
#  define THROW_SPEC0                    throw()
#  define THROW_SPEC1(x)                 throw(x)
#  define THROW_SPEC2(x,x2)              throw(x,x2)
#  define THROW_SPEC3(x,x2,x3)           throw(x,x2,x3)
#  define THROW_SPEC4(x,x2,x3,x4)        throw(x,x2,x3,x4)
# endif

//
// In a Borland C++ environment, use the file provided
//
# if defined(BI_COMP_BORLANDC)

#  include <except.h>

#   define BI_XBASE_DEFINED
#   define BI_XALLOC_DEFINED

//
// In a non-Borland C++ environment, use the C++ Exception Handling support
// and add missing components of that support.
//
# else
#    if defined(BI_COMP_MSC)
#    include <eh.h>
#    endif

#    include <stdlib.h>

#    if defined(BI_STD_EXCEPTIONS)
#      include <new>
#      if !defined(BI_XALLOC_DEFINED) && !defined(__XALLOC)
#        define XALLOC std::bad_alloc
#        define __XALLOC
#      endif //!defined(BI_XALLOC_DEFINED) && !defined(__XALLOC)
#    else
#      if !defined(BI_XBASE_DEFINED)
#       error compiler specific must define XBASE
#      endif
#      if !defined(BI_XALLOC_DEFINED) && !defined(__XALLOC)
#        define XALLOC xalloc
#        define __XALLOC

#        if !defined(BI_STD_STRING)
          __OWL_DECLCLASS(string);
          typedef __OWL string owl_string;
#        endif //BI_STD_STRING

class _OWLCLASS xalloc : public XBASE {
  public:
    _OWLENTRY xalloc(const owl_string &msg, size_t size);
    _OWLENTRY xalloc();
    _OWLENTRY xalloc(const xalloc& x);
    virtual _OWLENTRY ~xalloc();
    void  _OWLENTRY raise() THROW_SPEC1(xalloc);
    LPCTSTR _OWLENTRY what() const THROW_SPEC1(xalloc);
    xalloc& _OWLENTRY operator=(const xalloc &src);

  private:
    owl_string str;
};
#      endif // !defined(BI_XALLOC_DEFINED) && !defined(__XALLOC)
#    endif  // BI_STD_EXCEPTIONS

#  endif  // if/else BI_COMP_BORLANDC

#endif  // OWL_PRIVATE_EXCEPT_H
