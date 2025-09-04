//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TDib class, an encapsulation of Device Independent Bitmaps
// on disk and in memory.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_METAFILE_H)
# include <owl/metafile.h>
#endif
#if !defined(OWL_CLIPBOAR_H)
# include <owl/clipboar.h>
#endif
#if !defined(OWL_EXCEPT_H)
# include <owl/except.h>
#endif
#if !defined(OWL_FILE_H)
# include <owl/file.h>
#endif

#define BFT_BITMAP 0x4d42   /* 'BM' */

//
// Direct dib surface manipulation functions
//
extern "C"
{
  void WINAPI
  DibCopyBlt8(uint8 * dstBits, uint32 width, uint32 height, uint32 dstPitch,
              uint8 * srcBits, uint32 srcPitch);

  void WINAPI
  DibSpriteBlt8(uint8 * dstBits, uint32 width, uint32 height, uint32 dstPitch,
               uint8 * srcBits, uint32 srcPitch, uint8 transparent);
}


__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);        // General GDI diagnostic group

//
// Internal DIB file Read/Write functions talk to this IO implementation
//
class TFileIO : public TDib::IFileIn, public TDib::IFileOut {
  public:
    TFileIO(TFile& file) : File(file) {}

    virtual long Read(void * buffer, long size)
                 {
                   return File.Read(buffer, size);
                 }
    virtual void Skip(long size)
                 {
                   File.Seek(size, TFile::cur);
                 }
    virtual bool Write(void * buffer, long size)
                 {
                   return File.Write(buffer, size);
                 }

  private:
    TFile& File;
};

#if !defined(SECTION) || SECTION == 1  // Main ctors, dtor & helpers

//
// Given a BITMAPINFOHEADER, allocate a global buffer for Handle & init Info
//
void
TDib::InfoFromHeader(const BITMAPINFOHEADER& infoHeader)
{
  // Allocate & lock down the actual single memory block used by this DIB
  //
  int maskAlloc = infoHeader.biCompression==BI_BITFIELDS ? 3*sizeof(uint32) : 0;
  int colorAlloc = int(infoHeader.biClrUsed * sizeof(RGBQUAD));   // Color table size in mem
  IsResHandle = false;
  uint32 size = sizeof(BITMAPINFOHEADER) + maskAlloc + colorAlloc + infoHeader.biSizeImage;
  Handle = ::GlobalAlloc(GMEM_MOVEABLE, size);
  if (!Handle)
    TXOutOfMemory::Raise();

  // Setup Info ptr & copy over working infoHeader. Finish by using
  // InfoFromHandle to update all members from Info.
  //
  Info = (LPBITMAPINFO)::GlobalLock(Handle);
  Info->bmiHeader = infoHeader;

  InfoFromHandle();
}

//
// Lock the global/res handle if needed & extract the pointers and cached info
// maintained as member variables from Info.
//
// Needs only Info setup, or if Info is 0 then Handle & IsResHandle setup
//
void
TDib::InfoFromHandle()
{
  // Get the BITMAPINFO Info from the handle if needed
  //
  if (!Info) {
    if (IsResHandle)
      Info = (LPBITMAPINFO)::LockResource(Handle);
    else
      Info = (LPBITMAPINFO)::GlobalLock(Handle);
  }
  // Fail if the header does not look OK
  //
  if (Info->bmiHeader.biSize != sizeof(BITMAPINFOHEADER)) {
    ::GlobalUnlock(Handle);
    TXGdi::Raise(IDS_INVALIDDIBHANDLE);
  }

  // Determine if there is a bit mask (used by 16 & 32 bpp bmps) and where
  // the color table starts
  //
  if (Info->bmiHeader.biCompression == BI_BITFIELDS) {
    Mask = (uint32 *)Info->bmiColors;
    Colors = (TRgbQuad *)((char *)Info->bmiColors + 3 * sizeof(uint32));
  }
  else {
    Mask = 0;
    Colors = (TRgbQuad *)Info->bmiColors;
  }

  // Figure out the number of colors in the color table using biClrsUsed &
  // biBitCount.
  //
  NumClrs = Info->bmiHeader.biClrUsed
              ? Info->bmiHeader.biClrUsed
              : NColors(Info->bmiHeader.biBitCount);

  Bits = (char *)(char *)Colors + (int)NumClrs * sizeof(RGBQUAD);

  // Zero out Colors if there is no color table.
  //
  if (!NumClrs)
    Colors = 0;

  Mode = DIB_RGB_COLORS;
}

