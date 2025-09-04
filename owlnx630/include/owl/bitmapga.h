//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:12 $
//
// Definition of class TBitmapGadget
// Definition of class TDynamicBitmapGadget
//----------------------------------------------------------------------------

#if !defined(OWL_BITMAPGA_H)
#define OWL_BITMAPGA_H

#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
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

class _OWLCLASS TCelArray;

//
// class TBitmapGadget
// ~~~~~ ~~~~~~~~~~~~~
// Simple gadget that can display an array of bitmap images, one at a time.
//
class _OWLCLASS TBitmapGadget : public TGadget {
  public:
    TBitmapGadget(TResId imageResIdOrIndex,
                  int    id,
                  TBorderStyle borderStyle,
                  int    numImages,
                  int    startImage = 0,
                  bool   sharedCels = false);
   ~TBitmapGadget();

    int  SelectImage(int imageNum, bool immediate);
    int  GetImage() const;

    void SysColorChange();

  protected:
    void Paint(TDC& dc);

    void SetBounds(const TRect& boundRect);
    void GetDesiredSize(TSize& size);

  private:
    TResId     ResId;
    TCelArray* ImageArray;
    int        ImageIndex;
    int        CurrentImage;
    int        NumImages;
    TPoint     BitmapOrigin;
};

//
// class TDynamicBitmapGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~
// Class will send EvEnable message to window, 
// so you can set bitmap, using SetCheck() method of 
// TCommandEnabler 
//
class _OWLCLASS TDynamicBitmapGadget: public TBitmapGadget {
  public:
    TDynamicBitmapGadget(TResId imageResIdOrIndex,
                         int id,
                         TBorderStyle borderStyle,
                         int numImages,
                         int startImage = 0,
                         bool sharedCels = false);
    public:
      virtual void CommandEnable();
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// Inline implementations
//
//
inline int TBitmapGadget::GetImage() const {
  return CurrentImage;
}


__OWL_END_NAMESPACE

#endif  // OWL_BITMAPGA_H
