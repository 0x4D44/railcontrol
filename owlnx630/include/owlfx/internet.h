//----------------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//
// Definition of internet
//----------------------------------------------------------------------------
#if !defined(OWLFX_INTERNET_H)
#define OWLFX_INTERNET_H

#if !defined(OWLFX_DEFS_H)
#include <owlfx/defs.h>
#endif

#if !defined(BI_PLAT_WIN32)
# error OWLFX: internet.h only supports WIN32 targets
#endif

#if !defined(OWL_FILE_H)
# include <owl/file.h>
#endif

#if !defined(OWL_FILENAME_H)
# include <owl/filename.h>
#endif

#ifndef _WININET_
#if defined(BI_NEED_WININET_H)
#include <api_upd/wininet.h>
#else
#include <wininet.h>
#endif
#endif


_OWLFX_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


//////////////////////////////////////////////////////////////////////////////////
//
// Common Internet Base classes, and types
//

//
// enum TInternetService
// ~~~~ ~~~~~~~~~~~~~~~~
//
//
enum TInternetService {
	isServiceUnknown=0x1000,
	isServiceFtp=INTERNET_SERVICE_FTP,
	isServiceHttp=INTERNET_SERVICE_HTTP,
	isServiceGopher=INTERNET_SERVICE_GOPHER,
	isServiceFile=isServiceUnknown+1,
	isServiceMailto=isServiceUnknown+2,
	isServiceMid=isServiceUnknown+3,
	isServiceCid=isServiceUnknown+4,
	isServiceNews=isServiceUnknown+5,
	isServiceNntp=isServiceUnknown+6,
	isServiceProspero=isServiceUnknown+7,
	isServiceTelnet=isServiceUnknown+8,
	isServiceWais=isServiceUnknown+9,
	isServiceAfs=isServiceUnknown+10,
	isServiceHttps=isServiceUnknown+11,
};

//
// class TInternetHandle
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TInternetHandle {
	public:
  	typedef HINTERNET THandle;
		typedef INTERNET_STATUS_CALLBACK TCallBack;
	public:

		TInternetHandle();

		virtual ~TInternetHandle();

		operator THandle()  	{ return Handle; }
		THandle  GetHandle() 	{ return Handle; }

    bool IsOpen()         { return Handle!=0;}
    bool Close();
		bool QueryOption(uint32 option, void* buffer, uint32& size);
		bool SetOption(uint32 option, void* buffer, uint32 size);
		bool SetOption(uint32 option, void* buffer, uint32 size, uint32 flags);

		TCallBack SetStatusCallback(TCallBack callback);

		static uint GetHandleType(HINTERNET hdnl);

	protected:
		TInternetHandle(HINTERNET handle):Handle(handle){}

		THandle			Handle;
};

//
// class TInternetSession
// ~~~~~ ~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TInternetSession : public TInternetHandle{
	public:
		enum TOpenContext{
					OpenDirect		=	INTERNET_OPEN_TYPE_DIRECT,
					OpenProxy			=	INTERNET_OPEN_TYPE_PROXY,
					OpenPreconfig = INTERNET_OPEN_TYPE_PRECONFIG,
			};

		TInternetSession(){}
		TInternetSession(	LPCTSTR agent,
											TOpenContext = OpenDirect,
											LPCTSTR proxyName = 0,
											LPCSTR proxyBypass=0,
											uint32 flags = 0);
		~TInternetSession();

		bool Open(LPCTSTR agent = 0, TOpenContext = OpenDirect,
              LPCTSTR proxyName = 0, LPCSTR proxyBypass=0,
              uint32 flags = 0);

		__OWL TBufferedFile* OpenURL(LPCTSTR url,
			                     uint32 flags = INTERNET_FLAG_TRANSFER_BINARY,
		                       LPCTSTR headers = NULL, uint32 length = 0,
													 uint32 context = 0);

		static bool SetCookie(LPCSTR urlName, LPCTSTR cookieName, LPCTSTR cookieData);
		static bool GetCookie(LPCSTR urlName, LPCSTR cookieName,
			                    LPSTR cookieData, uint32& size);
		static uint32 GetCookieLength(LPCSTR urlName, LPCTSTR cookieName);
};

