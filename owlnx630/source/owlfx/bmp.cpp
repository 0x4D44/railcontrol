//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:59 $
//
// Source file for implementation of TBmp (TDib).
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/bmp.h>
#include <owlfx/owlfx.rh>

#define LSN(value)	((value) & 0x0f)	      /* Least-significant nibble */
#define MSN(value)	(((value) & 0xf0) >> 4)	/* Most-significant nibble  */

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

//DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlFx_Bmp, 1, 0);

/*
**	Presentation Manager (OS/2 1.x) Information Header Format.
*/
typedef struct _PmInfoHeader /* Offset   Description                     */
{
	uint32  Size;              /*  0Eh     Size of Remianing Header        */
	uint16  Width;             /*  12h     Width of Bitmap in Pixels       */
	uint16  Height;            /*  14h     Height of Bitmap in Pixels      */
	uint16  Planes;            /*  16h     Number of Planes                */
	uint16  BitCount;          /*  18h     Color Bits Per Pixel            */
} PMINFOHEAD;

/*
**  Presentation Manager (OS/2 2.0) Information Header Format.
*/
typedef struct _Pm2InfoHeader   /* Offset  Description                      */
{
	uint32  Size;               /*  0Eh    Size of Info Header (always 64)  */
	uint16  Width;              /*  12h    Width of Bitmap in Pixels        */
	uint16  Height;             /*  14h    Height of Bitmap in Pixels       */
	uint16  Planes;             /*  16h    Number of Planes                 */
	uint16  BitCount;           /*  18h    Color Bits Per Pixel             */
	uint32  Compression;        /*  1Ah    Compression Scheme (0=none)      */
	uint32  SizeImage;          /*  1Eh    Size of bitmap in bytes          */
	uint32  XPelsPerMeter;      /*  22h    Horz. Resolution in Pixels/Meter */
	uint32  YPelsPerMeter;      /*  26h    Vert. Resolution in Pixels/Meter */
	uint32  ClrUsed;            /*  2Ah    Number of Colors in Color Table  */
	uint32  ClrImportant;       /*  2Eh    Number of Important Colors       */
	uint16  Units;              /*  32h    Resolution Mesaurement Used      */
	uint16  Reserved;           /*  34h    Reserved FIelds (always 0)       */
	uint16  Recording;          /*  36h    Orientation of Bitmap            */
	uint16  Rendering;          /*  38h    Halftone Algorithm Used on Image */
	uint32  Size1;              /*  3Ah    Halftone Algorithm Data          */
	uint32  Size2;              /*  3Eh    Halftone Algorithm Data          */
	uint32  ColorEncoding;      /*  42h    Color Table Format (always 0)    */
	uint32  Identifier;         /*  46h    Misc. Field for Application Use  */
} PM2INFOHEAD;

