//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2010-03-24 11:54:27 $
//
// Definition of windowing system color classes
//----------------------------------------------------------------------------

#if !defined(OWL_COLOR_H)
#define OWL_COLOR_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(OWL_OBJSTRM_H)
# include <owl/objstrm.h>    // Need persist streaming classes & operators
#endif
#if defined(BI_PLAT_MSW)
# if !defined(OWL_WSYSINC_H)
#   include <owl/wsysinc.h>
# endif
#else
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//
// Simulate Windows structures
//

  typedef uint32 COLORREF;

  struct tagPALETTEENTRY {
    uint8    peRed;
    uint8    peGreen;
    uint8    peBlue;
    uint8    peFlags;
  };
# include <pshpack1.h>
  struct tagRGBTRIPLE {
    uint8    rgbtBlue;
    uint8    rgbtGreen;
    uint8    rgbtRed;
  };
# include <poppack.h>
  struct tagRGBQUAD {
    uint8    rgbBlue;
    uint8    rgbGreen;
    uint8    rgbRed;
    uint8    rgbReserved;
};

#endif

//Hue/Lumination/Saturation Color definition, thanks to Luigi Bianchi
///////////////////////////////////////////////////////////////////////////////
typedef DWORD HLSCOLOR;
#define HLS(h,l,s) ((HLSCOLOR)(((BYTE)(h)|((WORD)((BYTE)(l))<<8))|(((DWORD)(BYTE)(s))<<16)))

///////////////////////////////////////////////////////////////////////////////
#define HLS_H(hls) ((BYTE)(hls))
#define HLS_L(hls) ((BYTE)(((WORD)(hls)) >> 8))
#define HLS_S(hls) ((BYTE)((hls)>>16))



__OWL_BEGIN_NAMESPACE

//
// Functions to convert number of bits to number of palette colors and back,
// and build an RGB COLORREF
//
_OWLFUNC(long)  NColors(uint16 bitCount);
_OWLFUNC(uint16) NBits(long colors);

//
// Convert the red, green, and blue components into a COLORREF.
//
inline COLORREF MkRGB(uint r, uint g, uint b)
{
  return COLORREF((r & 0xFF) |
                  ((g & 0xFF) << 8) |
                  ((uint32(b) & 0xFF) << 16)
                 );
}

//  125 predefined COLORREF color definitions for OWL programs.
//  Comments : Many of these colors were converted from the 
//             color definitions in the POVRAY 2.2 include file 
//             COLORS.INC. Thanks POV-Team!
//  Author   : Rob McGregor, rob_mcgregor@compuserve.com

//
// Define RGB macros for various colors 
//

// A
#define CLR_AQUAMARINE            RGB(112, 219, 147)

// B
#define CLR_BAKERSCHOC            RGB(92, 51, 23)
#define CLR_BLACK                 RGB(0, 0, 0)
#define CLR_BLUE                  RGB(0, 0, 255)
#define CLR_BLUEVIOLET            RGB(159, 95, 159)
#define CLR_BRIGHTGOLD            RGB(217, 217, 26)
#define CLR_BRONZE1               RGB(140, 120, 36)
#define CLR_BRONZE2               RGB(166, 125, 61)
#define CLR_BROWN                 RGB(165, 42, 42)

// C
#define CLR_CADETBLUE             RGB(95, 159, 159)
#define CLR_CLEAR                 RGB(255, 255, 255)
#define CLR_COOLCOPPER            RGB(217, 135, 26)
#define CLR_COPPER                RGB(184, 115, 51)
#define CLR_CORAL                 RGB(255, 127, 0)
#define CLR_CORNFLOWERBLUE        RGB(66, 66, 111)
#define CLR_CYAN                  RGB(0, 255, 255)