//
// class TInternetConnection
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TInternetConnection : public TInternetHandle {
	protected:
  	TInternetConnection(TInternetSession& session);
    TInternetConnection(TInternetSession& session, LPCTSTR server,
                        INTERNET_PORT port=INTERNET_INVALID_PORT_NUMBER,
                        LPCTSTR user=NULL, LPCTSTR pasword =NULL,
												TInternetService service=isServiceUnknown,
                        uint32 flags=0, uint32 context=0);
    bool Open(LPCTSTR server, INTERNET_PORT port=INTERNET_INVALID_PORT_NUMBER,
              LPCTSTR user=0, LPCTSTR pasword = 0,
							TInternetService service=isServiceUnknown,
              uint32 flags=0, uint32 context=0);

	public:
    TInternetSession& GetSession()	{ return Session; }
    LPCTSTR						GetServer()		{ return Server.c_str(); }

		static bool ParseURL(LPCTSTR wild_str, TInternetService& serviceType,
			              owl_string& serverName, owl_string& object,
										INTERNET_PORT& port, uint32 flags = ICU_BROWSER_MODE);

	protected:
		TInternetSession& Session;
		owl_string				Server;
};

//
// class TInternetFileHandle
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TInternetFileHandle :
                        public TInternetHandle,
                        public __OWL TFileHandle      {
  public:
  	TInternetFileHandle(HINTERNET hHandle, LPCTSTR name=0, uint mode = 0);
  	~TInternetFileHandle();
  public:

    virtual uint    GetOpenMode();
    virtual const owl_string GetName();
    virtual uint32  LastError();
    virtual bool    IsOpen();

    virtual bool    Close();
    virtual uint32  Read(void * buffer, uint32 numBytes);
    virtual bool    Write(const void * buffer, uint32 numBytes);
    virtual uint64  Position64() const;
    virtual uint64  Length64() const;
    virtual uint32  Position() const;
    virtual uint32  Length() const;
    virtual uint64  Seek(int64 offset, __OWL TFile::seek_dir origin = __OWL TFile::beg);
    virtual uint32  Seek(long offset, __OWL TFile::seek_dir origin = __OWL TFile::beg);
    virtual bool    Flush() { return true; }

	// not supported
#if defined(BI_NO_COVAR_RET)
    virtual __OWL TFileHandle* 					Clone() const	{ return 0;}
#else
    virtual TInternetFileHandle* 	Clone() const	{ return 0;}
#endif
    virtual bool    LockRange(uint32 position, uint32 count){ return false;}
    virtual bool    UnlockRange(uint32 Position, uint32 count){ return false;}
    virtual bool    LockRange(uint64 position, uint64 count){ return false;}
    virtual bool    UnlockRange(uint64 Position, uint64 count){ return false;}
    virtual bool    Length(uint64 newLen) {return false;}
    virtual bool    Length(uint32 newLen)   {return false;}
    virtual bool    GetStatus(__OWL TFileStatus & status) const;

  protected:
    uint32      OpenMode;
    owl_string	Name;
};

//
// class TInternetFile
// ~~~~~ ~~~~~~~~~~~~~
//
class _OWLFXCLASS TInternetFile : public __OWL TBufferedFile {
	// Constructors
	protected:
		TInternetFile(TInternetFileHandle* handle=0);
   	//virtual ~TInternetFile(){}

		// supported only for TFtpFile
		virtual bool 	 	Write(const void* buffer, uint32 numBytes){	return false; }

		// not supported
		virtual bool LockRange(uint32 position, uint32 count){return false;}
		virtual bool UnlockRange(uint32 Position, uint32 count){return false;}
#if defined(BI_PLAT_WIN32)
		virtual bool LockRange(uint64 position, uint64 count){return false;}
		virtual bool UnlockRange(uint64 Position, uint64 count){return false;}
		virtual bool Length(uint64 newLen){return false;}
#endif
        virtual bool Length(uint32 newLen){return false;}
        virtual uint32 Length() const {return __OWL TBufferedFile::Length();}

	protected:
		//TInternetSession& Session;
};

//////////////////////////////////////////////////////////////////////////////////
//
// FTP related classes
//

//
//
// class TFtpConnection
// ~~~~~ ~~~~~~~~~~~~~~
//
class _OWLFXCLASS TFtpConnection : public TInternetConnection {
	public:
  	TFtpConnection(TInternetSession& session):TInternetConnection(session){}
  	TFtpConnection(TInternetSession& session, LPCTSTR server,LPCTSTR user = NULL,
                   LPCTSTR pasword = NULL, uint32 context = 0,
                   INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT,
                   bool passive = false);
    bool Open(LPCTSTR server, INTERNET_PORT port=INTERNET_DEFAULT_FTP_PORT,
              LPCTSTR user=0, LPCTSTR pasword = 0,
              bool passive = false, uint32 context=0);
};