//
// Under Win32, resources are read-only. So, to allow for modification of the
// Dib, a copy must be made.
//
void
TDib::ResToMemHandle()
{
  long size = ((char *)Bits + Info->bmiHeader.biSizeImage) -
                (char *)Info;
//  long size = ::SizeofResource(instance, resHandle);
  HANDLE tempHandle = ::GlobalAlloc(GMEM_MOVEABLE, size);
  if (!tempHandle)
    TXOutOfMemory::Raise();

  void* tempInfo = ::GlobalLock(tempHandle);
  memcpy(tempInfo, Info, size);

  // Handle will now be a memory handle, & no longer a res handle
  // Update Bits pointer & other members that may have moved.
  //
  Handle = tempHandle;
  Info = (LPBITMAPINFO)tempInfo;
  IsResHandle = false;
  InfoFromHandle();
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2  

//
// Construct a TDib from a borrowed handle
//
TDib::TDib(HGLOBAL handle, TAutoDelete autoDelete)
:
  TGdiBase(handle, autoDelete),
  Info(0), IsResHandle(false)
{
  InfoFromHandle();
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 3

//
// Construct a TDib borrowed from a clipboard handle
//
TDib::TDib(const TClipboard& clipboard)
:
  TGdiBase(clipboard.GetClipboardData(CF_DIB)),
  Info(0), IsResHandle(false)
{
  InfoFromHandle();
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 4

//
// Construct a TDib that is a copy of an existing one
//
TDib::TDib(const TDib& src)
:
  IsResHandle(false)
{
  PRECONDITION(src.Info != 0);

//  long size = ::GlobalSize(src.Handle); //!CQ assumes src has a mem handle
  uint32 size = src.SizeDib();
  Handle = ::GlobalAlloc(GMEM_MOVEABLE, size);
  if (!Handle)
    TXOutOfMemory::Raise();

  Info = (LPBITMAPINFO)::GlobalLock(Handle);
  memcpy(Info, src.Info, size);

  Mask = (uint32 *)(src.Mask ?
           (char *)Info + ((char *)src.Mask - (char *)src.Info) : 0);
  Bits = (char *)Info + ((char *)src.Bits - (char *)src.Info);

  NumClrs = src.NumClrs;

  // Zero out Colors if there is no color table.
  //
  if (!NumClrs)
    Colors = 0;
  else
    Colors = (TRgbQuad *)((char *)Info + ((char *)src.Colors - (char *)src.Info));

  Mode = src.Mode;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 5

//
// Destruct a TDib by unlocking & freeing its global memory handle as required
//
TDib::~TDib()
{
  if (Handle)
    if (IsResHandle) {
    }
    else {
      ::GlobalUnlock(Handle);
      if (ShouldDelete)
        ::GlobalFree(Handle);
    }
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 6

//
// Convert an absolute RGB color table to a palette relative color table
// Makes sure that color table is RGB
//
bool
TDib::ChangeModeToPal(const TPalette& pal)
{
  if (Mode != DIB_RGB_COLORS)
    return false;

  uint16 nEntries = pal.GetNumEntries();

  for (int c = 0; c < NumClrs; c++) {
    uint16 index = uint16(c);
    for (uint16 i = 0; i < nEntries; i++) {
      PALETTEENTRY pe; //getting all entries one time up front would be faster
      pal.GetPaletteEntry(i, pe);
      if (pe.peRed == Colors[c].rgbRed &&
          pe.peGreen == Colors[c].rgbGreen &&
          pe.peBlue == Colors[c].rgbBlue) {
        index = i;
        break;
      }
    }
    ((uint16*)Colors)[c] = index;
  }
  Mode = DIB_PAL_COLORS;
  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 7

//
// Convert a palette relative color table to an absolute RGB color table
// Makes sure that color table is pal relative & that there is enough space
//
bool
TDib::ChangeModeToRGB(const TPalette& pal)
{
  if (Mode != DIB_PAL_COLORS ||
      (uint)((char*)Bits - (char*)Info) < NumClrs*sizeof(RGBQUAD))
    return false;

  uint16 nEntries = pal.GetNumEntries();

  for (int c = (int)NumClrs-1; c >= 0; c--) {
    uint16 i = ((uint16*)Colors)[c];
    if (i >= nEntries)
      i = 0;
    PALETTEENTRY pe;
    pal.GetPaletteEntry(i, pe);
    Colors[c].rgbRed = pe.peRed;
    Colors[c].rgbGreen = pe.peGreen;
    Colors[c].rgbBlue = pe.peBlue;
  }
  Mode = DIB_RGB_COLORS;
  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 8

//
// Get, set, find and map the color table entries as colors (RGBQUADs)
//
TColor
TDib::GetColor(int entry) const
{
  if (entry >= 0 && entry < NumClrs)
    if (Mode == DIB_RGB_COLORS)
      return GetColors()[entry];

  return 0;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 9

//
// Set the color table entry to a specified color.
//
void
TDib::SetColor(int entry, const TColor& color)
{
  if (entry >= 0 && entry < NumClrs)
    if (Mode == DIB_RGB_COLORS)
      GetColors()[entry] = color;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 10

//
// Return the entry in the table that matches this color.
//
int
TDib::FindColor(const TColor& color)
{
  for (int entry = 0; entry < NumClrs; entry++)
    if (color == GetColors()[entry])  // Small data model requires this order
      return entry;
  return -1;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 11

//
// Map first or all occurances of a given palette color to a new color
//
int
TDib::MapColor(const TColor& fromColor, const TColor& toColor, bool mapAll)
{
  int count = 0;
  for (int entry = 0; entry < NumClrs; entry++)
    if (fromColor == GetColors()[entry]) {  // Small data model requires this order
      GetColors()[entry] = toColor;
      count++;
      if (!mapAll)
        break;
    }
  return count;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 12

//
// Get, set, find and map the color table entries as palette indices
//
uint16
TDib::GetIndex(int entry) const
{
  if (entry >= 0 && entry < NumClrs)
    if (Mode == DIB_PAL_COLORS)
      return ((uint16*)GetColors())[entry];

  return 0;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 13

//
// Set the entry in the table to the palette index.
//
void
TDib::SetIndex(int entry, uint16 index)
{
  if (entry >= 0 && entry < NumClrs)
    if (Mode == DIB_PAL_COLORS)
      ((uint16*)GetColors())[entry] = index;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 14

//
// Find the table entry that matches this palette index.
//
int
TDib::FindIndex(uint16 index)
{
  for (int entry = 0; entry < NumClrs; entry++)
    if (GetIndices()[entry] == index)
      return entry;
  return -1;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 15

//
// Map first or all occurances of a given palette index to a new index
//
int
TDib::MapIndex(uint16 fromIndex, uint16 toIndex, bool doAll)
{
  int count = 0;
  for (int entry = 0; entry < NumClrs; entry++)
    if (GetIndices()[entry] == fromIndex) {
      GetIndices()[entry] = toIndex;
      count++;
      if (!doAll)
        break;
    }
  return count;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 16

//
// Use MapColor to map selected standard UI colors in this DIB to their
// current system values. Also can map a background color. Used to update a
// UI DIB to current colors before rendering.
//
//   LtGray   -> 3d Face
//   Black    -> Button Text
//   Gray     -> 3d Shadow
//   White    -> 3d Hilight
//   Magenta  -> Window Frame
//   LtYellow -> bkColor
//
void
TDib::MapUIColors(uint mapColors, const TColor* bkColor)
{
  if (mapColors & TDib::MapFace)
#ifdef UNIX
    MapColor(TColor::LtGray, TColor::SysWindow);  //copied from old owl (DLN UNIX color 
                                                  //problem TColor::Sys3dFace);) (04-16-01 JAM)
#else
    MapColor(TColor::LtGray, TColor::Sys3dFace);
#endif

  if (mapColors & TDib::MapText)
    MapColor(TColor::Black, TColor::SysBtnText);
  if (mapColors & TDib::MapShadow)
    MapColor(TColor::Gray, TColor::Sys3dShadow);
  if (mapColors & TDib::MapHighlight)
    MapColor(TColor::White, TColor::Sys3dHilight);
  if (mapColors & TDib::MapFrame)
    MapColor(TColor::LtMagenta, TColor::SysWindowFrame);
  if (bkColor)
    MapColor(TColor::LtYellow, *bkColor);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 17

//
// Move this dib to the clipboard. Ownership of the DIB handle is passed to the
// clipboard.
//
void
TDib::ToClipboard(TClipboard& clipboard)
{
  if (Handle) {
    if (IsResHandle) {
    }
    else {
      ::GlobalUnlock(Handle);
    }
    clipboard.SetClipboardData(CF_DIB, Handle);
    ShouldDelete = false;
    Info = 0;
    Bits = 0;
    Handle = 0;
  }
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 18

//
// Construct a TDib given a .BMP file name
//
TDib::TDib(LPCTSTR name)
{
  if (!ReadFile(name))
    TXGdi::Raise(IDS_GDIFILEREADFAIL);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 19

//
// Create a dib from the file.
//
TDib::TDib(TFile& file, bool readFileHeader)
{
  if (!Read(file, readFileHeader))
    TXGdi::Raise(IDS_GDIFILEREADFAIL);
}

//
// Create a dib from the file stream.
//
TDib::TDib( istream& is, bool readFileHeader)
{
  if (!Read(is, readFileHeader))
    TXGdi::Raise(IDS_GDIFILEREADFAIL);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 20

//
// Construct an empty Dib for use by derived classes
//
TDib::TDib()
:
  Info(0), Mask(0), Colors(0), Bits(0),
  NumClrs(0),
  IsResHandle(false)
{
  // Handle must be set by derived class
  // InfoFromHandle() or equivalent must then be called
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 21

//
// Test if the passed file is a Windows 3.x (or PM 1.x) DIB and if so read it.
// Return true iff Dib was read OK
//
bool
TDib::ReadFile(LPCTSTR name)
{
  TFile file(name, TFile::ReadOnly|TFile::OpenExisting);
  if (!file.IsOpen()) {
    TRACEX(OwlGDI, 0, "Cannot open bitmap file '" << name << "' to read");
    return false;
  }

  // Read the bitmap in, file header & all
  //
  bool ok = Read(file, true);

  file.Close();
  return ok;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 22

//
// Read a Windows 3.x or PM 1.x device independent bitmap from a TFile
// Return true iff Dib was read OK
//
bool
TDib::Read(TFile& file, bool readFileHeader)
{
  TFileIO in(file);
  return Read(in, readFileHeader);
}

//
// The ios version of the DIB read requires large data model since the buffers
// are quite large.
//

//
//
//
class TStreamIn : public TDib::IFileIn {
  public:
    TStreamIn(istream& is) : Is(is) {}

    virtual long Read(void * buffer, long size)
                 {
                   //return Is.read((char*)buffer, size).good();
                   return Is.read((char*)buffer, size).gcount();
                 }
    virtual void Skip(long size)
                 {
                   Is.seekg(size, ios::cur);
                 }

  private:
    istream& Is;
};

//
//
// Read a Windows 3.0 or PM 1.X device independent bitmap from an istream
// Return true iff DIB was read OK
//
// Read a Windows 3.0 or PM 1.X device independent bitmap. (.BMP)
//  Check header, read Info, palette and bitmap.  PM DIBs can be converted to
//  Win 3.x DIBs on the fly.
// Return true iff DIB was read OK
//
bool
TDib::Read( istream& is, bool readFileHeader)
{
  TStreamIn in(is);
  return Read(in, readFileHeader);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 23

//
// Read a Windows 3.0 or PM 1.X device independent bitmap. (.BMP) from an
// implementor of TDib's IFileIn interface
//
// Check header, read Info, palette and bitmap.  PM Dibs are converted to
// Windows 3.x Dibs on the fly.
//
// Return true iff Dib was read OK
//
bool
TDib::Read(IFileIn& in, bool readFileHeader)
{
  long offBits = 0;

  // Read file header and verify the signature. Grab bfOffBits if it is != 0
  // We ignore all other information in the file header since some applications
  // do not put correct information in the fields...
  //
  if (readFileHeader) {
    BITMAPFILEHEADER bmf;
    if ((size_t)in.Read(&bmf, sizeof(bmf)) != sizeof(bmf) || bmf.bfType != BFT_BITMAP) {
      TRACEX(OwlGDI, 0, "Not a Windows 3.x or PM 1.x bitmap file");
      TXBadFormat::Raise();
      return false;
    }
    if (bmf.bfOffBits)
      offBits = bmf.bfOffBits - sizeof(BITMAPFILEHEADER);
  }

  // Read bitmap header size & check it. It must be one of the two known header
  // sizes.
  // Will add BITMAPV4HEADER support when available
  //
  uint32 headerSize;
  if ((size_t)in.Read(&headerSize, sizeof(headerSize)) != sizeof(headerSize)
      || headerSize != sizeof(BITMAPCOREHEADER)
      && headerSize != sizeof(BITMAPINFOHEADER)
     ) {
    TRACEX(OwlGDI, 0, "Not a Windows 3.x or PM 1.x bitmap file");
    TXBadFormat::Raise();
    return false;
  }

  // Prepare to build or read a header that is an Info (Windows) type
  //
  BITMAPINFOHEADER infoHeader;
  infoHeader.biSize = sizeof(BITMAPINFOHEADER);

  // If this is a PM 1.x DIB, read the core header & copy over to the Info header
  //
  bool   isCore = headerSize == sizeof(BITMAPCOREHEADER);
  if (isCore) {
    // Read in the rest of the core header, aborting if it is truncated
    //
    BITMAPCOREHEADER coreHeader;
    int size = (int)headerSize-sizeof(uint32);
    if (in.Read(&coreHeader.bcWidth, size) != size) {
      TRACEX(OwlGDI, 0, "Invalid PM 1.x DIB Header");
      TXBadFormat::Raise();
      return false;
    }

    // Copy core fields over to info fields, filling in the missing pieces
    //
    infoHeader.biWidth = coreHeader.bcWidth;
    infoHeader.biHeight = coreHeader.bcHeight;
    infoHeader.biPlanes = coreHeader.bcPlanes;
    infoHeader.biBitCount = coreHeader.bcBitCount;
    infoHeader.biCompression = BI_RGB;  // No compression
    infoHeader.biSizeImage = 0;         // Calculate this below
    infoHeader.biXPelsPerMeter = 0;     // Zero is OK
    infoHeader.biYPelsPerMeter = 0;     // Zero is OK
    infoHeader.biClrUsed = 0;           // Calculate this below
    infoHeader.biClrImportant = 0;      // Zero is OK
  }
  // This is a Win 3.x DIB, read the info header
  //
  else {
    // Read in the rest of the info header, aborting if it is truncated
    //
    int size = (int)headerSize-sizeof(uint32);
    if (in.Read(&infoHeader.biWidth, size) != size) {
      TRACEX(OwlGDI, 0, "Invalid Win 3.x DIB Header");
      TXBadFormat::Raise();
      return false;
    }
  }

  // Check number of planes. Windows supports only 1 plane DIBs
  //
  if (infoHeader.biPlanes != 1) {
    TRACEX(OwlGDI, 0, "Invalid number of planes in DIB");
    TXBadFormat::Raise();
    return false;
  }

  // Fill in the default value for biClrsUsed, if not supplied, using the
  // bit count. Will remain 0 for 16bpp or greater.
  //
  if (!infoHeader.biClrUsed)
    infoHeader.biClrUsed = NColors(infoHeader.biBitCount);

  // Some applications do not fill in the biSizeImage field in the header.
  // (Actually the truth is more likely that some drivers do not fill the
  // field in and the apps do not compensate for these buggy drivers.)
  //
  // Therefore, if compression was not used, we (re)compute the size,
  // but if compression is used, we have no choice but to trust the size.
  //
  if (infoHeader.biCompression == BI_RGB ||
      infoHeader.biCompression == BI_BITFIELDS)
    infoHeader.biSizeImage =
        ScanBytes(infoHeader.biWidth,
                  infoHeader.biBitCount) * infoHeader.biHeight;

  // Setup all members based on InfoHeader
  //
  InfoFromHeader(infoHeader);

  // Read color mask directly into allocated memory
  //
  if (Mask) {
    int size = 3*sizeof(uint32);
    if (in.Read((char *)Mask, size) != size) {
      TRACEX(OwlGDI, 0, "Could not read DIB color mask");
      ::GlobalUnlock(Handle);
      return false;
    }
  }

  // Read color table directly into allocated memory
  // Walk backwards & expand to RGBQUADs if it is a PM Core DIB
  //
  int colorRead = isCore ?          // Color table size on disk
        (int)NumClrs * sizeof(RGBTRIPLE) :
        (int)NumClrs * sizeof(RGBQUAD);
  if (Colors) {
    if (in.Read((char *)Colors, colorRead) != colorRead) {
      TRACEX(OwlGDI, 0, "Could not read DIB color table");
      ::GlobalUnlock(Handle);
      return false;
    }
    if (isCore) {
      for (int i = int(NumClrs-1); i >= 0; i--) {
        Colors[i].rgbRed = ((RGBTRIPLE*)Colors)[i].rgbtRed;
        Colors[i].rgbGreen = ((RGBTRIPLE*)Colors)[i].rgbtGreen;
        Colors[i].rgbBlue = ((RGBTRIPLE*)Colors)[i].rgbtBlue;
        Colors[i].rgbReserved = 0;
      }
    }
  }

  // Locate & Read Bits, skipping Pad if any.
  // Ignore offBits if it is zero, or if less than the current position--it's
  // probably bad
  //
  if (offBits && offBits - (long)(headerSize+colorRead) > 0)
    in.Skip(offBits - (headerSize+colorRead));
  if (!in.Read(Bits, infoHeader.biSizeImage)) {
    TRACEX(OwlGDI, 0, "Could not read DIB bits");
    return false;
  }

  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 24

//
// Construct a TDib given a module instance and a resource name or int Id
//
TDib::TDib(HINSTANCE instance, TResId resId)
:
  Info(0), Mask(0), Colors(0), Bits(0),
  NumClrs(0),
  IsResHandle(false)
{
  if (!LoadResource(instance, resId))
    TXGdi::Raise(IDS_GDIRESLOADFAIL);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 25

//
// Load a dib resource into an empty dib.
//
bool
TDib::LoadResource(HINSTANCE instance, TResId resId)
{
  PRECONDITION(!Handle);

  // First, load the resource into a global memory block.
  //
  HRSRC  resHandle = ::FindResource(instance, resId, RT_BITMAP);
  Handle = resHandle ? ::LoadResource(instance, resHandle) : 0;
  if (!Handle) {
    TRACEX(OwlGDI, 0, "Cannot access bitmap resource");
    return false;
  }
  IsResHandle = true;

  // Then update our pointers & other info.
  //
  InfoFromHandle();

  // Under Win32, resources are read-only. So, to allow for later modification
  // of the Dib, a copy must be made.
  //
  // Could postpone this until dib needed to be written on...
  //
  ResToMemHandle();

  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 26

//
// Write the DIB into an external file.
//
bool
TDib::WriteFile(LPCTSTR name)
{
  TFile file(name, TFile::WriteOnly|TFile::PermExclusive|TFile::CreateAlways);

  if (!file.IsOpen()) {
    TRACEX(OwlGDI, 0, "Cannot open bitmap file '" << name << "' to write");
    return false;
  }

  bool ok = Write(file, true);

  file.Close();
  if (!ok) {
    TRACEX(OwlGDI, 0, "Disk error writing file '" << name << "'");
    return false;
  }
  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 27

//
// Write the DIB into a file.
//
bool
TDib::Write(TFile& file, bool writeFileHeader)
{
  TFileIO out(file);
  return Write(out, writeFileHeader);
}

//
// The ios version of the DIB read requires large data model since the buffers
// are quite large.
//


//
//
//
class TStreamOut : public TDib::IFileOut {
  public:
    TStreamOut( ostream& os) : Os(os) {}

    virtual bool Write(void * buffer, long size)
                 {
                   return Os.write((char*)buffer, size).good();
                 }
  private:
    ostream& Os;
};

//
// Write the DIB into a file stream.
//
bool
TDib::Write( ostream& os, bool writeFileHeader)
{
  TStreamOut out(os);
  return Write(out, writeFileHeader);
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 28

//
// Write the DIB into a file stream.
//
bool
TDib::Write(IFileOut& out, bool writeFileHeader)
{
  long size = ::GlobalSize(Handle);

  // Write file header
  //
  if (writeFileHeader) {
    BITMAPFILEHEADER bmf;
    bmf.bfType = BFT_BITMAP;//'BM';
    bmf.bfSize = sizeof(bmf) + size;
    bmf.bfReserved1 = 0;
    bmf.bfReserved2 = 0;
    bmf.bfOffBits = sizeof bmf + (char *)Bits - (char *)Info;
    if (!out.Write(&bmf, sizeof bmf))
      return false;
  }

  // Write rest of dib, including dib header, color table & bits
  //
  if (!out.Write(Info, size)) //(void *)Info
    return false;

  return true;
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 29

//
// Construct a Dib given dimensions and color depth
//
TDib::TDib(int width, int height, uint32 nColors, uint16 mode)
{
  PRECONDITION(width && height && nColors);

  BITMAPINFOHEADER      infoHeader;
  infoHeader.biSize       = sizeof(BITMAPINFOHEADER);
  infoHeader.biWidth      = width;
  infoHeader.biHeight     = height;
  infoHeader.biPlanes     = 1;
  infoHeader.biBitCount   = NBits(nColors);
  infoHeader.biCompression   = BI_RGB;
  uint32 bytes = ScanBytes(width, infoHeader.biBitCount);
  infoHeader.biSizeImage = bytes * height;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed       = infoHeader.biBitCount > 8 ? 0 : nColors;
  infoHeader.biClrImportant  = 0;

  // Setup all members based on infoHeader
  //
  InfoFromHeader(infoHeader);

  // Initialize the color table--either Palette or RGB
  //
  Mode = mode;
  if (Mode == DIB_PAL_COLORS) {
    // Generate a 1:1 palette relative color table- it can later be translated
    // to RGB given a palette.
    //
    for (uint i = 0; i < nColors; i++)
      ((uint16*)Colors)[i] = (uint16)i;
  }
  else if(nColors){  
    // Get the system palette and convert to RGB quad format if exist support.
    //
    TScreenDC dc;
    if(dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE){
      uint clrCnt = ::GetSystemPaletteEntries(dc, 0, nColors, (LPPALETTEENTRY)Colors);
      for (uint32 i = 0; i < clrCnt; i++) {
            //FMM Changed Swap to swap
            #ifdef __HPUX__
            swap(Colors[i].rgbRed, Colors[i].rgbBlue);
            #else
        Swap(Colors[i].rgbRed, Colors[i].rgbBlue);
            #endif
        Colors[i].rgbReserved = 0;
      }
    }
  }
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 30

//
// Construct a TDib given a TBitmap and a TPalette
// If no palette is give, uses the one in the focus window.
//
TDib::TDib(const TBitmap& bitmap, const TPalette* palette)
{
  BITMAP  bm;
  bitmap.GetObject(bm);

  BITMAPINFOHEADER  infoHeader;
  infoHeader.biSize       = sizeof(BITMAPINFOHEADER);
  infoHeader.biWidth      = bm.bmWidth;
  infoHeader.biHeight     = bm.bmHeight;
  infoHeader.biPlanes     = 1;
  uint16 nColors;
  if (palette) {
    palette->GetObject(nColors);
    infoHeader.biBitCount = NBits(nColors);
  }
  else {
    infoHeader.biBitCount = uint16(bm.bmBitsPixel*bm.bmPlanes);
    nColors = (uint16)NColors(infoHeader.biBitCount);
  }
  infoHeader.biCompression   = BI_RGB;
  infoHeader.biSizeImage     = ScanBytes(infoHeader.biWidth,
                                         infoHeader.biBitCount) * infoHeader.biHeight;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed       = nColors;
  infoHeader.biClrImportant  = 0;

  // Setup all members based on infoHeader
  //
  InfoFromHeader(infoHeader);

  // Copy pixels from given bitmap into our new empty DIB, selecting palette
  // if provided
  //
  TScreenDC dc;
  if (palette)
    dc.SelectObject(*palette, false);
  dc.GetDIBits(bitmap, StartScan(), NumScans(), Bits, *Info, Usage());
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 31

//
// Blast the image from a dib onto this bitmap.
//
void
TDib::MultiBlt(int type, int dstX, int dstY, TDib& srcDib,
               int srcWidth, int srcHeight, uint8 transparentColor)
{
  // this == dst
  //
  if (!srcWidth)
    srcWidth = srcDib.Width();
  if (!srcHeight)
    srcHeight = srcDib.Height();

  int orientation = 1;
  long dstHeight = Height();

  if (dstHeight < 0) {   // check for top-down DIB
    orientation = -1;
    dstHeight = -dstHeight;
  }

  TRect dstRect(0, 0, Width(), int(dstHeight));
  TRect srcRect(dstX, dstY, dstX + srcWidth, dstY + srcHeight);
  TRect clipped = srcRect & dstRect;

  // Intersect the dst rectangle with the dst DIB
  //
  if (!clipped.IsEmpty()) {
    // default delta scan to width in bytes
    //
    long dstPitch = Pitch();

    long dstXOrigin = clipped.left;
    long dstYOrigin = clipped.top;

    long dstDeltaX = dstXOrigin - dstX;
    long dstDeltaY = dstYOrigin - dstY;

    int width = clipped.Width();
    int height = clipped.Height();

    long srcXOrigin = dstDeltaX;
    long srcYOrigin = dstDeltaY;

    uint8 * srcSurface = (uint8 *)srcDib.GetBits();
    srcSurface += long(srcHeight - (srcYOrigin + uint32(height))) * srcDib.Pitch();
    srcSurface += long(srcXOrigin);

    // Now we'll calculate the starting dst pointer taking into account we may
    // have a top-down dst
    //
    uint8 * dstSurface;
    if (orientation < 0) {
      // Dst is top-down
      //
      dstPitch = -dstPitch;
      dstSurface  = (uint8 *)GetBits();
      dstSurface += long(dstYOrigin + uint32(height) - 1) * Pitch();
      dstSurface += long(dstXOrigin);
//      dstSurface = (uint8 *)GetBits()
//                   + long(dstYOrigin + uint32(height) - 1) * Pitch()
//                   + dstXOrigin;
    }
    else {
      // Dst is bottom-up
      //
      dstSurface  = (uint8 *)GetBits();
      dstSurface += long(dstHeight - (dstYOrigin + uint32(height))) * Pitch();
      dstSurface += long(dstXOrigin);

//      dstSurface = (uint8 *)GetBits()
//                   + long(dstHeight - (dstYOrigin + uint32(height))) * Pitch()
//                   + dstXOrigin;
    }
    if (type == 0)
      ::DibCopyBlt8(dstSurface, width, height, dstPitch,
                    srcSurface, srcDib.Pitch());
    else
      ::DibSpriteBlt8(dstSurface, width, height, dstPitch,
                      srcSurface, srcDib.Pitch(), transparentColor);
  }
}

#endif
//-------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 32

//
// Modify this dib so that the pixels & color table coincide with a given
// palette
//!CQ 8bpp only!
//
void
TDib::MapToPalette(const TPalette& pal)
{
  // Create a mapping from our current color table to the target palette
  //
  uint8 map[256];
  for (int i = 0; i < 256; i++)
    map[i] = (uint8)pal.GetNearestPaletteIndex(GetColor(i));

  // Use the map to translate all of the pixels into indexes in the target
  // palette
  //
  uint8 * bits = (uint8 *)GetBits();
  long size = Pitch() * Height();
  while (size--) {
    *bits = map[*bits];
    bits++;
  }

  // Now copy the target palette into our color table
  //
  PALETTEENTRY pe[256];
  pal.GetPaletteEntries(0, 256, pe);
  TRgbQuad * colorTab = GetColors();
  for (int j = 0; j < 256; j++) {
    colorTab->rgbRed = pe[j].peRed;
    colorTab->rgbGreen = pe[j].peGreen;
    colorTab->rgbBlue = pe[j].peBlue;
    colorTab++;
  }
}

#endif

__OWL_END_NAMESPACE

