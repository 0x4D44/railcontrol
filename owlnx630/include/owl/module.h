//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Definition of class TModule.  TModule defines the basic behavior for OWL
// libraries and applications.
//----------------------------------------------------------------------------

#if !defined(OWL_MODULE_H)
#define OWL_MODULE_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_OBJSTRM_H)
# include <owl/objstrm.h>
#endif
#if !defined(OWL_EXCEPT_H)
# include <owl/except.h>
#endif
#if !defined(OWL_POINTER_H)
# include <owl/pointer.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
#include <owl/lclstrng.h>
#endif
#if !defined(OWL_GMODULE_H)
# include <owl/private/gmodule.h>
#endif
#if !defined(OWL_WSYSCLS_H)
# include <owl/wsyscls.h>
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

class _OWLCLASS TWindow;
class _OWLCLASS TDialog;
class _OWLCLASS TXInvalidModule;
class _OWLCLASS TXModuleVersionInfo; // [VH 2005-04-03]

//
// class TModule
// ~~~~~ ~~~~~~~
class _OWLCLASS TModule : public TStreamableBase {
  public:
    // Class scoped types
    //
    typedef HINSTANCE THandle;  // TModule encapsulates an HINSTANCE

    // Constructors & destructor
    //
    TModule(LPCTSTR name, bool shouldLoad = true, bool mustLoad = true, bool addToList=true);
    TModule(LPCTSTR name, THandle handle, bool addToList=true);
    TModule(LPCTSTR name, THandle handle, LPCTSTR cmdLine, bool addToList=true);
    virtual ~TModule();

    // Finish-up initialization of a module
    //
    void          InitModule(THandle handle, LPCTSTR cmdLine);

    // Get & set members. Use these instead of directly accessing members
    //
    LPCTSTR       GetName() const;
    void          SetName(LPCTSTR name);

    THandle       GetHandle() const;      // Get the module instance handle
    operator      THandle() const;
    bool operator ==(const TModule& m) const;
    bool          IsLoaded() const;

    // Module wide error handler. Called when fatal exceptions are caught.
    //
#if defined(BI_STD_EXCEPTIONS)
//FMM Had to make it std::exception from exception
    virtual int   Error(std::exception& x, uint captionResId, uint promptResId=0);
#else
    virtual int   Error(XBASE& x, uint captionResId, uint promptResId=0);
#endif

    // Windows HINSTANCE/HMODULE related API functions encapsulated
    //
    int           GetModuleFileName(LPTSTR buff, int maxChars);

    FARPROC       GetProcAddress(LPCSTR fcnName) const;

    HRSRC         FindResource(TResId id, LPCTSTR type) const;
    HRSRC         FindResourceEx(TResId id, LPCTSTR type, TLangId langId=LangNeutral) const;
    HGLOBAL       LoadResource(HRSRC hRsrc) const;
    uint32        SizeofResource(HRSRC hRsrc) const;


    int           LoadString(uint id, LPTSTR buf, int maxChars) const;
    owl_string    LoadString(uint id) const;
    HBITMAP       LoadBitmap(TResId id) const;
    bool          GetClassInfo(LPCTSTR name, WNDCLASS * wndclass) const;
    HACCEL        LoadAccelerators(TResId id) const;
    HMENU         LoadMenu(TResId id) const;
    HCURSOR       LoadCursor(TResId id) const;
    HICON         LoadIcon(LPCTSTR name) const;

    HICON         CopyIcon(HICON hIcon) const;

    //
    // Global  search for  resources
    //
    static TModule* FindResModule(TResId id, LPCTSTR type);
    static TModule* NextModule(TModule* module = 0);

  protected:
    void          SetHandle(THandle handle);  // Set the module instance handle

  protected_data:
    TCHAR*         Name;       // Name of the module
    union {
#if defined(OWL2_COMPAT)
      THandle     HInstance;  // Old 2.x name
#endif
      THandle     Handle;     // New name
    };

  private:
    bool          ShouldFree; // Should free the module when done?