//
// class TFtpFileName
// ~~~~~ ~~~~~~~~~~~~
//
//
class _OWLFXCLASS TFtpFileName: public __OWL TFileName {
	public:
    // Construct an empty FTP filename
    //
		TFtpFileName(TFtpConnection& connection);

    // Construct a filename from its parts. OK to pass 0 to skip part
    //
    TFtpFileName(TFtpConnection& connection, LPCTSTR path,
                 LPCTSTR file, LPCTSTR ext);

    // Construct a filename from a freeform string
    //
    TFtpFileName(TFtpConnection& connection, LPCTSTR  freeform);
    TFtpFileName(TFtpConnection& connection, const owl_string& freeform);

    // Construct a filename from another filename
    //
    TFtpFileName(const TFtpFileName& src);

    TFtpFileName& operator =(const TFtpFileName& src);
    TFtpFileName& operator =(const owl_string& src);
    TFtpFileName& operator =(LPCTSTR src);

    // Construct a filename representing a special file or directory
    //
    TFtpFileName(TFtpConnection& connection, TSpecialType type);

    virtual owl_string Canonical(bool forceUNC = false) const;
    virtual owl_string ShortName(bool forceUNC = false) const;

    // Is another filename equivalent to this filename?
    //
    bool operator ==(const TFtpFileName& other) const;

    // Remove the file or dir associated with this filename.
    //
    virtual bool Remove() const;
    virtual bool Move(const __OWL TFileName& newName, uint32 how = CopyAllowed);
    // Copy the file associated with this filename to a new file
    //
    virtual bool Copy(const __OWL TFileName& newName, bool failIfExists) const;
    virtual bool ChangeDir() const;
    virtual bool CreateDir() const;

		// new functionality
    bool PutFile(LPCTSTR localFile,
								 uint32 flags = FTP_TRANSFER_TYPE_BINARY,
								 uint32 context = 0);

		bool GetFile(LPCTSTR localFile, bool failIfExists = true,
									uint32 attr = FILE_ATTRIBUTE_NORMAL,
									uint32 flags = FTP_TRANSFER_TYPE_BINARY,
									uint32 context = 0);

		TFtpConnection& GetConnection() { return Connection; }
	protected:
		TFtpConnection& Connection;

	private:
    void   Parse(LPCTSTR freeform);
};

//
// class TFtpFileNameIterator
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~
//
//
class _OWLFXCLASS TFtpFileNameIterator : public __OWL TFileNameIterator {
	public:
    TFtpFileNameIterator(	TFtpConnection& connection,
													const owl_string& wildName,
													uint32 flags=INTERNET_FLAG_RELOAD,
													uint32 context=0);
		virtual ~TFtpFileNameIterator();

	protected:
    TFtpFileNameIterator(TFtpConnection& connection):Connection(connection){}
    // all work do here -> must fill TFileStatus structure return true on success
    virtual bool FindNext(__OWL TFileStatus& status, void* findData);

	protected:
		TFtpConnection& Connection;
};

//
// class TFtpFile
// ~~~~~ ~~~~~~~~~
//
class _OWLFXCLASS TFtpFile : public TInternetFile{
	friend class TInternetSession;
	public:
		TFtpFile(TFtpFileName& fileName, uint32 mode=TFtpFile::ReadOnly,
			       uint32 flags=0, uint32 context=0);
		virtual bool Write(const void* buffer, uint32 numBytes);
	protected:
		TFtpFile(TInternetFileHandle* handle):TInternetFile(handle){}
};

////////////////////////////////////////////////////////////////////////////////
//
// Gopher related classes
//
class _OWLFXCLASS TGopherConnection;
class _OWLFXCLASS TGopherFile;
class _OWLFXCLASS TGopherFileName;
class _OWLFXCLASS TGopherFileNameIterator;

