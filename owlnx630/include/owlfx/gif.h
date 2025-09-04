/////////////////////////////////////////////////////////////////////////////////////
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//
//	Class definition for TGif (TDib).
////////////////////////////////////////////////////////////////////////////////////
#if !defined(OWLFX_GIF_H)            // Sentry, use file only if it's not already included.
#define OWLFX_GIF_H

#include <owlfx/defs.h>
#include <owlfx/imgdescr.h>

_OWLFX_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>
//////////////////////////////////////
//@class GIF format handling class
//@base public | TGif
class _OWLFXCLASS TGif : public __OWL TDib {
	public:
		//enum TGifType { WindowsBitmap, OS1Bitmap, OS2Bitmap, };
		
		TGif (const LPBITMAPINFO lpbi, TAutoDelete autoDelete = NoAutoDelete);
		TGif(HGLOBAL bmpHandle, TAutoDelete autoDelete = NoAutoDelete);
		TGif(const TGif& bmp);
		TGif(const __OWL TDib& bmp);
		TGif(LPCTSTR name);
		virtual ~TGif();

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
		TGif(): __OWL TDib(){};

	private:
		TGif& operator =(const TGif&);

	public:
    //TBitmapType Type;
    //bool				Compressed;
};    

///////////////////////////////////////////////////////////////
//
// class TGifDescriptor
// ~~~~~ ~~~~~~~~~~~~~~
//
class _OWLFXCLASS TGifDescriptor: public TImageDescriptor {
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

		// get config dialog, dib - dib to configure, parent - parent
		virtual __OWL TDialog* GetConfigDlg(__OWL TDib& dib, __OWL TWindow* parent=0);

};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
//////////////////////////////////////////////////////////////

_OWLFX_END_NAMESPACE

#endif   // OWLFX_GIF_H sentry.
