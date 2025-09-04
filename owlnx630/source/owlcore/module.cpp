//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.33.2.4 $
//$Author: jogybl $
//$Date: 2010-03-24 11:54:28 $
//
// Implementation of class TModule. TModule defines the base behavior for
// OWL libraries and applications.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_POINTER_H)
# include <owl/pointer.h>
#endif
#include <stdio.h>
# include <winnls.h>
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if defined(BI_COMP_MSC)
#include <new.h>
#endif
#if defined(BI_COMP_MSC) && (defined(__DEBUG) || defined(_DEBUG))
#   include <owl/StackWalker.h>
#endif


long TlsAddRefs();
long TlsRelease();

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlApp);        // General Application diagnostic group

//
// Global message id for GetWindowPtr call defined in owl.cpp
//
extern _OWLDATA(uint) GetWindowPtrMsgId;

__OWL_END_NAMESPACE

__OWL_USING_STD_NAMESPACE

#if !defined(SECTION) || SECTION == 1

__OWL_BEGIN_NAMESPACE

//
// TSystemMessage
// ~~~~~~~~~~~~~~
//
TSystemMessage::TSystemMessage()
{
  Error = ::GetLastError();
  Init(LangNeutral);
}

//
//
//
TSystemMessage::TSystemMessage(uint32 error, TLangId langId)
{
  Error = error;
  ::SetLastError(Error);
  Init(langId);
}

//
//
//
int TSystemMessage::MessageBox(TWindow* win, LPCTSTR msg, LPCTSTR title, uint flags)
{
  flags |= MB_SYSTEMMODAL;
  
  owl_string message = msg ? msg : _T("");
  message  += Message;
  TApplication* appl;
  if(!win && (appl = OWLGetAppDictionary().GetApplication(0))!=0 && appl->GetMainWindow())
    win = (TWindow*)appl->GetMainWindow();
  if(win)
    return win->MessageBox(message.c_str(), title, flags);
  return ::MessageBox(0, message.c_str(), title, flags);
}

//
//
//
void TSystemMessage::Init(TLangId langId)
{
  LPVOID ptr = 0;
  if(::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, Error, langId, (LPTSTR)&ptr, 0, NULL) != 0)
  {
    Message = (LPTSTR)ptr;
    LocalFree(ptr); // free the memory allocated by FormatMessage
  }
  else{
    _TCHAR buffer[MAX_PATH];
    wsprintf(buffer, _T("Error: %#X occurred (no further info)"), Error);
    Message = buffer;
  }
}
//----------------------------------------------------------------------------

//__OWL_END_NAMESPACE

// Implement
//////////////////////////////////////////////////////////////////////////////////////////////////
// multithread support
class __TModuleList {
  public:
    static TModule*  Next(TModule* module);
    static bool      Find(TModule* module);
    static void      Add(TModule* module);
    static void      Remove(TModule* module);
    static TModule*  FindResModule(TResId id, LPCTSTR type);

    static TPtrArray<TModule*>* ModuleArray;

#if defined(BI_MULTI_THREAD_RTL)
    static TMRSWSection*  Lock;
#endif

};

TPtrArray<TModule*>* __TModuleList::ModuleArray = 0;
#if defined(BI_MULTI_THREAD_RTL)
TMRSWSection*  __TModuleList::Lock = 0;
#endif

