//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:35 $
//
// Main header of the Winsock OWL subsystem.
//----------------------------------------------------------------------------

#if !defined(OWL_WINSOCK_H)
#define OWL_WINSOCK_H

#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#if !defined (_WINSOCKAPI_)
# include <winsock.h>
#endif
#if !defined(OWL_WSKADDR_H)
# include <owl/wskaddr.h>
#endif
#if !defined(OWL_WSKSOCK_H)
# include <owl/wsksock.h>
#endif
#if !defined(OWL_WSKERR_H)
# include <owl/wskerr.h>
#endif
#if !defined(OWL_WSKHOSTM_H)
# include <owl/wskhostm.h>
#endif
#if !defined(OWL_WSKSOCKD_H)
# include <owl/wsksockd.h>
#endif
#if !defined(OWL_WSKSOCKM_H)
# include <owl/wsksockm.h>
#endif
#if !defined(OWL_WSKSERVM_H)
# include <owl/wskservm.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TWinSockDll
// ~~~~~ ~~~~~~~~~~~
// Wrapper for the WinSock dll itself
//
class _OWLCLASS TWinSockDll : public TModule {
  public:
    TWinSockDll();

    // Socket functions
    //
    static SOCKET      accept(SOCKET s, struct sockaddr * addr,
                        int * addrLen);
    static int         bind(SOCKET s, struct sockaddr * addr, int nameLen);
    static int         closesocket(SOCKET s);
    static int         connect(SOCKET s, struct sockaddr * name, int nameLen);
    static int         getpeername(SOCKET s, struct sockaddr * name,
                        int * nameLen);
    static int         getsockname(SOCKET s, struct sockaddr * name,
                        int * nameLen);
    static int         getsockopt(SOCKET s, int level, int optName, char * optVal,
                        int * optLen);
    static ulong       _htonl(ulong hostLong);
    static ushort      _htons(ushort hostShort);
    static ulong       inet_addr(const char * cp);
    static char *   inet_ntoa(struct in_addr in);
    static int         ioctlsocket(SOCKET s, long cmd, ulong * argp);
    static int         listen(SOCKET s, int backlog);
    static ulong       _ntohl(ulong netLong);
    static ushort      _ntohs(ushort netShort);
    static int         recv(SOCKET s, char * buf, int len, int flags);
    static int         recvfrom(SOCKET s, char * buf, int len, int flags,
                        struct sockaddr * from, int* fromLen);
    static int         select(int nfds, struct fd_set * readfds,
                        struct fd_set * writefds,
                        struct fd_set * exceptfds,
                        const struct timeval * timeout);
    static int         send(SOCKET s, const char * buf, int len, int flags);
    static int         sendto(SOCKET s, const char * buf, int len, int flags,
                        const struct sockaddr * to, int toLen);
    static int         setsockopt(SOCKET s, int level, int optName,
                        const char * optVal, int optLen);
    static int         shutdown(SOCKET s, int how);
    static SOCKET      socket(int af, int type, int protocol);

    // Database functions
    //
    static struct hostent  *  gethostbyaddr(const char * addr, int len,
                                  int type);
    static struct hostent  *  gethostbyname(const char * name);
    static int                   gethostname(char * name, int nameLen);
    static struct servent  *  getservbyname(const char * name,
                                  const char * proto);
    static struct servent  *  getservbyport(int port, const char * proto);
    static struct protoent *  getprotobyname(const char * name);
    static struct protoent *  getprotobynumber(int proto);

    // Microsoft Windows Extension functions
    //
    static int     WSAStartup(uint16 versionRequested, LPWSADATA WSAData);
    static int     WSACleanup(void);
    static void    WSASetLastError(int error);
    static int     WSAGetLastError(void);
    static BOOL    WSAIsBlocking(void);
    static int     WSAUnhookBlockingHook(void);
    static FARPROC WSASetBlockingHook(FARPROC blockFunc);
    static int     WSACancelBlockingCall(void);
    static HANDLE  WSAAsyncGetServByName(HWND hWnd, uint msg,
                    const char * name, const char * proto, char * buf,
                    int bufLen);
    static HANDLE  WSAAsyncGetServByPort(HWND hWnd, uint msg, int port,
                    const char * proto, char * buf, int bufLen);
    static HANDLE  WSAAsyncGetProtoByName(HWND hWnd, uint msg,
                    const char * name, char * buf, int bufLen);
    static HANDLE  WSAAsyncGetProtoByNumber(HWND hWnd, uint msg, int number,
                    char * buf, int bufLen);
    static HANDLE  WSAAsyncGetHostByName(HWND hWnd, uint msg,
                    const char * name, char * buf, int bufLen);
    static HANDLE  WSAAsyncGetHostByAddr(HWND hWnd, uint msg,
                    const char * addr, int len, int type, char * buf,
                    int bufLen);
    static int     WSACancelAsyncRequest(HANDLE hTaskHandle);
    static int     WSAAsyncSelect(SOCKET s, HWND hWnd, uint msg, long event);
    static int     WSARecvEx(SOCKET s, char * buf, int len, int * flags);

    // Check for presence of Winsock DLL
    //
    static TModule& WinSockModule();
    static bool IsAvailable();
};

//
// class TWinSock
// ~~~~~ ~~~~~~~~
// Loader for the WinSock dll
//

class _OWLCLASS TWinSock
{
  public:
    static bool         IsAvailable(void);
    static TWinSockDll* Dll(void);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_WSKDLL_H
