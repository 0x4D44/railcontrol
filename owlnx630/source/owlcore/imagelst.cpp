//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TImageList, an ImageList 'common control' wrapper
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_IMAGELST_H)
# include <owl/imagelst.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlCommCtrl);    // Common Controls diagnostic group

TImageListDrawParam::TImageListDrawParam()
{
  memset(this,0,sizeof(IMAGELISTDRAWPARAMS));
  cbSize = sizeof(IMAGELISTDRAWPARAMS);
}
//
// Construct an empty ImageList of a given size
//
TImageList::TImageList(const TSize& size, uint flags, int initCount, int growBy)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

//  if (!(flags & ILC_COLOR))
//    flags |= ILC_COLORDDB;

  if (!initCount)
    initCount = 1;

  ImageSize = TSize(size.cx/initCount, size.cy);
  Bitmap = 0;
  Handle = TCommCtrl::Dll()->ImageList_Create(size.cx, size.cy, flags, initCount, growBy);

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");
}

//
// Construct a TImageList from a bitmap, slicing a portion of the bitmap up
// into a horizontal array of specified sized images.
//!CQ add an optional mask color? or mask?
//
TImageList::TImageList(const TBitmap& bmp, uint flags, int initCount, int growBy)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

  if (!initCount)
    initCount = 1;

  ImageSize = TSize(bmp.Width() / initCount, bmp.Height());
  Bitmap = 0;

//  if (!(flags & 0xFE))
//    flags |= ILC_COLORDDB;         // default color type?
//  flags |= ILC_MASK;

  Handle = TCommCtrl::Dll()->ImageList_Create(ImageSize.cx, ImageSize.cy,
                                       flags, initCount, growBy);

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");

  // Use masked support with 3dFace color as background color
  //
  Add(bmp, TColor::Sys3dFace); //!CQ
}

//
// Construct a TImageList from a dib, slicing the dib up into a horizontal array
// of even sized images
//!CQ add an optional mask color? or mask? or palette?
//
TImageList::TImageList(const TDib& dib, uint flags, int initCount, int growBy)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

  if (!initCount)
    initCount = 1;

  ImageSize = TSize(dib.Width() / initCount, dib.Height());
  Bitmap = 0;

//  if (!(flags & 0xFE))
//    flags |= ILC_COLORDDB;         // default color type?
//  flags |= ILC_MASK;

  Handle = TCommCtrl::Dll()->ImageList_Create(ImageSize.cx, ImageSize.cy,
                                       flags, initCount, growBy);

  TPalette pal((HPALETTE)::GetStockObject(DEFAULT_PALETTE));
  // Use masked support with 3dFace color as background color
  //
  Add(TBitmap(dib, &pal), TColor::Sys3dFace);
}


//
// Construct a ImageList right from a bmp, icon or cursor resource in a file.
// 'type' should be one of the consts from winuser.h:
//   IMAGE_BITMAP
//   IMAGE_ICON
//   IMAGE_CURSOR
//   IMAGE_ENHMETAFILE ?
//
TImageList::TImageList(HINSTANCE hI, TResId resName, int w, int growBy,
                       const TColor& mask, uint type, uint flags)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

  ImageSize = TSize(0); //!CQ
  Bitmap = 0;
  Handle = TCommCtrl::Dll()->ImageList_LoadImage(hI, resName, w, growBy, mask, type, flags);

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");
}


//
// Construct a TImageList as a copy of an existing one.
//
TImageList::TImageList(const TImageList& src)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

  // using functionality Version 4.0
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95)
    if(TCommCtrl::Dll()->ImageList_Copy(Handle,0,src,0, ILCF_MOVE))
      return;

  TImageInfo ii(src);

  // Initialize members using ii
  //
  ImageSize = ii.GetImageRect().Size();
  Bitmap = 0;
  Handle = TCommCtrl::Dll()->ImageList_Create(ImageSize.cx, ImageSize.cy, ILC_COLORDDB,
                                       src.GetImageCount(), 10);
                                       //!CQ nGrowBy?

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");

  TCommCtrl::Dll()->ImageList_Add(Handle, ii.GetImageBM(), ii.GetMaskBM());
}


//
// Construct a C++ alias for an existing imagelist
//
TImageList::TImageList(HIMAGELIST imageList)
{
  if (!TCommCtrl::IsAvailable())
    TXCommCtrl::Raise();

  Handle = imageList;
  Bitmap = 0;
  int cx = 0;
  int cy = 0;

  TCommCtrl::Dll()->ImageList_GetIconSize(Handle, &cx, &cy);

  ImageSize.cx = cx;
  ImageSize.cy = cy;

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");
}