// D
#define CLR_DARKBROWN             RGB(92, 64, 51)
#define CLR_DARKGREEN             RGB(47, 79, 47)
#define CLR_DARKOLIVEGREEN        RGB(79, 79, 47)
#define CLR_DARKORCHID            RGB(153, 50, 204)
#define CLR_DARKPURPLE            RGB(135, 31, 120)
#define CLR_DARKSLATEBLUE         RGB(107, 35, 142)
#define CLR_DARKSLATEGRAY         RGB(47, 79, 79)
#define CLR_DARKSLATEGREY         RGB(47, 79, 79)
#define CLR_DARKTAN               RGB(150, 105, 79)
#define CLR_DARKTURQUOISE         RGB(112, 147, 219)
#define CLR_DARKWOOD              RGB(133, 94, 66)
#define CLR_DIMGRAY               RGB(84, 84, 84)
#define CLR_DIMGREY               RGB(84, 84, 84)
#define CLR_DKGREENCOPPER         RGB(74, 117, 110)
#define CLR_DUSTYROSE             RGB(133, 99, 99)

// F
#define CLR_FELDSPAR              RGB(209, 145, 117)
#define CLR_FIREBRICK             RGB(142, 35, 35)
#define CLR_FLESH                 RGB(245, 204, 176)
#define CLR_FORESTGREEN           RGB(35, 142, 35)

// G
#define CLR_GOLD                  RGB(204, 127, 50)
#define CLR_GOLDENROD             RGB(219, 219, 112)
#define CLR_GRAY                  RGB(192, 192, 192)
#define CLR_GRAY05                RGB(13, 13, 13)
#define CLR_GRAY10                RGB(26, 26, 26)
#define CLR_GRAY15                RGB(38, 38, 38)
#define CLR_GRAY20                RGB(51, 51, 51)
#define CLR_GRAY25                RGB(64, 64, 64)
#define CLR_GRAY30                RGB(77, 77, 77)
#define CLR_GRAY35                RGB(89, 89, 89)
#define CLR_GRAY40                RGB(102, 102, 102)
#define CLR_GRAY45                RGB(115, 115, 115)
#define CLR_GRAY50                RGB(128, 128, 128)
#define CLR_GRAY55                RGB(140, 140, 140)
#define CLR_GRAY60                RGB(153, 153, 153)
#define CLR_GRAY65                RGB(166, 166, 166)
#define CLR_GRAY70                RGB(178, 178, 178)
#define CLR_GRAY75                RGB(191, 191, 191)
#define CLR_GRAY80                RGB(204, 204, 204)
#define CLR_GRAY85                RGB(217, 217, 217)
#define CLR_GRAY90                RGB(229, 229, 229)
#define CLR_GRAY95                RGB(242, 242, 242)
#define CLR_GREEN                 RGB(0, 255, 0)
#define CLR_GREENCOPPER           RGB(82, 125, 117)
#define CLR_GREENYELLOW           RGB(147, 219, 112)
#define CLR_GREY                  RGB(192, 192, 192)

// H
#define CLR_HUNTERSGREEN          RGB(33, 94, 79)

// I
#define CLR_INDIANRED             RGB(79, 47, 47)

// K
#define CLR_KHAKI                 RGB(159, 159, 95)

// L
#define CLR_LIGHTBLUE             RGB(191, 216, 216)
#define CLR_LIGHTGRAY             RGB(168, 168, 168)
#define CLR_LIGHTGREY             RGB(168, 168, 168)
#define CLR_LIGHTSTEELBLUE        RGB(143, 143, 188)
#define CLR_LIGHTWOOD             RGB(232, 194, 166)
#define CLR_LIMEGREEN             RGB(50, 204, 50)

// M
#define CLR_MAGENTA               RGB(255, 0, 255)
#define CLR_MANDARINORANGE        RGB(227, 120, 51)
#define CLR_MAROON                RGB(142, 35, 107)
#define CLR_MEDIUMAQUAMARINE      RGB(50, 204, 153)
#define CLR_MEDIUMBLUE            RGB(50, 50, 204)
#define CLR_MEDIUMFORESTGREEN     RGB(107, 142, 35)
#define CLR_MEDIUMGOLDENROD       RGB(234, 234, 173)
#define CLR_MEDIUMORCHID          RGB(147, 112, 219)
#define CLR_MEDIUMSEAGREEN        RGB(66, 111, 66)
#define CLR_MEDIUMSLATEBLUE       RGB(127, 0, 255)
#define CLR_MEDIUMSPRINGGREEN     RGB(127, 255, 0)
#define CLR_MEDIUMTURQUOISE       RGB(112, 219, 219)
#define CLR_MEDIUMVIOLETRED       RGB(219, 112, 147)
#define CLR_MEDIUMWOOD            RGB(166, 128, 99)
#define CLR_MICA                  RGB(0, 0, 0)
#define CLR_MIDNIGHTBLUE          RGB(47, 47, 79)

