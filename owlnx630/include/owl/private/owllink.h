//----------------------------------------------------------------------------
// Object Windows Library
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.34.2.1 $
//$Author: jogybl $
//$Date: 2009-09-01 10:34:09 $
//
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_OWLLINK_H)
#define OWL_PRIVATE_OWLLINK_H

#if defined(__BORLANDC__) && __BORLANDC__ < 0x530
# if __BORLANDC__ < 0x510
#  error Unsupported version
# elif __BORLANDC__ < 0x520

#  include <owl/private/owllnkb1.h>

# else

#  include <owl/private/owllnkb2.h>

# endif

#else   //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530


#define _COMMENT_LIB_LIBRARY "owl"

#define _COMMENT_LIB_DELIMITER "-"

#define _COMMENT_LIB_VERSION "630"

#if defined(__BORLANDC__)
#define _COMMENT_LIB_COMPILER "b"

#if (__BORLANDC__ >= 0x650)
#define _COMMENT_LIB_COMPILER_VERSION "14"
#elif (__BORLANDC__ >= 0x640)
#define _COMMENT_LIB_COMPILER_VERSION "13"
#elif (__BORLANDC__ >= 0x630)
#define _COMMENT_LIB_COMPILER_VERSION "12"
#elif (__BORLANDC__ >= 0x620)
#define _COMMENT_LIB_COMPILER_VERSION "11"
#elif (__BORLANDC__ >= 0x610)
#define _COMMENT_LIB_COMPILER_VERSION "10"
#elif (__BORLANDC__ >= 0x590)
#define _COMMENT_LIB_COMPILER_VERSION "9"
#elif (__BORLANDC__ >= 0x580)
#define _COMMENT_LIB_COMPILER_VERSION "8"
#elif (__BORLANDC__ >= 0x570)
#define _COMMENT_LIB_COMPILER_VERSION "7"
#elif (__BORLANDC__ >= 0x560)
#define _COMMENT_LIB_COMPILER_VERSION "6"
#elif (__BORLANDC__ >= 0x550)
#define _COMMENT_LIB_COMPILER_VERSION "5"
#elif (__BORLANDC__ >= 0x540)
#define _COMMENT_LIB_COMPILER_VERSION "4"
#elif (__BORLANDC__ >= 0x530)
#define _COMMENT_LIB_COMPILER_VERSION "3"
#endif

#elif defined(_MSC_VER)	//#if defined(__BORLANDC__)

#define _COMMENT_LIB_COMPILER "v"

#if _MSC_VER >= 1700
#define _COMMENT_LIB_COMPILER_VERSION "11"
#elif _MSC_VER >= 1600
#define _COMMENT_LIB_COMPILER_VERSION "10"
#elif _MSC_VER >= 1500
#define _COMMENT_LIB_COMPILER_VERSION "9"
#elif _MSC_VER >= 1400
#define _COMMENT_LIB_COMPILER_VERSION "8"
#elif _MSC_VER >= 1300
#define _COMMENT_LIB_COMPILER_VERSION "7"
#elif _MSC_VER >= 1200
#define _COMMENT_LIB_COMPILER_VERSION "6"
#endif

#endif	//#if defined(__BORLANDC__)

#if (__DEBUG > 0)
#define _COMMENT_LIB_DEBUG "d"
#else
#define _COMMENT_LIB_DEBUG ""
#endif

#if defined(INC_VCL)
#define _COMMENT_LIB_THREADING "v"
#elif defined(__MT__) || defined(_MT)
#define _COMMENT_LIB_THREADING "t"
#else
#define _COMMENT_LIB_THREADING "s"
#endif

#if defined(USE_UNICOWS)
#define _COMMENT_LIB_CHARACTER "us"
#elif defined(UNICODE)
#define _COMMENT_LIB_CHARACTER "u"
#else
#define _COMMENT_LIB_CHARACTER ""
#endif

#if defined(OWL1_COMPAT)
#define _COMMENT_LIB_COMPAT "c1"
#elif defined(OWL2_COMPAT)
#define _COMMENT_LIB_COMPAT "c2"
#elif defined(OWL5_COMPAT)
#define _COMMENT_LIB_COMPAT "c5"
#else
#define _COMMENT_LIB_COMPAT ""
#endif

#if defined(_OWLDLL)
#define _COMMENT_LIB_IMPORT "i"
#else
#define _COMMENT_LIB_IMPORT ""
#endif

#pragma comment(lib, \
	_COMMENT_LIB_LIBRARY \
	_COMMENT_LIB_DELIMITER \
	_COMMENT_LIB_VERSION \
	_COMMENT_LIB_DELIMITER \
	_COMMENT_LIB_COMPILER \
	_COMMENT_LIB_COMPILER_VERSION \
	_COMMENT_LIB_DELIMITER \
	_COMMENT_LIB_DEBUG \
	_COMMENT_LIB_THREADING \
	_COMMENT_LIB_CHARACTER \
	_COMMENT_LIB_COMPAT \
	_COMMENT_LIB_IMPORT \
".lib")

#endif  //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530

#endif // OWL_PRIVATE_OWLLINK_H
