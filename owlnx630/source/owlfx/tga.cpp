//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//
// Source file for implementation of TTga (TDib).
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owl/file.h>
#include <owlfx/tga.h>
#include <owlfx/owlfx.rh>

__OWL_USING_STD_NAMESPACE
__OWL_USING_NAMESPACE

#if 0
#undef DOUT
#undef DOUTNAME
#undef DENABLE
#define DENABLE(_state_) bEnable = _state_;
#define DOUT(_lv_,_str_) if(bEnable && iLevel >= (_lv_)) _dbgstr_ << _str_ << endl
#define DOUTNAME(_Level_, _name_) static bool bEnable = true; static int iLevel = _Level_; static fstream _dbgstr_(_name_, ios::out)
DOUTNAME(4, "tga.log");
#endif

#define BYTESPERPIXEL   ((Header.BitCount + 7) >> 3)

//@const char* | TGASIGNATURE  | Tga Signature for footer
const char* TGASIGNATURE  = "TRUEVISION-XFILE.\0";


_OWLFX_BEGIN_NAMESPACE
///////////////////////////////////////////////////////////////

TTgaFooter::TTgaFooter() 
: 
ExtensionOffset(0), DeveloperOffset(0)
{
	strcpy(Signature, TGASIGNATURE);
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/*-------   tables for translations                 ------------------------- */
#include <pshpack1.h>
struct TTga_Bit15 {
	uint8 Blue   : 5;
	uint8 Green  : 5;
	uint8 Red    : 5;
};
struct TTga_Bit16 {
	uint8 Blue     : 5;
	uint8 Green    : 5;
	uint8 Red      : 5;
	uint8 Reserved : 1;
};
struct TTga_Bit24 {
	uint8 Blue;
	uint8 Green;
	uint8 Red;
};
struct TTga_Bit32 {
	uint8 Blue;
	uint8 Green;
	uint8 Red;
	uint8 Alpha;
};
#include <poppack.h>
/* -------------------------------------------------------------------------- */
/* ---                 T  G  A  format                                    --- */
//{{TTga Implementation}}
//@mfunc Constructor
// @syntax 1. TTga(const char* name) - create TTga from *.tga file
// @syntax 2. TTga (const LPBITMAPINFO lpbi) - create TTga around pointer to BITMAPINFO
// @syntax 3. TTga(HGLOBAL handle, TAutoDelete autoDelete) - create TTga around Global handle to BITMAPINFO
// @syntax 4. TTga(const TTga& tga) - create TTga from another TTga
// @parm const char* | name | name of file to open
// @parm const LPBITMAPINFO | lpbi | pointer to BITMAPINFO
// @parm HGLOBAL | handle | handle to existing tga
// @parm TAutoDelete | autoDelete | flag if free memory on deleting
// @parm const TClipboard& | clipboard | Reference to Clipboard object
// @parm TTga& | tga | Reference to existed tga
// @parm int | width | With of Tga to create
// @parm int | height | Height of Tga to create
// @parm int | nColors | Colors of Tga to create
// @parm uint16 | mode | Mode of Tga to create
// @parm HINSTANCE | hIn | Instance for this application
// @parm TResId | Id | Respurce Id for this resource
// @parm const TBitmap& | bitmap | bitmap to create frrom
// @parm const TPalette* | pal | palette to create from
TTga::TTga(LPCTSTR name)
//: 
//	Developer(0), Extension(0)
{
	if(!ReadFile(name))
		TXGdi::Raise(IDS_GDIFILEREADFAIL);
}
/* -------------------------------------------------------------------------- */
TTga::TTga (const LPBITMAPINFO lpbi, TAutoDelete autoDelete) 
:
TDib(lpbi,autoDelete)//, Developer(0), Extension(0)
{
	Init();
}
/* -------------------------------------------------------------------------- */
TTga::TTga(HGLOBAL handle, TAutoDelete autoDelete)
: 
TDib(handle, autoDelete)//, Developer(0), Extension(0)
{
	Init();
}
/* -------------------------------------------------------------------------- */
TTga::TTga(const TTga& dib)
: 
TDib(dib)//, Developer(0), Extension(0)
{
	Init();
}
//
TTga::TTga(const TDib& dib)
: 
TDib(dib)//, Developer(0), Extension(0)
{
	Init();
}
//
TTga::TTga()
//: 
//	Developer(0), Extension(0),DataOffset(0)
{
}
/* -------------------------------------------------------------------------- */
//@mfunc Destructor
TTga::~TTga()
{
	//	delete Developer;
	//	delete Extension;
}
/* -------------------------------------------------------------------------- */
//@mfunc Init all default data if creating Tga not from file
void TTga::Init()
{
	//DataOffset 	= 0;
	// set TARGAHEADER
	Header.IdLength 	= 0; // lenth of Image ID field 6 (Cmap.IdField)
	Header.CMapType 	= (uint8)((NumClrs > 0) ? 1 : 0);
	Header.ImageType 	= (uint8)((NumClrs > 0) ? 1 : 2); //default uncompressed + (true color/color mapped)
	Header.CMapIndex 	= 0; // start from first color index if any
	Header.CMapLength = (uint16)NumClrs; // default = 0, if color map exist: !=0
	Header.CMapEntrySize = (uint8)(NumClrs > 0 ? 24u : 0u); // compatible with windows palette
	Header.X_Origin 	= 0; // X origin of image = 0;
	Header.Y_Origin 	= 0; // X origin of image = 0;
	Header.Width			= (uint16)Info->bmiHeader.biWidth;
	Header.Height			= (uint16)Info->bmiHeader.biHeight;
	Header.BitCount		= Info->bmiHeader.biBitCount;
	Header.Desc				= 0; // bottom left origin

	/* Set the Image ID field. */
	char* descr = "OWL NExt.\0";
	Header.IdLength = (uint16)(strlen(descr)+1);
	Cmap.IdField = new int8 [Header.IdLength];
	strcpy((char*)Cmap.IdField, descr);

	// set Palette for image if any
	if(Header.CMapType){
		/* Allocate memory for the color map data */
		TTga_Bit24 * map = new TTga_Bit24 [NumClrs];
		for (uint16 i = 0; i < NumClrs; i++) {
			map[i].Red 		= Info->bmiColors[i].rgbRed;
			map[i].Green 	= Info->bmiColors[i].rgbGreen;
			map[i].Blue 	= Info->bmiColors[i].rgbBlue;
		}
		Cmap.CMapData = (uint8*)map;
	}
}
/* -------------------------------------------------------------------------- */
// @mfunc Read a Targa image (only 24-bit & 32 TrueColor images handled)
bool TTga::Read(IFileIn& file, bool)
{
	uint16	i;	/* Loop counters                    */

	/* Read the TGA image file header information */
	if(!ReadHeader(file)){
		// TXGdi::
		return false;
	}

	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);

	/* Set up bitmap info header                               */
	infoHeader.biClrUsed = NColors(Header.BitCount);
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biWidth = (int)Header.Width;
	infoHeader.biHeight = (int)Header.Height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = Header.BitCount;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = ScanBytes(Header.Width, Header.BitCount) * (long)Header.Height;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrImportant = 0;

	// Setup all members based on InfoHeader
	InfoFromHeader(infoHeader);

	/* ------------------------------------------------ */
	/* set color table                                  */
	if (Colors) {
		switch(Header.CMapEntrySize){
case 15: {
	TTga_Bit15 * map = (TTga_Bit15 *)Cmap.CMapData;
	for (i = 0; i < NumClrs; i++) {
		Info->bmiColors[i].rgbRed 	= map[i].Red;
		Info->bmiColors[i].rgbGreen = map[i].Green;
		Info->bmiColors[i].rgbBlue 	= map[i].Blue;
		Info->bmiColors[i].rgbReserved = 0;
	}
	break;
			}
case 16: {
	TTga_Bit16 * map = (TTga_Bit16 *)Cmap.CMapData;
	for (i = 0; i < NumClrs; i++) {
		Info->bmiColors[i].rgbRed 	= map[i].Red;
		Info->bmiColors[i].rgbGreen = map[i].Green;
		Info->bmiColors[i].rgbBlue 	= map[i].Blue;
		Info->bmiColors[i].rgbReserved = 0;
	}
	break;
			}
case 24: {
	TTga_Bit24 * map = (TTga_Bit24 *)Cmap.CMapData;
	for (i = 0; i < NumClrs; i++) {
		Info->bmiColors[i].rgbRed 	= map[i].Red;
		Info->bmiColors[i].rgbGreen = map[i].Green;
		Info->bmiColors[i].rgbBlue 	= map[i].Blue;
		Info->bmiColors[i].rgbReserved = 0;
	}
	break;
			}
case 32: {
	TTga_Bit32 * map = (TTga_Bit32 *)Cmap.CMapData;
	for (i = 0; i < NumClrs; i++) {
		Info->bmiColors[i].rgbRed 	= map[i].Red;
		Info->bmiColors[i].rgbGreen = map[i].Green;
		Info->bmiColors[i].rgbBlue 	= map[i].Blue;
		Info->bmiColors[i].rgbReserved = 0;
	}
	break;
			}
		}
	}

	/* read all tga bits to bits of bitmap uncompress if nessesary */
	return ReadBits(file);
}
/* -------------------------------------------------------------------------- */
// @mfunc Read the data Bits from TGA file.
// @parm MFile& | file | TGA image file
bool TTga::ReadBits(IFileIn& file)
{
	uint		j;    		   /* Loop counters                            */
	uint8		* buffer; /* Buffer to hold scan line data            */
	int			delta;
	uint		byteCount;
	uint32	offset;
	uint8*	dst;
	uint8*	src;
	uint32	counter,readedBytes, skipBytes=0;;
	uint		Height = Header.Height; 		/* number of scan lines */
	int			bitsInLine = ScanBytes(Header.Width, Header.BitCount);
	const uint bitsRead = bitsInLine * 50;
	uint32 bitsAlloc = 	(uint32)bitsInLine * (long)Header.Height;
	uint		bytesInLine = (Header.Width * Header.BitCount)/8;

	TTmpBuffer<uint8> __ClnObj1(bitsInLine);
	TTmpBuffer<uint8> __ClnObj2(bitsRead);
	uint8 * bitBuf = __ClnObj1;
	uint8 * readBuf = __ClnObj2;


	/* Seek to the image data */
	//file.Seek(DataOffset, MFile::beg);  //????????????????????????????????????????????????????

	if(Header.Desc & 0x20){ /* test 5 bit if read from from top  */
		/* point to end of buffer */
		buffer = (uint8 *)Bits + bitsAlloc;
		delta = -bitsInLine; /* reverse counting*/
	}
	else{
		buffer = (uint8*)Bits + bitsInLine;
		delta = bitsInLine;
	}
	/* If the image data is encoded then decode it.                   */
	if (Header.ImageType == 9 || Header.ImageType == 10 || Header.ImageType == 11){
		if(Header.Desc & 0x10){ /* test 4 bit if read rigt to left      */
			while(Height){

				/* Read a scan line  */
				readedBytes = counter = file.Read(readBuf+skipBytes, bitsRead-skipBytes);
				offset = 0;
				byteCount = 0;
				while(counter != TFILE_ERROR && counter > byteCount && Height){
					/* Decode a scan line                                   */
					byteCount = DecodeScanLine(bitBuf, readBuf+offset, Header.Width, BYTESPERPIXEL);
					if(counter < byteCount)
						break;

					dst = buffer;
					src = bitBuf;
					j = bitsInLine;

					while(j--) /* copy with reverse to bitmap bits          */
						*dst-- = *src++;

					/* to previous scan scan line                           */
					buffer += delta; 			/* becouse I create reverce order */
					Height--; 						/* decrement number of scan lines */
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
		}
		else{ /* if image bits in left to right order                */
			buffer -= bitsInLine;   /* return to start Bits             */
			while(Height){
				/* Read a scan line  */
				readedBytes = counter = file.Read(readBuf+skipBytes, bitsRead-skipBytes);
				offset = 0;
				byteCount = 0;
				while(counter != TFILE_ERROR && counter > byteCount && Height){
					/* Decode a scan line                                   */
					byteCount = DecodeScanLine(buffer, readBuf + offset, Header.Width, BYTESPERPIXEL);

					if(counter < byteCount){
						//TRACE("TTga::ReadBits() - 1, counter(" << dec << counter << ") < byteCount(" << dec << byteCount<< ")");
						break;
					}
					/* to previous scan scan line                           */
					buffer += delta; 			/* becouse I create reverce order */
					Height--; 						/* decrement number of scan lines */
					//TRACE("TTga::ReadBits() - 2, new Height = " << dec << Height << ", byteCount = " << byteCount << ", counter = " << counter);
					counter -= byteCount;	/* decrement count bytes          */
					offset 	+= byteCount; /* increment offset to read from  */
				}
				//TRACE("TTga::ReadBits() - 3, offset = " << offset);
				skipBytes = readedBytes - offset;
				CHECK(skipBytes >= 0);
				if(skipBytes){
					if(skipBytes > 0)
						memmove((uint8*)readBuf,(uint8*)readBuf+offset, skipBytes);
					file.Skip(skipBytes);
				}
			}
		}
	}
	else{ /* ---------------- not encoded  ----------------------------------- */

		if(Header.Desc & 0x10){ /* test 4 bit if read rigt to left*/

			while(Height){

				/* Read a 10 scan lines  */
				counter = file.Read(readBuf, bitsRead);
				offset = 0;
				while(counter && Height){
					dst = buffer;
					src = readBuf + offset;
					j = bytesInLine;
					while(j--) /* copy with reverse to bitmap bits          */
						*dst-- = *src++;

					/* to previous scan scan line                           */
					buffer += delta; 			 /* becouse I create reverce order*/
					Height--; 						 /* decrement number of scan lines*/
					counter -= bytesInLine; /* decrement count bytes         */
					offset 	+= bytesInLine; /* increment offset to read from */
				}
			}
		}
		else{ /* if image bits in left to right order */
			buffer -= bitsInLine;   /* return to start Bits */

			while(Height){
				file.Read(buffer, bytesInLine);
				buffer += delta; 			/* becouse I create reverce order */
				Height--; 						/* decrement number of scan lines */
			}
		} /* end if left to right */
	}
	return true;
}
/* -------------------------------------------------------------------------- */
//@mfunc Read the information from a TGA file info a TGAHEADER structure.
// The TGAHEADER structure (defined in the file TGA.H) contains all of the
//  information found in both version 1.0 and 2.0 of the TGA format except
//  for the actual image data.  Image data is not stored because of its
//  typically large size.
/*                                                                            */
//@parm	IFileIn& | file  | TGA image file input FILE stream
bool TTga::ReadHeader(IFileIn& in)
{

	/* Read the TGA header (Version 1.0 and 2.0). */
	if (in.Read(&Header, sizeof(TTgaHeader)) != (int)sizeof(TTgaHeader)){
		//TXGdi::Raise(IDS_GDIFILEREADFAIL);
		return false;
	}

	/* Read the Image ID field. */
	if (Header.IdLength){
		/* Allocate memory for the Id Field data */
		Cmap.IdField = new int8 [Header.IdLength];

		/* Read the Image ID data */
		/* Do not assume that it is a NULL-terminated string */
		if(	in.Read(Cmap.IdField, Header.IdLength) != Header.IdLength){
			//TXGdi::Raise(IDS_GDIFILEREADFAIL);
			return false;
		}
	}

	/* Read the color map data (Version 1.0 and 2.0). */
	if(Header.CMapType){
		/* Determine the size of the color map */
		uint cmapsize = ((Header.CMapEntrySize + 7) >> 3) * Header.CMapLength;

		/* Allocate memory for the color map data */
		Cmap.CMapData = new uint8 [cmapsize];

		/* Read the color map data */
		if(in.Read(Cmap.CMapData, cmapsize) != (int)cmapsize){
			//TXGdi::Raise(IDS_GDIFILEREADFAIL);
			return false;
		}
	}
	return true;
}

/* -------------------------------------------------------------------------- */
// @mfunc  Write the data Bits to a TGA file.
// @parm	MFile& | file | TGA image file output
uint32 TTga::WriteBits(IFileOut& out)
{
	uint16      bufSize;        /* Size of the scan line buffer             */
	int         byteCount;      /* Number of bytes in a buffer              */
	uint32      imageSize = 0;  /* Size of the image data in bytes          */
	uint32      offset = 0;
	uint8 * buffer;         /* Buffer to hold scan line data            */

	/* I want to write only one type of bits order:    */
	/* from left to right &  from bottom to  top       */
	uint   byteInLine = (uint)ScanBytes(Header.Width, Header.BitCount);
	uint32 bitsAlloc = (long)byteInLine * (long)Header.Height;

	/* If the image data must be encoded then encode it.                   */
	if (Header.ImageType == 9 || Header.ImageType == 10 ||Header.ImageType == 11){
		/* Calculate the encoded buffer size in bytes.  The worst case size  */
		/* is based on every pixel in the scan line being encoded as a       */
		/* single, raw run-length packet.                                    */
		bufSize = (uint16)(Header.Width + /* Number of packet count bytes */
			(Header.Width * 				/* Number of pixels             */
			BYTESPERPIXEL)); 			/* Number of bytes per pixel    */

		/* Allocate buffer memory */
		TTmpBuffer<uint8> buffer(bufSize);

		/* Encode a scan line and write it to the buffer                  */
		int height = Header.Height;
		while(height--){

			/* Encode a scan line                                           */
			if ((byteCount = EncodeScanLine((uint8*)buffer, (uint8 *)Bits + offset, 
				bufSize, Header.Width, BYTESPERPIXEL)) < 0){
					//TXGdi::Raise(IDS_GDIFILECOMPFAIL);
					return false;
			}
			TRACE("TTga::WriteBits() Height = " << dec << height);

			/* Write the encoded scan line to file       */
			if(!out.Write(buffer, byteCount)){
				//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
				return false;
			}

			/* Update the image data size byte count */
			imageSize += byteCount;
			offset 		+= byteInLine;
		}
	} 
	else{
		uint16 Height = Header.Height; 		/* number of scan lines */
		uint16 bytesInLine = uint16((Header.Width * Header.BitCount)/8);
		buffer = (uint8*)Bits;
		while(Height--){
			if(!out.Write(buffer, bytesInLine)){
				//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
				return false;
			}
			buffer += bytesInLine;
		}
		imageSize = bitsAlloc;
	}
	return imageSize;
}
/* -------------------------------------------------------------------------- */
// @mfunc
/*  Write the information from a TGAHEADER structure to a TGA file.           */
/*  and all file bits                                                         */
// @parm  MFile& | file | TGA image file output
/* -------------------------------------------------------------------------- */
bool TTga::Write(IFileOut& out, bool writeFileHeader)
{
	if(writeFileHeader){
		/* Write the TGA header (Version 1.0 and 2.0).                          */
		TTgaHeader hdr;
		memcpy(&hdr, &Header, sizeof(hdr));
		hdr.Desc &= ~0x30; /* reset to: from bottom/left to top/right */
		if (!out.Write(&hdr, sizeof(TTgaHeader))){
			//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
			return false;
		}


		/* Write the Image ID field.                                            */
		if (Header.IdLength)
			if(!out.Write(Cmap.IdField, Header.IdLength)){
				//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
				return false;
			}

			/* Write the color map data (Version 1.0 and 2.0).                      */
			if (Header.CMapType){
				/* Determine the size of the color map                                */
				int cmapsize = ((Header.CMapEntrySize + 7) >> 3) * Header.CMapLength;

				/* Write the color map data                                           */
				if(!out.Write(Cmap.CMapData, cmapsize)){
					//TXGdi::Raise(IDS_GDIFILEWRITEFAIL);
					return false;
				}
			}
	}
	/* Write the image data (Versions 1.0 and 2.0).                         */
	WriteBits(out);

	// if not header - so not buffer ether
	if(writeFileHeader){

		// write extensions ???


		/* Write the version 2.0 footer.    */
		if(!out.Write(&Footer, sizeof(TTgaFooter))){
			//MXGdi().Raise(IDS_GDIFILEWRITEFAIL);
			return false;
		}
	}
	return true;
}
/* -------------------------------------------------------------------------- */
//@mfunc Encode a TGA image file scan line.
//  Compress an image scan line using the TGA run-length encoding
//  algorithm.  Useful for algorithms that need to work on images
//  one scan line at a time. Read raw image data from a intermediate
//  buffer and write TGA RLE data to a storage buffer.
//
//  This function always produces raw packets that are a run of the same
//  pixel values.
//
//  We assume that when EncodeScanLine() is called src is pointing
//  to the first byte of unencoded image data in the buffer.
/*                                                                            */
// @parm uint8 * | dst | Pointer to buffer to hold encoded scan line
// @parm uint8 * | src | Pointer to buffer to hold unencoded scan line
// @parm uint16 | BufferSize | The size of the scan line buffer in bytes
// @parm uint | LineLength | The length of a scan line in pixels
// @parm uint | PixelSize | The number of bytes in a pixel
/*                                                                            */
// @rdesc The number of RLE packets written to the buffer.
int16 TTga::EncodeScanLine (uint8 * dst, uint8 * src, uint16 BufferSize,
							uint LineLength, uint PixelSize)
{
	uint	i;            	/* Loop Counter                                 		*/
	uint	runCount;     	/* The number of pixels in the current run      		*/
	uint	pixelCount;   	/* The number of pixels read from the scan line 		*/
	int 	bufIndex;     	/* The index of DecodedBuffer                   		*/
	uint8	pixelValue1[4]; /* Pixel value read from the scan line (4-byte max) */
	uint8	pixelValue2[4]; /* Pixel value read from the scan line          		*/
	bool	pixelsMatch;    /* TRUE if pixelValue1 and pixelValue2 are same 		*/

	bufIndex   = 0;
	runCount   = 1;
	pixelCount = 1;

	/* Read in first pixel value */
	for (i = 0; i < PixelSize; i++)
		pixelValue1[i] = *src++; /* get value and increment src pointer by one */

	/* Main encoding loop */
	for (;;){
		pixelsMatch = true; /* Assume we have a match */

		/* Read in another pixel value */
		for (i = 0; i < PixelSize; i++)	{
			pixelValue2[i] = *src++;
			if (pixelValue1[i] != pixelValue2[i])   /* Compare pixels */
				pixelsMatch = false;
		}
		pixelCount++;        /* Count number of pixels read so far */

		/* If the pixels are the same then start or continue a run */
		if (pixelsMatch == true){
			runCount++;             /* Advance the run count                */
			// always me not count last pixel -> if read 128 pixels as write 127!!!!!!!
			if (runCount < 128){    /* Maximum run-length is 128 pixels        			*/
				if (pixelCount < LineLength)  /* Don't run past end of scan line */
					continue;       /* Continue reading the run             */
			}
		}

		/* Pixel values are different, so stop the run */
		if (runCount < 4) { /* Write a raw packet      */

			/* If we've encoded the entire line then break out of the loop */
			// if last run in this line, pixelCount includes pixelValue2 and if
			// pixelsMatch != alse runCount not include pixelCount therefore
			// write also pixelCount2 if needed
			if (pixelCount >= LineLength){
				//	DOUT(4, "MTga::EncodeScanLine(): (write endline raw packet). runCount = " << (pixelsMatch ? (runCount - 1) : runCount) << ", byteRead = " << (PixelSize * (pixelCount-1)) << ", bufIndex => " << bufIndex);

				if(pixelCount > LineLength){
					//DOUT(5, "MTga::EncodeScanLine(): if PixelCountnt > LineLength, runCount = " << runCount << ", pixelCount => " << pixelCount);
					runCount--;
				}

				/* Write runCount if pixelsMatch = true or  runCount - 1 if not,      */
				/* to buffer.  MSB is off.                                            */
				if(pixelsMatch){
					// DOUT(5, "MTga::EncodeScanLine(): pixels match write to runCount => " << (runCount - 1));
					dst[bufIndex++] = (uint8)((runCount - 1) & ~0x80);
				}
				else{
					//DOUT(5, "MTga::EncodeScanLine(): pixels not math write to runCount => " << runCount);
					dst[bufIndex++] = (uint8)(runCount & ~0x80);
				}
				/* Write the pixel data run                                        */
				/* without last pixel if not pixelsMatch or with LastPixel if true */
				do{
					for (i = 0; i < PixelSize; i++)
						dst[bufIndex++] = pixelValue1[i];
				}while (--runCount);

				if(!pixelsMatch){ // if last pixel was not included write it
					//DOUT(4, "MTga::EncodeScanLine(): write last pixel, pixelCount => " << pixelCount);
					for (i = 0; i < PixelSize; i++)
						dst[bufIndex++] = pixelValue2[i];
				}
				break;
			}
			else{
				/* Write runCount - 1 to buffer.  MSB is off. */
				dst[bufIndex++] = (uint8)((runCount - 1) & ~0x80);

				/* Write the pixel data run (without last pixel)*/
				do{
					for (i = 0; i < PixelSize; i++)
						dst[bufIndex++] = pixelValue1[i];
				}while (--runCount);
			}
		}
		else{ /* else Write an encoded packet */
			/* Write the Pixel Count to the buffer.  MSB is on. (if read 129 pixels -> 2 else 1 )*/
			dst[bufIndex++] = (uint8)((runCount - (pixelsMatch ? 2 : 1 )) | 0x80);

			/* Write the pixel data run value */
			for (i = 0; i < PixelSize; i++)
				dst[bufIndex++] = pixelValue1[i];

			/* If we've encoded the entire line then break out of the loop */
			if (pixelCount >= LineLength){
				if(!pixelsMatch){ // if last pixel was not included write it
					runCount = 1;
					/* Write runCount - 1 to buffer.  MSB is off. */
					dst[bufIndex++] = (uint8)((runCount - 1) & ~0x80);
					//DOUT(4, "MTga::EncodeScanLine(): write last pixel in encoded run, pixelCount => " << pixelCount);
					for (i = 0; i < PixelSize; i++)
						dst[bufIndex++] = pixelValue2[i];
				}
				break;
			}

			/* Read in another pixel value for case runCount == 128 */
			/* becouse was written runcount => pixelValue2 also     */
			if(runCount == 128){
				//DOUT(4, "MTga::EncodeScanLine(): read added pixel(in encoded run).pixelCount => " << pixelCount);
				for (i = 0; i < PixelSize; i++)
					pixelValue2[i] = *src++;
				pixelCount++;        /* Count number of pixels read so far */
			}

		}/* End If Pixel values are different */

		runCount = 1;                /* Start a new pixel run count  */
		// copy data from pixelValue2 to pixelValue1 for next run
		memcpy(pixelValue1, pixelValue2, PixelSize);
	}
	//DOUT("TTga::EncodeScanLine(): end of encoding line, byteRead = " << (PixelSize * (pixelCount - 1)) << ", bufIndex = " << dec << bufIndex << ", pixelCount = " << (pixelCount - 1));
	return (uint16)bufIndex;
} /* End TgaEncodeScanLine()                                                  */

/* -------------------------------------------------------------------------- */
// @mfunc
//  Decode a TGA image file scan line.
//
//  UnCompress an image scan line using the TGA run-length encoding
//  algorithm.  Decoded data is returned in a buffer.  Useful for
//  algorithms that need to work on images one scan line at a time.
//  Read encoded data from a TGA image file, decode it, and write it to
//  a buffer.  Assume that the FILE pointer is pointing to the first byte
//  of encoded image data.
/*                                                                            */
// @parm uint8 * | dst | Pointer to buffer to hold encoded scan line
// @parm uint8 * | src | Pointer to buffer to hold dencoded scan line
// @parm uint | LineLength | The length of a scan line in pixels
// @parm uint | PixelSize | The number of bytes in a pixel
// @rdesc A number of bites reading from source buffer
int16 TTga::DecodeScanLine(uint8 * dst, uint8 * src, uint LineLength, uint PixelSize)
{
	uint	i;              /* Loop counter                                 */
	uint  runCount;       /* The pixel run count                          */
	uint  bufMark;        /* Index marker of DecodedBuffer                */
	int   byteCount = 0;  /* Number of bytes reading from src buffer      */
	uint	bufIndex = 0;   /* The index of DecodedBuffer                   */
	uint	pixelCount = 0; /* The number of pixels read from the scan line */

	/* ---   Main decoding loop                     --- */
	while (pixelCount < LineLength){
		/* Get the pixel count */
		runCount = *src++;
		byteCount++;

		/* Make sure writing this next run will not overflow the buffer   */
		//if ((pixelCount + (runCount & 0x7f) + 1) > LineLength){
		if ((pixelCount + (runCount & 0x7f)) > LineLength){ //why ?????????????????????????
			//TRACE("TTga::DecodeScanLine(): Pixel run will overflow buffer.");
			//DOUT("TTga::DecodeScanLine(): Pixel run will overflow buffer. bufIndex = " << dec << bufIndex << ", byteCount = " << dec << byteCount << ", pixelCount = " << pixelCount);
			return (int16)byteCount;
		}

		/* If the run is encoded... */
		if (runCount & 0x80){
			runCount &= ~0x80;              /* Mask off the upper bit       */

			bufMark = bufIndex;             /* Save the start-of-run index  */

			/* Update total pixel count */
			pixelCount += runCount + 1;

			/* Update the buffer byte count */
			//byteCount += ((runCount + 1) * PixelSize);

			/* Write the first pixel of the run to the buffer */
			for (i = 0; i < PixelSize; i++){
				dst[bufIndex++] = *src++;
				byteCount++;
			}

			/* Write remainder of pixel run to buffer 'runCount' times */
			while (runCount--){
				for (i = 0; i < PixelSize; i++)
					dst[bufIndex++] = dst[bufMark + i];
			}

		}
		else{ /* ...the run is unencoded (raw) */
			/* Update total pixel count           */
			pixelCount += runCount + 1;

			/* Write runCount pixels */
			do{
				for (i = 0; i < PixelSize; i++){
					dst[bufIndex++] = *src++;
					byteCount++;
				}
			}while (runCount--);
		}
	}
	//DOUT("TTga::DecodeScanLine(): end encoding line. bufIndex = " << dec << bufIndex << ", byteCount = " << dec << byteCount << ", pixelCount = " << pixelCount);
	return (int16)byteCount;
}
/* -------------------------------------------------------------------------- */
TTgaExtension::~TTgaExtension()
{
	delete ScanLineTable;
	delete StampImage;
}
/* -------------------------------------------------------------------------- */
TTgaDeveloper::~TTgaDeveloper()
{
	TTgaTag* list;
	// delete entare list of data;
	while(List){
		list = List->Next;
		delete List;
		List = list;
	}
}
/* -------------------------------------------------------------------------- */
/* --- TGACMAP structure --- */
TTgaColorMap::~TTgaColorMap()
{
	delete [] IdField;
	delete CMapData;
}
////////////////////////////////////////////////////////////////////
//
bool TTgaDescriptor::GetDescription(LPTSTR buffer, int size)
{
	return gModule->LoadString(IDS_TGA_DESCRIPTOR, buffer, size) != 0;
}
//
TDib* TTgaDescriptor::OpenInstance(LPCTSTR path)
{
	return new TTga(path);
}
//
TDib* TTgaDescriptor::CreateInstance(const TDib& dib)
{
	return new TTga(dib);
}
//
bool	TTgaDescriptor::VerifyFile(LPCTSTR /*path*/)
{
	// ....
	return false;
}
//
bool TTgaDescriptor::GetCapabilities(uint& pixType, uint& addit)
{
	pixType = pixcARGB|pixcRGB|pixcRGB555|pixcRGB_INDEX;
	addit   = cabCanConfig|cabCanRead|cabCanWrite;
	return true;
}
//
TDialog* TTgaDescriptor::GetConfigDlg(TDib& /*dib*/, TWindow* /*parent*/)
{
	return 0;//?????????????????????????????????
}
//
_OWLFX_END_NAMESPACE
/* ========================================================================== */