// N
#define CLR_NAVY                  RGB(35, 35, 142)
#define CLR_NAVYBLUE              RGB(35, 35, 142)
#define CLR_NEONBLUE              RGB(77, 77, 255)
#define CLR_NEONPINK              RGB(255, 110, 199)
#define CLR_NEWMIDNIGHTBLUE       RGB(0, 0, 156)
#define CLR_NEWTAN                RGB(235, 199, 158)

// O
#define CLR_OFFWHITE              RGB(240, 240, 240)
#define CLR_ORANGE                RGB(255, 128, 0)
#define CLR_ORANGERED             RGB(255, 0, 127)
#define CLR_ORCHID                RGB(219, 112, 219)

// P
#define CLR_PALEGREEN             RGB(143, 188, 143)
#define CLR_PINK                  RGB(188, 143, 143)
#define CLR_PLUM                  RGB(234, 173, 234)

// Q
#define CLR_QUARTZ                RGB(217, 217, 242)

// R
#define CLR_RED                   RGB(255, 0, 0)
#define CLR_RICHBLUE              RGB(89, 89, 171)

// S
#define CLR_SALMON                RGB(111, 66, 66)
#define CLR_SCARLET               RGB(140, 23, 23)
#define CLR_SEAGREEN              RGB(35, 142, 107)
#define CLR_SEMISWEETCHOC         RGB(107, 66, 38)
#define CLR_SIENNA                RGB(142, 107, 35)
#define CLR_SILVER                RGB(229, 232, 250)
#define CLR_SKYBLUE               RGB(50, 153, 204)
#define CLR_SLATEBLUE             RGB(0, 127, 255)
#define CLR_SPICYPINK             RGB(255, 28, 173)
#define CLR_SPRINGGREEN           RGB(0, 255, 127)
#define CLR_STEELBLUE             RGB(35, 107, 142)
#define CLR_SUMMERSKY             RGB(56, 176, 222)

// T
#define CLR_TAN                   RGB(219, 147, 112)
#define CLR_THISTLE               RGB(216, 191, 216)
#define CLR_TURQUOISE             RGB(173, 234, 234)

// V
#define CLR_VERYDARKBROWN         RGB(89, 41, 36)
#define CLR_VIOLET                RGB(79, 47, 79)
#define CLR_VIOLETRED             RGB(204, 50, 153)
#define CLR_VLIGHTGREY            RGB(204, 204, 204)

// W
#define CLR_WHEAT                 RGB(216, 216, 191)
#define CLR_WHITE                 RGB(255, 255, 255)

// Y
#define CLR_YELLOW                RGB(255, 255, 0)
#define CLR_YELLOWGREEN           RGB(153, 204, 50)

///////////////////////////////////////////////////////////////////
class _OWLCLASS TColor;

//
// forward definitions
__OWL ipstream& operator >>(__OWL ipstream& is, TColor& c);
__OWL opstream& operator <<(__OWL opstream& os, const TColor& c);

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TColor
// ~~~~~ ~~~~~~
// Class wrapper for management of color values.
// Internally, all colors are stored as a COLORREF.
//
class _OWLCLASS TColor {
  public:
    // Constructors
    //
    TColor();
    TColor(const TColor& src);

    TColor(COLORREF value);
    TColor(long value);

    TColor(int r, int g, int b);
    TColor(int r, int g, int b, int f);
    TColor(int index);

    TColor(const PALETTEENTRY & pe);
    TColor(const RGBQUAD & q);
    TColor(const RGBTRIPLE & t);

    // Conversion operator & value accessor
    //
    operator      COLORREF() const;
    COLORREF      GetValue() const;
    void          SetValue(const COLORREF& value);