//
// class TGopherConnection
// ~~~~~ ~~~~~~~~~~~~~~~~~
//
class _OWLFXCLASS TGopherConnection : public TInternetConnection {
	public:
  	TGopherConnection(TInternetSession& session):TInternetConnection(session){}
  	TGopherConnection(TInternetSession& session, LPCTSTR server, LPCTSTR user = NULL,
                   LPCTSTR pasword = NULL, uint32 context = 0,
                   INTERNET_PORT nPort = INTERNET_DEFAULT_GOPHER_PORT);
    bool Open(LPCTSTR server,	INTERNET_PORT port=INTERNET_DEFAULT_GOPHER_PORT,
              LPCTSTR user=0, LPCTSTR pasword = 0, uint32 context=0);

		INTERNET_PORT GetPort() { return Port; }

	protected:
		INTERNET_PORT	Port;
};

//
// class TGopherFileName
// ~~~~~ ~~~~~~~~~~~~~~~
//
//
class _OWLFXCLASS TGopherFileName: public __OWL TFileName {
	public:
		TGopherFileName(TGopherConnection& connection,
			              LPCTSTR displayString, LPCTSTR selectorString,
										uint32 type);
		TGopherFileName(TGopherFileNameIterator& iterator);
		~TGopherFileName();

		bool GetLocatorType(uint32& dwRef) const;
		bool GetAttribute(uint8* buffer, uint32 buffSize, uint32& retsuze,
			                LPCTSTR atrlName = 0,
											GOPHER_ATTRIBUTE_ENUMERATOR attrEnum = 0,
											uint32 context = 0);

		TGopherConnection&	GetConnection() { return Connection; }
	protected:
		_TCHAR* Locator;  // _not_ a zero-terminated string!
		uint32 	Length;
		TGopherConnection&	Connection;

	friend class TGopherFileNameIterator;
	friend class TGopherFile;
	private:
		void Parse(LPCTSTR name);
};

//
// class TGopherFileNameIterator
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~
//
//
class _OWLFXCLASS TGopherFileNameIterator : public __OWL TFileNameIterator{
	public:
    TGopherFileNameIterator(TGopherConnection& connection,
													const owl_string& wildName,
													uint32 flags=INTERNET_FLAG_RELOAD,
													uint32 context=0);
    TGopherFileNameIterator(TGopherFileName& name,
													const owl_string& wildName,
													uint32 flags=INTERNET_FLAG_RELOAD,
													uint32 context=0);
		virtual ~TGopherFileNameIterator();

		TGopherConnection&	GetConnection() { return Connection; }

  protected:
		void Init(LPCTSTR locator, LPCTSTR name, uint32 flags, uint32 context);
    TGopherFileNameIterator(TGopherConnection& connection):Connection(connection){}
    // all work do here -> must fill TFileStatus structure and return true on success
    virtual bool FindNext(__OWL TFileStatus& status, void* findData);
		LPCTSTR GetLocator();

	protected:
		TGopherConnection& Connection;
		friend class TGopherFileName;
};

//
// class TGopherFile
// ~~~~~ ~~~~~~~~~~~
//
class _OWLFXCLASS TGopherFile : public TInternetFile {
	friend class TInternetSession;
	public:
		TGopherFile(TGopherFileName& name, LPCTSTR view=0, uint32 flags=0, uint32 context = 0);
	protected:
		TGopherFile(TInternetFileHandle* handle):TInternetFile(handle){}

};


////////////////////////////////////////////////////////////////////////////////
//
// HTTP related classes
//
class _OWLFXCLASS THttpConnection;
class _OWLFXCLASS THttpFile;
class _OWLFXCLASS THttpFileName;
class _OWLFXCLASS THttpFileNameIterator;

//
// class THttpConnection
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLFXCLASS THttpConnection : public TInternetConnection{
	public:
 		THttpConnection(TInternetSession& session):TInternetConnection(session){}
  	THttpConnection(TInternetSession& session, LPCTSTR server,
										INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT,
										LPCTSTR user = NULL, LPCTSTR password = NULL,
										uint32 flags = 0, uint32 context = 0);

  	bool Open(LPCTSTR server, INTERNET_PORT port=INTERNET_DEFAULT_HTTP_PORT,
              LPCTSTR user=NULL, LPCTSTR pasword = NULL, uint32 flags = 0,
							uint32 context=0);
};