#if defined(BI_MULTI_THREAD_RTL)
#define LOCKLIST(s) TMRSWSection::TLock __lock(*__TModuleList::Lock, s)
#else
#define LOCKLIST(s)
#endif
//
TModule* __TModuleList::Next(TModule* module)
{
  if(!ModuleArray)
    return 0;
  LOCKLIST(true);
  if(!module){
    return (*ModuleArray)[0];
  }
  else{
    for(uint i = 0; i < ModuleArray->Size(); i++){
      if(module == (*ModuleArray)[i])
        return (i+1 < ModuleArray->Size()) ? (*ModuleArray)[i+1] : 0;
    }
  }
  return 0;
}
//
void __TModuleList::Add(TModule* module)
{
  if(Find(module))
    return;

  if(!ModuleArray)
  {
#if defined(BI_MULTI_THREAD_RTL)
    Lock = new TMRSWSection;
#endif
    ModuleArray = new TPtrArray<TModule*>;
  }
  {
    LOCKLIST(false);
    ModuleArray->AddAt(module,0);
  }
}
//
void __TModuleList::Remove(TModule* module)
{
  if(!Find(module))
    return;

  PRECONDITION(ModuleArray);
  {
    LOCKLIST(false);
    ModuleArray->DetachItem(module);
    if(ModuleArray->Empty())
	{
      delete ModuleArray;
      ModuleArray = 0;
    }
  }
#if defined(BI_MULTI_THREAD_RTL)
  if(!ModuleArray){
    delete Lock;
    Lock = 0;
  }
#endif
}
//
bool __TModuleList::Find(TModule* module)
{
  if(!ModuleArray)
    return false;
  LOCKLIST(true);
  
  for(uint i = 0; i < ModuleArray->Size(); i++){
    if(module == (*ModuleArray)[i])
      return true;
  }
  return false;
}
//
TModule* __TModuleList::FindResModule(TResId id, LPCTSTR type)
{
  if(!ModuleArray)
    return 0;

  LOCKLIST(true);

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  TLangId langId = appl ? appl->GetLangId() : LangNeutral;
  TPtrArray<TModule*>::Iterator Iter1(*ModuleArray);
  while(Iter1){
    if((*Iter1)->FindResourceEx(id, type, langId))
      return *Iter1;
    Iter1++;
  }

  TPtrArray<TModule*>::Iterator Iter(*ModuleArray);
  while(Iter){
    if((*Iter)->FindResource(id, type))
      return *Iter;
    Iter++;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////
//
TModule* TModule::NextModule(TModule* module)
{
  return __TModuleList::Next(module);
}

//__OWL_BEGIN_NAMESPACE

//
// Implementation of Constructors for a TModule object
//

//
// Construct a TModule that is an alias for a DLL. TModule will load & free
// the DLL if "shouldLoad" is true. If "shouldLoad" is false, then the Handle
// must be set some time later using InitModule().
// "mustLoad" determines if a load failure should cause an exception throw
//
TModule::TModule(LPCTSTR name, bool shouldLoad, bool mustLoad, bool addToList)
:
  Name(0)
{
#if defined(OWL1_COMPAT)
  lpCmdLine = 0;
#endif
  if (shouldLoad) {
    TErrorMode loadMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
    Handle = ::LoadLibrary(name);
    if (Handle <= HINSTANCE(HINSTANCE_ERROR) && mustLoad) {
#if !defined(BI_COMP_GNUC)
# pragma warn -ccc
#endif
      TRACEX(OwlApp, 0, _T("Unable to load DLL '") << owl_string(name).c_str() << _T('\''));
#if !defined(BI_COMP_GNUC)
# pragma warn .ccc
#endif
      TXInvalidModule::Raise(name);  //!CQ use a better exception. This is for windows
    }
  }
  else {
    Handle = 0;
  }

  ShouldFree = shouldLoad;
  SetName(name);

  //
  // Add Module  into global Array
  //
  if(addToList)
    __TModuleList::Add(this);
}

//
// Construct a TModule that is an alias for an already loaded DLL or program
// with an Handle available. Name is optional & can be 0. No cmdLine is
// setup
//
TModule::TModule(LPCTSTR name, HINSTANCE hInstance, bool addToList)
:
  Name(0)
{
  PRECONDITION(hInstance > HINSTANCE(HINSTANCE_ERROR));
  Handle = hInstance;
  ShouldFree = false;
#if defined(OWL1_COMPAT)
  lpCmdLine = 0;
#endif
  SetName(name);

  //
  // Add Module  into global Array
  //
  if(addToList)
    __TModuleList::Add(this);
}

//
// Construct a TModule for an Owl Program via TApplication. InitModule is
// called from here to initialize Handle & the CmdLine
//
TModule::TModule(LPCTSTR name, HINSTANCE hInstance, LPCTSTR cmdLine, bool addToList)
:
  Name(0)
{
  Handle = 0;
  ShouldFree = false;
#if defined(OWL1_COMPAT)
  lpCmdLine = 0;
#endif
  SetName(name);
  if (hInstance)
    InitModule(hInstance, cmdLine);

  //
  // Add Module  into global Array
  //
  if(addToList)
    __TModuleList::Add(this);
}

//
// Destruct a TModule, freeing the instance if appropriate, and deleting
// new'd strings
//
TModule::~TModule()
{
  if (ShouldFree && Handle > HINSTANCE(HINSTANCE_ERROR))
    ::FreeLibrary(Handle);
#if defined(OWL1_COMPAT)
  delete[] lpCmdLine;
#endif
  delete[] Name;

  //
  // Detach Module from global ModuleArray
  //
  __TModuleList::Remove(this);
}

//
// Global  search for  resources
//
TModule*
TModule::FindResModule(TResId id, LPCTSTR type)
{
  return __TModuleList::FindResModule(id, type);
}

//
//
//
void
TModule::SetName(LPCTSTR name)
{
  delete[] Name;
  TAPointer<TCHAR> moduleName;
  if (Handle > HINSTANCE(HINSTANCE_ERROR) && !name) {
    moduleName = new TCHAR[_MAX_PATH];
    if (GetModuleFileName(moduleName, _MAX_PATH))
      name = moduleName;
  }
  Name = strnewdup(name);
}

//
// Perform initialization of modules cmd line copy, and get proc
// instance handles for the standard procs.
//
void
#if defined(OWL1_COMPAT)
TModule::InitModule(HINSTANCE hInstance, LPCTSTR cmdLine)
#else
TModule::InitModule(HINSTANCE hInstance, LPCTSTR /*cmdLine*/)
#endif
{
  SetHandle(hInstance);

#if defined(BI_COMP_GNUC)
  // Borland removes the first element of argv (program name) from
  // OWL::CmdLine somehow ("the RTL takes care of this" above), but
  // g++ does not. Modifying the cmdLine argument won't have any effect
  // because it's const and is passed as string::c_str().
  char const* cmdLine = TApplication::GetCmdLine().c_str();
  if(cmdLine){
    while (*cmdLine && *cmdLine++ != _T(' '))
      ;
    TApplication::GetCmdLine() = cmdLine;
  }
#endif

#if defined(OWL1_COMPAT)
  if (cmdLine)
    lpCmdLine = strnewdup(cmdLine);
#endif

  // Register a system-wide "GetWindowPtr" message as GetWindowPtr(hInstance)
  // Each running copy of ObjectWindows will get a unique message Id
  // instance of OWL library. !!!
  if (!GetWindowPtrMsgId) {
    const TCHAR msgTemplate[] = _T("GetWindowPtr(%X)");
    TCHAR msgName[COUNTOF(msgTemplate) + 8];
    wsprintf(msgName, msgTemplate, unsigned(hInstance));
    GetWindowPtrMsgId = ::RegisterWindowMessage(msgName);
    CHECK(GetWindowPtrMsgId);
  }
}

//
// Replaceable exception handler; may be redefined to process OWL exceptions
// if canResume is false, then the user doesn't have the option of ignoring
//
int
TModule::Error(
#if defined(BI_STD_EXCEPTIONS)
  std::exception& x,
#else
  XBASE& x,
#endif
  unsigned captionResId, unsigned promptResId)
{
  TCHAR cbuf[80];
  TCHAR pbuf[80];

  if (!captionResId)
    captionResId = IDS_UNHANDLEDXMSG;
  return HandleGlobalException(x,
    LoadString(captionResId, cbuf, 80) ? cbuf : 0,
    promptResId ?
      (LoadString(promptResId, pbuf, 80) ? pbuf : _T("OK to Resume?"))
      : 0);
}

//
// Set the instance handle for a module that does not yet have one. Cannot
// be called on a module that already has an instance handle.
//
void
TModule::SetHandle(HINSTANCE hInstance)
{
  PRECONDITION(!ShouldFree && !Handle);
  Handle = hInstance;
}

//
// LoadString replacements which do not generated debug warning output
//
 typedef WCHAR* TResText;
 typedef WCHAR* TResCount;

//
// Wrapper for loading a string resource.
//
int
TModule::LoadString(uint id, LPTSTR buf, int bufSize) const
{
#ifdef MAINWIN
  int resource = ::LoadString( GetInstance(), id, buf, bufSize);
  if (resource)
    return resource;
#else //! MAINWIN
  uint len = 0;
  HRSRC     resHdl;
  HGLOBAL   glbHdl;
  TResText  resData;
  
  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id/16+1, RT_STRING, appl ? appl->GetLangId() : LangNeutral);
  if(!resHdl)
    resHdl = FindResource(id/16+1, RT_STRING);
  if(resHdl != 0){
    if((glbHdl = LoadResource(resHdl)) != 0) {
      if ((resData = (TResText)LockResource(glbHdl)) != 0) {
        // get pointer to our string
        int cnt;
        for (cnt = id % 16; len = *(TResCount)resData++, cnt--; resData += len)
          ;
        if (len != 0) {
#  if defined(UNICODE)
          if (len >= uint(bufSize))
            len = bufSize-1;
          wmemset(buf, 0, bufSize);
          ::_tcsncpy(buf, resData, len);
          //::_tcscpy(buf, resData); // Ma, 09.10.2003 string too long
#  else
          len = ::WideCharToMultiByte(CP_ACP, 0, resData, len, buf, bufSize, 0, 0);
          if (len > 0)
            buf[len] = 0;
#  endif
        }
#if !defined WINELIB  //this win32 un-needed function is directly removed from Winelib implementation
        UnlockResource(glbHdl);
#endif
      }
      FreeResource(glbHdl);
      if (len)
        return len;
    }
  }
  else{
    // if not found look in Module List
    TModule* module = TModule::FindResModule(id/16+1, RT_STRING);
    if (module)
      return module->LoadString(id, buf, bufSize);
  }
#endif //else not MAINWIN
  if (bufSize)
    *buf = 0;  // make empty string just in case caller doesn't check return
  return 0;    // indicate string not found
}

//
// Wrapper for loading a string resource.
// Returns a string object.
//
#ifdef UNIX
owl_string
TModule::LoadString(uint id) const
{

// Added by Val Ovechkin 7:18 PM 6/11/98
  char str[1024] = "<empty>";
  int res = ::LoadString( GetInstance(), id, str, sizeof str );
  if( !res ) str[0] = 0;
  return owl_string( str );


  uint len = 0;
  HRSRC     resHdl;
  HGLOBAL   glbHdl;
  TResText  resData;
  owl_string    retString;

  if ((resHdl = FindResource(id/16+1, RT_STRING)) != 0
   && (glbHdl = LoadResource(resHdl)) != 0) {
    if ((resData = (TResText)LockResource(glbHdl)) != 0) {
      int cnt;
      for (cnt = id % 16; len = *(TResCount)resData++, cnt--; resData += len)
        ;
      if (len != 0) {
#if (0) // This is dangerous unless string is changed to handle non-terminated
        // char arrays
        //
        retString.append(resData, 0, len);
#else
        int n = ::WideCharToMultiByte(CP_ACP, 0, resData, len, 0, 0, 0, 0);
        TAPointer<char> buf = new char[n+1];
        len = ::WideCharToMultiByte(CP_ACP, 0, resData, len, buf, n+1, 0, 0);
  #if   0
        retString.resize(len+1);
        uint i;
        for (i = 0; i < len; i++)
          retString[i] = (char)buf[i];
        retString[i] = 0;
  #else
        // The string object does not need the terminating null of C-strings
        //
        retString.resize(len);
        uint i;
        for (i = 0; i < len; i++)
          retString[i] = (char)buf[i];
  #endif
#endif
      }
#if !defined WINELIB  //this win32 un-needed function is directly removed from Winelib implementation
      UnlockResource(glbHdl);
#endif
    }
    FreeResource(glbHdl);
    if (len)
      return retString;
  }

#if defined(WINELIB)
  if (gModule != this)                   // look in OWL module if different
    return gModule->LoadString(id);
#else
  if (::Module != this)                   // look in OWL module if different
    return ::Module->LoadString(id);
#endif

  return retString;    // empty if string not found
}

#else

owl_string
TModule::LoadString(uint id) const
{
  int len    = 0;
  HRSRC     resHdl;
  HGLOBAL   glbHdl;
  TResText  resData;
   owl_string retString;

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id/16+1, RT_STRING, appl ? appl->GetLangId() : LangNeutral);
  if(!resHdl)
    resHdl = FindResource(id/16+1, RT_STRING);
  if (resHdl != 0){
    if((glbHdl = LoadResource(resHdl)) != 0){
      if ((resData = (TResText)LockResource(glbHdl)) != 0) {
        int cnt;
        for (cnt = id % 16; len = *(TResCount)resData++, cnt--; resData += len)
          ;
        if (len != 0) {
#if (0)   // This is dangerous unless string is changed to handle non-terminated
          // char arrays
          retString.append(resData, 0, len);
#else
#    if defined(UNICODE)
          // The string object does not need the terminating null of C-strings
          retString.resize(len); // Ma, 09.10.2003 string is not terminated!

          for (int i = 0; i < len; i++)
            retString[i] = resData[i];
#    else
          int n = ::WideCharToMultiByte(CP_ACP, 0, resData, len, 0, 0, 0, 0);
          TAPointer<_TCHAR> __ClnObj = new _TCHAR[n+1];
          _TCHAR* buf = __ClnObj;
          len = ::WideCharToMultiByte(CP_ACP, 0, resData, len, buf, n+1, 0, 0);
          // The string object does not need the terminating null of C-strings
          retString.resize(len);
          int i;
          for (i = 0; i < len; i++)
            retString[i] = buf[i];
#    endif
#endif
        }
#if !defined WINELIB  //this win32 un-needed function is directly removed from Winelib implementation
        UnlockResource(glbHdl);
#endif
      }
      FreeResource(glbHdl);
      if (len)
        return retString;
    }
  }
  else{
    // if not found look in Module List
    TModule* module = TModule::FindResModule(id/16+1, RT_STRING);
    if (module)
      return module->LoadString(id);
  }

  return retString;    // empty if string not found
}
#endif



