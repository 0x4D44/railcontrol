//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:20 $
//
// Winsock for OWL subsystem.
// Based on work by Paul Pedriana, 70541.3223@compuserve.com
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#if !defined(OWL_WINSOCK_H)
# include <owl/winsock.h>
#endif
#if !defined(OWL_WINSOCK_RH)
# include <owl/winsock.rh>
#endif
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Construct with the error code and the size of the buffer to allocate.
//
TSocketError::TSocketError(int error, unsigned sizeToAllocate)
:
  Error(error),
  SizeToAllocate(sizeToAllocate),
  String(0)
{
  GetErrorString();
}

//
// Destroy the allocated string.
//
TSocketError::~TSocketError()
{
  delete[] String;
}

//
// Copies the error code and string.
//
TSocketError::TSocketError(const TSocketError& src)
:
  Error (src.Error),
  String (strnewdup(src.String)),
  SizeToAllocate (src.SizeToAllocate)
{
}

//
// Copies the error code and string.
//
TSocketError& TSocketError::operator =(const TSocketError& src)
{
  Error = src.Error;
  delete[] String;
  String = strnewdup(src.String);
  SizeToAllocate = src.SizeToAllocate;

  return *this;
}

//
// The important criteria for determining equality is the error value.  The
//  string is unimportant.
//
bool operator ==(const TSocketError& socketError1,
                 const TSocketError& socketError2)
{
  return socketError1.Error == socketError2.Error;
}

//
// Initialize the error code.
//
void TSocketError::Init(int error)
{
  Error = error;
  GetErrorString();
}

//
// Return the error code.
//
int TSocketError::GetReasonValue() const
{
  return Error;
}

//
// This function simply hands the pointer to the string to the caller.
// The caller should not mess with this string, as it doesn't belong to him.
//
const _TCHAR* TSocketError::GetReasonString() const
{
  return String;
}

//
// This function appends the error string to whatever is in the string
// 'stringToAppendErrorTo' and put the result in 'destination'.  You may
// want to put something specific about the error in the string and then use
// AppendError() to add the Winsock error code and description to it.
//
// For example, you could say:
//   MessageBox(TSocketError(WSAENOTCONN).AppendError("Unable to send your mail"),
//                                                   "Error", MB_OK);
// And AppendError() will put "\n\nWinsock Error 10057: Socket is not presently connected"
// after the "Unable to send you mail" string.  Quite convenient.
//
// If destination is valid, destination provides the space for the result.
// Note! It is assumed that desination has enough space.
// If destination is 0, our internal string buffer is used for the result.
// Note! Using the internal string changes the result of subsequent calls to GetReasonString.
// Use Init to reset the error string.
//
_TCHAR* TSocketError::AppendError(const _TCHAR* stringToAppendErrorTo, _TCHAR* destination)
{
  // Create the error string.
  //
  owl_string s = stringToAppendErrorTo;
  s += _T("\n\n");
  s += GetReasonString();

  if (!destination) {
    //
    // Expand the internal string buffer if needed.
    //
    unsigned n = s.length() + 1;
    if (n > SizeToAllocate) {
      delete [] String;
      String = new _TCHAR[SizeToAllocate = n];
      }
    destination = String;
    }

  // Copy the string to the destination.

  _tcscpy(destination, s.c_str());
  return destination;
}

//
// This function is similar to AppendError(const _TCHAR*), but using a string resource.
//
_TCHAR* TSocketError::AppendError(int stringResourceToAppendErrorTo, _TCHAR* destination)
{
  owl_string s = GetModule().LoadString(stringResourceToAppendErrorTo);
  return AppendError (s.c_str(), destination);
}

//
// This function gets a string, suitable for display, based on the error value.
// The previous string is deleted if necessary.
// Note that even though the error strings you see below don't have
// error numbers associated with them, the function pre-pends the error number
// to the string before returning.  If you are writing string resources
// for the error strings, don't put error numbers in the string, since we
// do that for you below.
//
void TSocketError::GetErrorString()
{
  int resIdLookup[] = {
    WSAEINTR,
    WSAEBADF,
    WSAEACCES,
    WSAEFAULT,
    WSAEINVAL,
    WSAEMFILE,
    WSAEWOULDBLOCK,
    WSAEINPROGRESS,
    WSAEALREADY,
    WSAENOTSOCK,
    WSAEDESTADDRREQ,
    WSAEMSGSIZE,
    WSAEPROTOTYPE,
    WSAENOPROTOOPT,
    WSAEPROTONOSUPPORT,
    WSAESOCKTNOSUPPORT,
    WSAEOPNOTSUPP,
    WSAEPFNOSUPPORT,
    WSAEAFNOSUPPORT,
    WSAEADDRINUSE,
    WSAEADDRNOTAVAIL,
    WSAENETDOWN,
    WSAENETUNREACH,
    WSAENETRESET,
    WSAECONNABORTED,
    WSAECONNRESET,
    WSAENOBUFS,
    WSAEISCONN,
    WSAENOTCONN,
    WSAESHUTDOWN,
    WSAETOOMANYREFS,
    WSAETIMEDOUT,
    WSAECONNREFUSED,
    WSAELOOP,
    WSAENAMETOOLONG,
    WSAEHOSTDOWN,
    WSAEHOSTUNREACH,
    WSAENOTEMPTY,
    WSAEPROCLIM,
    WSAEUSERS,
    WSAEDQUOT,
    WSAESTALE,
    WSAEREMOTE,
    WSAEDISCON,
    WSASYSNOTREADY,
    WSAVERNOTSUPPORTED,
    WSANOTINITIALISED,
    WSAHOST_NOT_FOUND,
    WSATRY_AGAIN,
    WSANO_RECOVERY,
    WSANO_DATA,
  };
  unsigned id = 0;
  while (id < COUNTOF(resIdLookup) && resIdLookup[id] != Error)
    id++;

  owl_string s = GetModule().LoadString(IDS_WINSOCK_BASE+id+1);
  unsigned n = s.length() + 26; // Add space for prefix "Winsock Error 123456789: ".
  if (SizeToAllocate < n) SizeToAllocate = n;
  delete[] String;
  String = new _TCHAR[SizeToAllocate];
  String[0] = 0;
  
//Jogy  BC5.02 does not have the safe _sntprintf() function, so we'll have to live with the potential buffer overflow
#if defined(__BORLANDC__) && (__BORLANDC__ <= 0x520)
  _stprintf(String, _T("Winsock Error %d: %s"), Error, (_TCHAR*) s.c_str());
#else
  _sntprintf(String, SizeToAllocate, _T("Winsock Error %d: %s"), Error, (_TCHAR*) s.c_str());
#endif
}

//  
// Return the current module.  Used when loading strings.  
// Important when linking to OWL dynamically because ::LoadString
// would try to load from the EXE, but the WinSock resource 
// strings are in the DLL.
//
TModule& 
TSocketError::GetModule()         // this is a static function
{
  return *gModule;
}

__OWL_END_NAMESPACE

/* ========================================================================== */

