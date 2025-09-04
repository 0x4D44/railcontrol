//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//
//	Class definition for TTga (TDib).
//-------------------------------------------------------------------
#if !defined(OWLFX_TGA_H)              // Sentry, use file only if it's not already included.
#define OWLFX_TGA_H

#include <owlfx/defs.h>
#include <owlfx/imgdescr.h>

_OWLFX_BEGIN_NAMESPACE

#include <pshpack1.h>
//@struct TTgaHeader | TGA File Header
struct TTgaHeader {
	uint8  IdLength;      /* @field Image ID Field Length       */
	uint8  CMapType;      /* @field Color Map Type              */
	uint8  ImageType;     /* @field Image Type                  */
	/* color map specification                                  */
	uint16 CMapIndex;     /* @field First Color Map Entry Index */
	uint16 CMapLength;    /* @field Color Map Length            */
	uint8  CMapEntrySize; /* @field Color Map Entry Size        */
	/* Image Specification                                      */
	uint16 X_Origin;      /* @field X-origin of Image           */
	uint16 Y_Origin;      /* @field Y-origin of Image           */
	uint16 Width;         /* @field Image Width                 */
	uint16 Height;        /* @field Image Height                */
	uint8  BitCount;      /* @field Pixel Depth                 */
	uint8  Desc;          /* @field Image Descriptor            */
};

// @struct TGACMAP | Image/Color Map Data
struct _OWLFXCLASS TTgaColorMap{
	
	TTgaColorMap() : IdField(0), CMapData(0){}
	~TTgaColorMap();

	int8*  IdField;       /* @field Image ID Field            */
	uint8* CMapData;      /* @field Color Map Data            */
};

// @struct TGATAG | Developer Area Tag Structure
struct _OWLFXCLASS TTgaTag {
	
	TTgaTag() : Data(0),Next(0){}
	~TTgaTag(){	delete[] Data;}

	uint16 		Number;			/* @field Number of the Tag         */
	uint32 		Offset;  		/* @field Offset of the Tag Data    */
	uint32 		Size;    		/* @field Size of the Tag Data      */
	uint8*		Data;   		/* @field Pointer to the Tag Data   */
	TTgaTag*	Next;      	/* @field Link to next Tag          */
};

// @struct TTgaDeveloper | Developer Area
struct _OWLFXCLASS TTgaDeveloper {

	TTgaDeveloper() : List(0){}
	~TTgaDeveloper();
	
		uint16 		TagCount;   /* @field Number of Tags in Directory */
		TTgaTag*	List;       /* @field Link to list of Tags        */
};

// @struct TTgaExtension | Extension Area
struct _OWLFXCLASS TTgaExtension {

	TTgaExtension() : ScanLineTable(0), StampImage(0) {}
	~TTgaExtension();

	uint16  Size;               /* @field Extension Size             */
	int8    AuthorName[41];     /* @field Author Name                */
	int8    AuthorComment[324]; /* @field Author Comment             */
	uint16  StampMonth;         /* @field Date/Time Stamp: Month     */
	uint16  StampDay;           /* @field Date/Time Stamp: Day       */
	uint16  StampYear;          /* @field Date/Time Stamp: Year      */
	uint16  StampHour;          /* @field Date/Time Stamp: Hour      */
	uint16  StampMinute;        /* @field Date/Time Stamp: Minute    */
	uint16  StampSecond;        /* @field Date/Time Stamp: Second    */
	int8   	JobName[41];        /* @field Job Name/ID                */
	uint16  JobHour;            /* @field Job Time: Hours            */
	uint16  JobMinute;          /* @field Job Time: Minutes          */
	uint16  JobSecond;          /* @field Job Time: Seconds          */
	int8   	SoftwareId[41];     /* @field Software ID                */
	uint16  VersionNumber;      /* @field Software Version Number    */
	uint8   VersionLetter;      /* @field Software Version Letter    */
	uint32  KeyColor;           /* @field Key Color                  */
	uint16  PixelNumerator;     /* @field Pixel Aspect Ratio Numerator   */
	uint16  PixelDenominator;   /* @field Pixel Aspect Ratio Denominator */
	uint16  GammaNumerator;     /* @field Gamma Value                */
	uint16  GammaDenominator;   /* @field Gamma Value                */
	uint32 	ColorOffset;        /* @field Color Correction Offset    */
	uint32 	StampOffset;        /* @field Postage Stamp Offset       */
	uint32 	ScanOffset;         /* @field Scan Line Table Offset     */
	uint8   AttributesType;     /* @field Attributes Types           */
	uint32*	ScanLineTable;      /* @field Scan Line Table            */
	uint8  	StampWidth;         /* @field Width of postage stamp image  */
	uint8  	StampHeight;        /* @field Height of postage stamp image */
	uint8*	StampImage;					/* @field Postage Stamp Image        */
	uint16	ColorTable[1024];   /* @field Color Correction Table     */
};


