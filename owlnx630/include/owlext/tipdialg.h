//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
//  Copyright © 1998 by Yura Bidus. All Rights Reserved.
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TTipDialog (TDialog).
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
//------------------------------------------------------------------------------
#if !defined(OWLEXT_TIPDIALG_H) // Sentry, use file only if it's not already included.
#define OWLEXT_TIPDIALG_H


#include <owl/filename.h>

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class OWLEXTCLASS TTipDialog : public __OWL TDialog {
	public:
		TTipDialog(__OWL TWindow *parent, LPCTSTR regkey, __OWL TModule* module = 0);

		virtual ~TTipDialog();
		bool ShowOnStartup();

	protected:
		bool		 		ShowDlg;
		bool		 		TipError;
		owl_string	CurrentTip;
		owl_string	RegKey;
		_tifstream 		TipFile;
		__OWL TFileName		FileName;
		__OWL TBitmap* 		BulbBitmap;

	public:
		virtual void SetupWindow ();
		virtual void CleanupWindow ();

	protected:
		virtual void GetNextTipString();
		virtual void OpenTipFile(long pos);
		virtual long CloseTipFile();
		//Get User data from registry or from file, can be overriden
		// to get data from else place
		virtual long ReadSettings();
		//Write User data to registry or to file, can be overriden
		// to get data from else place
		virtual void WriteSettings(long pos);
		virtual owl_string GetTipName();
		virtual __OWL TBitmap* GetBitmap();

	protected:
		void EvPaint ();
		void CmNextTip();
		HBRUSH EvCtlColor (HDC hDC, HWND hWndChild, uint ctlType);

	DECLARE_RESPONSE_TABLE(TTipDialog);
	DECLARE_STREAMABLE(OWLEXTCLASS, __OWLEXT TTipDialog, 1);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif     // OWLEXT_TIPDIALG_H sentry.