    // Comparison operators
    //
    bool operator ==(const TColor& other) const;
    bool operator !=(const TColor& other) const;
    bool operator ==(COLORREF cr) const;
    bool operator !=(COLORREF cr) const;

    TColor& operator =(const TColor& src);

    // Accessors
    //
    int           Index() const;
    TColor        Rgb() const;
    TColor        PalIndex() const;
    TColor        PalRelative() const;

    uint8         Red() const;
    uint8         Green() const;
    uint8         Blue() const;
    uint8         Flags() const;
    bool          IsSysColor() const;
    bool          IsSpecialColor() const;

    // Additional methods
    //
    void Lighten(uint8 val);
    void Darken(uint8 val);
    void Merge(const TColor& other);

    //HLS conversion methods
    HLSCOLOR Rgb2Hls() const;
    void Hls2Rgb(HLSCOLOR hls);
    void HlsTransform(int percent_L, int percent_S);

    // Common RGB color values
    //
    static const TColor  Black;
    static const TColor  LtGray;
    static const TColor  Gray;
    static const TColor  LtRed;
    static const TColor  LtGreen;
    static const TColor  LtYellow;
    static const TColor  LtBlue;
    static const TColor  LtMagenta;
    static const TColor  LtCyan;
    static const TColor  White;

    // Special color values
    //
    static const TColor  None;         // not-a-color
    static const TColor  Transparent;  // a non-painting color

    // Symbolic system color values. Perform GetSysColor() on conversion to
    // COLORREF
    //
    static const TColor  SysScrollbar;
    static const TColor  SysDesktop;
    static const TColor  SysActiveCaption;
    static const TColor  SysInactiveCaption;
    static const TColor  SysMenu;
    static const TColor  SysWindow;
    static const TColor  SysWindowFrame;
    static const TColor  SysMenuText;
    static const TColor  SysWindowText;
    static const TColor  SysCaptionText;
    static const TColor  SysActiveBorder;
    static const TColor  SysInactiveBorder;
    static const TColor  SysAppWorkspace;
    static const TColor  SysHighlight;
    static const TColor  SysHighlightText;
    static const TColor  Sys3dFace;
    static const TColor  Sys3dShadow;
    static const TColor  SysGrayText;
    static const TColor  SysBtnText;
    static const TColor  SysInactiveCaptionText;
    static const TColor  Sys3dHilight;
    static const TColor  Sys3dDkShadow;
    static const TColor  Sys3dLight;
    static const TColor  SysInfoText;
    static const TColor  SysInfoBk;

  private:
    COLORREF    Value;          // The 32bit color value type (not a struct)

  friend __OWL ipstream& operator >>(__OWL ipstream& is, TColor& c);
  friend __OWL opstream& operator <<(__OWL opstream& os, const TColor& c);
};


//
// class TPaletteEntry
// ~~~~~ ~~~~~~~~~~~~~
// Wrapper for Windows' PALETTEENTRY type
//
class TPaletteEntry : public tagPALETTEENTRY {
  public:
    TPaletteEntry(int r, int g, int b, int f = 0);
    TPaletteEntry(const TColor& c);

    bool operator ==(COLORREF cr) const;
};


//
// class TRgbQuad
// ~~~~~ ~~~~~~~~
// Wrapper for Windows' RBGQUAD type
//
//JJH
#ifdef WINELIB
class TRgbQuad : public RGBQUAD {
#else
class TRgbQuad : public tagRGBQUAD {
#endif
  public:
    TRgbQuad(int r, int g, int b);
    TRgbQuad(const TColor& c);
    TRgbQuad(const RGBQUAD & q);

    bool operator ==(COLORREF cr) const;
};


//
// class TRgbTriple
// ~~~~~ ~~~~~~~~~~
// Wrapper for Windows' RBGTRIPLE type
//
//JJH
#ifdef WINELIB
class TRgbTriple : public RGBTRIPLE {
#else
class TRgbTriple : public tagRGBTRIPLE {
#endif
  public:
    TRgbTriple(int r, int g, int b);
    TRgbTriple(const TColor& c);
    TRgbTriple(const RGBTRIPLE & t);

