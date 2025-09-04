//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//
// Source file for implementation of TGif (TDib).
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/gif.h>
#include <owlfx/owlfx.rh>

#define LSN(value)	((value) & 0x0f)	      /* Least-significant nibble */
#define MSN(value)	(((value) & 0xf0) >> 4)	/* Most-significant nibble  */

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

//DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlFx_Bmp, 1, 0);

/* -------------------------------------------------------------------------- */
/* ---                 B  M  P  format                                    --- */
//{{TGif Implementation}}
//@mfunc Constructor
// @syntax 1. TGif(const char* name) - create TGif from *.bmp file
// @syntax 2. TGif (const LPBITMAPINFO lpbi) - create TGif around pointer to BITMAPINFO
// @syntax 3. TGif(HGLOBAL handle, TAutoDelete autoDelete) - create TGif around Global handle to BITMAPINFO
// @syntax 4. TGif(const TGif& bmp) - create TGif from another TGif
// @parm LPCTSTR | name | name of file to open
// @parm const LPBITMAPINFO | lpbi | pointer to BITMAPINFO
// @parm HGLOBAL | handle | handle to existing tga
// @parm TAutoDelete | autoDelete | flag if free memory on deleting
// @parm TGif& | bmp | Reference to existed tga
TGif::TGif(LPCTSTR name)
{
	if(!ReadFile(name))
		TXGdi::Raise(IDS_GDIFILEREADFAIL);
}
//------------------------------------------------------------------------------
TGif::TGif(const LPBITMAPINFO lpbi, TAutoDelete autoDelete)
:
TDib(lpbi,autoDelete)
{
}
//------------------------------------------------------------------------------
TGif::TGif(HGLOBAL handle, TAutoDelete autoDelete)
:
TDib(handle, autoDelete)
{
}
//------------------------------------------------------------------------------
TGif::TGif(const TGif& bmp)
:
TDib(bmp)
{
}
//
TGif::TGif(const TDib& dib)
:
TDib(dib)
{
}
//------------------------------------------------------------------------------
TGif::~TGif()
{
}
//------------------------------------------------------------------------------
// @mfunc Read a Bitmap Header
uint32 TGif::ReadHeader(IFileIn& file, BITMAPINFOHEADER& infoHeader)
{
	return 0;
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a Bitmap Header
bool TGif::ReadBits(IFileIn& file)
{
	return false;
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a Bitmap image
bool TGif::Read(IFileIn& file, bool readFileHeader)
{
	return false;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a BMP file
// @parm  IFileOut& | file | BMP image file output
bool TGif::WriteInfoHeader(IFileOut& file)
{
	return false;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the bits to a BMP file
// @parm  MFile& | file | TGA image file output
bool TGif::WriteBits(IFileOut& file)
{
	return false;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a BMP file
// @parm  MFile& | file | TGA image file output
bool TGif::Write(IFileOut& file, bool writeFileHeader)
{
	return false;
}
//@parm	uint8* | EncodedBuffer | Pointer to buffer to hold encodeded scan line
//@parm	uint16 | BufferSize |  Size of buffer holding unencoded data
int16 TGif::EncodeScanLine( uint8* EncodedBuffer, uint8* src, uint16 BufferSize)
{
	return 0;
}

/*
This function decodes 4-bit and 8-bit BMP RLE image data.  Note the
delta escape sequences are not supported because this function can
only decode one scan line at a time.
*/
int16 TGif::DecodeScanLine(uint8* DecodedBuffer, uint8* src, uint16 BufferSize)
{
	return 0;
}
//
bool TGifDescriptor::GetDescription(LPTSTR buffer, int size)
{
	return gModule->LoadString(IDS_BMP_DESCRIPTOR, buffer, size) != 0;
}
//
TDib* TGifDescriptor::OpenInstance(LPCTSTR path)
{
	return new TGif(path);
}
//
TDib* TGifDescriptor::CreateInstance(const TDib& dib)
{
	return new TGif(dib);
}
//
bool TGifDescriptor::VerifyFile(LPCTSTR path)
{
	// ....
	return false;
}
//
bool TGifDescriptor::GetCapabilities(uint& pixType, uint& addit)
{
	pixType = pixcRGB_INDEX;
	addit   = cabCanConfig|cabCanRead|cabCanWrite;
	return true;
}
//
TDialog* TGifDescriptor::GetConfigDlg(TDib& dib, TWindow* parent)
{
	return 0;//?????????????????????????????????
}
//
_OWLFX_END_NAMESPACE

/* ========================================================================== */
