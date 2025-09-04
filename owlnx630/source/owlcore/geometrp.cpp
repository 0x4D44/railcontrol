//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// Implementation of persistent streaming for window system structure and type
// encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(__HPUX_SOURCE)
#include <owl/bitset.h>
#include <owl/objstrm.h>
#else
#if !defined(OWL_GEOMETRY_H)
#include <owl/geometry.h>
#endif
#endif

__OWL_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// TRect streaming
//

//
// Extracts the rectangle from the persistent stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TRect& r)
{
  return is >> r.left >> r.top >> r.right >> r.bottom;
}

//
// Inserts the rectangle into a persistent stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TRect& r)
{
  return os << r.left << r.top << r.right << r.bottom;
}

//----------------------------------------------------------------------------
// TPointL streaming
//

//
// Extract the point from the input persistent stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TPointL& p)
{
  long x;
  is >> x;
  p.x = x;

  long y;
  is >> y;
  p.y = y;

  return is;
}

//
// Insert the point into an output persistent stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TPointL& p)
{
  return os << p.x << p.y;
}

//----------------------------------------------------------------------------
// TPointF streaming
//

//
// Extract a point from the persistent input stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TPointF& p)
{
  return is >> p.x >> p.y;
}

//
// Insert a point into the output persistent stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TPointF& p)
{
  return os << p.x << p.y;
}

//----------------------------------------------------------------------------
// TPoint streaming
//

//
// Extract a point from a persistent input stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TPoint& p)
{
  return is >> p.x >> p.y;
}

//
// Insert a point into a persistent output stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TPoint& p)
{
  return os << p.x << p.y;
}

//----------------------------------------------------------------------------
// TSize streaming
//

//
// Extract a size from a persistent input stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TSize& s)
{
  return is >> s.cx >> s.cy;
}

//
// Insert a size into a persistent output stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TSize& s)
{
  return os << s.cx << s.cy;
}


__OWL_END_NAMESPACE

