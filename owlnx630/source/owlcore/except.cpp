//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.3 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2009-12-16 09:49:28 $                                                    //
//
// Implementation of class TXOwl, the base exception class for OWL exceptions
// that can forward handling to the app module by default.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_EXCEPT_H)
# include <owl/except.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// mbModalFlag() determines the best MB modal flag to use in the current
// situation. Uses MB_TASKMODAL if under NT, or the task/thread has at least
// one toplevel window. 
//
bool CALLBACK hasWndProc(TWindow::THandle, bool * hasWnd)
{
	*hasWnd = true;
	return false;
}

static unsigned mbModalFlag()
{
	// NT can always open task modal, even before windows are up
	//
	if (TSystem::IsNT())
		return MB_TASKMODAL;

	// Windows95 need first window created before task modal msg
	// box can be created
	//
	bool hasWnds = false;
	::EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)hasWndProc,
		TParam2(&hasWnds));

	return hasWnds ? MB_TASKMODAL : MB_SYSTEMMODAL;
}

//
// Global exception handler used when an application object is not available.
// May be overriden by user code by redefining this function. Note that the
// program must be linked statically to OWL in order for an overridden version
// of this function to be called by the framework. If a valid
// application object is found by GetApplicationObject, then the virtual
// TModule::Error(TXOwl& x, char* caption, bool canResume) is usually used
// instead.
//
_OWLFUNC(int)
HandleGlobalException(XBASE& x, LPCTSTR caption, LPCTSTR canResume)
{
#if BI_MSG_LANGUAGE == 0x0411
	static const char defCaption[] = "–¢ˆ—‚Ì—áŠO";
	static const char defError[] = "–?’m‚I—aSO";
#else
	static const TCHAR defCaption[] = _T("Unhandled Exception");
	static const TCHAR defError[] = _T("Unknown Exception");
#endif

	owl_string msg;
#if defined(BI_STD_EXCEPTIONS)
	const TXBase* px = TYPESAFE_DOWNCAST(&x, const TXBase);
	if (px != 0)
	{
		msg = px->why();
	}
	else 
#elif defined(UNICODE)
#error This code is not supported under Unicode when no standard exceptions are available
#endif
    if (x.XWHY_STR && strlen(x.XWHY_STR) > 0) { // Jogy: XWHY_STR is always const char*.
		_USES_CONVERSION;
		msg = _A2W(x.XWHY_STR);
	}
	else
		msg = defError;

	if (canResume) {
		msg += _T('\n');
		msg += canResume;
	}

	const int flags = MB_ICONSTOP | MB_TOPMOST | mbModalFlag() | (canResume ? MB_YESNO : MB_OK);
	const int r = ::MessageBox(0, msg.c_str(), caption ? caption : defCaption, flags);
	return (r == IDYES) ? 0 : -1;
}

//----------------------------------------------------------------------------

//
// An OWL exception with a given message for displaying and an unsigned Id
// that can be used for identification or loading a string
//
TXOwl::TXOwl(const owl_string& msg, uint resId)
:
TXBase(msg),
ResId(resId)
{
}

//
// An OWL exception with a given unsigned Id that can is used for loading a
// message string & identification
//
TXOwl::TXOwl(unsigned resId, TModule* module)
:
TXBase(ResourceIdToString(0, resId, module)),
ResId(resId)
{
}

//
//
//
#if defined(BI_STD_EXCEPTIONS)
TXOwl::~TXOwl() THROW_SPEC0
#else
TXOwl::~TXOwl()
#endif
{
}

//
//
//
int
TXOwl::Unhandled(TModule* app, uint promptResId)
{
	return app->Error(*this, IDS_OWLEXCEPTION, promptResId);
}

#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXOwl*
#endif
TXOwl::Clone()
{
	return new TXOwl(*this);
}

//
//
//
void
TXOwl::Throw()
{
	THROW( *this );
}

