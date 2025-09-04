//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// This file implements the TBusyCursor class.
//
// Original code by Don Griffin; used with permission.
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:39 $
//-------------------------------------------------------------------
#include <owlext\pch.h>
#pragma hdrstop


#include <owlext/busy.h>

#include <owl/statusba.h>
#include <owlext/enumwnd.h>
#include <owlext/util.h>

__OWL_USING_NAMESPACE

OWLEXT_BEGIN_NAMESPACE


#ifdef _DEBUG
/*
static owl_string WndText (HWND hWnd){
char    text [33];

::GetWindowText (hWnd, text, sizeof text);
return owl_string(text);
}

static owl_string WndClass (HWND hWnd)
{
char className [80];

::GetClassName (hWnd, className, sizeof className);

return owl_string (className);
}
*/
#endif

static bool FilterWindow (HWND hWnd)
{
	bool filter = !::IsWindowEnabled (hWnd) || !::IsWindowVisible (hWnd);
	return filter;
}

//--------------------------------------------------------------------------
class TCountWindows : public __OWLEXT TEnumWindows{
public:
	TCountWindows () { mCount = 0; }

	uint            Count () const { return mCount; }

protected:
	uint            mCount;

	virtual bool    OnEnumWindow (HWND hWnd);
};

bool
TCountWindows::OnEnumWindow (HWND hWnd)
{
	if(!FilterWindow (hWnd))
		++mCount;

	return true;
}

//--------------------------------------------------------------------------

struct TWindowSubclass{
	HWND   	hWnd;
	WNDPROC fnPrevWndProc;
};

class TSubclassWindows : public __OWLEXT TEnumWindows {
public:
	TSubclassWindows (WNDPROC fnSubclass, TWindowSubclass * wnd)
		: mFnSubclass (LPARAM(fnSubclass)), mWnd (wnd) { }

protected:
	TWindowSubclass*	mWnd;
	LPARAM            mFnSubclass;

	virtual bool    OnEnumWindow (HWND hWnd);
};

bool TSubclassWindows::OnEnumWindow (HWND hWnd)
{
	if (!FilterWindow (hWnd)){
		mWnd->hWnd = hWnd;
		mWnd->fnPrevWndProc = (WNDPROC)::SetWindowLong (hWnd, GWL_WNDPROC,
			mFnSubclass);
		++mWnd;
	}
	return true;
}

//------------------------------------------------------------------------------
enum{
	WM_NCMOUSEFIRST = WM_NCMOUSEMOVE,
	WM_NCMOUSELAST  = WM_NCMBUTTONDBLCLK
};

class TBusyHook{
public:
	TBusyHook ();
	~TBusyHook ();

private:
	TAPointer<TWindowSubclass>  mSubclass;
	HCURSOR                     mBusyCursor;
	uint                        mCount;
	uint                        mHideCaretCount;

	LRESULT                 BusyWndMethod (HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK BusyWndProc (HWND, UINT, WPARAM, LPARAM);
	LRESULT                 DefWndProc (HWND, UINT, WPARAM, LPARAM);
};

static TBusyHook* sHook;

TBusyHook::TBusyHook()
{
	TCountWindows   counter;

	counter.EnumWindows (__OWLEXT TEnumWindows::emAllWindows);
	mCount      = counter.Count();
	mSubclass   = new TWindowSubclass [mCount];

	TSubclassWindows subclass (BusyWndProc, mSubclass);

	subclass.EnumWindows (__OWLEXT TEnumWindows::emAllWindows);

	HideCaret (0);
	mHideCaretCount = 1;
	mBusyCursor = ::LoadCursor (0, IDC_WAIT);
}

TBusyHook::~TBusyHook ()
{
	for (int n = 0; n < (int)mCount; ++n)
		::SetWindowLong (mSubclass[n].hWnd, GWL_WNDPROC,
		(LPARAM)mSubclass[n].fnPrevWndProc);

	while (mHideCaretCount--)
		ShowCaret (0);

	UpdateCursor();
}

LRESULT CALLBACK 
TBusyHook::BusyWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return sHook->BusyWndMethod (hWnd, uMsg, wParam, lParam);
}