/* -------------------------------------------------------------------------- */
/* ---                 B  M  P  format                                    --- */
//{{TBmp Implementation}}
//@mfunc Constructor
// @syntax 1. TBmp(const char* name) - create TBmp from *.bmp file
// @syntax 2. TBmp (const LPBITMAPINFO lpbi) - create TBmp around pointer to BITMAPINFO
// @syntax 3. TBmp(HGLOBAL handle, TAutoDelete autoDelete) - create TBmp around Global handle to BITMAPINFO
// @syntax 4. TBmp(const TBmp& bmp) - create TBmp from another TBmp
// @parm LPCTSTR | name | name of file to open
// @parm const LPBITMAPINFO | lpbi | pointer to BITMAPINFO
// @parm HGLOBAL | handle | handle to existing tga
// @parm TAutoDelete | autoDelete | flag if free memory on deleting
// @parm TBmp& | bmp | Reference to existed tga
TBmp::TBmp(LPCTSTR name)
:
Type(WindowsBitmap),
Compressed(false)
{
	if(!ReadFile(name))
		TXGdi::Raise(IDS_GDIFILEREADFAIL);
}
//------------------------------------------------------------------------------
TBmp::TBmp(const LPBITMAPINFO lpbi, TAutoDelete autoDelete)
:
TDib(lpbi,autoDelete),
Type(WindowsBitmap),
Compressed(false)
{
}
//------------------------------------------------------------------------------
TBmp::TBmp(HGLOBAL handle, TAutoDelete autoDelete)
:
TDib(handle, autoDelete),
Type(WindowsBitmap),
Compressed(false)
{
}
//------------------------------------------------------------------------------
TBmp::TBmp(const TBmp& bmp)
:
TDib(bmp),
Type(bmp.Type),
Compressed(bmp.Compressed)
{
}
//
TBmp::TBmp(const TDib& dib)
:
TDib(dib),
Type(WindowsBitmap),
Compressed(false)
{
}
//------------------------------------------------------------------------------
TBmp::~TBmp()
{
}
//------------------------------------------------------------------------------
// @mfunc Read a Bitmap Header
uint32 TBmp::ReadHeader(IFileIn& file, BITMAPINFOHEADER& infoHeader)
{
	// Read bitmap header size & check it. It must be one of the two known header
	// sizes.
	// Will add BITMAPV4HEADER support when available
	uint32 headerSize;
	if (!file.Read(&headerSize, sizeof headerSize)) {
		TRACE("Error read header size");
		//TXBadFormat::Raise();
		return 0;
	}
	if(headerSize == sizeof(BITMAPINFOHEADER))
		Type = WindowsBitmap;
	else if(headerSize == sizeof(PMINFOHEAD))
		Type = OS1Bitmap;
	else if(headerSize == sizeof(PM2INFOHEAD))
		Type = OS2Bitmap;
	else{
		TRACE("Not a Windows 3.x or PM 1.x or PM 2.x Bitmap file");
		//TXBadFormat::Raise();
		return 0;
	}

	// Prepare to build or read a header that is an Info (Windows) type
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// If this is a PM 1.x DIB, read the core header & copy over to the Info header
	if (Type == OS1Bitmap) {
		// Read in the rest of the core header, aborting if it is truncated
		PMINFOHEAD pm1Header;
		if (!file.Read(&pm1Header.Width, (int)headerSize-sizeof(uint32))) {
			TRACE("Invalid PM 1.x DIB Header");
			//TXBadFormat::Raise();
			return 0;
		}

		// Copy core fields over to info fields, filling in the missing pieces
		infoHeader.biWidth  				= pm1Header.Width;
		infoHeader.biHeight 				= pm1Header.Height;
		infoHeader.biPlanes 				= pm1Header.Planes;
		infoHeader.biBitCount 			= pm1Header.BitCount;
		infoHeader.biCompression 		= BI_RGB;   // No compression
		infoHeader.biSizeImage 			= 0; // Calculate this below
		infoHeader.biXPelsPerMeter 	= 0; // Zero is OK
		infoHeader.biYPelsPerMeter 	= 0; // Zero is OK
		infoHeader.biClrUsed 				= 0; // Calculate this below
		infoHeader.biClrImportant 	= 0; // Zero is OK
	}
	// if it is PM 2.x Bitmap
	else if (Type == OS2Bitmap){
		// Read in the rest of the core header, aborting if it is truncated
		PM2INFOHEAD pm2Header;
		if (!file.Read(&pm2Header.Width, (int)headerSize-sizeof(uint32))) {
			TRACE("Invalid PM 2.x DIB Header");
			//TXBadFormat::Raise();
			return 0;
		}

		// Copy core fields over to info fields, filling in the missing pieces
		infoHeader.biWidth 					= pm2Header.Width;
		infoHeader.biHeight 				= pm2Header.Height;
		infoHeader.biPlanes 				= pm2Header.Planes;
		infoHeader.biBitCount 			= pm2Header.BitCount;
		infoHeader.biCompression 		= pm2Header.Compression;
		infoHeader.biSizeImage 			= pm2Header.SizeImage;    // Calculate this below
		infoHeader.biXPelsPerMeter 	= pm2Header.XPelsPerMeter;
		infoHeader.biYPelsPerMeter 	= pm2Header.YPelsPerMeter;
		infoHeader.biClrUsed 				= pm2Header.ClrUsed;       // Calculate this below
		infoHeader.biClrImportant 	= pm2Header.ClrImportant;  // Zero is OK
	}
	// This is a Win 3.x DIB, read the info header
	else {
		// Read in the rest of the info header, aborting if it is truncated
		if (!file.Read(&infoHeader.biWidth, (int)headerSize-sizeof(uint32))) {
			TRACE("Invalid Win 3.x DIB Header");
			//TXBadFormat::Raise();
			return 0;
		}
	}

	// Check number of planes. Windows supports only 1 plane DIBs
	if (infoHeader.biPlanes != 1) {
		TRACE("Invalid number of planes in DIB");
		//TXBadFormat::Raise();
		return 0;
	}

	// Fill in the default value for biClrsUsed, if not supplied, using the
	// bit count. Will remain 0 for 16bpp or greater.
	if (!infoHeader.biClrUsed)
		infoHeader.biClrUsed = NColors(infoHeader.biBitCount);

	if (infoHeader.biCompression == BI_RLE4 ||
		infoHeader.biCompression == BI_RLE8){
			Compressed = true;
			infoHeader.biCompression = BI_RGB;
	}

	// Some applications do not fill in the biSizeImage field in the header.
	// (Actually the truth is more likely that some drivers do not fill the
	// field in and the apps do not compensate for these buggy drivers.)
	// Therefore, if compression was not used, we (re)compute the size,
	// but if compression is used, we have no choice but to trust the size.
	if (infoHeader.biCompression == BI_RGB ||
		infoHeader.biCompression == BI_BITFIELDS)
		infoHeader.biSizeImage =
		ScanBytes(infoHeader.biWidth,
		infoHeader.biBitCount) * abs(infoHeader.biHeight);
	return headerSize;
}
//------------------------------------------------------------------------------
// @mfunc Read a Bitmap Header
bool TBmp::ReadColors(IFileIn& file)
{
	// Read color mask directly into allocated memory
	if (Mask) {
		if (!file.Read((char *)Mask, 3*sizeof(uint32))) {
			TRACE("Could not read DIB color mask");
			::GlobalUnlock(Handle);
			//TXBadFormat::Raise();
			return false;
		}
	}

	// Read color table directly into allocated memory
	// Walk backwards & expand to RGBQUADs if it is a PM Core DIB
	int colorRead = (Type == OS1Bitmap) ? // Color table size on disk
		(int)NumClrs * sizeof(RGBTRIPLE) :
	(int)NumClrs * sizeof(RGBQUAD);

	if (Colors) {
		if (!file.Read((char *)Colors, colorRead)) {
			TRACE("Could not read DIB color table");
			::GlobalUnlock(Handle);
			//TXBadFormat::Raise();
			return false;
		}
		if (Type == OS1Bitmap) {
			for (int i = int(NumClrs-1); i >= 0; i--) {
				Colors[i].rgbRed = ((RGBTRIPLE*)Colors)[i].rgbtRed;
				Colors[i].rgbGreen = ((RGBTRIPLE*)Colors)[i].rgbtGreen;
				Colors[i].rgbBlue = ((RGBTRIPLE*)Colors)[i].rgbtBlue;
				Colors[i].rgbReserved = 0;
			}
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a Bitmap Header
bool TBmp::ReadBits(IFileIn& file)
{
	if(!Compressed){
		if (!file.Read(Bits, SizeImage())) {
			TRACE("Could not read DIB bits");
			//TXBadFormat::Raise();
			return false;
		}
	}
	else{
		uint8* buffer; /* Buffer to hold scan line data            */
		int16  delta,byteCount;
		uint32 offset,counter, readedBytes, skipBytes=0;

		uint16 height = (uint16)abs(Height()); 		/* number of scan lines */
		uint16 bitsInLine = (uint16)Pitch();
		const uint16 bitsRead = (uint16)(bitsInLine * 50);
		uint32 bitsAlloc = 	(uint32)bitsInLine * (uint32)height;
		TTmpBuffer<uint8> readBuf(bitsRead);

		if(Height() < 0){ /* test 5 bit if read from from top  */
			/* point to end of buffer */
			buffer = (uint8*)Bits + bitsAlloc;
			delta = (int16)-bitsInLine; /* reverse counting*/
		}
		else{
			buffer = (uint8*)Bits;
			delta = bitsInLine;
		}

		while(height){
			/* Read a scan line  */
			counter = readedBytes = file.Read((uint8*)readBuf+skipBytes, bitsRead-skipBytes);
			offset = 0;
			byteCount = 0;
			while(counter > (uint32)byteCount && height){
				/* Decode a scan line                                   */
				byteCount = DecodeScanLine(buffer, (uint8*)readBuf + offset, (uint16)(bitsInLine + 2));
				if(counter < (uint32)byteCount)
					break;

				/* to previous scan scan line                           */
				buffer += delta; 			/* becouse I create reverce order */
				height--; 						/* decrement number of scan lines */
				counter -= byteCount;	/* decrement count bytes          */
				offset 	+= byteCount; /* increment offset to read from  */
			}
			skipBytes = readedBytes - offset;
			CHECK(skipBytes >= 0);
			if(skipBytes){
				if(skipBytes>0)
					memmove((uint8*)readBuf,(uint8*)readBuf+offset, skipBytes);
				file.Skip(skipBytes);
			}
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
#define BFT_BITMAP 0x4d42   /* 'BM' */
// @mfunc Read a Bitmap image
bool TBmp::Read(IFileIn& file, bool readFileHeader)
{
	long offBits = 0;

	// Read file header and verify the signature. Grab bfOffBits if it is != 0
	// We ignore all other information in the file header since some applications
	// do not put correct information in the fields...
	if(readFileHeader){
		BITMAPFILEHEADER bmf;
		if (!file.Read(&bmf, sizeof bmf) || bmf.bfType != BFT_BITMAP) {
			TRACE("Not a Windows 3.x or PM 1.x bitmap file");
			//TXBadFormat::Raise();
			return false;
		}
		if (bmf.bfOffBits)
			offBits = bmf.bfOffBits - sizeof(BITMAPFILEHEADER);
	}

	BITMAPINFOHEADER infoHeader;
	// read Bitmap Info Header
	uint32 headerSize = ReadHeader(file, infoHeader);
	if(!headerSize)
		return false;

	// Setup all members based on InfoHeader
	InfoFromHeader(infoHeader);

	if(!ReadColors(file))
		return false;

	// Locate & Read Bits, skipping Pad if any.
	// Ignore offBits if it is zero, or if less than the current position--it's
	// probably bad
	int colorRead = (Type == OS1Bitmap) ? // Color table size on disk
		(int)NumClrs * sizeof(RGBTRIPLE) :
	(int)NumClrs * sizeof(RGBQUAD);
	if (offBits && offBits - (long)(headerSize + colorRead) > 0)
		file.Skip(offBits - (headerSize + colorRead));

	if(!ReadBits(file))
		return false;

	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a BMP file
// @parm  IFileOut& | file | BMP image file output
bool TBmp::WriteInfoHeader(IFileOut& file)
{
	int colorWrite = (Type == OS1Bitmap) ? // Color table size on disk
		(int)NumClrs * sizeof(RGBTRIPLE) :
	(int)NumClrs * sizeof(RGBQUAD);

	// write Windows Bitmap
	if(Type == WindowsBitmap){
		// Prepare to write a header that is an Info (Windows) type
		BITMAPINFOHEADER infoHeader;
		infoHeader.biSize = sizeof(BITMAPINFOHEADER);
		::CopyMemory(&infoHeader, Info, infoHeader.biSize);

		if(Compressed && NumClrs)
			infoHeader.biCompression = NumClrs == 16 ? BI_RLE4 : BI_RLE8;

		// Write bits
		if (!file.Write(Info, infoHeader.biSize))
			return false;

		if(Colors){
			if (!file.Write(Colors, colorWrite))
				return false;
		}
	}
	// Write OS/2 Bitmap
	else if(Type == OS2Bitmap){
		PM2INFOHEAD pm2Header;
		pm2Header.Size           = sizeof(PM2INFOHEAD);
		pm2Header.Width          = (uint16)Info->bmiHeader.biWidth;
		pm2Header.Height         = (uint16)Info->bmiHeader.biHeight;
		pm2Header.Planes         = Info->bmiHeader.biPlanes;
		pm2Header.BitCount       = Info->bmiHeader.biBitCount;
		pm2Header.Compression    = Info->bmiHeader.biCompression;
		pm2Header.SizeImage      = Info->bmiHeader.biSizeImage;
		pm2Header.XPelsPerMeter  = Info->bmiHeader.biXPelsPerMeter;
		pm2Header.YPelsPerMeter  = Info->bmiHeader.biYPelsPerMeter;
		pm2Header.ClrUsed        = Info->bmiHeader.biClrUsed;
		pm2Header.ClrImportant   = Info->bmiHeader.biClrImportant;
		pm2Header.Units          = 0;
		pm2Header.Reserved       = 0;
		pm2Header.Recording      = 0;
		pm2Header.Rendering      = 0;
		pm2Header.Size1          = 0;
		pm2Header.Size2          = 0;
		pm2Header.ColorEncoding  = 0;
		pm2Header.Identifier     = 0;

		//if(Compressed && NumClrs)
		//	pm2Header.Compression = NumClrs == 16 ? BI_RLE4 : BI_RLE8;

		if (!file.Write(&pm2Header, pm2Header.Size))
			return false;

		if(Colors){
			if (!file.Write(Colors, colorWrite) != colorWrite)
				return false;
		}
	}
	// Write OS/1 Bitmap
	else if(Type == OS1Bitmap){

		PMINFOHEAD  pm1Header;
		pm1Header.Size           = sizeof(PMINFOHEAD);
		pm1Header.Width          = (uint16)Info->bmiHeader.biWidth;
		pm1Header.Height         = (uint16)Info->bmiHeader.biHeight;
		pm1Header.Planes         = Info->bmiHeader.biPlanes;
		pm1Header.BitCount       = Info->bmiHeader.biBitCount;

		if (!file.Write(&pm1Header, pm1Header.Size))
			return false;

		if(Colors){
			TTmpBuffer<RGBTRIPLE> colors(NumClrs);
			for(int i = 0; i < NumClrs; i++){
				colors[i].rgbtRed 	= Colors[i].rgbRed;
				colors[i].rgbtGreen = Colors[i].rgbGreen;
				colors[i].rgbtBlue  = Colors[i].rgbBlue;
			}
			if (!file.Write((RGBTRIPLE*)colors, colorWrite))
				return false;
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the bits to a BMP file
// @parm  MFile& | file | TGA image file output
bool TBmp::WriteBits(IFileOut& file)
{
	// Write bits
	if(Compressed && Type == WindowsBitmap){

		uint16  bufSize;        /* Size of the scan line buffer             */
		int     byteCount;      /* Number of bytes in a buffer              */
		uint32  offset = 0;

		/* I wand to write only one type of bits order:    */
		/* from left to right &  from bottom to  top       */
		uint   byteInLine = Pitch();
		uint16 height = (uint16)abs(Height());

		/* Calculate the encoded buffer size in bytes.  The worst case size  */
		/* is based on every pixel in the scan line being encoded as a       */
		/* single, raw run-length packet.                                    */
		bufSize = (uint16)(byteInLine + 2);
		//bufSize = (bmpHead.WinInfo.Width * ((bmpHead.WinInfo.BitCount + 7) >> 3) + 2);

		/* Allocate buffer memory */
		TTmpBuffer<uint8> buffer(bufSize);

		/* Encode a scan line and write it to the buffer        */
		for (short i = 0; i < height; i++){

			/* Encode a scan line                                         */
			if((byteCount = EncodeScanLine(buffer, (uint8*)Bits + offset, (uint16)bufSize)) < 0){
				//TXGdi::Raise(IDS_GDIFILECOMPFAIL);
				return false;
			}

			/* Write the encoded scan line to file       */
			if(!file.Write(buffer, byteCount)){
				//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
				return false;
			}

			offset += byteInLine;
		}
		/* Write end of bitmap escape code to file       */
		uint8 esc = 0;
		file.Write(&esc, 1);
		esc = 1;
		file.Write(&esc, 1);
	}
	// if not Compressed or OS/1-2 bitmap
	else{
		if (!file.Write(Bits, SizeImage()))
			return false;
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a BMP file
// @parm  MFile& | file | TGA image file output
bool TBmp::Write(IFileOut& file, bool writeFileHeader)
{
	int colorWrite = (Type == OS1Bitmap) ? // Color table size on disk
		(int)NumClrs * sizeof(RGBTRIPLE) :
	(int)NumClrs * sizeof(RGBQUAD);
	int maskAlloc = GetInfoHeader()->biCompression==BI_BITFIELDS ? 3*sizeof(uint32) : 0;
	uint32 size = maskAlloc + colorWrite + SizeImage();

	if(Type == WindowsBitmap)
		size += sizeof(BITMAPINFOHEADER);
	else if(Type == OS1Bitmap)
		size += sizeof(PMINFOHEAD);
	else if(Type == OS2Bitmap)
		size += sizeof(PM2INFOHEAD);

	// Write file header
	if (writeFileHeader) {
		BITMAPFILEHEADER bmf;
		bmf.bfType = BFT_BITMAP; //'BM';
		bmf.bfSize = sizeof(bmf) + size;
		bmf.bfReserved1 = 0;
		bmf.bfReserved2 = 0;
		bmf.bfOffBits = sizeof bmf + (char *)Bits - (char *)Info;
		if (!file.Write(&bmf, sizeof bmf))
			return false;
	}

	// Write dib header & color table
	if(!WriteInfoHeader(file))
		return false;

	return WriteBits(file);
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a BMP file
/*
This function encodes raw BMP data into 4-bit or 8-bit BMP RLE data.
The algorithm always uses encoded runs for pixel runs greater than
five pixels in length, otherwise raw runs are used.  Delta and end
of bitmap escape sequences are not included in the encoding.  The
end of bitmap escape code must be written by the function calling
BmpEncodeData().
*/

/*
Read encoded data from a FILE stream and write to a buffer.

Pixels are either 4-bits or 8-bits in size.  The Method parameter
indcates the size with a value of COMPRESS_RLE4 or COMPRESS_RLE8.

Note that this code only writes absolute runs of pixels that are the same
value.  Literal runs may just as easily represent a line of pixels with
different values.

For 4-bit pixels the MSN (Most Significant Nibble) is the first pixel
value and the LSN (Least Significant Nibble) is the second pixel value.
This particular algorithm encodes 4-bit per pixel data two nibbles at a time.
In other words, if you had the raw run "11 11 15" only first four nibbles
would be encoded in the run.  The fifth nibble would be treated part of
of the next run.  Not the most efficient scheme, but it
simplifies the algorithm by not needing to tear apart bytes into sparate
nibble values and add padding for odd-length pixel runs.  So there.

**  Method may have one of the following values:
**
**      0 - Unencoded
**      1 - Four bits per pixel
**      2 - Eight bits per pixel
*/

//@parm	uint8* | EncodedBuffer | Pointer to buffer to hold encodeded scan line
//@parm	uint16 | BufferSize |  Size of buffer holding unencoded data
int16 TBmp::EncodeScanLine( uint8* EncodedBuffer, uint8* src, uint16 BufferSize)
{
	int16	  runCount;       /* The number of pixels in the current run      */
	uint16 	pixelCount;     /* The number of pixels read from the scan line */
	int16 	bufIndex;       /* The index of DecodedBuffer                   */
	uint8 	pixelValue1;    /* Pixel value read from the scan line (4-byte max) */
	uint8 	pixelValue2;    /* Pixel value read from the scan line          */
	int16  	byteCount = 0;  /* Number of bytes reading from src buffer      */

	uint8   delta = uint8(BitsPixel() == 4 ? 2 : 1);
	uint    LineLength = Width();

	bufIndex   = 0;

	runCount   = pixelCount = delta;

	/* Read in first pixel value */
	pixelValue1 = *src++;

	/* Main encoding loop */
	for (;;){

		/* Read in another pixel value */
		pixelValue2 = *src++;

		/* Count number of pixels read so far */
		pixelCount += delta;

		/* If the pixels are the same then start or continue a run */
		if (pixelValue1 == pixelValue2){  /* Compare pixels */
			/* Advance the run count */
			runCount += delta;

			if (runCount < 256){     /* Maximum run-length is 256 pixels     */
				if ((uint)pixelCount < LineLength)  /* Don't run past end of scan line */
					continue;       /* Continue reading the run             */
			}
			else{
				pixelValue2 = *src++;
				byteCount++;
			}
		}

		/* If we have gotten this far then we have either come to the end of */
		/* a pixel run, have reached the maximum number of pixels encodable  */
		/* in a run, or read to the end of the scan line.  Now encode the    */
		/* current run.                                                      */
		/* Literal runs must have a runCount greater than 2 or the           */
		/* literal run indicator will be confused with an escape code.       */
		/* This scheme will also only encode even-length runs as literal     */
		/* runs.  This frees us from keeping track of left over nibbles      */
		/* from odd-length runs.                                             */
		if (runCount > 2 && runCount < 8){ /* Write a Literal Run*/
			EncodedBuffer[bufIndex++] = 0;          /* Literal Run indicator  */
			EncodedBuffer[bufIndex++] = (uint8)runCount;   /* Number of pixels in run */
			byteCount += 2;

			runCount /= delta;

			bool align = runCount & 1u; // check if value odd

			/* Write the pixel data run */
			byteCount += runCount;
			while (runCount--)
				EncodedBuffer[bufIndex++] = pixelValue1;
			if(align){
				EncodedBuffer[bufIndex++] = 0;
				byteCount++;
			}
		}
		else {               /* Write an Encoded Run */
			//printf("runCount: %d  value %d\n", runCount, pixelValue1);
			EncodedBuffer[bufIndex++] = (uint8)runCount;/* Number of pixels in run */
			EncodedBuffer[bufIndex++] = pixelValue1;		/* Value of pixels in run */
			byteCount += 2;
		}

		/* Start a new pixel run count  */
		runCount = delta;

		/* If we've encoded the entire line then break out of the loop */
		if(pixelCount == LineLength && pixelValue1 == pixelValue2)
			break;

		if(pixelCount == LineLength && pixelValue1 != pixelValue2) {
			EncodedBuffer[bufIndex++] = (uint8)runCount;/* Number of pixels in run */
			EncodedBuffer[bufIndex++] = pixelValue2;		/* Value of pixels in run */
			byteCount += 2;
			break;
		}

		/* Store next pixel value run to match */
		pixelValue1 = pixelValue2;
	}

	/* Write an End of Scan Line Escape Code */
	EncodedBuffer[bufIndex++] = 0;
	EncodedBuffer[bufIndex] = 0;
	byteCount += 2;

	//TRACE("TBmp::EncodeScanLine(): 1 return: byteCount: " << byteCount);
	return byteCount;
}

/*
This function decodes 4-bit and 8-bit BMP RLE image data.  Note the
delta escape sequences are not supported because this function can
only decode one scan line at a time.
*/
int16 TBmp::DecodeScanLine(uint8* DecodedBuffer, uint8* src, uint16 BufferSize)
{
	uint8		runCount;       /* Number of pixels in the run  */
	uint8		runValue;       /* Value of pixels in the run   */
	uint8		Value;          /* Temporary pixel value holder */
	uint		bufIndex;       /* The index of DecodedBuffer   */
	int			byteCount = 0;  /* Number of bytes reading from src buffer      */

	bufIndex = 0;         /* Initialize the buffer index  */

	/* Main decoding loop */
	while (bufIndex < BufferSize){
		runCount = *src++;  /* Number of pixels in the run */
		runValue = *src++;  /* Value of pixels in the run  */
		byteCount += 2;

		switch(runCount){
		case 0:        /* Literal Run or Escape Code */
			switch(runValue){
		case 0:             /* End of Scan Line Escape Code */
			//TRACE("TBmp::DecodeScanLine(): End of scan line Code");
			//TRACE("TBmp::DecodeScanLine(): End of scan line Code ( byteCount: " << byteCount << ", bufIndex: " << bufIndex << " )");
			return byteCount;
		case 1:             /* End of Bitmap Escape Code */
			//TRACE("TBmp::DecodeScanLine(): End of bitmap Code");
			//TRACE("TBmp::DecodeScanLine(): End of bitmap Code ( byteCount: " << byteCount << ", bufIndex: " << bufIndex << " )");
			return byteCount;
		case 2:             /* Delta Escape Code (not supported) */
			//TRACE("TBmp::DecodeScanLine(): Delta Escape Codes not supported!");
			//TRACE("TBmp::DecodeScanLine(): Delta Escape Codes not supported!");
			return byteCount;
		default:            /* Literal Run */
			/* Check for a possible buffer overflow */
			if (bufIndex + runValue > BufferSize){
				TRACE("TBmp::DecodeScanLine(): 1 - Buffer overflow");
				return byteCount;
			}
			if (BitsPixel() == 8){
				bool get1 = runValue & 1u;  // check if value odd
				while (runValue--){
					DecodedBuffer[bufIndex++] = *src++;
					byteCount++;
				}
				// keep file word-aligned
				if(get1){
					*src++;
					byteCount++;
				}
			}
			else if (BitsPixel() == 4) {
				/* Alternate writing the most-significant and      */
				/* Least-significant nibble to the buffer.  The    */
				/* odd-length literal runs are a bit tricky.       */
				while (runValue--){
					Value = *src++;
					byteCount++;
					DecodedBuffer[bufIndex] = (uint8)(MSN(Value) << 4);
					if (runValue--)
						DecodedBuffer[bufIndex++] |= (uint8)LSN(Value);
				}
				uint8 get1 = runValue; // check if value odd
				if((get1 & 2) == 1 || (get1 & 3) == 2){
					*src++;
					byteCount++;
				}
			}
			}
			break;
		default:    /* Encoded Run */
			if (BitsPixel() == 4){            /* Write a 4-bit value */
				/* Check for a possible buffer overflow */
				if (bufIndex + (runCount / 2) > BufferSize){
					TRACE("TBmp::DecodeScanLine(): 2 - Buffer overflow");
					return byteCount;
				}
				/* Alternate writing the most-significant and      */
				/* Least-significant nibble to the buffer.         */
				while (runCount--){
					DecodedBuffer[bufIndex] = (uint8)(MSN(runValue) << 4);
					if (runCount--)
						DecodedBuffer[bufIndex++] |= (uint8)LSN(runValue);
				}
			}
			else{ /* Write an 8-bit value */
				//TRACE("bufIndex %d  runCount %d  BufferSize %d\n", bufIndex, runCount, BufferSize);
				/* Check for a possible buffer overflow */
				if (bufIndex + runCount > BufferSize){
					TRACE("TBmp::DecodeScanLine(): 3 - Buffer overflow (byteCount: " << byteCount << ", bufIndex: " << bufIndex << " )");
					return byteCount;
				}

				while (runCount--)
					DecodedBuffer[bufIndex++] = runValue;
			}
			break;
		}
	}
	//TRACE("TBmp::DecodeScanLine(): No End of Scan line code");
	return byteCount;
}
//
bool TBmpDescriptor::GetDescription(LPTSTR buffer, int size)
{
	return gModule->LoadString(IDS_BMP_DESCRIPTOR, buffer, size) != 0;
}
//
TDib* TBmpDescriptor::OpenInstance(LPCTSTR path)
{
	return new TBmp(path);
}
//
TDib* TBmpDescriptor::CreateInstance(const TDib& dib)
{
	return new TBmp(dib);
}
//
bool TBmpDescriptor::VerifyFile(LPCTSTR)
{
	// ....
	return false;
}
//
bool TBmpDescriptor::GetCapabilities(uint& pixType, uint& addit)
{
	pixType = pixcARGB|pixcRGB|pixcRGB555|pixcRGB_INDEX;
	addit   = cabCanConfig|cabCanRead|cabCanWrite;
	return true;
}
//
TDialog* TBmpDescriptor::GetConfigDlg(TDib&, TWindow*)
{
	return 0;//?????????????????????????????????
}
//
_OWLFX_END_NAMESPACE

/* ========================================================================== */
