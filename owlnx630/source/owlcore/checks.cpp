//--------------------------------------------------------------------------
// ObjectWindows                                                          //
// Copyright (c) 1993, 1996 by Borland International
// All Rights Reserved.
// Copyright (c) 1998  by Yura Bidus                                        //
//                                                                        //
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
//  CHECKS.CPP                                                            //
//                                                                        //
//------------------------------------------------------------------------//

#if defined(BI_COMP_BORLANDC) 
#  pragma hdrstop
#endif
 
#define CHECKS_CPP

//24.07.2007 Jogy - These lines caused a strange problem in the VS2005 debug DLL version
//#undef __DEBUG
//#define __DEBUG 0

//
// Make sure __TRACE is defined so that we can provide
// run-time support in non-debug versions of the library.
//
#if defined(__TRACE)
#  define __TRACE_PREVIOUSLY_DEFINED
#endif

#undef  __TRACE
#define __TRACE

#if !defined(__TRACE_PREVIOUSLY_DEFINED)
#define _DONT_DECLARE_DEF_DIAG_GROUP   // need this define so checks.h won't
                                       // try to declare Def diag group.
#endif

#include <owl/defs.h>

//#define DIAG_EXPORT __export
  

#include <owl/private/checks.h>
#if defined(UNICODE)
# if defined(BI_COMP_GNUC)
#    include <stdlib.h>
# else
#    include <malloc.h> 
# endif
#endif

#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif


#include <owl/module.h>
//#include <owl/template.h>
//#include <owl/contain.h>

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

//
// Definition of the default diagnostic group "Def" (expands to TDiagGroupDef)
//
#if defined(_BUILDOWLDLL)
#  if defined(__DEBUG) && defined(__TRACE_PREVIOUSLY_DEFINED)
   OWL_DIAG_DEFINE_EXPORTGROUP(Def,1,0); //JJH
#  else
DIAG_DEFINE_EXPORTGROUP(Def,1,0);
#  endif
#else
//DIAG_DEFINE_GROUP(Def,1,0);
#  if defined(__DEBUG) && defined(__TRACE_PREVIOUSLY_DEFINED)
OWL_DIAG_DEFINE_GROUP(Def,1,0); //DLN
#  else
DIAG_DEFINE_GROUP(Def,1,0);
#  endif
#endif

//
// Global stream for diagnostic messages
//
// BUG: doesn't always get constructed before use in
// constructors for static objects. Need to guarantee
// construction, probably with the same sort of mechanism as
// in the standard iostreams.
//

// Use dynamically allocated buffer so that we can clean up.
// multithreading support
struct __TDiagProcessData {  

    static TDiagBaseHook*  GlobalHook;
    static TDiagBase*      DiagGroupStaticHead;

#if defined(BI_MULTI_THREAD_RTL)
    static TMRSWSection*  Lock;
#endif
};

TDiagBaseHook*  __TDiagProcessData::GlobalHook  = 0;
TDiagBase*      __TDiagProcessData::DiagGroupStaticHead = 0;
#if defined(BI_MULTI_THREAD_RTL)
TMRSWSection*    __TDiagProcessData::Lock = 0;
#endif

//
#if defined(BI_MULTI_THREAD_RTL)
#  define LOCKDIAGDATA(s) TMRSWSection::TLock __lock(*__TDiagProcessData::Lock,s)
#else
#  define LOCKDIAGDATA(s)
#endif

#if defined(BI_MULTI_THREAD_RTL)
static void __checkLockS()
{
  if(!__TDiagProcessData::Lock)
  {
    __TDiagProcessData::Lock = new TMRSWSection; // uses malloc ???
  }
}
static void __checkLockE()
{
  if(!__TDiagProcessData::DiagGroupStaticHead && __TDiagProcessData::Lock)
  {
    delete __TDiagProcessData::Lock;
    __TDiagProcessData::Lock = 0;
  }
}
#endif

//
TDiagBaseHook*  
TDiagBase::SetGlobalHook(TDiagBaseHook* hook)
{
  LOCKDIAGDATA(false);
  TDiagBaseHook*  oldHook = __TDiagProcessData::GlobalHook;
  __TDiagProcessData::GlobalHook = hook;
  return oldHook;
}

//
TDiagBase::TDiagBase(LPCSTR name, uint8 e, uint8 l)
:
  Name(name), LocalHook(0)
{
  Flags.Enabled = e; 
  Flags.Level = l;
  
  AddDiagGroup(this);
}

