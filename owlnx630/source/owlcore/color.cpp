//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of color classes
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_COLOR_H)
#include <owl/color.h>
#endif
#if !defined(OWL_SYSTEM_H)
#include <owl/system.h>
#endif



__OWL_BEGIN_NAMESPACE

// Few constants only defined when WINVER >= 0x0400
//
#if !defined(COLOR_3DDKSHADOW)
# define COLOR_3DDKSHADOW        21
#endif
#if !defined(COLOR_3DLIGHT)
# define COLOR_3DLIGHT           22
#endif
#if !defined(COLOR_INFOTEXT)
# define COLOR_INFOTEXT          23
#endif
#if !defined(COLOR_INFOBK)
# define COLOR_INFOBK            24
#endif

const TColor TColor::Black(0, 0, 0);         // 0
const TColor TColor::LtGray(192, 192, 192);  // 7
const TColor TColor::Gray(128, 128, 128);    // 8
const TColor TColor::LtRed(255, 0, 0);       // 9
const TColor TColor::LtGreen(0, 255, 0);     // 10
const TColor TColor::LtYellow(255, 255, 0);  // 11
const TColor TColor::LtBlue(0, 0, 255);      // 12
const TColor TColor::LtMagenta(255, 0, 255); // 13
const TColor TColor::LtCyan(0, 255, 255);    // 14
const TColor TColor::White(255, 255, 255);   // 15

//
// Special marker colors using flag bit pattern. Value never really used.
// Value must not change for streaming compatibility w/ OWL's TWindow
//
const TColor TColor::None((COLORREF)0xFF000000l);
const TColor TColor::Transparent((COLORREF)0xFE000000l);

//
// Symbolic system colors looked up on evaluation or conversion
//
#define Symbolic 0x80000000UL
const TColor TColor::SysScrollbar          ((COLORREF) (Symbolic | COLOR_SCROLLBAR));
const TColor TColor::SysDesktop            ((COLORREF) (Symbolic | COLOR_BACKGROUND));
const TColor TColor::SysActiveCaption      ((COLORREF) (Symbolic | COLOR_ACTIVECAPTION));
const TColor TColor::SysInactiveCaption    ((COLORREF) (Symbolic | COLOR_INACTIVECAPTION));
const TColor TColor::SysMenu               ((COLORREF) (Symbolic | COLOR_MENU));
const TColor TColor::SysWindow             ((COLORREF) (Symbolic | COLOR_WINDOW));
const TColor TColor::SysWindowFrame        ((COLORREF) (Symbolic | COLOR_WINDOWFRAME));
const TColor TColor::SysMenuText           ((COLORREF) (Symbolic | COLOR_MENUTEXT));
const TColor TColor::SysWindowText         ((COLORREF) (Symbolic | COLOR_WINDOWTEXT));
const TColor TColor::SysCaptionText        ((COLORREF) (Symbolic | COLOR_CAPTIONTEXT));
const TColor TColor::SysActiveBorder       ((COLORREF) (Symbolic | COLOR_ACTIVEBORDER));
const TColor TColor::SysInactiveBorder     ((COLORREF) (Symbolic | COLOR_INACTIVEBORDER));
const TColor TColor::SysAppWorkspace       ((COLORREF) (Symbolic | COLOR_APPWORKSPACE));
const TColor TColor::SysHighlight          ((COLORREF) (Symbolic | COLOR_HIGHLIGHT));
const TColor TColor::SysHighlightText      ((COLORREF) (Symbolic | COLOR_HIGHLIGHTTEXT));
const TColor TColor::Sys3dFace             ((COLORREF) (Symbolic | COLOR_BTNFACE));
const TColor TColor::Sys3dShadow           ((COLORREF) (Symbolic | COLOR_BTNSHADOW));
const TColor TColor::SysGrayText           ((COLORREF) (Symbolic | COLOR_GRAYTEXT));
const TColor TColor::SysBtnText            ((COLORREF) (Symbolic | COLOR_BTNTEXT));

// NOTE: WINVER >= 0x030a
//
const TColor TColor::SysInactiveCaptionText((COLORREF) (Symbolic | COLOR_INACTIVECAPTIONTEXT));
const TColor TColor::Sys3dHilight          ((COLORREF) (Symbolic | COLOR_BTNHIGHLIGHT));

// NOTE: WINVER >= 0x0400
//
const TColor TColor::Sys3dDkShadow((COLORREF) (Symbolic | COLOR_3DDKSHADOW));
const TColor TColor::Sys3dLight   ((COLORREF) (Symbolic | COLOR_3DLIGHT));
const TColor TColor::SysInfoText  ((COLORREF) (Symbolic | COLOR_INFOTEXT));
const TColor TColor::SysInfoBk    ((COLORREF) (Symbolic | COLOR_INFOBK));

//
// Convert a bit count into a color count for color table use, verifying that
// the bit count is one that is supported by Windows, ie 1, 4, 8, 16, 24, 32.
//
// If the bit count is not supported, -1 is returned.
//
_OWLFUNC(long)
NColors(uint16 bitCount)
{
  if (bitCount == 1 || bitCount == 4 || bitCount == 8)
    return 1 << bitCount;
  if (bitCount == 16 || bitCount == 24 || bitCount == 32)
    return 0;
  return -1;
}

//
// Return the number of bits required to represent a given number of colors
//
_OWLFUNC(uint16)
NBits(long colors)
{
  if (colors <= 2)
    return 1;
  if (colors <= 16)
    return 4;
  if (colors <= 256)
    return 8;
  if (colors <= 65536L)
    return 16;
  if (colors <= 16777216L)
    return 24;
  return 32;
}

