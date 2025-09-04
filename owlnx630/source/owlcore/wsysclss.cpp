//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:49 $
//
// Implementation of streaming for window system structure and type
// encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_OWLCLS_H)
#include <owl/wsyscls.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

//
// Streaming operators for resource Ids
//
_OWLCFUNC(ostream&)
operator <<(ostream& os, const TResId& id)
{
  bool  isNumeric = ToBool(!id.IsString());

  if (isNumeric)
    os << id.Num;
  else
    os << id.Str;
  return os;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