//
// Wrapper for the Windows API.
//
HBITMAP TModule::LoadBitmap(TResId id) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
#if 0
  HRSRC    resHdl;
  HGLOBAL glbHdl;
  HBITMAP hBitmap=0;

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id, RT_BITMAP, appl ? appl->GetLangId() : LangNeutral);
  
  if(resHdl){
    if((glbHdl = LoadResource(resHdl)) !=0)
      hBitmap = (HBITMAP)LockResource(glbHdl);
  }
#else
  HBITMAP hBitmap = ::LoadBitmap(Handle, id);
#endif
  WARNX(OwlApp, !hBitmap, 0, _T("Bitmap resource not found Id:") << id);

  if(!hBitmap){// if not found look in Module List
     TModule* module = TModule::FindResModule(id, RT_BITMAP);
     if(module)
       return module->LoadBitmap(id);
  }

  CHECKX(hBitmap,_T("Bitmap resource not found even in module list."));
  return hBitmap;
}
//
// Wrapper for the Windows API.
//
HACCEL TModule::LoadAccelerators(TResId id) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));

#if 0  
  HRSRC resHdl;
  HGLOBAL glbHdl;

  HACCEL hAccel = 0;

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id, RT_ACCELERATOR, appl ? appl->GetLangId() : LangNeutral);
  if(!resHdl)
    resHdl = FindResource(id, RT_ACCELERATOR);
  if(resHdl){
    if((glbHdl = LoadResource(resHdl)) !=0)
      hAccel = (HACCEL)LockResource(glbHdl);
  }
