//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// TThread class implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_THREAD_H)
#include <owl/thread.h>
#endif
#if !defined(OWL_POINTER_H)
#include <owl/pointer.h>
#endif

#if !defined(WINELIB)
#include <process.h>
#endif //!WINELIB

#include <stdlib.h>

//FMM
#if defined(__HPUX_SOURCE)
#include <owl/except.h>
#include <owl/private/defs.h>

#endif

__OWL_BEGIN_NAMESPACE

DIAG_DEFINE_GROUP(Threads,1,0);

//----------------------------------------------------------------------------
// Semaphores Win32
//

//----------------------------------------
// TMutex WIN32

void TMutex::Release()
{
  ::ReleaseMutex(*this);
}

//----------------------------------------
// TCountedSemaphore WIN32

void TCountedSemaphore::Release()
{
  ::ReleaseSemaphore(*this, 1, 0);  
}

//----------------------------------------
// TEventSemaphore WIN32

void TEventSemaphore::Release()
{
  // Events don't need to be released
}


//-----------------------------------------------------------------------
//
// TWaitableTimerDLL
class TWTDll{
  public:
    static bool    Isvailable();
    static HANDLE  CreateWaitableTimer(LPSECURITY_ATTRIBUTES,BOOL,LPCTSTR);
    static HANDLE  OpenWaitableTimer(uint32 access, BOOL inherit, LPCTSTR name);
    static BOOL   SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER* duetime, int32 period, PTIMERAPCROUTINE compFunc, void* param, BOOL fResume);
    static BOOL   CancelWaitableTimer(HANDLE);
};

static _TCHAR TKernelStr[] = _T("kernel32");
#if defined(UNICODE)
  static char CreateWaitableTimerStr[] = "CreateWaitableTimerW";
  static char OpenWaitableTimerStr[]   = "OpenWaitableTimerW";
#else
  static char CreateWaitableTimerStr[] = "CreateWaitableTimerA";
  static char OpenWaitableTimerStr[]   = "OpenWaitableTimerA";
#endif
  static char SetWaitableTimerStr[]    = "SetWaitableTimer";
  static char CancelWaitableTimerStr[] = "CancelWaitableTimer";
  static LPCTSTR NotSupportedStr = _T("NotSupported function Call");
//
bool TWTDll::Isvailable()
{
  // Win 98 or NT 4.0
  static bool isAvailable = TSystem::GetMajorVersion()>4 || (TSystem::IsNT() && TSystem::GetMajorVersion()>=4);
  return isAvailable;
}
//
HANDLE TWTDll::CreateWaitableTimer(LPSECURITY_ATTRIBUTES sa, BOOL mr, LPCTSTR name)
{
  typedef DECLARE_FUNC(HANDLE,WINAPI,*TProc)(LPSECURITY_ATTRIBUTES,BOOL, LPCTSTR);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TKernelStr),CreateWaitableTimerStr);
  return Proc(sa, mr, name);
}
//
HANDLE TWTDll::OpenWaitableTimer(uint32 access, BOOL inherit, LPCTSTR name)
{
  typedef DECLARE_FUNC(HANDLE,WINAPI,*TProc)(uint32,BOOL,LPCTSTR);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TKernelStr),OpenWaitableTimerStr);
  return Proc(access, inherit, name);
}
//
BOOL TWTDll::SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER* duetime, int32 period, PTIMERAPCROUTINE compFunc, void* param, BOOL fResume)
{
  typedef DECLARE_FUNC(BOOL,WINAPI,*TProc)(HANDLE, const LARGE_INTEGER*,int32,PTIMERAPCROUTINE,void*,BOOL);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TKernelStr),SetWaitableTimerStr);
  return Proc(hTimer,duetime,period,compFunc,param,fResume);
}
//
BOOL TWTDll::CancelWaitableTimer(HANDLE hHandle)
{
  typedef DECLARE_FUNC(BOOL,WINAPI,*TProc)(HANDLE);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TKernelStr),CancelWaitableTimerStr);
  return Proc(hHandle);
}
//
//
//
TWaitableTimer::TWaitableTimer(bool manualReset, LPCTSTR name, LPSECURITY_ATTRIBUTES sa)
{
  if(!TWTDll::Isvailable())
    TXOwl::Raise(NotSupportedStr);
  Handle  =  TWTDll::CreateWaitableTimer(sa, manualReset, name);
}
//
TWaitableTimer::TWaitableTimer(LPCTSTR name, bool inherit,  uint32 access)
{
  if(!TWTDll::Isvailable())
    TXOwl::Raise(NotSupportedStr);
  Handle = TWTDll::OpenWaitableTimer(access, inherit, name);
}
//
bool  
TWaitableTimer::Cancel()
{
  if(!TWTDll::Isvailable())
    TXOwl::Raise(NotSupportedStr);
  return  TWTDll::CancelWaitableTimer(Handle) ? true : false;  // handle to a timer object
}
//
bool  
TWaitableTimer::Set(const TFileTime& duetime,int32 period, PTIMERAPCROUTINE compFunc,
                    void* param, bool resume)
{
  if(!TWTDll::Isvailable())
    TXOwl::Raise(NotSupportedStr);
  return TWTDll::SetWaitableTimer(Handle, (const LARGE_INTEGER*)&duetime,period,compFunc,param, resume);
}
//
void TWaitableTimer::Release()
{
}
//----------------------------------------
// TSemaphoreSet & its TLock for Win32