LRESULT
TBusyHook::BusyWndMethod (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND    hTopMostWindow;

	switch (uMsg){
case WM_SETCURSOR:
	::SetCursor (mBusyCursor);
	return TRUE;

case WM_MOUSEACTIVATE:
	hTopMostWindow = (HWND) wParam;

	if (hTopMostWindow == hWnd)
		return MA_ACTIVATEANDEAT;

	return ::DefWindowProc (hWnd, uMsg, wParam, lParam);

case WM_SETFOCUS:
	DefWndProc (hWnd, uMsg, wParam, lParam);
	HideCaret (0);
	++mHideCaretCount;
	return 0;
	}

	if ((uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) ||
		(uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) ||
		(uMsg >= WM_NCMOUSEFIRST && uMsg <= WM_NCMOUSELAST))
	{
		return 0; // ::DefWindowProc (hWnd, uMsg, wParam, lParam);
	}

	return DefWndProc (hWnd, uMsg, wParam, lParam);
}

LRESULT TBusyHook::DefWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int    n;
	for (n = 0; n < (int)mCount; ++n)
		if (mSubclass[n].hWnd == hWnd)
			return CallWindowProc(mSubclass[n].fnPrevWndProc, hWnd, uMsg, wParam, lParam);

	PRECONDITION (0);
	return 0;
}

//--------------------------------------------------------------------------

TBusyCursor* TBusyCursor::sTop;

TBusyCursor::TBusyCursor (owl_string message)
:
mMessage (message)
{
	Init();
}


TBusyCursor::TBusyCursor ()
{
	Init();
}


void TBusyCursor::Init ()
{
	mNext       = sTop;
	sTop        = this;
	mActive     = true;
	mBusyCursor = ::LoadCursor (0, IDC_WAIT);

	Activate ();
}


TBusyCursor::~TBusyCursor ()
{
	PRECONDITION(sTop == this);  // must be destructed in reverse order...

	Active(mNext ? mNext->mActive : false);  // update active state
	sTop = mNext;
}


////////////////////////////////////////////////////////////////////////////
//  This method activates or deactivates the busy state.
//
void TBusyCursor::Active (bool active)
{
	if (active == mActive)
		// Already active
		return;

	mActive = active;
	if (sTop != this)
		return;  // we're not top-most, so we don't control the global state


	if (mActive)
		Activate();
	else
		Deactivate();
}


////////////////////////////////////////////////////////////////////////////
//  This method activates the busy state.
//
void TBusyCursor::Activate ()
{
	::SetCursor (mBusyCursor);
	PRECONDITION(sHook == 0);
	sHook = new TBusyHook;

	UpdateMessage (mMessage.c_str());
}


////////////////////////////////////////////////////////////////////////////
//  This method deactivates the busy state.
//
void TBusyCursor::Deactivate ()
{
	PRECONDITION (sHook != 0);
	delete sHook;
	sHook = 0;

	UpdateMessage (0);
}


////////////////////////////////////////////////////////////////////////////
//  This method sets the message for the status bar.
//
void TBusyCursor::Message (owl_string message)
{
	mMessage = message;
	if (sTop == this && Active())
		UpdateMessage (mMessage);
}


////////////////////////////////////////////////////////////////////////////
//  This method flushes the message to the status bar.
//
void TBusyCursor::UpdateMessage(LPCTSTR message)
{
	//TApplication* app = GetApplicationObject();
	TStatusBar* bar = 0;

	if (bar){
		bar->SetText (message);
		bar->UpdateWindow();
	}
}


OWLEXT_END_NAMESPACE

//==============================================================================
