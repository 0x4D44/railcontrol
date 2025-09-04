//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:35 $
//
// Winsock for OWL subsystem.
// Based on work by Paul Pedriana, 70541.3223@compuserve.com
//----------------------------------------------------------------------------

#if !defined(OWL_WSKHOSTM_H)
#define OWL_WSKHOSTM_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_WINSOCK_H)
# include <owl/winsock.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

//
// Forward ref.   
//
class _OWLCLASS TSocketAddress;

#define WINSOCK_NOERROR (int)0
#define WINSOCK_ERROR   (int)SOCKET_ERROR

//
// Supposedly in RFC 883.
//
#define N_MAX_HOST_NAME 128
#define MSG_HOST_INFO_NOTIFY ((UINT)(WM_USER+303))

class _OWLCLASS THostInfoManager;

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class THostInfoWindow
// ~~~~~ ~~~~~~~~~~~~~~~
// A private class created by THostInfoManager to catch WinSock messages.
//
class _OWLCLASS THostInfoWindow : public TWindow {
  public:
    THostInfoWindow(THostInfoManager* hostInfoManagerParent);

    TResult DoNotification(TParam1 param1, TParam2 param2);

  public_data:
    // Object to notify of Winsock events
    //
    THostInfoManager* HostInfoManagerParent;

  DECLARE_RESPONSE_TABLE(THostInfoWindow);
};

//
// class THostEntry
// ~~~~~ ~~~~~~~~~~
// Encapsulates the attributes of a host (hostent).
//
class _OWLCLASS THostEntry : public hostent {
  public:
    THostEntry();

    int   GetAddressCount();
    ulong GetINetAddress();

    // An internet addressing -specific function.
    //
    ulong GetNthINetAddress(int nIndex = 0);
};

//
// class THostInfoManager
// ~~~~~ ~~~~~~~~~~~~~~~~
// Encapsulates hostent database functions.
//
class _OWLCLASS THostInfoManager {
  public:
    THostInfoManager();
    virtual ~THostInfoManager();

    int   GetLastError();
    int   GetHostName(char * name, int nameLength = N_MAX_HOST_NAME);
    int   GetHostAddress(char * szHostAddress, const char * szHostName);
    int   GetHostAddress(TSocketAddress& sAddress, const char * szHostName);
    int   GetHostInfo(THostEntry*& hEntry, const TSocketAddress& sAddress);
    int   GetHostInfo(THostEntry*& hEntry, const char * szName);
    int   GetHostInfoAsync(HANDLE& hTheHostRequest, TSocketAddress& sAddress);
    int   GetHostInfoAsync(HANDLE& hTheHostRequest, char * szName);
    int   GetHostInfoAsync(TWindow& wndNotify, HANDLE& hTheHostRequest,
                           TSocketAddress& sAddress, uint nMessage = MSG_HOST_INFO_NOTIFY,
                           char * chBuffer = 0);
    int   GetHostInfoAsync(TWindow& wndNotify, HANDLE& hTheHostRequest,
                           char * szName, uint nMessage=MSG_HOST_INFO_NOTIFY,
                           char * chBuffer = 0);
    int   CancelHostRequest(HANDLE hTheHostRequest = 0);
    short GetHostRequestCompleted();
    static int HostEntryToAddress(THostEntry* hEntry, char * szAddress);
    static int HostEntryToAddress(THostEntry* hEntry, TSocketAddress& sAddress);

//!CQ public?
    // Set to point to HostInfoBuffer.
    //
  public_data:
    THostEntry*     HostEntry;

  protected:
    bool            HostRequestCompleted;              // Flag if host completed last request
    HANDLE          HostRequest;                       // Handle of host to get info about
    int             LastError;                         // Last error code
    char            HostInfoBuffer[MAXGETHOSTSTRUCT];  // Used for calls to WSAAsync...()
    THostInfoWindow HostWindow;                        // Hidden window to catch notifications

    void  SetHostRequestCompleted(int error);

  friend class THostInfoWindow;
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return the Internet address of the host.
//
inline ulong
THostEntry::GetINetAddress()
{
  return GetNthINetAddress(0);
}

//
// Return the last error code.
//
inline int
THostInfoManager::GetLastError()
{
  return LastError;
}

//
//!TH really return a short?
// Return true if the host complete the last requested transaction.
//
inline short
THostInfoManager::GetHostRequestCompleted()
{
  return HostRequestCompleted;
}

__OWL_END_NAMESPACE


#endif  // OWL_WSKHOSTM_H