#else  
  HACCEL hAccel = ::LoadAccelerators(Handle, id);
#endif  
  WARNX(OwlApp,!hAccel,0,_T("Accelerator resource not found") << id);

  if(!hAccel){ // if not found look in Module List
    TModule* module = TModule::FindResModule(id, RT_ACCELERATOR);
     if(module)
       return module->LoadAccelerators(id);
  }

  WARNX(OwlApp,!hAccel,0,_T("Accelerator resource not found even in module list") << id);
  //CHECKX(hAccel,_T("Accelerator resource not found even in module list."));

  return hAccel;
}
//----------------------------------------------------------------------------

//
// Wrapper for the Windows API.
//
HMENU TModule::LoadMenu(TResId id) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
#if 0
  HRSRC resHdl;
  HGLOBAL glbHdl;
   HMENU hMenu = 0;
  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id, RT_MENU, appl ? appl->GetLangId() : LangNeutral);
  if(!resHdl)
    resHdl = FindResource(id, RT_MENU);
  if(resHdl){
    if((glbHdl = LoadResource(resHdl)) !=0)
      hMenu = (HMENU)LockResource(glbHdl);
  }
#else  
  HMENU hMenu = ::LoadMenu(Handle, id);
#endif
  WARNX(OwlApp,!hMenu,0,_T("Menu resource not found") << id);

  if (!hMenu){ // if not found look in Module List
    TModule* module = TModule::FindResModule(id, RT_MENU);
     if(module)
       return module->LoadMenu(id);
  }

  CHECKX(hMenu,_T("Menu resource not found even in module list."));
  return hMenu;
}
//----------------------------------------------------------------------------

//
// Wrapper for the Windows API.
//
HCURSOR TModule::LoadCursor(TResId id) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
#if 0
  HRSRC resHdl;
  HGLOBAL glbHdl;
   HCURSOR hCursor = 0;

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(id, RT_CURSOR, appl ? appl->GetLangId() : LangNeutral);
  
  if(resHdl){
    if((glbHdl = LoadResource(resHdl)) !=0)
      hCursor = (HCURSOR)LockResource(glbHdl);
  }
#else
  HCURSOR hCursor = ::LoadCursor(Handle, id);
#endif
  WARNX(OwlApp,!hCursor,0,_T("Cursor resource not found") << id);

  if (!hCursor){ // if not found look in Module List
    TModule* module = TModule::FindResModule(id, RT_CURSOR);
     if(module)
       return module->LoadCursor(id);
  }

  CHECKX(hCursor,_T("Cursor resource not found even in module list."));
  return hCursor;
}

//----------------------------------------------------------------------------
//
// Wrapper for the Windows API.
//
HICON TModule::LoadIcon(LPCTSTR name) const
{
  PRECONDITION(Handle > HINSTANCE(HINSTANCE_ERROR));
#if 0
  HRSRC resHdl;
  HGLOBAL glbHdl;
   HICON hIcon = 0;

  TApplication* appl = OWLGetAppDictionary().GetApplication(0);
  resHdl = FindResourceEx(TResId(name), RT_ICON, appl ? appl->GetLangId() : LangNeutral);

  if(resHdl){
    if((glbHdl = LoadResource(resHdl)) !=0)
      hIcon = (HICON)LockResource(glbHdl);
  }
#else
  HICON hIcon = ::LoadIcon(Handle, name);
#endif
  WARNX(OwlApp,!hIcon,0,_T("Icon resource not found") << name);
  
  if (!hIcon){ // if not found look in Module List
    TModule* module = TModule::FindResModule(name, RT_ICON);
     if(module)
       return module->LoadIcon(name);
  }

  CHECKX(hIcon,_T("Icon resource not found even in module list"));
  return hIcon;
}
//----------------------------------------------------------------------------

//
// Obsolete error handler--use Error(XBASE&,...) instead
//
#if defined(OWL1_COMPAT)
void
TModule::Error(int errorCode)
{
  TCHAR     errorStr[51];
  TModule*  module = GetApplicationObject();

#if BI_MSG_LANGUAGE == 0x0411
  sprintf(errorStr,
           "ґЧ°‚рЋуђM‚µ‚Ь‚µ‚Ѕ: ґЧ°є°ДЮ = %d\n‘±Ќs‚µ‚Ь‚·‚©?",
           errorCode);
#else
  wsprintf(errorStr,
           _T("Error received: error code = %d\nOK to proceed?"),
           errorCode);
#endif

  if (::MessageBox(0, errorStr, module ? module->GetName() : Name,
                   MB_ICONSTOP | MB_YESNO | MB_TASKMODAL) == IDNO)
  ::PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
}
#endif

//----------------------------------------------------------------------------
// Module entry dynamic binding base class

//
// Construct a Module entry object from a function name string or ordinal
//
TModuleProc::TModuleProc(const TModule& module, LPCSTR id)
{
  Proc = module.GetProcAddress(id);
  CHECK(Proc);
  if (!Proc) {
    owl_string msg;
    if (HiUint16(uint32(id)) != 0)
      msg = TXOwl::MakeMessage(IDS_INVALIDMODULEFCN, id);
    else
      msg = TXOwl::MakeMessage(IDS_INVALIDMODULEORD, LoUint16(uint32(id)));
    msg += module.GetName();  // Just tack on the module name...
    TXOwl::Raise(msg);
  }
}


//----------------------------------------------------------------------------
// Wrappers for Windows API
//

#if defined(UNICODE) && defined(USE_UNICOWS)
static const _TCHAR MSLUStr[] = _T("UNICOWS");

TModule&
GetModuleMSLU()
{
  static TModule MSLUModule(MSLUStr, ::GetModuleHandle(MSLUStr),false);
  return MSLUModule;
}
#endif


  static const _TCHAR userStr[] = _T("USER32");
# if defined(UNICODE)
    static const char LoadIconStr[] = "LoadIconW";
    static const char GetClassInfoStr[] = "GetClassInfoW";
    static const char GetMenuStringStr[]= "GetMenuStringW";
