//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//
// Source file for implementation of TPcx (TDib).
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/pcx.h>
#include <owlfx/owlfx.rh>

#if 0
#undef DENABLE
#undef DOUT
#undef DOUTNAME
#define DENABLE(_state_) static bEnable = _state_;
#define DOUT(_lv_,_str_) if(bEnable && iLevel >= (_lv_)) _dbgstr_ << _str_ << endl
#define DOUTNAME(_Level_, _name_) static bool bEnable = true; static int iLevel = _Level_; static fstream _dbgstr_(_name_, ios::out)
DOUTNAME(3, "pcxfmt.log");
#endif

#define PCX_IDNUMBER 0x0A


_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE


/* -------------------------------------------------------------------------- */
/* ---                 P  C  X  format                                    --- */
//{{TPcx Implementation}}
//@mfunc Constructor
// @syntax 1. TPcx(const char* name) - create TPcx from *.pcx file
// @syntax 2. TPcx (const LPBITMAPINFO lpbi) - create TPcx around pointer to BITMAPINFO
// @syntax 3. TPcx(HGLOBAL handle, MAutoDelete autoDelete) - create TPcx around Global handle to BITMAPINFO
// @syntax 4. TPcx(const TPcx& pcx) - create TPcx from another TPcx
// @parm const char* | name | name of file to open
// @parm const LPBITMAPINFO | lpbi | pointer to BITMAPINFO
// @parm HGLOBAL | handle | handle to existing tga
// @parm MAutoDelete | autoDelete | flag if free memory on deleting
// @parm TPcx& | pcx | Reference to existed tga
TPcx::TPcx(LPCTSTR name)
:
RunCount(0),
RunValue(0)
{
	if(!ReadFile(name))
		TXGdi::Raise(IDS_GDIFILEREADFAIL);
}
/* -------------------------------------------------------------------------- */
TPcx::TPcx(const LPBITMAPINFO lpbi, TAutoDelete autoDelete)
:
TDib(lpbi,autoDelete),
RunCount(0),
RunValue(0)
{
	Init();
}
/* -------------------------------------------------------------------------- */
TPcx::TPcx(HGLOBAL handle, TAutoDelete autoDelete)
:
TDib(handle, autoDelete),
RunCount(0),
RunValue(0)
{
	Init();
}
/* -------------------------------------------------------------------------- */
TPcx::TPcx(const TPcx& pcx)
:
TDib(pcx),
RunCount(0),
RunValue(0)
{
	memcpy(&Header, &pcx.Header, sizeof(Header));
}
//
TPcx::TPcx(const TDib& pcx)
:
TDib(pcx),
RunCount(0),
RunValue(0)
{
	Init();
}
//
TPcx::TPcx()
:
RunCount(0),
RunValue(0)
{
	Init();
}
/* -------------------------------------------------------------------------- */
//@mfunc Destructor
TPcx::~TPcx()
{
}
//
void TPcx::Init()
{
	Header.Id             = PCX_IDNUMBER; /*  00h     Manufacturer ID        */
	Header.Version        = 5;            /*  01h     Version                */
	Header.Compression    = 1;            /*  02h     Encoding Scheme        */
	Header.BitsPixelPlane = 8;            /*  03h     Bits/Pixel/Plane       */
	Header.Xmin           = 0;            /*  04h     X Start (upper left)   */
	Header.Ymin           = 0;            /*  06h     Y Start (top)          */
	Header.Xmax           = uint16(Width() - 1);  /*  08h     X End (lower right)    */
	Header.Ymax           = uint16(Height() - 1); /*  0Ah     Y End (bottom)         */
	Header.Hdpi           = 0;            /*  0Ch     Horizontal Resolution  ??????????*/
	Header.Vdpi           = 0;            /*  0Eh     Vertical Resolution    ??????????????????*/
	memset(&Header.EgaPalette, 0, sizeof(Header.EgaPalette)); /*  10h  16-Color EGA Palette   */
	Header.Reserved       = 0;            /*  40h     Reserved               */
	Header.NumberOfPlanes = uint16(BitsPixel() == 8 ? 1 : 3); /*  41h Number of Color Planes */
	Header.BytesLinePlane = uint16(Width()%2 ? Width()+1 : Width());/* 42h Bytes/Line/Plane       */
	Header.PaletteInfo    = 1;            /*  44h     Palette Interpretation */
	Header.HScreenSize    = 0;            /*  46h     Horizontal Screen Size */
	Header.VScreenSize    = 0;            /*  48h     Vertical Screen Size   */
	memset(&Header.Filler,0,sizeof(Header.Filler));/*  4Ah     Reserved               */
}
/* -------------------------------------------------------------------------- */
// @mfunc Read the information from a PCX image file header.
//  PCX header file information is read fromn the supplied FILE
//  stream and written to the supplied PCX header structure.
//  The ENDIANIO I/O finctions are used to insure that the header
//  information is written in little-endian order.
//  Returns: Nothing.
uint32 TPcx::ReadHeader(IFileIn& file, BITMAPINFOHEADER& infoHeader)
{
	// Read bitmap header size & check it. It must be one of the two known header
	// sizes.
	if (!file.Read(&Header, sizeof(Header))){
		TRACE("Error Read PCX header");
		//TXGdi::Raise(IDS_GDIFILEREADFAIL);
		return 0;
	}

	if(Header.Id != PCX_IDNUMBER){
		TRACE("Error Not a PCX format.");
		//TXGdi::Raise(IDS_GDIFILEREADFAIL);
		return 0;
	}

	if(Header.Version < 5){
		TRACE("Error Unsupported PCX version: " << Header.Version );
		//TXGdi::Raise(IDS_GDIFILEREADFAIL);
		return 0;
	}
	uint16 BitCount = (uint16)(Header.NumberOfPlanes * Header.BitsPixelPlane);
	if(BitCount != 8 && BitCount != 24){
		TRACE("Error: Unsupported Color depth: " << BitCount << ", NumberOfPlanes: " << Header.NumberOfPlanes << ",BitsPixelPlane: " << Header.BitsPixelPlane);
		//TXGdi::Raise(IDS_GDIFILEREADFAIL);
		return 0;
	}


	// Prepare to build or read a header that is an Info (Windows) type
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Copy core fields over to info fields, filling in the missing pieces
	infoHeader.biWidth  				= Header.Xmax - Header.Xmin + 1;
	infoHeader.biHeight 				= Header.Ymax - Header.Ymin + 1;
	infoHeader.biPlanes 				= 1;
	infoHeader.biBitCount 			= BitCount;
	infoHeader.biCompression 		= BI_RGB;   // No compression
	infoHeader.biSizeImage 			= 0; // Calculate this below
	infoHeader.biXPelsPerMeter 	= 0; // Zero is OK
	infoHeader.biYPelsPerMeter 	= 0; // Zero is OK
	infoHeader.biClrUsed 				= 0; // Calculate this below
	infoHeader.biClrImportant 	= 0; // Zero is OK


	// Fill in the default value for biClrsUsed, using the bit count.
	// Will remain 0 for 16bpp or greater.
	infoHeader.biClrUsed = NColors(infoHeader.biBitCount);

	// Some applications do not fill in the biSizeImage field in the header.
	// (Actually the truth is more likely that some drivers do not fill the
	// field in and the apps do not compensate for these buggy drivers.)
	// Therefore, if compression was not used, we (re)compute the size,
	// but if compression is used, we have no choice but to trust the size.
	infoHeader.biSizeImage = ScanBytes(infoHeader.biWidth,
		infoHeader.biBitCount) * infoHeader.biHeight;
	return sizeof(Header);
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a Bitmap Header
bool TPcx::ReadColors(IFileIn& file)
{
	// Read color table directly into allocated memory
	// Walk backwards & expand to RGBQUADs if it is a PM Core DIB
	int colorRead = (int)NumClrs * sizeof(RGBTRIPLE);

	if (Colors) {
		uint8 c;
		if(!file.Read(&c, sizeof c) || c !=  0x0C){     /* Is it the proper value? */
			TRACE("Isn't valid Id for PCX color table.");
			::GlobalUnlock(Handle);
			return false;
		}

		if (!file.Read((char *)Colors, colorRead)) {
			TRACE("Could not read PCX color table");
			::GlobalUnlock(Handle);
			return false;
		}
		for (int i = int(NumClrs-1); i >= 0; i--) {
			Colors[i].rgbRed = ((RGBTRIPLE*)Colors)[i].rgbtRed;
			Colors[i].rgbGreen = ((RGBTRIPLE*)Colors)[i].rgbtGreen;
			Colors[i].rgbBlue = ((RGBTRIPLE*)Colors)[i].rgbtBlue;
			Colors[i].rgbReserved = 0;
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a PCX Header
bool TPcx::ReadBits(IFileIn& file)
{
	// Undecoding
	/* Length of uncompressed scan line  */
	uint errorBreak = 0, byteCount;
	uint32 offset, counter,readedBytes, skipBytes=0;
	uint8  *r, *g, *b, *p;
	uint   numRgb;
	uint   bitsInLine = Header.NumberOfPlanes * Header.BytesLinePlane;
	TTmpBuffer<uint8> pcxBits(bitsInLine);
	uint	height = abs((int)Height()); 		/* number of scan lines */
	const uint bitsRead = bitsInLine * 50;
	TTmpBuffer<uint8> readBuf(bitsRead);
	uint8* buffer = (uint8*)Bits + Pitch() * (height - 1); /* Buffer to hold scan line data            */
	int		delta = - Pitch();
	uint8 oldCount,oldValue;

	RunCount = 0u;
	RunValue = 0u;

	while(height){
		/* Read a scan line  */
		readedBytes = counter = file.Read((uint8*)readBuf+skipBytes, bitsRead-skipBytes);
		if(!counter)
			break;
		offset = 0;
		byteCount = 0;
		while(counter > byteCount && height){
			oldCount = RunCount;
			oldValue = RunValue;
			if(Header.Compression){
				/* Decode a scan line                                   */
				// destination | source  | size of source
				byteCount = DecodeScanLine(pcxBits, (uint8*)readBuf + offset, (uint16)bitsInLine);
			}
			else{
				byteCount = bitsInLine;
				memcpy(pcxBits, (uint8*)readBuf + offset, bitsInLine);
			}

			if(counter < byteCount && height > 1){
				RunCount = oldCount;
				RunValue = oldValue;
				//OUT(4, "PCXFormat::ReadBits() ReReading Data height(" << dec << height <<"), counter(" << dec << counter << ") < byteCount(" << dec << byteCount<< ")");
				if(errorBreak++ > 10){
					//DOUT(0, "PCXFormat::ReadBits() error reading data.");
					return false;
				}
				break;
			}

			// ------ set data from scan line to our bitmap --------------------------
			if(BitsPixel() == 24){
				numRgb = Header.BytesLinePlane;
				r = pcxBits;
				g = r + numRgb;
				b = g + numRgb;
				p = buffer;
				while(numRgb--){
					*p++ = *b++;
					*p++ = *g++;
					*p++ = *r++;
				}
			}
			else
				memcpy(buffer, pcxBits, bitsInLine);

			/* to previous scan line                                */
			buffer += delta; 			/* becouse I create reverce order */
			height--; 						/* decrement number of scan lines */
			counter -= byteCount;	/* decrement count bytes          */
			offset 	+= byteCount; /* increment offset to read from  */
		}
		skipBytes = readedBytes - offset;
		CHECK(skipBytes >= 0);
		if(skipBytes){
			if(skipBytes > 0)
				memmove((uint8*)readBuf,(uint8*)readBuf+offset, skipBytes);
			file.Skip(skipBytes);
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a PCX image
bool TPcx::Read(IFileIn& file, bool readFileHeader)
{
	if(readFileHeader){
		BITMAPINFOHEADER infoHeader;
		// read Pcx Header
		if(!ReadHeader(file, infoHeader))
			return false;

		// Setup all members based on InfoHeader
		InfoFromHeader(infoHeader);
	}

	if(!ReadBits(file))
		return false;

	return ReadColors(file);
}
/* -------------------------------------------------------------------------- */
/* @mfunc Write the information to a PCX file
Write the information from a PCX image file structure.
PCX header file information is read from the supplied PCX
header structure and written to the supplied FILE stream.
The ENDIANIO I/O finctions are used to insure that the header
information is written in little-endian order.
Returns: Nothing.
*/
// @parm  IFileOut& | file | PCX image file output
bool TPcx::WriteHeader(IFileOut& file)
{
	if(!file.Write(&Header, sizeof(Header))){
		TRACE("Error Write PCX header");
		return false;
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the bits to a BMP file
// @parm  MFile& | file | TGA image file output
bool TPcx::WriteBits(IFileOut& file)
{
	/* Calculate the encoded buffer size in bytes.  The worst case size  */
	/* is based on every pixel in the scan line being encoded as a       */
	/* single, raw run-length packet.                                    */
	uint16  bufSize = 2048; /* Size of the scan line buffer              */
	int     byteCount;      /* Number of bytes in a buffer               */

	/* I wand to write only one type of bits order:    */
	/* from left to right &  from bottom to  top       */
	uint   byteInLine = Pitch();
	uint16 height = (uint16)abs(Height());
	uint16 actualLineLength = uint16(Header.BytesLinePlane * Header.NumberOfPlanes);

	TTmpBuffer<uint8> __ClnObj(actualLineLength);
	uint8* buffer = __ClnObj;

	/* Allocate buffer memory */
	uint8* data = (uint8*)Bits + byteInLine * (height - 1);

	TTmpBuffer<uint8> ebuffer(bufSize);

	/* Encode a scan line and write it to the buffer        */
	while(height--){

		// set buffer if needed
		if(BitsPixel() == 24){
			uint16 partSize = Header.BytesLinePlane;
			int idx;
			for(short i = 0; i < Width(); i++){
				idx = i*3;
				buffer[i]              = data[idx + 2]; // Blue
				buffer[partSize + i]   = data[idx + 1]; // Green
				buffer[partSize*2 + i] = data[idx];     // Red
			}
		}
		else
			memcpy(buffer, data, actualLineLength);

		if(Header.Compression){
			/* Encode a scan line                                         */
			if((byteCount = EncodeScanLine(ebuffer, (uint8*)buffer, actualLineLength)) < 0){
				//TXGdi::Raise(IDS_GDIFILECOMPFAIL);
				return false;
			}

			/* Write the encoded scan line to file       */
			if(!file.Write(ebuffer, byteCount)){
				//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
				return false;
			}
		}
		else{
			/* Write the scan line to file       */
			if(!file.Write(buffer, actualLineLength))
				return false;
		}	
		data -= byteInLine;
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write colors to a PCX file
// @parm  MFile& | file | PCX image file output
bool TPcx::WriteColors(IFileOut& file)
{
	if(!Colors)
		return true;

	/* If there is a VGA palette then read and append it to the output file */
	TPcxPalette PcxVgaPal;     /* PCX VGA palette structure */

	uint8 byte = 0x0c;
	if(!file.Write(&byte, sizeof(byte))){
		TRACE("Error Write PCX colors Id");
		return false;
	}
	int j;
	/* Create the VGA palette */
	for (int i = 0; i < 256; i++){
		j = i*3;
		PcxVgaPal.Palette[j]   = Colors[i].rgbRed;
		PcxVgaPal.Palette[j+1] = Colors[i].rgbGreen;
		PcxVgaPal.Palette[j+2] = Colors[i].rgbBlue;
	}
	/* Write the VGA palette */
	if(!file.Write(&PcxVgaPal, sizeof(PcxVgaPal))){
		TRACE("Error Write PCX colors");
		return false;
	}
	return true;
}
/* -------------------------------------------------------------------------- */
// @mfunc Write the information to a PCX file
// @parm  IFileOut& | file | TGA image file output
bool TPcx::Write(IFileOut& file, bool writeFileHeader)
{
	// Write file header
	if(writeFileHeader)
		if(!WriteHeader(file))
			return false;
	if(!WriteBits(file))
		return false;

	return WriteColors(file);
}
/* -------------------------------------------------------------------------- */
/****************************************************************************\
**	PCX Run-length Encoder                                                  **
**	Run-length encoding is as easy as decoding.  This function encodes a    **
**	single scan line of data at a time and will not encode across scan      **
**	lines.                                                                  **
**                                                                          **
**  Compress an image scan line using the PCX run-length encoding           **
**  algorithm.  Useful for algorithms that need to work on images one scan  **
**  line at a time.                                                         **
\****************************************************************************/

/*
**  Encode (compress) a PCX image file scan line.
**
**  EncodeScanLine() encodes a buffer containing raw scan line data
**  using the PCX PackBytes run-length encoding algorithm.  The encoded
**  data is written to a second buffer passed to the function.
**
**  The main problem when encoding a scan line is that we never know how
**  long the resulting encoded scan line will be.  Typically it will be
**  shorter than the unencoded line, however, this type of RLE encoding
**  can make the resulting encoded data larger in size than the unencoded
**  data depending upon the data being encoded.
**
**  It is therefore important that the buffer used to hold the encoded
**  scan line be larger than what will typically be needed.  A size of
**  2048 bytes should be more than plenty for typical VGA images.  The
**  size of the buffer can be trimmed down to the size of the encoded
**  data using realloc() and the return value of PcxEncodeScanLine()
**  function, if necessary.
**
**  Returns: Number of bytes reading from buffer.
*/
// @mfunc Encodes a buffer containing raw scan line data
//@parm	uint8* | EncodedBuffer | Pointer to buffer to hold encodeded scan line
//@parm	uint8* | DencodedBuffer | Pointer to buffer to hold decoded scan line
//@parm	uint16 | BufferSize |  Size of buffer holding unencoded data
int16 TPcx::EncodeScanLine(uint8* EncodedBuffer, uint8* DecodedBuffer, uint16 BufferSize)
{
	uint16 index = 0;        /* Index into uncompressed data buffer  */
	uint16 scanindex = 0;    /* Index into compressed data buffer    */
	uint8 runcount;          /* Length of encoded pixel run          */
	uint8 runvalue;          /* Value of encoded pixel run           */

	while (index < BufferSize){
		/* Get the run count of the next pixel value run.                   */
		/* Pixel value runs are encoded until a different pixel value       */
		/* is encountered, the end of the scan line is reached, or 63       */
		/* pixel values have been counted.                                  */
		for (runcount = 1, runvalue = DecodedBuffer[index];
			runvalue == DecodedBuffer[index + runcount] &&
			(index + runcount) < BufferSize && runcount < 63;
		runcount++)
			;

		/* Encode the run into a one or two-byte code.                       */
		/* Multiple pixel runs are stored in two-byte codes.  If a single    */
		/* pixel run has a value of less than 64 then it is stored in a      */
		/* one-byte code.  If a single pixel run has a value of 64 to 255    */
		/* then it is stored in a two-byte code.                             */
		if (runcount > 1){                   /* Multiple pixel run */
			EncodedBuffer[scanindex++] = uint8(runcount | 0xC0);
			EncodedBuffer[scanindex++] = runvalue;
		}
		else { /* Single pixel run   */
			if(DecodedBuffer[index] > 63)  /*  Value is 64 to 255                                */
				EncodedBuffer[scanindex++] = uint8(1 | 0xC0);
			EncodedBuffer[scanindex++] = runvalue;
		}
		index += runcount;  /* Jump ahead to next pixel run value */
	}
	return(scanindex);      /* Return the number of bytes written to buffer */
}

/*
PCX decoders need to be aware of image data that contains runs which
start at the end of one scan line and finish at the start of the next
line.  This function decodes one scan line at a time and will keep
track of runs if they extend to the next scan line.
*/

/****************************************************************************\
**  Title:       PCXDECODE.C                                                **
**  Purpose:     Decode a PCX image file scan line.                         **
**  Version:     1.0                                                        **
**  Date:        May 1991                                                   **
**  Author:      James D. Murray, Anaheim, CA  USA                          **
**  C Compilers: Borland C++ v2.0, Microsoft C v6.00a                       **
**                                                                          **
**  Decode an image scan line using the PCX run-length encoding algorithm   **
**  Decoded data is returned in a buffer.  Useful for algorithms that need  **
**  to work on images one scan line at a time.                              **
**                                                                          **
**  This module contains the following functions:                           **
**                                                                          **
**      PcxDecodeScanLine - Decode a PCX RLE-encoded scan line              **
**                                                                          **
**  Copyright (C) 1991 by Graphics Software Labs.  All rights reserved.     **
\****************************************************************************/

/*
**  Decode (uncompress) a PCX image file scan line.
**
**  PcxDecodeScanLine() decodes a buffer containing scan line data encoded
**  using the PCX run-length encoding algorithm.  The encoded data is read
**  from a FILE stream, decoded, and then written to a pointer to a buffer
**  passed to this function.
**
**  The PCX specification states (in so many words) that the run-length
**  encoding of a pixel run should stop at the end of each scan line.
**  However, some PCX encoders may continue the encoding of a pixel run on
**  to the beginning of the next scan line, if possible.  This code,
**  therefore, assumes that any pixel run can span scan lines.
**
**  To check for decoding errors, the value returned by PcxDecodeScanLine()
**  should be the same as the value of BufferSize (the length of an
**  uncompressed scan line).
**
**  Returns: Total number of pixels decoded from compressed scan line,
**           or EOF if a file stream error occured.
*/
int16 TPcx::DecodeScanLine(uint8* DecodedBuffer, uint8* src, uint16 BufferSize)
{
	uint16 index = 0;      /* Index into compressed scan line buffer */
	uint8  byte;           /* Data byte read from PCX file           */
	int16  byteCount = 0;  /* Number of bytes reading from src buffer*/

	/* If there is any data left over from the previous scan */
	/* line write it to the beginning of this scan line.     */
	do{
		/* Write the pixel run to the buffer */
		for(;
			RunCount && index < BufferSize;    /* Don't read past buffer   */
			RunCount--, index++)
			DecodedBuffer[index] = RunValue;    /* Assign value to buffer   */

		if(index >= BufferSize){
			//TRACE(0,"------------ TPcx::DecodeScanLine() index >= BufferSize => index = " << index << " BufferSize = " << BufferSize);
			break;
		}

		if(RunCount){         /* Encoded run ran past end of scan line    */
			//TRACE(0,"PCXFormat::DecodeScanLine() Encoded run ran past end of scan line, byteCount - 1 => " << (byteCount-1));
			return --byteCount;   /* Return number of bytes readed from buffer*/
		}

		/* Get the next encoded run packet.                                 */
		/* Read a byte of data.  If the two MBSs are 1 then this byte       */
		/* holds a count value (0 to 63) and the following byte is the      */
		/* data value to be repeated.  If the two MSBs are 0 then the       */
		/* count is one and the byte is the data value itself.              */
		byte = *src++;              /* Get next byte    */
		byteCount++;

		if((byte & 0xC0) == 0xC0){       /* Two-byte code    */
			RunCount = uint8(byte & 0x3F); /* Get run count    */
			RunValue = *src++;             /* Get pixel value  */
			byteCount++;                   /*                  */
		}
		else{                       /* One byte code    */
			RunCount = 1;             /* Run count is one */
			RunValue = byte;          /* Pixel value      */
		}
	}
	while(index < BufferSize);    /* Read until the end of the buffer  */

	return byteCount; /* Return number of bytes readed from buffer   */
}
////////////////////////////////////////////////////////////////////
//
bool TPcxDescriptor::GetDescription(LPTSTR buffer, int size)
{
	return gModule->LoadString(IDS_PCX_DESCRIPTOR, buffer, size) != 0;
}
//
TDib* TPcxDescriptor::OpenInstance(LPCTSTR path)
{
	return new TPcx(path);
}
//
TDib* TPcxDescriptor::CreateInstance(const TDib& dib)
{
	return new TPcx(dib);
}
//
bool TPcxDescriptor::VerifyFile(LPCTSTR /*path*/)
{
	// ....
	return false;
}
//
bool TPcxDescriptor::GetCapabilities(uint& pixType, uint& addit)
{
	pixType = pixcRGB|pixcRGB_INDEX;
	addit   = cabCanConfig|cabCanRead|cabCanWrite;
	return true;
}
//
TDialog* TPcxDescriptor::GetConfigDlg(TDib& /*dib*/, TWindow* /*parent*/)
{
	return 0;//?????????????????????????????????
}
//
_OWLFX_END_NAMESPACE

/* ========================================================================== */