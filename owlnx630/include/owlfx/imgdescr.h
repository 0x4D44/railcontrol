//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//-------------------------------------------------------------------
#if !defined(OWLFX_IMGDESCR_H) // Sentry, use file only if it's not already included.
#define OWLFX_IMGDESCR_H

#include <owlfx/defs.h>

_OWLFX_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

#include <owl/dc.h>
#include <owl/dialog.h>

//class __OWL TDib;
//class __OWL TDialog;
//class __OWL TWindow;


//{{GENERIC = TImageDescriptor}}
//
// class TImageDescriptor
// ~~~~~ ~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TImageDescriptor {
	public:
		enum TPixelType{ 	
					pixcNone			= 0x0000, 			/* error rrrrrrrrrrrrrrrrrrrrrrrr				*/
					pixcARGB			= 0x0001, 			/* 32 bit PC standard supportes  				*/
					pixcRGB				= 0x0002,       /* 24 bit PC standard supportes         */
					pixcRGB555		= 0x0004,       /* 16 bit PC standard r:g:b:1 supportes */
					pixcRGB_INDEX	= 0x0008,     	/*  8 bit PC standard supportes         */
		};
		enum TAddCapab{ 	
					cabCanConfig	= 0x0000,
					cabCanRead  	= 0x0001,
					cabCanWrite  	= 0x0002,
		};
	public:
		TImageDescriptor():Next(0){}
		virtual ~TImageDescriptor(){}
		
		// Get image file description: "Bitmap Files|*.bmp;*.dib|bmp|"
		virtual bool GetDescription(LPTSTR buffer, int size) = 0;

		// open instance from file
		virtual __OWL TDib* OpenInstance(LPCTSTR path) = 0;
		
		// create instance from existing Dib
		virtual __OWL TDib* CreateInstance(const __OWL TDib& dib) = 0;
		
		// checks intenal structure of file to verify it's validity.
		virtual bool	VerifyFile(LPCTSTR path) = 0;
		
		// Get color capabilities
		virtual bool GetCapabilities(uint& pixType, uint& addit) = 0;

		// get config dialog, dib - dib to configure, arent - parent
		virtual __OWL TDialog* GetConfigDlg(__OWL TDib& dib, __OWL TWindow* parent=0) = 0;

		// Return a matching template given a path.
		//
		// This implementation compares the path's extension with the ';' separated
		// filter lists in each template looking for a case-insensitive match. Allows
		// '?'s in the template filters to match any character in the path's extension
		//
		// Example:
		//   The path: "documents.1995\accounts_receivable.plan"
		//   Would match the filter: "*.xls;*.wk?;*.plan"
		static TImageDescriptor* MatchTemplate(LPCTSTR path, bool write=false);
		
		// find ImageDescritor and Create Instance of TDib
		static __OWL TDib* CreateImage(LPCTSTR path);
		
		// registrer ImageDescriptor instance
		static void Register(TImageDescriptor* item);
		
		// unregistrer ImageDescriptor instance
		static void Unregister(TImageDescriptor* item);
		
		// Walk on all ImageDescriptors
		// if item = 0 return ImageDescriptorHead
		static TImageDescriptor* GetNext(TImageDescriptor* item=0);
		
	protected:
		//
		// Helper method used by 'MatchTemplate' to determine whether a filename
		// fits the pattern [wilcard] described by a filter.
		//
		static bool IsAMatch(LPCTSTR path, LPCTSTR fltr);

	protected_data:
		TImageDescriptor* Next;
		static TImageDescriptor* ImageDescriptorHead;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

extern "C" _OWLFXFUNC(void) RegisterImageDescriptors();
extern "C" _OWLFXFUNC(void) UnregisterImageDescriptors();

_OWLFX_END_NAMESPACE


#endif  // OWLFX_IMGDESCR_H sentry.

