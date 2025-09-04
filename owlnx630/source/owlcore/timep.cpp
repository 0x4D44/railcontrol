//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// TTime class persistent streaming implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_TIME_H)
#include <owl/time.h>
#endif
#if !defined(OWL_OBJSTRM_H)
#include <owl/objstrm.h>
#endif
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif

__OWL_BEGIN_NAMESPACE

_OWLCFUNC(opstream  &) operator << ( opstream  & os,
                                            const TTime  & d )
{
    return os << d.Sec;
}

_OWLCFUNC(ipstream  &) operator >> ( ipstream  & is,
                                            TTime  & d )
{
    return is >> d.Sec;
}

//
_OWLCFUNC(opstream &) operator << ( opstream  & s, const TFileTime  & d ){
  s << d.dwLowDateTime << d.dwHighDateTime;
  return s;
}

//
_OWLCFUNC(ipstream &) operator >> ( ipstream  & s, TFileTime  & d )
{
  s >> d.dwLowDateTime >> d.dwHighDateTime;
  return s;
}

__OWL_END_NAMESPACE
/* ========================================================================== */


