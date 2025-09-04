//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:12 $
//
// Definition of a bitmap Cel array class.
//----------------------------------------------------------------------------

#if !defined(OWL_CELARRAY_H)
#define OWL_CELARRAY_H

#if !defined(OWL_GDIOBJCT_H)
# include <owl/gdiobjec.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


//
// class TCelArray
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TCelArray {
  public:
    // Constructors
    //
    TCelArray(TBitmap* bmp, int numCels, TSize celSize = 0,
              TPoint offset = 0, int numRows = 1, TAutoDelete = AutoDelete);
    TCelArray(const TDib& dib, int numCels, int numRows=1);
    TCelArray(const TCelArray& src);
    TCelArray(const TSize& size, uint flags, int init, int grow, int numRows = 1);

    virtual   ~TCelArray();

    TCelArray& operator =(const TCelArray&);
    operator   const TBitmap&() const;
    operator   TBitmap&();
//! #if !defined(OWL_NATIVECTRL_NEVER)
//!    operator HIMAGELIST() const;
//! #endif

    // 'Get' accessors
    //
    TPoint     Offset() const;
    int        NumCels() const;
    int        NumAllocCels() const;
    int        CurRow() const;
    int        NumRows() const;
    TSize      CelSize() const;
    TPoint     CelOffset(int cel) const;
    TRect      CelRect(int cel) const;
    TRect      operator [](int cel) const;

    // 'Set' accessors
    //!CQ not functional w/ ImageList.
    //
    void       SetNumCels(int numCels);
    void       SetCelSize(TSize size);
    void       SetOffset(TPoint offs);
    void       SetNGrowBy(int growBy);
    void       SetCurRow(int raw);

    // Manipulating cel images
    //
    int        Add(const TBitmap& image);
    int        Add(const TCelArray& src, int index);

    bool       Remove(int index = -1);
    bool       RemoveAll();
    bool       Replace(int index, const TBitmap& image);
    bool       Replace(int index, const TCelArray& src, int srcidx);
    TColor     GetBkColor() const;
    TColor     SetBkColor(const TColor&);

    bool       BitBlt(int index, TDC& dc, int x, int y, uint32 rop = SRCCOPY);
    bool       StretchBlt(int index, TDC& dc, const TRect& dstRect, uint32 rop = SRCCOPY);

    // not implemented functionality
    bool       BitBlt(int index, TDC&, int x, int y, int dx, int dy,
                      const TColor& bgClr, const TColor& fgClr);

  protected_data:
    TBitmap*    Bitmap;         // Main Cel bitmap
    bool        ShouldDelete;   // Does this CelArray own the Bitmap?
    TPoint      Offs;           // Offset within Bitmap of actual CelArray
    int         NCels;          // Number of cels allocated in Bitmap
    TSize       CSize;          // Size of one cel
    int         NGrowBy;        // How much to grow the array by when full
    TColor      BkColor;        // Background color used when image < cellsize
    int         NCelsUsed;      // Numbers of cels currently in use
    int         NRows;          // Number of rows existed in Bitmap
    int         NCurRow;        // Current raw in Bitmap

  private:
    TPoint      CelOffset(int cel, int row/*=-1*/) const;
    TRect       CelRect(int cel, int row/*=-1*/) const;
    bool        MaybeResize(int need);
    bool        Resize(int newCount);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


//------------------------------------------------------------------------
// Inline implementations
//

//
// Return the offset within the bitmap for the celarray.
//
inline TPoint TCelArray::Offset() const {
  return Offs;
}

//
// Return the size of the celarray.
//
inline TSize TCelArray::CelSize() const {
  return CSize;
}

//
//
//
inline TPoint TCelArray::CelOffset(int cel) const{
  return CelOffset(cel,-1);
}

//
//
//
inline TRect TCelArray::CelRect(int cel) const{
  return CelRect(cel,-1);
}

//
// Return number of cels currently in this CelArray
//
inline int TCelArray::NumCels() const {
  return NCelsUsed;
}

//
// Return number of cels allocated in this CelArray
//
inline int TCelArray::NumAllocCels() const{
  return NCels;
}

//
//
//
inline int TCelArray::CurRow() const {
  return NCurRow;
}

//
//
//
inline int TCelArray::NumRows() const {
  return NRows;
}

//
// Retrieve the 'cel'th image from the celarray.
//
inline TRect TCelArray::operator [](int cel) const {
  return CelRect(cel);
}

//
// Set the number of cels within the celarary.
//
inline void TCelArray::SetNumCels(int numCels) {
  NCels = numCels < 1 ? 1 : numCels;
}

//
//
//
inline void TCelArray::SetCurRow(int row){
  NCurRow  = row;
}

//
// Set the size of each cel.
// They are all assumed to be the same.
//
inline void TCelArray::SetCelSize(TSize size) {
  CSize = size;
}

//
// Set the increment by which the bitmap is resized
//
inline void TCelArray::SetNGrowBy(int growBy) {
  NGrowBy = growBy;
}

//
// Set the offset within a bitmap for the start of the celarray.
//
inline void TCelArray::SetOffset(TPoint offs) {
  Offs = offs;
}

//
// Remove all the cels from the array.
//
inline bool TCelArray::RemoveAll() {
  return Remove(-1);
}

//
// Return the image bitmap used by this CelArray
//
inline TCelArray::operator const TBitmap&() const  {
  return *Bitmap;
}

//
// Return the image bitmap used by this CelArray
//
inline TCelArray::operator TBitmap&() {
  return *Bitmap;
}

#if !defined(OWL_NATIVECTRL_NEVER)
//
//inline TCelArray::operator HIMAGELIST() const {
//  return ;
//}
#endif

//
// Get the current background color for this CelArray
//
inline TColor TCelArray::GetBkColor() const {
  return BkColor;
}

//
// Set the current background color for this CelArray, returning the previous
// color.
//
inline TColor TCelArray::SetBkColor(const TColor& color) {
  TColor obkColor = BkColor;
  BkColor = color;
  return obkColor;
}


__OWL_END_NAMESPACE

#endif  // OWL_CELARRAY_H
