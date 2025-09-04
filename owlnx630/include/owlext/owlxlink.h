//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1998 by Bidus Yura.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_OWLXLINKS_H)
#define __OWLEXT_OWLXLINKS_H

#if defined(__BORLANDC__) && __BORLANDC__ < 0x530
# if __BORLANDC__ < 0x510
#  error Unsupported version
# elif __BORLANDC__ < 0x520

#  include <owlext/owlxlnkb1.h>

# else

#  include <owlext/owlxlnkb2.h>

# endif

#else   //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530


#define _COMMENT_OWLEXTLIB_LIBRARY "owlext"

#define _COMMENT_OWLEXTLIB_DELIMITER "-"

#define _COMMENT_OWLEXTLIB_VERSION "630"

#if defined(__BORLANDC__)
#define _COMMENT_OWLEXTLIB_COMPILER "b"

#if (__BORLANDC__ >= 0x650)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "14"
#elif (__BORLANDC__ >= 0x640)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "13"
#elif (__BORLANDC__ >= 0x630)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "12"
#elif (__BORLANDC__ >= 0x620)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "11"
#elif (__BORLANDC__ >= 0x610)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "10"
#elif (__BORLANDC__ >= 0x590)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "9"
#elif (__BORLANDC__ >= 0x580)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "8"
#elif (__BORLANDC__ >= 0x570)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "7"
#elif (__BORLANDC__ >= 0x560)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "6"
#elif (__BORLANDC__ >= 0x550)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "5"
#elif (__BORLANDC__ >= 0x540)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "4"
#elif (__BORLANDC__ >= 0x530)
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "3"
#endif

#elif defined(_MSC_VER)	//#if defined(__BORLANDC__)

#define _COMMENT_OWLEXTLIB_COMPILER "v"

#if _MSC_VER >= 1700
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "11"
#elif _MSC_VER >= 1600
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "10"
#elif _MSC_VER >= 1500
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "9"
#elif _MSC_VER >= 1400
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "8"
#elif _MSC_VER >= 1300
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "7"
#elif _MSC_VER >= 1200
#define _COMMENT_OWLEXTLIB_COMPILER_VERSION "6"
#endif

#endif	//#if defined(__BORLANDC__)

#if (__DEBUG > 0)
#define _COMMENT_OWLEXTLIB_DEBUG "d"
#else
#define _COMMENT_OWLEXTLIB_DEBUG ""
#endif

#if defined(INC_VCL)
#define _COMMENT_OWLEXTLIB_THREADING "v"
#elif defined(__MT__) || defined(_MT)
#define _COMMENT_OWLEXTLIB_THREADING "t"
#else
#define _COMMENT_OWLEXTLIB_THREADING "s"
#endif

#if defined(USE_UNICOWS)
#define _COMMENT_OWLEXTLIB_CHARACTER "us"
#elif defined(UNICODE)
#define _COMMENT_OWLEXTLIB_CHARACTER "u"
#else
#define _COMMENT_OWLEXTLIB_CHARACTER ""
#endif

#if defined(OWL1_COMPAT)
#define _COMMENT_OWLEXTLIB_COMPAT "c1"
#elif defined(OWL2_COMPAT)
#define _COMMENT_OWLEXTLIB_COMPAT "c2"
#elif defined(OWL5_COMPAT)
#define _COMMENT_OWLEXTLIB_COMPAT "c5"
#else
#define _COMMENT_OWLEXTLIB_COMPAT ""
#endif

#if defined(_OWLDLL)
#define _COMMENT_OWLEXTLIB_IMPORT "i"
#else
#define _COMMENT_OWLEXTLIB_IMPORT ""
#endif

#pragma comment(lib, \
	_COMMENT_OWLEXTLIB_LIBRARY \
	_COMMENT_OWLEXTLIB_DELIMITER \
	_COMMENT_OWLEXTLIB_VERSION \
	_COMMENT_OWLEXTLIB_DELIMITER \
	_COMMENT_OWLEXTLIB_COMPILER \
	_COMMENT_OWLEXTLIB_COMPILER_VERSION \
	_COMMENT_OWLEXTLIB_DELIMITER \
	_COMMENT_OWLEXTLIB_DEBUG \
	_COMMENT_OWLEXTLIB_THREADING \
	_COMMENT_OWLEXTLIB_CHARACTER \
	_COMMENT_OWLEXTLIB_COMPAT \
	_COMMENT_OWLEXTLIB_IMPORT \
".lib")

#endif  //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530

#endif  // __OWLEXT_OWLXLINKS_H