//
// class THttpFile
// ~~~~~ ~~~~~~~~~
//
class _OWLFXCLASS THttpFile : public TInternetFile {
	friend class TInternetSession;
	public:
		enum THttpVerb {
			verbMin=0, verbPost = verbMin, verbGet, verbHead, verbPut,
			verbLink, verbDelete, verbUnlink, verbMax	= verbUnlink,
		};
	public:
		THttpFile(THttpConnection& connect);
		THttpFile(THttpConnection& connect,LPCTSTR pstrVerb,
			        LPCTSTR name, LPCTSTR pstrReferer=0, uint32 context=0,
							LPCTSTR* ppstrAcceptTypes=0, LPCTSTR pstrVersion=0,
							uint32 flags=INTERNET_FLAG_EXISTING_CONNECT);
		THttpFile(THttpConnection& connect, THttpVerb verb,
			        LPCTSTR name, LPCTSTR pstrReferer=0, uint32 context=0,
							LPCTSTR* ppstrAcceptTypes=0, LPCTSTR pstrVersion=0,
							uint32 flags=INTERNET_FLAG_EXISTING_CONNECT);

		bool Open(LPCTSTR pstrVerb, LPCTSTR name, LPCTSTR pstrReferer=0,
			        uint32 context=0, LPCTSTR* ppstrAcceptTypes=0,
							LPCTSTR pstrVersion=0,
							uint32 flags=INTERNET_FLAG_EXISTING_CONNECT);
		bool Open(THttpVerb verb, LPCTSTR name, LPCTSTR pstrReferer=0,
			        uint32 context=0, LPCTSTR* ppstrAcceptTypes=0,
							LPCTSTR pstrVersion=0,
							uint32 flags=INTERNET_FLAG_EXISTING_CONNECT);

		owl_string GetVerb() const { return VerbName; }
		THttpConnection* GetConnection() { return Connection; }

		uint32 ErrorDlg(__OWL TWindow* parent,
			              uint32 error=ERROR_INTERNET_INCORRECT_PASSWORD,
										uint32 flags=FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
										LPVOID* lppvData=NULL);

		bool AddRequestHeaders(LPCTSTR headers,
			                     uint32 flags = HTTP_ADDREQ_FLAG_ADD_IF_NEW,
													 int headersLen = -1);
		bool SendRequest(LPCTSTR headers = 0,
			               uint32 len = 0,
										 void _FAR* optional = 0,
										 uint32 optionalLen = 0);
		bool QueryInfo(uint32 level,
			             void _FAR* buffer,
									 uint32& size,
				  				 uint32* index = 0) const;
		bool QueryInfoStatusCode(uint32& statusCode) const;

	protected:
		THttpFile(TInternetFileHandle* handle):Connection(0),TInternetFile(handle){}

	protected:
		THttpConnection*	Connection;
		owl_string				VerbName;
};

//////////////////////////////////////////////////////////////////////////////////
//
// class TXInternet
// ~~~~~ ~~~~~~~~~~
//
class _OWLFXCLASS TXInternet : public __OWL TXOwl {
	public:
  	TXInternet(uint resId, __OWL TModule* module = gModule);

		bool GetErrorMessage(LPTSTR pstrError, uint nMaxError);
#if defined(BI_NO_COVAR_RET)
    __OWL TXBase* Clone();
#else
    TXInternet* Clone();
#endif
    void Throw();
    // Construct a TXInternet exception from scratch, and throw it.
    //
  	static void Raise(uint resId, __OWL TModule* module=0);


	protected:
		uint32	LastError;
};

//////////////////////////////////////////////////////////////////////////////////
//
// class TWinInetDll
// ~~~~~ ~~~~~~~~~~~
// Wrapper for the WinInet dll itself
//
class _OWLFXCLASS TWinInetDll {
  public:
    static HINTERNET InternetOpen(LPCTSTR lpszAgent, DWORD dwAccessType,
                                  LPCTSTR lpszProxyName,LPCSTR lpszProxyBypass,
                                  DWORD dwFlags);

    static HINTERNET InternetConnect(HINTERNET hInternetSession,
                                     LPCTSTR lpszServerName, INTERNET_PORT nServerPort,
                                     LPCTSTR lpszUsername,LPCTSTR lpszPassword,
                                     DWORD dwService,DWORD dwFlags,DWORD dwContext);
    static BOOL InternetCloseHandle(HINTERNET hInet);
    static BOOL InternetQueryOption(HINTERNET hInternet,DWORD dwOption,
                                    LPVOID lpBuffer,LPDWORD lpdwBufferLength);
    static BOOL InternetSetOption(HINTERNET hInternet,DWORD dwOption,
    	                            LPVOID lpBuffer,DWORD dwBufferLength);
    static BOOL InternetSetOptionEx(HINTERNET hInternet,DWORD dwOption,
			                              LPVOID lpBuffer,DWORD dwBufferLength,
																		DWORD dwFlags);
    static INTERNET_STATUS_CALLBACK InternetSetStatusCallback(HINTERNET hInternet,
    														INTERNET_STATUS_CALLBACK lpfnInternetCallback);