    bool operator ==(COLORREF cr) const;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline Implementations
//

//
// Extract the color value from a persistent input stream.
//
inline __OWL ipstream& operator >>(__OWL ipstream& is, TColor& c)
{
  is >> c.Value;
  return is;
}

//
// Insert the color value into a persistent output stream.
//
inline __OWL opstream& operator <<(__OWL opstream& os, const TColor& c)
{
  os << c.Value;
  return os;
}


//
// Construct a null color.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TColor::TColor()
:
  Value(0)
{
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Construct a color as a copy of another color.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TColor::TColor(const TColor& src)
:
  Value(src.Value)
{
}

//
// Construct a color with an existing COLORREF value.
//
inline
TColor::TColor(COLORREF value)
:
  Value(value)
{
}

//
// Construct a color given a particular system value representation.
//
inline
TColor::TColor(long value)
:
  Value((COLORREF)value)
{
}

//
// Construct a color given the red, green, and blue components of color.
//
inline
TColor::TColor(int r, int g, int b)
:
  Value(MkRGB(r, g, b))
{
}

//
// Construct a color given red, green, blue, and flag components of color.
// The flag component is defined in the Windows SDK reference.
//
inline
TColor::TColor(int r, int g, int b, int f)
:
  Value(((uint32) f << 24) | MkRGB(r, g, b))
{
}

//
// Construct a color that is an index into a palette.
//
inline
TColor::TColor(int index)
:
  Value(PALETTEINDEX(index))
{
}

//
// Construct a color based on an existing PALETTEENTRY.
//
inline
TColor::TColor(const PALETTEENTRY & pe)
:
  Value(MkRGB(pe.peRed, pe.peGreen, pe.peBlue))
{
}

//
// Construct a color based on an existing RGBQUAD.
//
inline
TColor::TColor(const RGBQUAD & q)
:
  Value(MkRGB(q.rgbRed, q.rgbGreen, q.rgbBlue))
{
}

//
// Construct a color based on an existing RGBTRIPLE.
//
inline
TColor::TColor(const RGBTRIPLE & t)
:
  Value(MkRGB(t.rgbtRed, t.rgbtGreen, t.rgbtBlue))
{
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Convert the stored color into a COLORREF.
//
inline
TColor::operator COLORREF() const
{
  return GetValue();
}

//
// Return true if two colors are equal.
// This function compares between two binary representation of colors, it
// does not compare colors logically.
//
// For example, if palette entry 4 is solid red (rgb components (255, 0, 0)),
// The following will return false:
//   if (TColor(4) == TColor(255, 0, 0))
//     /* colors match */
//   else
//     /* colors do not match */
//
//  To actually compare the RGB values of two TColor objects, use the
//  operator == (COLORREF). For example,
//  TColor colorA, colorB;
//  if (colorA == colorB.GetValue()) {
//      // ColorA & ColorB
//  }
//
inline bool
TColor::operator ==(const TColor& other) const
{
  return Value == other.Value;
}

//
// Return true if two colors are not equal.
// See additional comments in TColor::operator ==.
//
inline bool
TColor::operator !=(const TColor& other) const
{
  return Value != other.Value;
}

//
// Return true if this color matches a COLORREF.
// See additional comments in TColor::operator ==.
//
inline bool
TColor::operator ==(COLORREF cr) const
{
  return GetValue() == cr;
}

//
// Return true if this color does not match a COLORREF.
// See additional comments in TColor::operator ==.
//
inline bool
TColor::operator !=(COLORREF cr) const
{
  return GetValue() != cr;
}

//
// Change the color after it has been constructed.
//
inline void
TColor::SetValue(const COLORREF& value)
{
  Value = value;
}

//
// Set the value of color after it has been constructed.
//
inline TColor&
TColor::operator =(const TColor& src)
{
  Value = src.Value;
  return *this;
}

//
// Return the index of the palette entry.
//
inline int
TColor::Index() const
{
  return (int)Value & 0xFFFF;
}

//
// Return the RGB color value.
//
inline TColor
TColor::Rgb() const
{
  return GetValue();
}

//
// Return the palette index.
// Assumes that the color really is a palette index
//
inline TColor
TColor::PalIndex() const
{
//  return (COLORREF)Index() | 0x01000000UL;
  return (COLORREF) ((COLORREF)Index() | 0x01000000UL);
}

//
// Return the palette relative entry.
//
inline TColor
TColor::PalRelative() const
{
//  return Rgb() | 0x02000000UL;
  return (COLORREF) ((COLORREF)Rgb() | 0x02000000UL);
}

//
// Return the flags entry.
//
inline uint8
TColor::Flags() const
{
  return (uint8)(uint16)(Value >> 24);
}

//
// Return true if the color is a system color.
//
inline bool
TColor::IsSysColor() const
{
  return (Value & 0xF0000000UL) == 0x80000000UL;
//DLN UNIX change because MainWin uses high byte too  return (Value & 0xFF000000UL) == 0x80000000UL;

}

//
// Return true if the color is a special color (currently None or Transparent)
//
inline bool
TColor::IsSpecialColor() const
{
  return (Value & 0xF0000000UL) == 0xF0000000UL;
}

//
// Construct a palette entry from the red, green, blue and flag components.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TPaletteEntry::TPaletteEntry(int r, int g, int b, int f)
{
  peRed = (uint8)(uint16)r;
  peGreen = (uint8)(uint16)g;
  peBlue = (uint8)(uint16)b;
  peFlags = (uint8)(uint16)f;
}

//
// Construct a palette entry from a TColor.
//
inline
TPaletteEntry::TPaletteEntry(const TColor& c)
{
  peRed = c.Red();
  peGreen = c.Green();
  peBlue = c.Blue();
  peFlags = c.Flags();
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Returns true if the palette entries have the same color components.
//
inline bool
TPaletteEntry::operator ==(COLORREF cr) const
{
  return peRed == GetRValue(cr) &&
         peGreen == GetGValue(cr) &&
         peBlue == GetBValue(cr);
}

//
// Construct an RGB quad from the red, green and blue components
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TRgbQuad::TRgbQuad(int r, int g, int b)
{
  rgbRed = (uint8)(uint16)r;
  rgbGreen = (uint8)(uint16)g;
  rgbBlue = (uint8)(uint16)b;
  rgbReserved = 0;
}

//
// Construct an RGB quad from a TColor.
//
inline
TRgbQuad::TRgbQuad(const TColor& c)
{
  rgbRed = c.Red();
  rgbGreen = c.Green();
  rgbBlue = c.Blue();
  rgbReserved = 0;
}

//
// Wrap an object around an existing RGBQUAD.
//
inline
TRgbQuad::TRgbQuad(const RGBQUAD & q)
{
  *(RGBQUAD*)this = q;
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Return true if the RGBQUAD has the same color components.
//
inline bool
TRgbQuad::operator ==(COLORREF cr) const
{
  return rgbRed == GetRValue(cr) &&
         rgbGreen == GetGValue(cr) &&
         rgbBlue == GetBValue(cr);
}

//
// Construct an RGB triple from its red, green, and blue components.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TRgbTriple::TRgbTriple(int r, int g, int b)
{
  rgbtRed = (uint8)(uint16)r;
  rgbtGreen = (uint8)(uint16)g;
  rgbtBlue = (uint8)(uint16)b;
}

//
// Construct an RGB triple from a TColor.
//
inline
TRgbTriple::TRgbTriple(const TColor& c)
{
  rgbtRed = c.Red();
  rgbtGreen = c.Green();
  rgbtBlue = c.Blue();
}

//
// Wrap an object around an existing RGBTRIPLE.
//
inline
TRgbTriple::TRgbTriple(const RGBTRIPLE & t)
{
  *(RGBTRIPLE*)this = t;
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Return true if the triple match color components.
//
inline bool
TRgbTriple::operator ==(COLORREF cr) const
{
  return rgbtRed == GetRValue(cr) &&
         rgbtGreen == GetGValue(cr) &&
         rgbtBlue == GetBValue(cr);
}

__OWL_END_NAMESPACE

#endif  // OWL_COLOR_H
