//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:48 $
//
// Definition of DeviceIndependent Bitmap object class
//----------------------------------------------------------------------------

#if !defined(OWL_DIBITMAP_H)
#define OWL_DIBITMAP_H

#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

//FMM
#if defined(__HPUX_SOURCE)
#include <owl/pointer.h>
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TDiBitmap
// ~~~~~ ~~~~~~~~~
// The TDiBitmap class is a combination of a TDib and a TBitmap. It can be
// constructed and manipulated like a TDib, but it can also be selected into
// a dc and manipulated with GDI. To guarantee functionality on all platforms,
// the TDibDC should be used as the memory dc to hold a TDiBitmap
//
class _OWLCLASS TDiBitmap : public TBitmap, public TDib {
  public:
    // Constructors and destructor
    TDiBitmap(HGLOBAL handle, TAutoDelete autoDelete = NoAutoDelete);
    TDiBitmap(const TClipboard& clipboard);
    TDiBitmap(const TDib& src);

    TDiBitmap(int width, int height, int nColors, uint16 mode=DIB_RGB_COLORS);
    TDiBitmap(HINSTANCE module, TResId resid);
    TDiBitmap(LPCTSTR name);
    TDiBitmap(TFile& file, bool readFileHeader = true);
    TDiBitmap(NS_OWL_STREAMS::istream& is, bool readFileHeader = false);
    TDiBitmap(const TBitmap& bitmap, const TPalette* pal = 0);
   ~TDiBitmap();

    int         Width() const;
    int         Height() const;
    TSize       Size() const;
    int         BitsPixel() const;

  protected:
    void InitBitmap();
    //JJH - removed, because CheckValid wasn't defined
    #if !defined(STRIP_UNDEFINED)
    void CheckValid(uint resId = IDS_GDIFAILURE);
    #endif

  private:
    // Prevent accidental copying of object
    TDiBitmap& operator =(const TDiBitmap&);
    TDiBitmap(const TDiBitmap& src);
     TDiBitmap();

};

//
// class TIdentityPalette
// ~~~~~ ~~~~~~~~~~~~~~~~
// An identity palette is a palette where the colors and order of those colors
// match the physical palette.
// Use the identity palette whenever possible, because if the system does not
// have to map colors, it can speed bitmap drawing.
//
class _OWLCLASS TIdentityPalette : public TPalette {
  public:
    TIdentityPalette(const TPalette& palette);
};

//
// class THalftonePalette
// ~~~~~ ~~~~~~~~~~~~~~~~
// A halftone palette is intended for use when the stretching mode of a
// device context is set to HALFTONE.
//
class _OWLCLASS THalftonePalette : public TPalette {
  public:
    THalftonePalette();
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return the width of the dib.
//
inline int
TDiBitmap::Width() const
{
  return TDib::Width();
}

//
// Return the height of the dib.
//
inline int
TDiBitmap::Height() const
{
  return TDib::Height();
}

//
// Return the size of the dib.
//
inline TSize
TDiBitmap::Size() const
{
  return TDib::Size();
}

//
// Return number of bits to hold each pixel.
//
inline int
TDiBitmap::BitsPixel() const
{
  return TDib::BitsPixel();
}


__OWL_END_NAMESPACE


#endif  // OWL_DIBITMAP_H