  	static DWORD InternetConfirmZoneCrossing(HWND hWnd, LPSTR szUrlPrev,
                                             LPSTR szUrlNew, BOOL bPost);

  	static BOOL InternetTimeFromSystemTime(CONST SYSTEMTIME* pst, DWORD dwRFC,
                                           LPSTR lpszTime, DWORD cbTime);
  	static BOOL InternetTimeToSystemTime(LPCSTR lpszTime,SYSTEMTIME* pst,
                                         DWORD dwReserved);
  	static DWORD InternetAttemptConnect(DWORD dwReserved);
  	static BOOL InternetReadFile(HINTERNET hFile,LPVOID lpBuffer, DWORD dwNumToRead,
                                 LPDWORD lpNumReaded);
  	static BOOL InternetSetFilePointer(HINTERNET hFile, LONG IDistanceToMove,
                                       PVOID pReserved, DWORD dwMoveMethod,
                                       DWORD dwContext);
  	static BOOL InternetFindNextFile(HINTERNET hFind,LPVOID lpvFindData);
  	static BOOL InternetQueryDataAvailable(HINTERNET hFile,
                                           LPDWORD lpdwNumberOfBytesAvailable,
                                           DWORD dwFlags, DWORD dwContext);
  	static BOOL InternetGetLastResponseInfo(LPDWORD lpdwError,LPTSTR lpszBuffer,
                                            LPDWORD lpdwBufferLength);
  	static BOOL InternetWriteFile(HINTERNET hFile,LPCVOID lpBuffer,
                                  DWORD dwNumbToWrite, LPDWORD lpdwNumWritten);

		// URL
  	static BOOL InternetCrackUrl(LPCSTR lpszUrl,DWORD dwUrlLength,DWORD dwFlags,
                                 LPURL_COMPONENTS lpUrlComponents);
  	static BOOL InternetCreateUrl(LPURL_COMPONENTS lpUrlComponents,DWORD dwFlags,
                                  LPSTR lpszUrl,LPDWORD lpdwUrlLength);
  	static BOOL InternetCanonicalizeUrl(LPCTSTR lpszUrl, LPTSTR lpszBuffer,
                                        LPDWORD lpdwBufferLength, DWORD dwFlags);
  	static BOOL InternetCombineUrl(LPCTSTR lpszBaseUrl, LPCTSTR lpszRelativeUrl,
                                   LPTSTR lpszBuffer,LPDWORD lpdwBufferLength,
                                   DWORD dwFlags);
  	static HINTERNET InternetOpenUrl(HINTERNET hInternetSession,LPCTSTR lpszUrl,
                                     LPCTSTR lpszHeaders, DWORD dwHeadersLength,
                                     DWORD dwFlags, DWORD dwContext);
    //FTP Functions
  	static HINTERNET FtpFindFirstFile(HINTERNET hFtpSession,
                                      LPCTSTR lpszSearchFile,
                                      LPWIN32_FIND_DATA lpFindFileData,
                                      DWORD dwFlags, DWORD dwContext);
  	static BOOL FtpGetFile(HINTERNET hFtpSession, LPCSTR lpszRemoteFile,
                           LPCSTR lpszNewFile, BOOL fFailIfExists,
                           DWORD dwFlagsAndAttributes, DWORD dwFlags,
                           DWORD dwContext);
  	static BOOL FtpPutFile(HINTERNET hFtpSession, LPCTSTR lpszLocalFile,
                           LPCTSTR lpszNewRemoteFile, DWORD dwFlags,
                           DWORD dwContext);
  	static BOOL FtpDeleteFile(HINTERNET hFtpSession, LPCTSTR lpszFileName);
  	static BOOL FtpRenameFile(HINTERNET hFtpSession, LPCTSTR lpszExisting,
                              LPCTSTR lpszNew);
  	static HINTERNET FtpOpenFile(HINTERNET hFtpSession,LPCSTR lpszFileName,
                                 DWORD fdwAccess, DWORD dwFlags, DWORD dwContext);
  	static BOOL FtpCreateDirectory(HINTERNET hFtpSession, LPCTSTR lpszDirectory);
  	static BOOL FtpRemoveDirectory(HINTERNET hFtpSession, LPCTSTR lpszDirectory);
  	static BOOL FtpSetCurrentDirectory(HINTERNET hFtpSession,LPCTSTR lpszDirectory);
  	static BOOL FtpGetCurrentDirectory(HINTERNET hFtpSession,
                                       LPCTSTR lpszCurrentDirectory,
                                       LPDWORD lpdwCurrentDirectory);
		// Gopher Functions
  	static HINTERNET GopherFindFirstFile(HINTERNET hGopherSession,
                                         LPCTSTR lpszLocator,
                                         LPCTSTR lpszSearchString,
                                         LPGOPHER_FIND_DATA lpFindData,
                                         DWORD dwFlags,DWORD dwContext);
  	static HINTERNET GopherOpenFile(HINTERNET hGopherSession, LPCTSTR lpszLocator,
                                    LPCTSTR lpszView, DWORD dwFlags,
                                    DWORD dwContext);
  	static BOOL GopherCreateLocator(LPCTSTR lpszHost, INTERNET_PORT nServerPort,
                                    LPCTSTR lpszDisplayString,
                                    LPCTSTR lpszSelectorString,
                                    DWORD dwGopherType,
                                    LPCTSTR lpszLocator,
                                    LPDWORD lpdwBufferLength);
  	static BOOL GopherGetAttribute(HINTERNET hGopherSession, LPCTSTR lpszLocator,
                                   LPCTSTR lpszAttributeName, LPBYTE lpBuffer,
                                   DWORD dwBufferLength,
                                   LPDWORD lpdwCharactersReturned,
                                   GOPHER_ATTRIBUTE_ENUMERATOR lpfnEnumerator,
                                   DWORD dwContext);
  	static BOOL GopherGetLocatorType(LPCTSTR lpszLocator,
			                               LPDWORD lpdwGopherType);