//
TDiagBase::~TDiagBase()
{
  RemoveDiagGroup(this);
}

//
void TDiagBase::AddDiagGroup(TDiagBase* group)
{
#if defined(BI_MULTI_THREAD_RTL)
  __checkLockS();
#endif
  LOCKDIAGDATA(false);                                                           \

  if(__TDiagProcessData::DiagGroupStaticHead==0)
    __TDiagProcessData::DiagGroupStaticHead = group;
  else{
    TDiagBase* last = __TDiagProcessData::DiagGroupStaticHead;
    while(last->NextGroup)
      last = last->NextGroup;
    last->NextGroup = group;
  }
}

//
void TDiagBase::RemoveDiagGroup(TDiagBase* group)
{
  {
    LOCKDIAGDATA(false);                                                           \

    if(__TDiagProcessData::DiagGroupStaticHead==group)
      __TDiagProcessData::DiagGroupStaticHead = group->NextGroup;
    else{
      TDiagBase* last = __TDiagProcessData::DiagGroupStaticHead;
      while(last->NextGroup != group)
        last = last->NextGroup;
      last->NextGroup = group->NextGroup;
    }
  }
#if defined(BI_MULTI_THREAD_RTL)
  __checkLockE();
#endif
}

//
//
//
TDiagBase* 
TDiagBase::GetDiagGroup(TDiagBase* curr)
{
  if(curr)
    return curr->NextGroup;
  return __TDiagProcessData::DiagGroupStaticHead;
}

//
// TDiagBase::Output - Sends the specified message to the debug output
// device. Under Windows, the OutputDebugString() function (which sends
// the string to the AUX: device) is used.  Under DOS, the message is
// streamed out to cout.
//
//#if defined(__FLAT__)
//extern unsigned char __isGUI;   /* defined in c0nt.asm, to determine at runtime
//                                   if we're a GUI or Console mode application */
//#endif
void TDiagBase::Output(LPCTSTR msg)
{
  if(__TDiagProcessData::GlobalHook)
    __TDiagProcessData::GlobalHook->Output(this, msg);
  else if(LocalHook)
    LocalHook->Output(this, msg);
  else{

//   if(__isGUI)
    OwlOutputDebugString(msg);
//    else
//      cout << msg;
  }
}

//#include <ostream>
//
// TDiagBase::Trace - Formats a trace message using the specified parameters
// and outputs it using the output() function.
//
// Note that the formatted message is limited to DIAG_MAXBUFFER characters.
//

void TDiagBase::Message( LPCTSTR type, LPCTSTR msg, LPCSTR fname, uint32 line)
{
  diag_stream out;

  _USES_CONVERSION;

#if defined(BI_COMP_MSC) 
  // format especially for Visual Studio environment
  out << _A2W(fname) << _T("(") << line << _T(") : ") << type 
      << _T(": [") << Name << _T("] ") << msg
      << _T("\n\r") << ends;
#else 
  out << type << _T(" ") << _A2W(fname) << _T(" ") << line
        << _T(": [") << Name << _T("] ") << msg
        << _T("\n\r");
#endif

#if defined(BI_STD_NEWSSTREAM)
  Output(out.str().c_str());
#else
  Output(out.str());
  out.rdbuf()->freeze(0);
#endif
}


////////////////////////////////////////////////////////////////////////
static LONG TraceIndent = 0; //must be aligned on a 32-bit boundary

//
// class TDiagFunction
// ~~~~~ ~~~~~~~~~~~~~
//
TDiagFunction::TDiagFunction(TDiagBase& group, int level, LPCTSTR name, 
                             LPCSTR file, int line)
: 
  Group(group), Name (name), File(file),Line(line)
{
  Enable = (group.IsEnabled() && level <= group.GetLevel());

  if(Enable){
    Trace(_T(">> "),0,File,Line);
    InterlockedIncrement(&TraceIndent);
  }
}

TDiagFunction::TDiagFunction(TDiagBase& group, int level, LPCTSTR name, 
                             LPCTSTR params, LPCSTR file, int line)
: Group(group), Name(name), File(file),Line(line)
{
  Enable = (group.IsEnabled() && level <= group.GetLevel());

  if(Enable){
    Trace(_T(">> "),params,File,Line);
    InterlockedIncrement(&TraceIndent);
  }
}

