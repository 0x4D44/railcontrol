/////////////////////////////////////////////////////////////////////////////////////
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//
//	Class definition for TBmp (TDib).
////////////////////////////////////////////////////////////////////////////////////
#if !defined(OWLFX_BMP_H)            // Sentry, use file only if it's not already included.
#define OWLFX_BMP_H

#include <owl/gdiobjec.h>

#include <owlfx/defs.h>
#include <owlfx/imgdescr.h>


_OWLFX_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>
//////////////////////////////////////
//@class BMP format handling class
//@base public | TBmp
class _OWLFXCLASS TBmp : public __OWL TDib {
	public:
		enum TBitmapType { WindowsBitmap, OS1Bitmap, OS2Bitmap, };
		
		TBmp (const LPBITMAPINFO lpbi, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TBmp(HGLOBAL bmpHandle, __OWL TAutoDelete autoDelete = __OWL NoAutoDelete);
		TBmp(const TBmp& bmp);
		TBmp(const __OWL TDib& bmp);
		TBmp(LPCTSTR name);
		virtual ~TBmp();

	protected:
		// overriding virtuals
    virtual bool Read(IFileIn& in, bool readFileHeader = false);
    virtual bool Write(IFileOut& out, bool writeFileHeader = false);

		bool		ReadColors(IFileIn& file);
		uint32	ReadHeader(IFileIn& file, BITMAPINFOHEADER& infoHeader);
		bool		ReadBits(IFileIn& file);

		bool		WriteInfoHeader(IFileOut& file);
		bool		WriteBits(IFileOut& file);

		int16		EncodeScanLine(uint8* EncodedBuffer, uint8* src, uint16 BufferSize);
		int16		DecodeScanLine(uint8* DecodedBuffer, uint8* src, uint16 BufferSize);

	protected:
		TBmp(): __OWL TDib(){};

	private:
		TBmp& operator =(const TBmp&);

	public:
    TBitmapType Type;
    bool				Compressed;
};    

///////////////////////////////////////////////////////////////
//
// class TBmpDescriptor
// ~~~~~ ~~~~~~~~~~~~~~
//
class _OWLFXCLASS TBmpDescriptor: public TImageDescriptor {
	public:
		// Get image file description: "Bitmap Image|*.bmp;*.dib;*.rle|bmp|"
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

#endif   // OWLFX_BMP_H sentry.
