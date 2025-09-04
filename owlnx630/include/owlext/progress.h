//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// TDialog-derived class which provides a dialog box containing a gauge
// and a text box, and responds to defined messages to update the gauge.
//
// Works in both 16- and 32- bit environments
//
// Original code:
// Copyright (c) 1997 Rich Goldstein, MD
// goldstei@interport.net
//
// See THREDPRG.CPP for an example of use of this class
//
// May be used without permission, if appropriate credit is given
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_PROGRESS_H)
#define __OWLEXT_PROGRESS_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif


#ifndef OWL_DIALOG_H
# include <owl/dialog.h>
#endif
#ifndef OWL_STATIC_H
# include <owl/static.h>
#endif
#ifndef OWL_GAUGE_H
# include <owl/gauge.h>
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


/* WM_GSC_PROGRESSSTART arrives with LPARAM equal to the range (0-LPARAM)
   WM_GSC_PROGRESS arrives with LPARAM equal to current value
   WM_GSC_PROGRESSEND arrives without parameters
*/

#define WM_GSC_PROGRESS_START  WM_USER + 101
#define WM_GSC_PROGRESS        WM_USER + 102
#define WM_GSC_PROGRESS_END    WM_USER + 103


class OWLEXTCLASS TProgressDlg : public __OWL TDialog {
	// Internal (class-specific) constants
	//
	public:
		enum {
			// These are the control IDs within the progress dialog specified in
			// progress.rc; if they change, you need to reflect that change here.
			IDD_PROGRESS = 8001,
			IDC_TEXT = 101,
			IDC_GAUGE = 102
		};

	// Object lifetime methods
	//
	public:
		TProgressDlg(__OWL TWindow* parent, LPCTSTR text,
								 __OWL TResId id = IDD_PROGRESS,
								 __OWL TModule* mod = 0):
				__OWL TDialog(parent, id, mod), m_initText(text)
		{
			m_gauge = new __OWL TGauge(this, IDC_GAUGE);
			m_gauge->SetNativeUse(__OWL nuNever);
			// range etc set by messages. We use the default here
			m_text = new __OWL TStatic(this, IDC_TEXT);
		}

	// OWL-overridden methods
	//
	public:
		// This forces the use of TApplication::BeginModal to
		// execute this dialog.
		virtual int DoExecute()
			{ return __OWL TWindow::DoExecute(); }

	// Mutators
	//
	public:
		virtual void ProcessComplete()
		{
			// By default, the window is closed when the process completes
			CloseWindow(IDOK);
		}

		void SetMessage(LPCTSTR msg)
			{ m_text->SetText(msg); }
		void SetRange(int l, int h)
			{ m_gauge->SetRange(l, h); }
		void SetValue(int x)
			{ m_gauge->SetValue(x); }

	// OWL event methods
	//
	protected:
		LRESULT EvProgressStart(WPARAM, LPARAM range);
		LRESULT EvProgress(WPARAM, LPARAM value);
		LRESULT EvProgressEnd(WPARAM, LPARAM);
		virtual void SetupWindow()
		{
			// Call up the chain
			__OWL TDialog::SetupWindow();

			// Set our gauge & text to their initial values
			m_text->SetText(m_initText.c_str());
			m_gauge->SetLed(0);
		}

	// Internal data
	//
	protected:
		owl_string m_initText;
		__OWL TGauge* m_gauge;
		__OWL TStatic* m_text;

	DECLARE_RESPONSE_TABLE(TProgressDlg);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


#endif

