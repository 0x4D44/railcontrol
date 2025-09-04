/*------------------------------------------------------------------------*/
/*                                                                        */
/*  REGEXP.H                                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/

/*
 *      C/C++ Run Time Library - Version 7.0
 *
 *      Copyright (c) 1987, 1996 by Borland International
 *      All Rights Reserved.
 *
 */


#ifndef __cplusplus
#error Must use C++ for REGEXP.H
#endif

#ifndef __REGEXP_H
#define __REGEXP_H


#if defined(__STDC__)
#if !defined(__GNUC__) //JJH added removal of pragma warn
#pragma warn -nak
#endif //__GNUC__
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


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  class TRegexp                                                         */
/*                                                                        */
/*  Implements regular expression searching                               */
/*                                                                        */
/*------------------------------------------------------------------------*/

class _OWLCLASS TRegexp {
  public:
    enum StatVal {
        OK=0,
        ILLEGAL,
        NOMEMORY,
        TOOLONG
    };

    _OWLENTRY TRegexp( const _TCHAR _FAR *cp );
    _OWLENTRY TRegexp( const TRegexp _FAR &r );
    _OWLENTRY ~TRegexp();

    TRegexp _FAR & _OWLENTRY operator = ( const TRegexp _FAR &r );
    TRegexp _FAR & _OWLENTRY operator = ( const _TCHAR _FAR *cp );
    size_t _OWLENTRY find( const owl_string _FAR &s,
                           size_t _FAR *len,
                           size_t start = 0 ) const;
    StatVal _OWLENTRY status() throw();

private:

    void _OWLENTRY copy_pattern( const TRegexp _FAR &r );
    void _OWLENTRY gen_pattern( const _TCHAR _FAR *cp );

    _TUCHAR _FAR *the_pattern;
    StatVal stat;
    static const unsigned maxpat;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

#if defined(__STDC__)
#if !defined(__GNUC__) //JJH added removal of pragma warn
#pragma warn .nak
#endif //__GNUC__
#endif

#endif /* __REGEXP_H */
