//----------------------------------------------------------------------------
// Object Windows Library
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// I/O stream defines
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#define OWL_PRIVATE_STRMDEFS_H

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
#pragma read_only_file
#endif

#if !defined(BI_STD_IO)

#include <iostream.h>

# if defined(UNICODE)
#    error Need unicode stream
#  else
#    if !defined(BI_USE_LONG_HNAMES)
#      include <strstrea.h>
#    else
#      include <strstream.h>
#    endif
#  include <fstream.h>
    typedef ostrstream  _tostrstream;
    typedef istrstream  _tistrstream;
    typedef strstream    _tstrstream;
    typedef istream      _tistream;
    typedef ostream      _tostream;
    typedef streambuf    _tstreambuf;
    typedef ios          _tios;

	typedef ifstream _tifstream;
	typedef ofstream _tofstream;

#  endif
#else // BI_STD_IO

#include <iomanip>
#include <iosfwd>

# if defined(UNICODE)
#   if defined(BI_STD_NEWSSTREAM)
#      include <sstream>
      typedef std::wostringstream  _tostrstream;
      typedef std::wistringstream  _tistrstream;
      typedef std::wstringstream  _tstrstream;
#   else
#      include <owl/private/wstrstream>
      typedef std::wostrstream  _tostrstream;
      typedef std::wistrstream  _tistrstream;
      typedef std::wstrstream    _tstrstream;
#   endif
    typedef std::wistream      _tistream;
    typedef std::wostream      _tostream;
    typedef std::wstreambuf    _tstreambuf;
    typedef std::wios          _tios;

	typedef std::wifstream _tifstream;
    typedef std::wofstream _tofstream;

#  else // # if defined(UNICODE) 
#if defined(BI_STD_NEWSSTREAM)
#    include <sstream>
    typedef std::ostringstream  _tostrstream;
    typedef std::istringstream  _tistrstream;
    typedef std::stringstream    _tstrstream;
#else   // # if defined(UNICODE)
#    include <strstream>
    typedef std::ostrstream    _tostrstream;
    typedef std::istrstream    _tistrstream;
    typedef std::strstream    _tstrstream;
#endif
    typedef std::istream      _tistream;
    typedef std::ostream      _tostream;
    typedef std::streambuf    _tstreambuf;
    typedef std::ios          _tios;
    typedef std::ifstream _tifstream;
    typedef std::ofstream _tofstream;

// SIR June 19th 2007
// See "using namespace std" directive in OBJSTRM.H ! These using
// declarations were not included in OWL 6.13 and it worked just fine.
// Disable these using declaration for OWL user code to work with MSVC6.
// With these enabled, MSVC6 complains about overloaded functions
// when seeing undecorated version of manipulator e.g. setw .
// Decorating manipulators e.g. std::setw (TIMEIO.CPP) is cumbersome. Also,
// these using declarations preclude having STLport map and manipulators
// (decorated or not) in same translation unit.
#if !defined(_MSC_VER) || _MSC_VER > 1200
//  using std::ends;
//  using std::endl;
//  using std::setfill;
//  using std::setw;
//  using std::ios_base;
//  using std::streamsize;
//  using std::hex;
//  using std::dec;
#endif // #if !defined(_MSC_VER)

#  endif  // # if defined(UNICODE)
#endif  // BI_STD_IO


#endif //OWL_PRIVATE_STRMDEFS_H