# else
    static const char LoadIconStr[] = "LoadIconA";
    static const char GetClassInfoStr[] = "GetClassInfoA";
    static const char GetMenuStringStr[]= "GetMenuStringA";
# endif

static const char GetMenuStateStr[] = "GetMenuState";
static const char DestroyIconStr[] = "DestroyIcon";

//
// Returns TModule object wrapping the handle of the USER module
//
TModule&
TUser::GetModule()
{
  static TModule userModule(userStr, ::GetModuleHandle(userStr),false);
  return userModule;
}

//
// Invokes 'LoadIcon' indirectly
//
HICON
TUser::LoadIcon(HINSTANCE p1, LPCTSTR p2)
{
  static TModuleProc2<HICON, HINSTANCE, LPCTSTR>
#if defined(UNICODE) && defined(USE_UNICOWS)
         loadIcon(GetModuleMSLU(), LoadIconStr);
#else
         loadIcon(GetModule(), LoadIconStr);
#endif
  return loadIcon(p1, p2);
}

//
// Invokes 'DestroyIcon' indirectly
//
BOOL
TUser::DestroyIcon(HICON p1)
{
  static TModuleProc1<BOOL, HICON> destroyIcon(GetModule(), DestroyIconStr);
  return destroyIcon(p1);
}

//
// Invokes 'GetClassInfo' indirectly
//
BOOL
TUser::GetClassInfo(HINSTANCE p1, LPCTSTR p2, LPWNDCLASS p3)
{
  static TModuleProc3<BOOL, HINSTANCE, LPCTSTR, LPWNDCLASS>
#if defined(UNICODE) && defined(USE_UNICOWS)
         getClassInfo(GetModuleMSLU(), GetClassInfoStr);
#else
         getClassInfo(GetModule(), GetClassInfoStr);
#endif
  return getClassInfo(p1, p2, p3);
}

//
// Invokes 'GetMenuString' indirectly
//
int
TUser::GetMenuString(HMENU p1, UINT p2, LPTSTR p3, int p4, UINT p5)
{
  static TModuleProc5<int, HMENU, uint, LPTSTR, int, int>
#if defined(UNICODE) && defined(USE_UNICOWS)
         getMenuString(GetModuleMSLU(), GetMenuStringStr);
#else
         getMenuString(GetModule(), GetMenuStringStr);
#endif
  return getMenuString(p1, p2, p3, p4, p5);
}

//
// Invokes 'GetMenuState' indirectly
//
UINT
TUser::GetMenuState(HMENU p1, UINT p2, UINT p3)
{
  static TModuleProc3<UINT, HMENU, UINT, UINT>
         getMenuState(GetModule(), GetMenuStateStr);
  return getMenuState(p1, p2, p3);
}

/////////////////////////////////////////////////////////////
// VERSION.DLL or VER.DLL 
  static const _TCHAR verStr[] = _T("VERSION.DLL");
# if defined(UNICODE)
    static const char GetFileVersionInfoStr[] = "GetFileVersionInfoW";
    static const char GetFileVersionInfoSizeStr[] = "GetFileVersionInfoSizeW";
    static const char VerQueryValueStr[]= "VerQueryValueW";
    static const char VerLanguageNameStr[]  = "VerLanguageNameW";
# else
    static const char GetFileVersionInfoStr[] = "GetFileVersionInfoA";
    static const char GetFileVersionInfoSizeStr[] = "GetFileVersionInfoSizeA";
    static const char VerQueryValueStr[]= "VerQueryValueA";
    static const char VerLanguageNameStr[]  = "VerLanguageNameA";
# endif

//
// Returns TModule object wrapping the handle of the VERSION.DLL module
//
TModule&
TVersion::GetModule()
{
  static TModule verModule(verStr, true, true, false);
  return verModule;
}

//
// Invokes 'GetFileVersionInfo' indirectly
//
BOOL
TVersion::GetFileVersionInfo(LPTSTR p1, DWORD p2, DWORD p3, LPVOID p4)
{
  static TModuleProc4<BOOL,LPTSTR,DWORD,DWORD,LPVOID>
#if defined(UNICODE) && defined(USE_UNICOWS)
         getFileVersionInfo(GetModuleMSLU(), GetFileVersionInfoStr);
#else
         getFileVersionInfo(GetModule(), GetFileVersionInfoStr);
#endif
  return getFileVersionInfo(p1, p2, p3, p4);
}

//
// Invokes 'GetFileVersionInfoSize' indirectly
//
DWORD      
TVersion::GetFileVersionInfoSize(LPTSTR p1, LPDWORD p2)
{
  static TModuleProc2<DWORD,LPTSTR,LPDWORD>
#if defined(UNICODE) && defined(USE_UNICOWS)
         getFileVersionInfoSize(GetModuleMSLU(), GetFileVersionInfoSizeStr);
#else
         getFileVersionInfoSize(GetModule(), GetFileVersionInfoSizeStr);
#endif
  return getFileVersionInfoSize(p1, p2);
}

//
// Invokes 'VerQueryValue' indirectly
//
BOOL      
TVersion::VerQueryValue(const LPVOID p1,LPTSTR p2,LPVOID p3, uint * p4)
{
  static TModuleProc4<BOOL,const LPVOID,LPTSTR,LPVOID,uint *>
#if defined(UNICODE) && defined(USE_UNICOWS)
         verQueryValue(GetModuleMSLU(), VerQueryValueStr);
#else
         verQueryValue(GetModule(), VerQueryValueStr);
#endif
  return verQueryValue(p1, p2, p3, p4);
}

//
// Invokes 'VerLanguageName' indirectly
//
DWORD 
TVersion::VerLanguageName(DWORD p1, LPTSTR p2, DWORD p3)
{
  static TModuleProc3<DWORD,DWORD,LPTSTR,DWORD>
#if defined(UNICODE) && defined(USE_UNICOWS)
         verLanguageName(GetModuleMSLU(), VerLanguageNameStr);
#else
         verLanguageName(GetModule(), VerLanguageNameStr);
#endif
  return verLanguageName(p1, p2, p3);
}

#if !defined(BI_SUPPRESS_OLE)
/////////////////////////////////////////////////////////////
//
//  Delay loading OLE libraries 
//
static const _TCHAR oleStr[]            = _T("OLE32.DLL");
static const _TCHAR oleAutStr[]          = _T("OLEAUT32.DLL");

static const char CoCreateInstanceStr[]  = "CoCreateInstance";
static const char SysFreeStringStr[]    = "SysFreeString";
static const char SysStringLenStr[]      = "SysStringLen";
static const char SysAllocStringStr[]    = "SysAllocString";

