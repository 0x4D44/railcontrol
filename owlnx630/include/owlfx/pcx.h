//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//
// Class definition for TPcx (TDib).
//-------------------------------------------------------------------
#if !defined(OWLFX_PCX_H)            // Sentry, use file only if it's not already included.
#define OWLFX_PCX_H

#include <owlfx/defs.h>
#include <owlfx/imgdescr.h>

_OWLFX_BEGIN_NAMESPACE

///////////////////////////////////////////////////////////////////
/*   The PCX header format                                       */  
#include <pshpack1.h>
struct TPcxHeader {            /* Offset   Description            */
	uint8  Id;                  /*  00h     Manufacturer ID        */
	uint8  Version;             /*  01h     Version                */
	uint8  Compression;         /*  02h     Encoding Scheme        */
	uint8  BitsPixelPlane;      /*  03h     Bits/Pixel/Plane       */
	uint16 Xmin;                /*  04h     X Start (upper left)   */
	uint16 Ymin;                /*  06h     Y Start (top)          */
	uint16 Xmax;                /*  08h     X End (lower right)    */
	uint16 Ymax;                /*  0Ah     Y End (bottom)         */
	uint16 Hdpi;                /*  0Ch     Horizontal Resolution  */
	uint16 Vdpi;                /*  0Eh     Vertical Resolution    */
	uint8  EgaPalette[48];      /*  10h     16-Color EGA Palette   */
	uint8  Reserved;            /*  40h     Reserved               */
	uint8  NumberOfPlanes;      /*  41h     Number of Color Planes */
	uint16 BytesLinePlane;      /*  42h     Bytes/Line/Plane       */
	uint16 PaletteInfo;         /*  44h     Palette Interpretation */
	uint16 HScreenSize;         /*  46h     Horizontal Screen Size */
	uint16 VScreenSize;         /*  48h     Vertical Screen Size   */
	uint8  Filler[54];          /*  4Ah     Reserved               */
};

/*  PCX VGA palette.                                             */
struct TPcxPalette {
	uint8 Palette[768];     /*          256 VGA Color Palette  */
};
#include <poppack.h>

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>
///////////////////////////////
//PCX format handling class
// class TPcx
// ~~~~~ ~~~~
// 
//@class PCX format handling class
//@base public | TDib
class _OWLFXCLASS TPcx : public __OWL TDib {
	public:
		TPcx(const LPBITMAPINFO lpbi, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TPcx(HGLOBAL handle, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TPcx(LPCTSTR name);
		TPcx(const TPcx& pcx);
		TPcx(const __OWL TDib& pcx);
		TPcx();

		virtual ~TPcx();


	protected:
		virtual bool Read(IFileIn& file, bool readFileHeader);
		virtual bool Write(IFileOut& file, bool writeFileHeader);

		bool ReadColors(IFileIn& file);
		uint32 ReadHeader(IFileIn& file, BITMAPINFOHEADER& infoHeader);
		bool ReadBits(IFileIn& file);
		
		bool WriteHeader(IFileOut& file);
		bool WriteBits(IFileOut& file);
		bool WriteColors(IFileOut& file);
		
		int16 EncodeScanLine(uint8* EncodedBuffer, uint8* src, uint16 BufferSize);
		int16 DecodeScanLine(uint8* DecodedBuffer, uint8* src, uint16 BufferSize);

	private:
		TPcx& operator =(const TPcx&);
		void Init();

	private:
	  TPcxHeader	Header;
  	uint8				RunCount;   /* Length of decoded pixel run     */
  	uint8				RunValue;   /* Value of decoded pixel run      */

}; 

///////////////////////////////////////////////////////////////
//
// class TPcxDescriptor
// ~~~~~ ~~~~~~~~~~~~~~
//
class _OWLFXCLASS TPcxDescriptor: public TImageDescriptor {
	public:
		// Get image file description: "PCX (Z-Soft) Image|*.pcx|pcx|"
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

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
//////////////////////////////////////////////////////////////

_OWLFX_END_NAMESPACE

#endif  // OWLFX_PCX_H sentry.