//
// Constructs a wrapper for the current drag imagelist and
// specifies the location and hotspot of the imagelist.
//
TImageList::TImageList(TPoint& pt, TPoint& hotspot)
{
  ImageSize = TSize(0); //!CQ
  Bitmap = 0;
  Handle = TCommCtrl::Dll()->ImageList_GetDragImage(&pt, &hotspot);

  CheckValid();
  WARNX(OwlCommCtrl, !Handle, 0, "Cannot create ImageList");
}

//
// Destruct the ImageList and clean up the image list handle.
//
TImageList::~TImageList()
{
  TCommCtrl::Dll()->ImageList_Destroy(Handle);
  delete Bitmap;
}

//
// Throw an exception if this image list handle is invalid
//
void
TImageList::CheckValid()
{
  if (!Handle)
    TXCommCtrl::Raise();
}



//
// Assign a ImageList over this ImageList, replacing all contents
//
TImageList&
TImageList::operator =(const TImageList& src)
{
  // using functionality Version 4.0
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95)
    if(TCommCtrl::Dll()->ImageList_Copy(Handle,0,src,0,ILCF_MOVE))
      return *this;

  TCommCtrl::Dll()->ImageList_Remove(Handle, -1);  // -1 is to remove all
   TImageInfo ii(src);
   TCommCtrl::Dll()->ImageList_Add(Handle, ii.GetImageBM(), ii.GetMaskBM());

  return *this;
}

//
// Return the image bitmap used by this ImageList
//
TImageList::operator TBitmap&()
{
  // Build the bitmap object on the fly if needed
  //!CQ To work around a win95 bug, making whole copy for now...
  //
  TImageInfo ii(*this);
  if (!Bitmap || (HBITMAP)*Bitmap != ii.GetImageBM()) {
    delete Bitmap;
    Bitmap = new TBitmap(TScreenDC(), TDib(TBitmap(ii.GetImageBM())));
  }

  return *Bitmap;
}



//
// Draw an image onto a target DC at a given coordinate & with a given style
//
bool
TImageList::Draw(int index, TDC& dc, int x, int y, uint style, int overlay)
{
	// Frank Rast: ILD_OVERLAYMASK must not be used here
  if (overlay)
    style |= INDEXTOOVERLAYMASK(overlay);
  return TCommCtrl::Dll()->ImageList_Draw(Handle, index, dc, x, y, style);
}


//
// Extended version of draw that takes a foreground color and background color
//
bool
TImageList::Draw(int index, TDC& dc, int x, int y, int dx, int dy,
                 const TColor& bgClr, const TColor& fgClr, uint style,
                 int overlay)
{
	// Frank Rast: ILD_OVERLAYMASK must not be used here
  if (overlay)
    style |= INDEXTOOVERLAYMASK(overlay);
  COLORREF bgCr = (bgClr == TColor::None) ? (COLORREF)CLR_NONE : (COLORREF)bgClr;
  COLORREF fgCr = (fgClr == TColor::None) ? (COLORREF)CLR_NONE : (COLORREF)fgClr;

  return TCommCtrl::Dll()->ImageList_DrawEx(Handle, index, dc, x, y, dx, dy, bgCr, fgCr, style);
}

//
// Return number of images currently in this ImageList
//
int
TImageList::GetImageCount() const
{
  return TCommCtrl::Dll()->ImageList_GetImageCount(Handle);
}

//
// Add new image(s) to the ImageList - return index of new addition.
// No mask bitmap is added.
//
int
TImageList::Add(const TBitmap& image)
{
  return TCommCtrl::Dll()->ImageList_Add(Handle, image, 0);
}


//
// Add new image/mask pair(s) to the ImageList - return index of new
// addition.
//
int
TImageList::Add(const TBitmap& image, const TBitmap& mask)
{
  return TCommCtrl::Dll()->ImageList_Add(Handle, image, mask);
}


//
// Add new image(s) to ImageList, specifying a mask color to generate a mask.
// return index of the new addition.
//
int
TImageList::Add(const TBitmap& image, const TColor& mskColor)
{
  return TCommCtrl::Dll()->ImageList_AddMasked(Handle, image, mskColor);
}

//
// Add an icon to the ImageList - return index of new addition.
//
int
TImageList::Add(const TIcon& icon)
{
  return TCommCtrl::Dll()->ImageList_AddIcon(Handle, icon);
}

//
// Remove an image (or all images if index is -1) from this ImageList.
//
bool
TImageList::Remove(int index)
{
  return TCommCtrl::Dll()->ImageList_Remove(Handle, index);
}

//
// Replace an image in this ImageList.
//
bool
TImageList::Replace(int index, const TBitmap& image)
{
  return TCommCtrl::Dll()->ImageList_Replace(Handle, index, image, 0);
}

//
// Replace an image and mask in the ImageList.
//
bool
TImageList::Replace(int index, const TBitmap& image, const TBitmap& mask)
{
  return TCommCtrl::Dll()->ImageList_Replace(Handle, index, image, mask);
}