//
// Returns TModule object wrapping the handle of the OLE32.DLL/COMPOBJ.DLL module
//
TModule&
TOle::GetModule()
{
  static TModule oleModule(oleStr, true, true, false);
  return oleModule;
}

//
// Invokes 'CoCreateInstance' indirectly
//
HRESULT 
TOle::CoCreateInstance(REFCLSID p1, LPUNKNOWN p2, DWORD p3, REFIID p4, LPVOID* p5)
{
  static TModuleProc5<HRESULT,REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*>
         coCreateInstance(GetModule(), CoCreateInstanceStr);
  return coCreateInstance(p1, p2, p3, p4, p5);
};

//
// Returns TModule object wrapping the handle of the OLE32.DLL/COMPOBJ.DLL module
//
TModule&
TOleAuto::GetModule()
{
  static TModule oleautoModule(oleAutStr, true, true, false);
  return oleautoModule;
}

//
// Invokes 'SysFreeString' indirectly
//
HRESULT    
TOleAuto::SysFreeString(BSTR p1)
{
  static TModuleProc1<HRESULT,BSTR> 
         sysFreeString(GetModule(), SysFreeStringStr);
  return sysFreeString(p1);
}

//
// Invokes 'SysStringLen' indirectly
//
UINT      
TOleAuto::SysStringLen(BSTR p1)
{
  static TModuleProc1<UINT,BSTR> 
         sysStringLen(GetModule(), SysStringLenStr);
  return sysStringLen(p1);
}


//
// Invokes 'SysAllocString' indirectly
//
BSTR
TOleAuto::SysAllocString(const OLECHAR * p1)
{
  static TModuleProc1<BSTR,const OLECHAR *>
         sysAllocString(GetModule(), SysAllocStringStr);
  return sysAllocString(p1);
}
#endif // #if !defined(BI_SUPPRESS_OLE)
//----------------------------------------------------------------------------
//
// Exception class
//

//
// Create the Invalid Module exception.
//
TXInvalidModule::TXInvalidModule(LPCTSTR name)
:
  TXOwl(MakeMessage(IDS_INVALIDMODULE, name))
{
}

//
// Creates a copy of exception for 16-bit Windows.
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXInvalidModule*
#endif
TXInvalidModule::Clone()
{
  return new TXInvalidModule(*this);
}

//
// Throw the exception
//
void
TXInvalidModule::Throw()
{
  THROW( *this );
}

//
// Throw the exception
//
void
TXInvalidModule::Raise(LPCTSTR name)
{
  TXInvalidModule(name).Throw();
}
__OWL_END_NAMESPACE

#endif
#if !defined(SECTION) || SECTION == 2

__OWL_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// TModule streaming
//

IMPLEMENT_STREAMABLE(TModule);

#if !defined(BI_NO_OBJ_STREAMING)

//
// Extract the module object from the persistent stream.
//
void*
TModule::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TModule* o = GetObject();
  is >> (TResId&)o->Name;
#if defined(OWL1_COMPAT)
  is >> (TResId&)o->lpCmdLine;
#endif
  is >> o->ShouldFree;
  if (o->ShouldFree)
    o->Handle = ::LoadLibrary(o->Name);

  return o;
}

