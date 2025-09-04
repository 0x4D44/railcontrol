//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// Implementation of persistent streaming for window system geometry classes
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif

//FMM
#if defined(__HPUX_SOURCE)
#include <owl/bitset.h>
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
// Extracts the rectangle from a regular input stream.
//
_OWLCFUNC(_tistream&)
operator >>(_tistream& is, TRect& r)
{
  _TCHAR ch;
  return is >> ch >> r.left >> ch >> r.top >> ch
            >> r.right >> ch >> r.bottom >> ch;
}

//
// Inserts the rectangle into a regular output stream.
//
_OWLCFUNC(_tostream&)
operator <<(_tostream& os, const TRect& r)
{
  return os << _T('(') << r.left << _T(',') << r.top << _T('-')
            << r.right << _T(',') << r.bottom << _T(')');
}

//----------------------------------------------------------------------------
// TPointL streaming
//

//
// Extract the point from a regular input stream.
//
_OWLCFUNC(_tistream&)
operator >>(_tistream& is, TPointL& p)
{
  _TCHAR c;

  is >> c;

  long x;
  is >> x;
  p.x = x;

  is >> c;

  long y;
  is >> y;
  p.y = y;

  is >> c;

  return is;
}

//
// Insert the point into an output stream.
//
_OWLCFUNC(_tostream&)
operator <<(_tostream& os, const TPointL& p)
{
  return os << _T('(') << p.x << _T(',') << p.y << _T(')');
}

//----------------------------------------------------------------------------
// TPointF streaming
//

//
// Extract a point from a regular input stream.
//
_OWLCFUNC(_tistream&)
operator >>(_tistream& is, TPointF& p)
{
  _TCHAR c;
  return is >> c >> p.x >> c >> p.y >> c;
}

//
// Insert a point into the output stream.
//
_OWLCFUNC(_tostream&)
operator <<(_tostream& os, const TPointF& p)
{
  return os << _T('(') << p.x << _T(',') << p.y << _T(')');
}


//----------------------------------------------------------------------------
// TPoint streaming
//

//
// Extract a point from an input stream.
//
_OWLCFUNC(_tistream&)
operator >>(_tistream& is, TPoint& p)
{
  _TCHAR c;
  return is >> c >> p.x >> c >> p.y >> c;
}

//
// Insert a point into an output stream.
//
_OWLCFUNC(_tostream&)
operator <<(_tostream& os, const TPoint& p)
{
  return os << _T('(') << p.x << _T(',') << p.y << _T(')');
}

//----------------------------------------------------------------------------
// TSize streaming
//

//
// Extract a size from a regular input stream.
//
_OWLCFUNC(_tistream&)
operator >>(_tistream& os, TSize& s)
{
  _TCHAR c;
  return os >> c >> s.cx >> c >> s.cy >> c;
}

//
// Insert a size into an output stream.
//
_OWLCFUNC(_tostream&)
operator <<(_tostream& os, const TSize& s)
{
  return os << _T('(') << s.cx << _T('x') << s.cy << _T(')');
}

__OWL_END_NAMESPACE

