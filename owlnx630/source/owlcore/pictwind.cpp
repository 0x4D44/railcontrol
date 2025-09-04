//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
//  Implements TPictureWindow
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_PICTWIND_H)
# include <owl/pictwind.h>
#endif
#if !defined(OWL_POINTER_H)
# include <owl/pointer.h>
#endif
#if !defined(OWL_SYSTEM_H)
# include <owl/system.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Constructor assumes ownership of the DIB passed in.
//
TPictureWindow::TPictureWindow(TWindow* parent, TDib* dib, TDisplayHow how,
                               LPCTSTR title, TModule* module)
:
  TWindow(parent, title, module),
  Dib(dib)
{
  SetHowToDisplay(how);
  if (GetHowToDisplay() == Stretch) {
    if (dib) {
      Attr.W = dib->Width();
      Attr.H = dib->Height();
    }
  }
}

//
// Destructor deletes the owned DIB.
//
TPictureWindow::~TPictureWindow()
{
  delete Dib;
}


//
// Allow changing of the dib.
// Return old dib.
//
TDib*
TPictureWindow::SetDib(TDib* newDib)
{
  TDib* retDib = Dib;
  Dib = newDib;
  return retDib;
}


//
// Paint the dib onto the window.
//
void
TPictureWindow::Paint(TDC& dc, bool /*erase*/, TRect& /*rect*/)
{
  TPointer<TPalette> palette = 0;
  bool hasPalette = ToBool(dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE);

  TDib* dib = GetDib();
  if (dib) {
    if (hasPalette) {
      palette = new TPalette(*GetDib());
      dc.SelectObject(*palette);
      dc.RealizePalette();
    }

    // figure out upper left corner of the client area
    //
    TRect clientRect(GetClientRect());
    TPoint sourcePoint(0, 0);

    // adjust the upper left corner for centering picture
    //
    if (HowToDisplay == Center) {
      // determine offsets
      //
      int offsetX = abs(dib->Width() - clientRect.Width()) / 2;
      if (dib->Width() > clientRect.Width())
        sourcePoint.x += offsetX;
      else
        clientRect.Offset(offsetX, 0);

      int offsetY = abs(dib->Height() - clientRect.Height()) / 2;
      if (dib->Height() > clientRect.Height())
        sourcePoint.y += offsetY;
      else
        clientRect.Offset(0, offsetY);
    }

    // adjust the lower right corner
    //
    if (HowToDisplay != Stretch) {
      clientRect.bottom = clientRect.top + dib->Height();
      clientRect.right  = clientRect.left + dib->Width();

      // if the picture is larger than screen dimensions,
      // adjust the upper left corner.
      //
      clientRect.top   -= sourcePoint.y;
      clientRect.left  -= sourcePoint.x;
    }

    // display the dib
    //
    switch (HowToDisplay) {
      case UpperLeft:
      case Center:
        dc.SetDIBitsToDevice(clientRect, sourcePoint, *dib);
//        if(!dc.SetDIBitsToDevice(clientRect, sourcePoint, *dib)){
//          TSystemMessage().MessageBox();
//        }
        break;
      case Stretch: {
        TRect sourceRect(0, 0, dib->Width(), dib->Height());
        dc.StretchDIBits(clientRect, sourceRect, *dib);
        break;
      }
    } // switch HowToDisplay

    dc.RestoreObjects();
  }
}


//
// Change the formatting of the dib.
//
void
TPictureWindow::SetHowToDisplay(TDisplayHow how)
{
  HowToDisplay = how;
  if (IsWindow())
    Invalidate(true);
}


//
// Overridden from TWindow.
// Return unique name to force GetWindowClass to be called.
//
LPCTSTR
TPictureWindow::GetClassName()
{
  return _T("OWL_PictureWindow");
}


//
// Overridden from TWindow.
//
void
TPictureWindow::GetWindowClass(WNDCLASS& wndClass)
{
  TWindow::GetWindowClass(wndClass);
  wndClass.style |= CS_HREDRAW | CS_VREDRAW;
}


__OWL_END_NAMESPACE
/* ========================================================================== */