//
// Write the module into a persistent stream.
//
void
TModule::Streamer::Write(opstream& os) const
{
  TModule* o = GetObject();
  os << TResId(o->Name);
#if defined(OWL1_COMPAT)
  os << TResId(o->lpCmdLine);
#endif
  os << o->ShouldFree;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

__OWL_END_NAMESPACE

#endif
#if !defined(SECTION) || SECTION == 3

//----------------------------------------------------------------------------
//
// Entry (& exit) functions for Owl in a DLL
//

#if defined(_BUILDOWLDLL)

__OWL_BEGIN_NAMESPACE
//
// TModule derived class to facilitate streaming pointer to the OWL Library
// the OWL module must be streamed by reference before any pointers to it
// the following code simply prevents writing data back over the OWL module
//

class _OWLCLASS TObjectWindowsLibrary : public TModule {
 public:
  TObjectWindowsLibrary(HINSTANCE hInst) : TModule(_T("ObjectWindowsDLL"), hInst){}
  DECLARE_STREAMABLE(_OWLCLASS, TObjectWindowsLibrary, 1);
};


IMPLEMENT_STREAMABLE1(TObjectWindowsLibrary, TModule);

#  if !defined(BI_NO_OBJ_STREAMING)

//
// Read the object from the persistent stream.
//
void*
TObjectWindowsLibrary::Streamer::Read(ipstream&, uint32) const
{
  return GetObject();
}

//
// Write the object into a persistent stream.
//
void
TObjectWindowsLibrary::Streamer::Write(opstream&) const
{
}

#  endif  // if !defined(BI_NO_OBJ_STREAMING)

__OWL_END_NAMESPACE

//JJH - following stuff is defined in global.cpp,
//      we do not need it here.
//Jogy - it is needed in both places for DLL builds
# if !defined(WINELIB) //?use guard or comment it out completely?
//
// Global pointer to this module
//
#  if defined(BI_COMP_BORLANDC) && !defined(BI_NEW_GMODULE)
__OWL TModule* Module = 0;
#  else  // (BI_COMP_BORLANDC) && !defined(BI_NEW_GMODULE)
__OWL TModule* gModule = 0;
#  endif // (BI_COMP_BORLANDC) && !defined(BI_NEW_GMODULE)

# endif //!WINELIB


__OWL_BEGIN_NAMESPACE

#  if !defined(BI_STD_EXCEPTIONS)
_OWLDATA(xalloc) __xalloc( _T("Out of memory"), 0 );
#  endif // BI_STD_EXCEPTIONS
__OWL_END_NAMESPACE

#  if !defined(BI_COMP_BORLANDC) && !defined(BI_COMP_GNUC)

//static void  __cdecl xnew_handler() { throw __xalloc;}
#    if !defined(BI_STD_EXCEPTIONS)
int  __cdecl xnew_handler(size_t) { throw __xalloc; return 0;}
#    else  // BI_STD_EXCEPTIONS
int  __cdecl xnew_handler(size_t) { throw XALLOC(); return 0;}
#    endif // BI_STD_EXCEPTIONS

static void init_new_handler(void)
{
  _set_new_handler(&xnew_handler);
}
#  endif // !defined(BI_COMP_BORLANDC) && !defined(BI_COMP_GNUC)



static int  Attaches = 0;  // Win32s doesn't have per-instance data-- keep
                           // track of number of attached processes
// for MSVC compiler
#    if defined(BI_COMP_MSC)
#      ifdef _DEBUG
#        if (_MSC_VER < 1200) || (_MSC_VER > 1700)
#          error Only MS VC++ 6/7/8/9/10/11 supported.  Add new runtime version - module.cpp
#        endif
#        if _MSC_VER == 1600
#          define RTLDLL "MSVCR110D.DLL"
#        elif _MSC_VER == 1600
#          define RTLDLL "MSVCR100D.DLL"
#        elif _MSC_VER == 1500
#          define RTLDLL "MSVCR90D.DLL"
#        elif _MSC_VER == 1400
#          define RTLDLL "MSVCR80D.DLL"
#        elif _MSC_VER == 1310
#          define RTLDLL "MSVCR71D.DLL"
#        elif _MSC_VER == 1300
#          define RTLDLL "MSVCR70D.DLL"
#        else          // MSVC 5 and 6 use MSVCRTD.DLL
#          define RTLDLL "MSVCRTD.DLL"
#        endif
#      else
#        if (_MSC_VER < 1200) || (_MSC_VER > 1700)
#          error Only MS VC++ 6/7/8/9/10/11 supported.  Add new runtime version - module.cpp
#        endif
#        if _MSC_VER == 1700
#          define RTLDLL "MSVCR110.DLL"
#        elif _MSC_VER == 1600
#          define RTLDLL "MSVCR100.DLL"
#        elif _MSC_VER == 1500
#          define RTLDLL "MSVCR90.DLL"
#        elif _MSC_VER == 1400
#          define RTLDLL "MSVCR80.DLL"
#        elif _MSC_VER == 1310
#          define RTLDLL "MSVCR71.DLL"
#        elif _MSC_VER == 1300
#          define RTLDLL "MSVCR70.DLL"
#        else          // MSVC 5 and 6 use MSVCRT.DLL
#          define RTLDLL "MSVCRT.DLL"
#        endif
#      endif //_DEBUG

/*static*/ HINSTANCE _hInstance; //JJH removed static, 'coz _hInstance cannot be extern and static at once (see defs.h:104)

#     elif defined(BI_COMP_BORLANDC) // defined(BI_COMP_MSC)

#     if __BORLANDC__ >= 0x650
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC32120MT.DLL"
#        else
#          define RTLDLL "CC32120.DLL"
#        endif
#     elif __BORLANDC__ >= 0x640
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC32120MT.DLL"
#        else
#          define RTLDLL "CC32120.DLL"
#        endif
#     elif __BORLANDC__ >= 0x630
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC32110MT.DLL"
#        else
#          define RTLDLL "CC32110.DLL"
#        endif
#     elif __BORLANDC__ >= 0x620
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC32100MT.DLL"
#        else
#          define RTLDLL "CC32100.DLL"
#        endif
#     elif __BORLANDC__ >= 0x610
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC3290MT.DLL"
#        else
#          define RTLDLL "CC3290.DLL"
#        endif
#     elif __BORLANDC__ >= 0x590
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC3280MT.DLL"
#        else
#          define RTLDLL "CC3280.DLL"
#        endif
#     elif __BORLANDC__ >= 0x580
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC3270MT.DLL"
#        else
#          define RTLDLL "CC3270.DLL"
#        endif
#     elif __BORLANDC__ >= 0x560
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC3260MT.DLL"
#        else
#          define RTLDLL "CC3260.DLL"
#        endif
#     elif __BORLANDC__ >= 0x550
#       if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CC3250MT.DLL"
#        else
#          define RTLDLL "CC3250.DLL"
#        endif
#      elif __BORLANDC__ == 0x540
#        if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CW3245MT.DLL"
#        else
#          define RTLDLL "CW3245.DLL"
#        endif
#      elif __BORLANDC__ == 0x530
#        if defined(BI_MULTI_THREAD_RTL)
#          if defined(VCLRTL) 
#            define RTLDLL "CP3240MT.DLL"
#          else
#            define RTLDLL "CW3240MT.DLL"
#          endif
#        else
#          define RTLDLL "CW3240.DLL"
#       endif
#      elif __BORLANDC__ == 0x520
#        if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CW3230MT.DLL"
#        else
#          define RTLDLL "CW3230.DLL"
#       endif
#      elif __BORLANDC__ == 0x500
#        if defined(BI_MULTI_THREAD_RTL)
#          define RTLDLL "CW3220MT.DLL"
#        else
#          define RTLDLL "CW3220.DLL"
#       endif
#      else 
#       error Unknown BC compiler RTL
#      endif 
#    elif defined(BI_COMP_GNUC)
#      if defined(__CYGWIN__)
#        define RTLDLL "CYGWIN1.DLL"
#      elif defined(__MINGW__) || defined(__MINGW32__)
#       if defined(__MSVCRT__)
#          define RTLDLL "MSVCRT.DLL"
#       else 
#          define RTLDLL "CRTDLL.DLL"
#       endif
#      else
#       if !defined(WINELIB) //JJH
#         error Unknown GNU compiler
#       else  //normal GCC
#         if defined(__MSVCRT__)
#           define RTLDLL "MSVCRT.DLL"
#         else
#           define RTLDLL "CRTDLL.DLL"
#         endif
#       endif
#      endif
#    else  //  defined(BI_COMP_BORLANDC)
#      error Unknown compiler
#    endif // if defined(BI_COMP_MSC)

extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID);
#    if defined(BI_COMP_BORLANDC)  
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = RawDllMain;
#    else
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID); //JJH split this to declaration and initialization
BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &RawDllMain;
#    endif

