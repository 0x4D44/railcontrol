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
#if !defined(OWLFX_PREVIEWD_H) // Sentry, use file only if it's not already included.
#define OWLFX_PREVIEWD_H

#include <owlfx/defs.h>

//#include <owl/window.h>
#include <owl/opensave.h>



//__OWL_DECLCLASS(TStatic);
//__OWL_DECLCLASS(TDib);
//__OWL_DECLCLASS(TDialog);
//__OWL_DECLCLASS(TWindow);

_OWLFX_BEGIN_NAMESPACE


// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


///////////////////////////////////////////////////////////////
//{{TControl = TPictureControl}}
//@class TPictureControl | Picture control
//@base public | TControl
class _OWLFXCLASS TPictureControl: public __OWL TControl {
	public:
		// create from scratch
		TPictureControl(__OWL TWindow* parent, int Id, __OWL TDib* dib, __OWL TModule*  module = 0);
		// create from resource
		TPictureControl(__OWL TWindow* parent, int Id, __OWL TModule*  module = 0);
		~TPictureControl();

		__OWL TDib* SetDib(__OWL TDib* dib) { __OWL TDib* tmp = Dib; Dib = dib; return tmp;}
		__OWL TDib* GetDib() { return Dib; }

	protected:
		__OWL TDib* 	 	Dib;
		HANDLE    hDDC;

	protected:
		virtual void	Paint(__OWL TDC& dc, bool /*erase*/, __OWL TRect& /*rect*/);
		virtual void	ODADrawEntire(DRAWITEMSTRUCT  &drawInfo);
		bool					EvEraseBkgnd(HDC /*hDC*/);
		bool 					EvQueryNewPalette();

	DECLARE_RESPONSE_TABLE(TPictureControl);
}; //{{TPictureControl}}

class TImagePreviewWnd;

//{{GENERIC = TImagePreviewBase}}
//
// class TImagePreviewBase
// ~~~~~ ~~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TImagePreviewBase {
	public:
		TImagePreviewBase();

		void HandleNewFileName(__OWL TDialog& dlg, LPCTSTR file);
		virtual __OWL TDib* CreateImage(LPCTSTR path);
		virtual void	SetDialogFilter(__OWL TFileOpenDialog::TData& data, bool openDlg=true);
	
	protected:
		bool BaseDialogFunction(__OWL TDialog* dlg, uint message, __OWL TParam1, __OWL TParam2);

	protected:
		TImagePreviewWnd*	PreWnd;
		__OWL TRect 						Dest;
		bool							IsTimer;
};

/* -------------------------------------------------------------------------- */
//{{TFileOpenDialog = TFileOpenPreviewDlg}}
//@class TFileOpenPreviewDlg<nl>
//
// TFileOpenPreviewDlg encapsulates the FileOpen dialog used by the
// TDocView manager. It enhances the standard ObjectWindows TFileOpenDialog
// by providing logic to show picture saved in project.
//@base public | TFileOpenDialog, TImagePreviewBase
class _OWLFXCLASS TFileOpenPreviewDlg: public __OWL TFileOpenDialog, 
                                       public TImagePreviewBase {
	public:
    TFileOpenPreviewDlg(__OWL TWindow* parent,
			                  TData& data, 
												__OWL TResId templateID = 0, 
												LPCTSTR title = 0,  
												__OWL TModule* module = 0);
		virtual ~TFileOpenPreviewDlg();

		virtual void SetupWindow();
		virtual void CleanupWindow();

	protected:
		bool DialogFunction(uint message, __OWL TParam1, __OWL TParam2);

}; //{{TFileOpenPreviewDlg}}

//////////////////////////////////////////////////////
//{{TFileOpenDialog = TFileSavePreviewDlg}}
//@class TFileSavePreviewDlg<nl>
//
// MCBaseOpenDlg encapsulates the FileOpen dialog used by the
// MDocView manager. It enhances the standard ObjectWindows TFileOpenDialog
// by providing logic to show picture saved in project.
//@base public | TFileOpenDialog, MCommonBaseDlg
class _OWLFXCLASS TFileSavePreviewDlg: public __OWL TFileSaveDialog, 
                                       public TImagePreviewBase {
	public:
		TFileSavePreviewDlg(__OWL TWindow* parent, 
			                  TData& data, 
												__OWL TResId templateID = 0, 
												LPCTSTR title = 0,  
												__OWL TModule* module = 0);
		virtual ~TFileSavePreviewDlg();

		virtual void SetupWindow();
		virtual void CleanupWindow();

	protected:
		bool DialogFunction(uint message, __OWL TParam1, __OWL TParam2);
};    //{{TFileSavePreviewDlg}}

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

_OWLFX_END_NAMESPACE

#endif  // OWLFX_PREVIEWD_H sentry.