//
TSemaphoreSet::TSemaphoreSet(const TSemaphore* sems[], int size)
:
  Sems(0)
{
  int count = 0;
  if (sems)
    while (sems[count])
      count++;
  Count = count;
  Size = size >= 0 ? size : count;
  if (Size) {
    Sems = CONST_CAST(const TSemaphore**, new TSemaphorePtr[Size]);
    int i = 0;
    if (sems)
      for (; i < Count; i++)
        Sems[i] = sems[i];
    for (; i < Size; i++)
      Sems[i] = 0;
  }
}

//
TSemaphoreSet::~TSemaphoreSet()
{
  delete[] Sems;
}

//
void TSemaphoreSet::Add(const TSemaphore& sem)
{
  if (Count < Size)
    Sems[Count++] = &sem;
}

//
void TSemaphoreSet::Remove(const TSemaphore& sem)
{
  CHECK(Count <= Size);
  for (int i = 0; i < Count; i++)
    if (Sems[i] == &sem) {
      for (int j = i; j < Count-1; j++)  // Shift rest down to keep packed
        Sems[j] = Sems[j+1];
      Sems[Count-1] = 0;
      return;
    }
}

//
void TSemaphoreSet::Release(int index)
{
  if (index >= 0)
    CONST_CAST(TSemaphore*,Sems[index])->Release();
  else
    for (int i = 0; i < Count; i++)
      CONST_CAST(TSemaphore*,Sems[i])->Release();
}

//
static HANDLE* newHandles(const TSemaphoreSet& set)
{
  HANDLE* handles = new HANDLE[set.GetCount()];
  for (int i = 0; i < set.GetCount(); i++) {
    CHECK(set[i]);
    handles[i] = *set[i];  // Assumes non-0 since i is in set range
  }
  return handles;
}

//
TSemaphoreSet::TLock::TLock(const TSemaphoreSet& set, TWaitWhat wait,
                            ulong timeOut, bool alertable)
:
  Set(0)
{
  TAPointer<THandle> handles = newHandles(set);

  if (InitLock(set.Count, wait,
               ::WaitForMultipleObjectsEx(set.Count, handles, wait, timeOut, alertable)))
    Set = &set;
}

//
TSemaphoreSet::TLock::TLock(ulong msgMask, const TSemaphoreSet& set,
                            TWaitWhat wait, ulong timeOut)
{
  TAPointer<THandle> handles = newHandles(set);

  if (InitLock(set.Count, wait,
               ::MsgWaitForMultipleObjects(set.Count, handles, wait, timeOut, msgMask)))
    Set = &set;
}