//
// Create and retreive an icon from an image and mask in the ImageList.
//
HICON
TImageList::GetIcon(int index, uint flags) const
{
  return TCommCtrl::Dll()->ImageList_GetIcon(Handle, index, flags);
}

//
// Replaces the image at index 'index' with the icon or cursor.
//
int
TImageList::ReplaceIcon(int index, HICON icon)
{
  return TCommCtrl::Dll()->ImageList_ReplaceIcon(Handle, index, icon);
}

//
// Return the icon size
//
bool
TImageList::GetIconSize(int& cx, int& cy)
{
  return TCommCtrl::Dll()->ImageList_GetIconSize(Handle, &cx, &cy);
}

//
// Get general information about a given image
//
bool
TImageList::GetImageInfo(int index, TImageInfo& imageInfo) const
{
  return TCommCtrl::Dll()->ImageList_GetImageInfo(Handle, index, (IMAGEINFO*)&imageInfo);
}

//
// Get the current background color for this ImageList
//
TColor
TImageList::GetBkColor() const
{
  return TCommCtrl::Dll()->ImageList_GetBkColor(Handle);
}

//
// Set the current background color for this ImageList, returning the previous
// color.
//
TColor
TImageList::SetBkColor(const TColor& color)
{
  return TCommCtrl::Dll()->ImageList_SetBkColor(Handle, color);
}

//
// Select an image for use as an overlay. Up to four can be selected.
//
bool
TImageList::SetOverlayImage(int index, int overlay)
{
  return TCommCtrl::Dll()->ImageList_SetOverlayImage(Handle, index, overlay);
}


//
// Combine the current drag image with another image in the list.
// Typically a mouse cursor would be added to the image list and merged
// with the drag image list.
//
bool
TImageList::SetDragCursorImage(int drag, int dxHotspot, int dyHotspot)
{
  return TCommCtrl::Dll()->ImageList_SetDragCursorImage(Handle, drag, dxHotspot, dyHotspot);
}

//
// BeginDrag sets this imagelist to be the drag imagelist.
// There can only be one drag imagelist at any time.
//
bool
TImageList::BeginDrag(int index, int dxHotspot, int dyHotspot)
{
  return TCommCtrl::Dll()->ImageList_BeginDrag(Handle, index, dxHotspot, dyHotspot);
}

//
// Typically, this function is called in response to a WM_LBUTTONDOWN message.
// The 'x' and 'y' parameters are relative to the upper-left corner of the
// window's rectangle and NOT the client area.
// The window 'hWndLock' is locked from further updates.
//
bool
TImageList::DragEnter(HWND hWndLock, int x, int y)
{
  return TCommCtrl::Dll()->ImageList_DragEnter(hWndLock, x, y);
}

//
// DragMove is typically called when receiving a WM_MOUSEMOVE message.
// The 'x' and 'y' parameters are generally passed from the message to this
// function.
//
bool
TImageList::DragMove(int x, int y)
{
  return TCommCtrl::Dll()->ImageList_DragMove(x, y);
}

//
// DragLeave is typically called when receiving a WM_LBUTTONUP message.
// The 'hWndLock' window is unlocked from updates.
//
bool
TImageList::DragLeave(HWND hWndLock)
{
  return TCommCtrl::Dll()->ImageList_DragLeave(hWndLock);
}

//
// EndDrag removes the current drag imagelist from the system.
//
void
TImageList::EndDrag()
{
  TCommCtrl::Dll()->ImageList_EndDrag();
}

//
// Lock or unlocks the window from updates.
//
bool
TImageList::DragShowNolock(bool show)
{
  return TCommCtrl::Dll()->ImageList_DragShowNolock(show);
}

//
// Version 4.70
//
bool
TImageList::Copy(TImageList& src, int ifrom, int ito, uint flags)
{
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95)
    return TCommCtrl::Dll()->ImageList_Copy(Handle,ito,src,ifrom,flags);
  return false;
}

//
// Version 4.70
//
bool
TImageList::DrawIndirect(const TImageListDrawParam& imldp)
{
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95)
    return TCommCtrl::Dll()->ImageList_DrawIndirect((IMAGELISTDRAWPARAMS*)&imldp);
  return false;
}

//
// Version 4.71
//
TImageList*
TImageList::Duplicate()
{
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95){
    HIMAGELIST hImg = TCommCtrl::Dll()->ImageList_Duplicate(Handle);
    if(hImg)
      return new TImageList(hImg);
  }
  return 0;
}

//
// Version 4.70
//
bool
TImageList::SetImageCount(uint newcount)
{
  if(TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionWin95)
    return TCommCtrl::Dll()->ImageList_SetImageCount(Handle, newcount);
  return false;
}


__OWL_END_NAMESPACE
/* ========================================================================== */