    // Hidden to prevent accidental copying or assignment
    //
    TModule(const TModule&);
    TModule& operator =(const TModule&);

#if defined(OWL2_COMPAT)
  // Obsolete members for Owl 2.x compatibility
  //
  public:
    THandle       GetInstance() const;
    void          SetInstance(THandle handle);
    typedef __OWL_GLOBAL::TXInvalidModule TXInvalidModule;  // Exceptions moved to global scope
#endif
#if defined(OWL1_COMPAT)
  // Obsolete members for Owl 1 compatibility
  //
  public:
    LPTSTR     lpCmdLine;    // Use argv & argc for portability
    TStatus   Status;       // Use exceptions

    TWindow*  ValidWindow(TWindow* win);
    TWindow*  MakeWindow(TWindow* win);
    int       ExecDialog(TDialog* dialog);

    virtual void  Error(int errorCode);

    HWND      GetClientHandle(HWND hWnd);
    TWindow*  GetParentObject(HWND hWndParent);

    bool      LowMemory();
    void      RestoreMemory();
#endif   //#if defined(OWL1_COMPAT)

    friend _OWLCFUNC(NS_OWL_STREAMS::ostream&) operator <<(NS_OWL_STREAMS::ostream& os, const TModule& m);
  DECLARE_STREAMABLE_OWL(__OWL TModule, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( __OWL TModule);

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

//
// Bring in the system's version info header if not already included
//
__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TModuleVersionInfo
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// Access to a TModule's VERSIONINFO resource.
//
class _OWLCLASS TModuleVersionInfo {
  public:
    // TFileOS values are returned by GetFileOS()
    enum TFileOS { OSUnknown    = VOS_UNKNOWN,
                   DOS          = VOS_DOS,
                   OS216        = VOS_OS216,
                   OS232        = VOS_OS232,
                   NT           = VOS_NT,
                   Windows16    = VOS__WINDOWS16,
                   PM16         = VOS__PM16,
                   PM32         = VOS__PM32,
                   Windows32    = VOS__WINDOWS32,
                   DosWindows16 = VOS_DOS_WINDOWS16,
                   DosWindows32 = VOS_DOS_WINDOWS32,
                   OS216PM16    = VOS_OS216_PM16,
                   OS232PM32    = VOS_OS232_PM32,
                   NTWindows32  = VOS_NT_WINDOWS32
    };
    // TFileType is returned by GetFileType()
    enum TFileType { TypeUnknown = VFT_UNKNOWN,
                     App         = VFT_APP,
                     DLL         = VFT_DLL,
                     DevDriver   = VFT_DRV,
                     Font        = VFT_FONT,
                     VirtDevice  = VFT_VXD,
                     StaticLib   = VFT_STATIC_LIB
    };
    // TFileSubType values are returned by GetFileSubType() if GetFileType
    // returned DevDriver or Font
    enum TFileSubType { UnknownDevDriver,  //VFT2_UNKNOWN
                        PtrDriver,         //VFT2_DRV_PRINTER
                        KybdDriver,        //VFT2_DRV_KEYBOARD
                        LangDriver,        //VFT2_DRV_LANGUAGE
                        DisplayDriver,     //VFT2_DRV_DISPLAY
                        MouseDriver,       //VFT2_DRV_MOUSE
                        NtwkDriver,        //VFT2_DRV_NETWORK
                        SysDriver,         //VFT2_DRV_SYSTEM
                        InstallableDriver, //VFT2_DRV_INSTALLABLE
                        SoundDriver,       //VFT2_DRV_SOUND
                        UnknownFont,       //VFT2_UNKNOWN
                        RasterFont,        //VFT2_FONT_RASTER
                        VectorFont,        //VFT2_FONT_VECTOR
                        TrueTypeFont       //VFT2_FONT_TRUETYPE
    };
    TModuleVersionInfo(TModule::THandle module);
    TModuleVersionInfo(owl_string modFName); // String version. [VH 2005-04-03]
   ~TModuleVersionInfo();

    VS_FIXEDFILEINFO & GetFixedInfo();

    uint32    GetSignature() const;
    uint32    GetStrucVersion() const;
    uint32    GetFileVersionMS() const;
    uint32    GetFileVersionLS() const;
    uint32    GetProductVersionMS() const;
    uint32    GetProductVersionLS() const;
    bool      IsFileFlagSet(uint32 flag) const;
    uint32    GetFileFlagsMask() const;
    uint32    GetFileFlags() const;
    bool      IsDebug() const;
    bool      InfoInferred() const;
    bool      IsPatched() const;
    bool      IsPreRelease() const;
    bool      IsPrivateBuild() const;
    bool      IsSpecialBuild() const;
    uint32    GetFileOS() const;  // returns TFileOS values
    TFileType GetFileType() const;
    uint32    GetFileSubType() const;
    FILETIME  GetFileDate() const;

    bool GetInfoString(LPCTSTR str, LPCTSTR& value, uint lang=0);

    bool GetFileDescription(LPCTSTR& fileDesc, uint lang=0);
    bool GetFileVersion(LPCTSTR& fileVersion, uint lang=0);
    bool GetInternalName(LPCTSTR& internalName, uint lang=0);
    bool GetLegalCopyright(LPCTSTR& copyright, uint lang=0);
    bool GetOriginalFilename(LPCTSTR& originalFilename, uint lang=0);
    bool GetProductName(LPCTSTR& prodName, uint lang=0);
    bool GetProductVersion(LPCTSTR& prodVersion, uint lang=0);
    bool GetSpecialBuild(LPCTSTR& debug, uint lang=0);

    // String versions of the query functions. [VH 2005-04-03]
    // These functions throw TXModuleVersionInfo on error.

    owl_string GetInfoString(owl_string str, uint lang=0);

    owl_string GetFileDescription(uint lang=0);
    owl_string GetFileVersion(uint lang=0);
    owl_string GetInternalName(uint lang=0);
    owl_string GetLegalCopyright(uint lang=0);
    owl_string GetOriginalFilename(uint lang=0);
    owl_string GetProductName(uint lang=0);
    owl_string GetProductVersion(uint lang=0);
    owl_string GetSpecialBuild(uint lang=0);

    uint        GetLanguage() const;
    owl_string  GetLanguageName() const;

    static owl_string GetLanguageName(uint language);

  protected:
    void Init(LPCTSTR modFName);

    uint8 *            Buff;       // new'd File version info buffer
    uint32                Lang;       // Default language translation
    VS_FIXEDFILEINFO * FixedInfo;  // Fixed file info structure

  private:
    // Don't allow this object to be copied.
    //
    TModuleVersionInfo(const TModuleVersionInfo&);
    TModuleVersionInfo& operator =(const TModuleVersionInfo&);
};

//
// @class TXInvalidModule
// ~~~~~ ~~~~~~~~~~~~~~~
//@base public | TXOwl
class _OWLCLASS TXInvalidModule : public TXOwl {
  public:
    TXInvalidModule(LPCTSTR name = 0);

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXInvalidModule* Clone();
#endif
    void Throw();

    static void Raise(LPCTSTR name = 0);
};

//
// Exception class for TModuleVersionInfo. [VH 2005-04-03]
//
class _OWLCLASS TXModuleVersionInfo : public TXOwl {
  public:
    TXModuleVersionInfo(LPCTSTR name = 0);

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXModuleVersionInfo* Clone();
#endif
    void Throw();

    static void Raise(LPCTSTR name = 0);
};


//
//
//@class TSystemMessage
//~~~~~~~~~~~~~~~~~~~~~
//
class _OWLCLASS TSystemMessage {
  public:
    TSystemMessage(); // default errorId, def language
    TSystemMessage(uint32 error, TLangId langId = LangNeutral);

     int MessageBox(TWindow* wnd=0, LPCTSTR msg=0, LPCTSTR title=0, uint flags=MB_OK);
    const owl_string& SysMessage() const;
    const uint32      SysError() const;

  protected:
    void        Init(TLangId langId);
    uint32      Error;
    owl_string   Message;
};

//
// @class TErrorMode
// ~~~~~ ~~~~~~~~~~
// Simple encapsulation of the SetErrorMode call. Manages putting the error
// mode back to its previous state on destruction, thus is exception safe.
//
class TErrorMode {
  public:
    TErrorMode(uint mode);
   ~TErrorMode();

  private:
    uint PrevMode;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

//----------------------------------------------------------------------------
// Global variable and functions

//
// Exported pointers from OWL modules, implemented in GLOBAL.CPP
// Unmanagled to allow easy loading via LoadLibrary
//
__OWL_DECLCLASS(TDocTemplate);
#if defined(BI_COMP_BORLANDC) 
extern "C" {
  __OWL TDocTemplate** PASCAL GetDocTemplateHead(uint32 version);
  __OWL TModule** PASCAL GetModulePtr(uint32 version);
}
#else
STDAPI_(__OWL TDocTemplate**) GetDocTemplateHead(uint32 version);
STDAPI_(__OWL TModule**) GetModulePtr(uint32 version);
#endif
//
// Main entry point for an Owl application
//
int OwlMain(int argc, _TCHAR* argv[]);

extern "C" {

//
// Initialization routine that must be called from User DLL if DLL
// provides it's own entry point [i.e. LibMain or DllEntryPoint]
//
bool OWLInitUserDLL(HINSTANCE hInstance, LPTSTR cmdLine);
}

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//----------------------------------------------------------------------------
// Definition of TDllLoader template, and TModuleProc class & derived
// templates. TDllLoader<> provides an easy way to load one instance of a dll
// on demand.
// TModuleProcX<>s provide dynamic binding & call access to exported module
// procs
//

//
// @class TDllLoader<>
// ~~~~~ ~~~~~~~~~~~~
template <class T> class TDllLoader {
  public:
    TDllLoader();
   ~TDllLoader();
    static bool IsAvailable();
    static T*   Dll();

#if 0
    //!BB Static data members in template is a problem when
    //!BB exporting/importing templates. Basically, the initialization of
    //!BB the static member [see the statement which initializes Dll to 0
    //!BB below] must only be seen when exporting the template and causes an
    //!BB error when one is importing the template: 'Cannot initialize
    //!BB extern'. The error message makes sense since to the importer, the
    //!BB data member is an extern variable. However, there's a catch 22:
    //!BB Only the user of the template header knows whether it's
    //!BB importing or exporting a particular instance of the template. So
    //!BB how does the user communicate to the header whether or not to have
    //!BB the initialization code?? Furthermore, what if a particular user of
    //!BB the template is importing the template for Type1 and exporting it
    //!BB for Type2. Now, we have a problem either way.
    //!BB
    //!BB Ideally the compiler would simply ignore the initialization of
    //!BB a static data member when the code including the template is
    //!BB actually importing the template. For now though, we'll avoid
    //!BB static data members and use a static variable within a static
    //!BB method instead. It's less elegant but it solves this problem.

    static T*   Dll;
#endif
  private:
    static T*&  DllPtrRef();
};


#if 0
//!BB See comment about static data members and importing/exporting
//!BB templates above.

//
// One static pointer to the dll object, loaded on demand
//
template <class T> T* TDllLoader<T>::Dll = 0;
#endif

//
// Create a dynamic copy of the DLL object (i.e. an instance of T) if we
// do not have one already...
//
template <class T> TDllLoader<T>::TDllLoader()
{
  T*& dll = DllPtrRef();
  WARN(dll != 0, _T("Multiple copies of DllLoaders for DLL: ") << *dll);
  if (dll == 0) {
    try {
      dll = new T;
    }
    catch (...) {
    }
  }
}

//
// Delete the dll object when we go away to release the dll from memory
//
template <class T> TDllLoader<T>::~TDllLoader()
{
#if 0 //!BB
  delete Dll;
  Dll = 0;
#endif
  T*& dll = DllPtrRef();
  delete dll;
  dll = 0;
}

//
// Load the dll on demand, returning true if it was loaded OK
//
//#if !defined(WINELIB)//JJH
template <class T> bool TDllLoader<T>::IsAvailable()
{
  static TDllLoader<T> ThisLoader;
  return DllPtrRef() != 0;
}
//#endif

//
//
//
template <class T> T* TDllLoader<T>::Dll()
{
  PRECONDITION(DllPtrRef() != 0);
  return DllPtrRef();
}

//
// Method encapsulating single instance of pointer to DLL objecct
//
//#if !defined(WINELIB)//JJH
template <class T> T*& TDllLoader<T>::DllPtrRef()
{
  static T* ThisDll = 0;
  return ThisDll;
}
//#endif

//
// class TModuleProc
// ~~~~~ ~~~~~~~~~~~
// Base module proc class does inital binding. Throws an exception if it
// cannot bind
//
class _OWLCLASS TModuleProc {
  public:
    TModuleProc(const TModule& module, LPCSTR id);

  protected:
    FARPROC Proc;
};

//
// Derived template classes perform type-safe parameter passing on call.
// Different class for each number of parameters, 'V' version for void return.
//

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class TModuleProcV0 : public TModuleProc {
  public:
    TModuleProcV0(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)();
    void operator ()() {
      ((TProc)Proc)();
    }
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
template <class R>
class TModuleProc0 : public TModuleProc {
  public:
    TModuleProc0(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)();
    R operator ()() {
      return ((TProc)Proc)();
    }
};

//
template <class P1>
class TModuleProcV1 : public TModuleProc {
  public:
    TModuleProcV1(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1);
    void operator ()(P1 p1) {
      ((TProc)Proc)(p1);
    }
};

//
template <class R, class P1>
class TModuleProc1 : public TModuleProc {
  public:
    TModuleProc1(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1);
    R operator ()(P1 p1) {
      return ((TProc)Proc)(p1);
    }
};

//
template <class P1, class P2>
class TModuleProcV2 : public TModuleProc {
  public:
    TModuleProcV2(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 a2);
    void operator ()(P1 p1, P2 a2) {
      ((TProc)Proc)(p1, a2);
    }
};

//
template <class R, class P1, class P2>
class TModuleProc2 : public TModuleProc {
  public:
    TModuleProc2(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2);
    R operator ()(P1 p1, P2 p2) {
      return ((TProc)Proc)(p1, p2);
    }
};

//
template <class P1, class P2, class P3>
class TModuleProcV3 : public TModuleProc {
  public:
    TModuleProcV3(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3);
    void operator ()(P1 p1, P2 p2, P3 p3) {
      ((TProc)Proc)(p1, p2, p3);
    }
};

//
template <class R, class P1, class P2, class P3>
class TModuleProc3 : public TModuleProc {
  public:
    TModuleProc3(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3);
    R operator ()(P1 p1, P2 p2, P3 p3) {
      return ((TProc)Proc)(p1, p2, p3);
    }
};

//
template <class P1, class P2, class P3, class P4>
class TModuleProcV4 : public TModuleProc {
  public:
    TModuleProcV4(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
      ((TProc)Proc)(p1, p2, p3, p4);
    }
};

//
template <class R, class P1, class P2, class P3, class P4>
class TModuleProc4 : public TModuleProc {
  public:
    TModuleProc4(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4) {
      return ((TProc)Proc)(p1, p2, p3, p4);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5>
class TModuleProcV5 : public TModuleProc {
  public:
    TModuleProcV5(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
      ((TProc)Proc)(p1, p2, p3, p4, p5);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5>
class TModuleProc5 : public TModuleProc {
  public:
    TModuleProc5(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6>
class TModuleProcV6 : public TModuleProc {
  public:
    TModuleProcV6(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
class TModuleProc6 : public TModuleProc {
  public:
    TModuleProc6(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class TModuleProcV7 : public TModuleProc {
  public:
    TModuleProcV7(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                     P7 p7);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
      ((TProc)Proc) (p1, p2, p3, p4, p5, p6, p7);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7>
class TModuleProc7 : public TModuleProc {
  public:
    TModuleProc7(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8>
class TModuleProcV8 : public TModuleProc {
  public:
    TModuleProcV8(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                     P7 p7, P8 p8);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8>
class TModuleProc8 : public TModuleProc {
  public:
    TModuleProc8(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7, P8 p8);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9>
class TModuleProcV9 : public TModuleProc {
  public:
    TModuleProcV9(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                    P7 p7, P8 p8, P9 p9);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9>
class TModuleProc9 : public TModuleProc {
  public:
    TModuleProc9(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7, P8 p8, P9 p9);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10>
class TModuleProcV10 : public TModuleProc {
  public:
    TModuleProcV10(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                     P7 p7, P8 p8, P9 p9, P10 p10);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10>
class TModuleProc10 : public TModuleProc {
  public:
    TModuleProc10(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7, P8 p8, P9 p9, P10 p10);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11>
class TModuleProcV11 : public TModuleProc {
  public:
    TModuleProcV11(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                     P7 p7, P8 p8, P9 p9, P10 p10, P11 p11);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11>
class TModuleProc11 : public TModuleProc {
  public:
    TModuleProc11(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7, P8 p8, P9 p9, P10 p10, P11 p11);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12>
class TModuleProcV12 : public TModuleProc {
  public:
    TModuleProcV12(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                     P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11, P12 p12) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12>
class TModuleProc12 : public TModuleProc {
  public:
    TModuleProc12(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                  P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11, P12 p12) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    }
};

//
template <class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13>
class TModuleProcV13 : public TModuleProc {
  public:
    TModuleProcV13(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                    P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,
                                    P12 p12, P13 p13);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11, P12 p12, P13 p13) {
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    }
};

//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13>
class TModuleProc13 : public TModuleProc {
  public:
    TModuleProc13(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                                    P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,
                                    P12 p12, P13 p13);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
                  P10 p10, P11 p11, P12 p12, P13 p13) {
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,class P14>
class TModuleProcV14 : public TModuleProc {
  public:
    TModuleProcV14(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,p13,p14);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14>
class TModuleProc14 : public TModuleProc {
  public:
    TModuleProc14(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,p13,p14);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,class P14, 
          class P15>
class TModuleProcV15 : public TModuleProc {
  public:
    TModuleProcV15(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,p15);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15>
class TModuleProc15 : public TModuleProc {
  public:
    TModuleProc15(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16>
class TModuleProcV16 : public TModuleProc {
  public:
    TModuleProcV16(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,
                    p15,p16);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16>
class TModuleProc16 : public TModuleProc {
  public:
    TModuleProc16(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15,p16);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17>
class TModuleProcV17 : public TModuleProc {
  public:
    TModuleProcV17(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,
                    p15,p16,p17);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17>
class TModuleProc17 : public TModuleProc {
  public:
    TModuleProc17(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15,p16,p17);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18>
class TModuleProcV18 : public TModuleProc {
  public:
    TModuleProcV18(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, P18 p18){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,
                    p15,p16,p17,p18);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18>
class TModuleProc18 : public TModuleProc {
  public:
    TModuleProc18(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, P18 p18){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15,p16,p17,p18);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18, class P19>
class TModuleProcV19 : public TModuleProc {
  public:
    TModuleProcV19(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18, P19 p19);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, 
         P18 p18, P19 p19){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,
                    p15,p16,p17,p18,p19);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18, class P19>
class TModuleProc19 : public TModuleProc {
  public:
    TModuleProc19(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18, P19 p19);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, 
         P18 p18, P19 p19){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15,p16,p17,p18,p19);
    }
};
//
template <class P1, class P2, class P3, class P4, class P5, class P6,class P7, 
          class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18, class P19, 
          class P20>
class TModuleProcV20 : public TModuleProc {
  public:
    TModuleProcV20(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef void ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20);
    void operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, 
         P18 p18, P19 p19, P20 p20){
      ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14,
                    p15,p16,p17,p18,p19,p20);
    }
};
//
template <class R, class P1, class P2, class P3, class P4, class P5, class P6,
          class P7, class P8, class P9, class P10, class P11, class P12, class P13,
          class P14, class P15, class P16, class P17, class P18, class P19, 
          class P20>
class TModuleProc20 : public TModuleProc {
  public:
    TModuleProc20(const TModule& module, LPCSTR id) : TModuleProc(module, id) {}

    typedef R ( WINAPI* TProc)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
         P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,P12 p12, P13 p13, P14 p14,
         P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20);
    R operator ()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9,
         P10 p10, P11 p11, P12 p12, P13 p13, P14 p14,P15 p15, P16 p16, P17 p17, 
         P18 p18, P19 p19, P20 p20){
      return ((TProc)Proc)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, 
                           p13,p14,p15,p16,p17,p18,p19,p20);
    }
};

//----------------------------------------------------------------------------
// System DLL Wrappers
//


//
// class TUser 
// ~~~~~ ~~~~~
// delay loading USER :), just for some memory/resource check programs
class _OWLCLASS TUser {
  public:
    static  HICON      LoadIcon(HINSTANCE, LPCTSTR);
    static  BOOL      DestroyIcon(HICON);
    static  BOOL      GetClassInfo(HINSTANCE, LPCTSTR, LPWNDCLASS);
    static  int        GetMenuString(HMENU, UINT, LPTSTR, int, UINT);
    static  UINT      GetMenuState(HMENU, UINT, UINT);
    static  TModule&  GetModule();
};

//
// class TVersion 
// ~~~~~ ~~~~~~~~
// delay loading VERSION.DLL
class _OWLCLASS TVersion {
  public:
    static  BOOL  GetFileVersionInfo(LPTSTR,DWORD,DWORD,LPVOID);
    static  DWORD  GetFileVersionInfoSize(LPTSTR, LPDWORD);
    static  BOOL  VerQueryValue(const LPVOID,LPTSTR,LPVOID,uint *);
    static  DWORD VerLanguageName(DWORD,LPTSTR,DWORD);
    static  TModule&  GetModule();
};

#if !defined(BI_SUPPRESS_OLE)
//
// class TOle 
// ~~~~~ ~~~~~~~~
// delay loading Ole32.DLL/COMPOBJ.DLL
class _OWLCLASS TOle {
  public:
    static  HRESULT    CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
    static  TModule&  GetModule();
};

//
// class TOleAuto 
// ~~~~~ ~~~~~~~~
// delay loading OLEAUT32.DLL/OLE2DISP.DLL
class _OWLCLASS TOleAuto {
  public:
    static  HRESULT    SysFreeString(BSTR);
    static  UINT      SysStringLen(BSTR);
    static  BSTR      SysAllocString(const OLECHAR *);
    static  TModule&  GetModule();
};
#endif // BI_SUPPRESS_OLE

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementation
//

//
// Return the name of the module.
//
inline LPCTSTR TModule::GetName() const
{
  return Name;
}

//
// Return the instance handle of the library module represented by the
// TModule obect.
//
inline TModule::THandle TModule::GetHandle() const
{
  return Handle;
}

//
// Operator returning the instance handle of the library module represented
// by the TModule obect.
//
inline TModule::operator TModule::THandle() const
{
  return GetHandle();
}

//
// Return true if the handles of the modules are identical.
//
inline bool TModule::operator ==(const TModule& m) const
{
  return GetHandle() == m.GetHandle();
}

//
// Return true if the module has been successfully loaded.
//
inline bool TModule::IsLoaded() const
{
  return GetHandle() > HINSTANCE(HINSTANCE_ERROR);
}


#if defined(OWL2_COMPAT)
//
// Return the handle of the module.
//
inline TModule::THandle TModule::GetInstance() const
{
  return GetHandle();
}

//
// Set the handle of the module.
//
inline void TModule::SetInstance(TModule::THandle handle)
{
  SetHandle(handle);
}
#endif

#if defined(OWL1_COMPAT)
//
inline TWindow* TModule::ValidWindow(TWindow* win)
{
  return win;
}

//
inline bool TModule::LowMemory()
{
  return false;
}

//
inline void TModule::RestoreMemory()
{
}
#endif  //#if defined(OWL1_COMPAT)

//
// Return the full path of the location of the module.
//
inline int TModule::GetModuleFileName(LPTSTR buff, int maxChars)
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return ::GetModuleFileName(Handle, buff, maxChars);
}

//
// Return the function address of a module.
//
inline FARPROC TModule::GetProcAddress(LPCSTR fcnName) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return ::GetProcAddress(Handle, fcnName);
}


//
// Wrapper for the Windows API to find a particular resource.
//
inline HRSRC TModule::FindResource(TResId id, LPCTSTR type) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return ::FindResource(Handle, id, type);
}

//
// Wrapper for the Windows API to find a particular resource.
//
inline HRSRC TModule::FindResourceEx(TResId id, LPCTSTR type, TLangId langId) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
//  return ::FindResourceEx(Handle, id, type, langId);  // The order of parameters is reversed
  return ::FindResourceEx(Handle, type, id, langId);
}
//
// Wrapper for the Windows API.
//
inline HGLOBAL TModule::LoadResource(HRSRC hRsrc) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return ::LoadResource(Handle, hRsrc);
}

//
// Wrapper for the Windows API.
//
inline uint32 TModule::SizeofResource(HRSRC hRsrc) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return ::SizeofResource(Handle, hRsrc);
}


//
// Wrapper for the Windows API.
//
inline bool TModule::GetClassInfo(LPCTSTR name, WNDCLASS * wndclass) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
  return TUser::GetClassInfo(Handle, name, wndclass);
}


//
// Wrapper for the Windows API.
//
inline HICON TModule::CopyIcon(HICON hIcon) const
{
  return ::CopyIcon(hIcon);
}

//
// Return the version information about this module.
//
inline VS_FIXEDFILEINFO & TModuleVersionInfo::GetFixedInfo()
{
  PRECONDITION(FixedInfo);
  return *FixedInfo;
}

//
inline uint32 TModuleVersionInfo::GetSignature() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwSignature;
}

//
inline uint32 TModuleVersionInfo::GetStrucVersion() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwStrucVersion;
}

//
// Get the major file version (first 32-bits).
//
inline uint32 TModuleVersionInfo::GetFileVersionMS() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwFileVersionMS;
}

//
// Get the minor file version (last 32-bits).
//
inline uint32 TModuleVersionInfo::GetFileVersionLS() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwFileVersionLS;
}

//
// Get the major product version number (first 32-bits).
//
inline uint32 TModuleVersionInfo::GetProductVersionMS() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwProductVersionMS;
}

//
// Get the minor product version number (last 32-bits).
//
inline uint32 TModuleVersionInfo::GetProductVersionLS() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwProductVersionLS;
}

//
// Return true if the flag has been set in the version info.
//
inline bool TModuleVersionInfo::IsFileFlagSet(uint32 flag) const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlagsMask & flag) && (FixedInfo->dwFileFlags & flag);
}

//
inline uint32 TModuleVersionInfo::GetFileFlagsMask() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwFileFlagsMask;
}

//
inline uint32 TModuleVersionInfo::GetFileFlags() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwFileFlags;
}

//
inline bool TModuleVersionInfo::IsDebug() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_DEBUG) ?
         true : false;
}

//
inline bool TModuleVersionInfo::InfoInferred() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_INFOINFERRED) ?
          true : false;
}

//
inline bool TModuleVersionInfo::IsPatched() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_PATCHED) ?
          true : false;
}

//
inline bool TModuleVersionInfo::IsPreRelease() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_PRERELEASE) ?
          true : false;
}

//
inline bool TModuleVersionInfo::IsPrivateBuild() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_PRIVATEBUILD) ?
          true : false;
}

//
inline bool TModuleVersionInfo::IsSpecialBuild() const
{
  PRECONDITION(FixedInfo);
  return (FixedInfo->dwFileFlags & FixedInfo->dwFileFlagsMask & VS_FF_SPECIALBUILD) ?
          true : false;
}

// returns TFileOS values
inline uint32 TModuleVersionInfo::GetFileOS() const
{
  PRECONDITION(FixedInfo);
  return FixedInfo->dwFileOS;
}

//
inline TModuleVersionInfo::TFileType TModuleVersionInfo::GetFileType() const
{
  PRECONDITION(FixedInfo);
  return (TFileType)FixedInfo->dwFileType;
}

//
// Return the language id of this module.
//
inline uint TModuleVersionInfo::GetLanguage() const
{
  return uint(Lang);
}

//
// Return the language name of this module.
//
inline owl_string TModuleVersionInfo::GetLanguageName() const
{
  return GetLanguageName(GetLanguage());
}
inline const owl_string& TSystemMessage::SysMessage() const {
  return Message;
}
inline const uint32 TSystemMessage::SysError() const {
  return Error;
}
//
// Construct a TErrorMode object which invokes the 'SetErrorMode' API
// function to control how/whether Windows handles interrupt 24h errors.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TErrorMode::TErrorMode(uint mode)
{
  PrevMode = ::SetErrorMode(mode);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Destructor of TErrorMode object - restore the state of the error mode
// saved during construction of the object.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TErrorMode::~TErrorMode()
{
  ::SetErrorMode(PrevMode);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

__OWL_END_NAMESPACE

//
// Get this header to get GetApplicationObject() for backward compatibility
// with Owl 2.0
//
#if defined(OWL2_COMPAT) && !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif



#endif  // OWL_MODULE_H