		// HTTP (World Wide Web) Functions
  	static HINTERNET HttpOpenRequest(HINTERNET hHttpSession, LPCTSTR lpszVerb,
                              LPCTSTR lpszObjectName, LPCTSTR lpszVersion,
                              LPCTSTR lpszReferer, LPCTSTR * lpszAcceptTypes,
                              DWORD dwFlags, DWORD dwContext);
  	static BOOL HttpAddRequestHeaders(HINTERNET hHttpRequest, LPCTSTR lpszHeaders,
                                      DWORD dwHeadersLength, DWORD dwModifiers);
  	static BOOL HttpSendRequest(HINTERNET hHttpRequest, LPCTSTR lpszHeaders,
                                DWORD dwHeadersLength, LPVOID lpOptional,
                                DWORD dwOptionalLength);
  	static BOOL HttpQueryInfo(HINTERNET hHttpRequest,DWORD dwInfoLevel,
                              LPVOID lpvBuffer, LPDWORD lpdwBufferLength,
                              LPDWORD lpdwIndex);
  	static DWORD InternetErrorDlg(HWND hWnd, HINTERNET hInternet, DWORD dwError,
                                  DWORD dwFlags,LPVOID *lppvData);

    //Cookie Functions
  	static BOOL InternetGetCookie(LPCSTR lpszUrlName,LPCSTR lpszCookieName,
                                  LPSTR lpszCookieData,LPDWORD  lpdwSize);
  	static BOOL InternetSetCookie(LPCSTR lpszUrlName, LPCSTR lpszCookieName,
                                  LPCSTR lpszCookieData);
    //Cache Functions
    //
    //

    //
    //
    static __OWL TModule& GetModule();
    static bool     IsAvailable();
};

//
// class TWinInet
// ~~~~~ ~~~~~~~~
//
class _OWLFXCLASS TWinInet {
  public:
    static bool       	IsAvailable();
    static TWinInetDll*	Dll();
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

///////////////////////////
// inlines

//
//
//
inline uint	TInternetFileHandle::GetOpenMode(){
	return OpenMode;
}

//
//
//
inline bool	TInternetFileHandle::IsOpen(){
	return Handle != 0;
}


_OWLFX_END_NAMESPACE

//
#endif // OWL_INTERNET_H