// @struct TTgaFooter | TGA File Footer
struct _OWLFXCLASS TTgaFooter {
	
	TTgaFooter();

	uint32  ExtensionOffset;      /* @field Extension Area Offset      */
	uint32  DeveloperOffset;      /* @field Developer Directory Offset */
	char    Signature[18];        /* @field Signature, dot, and NULL   */
};
#include <poppack.h>

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//{{TDib = TTga}}
//-------------------------------------
// class TTga
// ~~~~~ ~~~~
//
//@class Tga format handling class
//@base public | TDib
class _OWLFXCLASS TTga : public __OWL TDib {
	public:
		TTga (const LPBITMAPINFO lpbi, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TTga(HGLOBAL bmpHandle, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TTga(const TTga& tga);
		TTga(const __OWL TDib& dib);
		TTga(LPCTSTR name);

		virtual ~TTga();

	protected:
		void Init();
		TTga();

		bool Read(IFileIn& file, bool readFileHeader);
		bool Write(IFileOut& file, bool writeFileHeader);

		bool ReadHeader(IFileIn& file);
		bool ReadBits(IFileIn& file);

		uint32	WriteBits(IFileOut& file);

		int16 EncodeScanLine(uint8 * EncodedBuffer, uint8 * UnencodedBuffer,
												 uint16 BufferSize, uint LineLength, uint PixelSize);
		int16 DecodeScanLine(uint8 * DecodedBuffer, uint8 * EncodedBuffer,
												 uint LineLength, uint PixelSize);

	private:
		TTga& operator =(const TTga&);

	public:
		TTgaHeader			Header;
		TTgaColorMap		Cmap;
		TTgaFooter	 		Footer;
		//TTgaDeveloper*	Developer;
		//TTgaExtension*	Extension;
		//uint32  				DataOffset;
};    

///////////////////////////////////////////////////////////////
//
// class TTgaDescriptor
// ~~~~~ ~~~~~~~~~~~~~~
//
class _OWLFXCLASS TTgaDescriptor: public TImageDescriptor {
	public:
		// Get image file description: "Targa Image|*.tga;*.vda;*.icb;*.vst|tga|"
		virtual bool	GetDescription(LPTSTR buffer, int size);

		// open instance
		virtual __OWL TDib* OpenInstance(LPCTSTR path);
		
		// create instance from existing Dib
		virtual __OWL TDib* CreateInstance(const __OWL TDib& dib);

		// checks intenal structure of file to verify it's validity.
		virtual bool	VerifyFile(LPCTSTR path);

		// Get color capabilities
		virtual bool GetCapabilities(uint& pixType, uint& addit);

		// get config dialog, dib - dib to configure, arent - parent
		virtual __OWL TDialog* GetConfigDlg(__OWL TDib& dib, __OWL TWindow* parent=0);
};
//////////////////////////////////////////////////////////////

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


_OWLFX_END_NAMESPACE

#endif  // OWLFX_TGA_H sentry.

