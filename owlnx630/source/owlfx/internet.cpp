//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.1.4.2 $
//
// Implementation
//----------------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop

#include <owlfx/internet.h>

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE
__OWL_USING_STD_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////
//
// class TWinInet
// ~~~~~ ~~~~~~
// delay loading WININET.DLL
static const _TCHAR winInetStr[]						= _T("WININET.DLL");

static const char InternetCloseHandleStr[]				= "InternetCloseHandle";
static const char InternetSetStatusCallbackStr[]	= "InternetSetStatusCallback";
static const char InternetConfirmZoneCrossingStr[]= "InternetConfirmZoneCrossing";
static const char InternetTimeFromSystemTimeStr[]	= "InternetTimeFromSystemTime";
static const char InternetTimeToSystemTimeStr[]		= "InternetTimeToSystemTime";
static const char InternetAttemptConnectStr[]			= "InternetAttemptConnect";
static const char InternetReadFileStr[]						= "InternetReadFile";
static const char InternetWriteFileStr[]					= "InternetWriteFile";
static const char InternetSetFilePointerStr[]			= "InternetSetFilePointer";
static const char InternetQueryDataAvailableStr[]	= "InternetQueryDataAvailable";
static const char InternetErrorDlgStr[]						= "InternetErrorDlg";
#if defined(UNICODE)
static const char InternetConnectStr[]						= "InternetConnectW";
static const char InternetOpenStr[]								= "InternetOpenW";
static const char InternetQueryOptionStr[]				= "InternetQueryOptionW";
static const char InternetSetOptionStr[]					= "InternetSetOptionW";
static const char InternetSetOptionExStr[]				= "InternetSetOptionExW";
static const char InternetFindNextFileStr[]				= "InternetFindNextFileW";
static const char InternetGetLastResponseInfoStr[]= "InternetGetLastResponseInfoW";
static const char InternetCrackUrlStr[]						= "InternetCrackUrlW";
static const char InternetCreateUrlStr[]					= "InternetCreateUrlW";
static const char InternetCanonicalizeUrlStr[]		= "InternetCanonicalizeUrlW";
static const char InternetCombineUrlStr[]					= "InternetCombineUrlW";
static const char InternetOpenUrlStr[]						= "InternetOpenUrlW";
static const char FtpFindFirstFileStr[]						= "FtpFindFirstFileW";
static const char FtpGetFileStr[]									= "FtpGetFileW";
static const char FtpPutFileStr[]									= "FtpPutFileW";
static const char FtpDeleteFileStr[]							= "FtpDeleteFileW";
static const char FtpRenameFileStr[]							= "FtpRenameFileW";
static const char FtpOpenFileStr[]								= "FtpOpenFileW";
static const char FtpCreateDirectoryStr[]					= "FtpCreateDirectoryW";
static const char FtpRemoveDirectoryStr[]					= "FtpRemoveDirectoryW";
static const char FtpSetCurrentDirectoryStr[]			= "FtpSetCurrentDirectoryW";
static const char FtpGetCurrentDirectoryStr[]			= "FtpGetCurrentDirectoryW";
static const char GopherFindFirstFileStr[]				= "GopherFindFirstFileW";
static const char GopherOpenFileStr[]							= "GopherOpenFileW";
static const char GopherCreateLocatorStr[]				= "GopherCreateLocatorW";
static const char GopherGetLocatorTypeStr[]				= "GopherGetLocatorTypeW";
static const char GopherGetAttributeStr[]					= "GopherGetAttributeW";
static const char HttpOpenRequestStr[]						= "HttpOpenRequestW";
static const char HttpAddRequestHeadersStr[]			= "HttpAddRequestHeadersW";
static const char HttpSendRequestStr[]						= "HttpSendRequestW";
static const char HttpQueryInfoStr[]							= "HttpQueryInfoW";
static const char InternetGetCookieStr[]					= "InternetGetCookieW";
static const char InternetSetCookieStr[]					= "InternetSetCookieW";

#else
static const char InternetConnectStr[]						= "InternetConnectA";
static const char InternetOpenStr[]								= "InternetOpenA";
static const char InternetQueryOptionStr[]				= "InternetQueryOptionA";
static const char InternetSetOptionStr[]					= "InternetSetOptionA";
static const char InternetSetOptionExStr[]				= "InternetSetOptionExA";
static const char InternetFindNextFileStr[]				= "InternetFindNextFileA";
static const char InternetGetLastResponseInfoStr[]= "InternetGetLastResponseInfoA";
static const char InternetCrackUrlStr[]						= "InternetCrackUrlA";
static const char InternetCreateUrlStr[]					= "InternetCreateUrlA";
static const char InternetCanonicalizeUrlStr[]		= "InternetCanonicalizeUrlA";
static const char InternetCombineUrlStr[]					= "InternetCombineUrlA";
static const char InternetOpenUrlStr[]						= "InternetOpenUrlA";
static const char FtpFindFirstFileStr[]						= "FtpFindFirstFileA";
static const char FtpGetFileStr[]									= "FtpGetFileA";
static const char FtpPutFileStr[]									= "FtpPutFileA";
static const char FtpDeleteFileStr[]							= "FtpDeleteFileA";
static const char FtpRenameFileStr[]							= "FtpRenameFileA";
static const char FtpOpenFileStr[]								= "FtpOpenFileA";
static const char FtpCreateDirectoryStr[]					= "FtpCreateDirectoryA";
static const char FtpRemoveDirectoryStr[]					= "FtpRemoveDirectoryA";
static const char FtpSetCurrentDirectoryStr[]			= "FtpSetCurrentDirectoryA";
static const char FtpGetCurrentDirectoryStr[]			= "FtpGetCurrentDirectoryA";
static const char GopherFindFirstFileStr[]				= "GopherFindFirstFileA";
static const char GopherOpenFileStr[]							= "GopherOpenFileA";
static const char GopherCreateLocatorStr[]				= "GopherCreateLocatorA";
static const char GopherGetLocatorTypeStr[]				= "GopherGetLocatorTypeA";
static const char GopherGetAttributeStr[]					= "GopherGetAttributeA";
static const char HttpOpenRequestStr[]						= "HttpOpenRequestA";
static const char HttpAddRequestHeadersStr[]			= "HttpAddRequestHeadersA";
static const char HttpSendRequestStr[]						= "HttpSendRequestA";
static const char HttpQueryInfoStr[]							= "HttpQueryInfoA";
static const char InternetGetCookieStr[]					= "InternetGetCookieA";
static const char InternetSetCookieStr[]					= "InternetSetCookieA";
#endif


static const _TCHAR szURLftp[] = _T("ftp://");
static const _TCHAR szURLgopher[] = _T("gopher://");
static const _TCHAR szURLhttp[] = _T("http://");
static const _TCHAR szURLfile[] = _T("file://");
static const _TCHAR dirSeparatorStr[] = _T("/");