//
// Init the Set and Locked members after a system wait call
//
bool TSemaphoreSet::TLock::InitLock(int count, TWaitWhat wait, int index)
{
  if (index >= (int)WAIT_OBJECT_0 && index < int(WAIT_OBJECT_0+count) ||
      index >= (int)WAIT_ABANDONED_0 && index < int(WAIT_ABANDONED_0+count))
  {
    if (wait == WaitAny) {
      if (index >= WAIT_ABANDONED_0)
        index -= WAIT_ABANDONED_0;
      Locked = index;      // Just this one is locked
    }
    else
      Locked = AllAquired;         // They are all locked
    return true;
  }
  else if (index == int(WAIT_OBJECT_0+count))
    Locked = MsgWaiting;
  else if (index == WAIT_TIMEOUT)
    Locked = TimedOut;
  else if (index == WAIT_IO_COMPLETION)
    Locked = IoComplete;
  return false;
}

//
TSemaphoreSet::TLock::~TLock()
{
  Release();
}

//
void TSemaphoreSet::TLock::Release(bool relinquish)
{
  if (Set) {
    CONST_CAST(TSemaphoreSet*,Set)->Release(Locked);
    if (relinquish)
      Set = 0;
  }
}
//
//
//
//
class TNtDll{
  public:
    static bool    Isvailable();
    static void    RtlInitialize(TMRSWSection::RTL_RWLOCK*);
    static void    RtlDeleteResource(TMRSWSection::RTL_RWLOCK*);
    static uint8  RtlAcqResourceExclusive(TMRSWSection::RTL_RWLOCK*, uint8);
    static uint8  RtlAcqResourceShared(TMRSWSection::RTL_RWLOCK*, uint8);
    static void    RtlReleaseResource(TMRSWSection::RTL_RWLOCK*);
    static void    RtlDumpResource(TMRSWSection::RTL_RWLOCK*);
};
static _TCHAR TNtDllStr[] = _T("ntdll.dll");
static char TInitStr[] = "RtlInitializeResource";
static char TDeleteStr[] = "RtlDeleteResource";
static char TGetExclusiveStr[] = "RtlAcquireResourceExclusive";
static char TGetSharedStr[] = "RtlAcquireResourceShared";
static char TReleaseStr[] = "RtlReleaseResource";
static char TDumpStr[] = "RtlDumpResource";
//
static bool isVersionNT()
{
  OSVERSIONINFO os;
  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  ::GetVersionEx(&os);
  return os.dwPlatformId==VER_PLATFORM_WIN32_NT;
}
bool TNtDll::Isvailable()
{
  static bool isAvailable = isVersionNT() && 
                            GetModuleHandle(TNtDllStr) &&
                            GetProcAddress(GetModuleHandle(TNtDllStr),TInitStr);
  return isAvailable;
}
//
void TNtDll::RtlInitialize(TMRSWSection::RTL_RWLOCK* lock)
{
  typedef DECLARE_FUNC(void,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TInitStr);
  Proc(lock);
}
//
void TNtDll::RtlDeleteResource(TMRSWSection::RTL_RWLOCK* lock)
{
  typedef DECLARE_FUNC(void,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TDeleteStr);
  Proc(lock);
}
//
uint8  TNtDll::RtlAcqResourceExclusive(TMRSWSection::RTL_RWLOCK* lock, uint8 wait)
{
  typedef DECLARE_FUNC(uint8,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*,uint8);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TGetExclusiveStr);
  return Proc(lock,wait);
}
//
uint8  TNtDll::RtlAcqResourceShared(TMRSWSection::RTL_RWLOCK* lock, uint8 wait)
{
  typedef DECLARE_FUNC(uint8,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*,uint8);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TGetSharedStr);
  return Proc(lock,wait);
}
//
void TNtDll::RtlReleaseResource(TMRSWSection::RTL_RWLOCK* lock)
{
  typedef DECLARE_FUNC(void,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TReleaseStr);
  Proc(lock);
}
//
void TNtDll::RtlDumpResource(TMRSWSection::RTL_RWLOCK* lock)
{
  typedef DECLARE_FUNC(void,WINAPI,*TProc)(TMRSWSection::RTL_RWLOCK*);
  static TProc Proc = (TProc)GetProcAddress(GetModuleHandle(TNtDllStr),TDumpStr);
  Proc(lock);
}
#if defined(_ANONYMOUS_STRUCT)
#define _RWSEC_ RWSec
#define _RWCS_  CritSec
#else
#define _RWSEC_ u.RWSec
#define _RWCS_  u.CritSec
#endif
//
//
//
// class TMRSWSection
// ~~~~~ ~~~~~~~~~~~~
//
TMRSWSection::TMRSWSection()
{
  if(TNtDll::Isvailable())
    TNtDll::RtlInitialize(&_RWSEC_);
  else
    ::InitializeCriticalSection(CONST_CAST(CRITICAL_SECTION*,&_RWCS_));
}
//
TMRSWSection::~TMRSWSection()
{
  if(TNtDll::Isvailable())
    TNtDll::RtlDeleteResource(&_RWSEC_);
  else
    ::DeleteCriticalSection(CONST_CAST(CRITICAL_SECTION*,&_RWCS_));
}
//
void TMRSWSection::Dump()
{
  if(TNtDll::Isvailable())
    TNtDll::RtlDumpResource(&_RWSEC_);
}
//
TMRSWSection::TLock::TLock(const TMRSWSection& rwsec, bool shared, bool wait)
:
  MRSWSecObj(rwsec)
{
  if(TNtDll::Isvailable()){
    if(shared)
      TNtDll::RtlAcqResourceShared(CONST_CAST(TMRSWSection::RTL_RWLOCK*,&MRSWSecObj._RWSEC_),wait);
    else
      TNtDll::RtlAcqResourceExclusive(CONST_CAST(TMRSWSection::RTL_RWLOCK*,&MRSWSecObj._RWSEC_),wait);
  }
  else
    ::EnterCriticalSection(CONST_CAST(CRITICAL_SECTION*,&MRSWSecObj._RWCS_));
}
//
TMRSWSection::TLock::~TLock()
{
  if(TNtDll::Isvailable())
    TNtDll::RtlReleaseResource(CONST_CAST(TMRSWSection::RTL_RWLOCK*,&MRSWSecObj._RWSEC_));
  else
    ::LeaveCriticalSection(CONST_CAST(CRITICAL_SECTION*,&MRSWSecObj._RWCS_));
}
// 

