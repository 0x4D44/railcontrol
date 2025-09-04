//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:45 $
//
// Implementation of persistent streaming for window system structure and type
// encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_WSYSCLS_H)
#include <owl/wsyscls.h>
#endif

__OWL_BEGIN_NAMESPACE

//
// Persistent streaming operators for resource Ids
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TResId& id)
{
  bool  isNumeric;
  is >> isNumeric;

  if (isNumeric) {
    long  nid;
    is >> nid;
    id = int(nid);
  }
  else{
    _USES_CONVERSION;
    id = (const _TCHAR  *)_A2W(is.freadString());
  }
  return is;
}

_OWLCFUNC(opstream&)
operator <<(opstream& os, const TResId& id)
{
  bool  isNumeric = ToBool(!id.IsString());
  os << isNumeric;

  if (isNumeric)
    os << id.Num;

  else{
    _USES_CONVERSION;
    os.fwriteString(_W2A(id.Str));
  }
  return os;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
