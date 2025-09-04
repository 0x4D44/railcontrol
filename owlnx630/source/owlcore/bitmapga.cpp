//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:17 $
//
// Implementation of class TBitmapGadget
// Implementation of class TDynamicBitmapGadget
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_BITMAPGA_H)
# include <owl/bitmapga.h>
#endif
#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_CELARRAY_H)
# include <owl/celarray.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGadget);  // diagnostic group for gadgets

//
// Construct a bitmap gadget
//
TBitmapGadget::TBitmapGadget(TResId imageResIdOrIndex,
                             int    id,
                             TBorderStyle borderStyle,
                             int    numImages,
                             int    startImage,
                             bool   sharedCels)
:
  TGadget(id, borderStyle),
  ImageArray(0),            // Created on first call to GetDesiredSize
  CurrentImage(startImage),
  NumImages(numImages),
  BitmapOrigin(0)
{
  if (sharedCels) {
    ResId = 0;
    ImageIndex = (int)(LPCTSTR)imageResIdOrIndex;
  }
  else {
    ResId = imageResIdOrIndex;
    ImageIndex = 0;
  }

  TRACEX(OwlGadget, OWL_CDLEVEL, "TBitmapGadget constructed @" << this);
}

//
// Destruct a bitmap gadget and free its resources
//
TBitmapGadget::~TBitmapGadget()
{
  delete ImageArray;
  TRACEX(OwlGadget, OWL_CDLEVEL, "TBitmapGadget destructed @" << this);
}

//
// Handle a system color change by cleaning up & reloading & processing the
// bitmap. Is also called to create the initial bitmap.
//
void
TBitmapGadget::SysColorChange()
{
  delete ImageArray;

  if (ResId) {
    TDib dib(*Window->GetModule(), ResId);
    dib.MapUIColors(TDib::MapFace | TDib::MapText | TDib::MapShadow |
      TDib::MapHighlight);
    ImageArray = new TCelArray(dib, NumImages);
  }
}

//
// Set the bounding rect for this button gadget. Also takes care of
// re-centering the image
//
void
TBitmapGadget::SetBounds(const TRect& boundRect)
{
  TRACEX(OwlGadget, 1, "TBitmapGadget::SetBounds() called @" << this);
  TGadget::SetBounds(boundRect);

  TRect innerRect;
  GetInnerRect(innerRect);

  TSize bitmapSize = ImageArray->CelSize();

  BitmapOrigin.x = innerRect.left + (innerRect.Width()-bitmapSize.cx)/2;
  BitmapOrigin.y = innerRect.top + (innerRect.Height()-bitmapSize.cy)/2;
}

//
// Find out how big this bitmap gadget wants to be. Calculated using the base
// size to get the borders, etc. plus the image size.
//
void
TBitmapGadget::GetDesiredSize(TSize& size)
{
  TRACEX(OwlGadget, 1, "TBitmapGadget::GetDesiredSize() called @" << this);
  TGadget::GetDesiredSize(size);

  if (!ImageArray)
    SysColorChange();   // Get the initial bitmap

  size += (ImageArray ? ImageArray->CelSize() : Window->GetCelArray().CelSize());
}

//
// Choose the relative image to display. If immediate is true, this gadget is
// repainted immediately
//
int
TBitmapGadget::SelectImage(int imageNum, bool immediate)
{
  PRECONDITION(imageNum >=0 && imageNum < NumImages);

  uint oldImageNum = CurrentImage;

  if (imageNum != CurrentImage) {
    CurrentImage = imageNum;
    Invalidate(false);
  }

  if (immediate)
    Update();
  return oldImageNum;
}

//
// Paint this bitmap gadget. Uses normal borders, plus draws the image centered
//
void
TBitmapGadget::Paint(TDC& dc)
{
  PaintBorder(dc);

  TRect destRect;
  GetInnerRect(destRect);

  TCelArray& imageArray = ImageArray ? *ImageArray : Window->GetCelArray();

  imageArray.BitBlt(ImageIndex+CurrentImage, dc, destRect.left, destRect.top);
}

//
// class TDynamicBitmapGadgetEnabler
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class TDynamicBitmapGadgetEnabler : public TCommandEnabler {
  public:
    TDynamicBitmapGadgetEnabler(TWindow::THandle hReceiver, 
                                TDynamicBitmapGadget* g)
    :
      TCommandEnabler(g->GetId(), hReceiver),
      Gadget(g)
    {
    }

    // Override TCommandEnabler virtuals
    //
    void  Enable(bool enable);
    void  SetText(LPCTSTR text){}
    void  SetCheck(int state);

  protected:
    TDynamicBitmapGadget* Gadget;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
void TDynamicBitmapGadgetEnabler::Enable(bool enable)
{
  TCommandEnabler::Enable(enable);
  if(Gadget->GetEnabled() != enable)
    Gadget->SetEnabled(enable);
}
//
void  TDynamicBitmapGadgetEnabler::SetCheck(int state)
{
  Gadget->SelectImage(state, false);
}
//

//--------------------------------------------------------
// TDynamicBitmapGadget Constructor
//
TDynamicBitmapGadget::TDynamicBitmapGadget(TResId imageResIdOrIndex, 
      int id, TBorderStyle borderStyle, int numImages,
      int startImage, bool sharedCels) 
: 
  TBitmapGadget(imageResIdOrIndex,id,borderStyle,numImages,startImage,
                sharedCels)
{
}

//
void TDynamicBitmapGadget::CommandEnable()
{ 
  PRECONDITION(Window);

  // Must send, not post here, since a ptr to a temp is passed
  //
  // This might be called during idle processing before the
  // HWND has created.  Therefore, confirm handle exists.
  //
  if (Window->GetHandle()){
    TDynamicBitmapGadgetEnabler ge(*Window, this);
    Window->HandleMessage(WM_COMMAND_ENABLE, 0, TParam2(&ge));
  }
}
//

__OWL_END_NAMESPACE
//==================================================================================