extern "C"
//#    if !defined(BI_COMP_MSC)
//BOOL WINAPI RawDllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
//#    else
BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID)
//#    endif
{
  if (dwReason == DLL_PROCESS_ATTACH){
//#   if defined(BI_COMP_MSC)
//    _hInstance = hInstance; // init hInstance;
//#   endif
#   if !defined(WINELIB) //JJH
    // Prevent the C runtime DLL from being unloaded prematurely
    //TODO: Jogy - Is this code really neccessary??
    LoadLibraryA(RTLDLL);
#   endif
    //Start gathering memory leaks:
#    if defined(OWL_STACKWALKER) && defined(BI_COMP_MSC) && (defined(__DEBUG) || defined(_DEBUG))
    InitAllocCheck(ACOutput_XML);
#   endif

#    ifdef _UNICODE
//TODO: Jogy - Check this logic with Unicows
    // give error message and exit if running Unicode on non-Unicode system
    if (::GetVersion() & 0x80000000){
      // Note: this message is for programmers who can read english
      ::MessageBoxA(NULL,
        "This application or DLL can not be loaded "
        "on Windows 95 or on Windows 3.1.  It takes advantage "
        "of Unicode features only available on Windows NT.",
        "OWL Runtime Module", MB_ICONSTOP|MB_OK);
      return FALSE; // and fail
    }
#    endif
    
// Prevent the C runtime DLL from being unloaded prematurely
//TODO: Jogy - Is this code really neccessary??
    LoadLibraryA(RTLDLL);


    //
    //!! SetErrorMode(GetErrorMode(0)|SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    // add a reference to thread local storage data
    TlsAddRefs();

  }
  else if (dwReason == DLL_PROCESS_DETACH){
    TlsRelease();
#    if defined(OWL_STACKWALKER) && defined(BI_COMP_MSC) && (defined(__DEBUG) || defined(_DEBUG))
    //Stop gathering memory leaks:
    DeInitAllocCheck();
#   endif
    #if !defined(WINELIB) //JJH
    // Now it's OK for C runtime DLL to be unloaded (see LoadLibrary above)
    FreeLibrary(GetModuleHandleA(RTLDLL));
    #endif
  }
  return TRUE;    // ok
}
//
// BUG with BC 5.02
// NOTE: We use priority 31 to come just before/after ctr/dtr of global
//       objects (which are assigned a priorority of 32)
//
#if defined(BI_COMP_BORLANDC) && (__BORLANDC__ == 0x520)
static void __initOWL()
{
  TlsAddRefs();
}
#pragma startup __initOWL 31

//
static void __termOWL()
{
  TlsRelease();
}
#pragma exit __termOWL 31
#endif // BI_COMP_BORLANDC

#if !defined(BI_COMP_BORLANDC)
#if defined(WINELIB) //JJH
extern "C"
#endif
BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD reason, LPVOID lpreserved)
#else
int WINAPI
DllEntryPoint(HINSTANCE hInstance, uint32 reason, void*)
#endif
{
  switch (reason) {
    case DLL_PROCESS_ATTACH: {
      if (!Attaches) {
#if !defined(BI_COMP_BORLANDC) && !defined(BI_COMP_GNUC)
        init_new_handler();
#endif

//JJH Ok, we don't need to initialize gModule, since TApplication object
//    does this for us, see also applicat.cpp:420, applicat.h:132
#if !defined(WINELIB)
        gModule = new __OWL TObjectWindowsLibrary(hInstance);
#endif
      }
      Attaches++;
      break;
    }
    case DLL_PROCESS_DETACH: {
      Attaches--;
//JJH We don't need this here, check my previous comment for gModule init.
#if !defined(WINELIB)
      if (!Attaches)
        delete gModule;
#endif
      break;
    }
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// Here VC bug with vector constructor
//
// Special code to wire up vector deleting destructors
#if defined(BI_COMP_MSC)
static void _OwlForceVectorDelete()
{
#ifdef _DEBUG
  PRECONDITION(false);  // never called
#endif

  new __OWL TPoint[2];
  new __OWL TPointF[2];
  new __OWL TPointL[2];
  new __OWL TRect[2];
  new __OWL TSize[2];
  new __OWL TColor[2];
  new __OWL TMenu[2];
  new __OWL TResId[2];
  new __OWL TFileTime[2];
  new __OWL TSystemTime[2];
}
#endif // BI_COM_MSC
#else  // else _BUILDOWLDLL
////////////////////////////////////////////////////////////////////////////////////////
// static lib????
/////////////////////////////////////
#if defined(BI_COMP_BORLANDC)
//
// Doesn't work here -> moved to winmain
//
//
// NOTE: We use priority 31 to come just before/after ctr/dtr of global
//       objects (which are assigned a priorority of 32)
//
void __initOWL()
{
  TlsAddRefs();
}
#pragma startup __initOWL 31

//
void __termOWL()
{
  TlsRelease();
}
#pragma exit __termOWL 31

//
#elif defined(BI_COMP_MSC)


#if defined(BI_COMP_FORTE) || defined(BI_COMP_ACC) //|| defined(__EDG__)
  void __cdecl xnew_handler() { throw XALLOC(); }
#else
#if !defined(BI_STD_EXCEPTIONS)
int  __cdecl xnew_handler(size_t) { throw __xalloc; return 0;}
#else  // BI_STD_EXCEPTIONS
  int  __cdecl xnew_handler(size_t) { throw XALLOC(); return 0;}
#endif // BI_STD_EXCEPTIONS
#endif //else not BI_COMP_FORTE
static void init_new_handler(void)
{
//#ifdef __EDG__
//  std::set_new_handler(&xnew_handler);
//#else
  _set_new_handler(&xnew_handler);
//#endif
}

// force initialization early
#pragma warning(disable: 4073)
#pragma init_seg(lib)

static void __initOWL()
{
  //_hInstance = GetModuleHandleA(NULL);
  TlsAddRefs();

  init_new_handler();

#if defined(BI_DBCS_SUPPORT)
//  _setmbcp(_MB_CP_ANSI); // check
#endif
}

//
#ifdef BI_COMP_FORTE
extern "C"
#endif
void _cdecl __termOWL()
{
  TlsRelease();
}

static char __initOWLS = (char)(__initOWL(), atexit(&__termOWL));

//
# elif defined(BI_COMP_GNUC)

// //JJH
//#if defined(WINELIB)
//void __termOWL()
//#else
void _cdecl __termOWL()
//#endif
{
  TlsRelease();
}

static void __initOWL()
{
  TlsAddRefs();
}

// //JJH
//#if defined(WINELIB)
//typedef void (__attribute__((__stdcall__)) *STDCALL_FN)();
//static char __initOWLS = (char)(__initOWL(),atexit((STDCALL_FN)__termOWL));
//#else
static char __initOWLS = (char)(__initOWL(),atexit(__termOWL));
//#endif

#else
#error Unknown compiler
#endif // #ifdef BI_COMP_FORTE


#endif  // defined(_BUILDOWLDLL)
#endif  // SECTION == 3

#if !defined(SECTION) || SECTION == 4
__OWL_BEGIN_NAMESPACE
//
// Inserter for formated output of instance handle
//
_OWLCFUNC(ostream&)
operator <<(ostream& os, const TModule& m)
{
  return os << hex << uint(m.Handle);
}
__OWL_END_NAMESPACE

#endif //section 4

/* ========================================================================== */