//
// Construct a TXOwl exception from scratch, and throw it. Two versions
// corresponding to the two constructor signatures
//
void
TXOwl::Raise(const owl_string& msg, uint resId)
{
	TXOwl(msg, resId).Throw();
}

//
//
//
void
TXOwl::Raise(uint resId, TModule* module)
{
	TXOwl(resId, module).Throw();
}

//
// Static member function used to convert a resource id to a 'string'. This
// is necessary since we must pass a string to the xmsg base class
// constructor.  Sets found to true if the resource was located, otherwise
// false.  In either case, the string is initialized to something
// printable.
//
owl_string
TXOwl::ResourceIdToString(bool* found, uint resId, TModule* module)
{
	TCHAR buf[128];

	bool status = module && module->LoadString(resId, buf, COUNTOF(buf));
	if (found)
		*found = status;

	if (!status)
#if BI_MSG_LANGUAGE == 0x0411
		sprintf(buf, "—áŠO #%u (Ò¯¾°¼Þ‚ª—pˆÓ‚³‚ê‚Ä‚¢‚Ü‚¹‚ñ. <owl/except.rc> ‚ÌÊÞ²ÝÄÞ‚ðŠm”F‚µ‚Ä‚­‚¾‚³‚¢).", resId);
#else
		_stprintf(buf, _T("Exception #%u (Could not load description string; <owl/except.rc> not bound?)."), resId);
#endif

	owl_string rscStr(buf);
	return rscStr;
}

//
// Extension to string loader adds the feature of sprintf'ing an
// additional information string into the resource message string.
//
owl_string
TXOwl::MakeMessage(uint resId, LPCTSTR infoStr, TModule* module)
{
	owl_string rscMsg = ResourceIdToString(0, resId, module);
	TCHAR buf[255];
	_stprintf(buf, rscMsg.c_str(), infoStr);
	return owl_string(buf);
}

//
// Extension to string loader adds the feature of sprintf'ing an additional
// information number into the resource message string.
//
owl_string
TXOwl::MakeMessage(uint resId, uint infoNum, TModule* module)
{
	owl_string rscMsg = ResourceIdToString(0, resId, module);
	TCHAR buf[255];
	_stprintf(buf, rscMsg.c_str(), infoNum);
	return owl_string(buf);
}
#if defined(UNICODE)
owl_string 
TXOwl::MakeMessage(uint resId, LPCSTR infoStr, TModule* module)
{
	_USES_CONVERSION;
	owl_string rscMsg = ResourceIdToString(0, resId, module);
	TCHAR buf[255];
	_stprintf(buf, rscMsg.c_str(), _A2W(infoStr));
	return owl_string(buf);
}
#endif
//----------------------------------------------------------------------------

//
//
//
TXOutOfMemory::TXOutOfMemory()
:
TXOwl(IDS_OUTOFMEMORY)
{
}

#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXOutOfMemory*
#endif
TXOutOfMemory::Clone()
{
	return new TXOutOfMemory(*this);
}

//
//
//
void
TXOutOfMemory::Throw()
{
	THROW( *this );
}

//
// Construct a TXOutOfMemory exception from scratch, and throw it
//
void
TXOutOfMemory::Raise()
{
	TXOutOfMemory().Throw();
}
//
//
//
TXNotSupportedCall::TXNotSupportedCall()
:
TXOwl(IDS_NOTSUPPORTEDCALL)
{
}

#if defined(BI_NO_COVAR_RET)
TXBase* 
#else
TXNotSupportedCall* 
#endif
TXNotSupportedCall::Clone()
{
	return new TXNotSupportedCall(*this);
}
//
void TXNotSupportedCall::Throw()
{
	THROW( *this );
}
//
void TXNotSupportedCall::Raise()
{
	TXOutOfMemory().Throw();
}


///
__OWL_END_NAMESPACE
/* ========================================================================== */
