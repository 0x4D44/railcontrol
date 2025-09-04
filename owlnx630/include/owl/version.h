//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
// 1998-2000 by Yura Bidus
//
//$Revision: 1.39.2.9 $
//$Author: jogybl $
//$Date: 2010-03-29 07:36:47 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_VERSION_H)
#define OWL_VERSION_H

// OWL Version Number (Major, Minor, Revision, Build)
// All other version numbers and strings should be defined in terms of this.
// NB! No leading zeroes in the numbers! (leading zero = octal)
//
#define _OWL_VERSION(v) v(6,30,15,6654)

// OWL File Version (major, minor, revision, build)
//
#define V_4WORD(major,minor,revision,build) major, minor, revision, build
#define OWLFileVersion _OWL_VERSION(V_4WORD)

// OWL Product version (major, minor, revision, build)
//
#define OWLProductVersion OWLFileVersion

// OWL File Version String ("major.minor.revision.build")
//
#define V_STRING(major,minor,revision,build) #major "." #minor "." #revision "." #build
#define OWLFileVersionString _OWL_VERSION(V_STRING)

// OWL Product Version String ("major.minor")
//
#define V_MAJORMINORSTRING(major,minor,revision,build) #major "." #minor
#define OWLProductVersionString _OWL_VERSION(V_MAJORMINORSTRING)

// 32-bit internal version number
// 0xMmNnRbbb, where M=Major/10, m=Major%10, N=Minor/10, n=minor%10, R=Revision, bbb=Build
// E.g. 0x06201193 (6.20.1.403)
//
#define V_16BIT(major,minor,revision,build) (((major/10) << 12) | ((major%10) << 8) | ((minor/10) << 4) | (minor%10))
#define V_32BIT(major,minor,revision,build) (((V_16BIT(major, minor, 0, 0)) << 16) | ((revision%10) << 12) | build)
#define OWLInternalVersion _OWL_VERSION(V_32BIT)
#define OWLVersion _OWL_VERSION(V_16BIT)

#endif  //  OWL_VERSION_H