//----------------------------------------------------------------------------
// TThread Win32 

//
// TThread constructors
//
TThread::TThread()
:
  ThreadId(0),
  Handle(0),
  Stat(Created),
  TerminationRequested(0),
  Attached(false)
{
}

//
// Attach to a running thread
//
TThread::TThread(TCurrent)
:
  ThreadId(::GetCurrentThreadId()),
  Handle(0),
  Stat(Running),
  TerminationRequested(0),
  Attached(true)
{
  ::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(),
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

//
//
//
TThread::TThread(const TThread&)
:
  ThreadId(0),
  Handle(0),
  Stat(Created),
  TerminationRequested(0),
  Attached(false)
{
}

//
// TThread assignment operator
//
// Used when assigning derived objects. Attempting to assign from a running
// object is an error, since the data fields in the running object can be
// changing asynchronously.
//
const TThread& TThread::operator =(const TThread& thread)
{
  switch (GetStatus()) {
    case Created:
    case Suspended:
    case Finished: {
      if (this != &thread) {
        Handle = 0;
        ThreadId = 0;
        Stat = Created;
        TerminationRequested = false;
        Attached = false;
      }
      return *this;
    }
    default:
      throw TThreadError(TThreadError::AssignError);
  }
#if defined(BI_COMP_MSC)
  return *this;     // Bogus return to make MSVC happy
#endif
}

//
// TThread destructor
//
// If the thread hasn't finished, destroying its control object is an error.
//
TThread::~TThread()
{
  if (!Attached && (GetStatus() == Running || GetStatus() == Suspended))
    throw TThreadError(TThreadError::DestroyBeforeExit);
//
// The RTL calls CloseHandle in _endthread, so we shouldn't if the thread
// was started with _beginthreadNT(...).
#  if !defined(BI_MULTI_THREAD_RTL) || !defined(BI_COMP_BORLANDC)
  ::CloseHandle(Handle);
#  endif
}

//
// Starts the thread executing. The actual call depends on the operating system.
// After the system call we check status.
//
TThread::THandle TThread::Start()
{
  // If Start() has already been called for this thread, release the
  // previously created system thread object before launching a new one.
  //
  if ((GetStatus() != Created) && Handle) {
    ::CloseHandle(Handle);
  }

# if defined(BI_MULTI_THREAD_RTL)
#    if defined(BI_COMP_MSC) || defined(BI_COMP_GNUC)
  Handle = (HANDLE)::_beginthreadex(0, 4096, &TThread::Execute, this, 0, (uint*)&ThreadId);
#    else
  Handle = (HANDLE)::_beginthreadNT(&TThread::Execute, 4096, this, 0, 0, &ThreadId);
#    endif
# else
  Handle = ::CreateThread(0, 0, &TThread::Execute, this, 0, &ThreadId);
# endif


  if (Handle) {
    TRACEX(Threads, 1, _T("Thread started [id:") << Handle << _T(']'));
    Stat = Running;
  }
  else {
    TRACEX(Threads, 2, _T("Thread failed to start"));
    Stat = Invalid;
    throw TThreadError(TThreadError::CreationFailure);
  }

  return Handle;
}

//
// It's an error to try to suspend a thread that hasn't been started or that
// has already terminated.
//
ulong TThread::Suspend()
{
  switch (GetStatus()) {
    case Created:
      TRACEX(Threads, 2, _T("Illegal Created thread suspension [id:") << Handle << _T(']'));
      throw TThreadError(TThreadError::SuspendBeforeRun);
    case Finished:
      TRACEX(Threads, 2, _T("Illegal Finished thread suspension [id:") << Handle << _T(']'));
      throw TThreadError(TThreadError::SuspendAfterExit);
    default:
      ulong res = ::SuspendThread(Handle);
      if (res < MAXIMUM_SUSPEND_COUNT)  // Else a problem
        Stat = Suspended;

      TRACEX(Threads, 0, _T("Thread suspended [id:") << Handle << _T(", Count:") << res << _T(']'));
      return res;
  }
}

//
// It's an error to try to resume a thread that isn't suspended.
//
ulong TThread::Resume()
{
  switch (GetStatus()) {
    case Created:
      TRACEX(Threads, 2, _T("Illegal Created thread resumption [id:") << Handle << _T(']'));
      throw TThreadError(TThreadError::ResumeBeforeRun);
    case Running:
      TRACEX(Threads, 2, _T("Illegal Running thread resumption [id:") << Handle << _T(']'));
      throw TThreadError(TThreadError::ResumeDuringRun);
    case Finished:
      TRACEX(Threads, 2, _T("Illegal Finished thread resumption [id:") << Handle << _T(']'));
      throw TThreadError(TThreadError::ResumeAfterExit);
    default:
      ulong res = ::ResumeThread(Handle);
      TRACEX(Threads, 0, _T("Thread resumed [id:") << Handle << _T(", Count:") << res << _T(']'));
      if (res == 1) // Y.B. suggested by Richard.D.Crossley and Fendy Riyanto
        Stat = Running;
      return res;
    }
}

//
// Mark the thread for termination.
//
void TThread::Terminate()
{
  TRACEX(Threads, 1, _T("Thread termination requested [handle:") << Handle << _T(']'));
  TerminationRequested = true;
}

//
// Block until the thread terminates.
//
// IMPORTANT: the meaning of the 'timeout' parameter is different for NT and
// OS/2. Under NT it specifies how long to wait for termination. Under OS/2 it
// specifies whether to wait or to return immediately if the thread hasn't
// terminated.
//
ulong TThread::WaitForExit(ulong timeout)
{
  TRACEX(Threads, 1, _T("Waiting for thread exit [id:") << Handle << _T(']'));
  if (Stat == Running)
    return ::WaitForSingleObject(Handle, timeout);
  else
    return (ulong)-1;
}

//
// See note for WaitForExit().
//
ulong TThread::TerminateAndWait(ulong timeout)
{
  Terminate();
  return WaitForExit(timeout);
}

//
// Set the thread's priority.
//
int TThread::SetPriority(int pri)
{
  TRACEX(Threads, 1, _T("Thread priority changed to ") << pri <<
                     _T(" [id:") << Handle << _T(']'));
  return ::SetThreadPriority(Handle, pri);
}

//
//
//
int
TThread::Run()
{
  TRACEX(Threads, 1, _T("Illegal Run() on base TThread [id:") << Handle << _T(']'));
  return -1;
}

//
// Run the thread. This static function is given as the thread start address,
// with 'this' thread object passed as the param. Invoke the Run() method on
// the thread
//
#if defined(BI_MULTI_THREAD_RTL)
#  if  defined(BI_COMP_BORLANDC)
void _USERENTRY TThread::Execute(void* thread)
{
  STATIC_CAST(TThread*,thread)->Run();
  STATIC_CAST(TThread*,thread)->Stat = Finished;
}
#  else
uint _stdcall TThread::Execute(void* thread)
{
  int code = STATIC_CAST(TThread*,thread)->Run();
  STATIC_CAST(TThread*,thread)->Stat = Finished;
  return code;
}
#  endif
#else //#if defined(BI_MULTI_THREAD_RTL)
DWORD _stdcall TThread::Execute(void* thread)
{
  int code = STATIC_CAST(TThread*,thread)->Run();
  STATIC_CAST(TThread*,thread)->Stat = Finished;
  return code;
}
#endif //#if defined(BI_MULTI_THREAD_RTL)

//
// Alternative to returning from Run(). Called from within the thread that
// wants to exit early.
//
void
TThread::Exit(ulong code)
{
  Stat = Finished;
#  if (defined(BI_COMP_MSC)||defined(BI_COMP_GNUC)) && defined(BI_MULTI_THREAD_RTL)
    _endthreadex(code);
#  else
    ::ExitThread(code);
# endif
}

//
// Call only when Stat claims that the thread is Running.
//
TThread::TStatus
TThread::CheckStatus() const
{
  DWORD code;
  ::GetExitCodeThread(Handle, &code);
  if (code == STILL_ACTIVE)
    return Running;
  else
    return Finished;
}

//----------------------------------------------------------------------------

//
// TThread::TThreadError constructor
//
TThread::TThreadError::TThreadError(TErrorType type)
:
  TXBase(MakeString(type)),
  Type(type)
{
}

//
// TThread::TThreadError::MakeString()
//
// Translates an error code into a string.
//
owl_string TThread::TThreadError::MakeString(TErrorType type)
{
  static _TCHAR* Names[] = {
    _T("Suspend() before Run()"),
    _T("Resume() before Run()"),
    _T("Resume() during Run()"),
    _T("Suspend() after Exit()"),
    _T("Resume() after Exit()"),
    _T("creation failure"),
    _T("destroyed before Exit()"),
    _T("illegal assignment"),
    _T("Multithreaded Runtime not selected"),
  };
  owl_string Msg;
  Msg.reserve(80);
  Msg = _T("Error[thread]: ");
  Msg += Names[type];
  return Msg;
}
__OWL_END_NAMESPACE
//

#if defined(BI_MULTI_THREAD_RTL)
long TlsAddRefs();
__OWL_BEGIN_NAMESPACE
static long _owlTlsRef = -1;

// global _owlTlsDataMng used to allocate thread local indexes
uint8 __owlTlsDataMng[sizeof(TTlsDataManager)];

TTlsDataManager* _owlTlsDataMng = 0;

TTlsDataManager* GetTlsDataMng()
{
  if(!_owlTlsDataMng)
    TlsAddRefs();
  return _owlTlsDataMng;
}

__OWL_END_NAMESPACE

__OWL_USING_NAMESPACE

//
long TlsAddRefs()
{
  if(::InterlockedIncrement(&_owlTlsRef)==0){
    ::InterlockedIncrement(&_owlTlsRef);
    if(!_owlTlsDataMng)
	{
      _owlTlsDataMng = ::new(__owlTlsDataMng) TTlsDataManager;
    }
  }
  return _owlTlsRef;
}
//
long TlsRelease()
{
  if (::InterlockedDecrement(&_owlTlsRef) == 0)
  {
    if(_owlTlsDataMng)
	{
      _owlTlsDataMng->~TTlsDataManager();
      _owlTlsDataMng = 0;
    }
  }
  return _owlTlsRef;
}
#else

static long _owlTlsRef = 0;
long TlsAddRefs()
{  
  return _owlTlsRef++;
}
long TlsRelease()
{  
  return --_owlTlsRef;
}
#endif
/* ========================================================================== */