//////////////////////
//
// Returns TModule object wrapping the handle of the SHELL[32].DLL module
//
TModule&
TWinInetDll::GetModule()
{
	static TModule winInetModule(winInetStr, true, true, false);
	return winInetModule;
}
//
HINTERNET TWinInetDll::InternetOpen(LPCTSTR p1, DWORD p2, LPCTSTR p3, LPCSTR p4, DWORD p5)
{
	static TModuleProc5<HINTERNET,LPCTSTR,DWORD,LPCTSTR,LPCSTR,DWORD>
		internetOpen(GetModule(), InternetOpenStr);
	return internetOpen(p1, p2, p3, p4, p5);
}
//
HINTERNET TWinInetDll::InternetConnect(HINTERNET p1, LPCTSTR p2, INTERNET_PORT p3,
									   LPCTSTR p4, LPCTSTR p5,DWORD p6,DWORD p7,
									   DWORD p8)
{
	static TModuleProc8<HINTERNET,HINTERNET,LPCTSTR,INTERNET_PORT,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD>
		internetConnect(GetModule(), InternetConnectStr);
	return internetConnect(p1, p2, p3, p4, p5,p6,p7, p8);
}
//
BOOL TWinInetDll::InternetCloseHandle(HINTERNET p1)
{
	static TModuleProc1<BOOL,HINTERNET>
		internetCloseHandle(GetModule(), InternetCloseHandleStr);
	return internetCloseHandle(p1);
}
//
BOOL TWinInetDll::InternetQueryOption(HINTERNET p1,DWORD p2, LPVOID p3,LPDWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,DWORD,LPVOID,LPDWORD>
		internetQueryOption(GetModule(), InternetQueryOptionStr);
	return internetQueryOption(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetSetOption(HINTERNET p1,DWORD p2,LPVOID p3,DWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,DWORD,LPVOID,DWORD>
		internetSetOption(GetModule(), InternetSetOptionStr);
	return internetSetOption(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetSetOptionEx(HINTERNET p1,DWORD p2, LPVOID p3,DWORD p4, DWORD p5)
{
	static TModuleProc5<BOOL,HINTERNET,DWORD,LPVOID,DWORD,DWORD>
		internetSetOptionEx(GetModule(), InternetSetOptionExStr);
	return internetSetOptionEx(p1,p2,p3,p4,p5);
}
//
INTERNET_STATUS_CALLBACK TWinInetDll::InternetSetStatusCallback(HINTERNET p1,
																INTERNET_STATUS_CALLBACK p2)
{
	static TModuleProc2<INTERNET_STATUS_CALLBACK,HINTERNET,INTERNET_STATUS_CALLBACK>
		internetSetStatusCallback(GetModule(), InternetSetStatusCallbackStr);
	return internetSetStatusCallback(p1,p2);
}
//
DWORD TWinInetDll::InternetConfirmZoneCrossing(HWND p1, LPSTR p2, LPSTR p3, BOOL p4)
{
	static TModuleProc4<DWORD,HWND,LPSTR,LPSTR,BOOL>
		internetConfirmZoneCrossing(GetModule(), InternetConfirmZoneCrossingStr);
	return internetConfirmZoneCrossing(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetTimeFromSystemTime(CONST SYSTEMTIME* p1, DWORD p2,
											 LPSTR p3, DWORD p4)
{
	static TModuleProc4<BOOL,CONST SYSTEMTIME*,DWORD,LPSTR,DWORD>
		internetTimeFromSystemTime(GetModule(), InternetTimeFromSystemTimeStr);
	return internetTimeFromSystemTime(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetTimeToSystemTime(LPCSTR p1, SYSTEMTIME* p2,
										   DWORD p3)
{
	static TModuleProc3<BOOL,LPCSTR,SYSTEMTIME*,DWORD>
		internetTimeToSystemTime(GetModule(), InternetTimeToSystemTimeStr);
	return internetTimeToSystemTime(p1,p2,p3);
}
//
DWORD TWinInetDll::InternetAttemptConnect(DWORD p1)
{
	static TModuleProc1<DWORD,DWORD>
		internetAttemptConnect(GetModule(), InternetAttemptConnectStr);
	return internetAttemptConnect(p1);
}
//
BOOL TWinInetDll::InternetReadFile(HINTERNET p1,LPVOID p2, DWORD p3, LPDWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,LPVOID,DWORD,LPDWORD>
		internetReadFile(GetModule(), InternetReadFileStr);
	return internetReadFile(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetSetFilePointer(HINTERNET p1, LONG p2,PVOID p3, DWORD p4, DWORD p5)
{
	static TModuleProc5<BOOL,HINTERNET,LONG,PVOID,DWORD,DWORD>
		internetSetFilePointer(GetModule(), InternetSetFilePointerStr);
	return internetSetFilePointer(p1,p2,p3,p4,p5);
}
//
BOOL TWinInetDll::InternetFindNextFile(HINTERNET p1, LPVOID p2)
{
	static TModuleProc2<BOOL,HINTERNET,LPVOID>
		internetFindNextFile(GetModule(), InternetFindNextFileStr);
	return internetFindNextFile(p1,p2);
}
//
BOOL TWinInetDll::InternetQueryDataAvailable(HINTERNET p1, LPDWORD p2, DWORD p3, DWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,LPDWORD,DWORD,DWORD>
		internetQueryDataAvailable(GetModule(), InternetQueryDataAvailableStr);
	return internetQueryDataAvailable(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetGetLastResponseInfo(LPDWORD p1, LPTSTR p2, LPDWORD p3)
{
	static TModuleProc3<BOOL,LPDWORD,LPTSTR,LPDWORD>
		internetGetLastResponseInfo(GetModule(), InternetGetLastResponseInfoStr);
	return internetGetLastResponseInfo(p1,p2,p3);
}
//
BOOL TWinInetDll::InternetWriteFile(HINTERNET p1, LPCVOID p2, DWORD p3, LPDWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,LPCVOID,DWORD,LPDWORD>
		internetWriteFile(GetModule(), InternetWriteFileStr);
	return internetWriteFile(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetCrackUrl(LPCSTR p1, DWORD p2, DWORD p3, LPURL_COMPONENTS p4)
{
	static TModuleProc4<BOOL,LPCSTR,DWORD,DWORD,LPURL_COMPONENTS>
		internetCrackUrl(GetModule(), InternetCrackUrlStr);
	return internetCrackUrl(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetCreateUrl(LPURL_COMPONENTS p1,DWORD p2, LPSTR p3,LPDWORD p4)
{
	static TModuleProc4<BOOL,LPURL_COMPONENTS,DWORD,LPSTR,LPDWORD>
		internetCreateUrl(GetModule(), InternetCreateUrlStr);
	return internetCreateUrl(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetCanonicalizeUrl(LPCTSTR p1, LPTSTR p2, LPDWORD p3, DWORD p4)
{
	static TModuleProc4<BOOL,LPCTSTR,LPTSTR,LPDWORD,DWORD>
		internetCanonicalizeUrl(GetModule(), InternetCanonicalizeUrlStr);
	return internetCanonicalizeUrl(p1,p2,p3,p4);
}
//
BOOL TWinInetDll::InternetCombineUrl(LPCTSTR p1, LPCTSTR p2, LPTSTR p3,LPDWORD p4, DWORD p5)
{
	static TModuleProc5<BOOL,LPCTSTR,LPCTSTR,LPTSTR,LPDWORD,DWORD>
		internetCombineUrl(GetModule(), InternetCombineUrlStr);
	return internetCombineUrl(p1,p2,p3,p4,p5);
}
//
HINTERNET TWinInetDll::InternetOpenUrl(HINTERNET p1,LPCTSTR p2, LPCTSTR p3, DWORD p4, DWORD p5, DWORD p6)
{
	static TModuleProc6<HINTERNET,HINTERNET,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD>
		internetOpenUrl(GetModule(), InternetOpenUrlStr);
	return internetOpenUrl(p1,p2,p3,p4,p5,p6);
}
//
HINTERNET TWinInetDll::FtpFindFirstFile(HINTERNET p1,LPCTSTR p2,LPWIN32_FIND_DATA p3,DWORD p4,DWORD p5)
{
	static TModuleProc5<HINTERNET,HINTERNET,LPCTSTR,LPWIN32_FIND_DATA,DWORD,DWORD>
		ftpFindFirstFile(GetModule(), FtpFindFirstFileStr);
	return ftpFindFirstFile(p1,p2,p3,p4,p5);
}
//
BOOL TWinInetDll::FtpGetFile(HINTERNET p1, LPCSTR p2,LPCSTR p3, BOOL p4, DWORD p5, DWORD p6, DWORD p7)
{
	static TModuleProc7<BOOL,HINTERNET,LPCSTR,LPCSTR,BOOL,DWORD,DWORD,DWORD>
		ftpGetFile(GetModule(), FtpGetFileStr);
	return ftpGetFile(p1,p2,p3,p4,p5,p6,p7);
}
//
BOOL TWinInetDll::FtpPutFile(HINTERNET p1, LPCTSTR p2, LPCTSTR p3, DWORD p4, DWORD p5)
{
	static TModuleProc5<BOOL,HINTERNET,LPCTSTR,LPCTSTR,DWORD,DWORD>
		ftpPutFile(GetModule(), FtpPutFileStr);
	return ftpPutFile(p1,p2,p3,p4,p5);
}
//
BOOL TWinInetDll::FtpDeleteFile(HINTERNET p1, LPCTSTR p2)
{
	static TModuleProc2<BOOL,HINTERNET,LPCTSTR>
		ftpDeleteFile(GetModule(), FtpDeleteFileStr);
	return ftpDeleteFile(p1,p2);
}
//
BOOL TWinInetDll::FtpRenameFile(HINTERNET p1, LPCTSTR p2, LPCTSTR p3)
{
	static TModuleProc3<BOOL,HINTERNET,LPCTSTR,LPCTSTR>
		ftpRenameFile(GetModule(), FtpRenameFileStr);
	return ftpRenameFile(p1,p2,p3);
}
//
HINTERNET TWinInetDll::FtpOpenFile(HINTERNET p1,LPCSTR p2, DWORD p3, DWORD p4, DWORD p5)
{
	static TModuleProc5<HINTERNET,HINTERNET,LPCSTR, DWORD, DWORD,DWORD>
		ftpOpenFile(GetModule(), FtpOpenFileStr);
	return ftpOpenFile(p1,p2,p3,p4,p5);
}
//
BOOL TWinInetDll::FtpCreateDirectory(HINTERNET p1, LPCTSTR p2)
{
	static TModuleProc2<BOOL,HINTERNET,LPCTSTR>
		ftpCreateDirectory(GetModule(), FtpCreateDirectoryStr);
	return ftpCreateDirectory(p1,p2);
}
//
BOOL TWinInetDll::FtpRemoveDirectory(HINTERNET p1, LPCTSTR p2)
{
	static TModuleProc2<BOOL,HINTERNET,LPCTSTR>
		ftpRemoveDirectory(GetModule(), FtpRemoveDirectoryStr);
	return ftpRemoveDirectory(p1,p2);
}
//
BOOL TWinInetDll::FtpSetCurrentDirectory(HINTERNET p1, LPCTSTR p2)
{
	static TModuleProc2<BOOL,HINTERNET,LPCTSTR>
		ftpSetCurrentDirectory(GetModule(), FtpSetCurrentDirectoryStr);
	return ftpSetCurrentDirectory(p1,p2);
}
//
BOOL TWinInetDll::FtpGetCurrentDirectory(HINTERNET p1,LPCTSTR p2, LPDWORD p3)
{
	static TModuleProc3<BOOL,HINTERNET,LPCTSTR,LPDWORD>
		ftpGetCurrentDirectory(GetModule(), FtpGetCurrentDirectoryStr);
	return ftpGetCurrentDirectory(p1,p2,p3);
}
// Gopher Functions
HINTERNET TWinInetDll::GopherFindFirstFile(HINTERNET p1,LPCTSTR p2,LPCTSTR p3,
										   LPGOPHER_FIND_DATA p4, DWORD p5, DWORD p6)
{
	static TModuleProc6<HINTERNET,HINTERNET,LPCTSTR,LPCTSTR,LPGOPHER_FIND_DATA,DWORD,DWORD>
		gopherFindFirstFile(GetModule(), GopherFindFirstFileStr);
	return gopherFindFirstFile(p1,p2,p3,p4,p5,p6);
}
//
HINTERNET TWinInetDll::GopherOpenFile(HINTERNET p1, LPCTSTR p2, LPCTSTR p3, 
									  DWORD p4, DWORD p5)
{
	static TModuleProc5<HINTERNET,HINTERNET,LPCTSTR,LPCTSTR,DWORD,DWORD>
		gopherOpenFile(GetModule(), GopherOpenFileStr);
	return gopherOpenFile(p1,p2,p3,p4,p5);
}
//
BOOL TWinInetDll::GopherCreateLocator(LPCTSTR p1, INTERNET_PORT p2,LPCTSTR p3,
									  LPCTSTR p4,DWORD p5,LPCTSTR p6,LPDWORD p7)
{
	static TModuleProc7<BOOL,LPCTSTR,INTERNET_PORT,LPCTSTR,LPCTSTR,DWORD,LPCTSTR,LPDWORD>
		gopherCreateLocator(GetModule(), GopherCreateLocatorStr);
	return gopherCreateLocator(p1,p2,p3,p4,p5,p6,p7);
}
//
BOOL TWinInetDll::GopherGetLocatorType(LPCTSTR p1, LPDWORD p2)
{
	static TModuleProc2<BOOL,LPCTSTR,LPDWORD>
		gopherGetLocatorType(GetModule(), GopherGetLocatorTypeStr);
	return gopherGetLocatorType(p1, p2);
}
//
BOOL TWinInetDll::GopherGetAttribute(HINTERNET p1, LPCTSTR p2,LPCTSTR p3, LPBYTE p4,
									 DWORD p5,LPDWORD p6,GOPHER_ATTRIBUTE_ENUMERATOR p7,
									 DWORD p8)
{
	static TModuleProc8<BOOL,HINTERNET, LPCTSTR,LPCTSTR,LPBYTE,DWORD,LPDWORD,GOPHER_ATTRIBUTE_ENUMERATOR,DWORD>
		gopherGetAttribute(GetModule(), GopherGetAttributeStr);
	return gopherGetAttribute(p1,p2,p3,p4,p5,p6,p7,p8);
}
// HTTP (World Wide Web) Functions
HINTERNET TWinInetDll::HttpOpenRequest(HINTERNET p1, LPCTSTR p2,LPCTSTR p3, LPCTSTR p4,
									   LPCTSTR p5, LPCTSTR * p6, DWORD p7, DWORD p8)
{
	static TModuleProc8<HINTERNET,HINTERNET,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR *,DWORD,DWORD>
		httpOpenRequest(GetModule(), HttpOpenRequestStr);
	return httpOpenRequest(p1,p2,p3,p4,p5,p6,p7,p8);
}
//
BOOL TWinInetDll::HttpAddRequestHeaders(HINTERNET p1, LPCTSTR p2, DWORD p3, DWORD p4)
{
	static TModuleProc4<BOOL,HINTERNET,LPCTSTR,DWORD,DWORD>
		httpAddRequestHeaders(GetModule(), HttpAddRequestHeadersStr);
	return httpAddRequestHeaders(p1, p2, p3, p4);
}
//
BOOL TWinInetDll::HttpSendRequest(HINTERNET p1, LPCTSTR p2, DWORD p3, LPVOID p4, DWORD p5)
{
	static TModuleProc5<BOOL,HINTERNET,LPCTSTR,DWORD,LPVOID,DWORD>
		httpSendRequest(GetModule(), HttpSendRequestStr);
	return httpSendRequest(p1, p2, p3, p4,p5);
}
//
BOOL TWinInetDll::HttpQueryInfo(HINTERNET p1, DWORD p2, LPVOID p3, LPDWORD p4, LPDWORD p5)
{
	static TModuleProc5<BOOL,HINTERNET,DWORD,LPVOID,LPDWORD,LPDWORD>
		httpQueryInfo(GetModule(), HttpQueryInfoStr);
	return httpQueryInfo(p1, p2, p3, p4,p5);
}
//
DWORD TWinInetDll::InternetErrorDlg(HWND p1, HINTERNET p2, DWORD p3, DWORD p4, LPVOID* p5)
{
	static TModuleProc5<DWORD,HWND,HINTERNET,DWORD,DWORD,LPVOID*>
		internetErrorDlg(GetModule(), InternetErrorDlgStr);
	return internetErrorDlg(p1, p2, p3, p4,p5);
}
//Cookie Functions
BOOL TWinInetDll::InternetGetCookie(LPCSTR p1, LPCSTR p2, LPSTR p3, LPDWORD p4)
{
	static TModuleProc4<BOOL,LPCSTR,LPCSTR,LPSTR,LPDWORD>
		internetGetCookie(GetModule(), InternetGetCookieStr);
	return internetGetCookie(p1, p2, p3, p4);
}
//
BOOL TWinInetDll::InternetSetCookie(LPCSTR p1, LPCSTR p2, LPCSTR p3)
{
	static TModuleProc3<BOOL,LPCSTR,LPCSTR,LPCSTR>
		internetSetCookie(GetModule(), InternetSetCookieStr);
	return internetSetCookie(p1, p2, p3);
}
//
bool       
TWinInet::IsAvailable()
{
	return TWinInetDll::GetModule().IsLoaded();
}
//
TWinInetDll*  
TWinInet::Dll()
{
	static TWinInetDll winInetDLL;
	return &winInetDLL;
}
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// class TXInternet
// ~~~~~ ~~~~~~~~~~
//
//
static uint32 error;
// trick to set last error
static TModule* __setLError(TModule* module)
{
	error = ::GetLastError();
	return module;
}
//
TXInternet::TXInternet(uint resId, TModule* module)
:
TXOwl(resId, __setLError(module))
{
	LastError = error;
}
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXInternet*
#endif
TXInternet::Clone()
{
	TXInternet* xinet = new TXInternet(*this);
	xinet->LastError = LastError;
	return xinet;
}
//
void
TXInternet::Throw()
{
	THROW( *this );
}
// Construct a TXInternet exception from scratch, and throw it.
//
void
TXInternet::Raise(uint resId, TModule* module)
{
	TXInternet(resId,module).Throw();
}
//
bool TXInternet::GetErrorMessage(LPTSTR pstrError, uint nMaxError)
{
	PRECONDITION(pstrError);

	LPTSTR lpBuffer;
	bool bRet = true;

	if (!TWinInet::IsAvailable() ||
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
		TWinInet::Dll()->GetModule(), LastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTSTR)&lpBuffer, 0, NULL) == 0)
	{
		// it failed! try Windows...
		bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, LastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL);
	}

	if (!bRet)
		*pstrError = '\0';
	else{
		if(LastError == ERROR_INTERNET_EXTENDED_ERROR){
			LPTSTR lpExtended;
			DWORD dwLength = 0;
			DWORD dwError;

			// find the length of the error
			if(!TWinInet::Dll()->InternetGetLastResponseInfo(&dwError, NULL, &dwLength) &&
				::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				lpExtended = (LPTSTR)LocalAlloc(LPTR, dwLength);
				TWinInet::Dll()->InternetGetLastResponseInfo(&dwError, lpExtended, &dwLength);
				lstrcpyn(pstrError, lpExtended, nMaxError);
				pstrError += dwLength;
				nMaxError -= dwLength;
				if (nMaxError < 0)
					nMaxError = 0;
				LocalFree(lpExtended);
			}
			else{
				TRACE("Warning: Extended error reported with no response info\n");
			}
			bRet = true;
		}
		else{
			lstrcpyn(pstrError, lpBuffer, nMaxError);
			bRet = true;
		}
		LocalFree(lpBuffer);
	}
	return bRet;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// class TInternetHandle
// ~~~~~ ~~~~~~~~~~~~~~~
//
TInternetHandle::TInternetHandle()
:
Handle(0)
{
}
//------------------------------------------------------------------------------
TInternetHandle::~TInternetHandle()
{
	Close();
}
//------------------------------------------------------------------------------
bool
TInternetHandle::Close()
{
	PRECONDITION(this);
	bool retval = true;
	if(Handle && TWinInet::IsAvailable())
		retval = TWinInet::Dll()->InternetCloseHandle(Handle);
	Handle = 0;
	return retval;
}
//------------------------------------------------------------------------------
bool
TInternetHandle::QueryOption(uint32 option,void* buffer,uint32& size)
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetQueryOption(Handle,option,buffer, &size);
	return false;
}
//------------------------------------------------------------------------------
bool
TInternetHandle::SetOption(uint32 option, void* buffer, uint32 size)
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetSetOption(Handle, option, buffer, size);
	return false;
}
//------------------------------------------------------------------------------
bool
TInternetHandle::SetOption(uint32 option, void* buffer, uint32 size, uint32 flags)
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetSetOptionEx(Handle, option, buffer, size, flags);
	return false;
}
//------------------------------------------------------------------------------
TInternetHandle::TCallBack
TInternetHandle::SetStatusCallback(TCallBack callback)
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetSetStatusCallback(Handle, callback);
	return 0;
}
//
uint TInternetHandle::GetHandleType(HINTERNET hQuery)
{
	DWORD dwServiceType;
	DWORD dwTypeLen = sizeof(dwServiceType);
	if (hQuery == NULL || 
		!TWinInet::IsAvailable() || 
		!TWinInet::Dll()->InternetQueryOption(hQuery, INTERNET_OPTION_HANDLE_TYPE,
		&dwServiceType, &dwTypeLen))
		return isServiceUnknown;
	else
		return dwServiceType;
}
////////////////////////////////////////////////////////////////////////////////
//
// class TInternetSession
// ~~~~~ ~~~~~~~~~~~~~~~~
//
TInternetSession::TInternetSession(	LPCTSTR agent, TOpenContext context,
								   LPCTSTR proxyName, LPCSTR proxyBypass, uint32 flags)
{
	if(!Open(agent,context,proxyName,proxyBypass,flags))
		TXInternet::Raise(0);
}
//
TInternetSession::~TInternetSession()
{
}
//
bool TInternetSession::Open(LPCTSTR agent, TOpenContext context,
							LPCTSTR proxyName, LPCSTR proxyBypass,
							uint32 flags)
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		Handle = TWinInet::Dll()->InternetOpen(agent,context, proxyName, proxyBypass, flags);
	return Handle != 0;
}
//
bool TInternetSession::SetCookie(LPCSTR urlName, LPCTSTR cookieName, 
								 LPCTSTR cookieData)
{
	PRECONDITION(urlName);
	PRECONDITION(cookieData);
	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetSetCookie(urlName, 
		cookieName,
		cookieData)==TRUE;
	return false;
}
//
bool TInternetSession::GetCookie(LPCSTR urlName, LPCSTR cookieName, 
								 LPSTR cookieData, uint32& size)
{
	PRECONDITION(urlName);
	PRECONDITION(cookieName);
	PRECONDITION(cookieData);

	if(TWinInet::IsAvailable())
		return TWinInet::Dll()->InternetGetCookie(urlName, 
		cookieName,cookieData, &size)==TRUE;
	return false;
}
//
uint32 TInternetSession::GetCookieLength(LPCSTR urlName, LPCTSTR cookieName)
{
	PRECONDITION(urlName);
	PRECONDITION(cookieName);

	if(TWinInet::IsAvailable()){
		uint32 size;
		if(TWinInet::Dll()->InternetGetCookie(urlName, cookieName,NULL,&size))
			return size;
	}
	return 0;
}
//
TBufferedFile*
TInternetSession::OpenURL(LPCTSTR url, uint32 flags, 
						  LPCTSTR headers, uint32 length, 
						  uint32 context)
{
	if(!TWinInet::IsAvailable())
		return 0;

	PRECONDITION(url);
	PRECONDITION(length == 0 || headers != NULL);
	// must have TRANSFER_BINARY or TRANSFER_ASCII but not both
#define _OWL_TRANSFER_MASK (INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_TRANSFER_ASCII)
	PRECONDITION((flags & _OWL_TRANSFER_MASK) != 0);
	PRECONDITION((flags & _OWL_TRANSFER_MASK) != _OWL_TRANSFER_MASK);

	TInternetService serviceType;
	owl_string strServer;
	owl_string fileName;
	INTERNET_PORT nPort;
	TBufferedFile* file = 0;

	bool bParsed = TInternetConnection::ParseURL(url, serviceType, 
		strServer, fileName, nPort);

	// if it turns out to be a file...
	if (bParsed && serviceType == isServiceFile){
		uint nMode = TFile::ReadOnly/* | TFile::Compat*/;
		if ((flags & INTERNET_FLAG_TRANSFER_BINARY)==0)
			nMode |= (uint)TFile::Text;
		file = new TBufferedFile(fileName.c_str(), nMode);
	}
	else{
		HINTERNET hOpener;

		hOpener = TWinInet::Dll()->InternetOpenUrl(Handle, url, headers,
			length, flags, context);
		if (!hOpener)
			TXInternet::Raise(0);

		if(!bParsed)
			serviceType = (TInternetService)TInternetHandle::GetHandleType(hOpener);

		switch (serviceType){
			case INTERNET_HANDLE_TYPE_GOPHER_FILE:
			case isServiceGopher:
				//WINBUG: WININET supplies no way to
				// convert from a URL to a Gopher locator
				file = new TGopherFile(new TInternetFileHandle(hOpener));
				break;

			case INTERNET_HANDLE_TYPE_FTP_FILE:
			case isServiceFtp:
				file = new TFtpFile(new TInternetFileHandle(hOpener,fileName.c_str(),
					TFtpFile::ReadOnly));
				break;

			case INTERNET_HANDLE_TYPE_HTTP_REQUEST:
			case isServiceHttp:
			case isServiceHttps:
				file = new THttpFile(new TInternetFileHandle(hOpener, fileName.c_str()));
				break;

			default:
				TRACE("Error: Unidentified service type:" << hex << serviceType << dec);
		}
	}
	return file;
}
///////////////////////////////////////////
//
// class TInternetConnection
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
TInternetConnection::TInternetConnection(TInternetSession& session)
:
Session(session),
Server(_T(""))
{
}
//
TInternetConnection::TInternetConnection(TInternetSession& session, LPCTSTR server,
										 INTERNET_PORT port, LPCTSTR user,
										 LPCTSTR pasword, TInternetService service,
										 uint32 flags, uint32 context)
										 :
Session(session)
{
	PRECONDITION(server);
	Server = server;
	if(!Open(server,port,user,pasword,service,flags,context))
		TXInternet::Raise(0);
}
//
bool TInternetConnection::Open(LPCTSTR server, INTERNET_PORT port, LPCTSTR user,
							   LPCTSTR password, TInternetService service, uint32 flags,
							   uint32 context)
{
	PRECONDITION(this);
	PRECONDITION(server);
	PRECONDITION((flags & INTERNET_FLAG_ASYNC) == 0);

	Server = server;
	if(TWinInet::IsAvailable())
		Handle = TWinInet::Dll()->InternetConnect(Session, server, port, user,
		password, service==isServiceUnknown?0:service,
		flags, context);
	return Handle != 0;
}
//
bool TInternetConnection::ParseURL(LPCTSTR wild_name, TInternetService& serviceType, 
								   owl_string& server, owl_string& strObj, 
								   INTERNET_PORT& nPort, uint32 flags)
{
	PRECONDITION(wild_name);
	PRECONDITION((flags & INTERNET_FLAG_ASYNC) == 0);

	if(!TWinInet::IsAvailable())
		return false;
	serviceType = isServiceUnknown;

	if (!wild_name)
		return false;

	// 1) canonicalize URL
	LPTSTR pstrCanonicalizedURL;
	TAPointer<_TCHAR> __ClnObj;
	TTmpBuffer<_TCHAR> szCanonicalizedURL(INTERNET_MAX_URL_LENGTH+1);

	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	DWORD dwCanonicalizeFlags = flags & (ICU_NO_ENCODE|
		ICU_DECODE|ICU_NO_META| ICU_ENCODE_SPACES_ONLY|ICU_BROWSER_MODE);

	BOOL bRetVal = TWinInet::Dll()->InternetCanonicalizeUrl(wild_name, 
		szCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);
	if(!bRetVal){
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return false;

		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		__ClnObj = pstrCanonicalizedURL;
		bRetVal = TWinInet::Dll()->InternetCanonicalizeUrl(wild_name, 
			pstrCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);
		if(!bRetVal)
			return false;
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	DWORD dwCrackFlags = flags & (ICU_ESCAPE|ICU_USERNAME);
	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);


	TTmpBuffer<_TCHAR> buffer1(INTERNET_MAX_URL_LENGTH+1);
	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = (LPTSTR)buffer1;

	TTmpBuffer<_TCHAR> buffer2(INTERNET_MAX_URL_LENGTH+1);
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = (LPTSTR)buffer2;

	bRetVal = TWinInet::Dll()->InternetCrackUrl(pstrCanonicalizedURL, 0,
		dwCrackFlags, &urlComponents);

	// convert to MFC-style service ID

	if (bRetVal){ // serviceType
		nPort		= urlComponents.nPort;
		server	= urlComponents.lpszHostName; 
		strObj	= urlComponents.lpszUrlPath;
		serviceType = isServiceUnknown;

		switch (urlComponents.nScheme){
			case INTERNET_SCHEME_FTP:
				serviceType = isServiceFtp;
				break;

			case INTERNET_SCHEME_GOPHER:
				serviceType = isServiceGopher;
				break;

			case INTERNET_SCHEME_HTTP:
				serviceType = isServiceHttp;
				break;

			case INTERNET_SCHEME_HTTPS:
				serviceType = isServiceHttps;
				break;

			case INTERNET_SCHEME_FILE:
				serviceType = isServiceFile;
				break;

			case INTERNET_SCHEME_NEWS:
				serviceType = isServiceNntp;
				break;

			case INTERNET_SCHEME_MAILTO:
				serviceType = isServiceMailto;
				break;
		}
	}
	return bRetVal==TRUE;
}
//
////////////////////////////////////////////////////////////////////////////////
//
// class TInternetFileHandle
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
TInternetFileHandle::TInternetFileHandle(HINTERNET hHandle, LPCTSTR name, uint mode)
:
TInternetHandle(hHandle),
OpenMode(mode)
{
	Name = name ? name : _T("");
}

TInternetFileHandle::~TInternetFileHandle()
{
	if(Handle)
		Close();
}
//
const owl_string 
TInternetFileHandle::GetName()
{
	return Name;
}
//
uint32 
TInternetFileHandle::LastError()
{
	return ::GetLastError();
}
//
bool
TInternetFileHandle::Close()
{
	PRECONDITION(this);
	return TInternetHandle::Close();
}
//
uint32
TInternetFileHandle::Read(void * buffer, uint32 numBytes)
{
	PRECONDITION(this);
	PRECONDITION(Handle);
	PRECONDITION((OpenMode&TFile::ReadOnly)!=0);

	DWORD dwBytes;

	if(TWinInet::IsAvailable()){
		if(TWinInet::Dll()->InternetReadFile(Handle, buffer, numBytes, &dwBytes))
			return dwBytes;
	}
	return TFILE_ERROR;
}
//
bool
TInternetFileHandle::Write(const void * buffer, uint32 numBytes)
{
	PRECONDITION(this);
	PRECONDITION(Handle);
	PRECONDITION((OpenMode&TFile::WriteOnly)!=0);

	DWORD dwBytes;
	if(TWinInet::IsAvailable()){
		if(TWinInet::Dll()->InternetWriteFile(Handle, buffer, numBytes, &dwBytes) &&
			dwBytes == numBytes)
			return true;
	}
	return false;
}
//
uint64
TInternetFileHandle::Position64() const
{
	PRECONDITION(this);
	uint32 retval = Position();
	return retval == TFILE_ERROR ? TFILE64_ERROR : uint64(retval);
}
//
uint64
TInternetFileHandle::Length64() const
{
	PRECONDITION(this);
	uint32 retval = Length();
	return retval == TFILE_ERROR ? TFILE64_ERROR : uint64(retval);
}
//
uint32
TInternetFileHandle::Position() const
{
	PRECONDITION(this);
	return ((TInternetFileHandle*)this)->Seek(0l, TFile::cur);
}
//
uint32
TInternetFileHandle::Length() const
{
	PRECONDITION(this);
	PRECONDITION(Handle);

	DWORD dwRet = 0;
	if (Handle && TWinInet::IsAvailable()){
		if(!TWinInet::Dll()->InternetQueryDataAvailable(Handle, &dwRet, 0, 0))
			dwRet = 0;
	}
	return dwRet;
}
//
uint64
TInternetFileHandle::Seek(int64 offset, TFile::seek_dir origin)
{
	PRECONDITION(this);
	uint32 retval = Seek(long(offset), origin);
	return retval == TFILE_ERROR ? TFILE64_ERROR : uint64(retval);
}
//
uint32
TInternetFileHandle::Seek(long offset, TFile::seek_dir origin)
{
	PRECONDITION(this);
	PRECONDITION(Handle);

	// can't do this on a file for writing
	// can't do this on a file that's got a buffer
	/*
	if ((OpenMode&ReadOnly)==0)
	AfxThrowInternetException(m_dwContext, ERROR_INVALID_HANDLE);
	*/
	uint32 from[] = {FILE_BEGIN ,FILE_CURRENT, FILE_END };
	if(TWinInet::IsAvailable())
		return uint32(TWinInet::Dll()->InternetSetFilePointer(Handle, offset, NULL,
		from[origin], 0));
	return TFILE_ERROR;
}
//
bool    
TInternetFileHandle::GetStatus(TFileStatus  & status) const
{
	TXNotSupportedCall::Raise(); //??????????????????????????????????????????????????
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// class TInternetFile
// ~~~~~ ~~~~~~~~~~~~~
//
TInternetFile::TInternetFile(TInternetFileHandle* handle)
:
TBufferedFile()
{
	Handle = handle;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// class TFtpConnection
// ~~~~~ ~~~~~~~~~~~~~~
//
TFtpConnection::TFtpConnection(TInternetSession& session, LPCTSTR server, 
							   LPCTSTR user, LPCTSTR pasword, uint32 context,
							   INTERNET_PORT port, bool passive)
							   :
TInternetConnection(session,server,port,user,pasword, isServiceFtp,
					passive ? INTERNET_FLAG_PASSIVE : 0, context)
{
}
//
bool TFtpConnection::Open(LPCTSTR server, INTERNET_PORT port, LPCTSTR user,
						  LPCTSTR pasword, bool passive, uint32 context)
{
	PRECONDITION(this);
	return TInternetConnection::Open(server, port, user,pasword,
		isServiceFtp, passive ? INTERNET_FLAG_PASSIVE : 0,
		context);
}
////////////////////////////////////////////////////////////////////
//
// class TFtpFileName
// ~~~~~ ~~~~~~~~~~~~ 
//
//
TFtpFileName::TFtpFileName(TFtpConnection& connection)
:
Connection(connection)
{
}
//
TFtpFileName::TFtpFileName(TFtpConnection& connection, LPCTSTR path,
						   LPCTSTR file, LPCTSTR ext)
						   :
Connection(connection)
{
	////////////////////
}
//
TFtpFileName::TFtpFileName(TFtpConnection& connection, LPCTSTR freeform)
:
Connection(connection)
{
	////////////
}
//
TFtpFileName::TFtpFileName(TFtpConnection& connection, const owl_string& freeform)
:
Connection(connection)
{
	//////////////
}
//
TFtpFileName::TFtpFileName(const TFtpFileName& src)
:
Connection(src.Connection)
{
	////////////
}
//
TFtpFileName& TFtpFileName::operator =(const TFtpFileName& src)
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return *this;
}
//
TFtpFileName& TFtpFileName::operator =(const owl_string& src)
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return *this;
}
//
TFtpFileName& TFtpFileName::operator =(LPCTSTR src)
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return *this;
}
//
TFtpFileName::TFtpFileName(TFtpConnection& connection, TSpecialType type)
:
Connection(connection)
{
	switch (type) {
	case TempFile:
		break;
	case TempDir: 
		break;
	case CurrentDir: {
		_TCHAR tmp[INTERNET_MAX_PATH_LENGTH];
		uint32 size = INTERNET_MAX_PATH_LENGTH;
		TWinInet::Dll()->FtpGetCurrentDirectory(Connection, tmp, &size);
		_tcscat(tmp, dirSeparatorStr);
		owl_string directory = szURLftp;
		directory += connection.GetServer();
		if (tmp[0] != _T('/'))
			directory += _T('/');
		Parse(tmp);
					 }
					 break;
	case HomeDir: 
		break;
	case BootDir:
		//!Y.B ??????????????
	case MachineDir:
		//!Y.B ??????????????
	case SharedDir:
		//!Y.B ??????????????
		break;
	case SysDir: 
		break;
	case ComputerName: 
		break;
	}
}
//
owl_string TFtpFileName::Canonical(bool forceUNC) const
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return owl_string("");
}
//
owl_string TFtpFileName::ShortName(bool forceUNC) const
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return owl_string("");
}
//
bool TFtpFileName::operator ==(const TFtpFileName& other) const
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return false;
}
//
bool TFtpFileName::Copy(const TFileName& newName, bool failIfExists) const
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return false;
}
// Remove the file or dir associated with this filename.
//
bool TFtpFileName::Remove() const
{
	PRECONDITION(this);
	if(HasParts(File)){
		if(!TWinInet::Dll()->FtpDeleteFile(Connection, GetParts(Path|File|Ext)))
			return false;
	}
	else{
		if(!TWinInet::Dll()->FtpRemoveDirectory(Connection, GetParts(Path)))
			return false;
	}
	return true;
}
//
//
bool TFtpFileName::Move(const TFileName& newName, uint32 /*how*/)
{
	PRECONDITION(this);
	if(GetParts(Device)==newName.GetParts(Device))// if from the same address
		return TWinInet::Dll()->FtpRenameFile(Connection, GetParts(Path|File|Ext), 
		newName.GetParts(Path|File|Ext));
	else	//????????????????????
		return false;
}
//
bool TFtpFileName::ChangeDir() const
{
	PRECONDITION(this);
	return TWinInet::Dll()->FtpSetCurrentDirectory(Connection, Canonical().c_str());
}
//
bool TFtpFileName::CreateDir() const
{
	PRECONDITION(this);
	return TWinInet::Dll()->FtpCreateDirectory(Connection, Canonical().c_str());
}
//
bool TFtpFileName::PutFile(LPCTSTR localFile, uint32 flags, uint32 context)
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return false;
}
//
bool TFtpFileName::GetFile(LPCTSTR localFile, bool failIfExists,uint32 attr,
						   uint32 flags, uint32 context)
{
	PRECONDITION(this);

	TXNotSupportedCall::Raise();

	return false;
}
//
void TFtpFileName::Parse(LPCTSTR /*name*/)
{
	TXNotSupportedCall::Raise();
}

///////////////////////////////
// struct __TFtpFindFileStr
// ~~~~~~ ~~~~~~~~~~~~~~~~~
//
struct __TFtpFindFileStr{
	HINTERNET				Handle;
	WIN32_FIND_DATA Data;
};
//
uint __sys2Atr(uint32 attrib)
{
	uint attr = 0;
	if(attrib & FILE_ATTRIBUTE_ARCHIVE)
		attr	|= TFile::Archive;
	if(attrib & FILE_ATTRIBUTE_DIRECTORY)
		attr	|= TFile::Directory;
	if(attrib & FILE_ATTRIBUTE_HIDDEN)
		attr	|= TFile::Hidden;
	if(attrib & FILE_ATTRIBUTE_NORMAL)
		attr	|= TFile::Normal;
	if(attrib & FILE_ATTRIBUTE_READONLY)
		attr	|= TFile::RdOnly;
	if(attrib & FILE_ATTRIBUTE_SYSTEM)
		attr	|= TFile::System;
	if(attrib & FILE_ATTRIBUTE_TEMPORARY)
		attr	|= TFile::Temporary;
	return attr;
}
//
static void FillFileStr( __TFtpFindFileStr& data, TFileStatus& status)
{
	_tcscpy(status.fullName, data.Data.cFileName);
	status.attribute	= __sys2Atr(data.Data.dwFileAttributes);
	status.createTime = TTime(TFileTime(data.Data.ftCreationTime));
	status.modifyTime = TTime(TFileTime(data.Data.ftLastWriteTime));
	status.accessTime = TTime(TFileTime(data.Data.ftLastAccessTime));
	status.size				= data.Data.nFileSizeLow;
}
//
TFtpFileNameIterator::TFtpFileNameIterator(TFtpConnection& connection, 
										   const owl_string& wildName, 
										   uint32 flags, uint32 context)
										   :
Connection(connection)
{
	Done = false;
	__TFtpFindFileStr* data = new __TFtpFindFileStr;
	Data = data;
	if(TWinInet::IsAvailable()){
		data->Handle = TWinInet::Dll()->FtpFindFirstFile(Connection,
			wildName.c_str(), &data->Data, flags, context);
		if(data->Handle != INVALID_HANDLE_VALUE){
			FillFileStr(*data, Status);
			return;
		}
	}
	Done = true;
}
//
TFtpFileNameIterator::~TFtpFileNameIterator()
{
	PRECONDITION(this);
	if(Data){
		if (((__TFtpFindFileStr*)Data)->Handle != INVALID_HANDLE_VALUE)
			TWinInet::Dll()->InternetCloseHandle(((__TFtpFindFileStr*)Data)->Handle);
		delete (__TFtpFindFileStr*)Data;
		Data = 0;
	}
}
//
bool TFtpFileNameIterator::FindNext(TFileStatus& status, void* findData)
{
	PRECONDITION(this);
	PRECONDITION(findData);
	__TFtpFindFileStr& data = *(__TFtpFindFileStr*)findData;
	if (!TWinInet::Dll()->InternetFindNextFile(data.Handle, &data.Data))
		return false;

	FillFileStr(data, status);
	return true;
}
//
// class TFtpFile
// ~~~~~ ~~~~~~~~
//
//
TFtpFile::TFtpFile(TFtpFileName& fileName, uint32 mode, uint32 flags, uint32 context)
{
	PRECONDITION(this);
	HINTERNET hFile = 
		TWinInet::Dll()->FtpOpenFile(fileName.GetConnection(), fileName.GetParts(
		TFileName::Path|TFileName::File|TFileName::Ext),
		0/*dwAccess*/, flags, context);
	Handle = new TInternetFileHandle(hFile);
}
//
bool TFtpFile::Write(const void* buffer, uint32 numBytes)
{
	PRECONDITION(this);
	PRECONDITION(buffer);
	return TBufferedFile::Write(buffer, numBytes);
}
////////////////////////////////////////////////////////////////////////////////
//
// class TGopherConnection
// ~~~~~ ~~~~~~~~~~~~~~~~~
//
TGopherConnection::TGopherConnection(TInternetSession& session, LPCTSTR server,
									 LPCTSTR user, LPCTSTR pasword,
									 uint32 context, INTERNET_PORT port)
									 :
TInternetConnection(session,server,port,user,pasword, isServiceGopher,
					0, context)
{
}
//
bool TGopherConnection::Open(LPCTSTR server, INTERNET_PORT port, LPCTSTR user,
							 LPCTSTR pasword, uint32 context)
{
	PRECONDITION(this);
	Port		= port;
	return TInternetConnection::Open(server, port, user, pasword,
		isServiceGopher, 0, context);
}
//
// class TGopherFileName
// ~~~~~ ~~~~~~~~~~~~~~~
//
TGopherFileName::TGopherFileName(TGopherConnection& connection, LPCTSTR displayStr, 
								 LPCTSTR selectorStr, uint32 type)
								 :
//Locator(connection, displayString, selectorString, type),
Connection(connection)
{
	if(TWinInet::IsAvailable()){
		TAPointer<_TCHAR> locator = new _TCHAR[MAX_GOPHER_LOCATOR_LENGTH];
		uint32 size = MAX_GOPHER_LOCATOR_LENGTH;
		if(TWinInet::Dll()->GopherCreateLocator(connection.GetServer(),
			connection.GetPort(),displayStr, selectorStr, type, 
			locator, &size)){
				Length  = size;
				Locator = new _TCHAR[size];  // _not_ a zero-terminated string!
				memcpy(Locator,(_TCHAR*)locator, size);
				return;
		}
	}
	TXInternet::Raise(1); //????????????????????????????
}
//
TGopherFileName::TGopherFileName(TGopherFileNameIterator& itr)
:
Connection(itr.Connection)
{
	LPCTSTR locator = itr.GetLocator();
	Length  = _tcslen(locator);
	Locator = new _TCHAR[Length];  // _not_ a zero-terminated string!
	memcpy(Locator,(uint8*)locator,Length);
}
//
TGopherFileName::~TGopherFileName()
{
	delete[] Locator;
}
//
bool TGopherFileName::GetLocatorType(uint32& dwRef) const
{
	PRECONDITION(this);
	if(TWinInet::IsAvailable())
		return ToBool(TWinInet::Dll()->GopherGetLocatorType((LPCTSTR)Locator, &dwRef)); 
	return false;
}
//
bool TGopherFileName::GetAttribute(uint8* buffer, uint32 buffSize, 
								   uint32& retsuze, LPCTSTR atrlName, 
								   GOPHER_ATTRIBUTE_ENUMERATOR attrEnum,
								   uint32 context)
{
	if(TWinInet::IsAvailable())
		return ToBool(TWinInet::Dll()->GopherGetAttribute(Connection, 
		(LPCTSTR)Locator,atrlName, buffer, buffSize, &retsuze, 
		attrEnum, context));
	return false;
}
//
// struct __TGopherFindFileStr
// ~~~~~~ ~~~~~~~~~~~~~~~~~~~~
//
struct __TGopherFindFileStr{
	HINTERNET					Handle;
	GOPHER_FIND_DATA	Data;
};
static void FillFileStr( __TGopherFindFileStr& data, TFileStatus& status)
{
	_tcscpy(status.fullName, data.Data.DisplayString);
	status.attribute	= data.Data.GopherType;
	status.createTime = status.modifyTime = status.accessTime = 
		TTime(TFileTime(data.Data.LastModificationTime));
	status.size				= data.Data.SizeLow;
}
//
// class TGopherFileNameIterator
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~
//
//
TGopherFileNameIterator::TGopherFileNameIterator(TGopherConnection& connection,
												 const owl_string& wildName, uint32 flags, uint32 context)
												 :
Connection(connection)
{
	Init(NULL, wildName.c_str(), flags, context);
}
//
TGopherFileNameIterator::TGopherFileNameIterator(TGopherFileName& name,
												 const owl_string& wildName, uint32 flags, uint32 context)
												 :
Connection(name.Connection)
{
	Init((LPCTSTR)name.Locator, wildName.c_str(), flags, context);
}
//
void TGopherFileNameIterator::Init(LPCTSTR locator, LPCTSTR wildName, 
								   uint32 flags, uint32 context)
{
	Done = false;
	__TGopherFindFileStr* data = new __TGopherFindFileStr;
	Data = data;
	if(TWinInet::IsAvailable()){
		data->Handle = 
			TWinInet::Dll()->GopherFindFirstFile(Connection, locator,
			wildName, &data->Data, flags, context);
		if(data->Handle != NULL){
			FillFileStr(*data, Status);
			return;
		}
	}
	Done = true;
}
//
TGopherFileNameIterator::~TGopherFileNameIterator()
{
	PRECONDITION(this);
	if(Data){
		if (((__TGopherFindFileStr*)Data)->Handle != NULL)
			TWinInet::Dll()->InternetCloseHandle(((__TGopherFindFileStr*)Data)->Handle);
		delete (__TGopherFindFileStr*)Data;
		Data = 0;
	}
}
// all work do here -> must fill TFileStatus structure return true on success
bool TGopherFileNameIterator::FindNext(TFileStatus& status, void* findData)
{
	PRECONDITION(this);
	PRECONDITION(findData);
	__TGopherFindFileStr& data = *(__TGopherFindFileStr*)findData;
	if (!TWinInet::Dll()->InternetFindNextFile(data.Handle, &data.Data))
		return false;
	FillFileStr(data, status);
	return true;
}
//
LPCTSTR TGopherFileNameIterator::GetLocator()
{
	__TGopherFindFileStr& data = *(__TGopherFindFileStr*)Data;
	return data.Data.Locator;
}
//
// class TFtpFile
// ~~~~~ ~~~~~~~~
//
TGopherFile::TGopherFile(TGopherFileName& fileName, LPCTSTR view, 
						 uint32 flags, uint32 context)
{
	PRECONDITION(this);
	HINTERNET hFile = TWinInet::Dll()->GopherOpenFile(fileName.Connection, 
		fileName.Locator, view, flags, context);
	Handle = new TInternetFileHandle(hFile);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                                                            //
////////                                                                ////////
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// class THttpConnection
// ~~~~~ ~~~~~~~~~~~~~~~
//
THttpConnection::THttpConnection(TInternetSession& session, LPCTSTR server,
								 INTERNET_PORT port, LPCTSTR user,
								 LPCTSTR password, uint32 flags, 
								 uint32 context)
								 :
TInternetConnection(session,server,port,user,password, 
					isServiceHttp, flags, context)
{
	// --------???????????????????????????????---------
}
//
bool THttpConnection::Open(LPCTSTR server, INTERNET_PORT port, LPCTSTR user,
						   LPCTSTR pasword, uint32 flags, uint32 context)
{
	PRECONDITION(this);
	return TInternetConnection::Open(server, port, user, pasword,
		isServiceHttp, flags, context);
}
//
// class THttpFile
// ~~~~~ ~~~~~~~~~
//
static const LPCTSTR szHtmlVerbs[] = {
	_T("POST"),
	_T("GET"),
	_T("HEAD"),
	_T("PUT"),
	_T("LINK"),
	_T("DELETE"),
	_T("UNLINK"),
};
//
THttpFile::THttpFile(THttpConnection& connect)
:
Connection(&connect)
{
}
//
THttpFile::THttpFile(THttpConnection& connect, LPCTSTR pstrVerb,
					 LPCTSTR name, LPCTSTR pstrReferer, uint32 context,
					 LPCTSTR* ppstrAcceptTypes, LPCTSTR pstrVersion, 
					 uint32 flags)
					 :
Connection(&connect)
{
	if(!Open(pstrVerb,name, pstrReferer, context, ppstrAcceptTypes, 
		pstrVersion, flags)){
			TXInternet::Raise(0);
	}
}
//
THttpFile::THttpFile(THttpConnection& connect, THttpVerb verb,
					 LPCTSTR name, LPCTSTR pstrReferer, uint32 context,
					 LPCTSTR* ppstrAcceptTypes, LPCTSTR pstrVersion, 
					 uint32 flags)
					 :
Connection(&connect)
{
	if(!Open(verb,name, pstrReferer, context, ppstrAcceptTypes, 
		pstrVersion, flags)){
			TXInternet::Raise(0);
	}
}
//
uint __getHttpOpenMode(LPCTSTR verb)
{
	uint openMode = 0;
	if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbPost])==0)
		openMode = TFile::WriteOnly;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbGet])==0)
		openMode = TFile::ReadOnly;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbHead])==0)
		openMode = TFile::ReadOnly;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbPut])==0)
		openMode = TFile::WriteOnly;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbLink])==0)
		openMode = 0;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbDelete])==0)
		openMode = 0;
	else if(_tcsicmp(verb, szHtmlVerbs[THttpFile::verbUnlink])==0)
		openMode = 0;
	return openMode;
}
//
bool THttpFile::Open(LPCTSTR pstrVerb, LPCTSTR name, LPCTSTR pstrReferer, 
					 uint32 context, LPCTSTR* ppstrAcceptTypes, 
					 LPCTSTR pstrVersion, uint32 flags)
{
	PRECONDITION(this);
	PRECONDITION(!IsOpen());

	if(!pstrVersion)
		pstrVersion = HTTP_VERSION;

	VerbName = pstrVerb ? pstrVerb : _T("");

	if(TWinInet::IsAvailable()){
		HINTERNET hFile = TWinInet::Dll()->HttpOpenRequest(*Connection, 
			pstrVerb, name, pstrVersion, pstrReferer, 
			ppstrAcceptTypes, flags, context);
		if(hFile){
			Handle = new TInternetFileHandle(hFile, name, __getHttpOpenMode(pstrVerb));
			return true;
		}
	}
	return false;
}
//
bool THttpFile::Open(THttpVerb verb,LPCTSTR name, LPCTSTR pstrReferer, 
					 uint32 context, LPCTSTR* ppstrAcceptTypes, 
					 LPCTSTR pstrVersion, uint32 flags)
{
	PRECONDITION(this);
	PRECONDITION(!IsOpen());

	PRECONDITION(verb >= verbMin && verb <= verbMax);

	LPCTSTR verbName = 0;
	if (verb >= verbMin && verb <= verbMax)
		verbName = szHtmlVerbs[verb];

	return Open(verbName, name, pstrReferer, context, 
		ppstrAcceptTypes, pstrVersion, flags);
}
//
uint32 THttpFile::ErrorDlg(TWindow* parent, uint32 error, 
						   uint32 flags, LPVOID*lppvData)
{
	HWND hWnd;
	LPVOID lpEmpty;
	LPVOID* lppvHolder;

	if (lppvData == NULL){
		lpEmpty = NULL;
		lppvHolder = &lpEmpty;
	}
	else
		lppvHolder = lppvData;

	if (!parent || !parent->GetHandle())
		hWnd = ::GetDesktopWindow();
	else
		hWnd = parent->GetHandle();

	TInternetFileHandle* handle = TYPESAFE_DOWNCAST(Handle, TInternetFileHandle);
	if(handle)
		return TWinInet::Dll()->InternetErrorDlg(hWnd, handle->GetHandle(), 
		error, flags, lppvHolder);
	return 0;
}
//
bool THttpFile::AddRequestHeaders(LPCTSTR headers, uint32 flags, int headersLen)
{
	PRECONDITION(this);
	PRECONDITION(headersLen == 0 || headers != 0);
	PRECONDITION(Handle);

	if (headersLen == -1){
		if (!headers)
			headersLen = 0;
		else
			headersLen = _tcslen(headers);
	}
	TInternetFileHandle* handle = TYPESAFE_DOWNCAST(Handle, TInternetFileHandle);
	if(handle)
		return TWinInet::Dll()->HttpAddRequestHeaders(handle->GetHandle(), 
		headers, headersLen, flags)==TRUE;
	return false;
}
//
bool THttpFile::SendRequest(LPCTSTR headers, uint32 headersLen, 
							void _FAR* optional, uint32 optionalLen)
{
	PRECONDITION(this);
	PRECONDITION(Handle);
	PRECONDITION(optionalLen == 0 || optional != NULL);
	PRECONDITION(headersLen == 0 || headers != NULL);

	TInternetFileHandle* handle = TYPESAFE_DOWNCAST(Handle, TInternetFileHandle);
	BOOL bRet = FALSE;
	if(handle)
		bRet = TWinInet::Dll()->HttpSendRequest(handle->GetHandle(), headers, 
		headersLen, optional, optionalLen);
	return bRet==TRUE;
}
//
bool THttpFile::QueryInfo(uint32 level, void _FAR* buffer, 
						  uint32& size, uint32* index) const
{
	PRECONDITION(this);
	PRECONDITION(Handle);
	PRECONDITION((HTTP_QUERY_HEADER_MASK & level) <= HTTP_QUERY_MAX && level != 0);
	PRECONDITION(buffer != 0 && size > 0);

	TInternetFileHandle* handle = TYPESAFE_DOWNCAST(Handle, TInternetFileHandle);
	if(handle)
		return TWinInet::Dll()->HttpQueryInfo(handle->GetHandle(), level, buffer,
		&size, index)==TRUE;
	return false;
}
//
bool THttpFile::QueryInfoStatusCode(uint32& statusCode) const
{
	PRECONDITION(this);
	PRECONDITION(Handle);

	_TCHAR szBuffer[80];
	uint32 size = COUNTOF(szBuffer);

	TInternetFileHandle* handle = TYPESAFE_DOWNCAST(Handle, TInternetFileHandle);
	BOOL bRet = FALSE;
	if(handle)
		bRet = TWinInet::Dll()->HttpQueryInfo(handle->GetHandle(), HTTP_QUERY_STATUS_CODE,
		szBuffer, &size, NULL);

	if (bRet)
		statusCode = (uint32) _ttol(szBuffer);
	return bRet==TRUE;
}
//
_OWLFX_END_NAMESPACE

//===========================================================================