//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// Implementation of classes for windowing system geometry
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_GEOMETRY_H)
#include <owl/geometry.h>
#endif

__OWL_BEGIN_NAMESPACE

// Let the compiler know that the following template instances will be defined elsewhere.
//#pragma option -Jgx

//
// Calculates the integer square root of a 32bit signed long. Returns a 16bit
// signed. Is fairly fast, esp. compared to FP versions
//
_OWLFUNC(int16)
Sqrt(int32 val)
{
  if (val <= 0)
    return 0;     // Could throw a math exception?

  uint mask = 0x4000;   // Bit mask to shift right
  int best = 0;         // Best estimate so far

  for (; mask; mask >>= 1)
    if (long(((uint32)(long)best+mask)*((uint32)(long)best+mask)) <= val)
      best |= mask;

  return int16(best);
}

_OWLFUNC(int)
Sqrt(int64 val)
{
  if (val <= int64(0l))
    return 0;     // Could throw a math exception?

  uint mask = 0x40000000;   // Bit mask to shift right
  int64 best = 0l;         // Best estimate so far
  int64 temp;

  for (; mask; mask >>= 1) {
    temp = best | int64(mask);
    if (temp * temp <= val)
      best = temp;
  }
  return int32(best);
}
//
// Return the distance between the origin and the point.
//
int TPoint::Magnitude() const
{
  int64 x64 = x;
  int64 y64 = y;
  return Sqrt(x64 * x64 + y64 * y64);
}

//
// Return the distance between the origin and the point.
//
int TSize::Magnitude() const
{
  int64 x64 = cx;
  int64 y64 = cy;
  return Sqrt(x64 * x64 + y64 * y64);
}

//
// Normalize the rectangle so that left is less than right and
// top is less than bottom.
//
TRect&
TRect::Normalize()
{
  if (left > right)
    #ifdef __HPUX__
    swap(left, right);
    #else
    Swap(left, right);
    #endif
  if (top > bottom)
    #ifdef __HPUX__
    swap(top, bottom);
    #else
    Swap(top, bottom);
    #endif
  return *this;
}

//
// Move the rectangle so that the new top left point is (left+dx, top+dy) and
// the new bottom right point is (right+dx, bottom+dy).
//
TRect&
TRect::Offset(int dx, int dy)
{
  left += dx;
  top += dy;
  right += dx;
  bottom += dy;
  return *this;
}

//
// Inflate the rectangle so that new top left point is (left-dx, top-dy) and
// the new bottom right point is (right+dx, bottom+dy).
//
TRect&
TRect::Inflate(int dx, int dy)
{
  left -= dx;
  top -= dy;
  right += dx;
  bottom += dy;
  return *this;
}
//
// Inflate the rectangle so that new top left point is (left-dx, top-dy) and
// the new bottom right point is (right+dx, bottom+dy).
//
TRect&
TRect::DeflateRect(int dx, int dy)
{
  return Inflate(-dx,-dy);
}

//
// Return the largest rectangle that is common to both rectangles.
//
TRect&
TRect::operator &=(const TRect& other)
{
  if (!IsNull()) {
    if (other.IsNull())
      SetNull();
    else {
      left = __max(left, other.left);
      top = __max(top, other.top);
      right = __min(right, other.right);
      bottom = __min(bottom, other.bottom);
    }
  }
  return *this;
}

//
// Return the largest rectangle that contains both rectangles.
//
TRect&
TRect::operator |=(const TRect& other)
{
  if (!other.IsNull()) {
    if (IsNull())
      *this = other;
    else {
      left = __min(left, other.left);
      top = __min(top, other.top);
      right = __max(right, other.right);
      bottom = __max(bottom, other.bottom);
    }
  }
  return *this;
}

//
// Deter::mines the parts of this rect that do not lie within "other"
//
// Resulting rectangles are placed in the "result" array.
//
// Returns the resulting number of rectangles which will be in the
// range "0 .. 4" inclusive
//
int
TRect::Subtract(const TRect& other, TRect result[]) const
{
  // Case of non-intersection, result is just this rectangle
  //
  if (!Touches(other)) {
    result[0] = *this;
    return 1;
  }
  // Check for up to four sub-rectangles produced
  //
  else {
    int  i = 0;

    // Top piece of this rect
    //
    if (other.top > top) {
      result[i].left = left;
      result[i].top = top;
      result[i].right = right;
      result[i].bottom = other.top;
      i++;
    }

    // Bottom piece of this rect
    //
    if (other.bottom < bottom) {
      result[i].left = left;
      result[i].top = other.bottom;
      result[i].right = right;
      result[i].bottom = bottom;
      i++;
    }

    // Left piece of this rect
    //
    if (other.left > left) {
      result[i].left = left;
      result[i].top = __max(top, other.top);
      result[i].right = other.left;
      result[i].bottom = __min(bottom, other.bottom);
      i++;
    }

    // Right piece of this rect
    //
    if (other.right < right) {
      result[i].left = other.right;
      result[i].top = __max(top, other.top);
      result[i].right = right;
      result[i].bottom = __min(bottom, other.bottom);
      i++;
    }
    return i;
  }
}

__OWL_END_NAMESPACE

