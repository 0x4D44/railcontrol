//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// TDate class persistant streaming implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/date.h>
#include <owl/objstrm.h>
//#if defined(BI_STD_RTL)
///#include <strstream>
//#else
//#include <strstrea.h>
//#endif

__OWL_BEGIN_NAMESPACE

_OWLCFUNC(opstream  &) operator << ( opstream  & os, const TDate  & d )
{
    return os << d.Julnum;
}

_OWLCFUNC(ipstream  &) operator >> ( ipstream  & is, TDate  & d )
{
    return is >> d.Julnum;
}

__OWL_END_NAMESPACE