//
// Get a 32bit COLORREF type from this color object. Performs a
// GetSysColor() lookup if the object represents a symbolic sys-color index.
//
COLORREF
TColor::GetValue() const
{
#if defined(BI_PLAT_MSW) || defined(UNIX)  //  copied || defined(UNIX) from old unix owl (JAM 04-16-01)
  return IsSysColor() ? ::GetSysColor(Index()) : Value;
#else
  return Value;
#endif
}

// copied from old unix owl (JAM 04-16-01)
//DLN UNIX added MwGetTrueRGBValue conversion calls
#if defined(UNIX) && !defined(WINELIB)
#include <mainwin.h>
#endif


//
// Return the color's red component
//
uint8
TColor::Red() const
{
  return (uint8)(uint16)GetValue();
}

//
// Return the color's green component
//
uint8
TColor::Green() const
{
  return (uint8)(uint16)(((uint16)GetValue()) >> 8);
}

//
// Return the color's blue component
//
uint8
TColor::Blue() const
{
  return (uint8)(uint16)(GetValue()>>16);
}

////////////////////////////////////////////////////////////////////////////////

void
TColor::Lighten(uint8 val)
{
  uint r = Red();
  uint g = Green();
  uint b = Blue();

  r = (r > (uint)(255 - val)) ? 255 : r + val;
  g = (g > (uint)(255 - val)) ? 255 : g + val;
  b = (b > (uint)(255 - val)) ? 255 : b + val;

  Value = RGB(r, g, b);
}

void
TColor::Darken(uint8 val)
{
  uint r = Red();
  uint g = Green();
  uint b = Blue();

  r = (r < (uint)val) ? 0 : r - val;
  g = (g < (uint)val) ? 0 : g - val;
  b = (b < (uint)val) ? 0 : b - val;

  Value = RGB(r, g, b);
}

void
TColor::Merge(const TColor& other)
{
  uint8 r = (uint8)((Red() + other.Red()) / 2);
  uint8 g = (uint8)((Green() + other.Green()) / 2);
  uint8 b = (uint8)((Blue() + other.Blue()) / 2);

  Value = RGB(r, g, b);
}

////////////////////////////////////////////////////////////////////////////////

HLSCOLOR
TColor::Rgb2Hls() const
{
  uint8 minval = Red();
  uint8 maxval = Red();
  
  if (minval > Green())
    minval = Green();
  if (maxval < Green())
    maxval = Green();
  
  if (minval > Blue())
    minval = Blue();
  if (maxval < Blue())
    maxval = Blue();
  
  float mdiff  = float(maxval) - float(minval);
  float msum   = float(maxval) + float(minval);

  float luminance = msum / 510.0f;
  float saturation = 0.0f;
  float hue = 0.0f;

  if (maxval != minval)
  {
      float rnorm = (maxval - Red()) / mdiff;
      float gnorm = (maxval - Green()) / mdiff;
      float bnorm = (maxval - Blue()) / mdiff;

      saturation = (luminance <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

      if (Red() == maxval)
        hue = 60.0f * (6.0f + bnorm - gnorm);

      if (Green() == maxval)
        hue = 60.0f * (2.0f + rnorm - bnorm);
        
      if (Blue() == maxval)
        hue = 60.0f * (4.0f + gnorm - rnorm);

      if (hue > 360.0f)
        hue = hue - 360.0f;
  }
  
  return HLS ((hue * 255) / 360, luminance * 255, saturation * 255);
}

static uint8
_ToRgb(float rm1, float rm2, float rh)
{
  if (rh > 360.0f)
    rh -= 360.0f;
  else if (rh < 0.0f)
    rh += 360.0f;
 
  if (rh <  60.0f)
    rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
  else if (rh < 180.0f)
    rm1 = rm2;
  else if (rh < 240.0f)
    rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;      
                   
  return (BYTE)(rm1 * 255);
}

void
TColor::Hls2Rgb(HLSCOLOR hls)
{
  float hue        = ((int)HLS_H(hls)*360)/255.0f;
  float luminance  = HLS_L(hls)/255.0f;
  float saturation = HLS_S(hls)/255.0f;

  if (saturation == 0.0f)
  {
    SetValue(RGB(HLS_L(hls), HLS_L(hls), HLS_L(hls)));
    return;
  }
  
  float rm1, rm2;

  if (luminance <= 0.5f)
    rm2 = luminance + luminance * saturation;
  else
    rm2 = luminance + saturation - luminance * saturation;

  rm1 = 2.0f * luminance - rm2;
  BYTE red   = _ToRgb(rm1, rm2, hue + 120.0f);
  BYTE green = _ToRgb(rm1, rm2, hue);
  BYTE blue  = _ToRgb(rm1, rm2, hue - 120.0f);

  SetValue(RGB(red, green, blue));
}

void
TColor::HlsTransform(int percent_L, int percent_S)
{
  HLSCOLOR hls = Rgb2Hls();
  BYTE h = HLS_H(hls);
  BYTE l = HLS_L(hls);
  BYTE s = HLS_S(hls);

  if (percent_L > 0)
  {
    l = BYTE(l + ((255 - l) * percent_L) / 100);
  }
  else if (percent_L < 0)
  {
    l = BYTE((l * (100 + percent_L)) / 100);
  }

  if (percent_S > 0)
  {
    s = BYTE(s + ((255 - s) * percent_S) / 100);
  }
  else if (percent_S < 0)
  {
    s = BYTE((s * (100 + percent_S)) / 100);
  }

  Hls2Rgb(HLS(h, l, s));
}



__OWL_END_NAMESPACE
/* ========================================================================== */