TDiagFunction::~TDiagFunction()
{
  if(Enable){
    InterlockedDecrement(&TraceIndent);
    Trace(_T("<< "),0,File,Line);
  }
}

//
//  This method writes a trace message for entering of leaving a function.
//
void TDiagFunction::Trace(LPCTSTR prefix, LPCTSTR param, LPCSTR file, int line)
{
  diag_string text;
  
  int indent = TraceIndent;
  while(indent--)
    text += _T(" ");

  text += prefix;

  text += Name;

  if(param){
    text += _T(" (");
    text += param;
    text += _T(")");
  }

  {
    diag_stream out;
    out << text.c_str() << ends;
#if defined(BI_STD_NEWSSTREAM)
    Group.Trace(out.str().c_str(), file, line);
#else
    Group.Trace(out.str(), file, line);
    out.rdbuf()->freeze(0);
#endif
  }
}

//
//
//
#if defined(UNICODE)
_OWLFUNC(int) 
BreakMessage(LPCTSTR type, LPCSTR msg, LPCSTR file, int line)
{
  USES_CONVERSION; 
  return BreakMessage(type, A2W(msg), file, line);
}
#endif
//
static LONG owlAssertBusy = -1;
//
//
_OWLFUNC(int) 
BreakMessage(LPCTSTR type, LPCTSTR msg, LPCSTR file, int line)
{
  _TCHAR buffer[256];

  if (InterlockedIncrement(&owlAssertBusy) > 0){
    // assume the debugger or auxiliary port
    wsprintf(buffer, _T("ERROR: %s, File %hs, Line %d\n"),msg, file, line);
    OutputDebugString(buffer);
    InterlockedDecrement(&owlAssertBusy);

    // assert within assert (examine call stack to determine first one)
    _OWL_BREAK
    return 3;
  }

  _USES_CONVERSION; 
  wsprintf(buffer,_T("ERROR:\n")
               _T(" ** %s ** \n")
               _T("File: %s\n")
              _T("Line: %u\n")
              _T("Select:\n")
              _T("  Abort  - Terminate the program\n")
              _T("  Retry  - Debug the program\n")
              _T("  Ignore - Continue\n"),
              msg,_A2W(file),line);

  // active popup window for the current thread
  HWND hParent = ::GetActiveWindow();
  if (hParent){
    HWND hPopup = ::GetLastActivePopup(hParent);

    if (hPopup)
      hParent = hPopup;
  }

  // we remove WM_QUIT because if it is in the queue then the message box
  // won't display
  MSG    quitMsg;
  bool  quit = ::PeekMessage (&quitMsg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
  int ret = ::MessageBox(hParent,buffer,type, 
    MB_ABORTRETRYIGNORE|MB_DEFBUTTON3|MB_TASKMODAL|MB_ICONHAND|MB_SETFOREGROUND
    );

  if (quit)   // if there was a WM_QUIT message pending...
    PostQuitMessage (quitMsg.wParam);  // ...put it back

  // cleanup
  InterlockedDecrement(&owlAssertBusy);
  
  return ret;
}

//
diag_string
xerror::MakeString( LPCTSTR type, LPCTSTR txt, LPCSTR file, uint32 line )
{
#if defined(BI_STD_NEWSSTREAM)
  diag_stream temp;
#else
  _TCHAR buf[256];
  diag_stream temp(buf, COUNTOF(buf));
#endif
  temp << type << _T(" failed ") << file << _T(" ") << line << _T(": ") << txt << ends;
#if defined(BI_STD_NEWSSTREAM)
  return temp.str();
#else
  return diag_string(buf);
#endif
}
#if defined(UNICODE)
diag_string 
xerror::MakeString( LPCTSTR type, LPCSTR txt, LPCSTR file, uint32 line )
{
#if defined(BI_STD_NEWSSTREAM)
  diag_stream temp;
#else
  _TCHAR buf[256];
  diag_stream temp(buf,COUNTOF(buf));
#endif
  USES_CONVERSION; 
  temp << type << _T(" failed ") << A2W(file) << _T(" ") << line << _T(": ") 
       <<  A2W(txt) << ends;
#if defined(BI_STD_NEWSSTREAM)
  return temp.str();
#else
  return diag_string(buf);
#endif
}

#endif

//-----------------------------------------------------------------------------

__OWL_END_NAMESPACE
/* ========================================================================== */

