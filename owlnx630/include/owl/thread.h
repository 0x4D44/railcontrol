//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Defines the class TSemaphore and its nested class TLock.
// Defines the derived semaphore class TMutex
// Defines the derived semaphore class TCountedSemaphore
// Defines the derived semaphore class TEventSemaphore
// Defines the derived semaphore class TWaitableTimer
// Defined the semaphore set class TSemaphoreSet and its nested class TLock.
//
// Defines the class TCriticalSection and its nested class TLock.
// Defines the class TMRSWSection and its nested class TLock.
//
// Defines the class TSync and its nested class TLock.
// Defines the template TStaticSync and its nested class TLock.
//
// Defines the class TThread.
//
// Defines the class TTlsAllocator
// Defines the class TLocalObject
// Defines the class TLocalData
// Defines the class TTlsDataManager
// Defines the class TLocalObject
//----------------------------------------------------------------------------

#if !defined(OWL_THREAD_H)
#define OWL_THREAD_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(BI_MULTI_THREAD)
#error Thread classes require multi-threaded operating system.
#endif
#if !defined(OWL_PRIVATE_WSYSINC_H)
# include <owl/private/wsysinc.h>
#endif
#if !defined(OWL_WSYSCLS_H)
# include <owl/wsyscls.h>
#endif
#if !defined(OWL_TEMPLATE_H) && defined(BI_MULTI_THREAD_RTL)
# include <owl/template.h>
#endif
#if !defined(OWL_EXBASE_H)
# include <owl/exbase.h>
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

#if defined(BI_OWL_NO_po)
# pragma option -po-
#endif

//
// class TSemaphore
// ~~~~~ ~~~~~~~~~~
// Base class for handle-based thread synchronization classes, TMutex,
// TCountedSemaphore and TEventSemaphore. Defines some types & constants, as
// well as holding the system object handle for Win32.
//
class _OWLCLASS TSemaphore{
  public:
    virtual ~TSemaphore();

    enum { NoWait = 0, NoLimit = INFINITE };
    typedef HANDLE THandle;
    operator THandle() const;


    class TLock {
      public:
        TLock(const TSemaphore&, ulong timeOut = NoLimit, bool alertable = false);
       ~TLock();

        bool WasAquired() const;  // See if really aquired, or just timed-out

        // Release lock early & relinquish, i.e. before destructor. Or, just
        // drop the lock count on an exiting semaphore & keep locked 'til dtor
        //
        void Release(bool relinquish = false);

      private:
        const TSemaphore* Sem;
    };
    friend class TLock;
    friend class _OWLCLASS TSemaphoreSet;

  protected:
    virtual void Release() = 0;  // Derived class must provide release

    THandle Handle;              // Derived class must initialize
};

//
// class TMutex
// ~~~~~ ~~~~~~
// Mutual-exclusive semaphore
//
// TMutex provides a system-independent interface to critical sections in
// threads. With suitable underlying implementations the same code can be used
// under OS/2 and Windows NT.
//
// An object of type TMutex can be used in conjunction with objects of type
// TMutex::TLock (inherited from TSemaphore) to guarantee that only one thread
// can be executing any of the code sections protected by the lock at any
// given time.
//
// The differences between the classes TCriticalSection and TMutex are that a
// timeout can be specified when creating a Lock on a TMutex object, and that
// a TMutex object has a HANDLE which can be used outside the class. This
// mirrors the distinction made in Windows NT between a CRITICALSECTION and a
// Mutex. Under NT a TCriticalSection object is much faster than a TMutex
// object. Under operating systems that don't make this distinction a
// TCriticalSection object can use the same underlying implementation as a
// TMutex, losing the speed advantage that it has under NT.
//
class _OWLCLASS TMutex : public TSemaphore{
  public:
    TMutex(LPCTSTR name = 0, LPSECURITY_ATTRIBUTES sa = 0);
    TMutex(LPCTSTR name, bool inherit, uint32 access = MUTEX_ALL_ACCESS);
    TMutex(THandle handle);

    typedef TLock Lock;  // For compatibility with old T-less typename

    // If another mutex with the same name existed when this mutex
    // was created, then another handle to the object exists and
    // someone else may be using it too.
    //
    bool IsShared();

  private:
    TMutex(const TMutex&);
    const TMutex& operator =(const TMutex&);

    virtual void Release();  // Release this mutex semaphore

    bool Shared;

};

//
// class TCountedSemaphore
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Counted semaphore. Currently Win32 only
//
class _OWLCLASS TCountedSemaphore : public TSemaphore{
  public:
    TCountedSemaphore(int initialCount, int maxCount, LPCTSTR name = 0,
                      LPSECURITY_ATTRIBUTES sa = 0);
    TCountedSemaphore(LPCTSTR name, bool inherit,
                      uint32 access = SEMAPHORE_ALL_ACCESS);
    TCountedSemaphore(THandle handle);

  private:
    TCountedSemaphore(const TCountedSemaphore&);
    const TCountedSemaphore& operator =(const TCountedSemaphore&);

    virtual void Release();  // Release this counted semaphore
};

//
// class TEventSemaphore
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLCLASS TEventSemaphore : public TSemaphore{
  public:
    TEventSemaphore(bool manualReset=false, bool initialState=false,
                    LPCTSTR name = 0, LPSECURITY_ATTRIBUTES sa = 0);
    TEventSemaphore(LPCTSTR name, bool inherit,
                    uint32 access = SEMAPHORE_ALL_ACCESS);
    TEventSemaphore(THandle handle);

    void Set();
    void Reset();
    void Pulse();


  private:
    TEventSemaphore(const TMutex&);
    const TEventSemaphore& operator =(const TEventSemaphore&);

    virtual void Release();  // Release this event semaphore

};

//
// class TWaitableTimer
// ~~~~~ ~~~~~~~~~~~~~~
//  Encapsulation of Waitable Timer  over Borland Classlib TSemaphore hierarchy
//  Created by Marco Savegnago (msave) email: msave@tin.it
//
//   Waitable timers is available only in WinNT 4.0 or Windows 98
//   _WIN32_WINNT=0x0400 must be defined
//
//
// additional typedef for old headers
#if !defined(CreateWaitableTimer)
typedef VOID (APIENTRY *PTIMERAPCROUTINE)(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue );
#endif


class _OWLCLASS TWaitableTimer : public TSemaphore {
  public:
    TWaitableTimer(bool manualReset=false, LPCTSTR name = 0, LPSECURITY_ATTRIBUTES sa = 0);
    TWaitableTimer(LPCTSTR name, bool inherit = false,  uint32  dwDesiredAccess = FILE_ALL_ACCESS);
    TWaitableTimer(THandle handle);
    virtual ~TWaitableTimer();

    bool Set(const TFileTime& duetime,  int32 period=0,PTIMERAPCROUTINE compFunc=0,
             void* param=0,  bool resume=false);

    bool Cancel();
    virtual void Release();

  private:
    TWaitableTimer(const TWaitableTimer&);
    const TWaitableTimer& operator =(const TWaitableTimer&);
};


//
// class TSemaphoreSet
// ~~~~~ ~~~~~~~~~~~~~
// Semaphore object aggregator. Used to combine a set of semaphore objects so
// that they can be waited upon (locked) as a group. The lock can wait for any
// one, or all of them. The semaphore objects to be aggregated MUST live at
// least as long as this TSemaphoreSet, as it maintains pointers to them.
//
class _OWLCLASS TSemaphoreSet{
  public:
    // sems is initial array of sem ptrs, may be 0 to add sems later,
    // size is maximum sems to hold, -1 means count the 0-terminated array
    // Passing (0,-1) is not valid
    //
    TSemaphoreSet(const TSemaphore* sems[], int size = -1);
   ~TSemaphoreSet();

    void Add(const TSemaphore& sem);
    void Remove(const TSemaphore& sem);
    int  GetCount() const;
    const TSemaphore* operator [](int index) const;

    enum TWaitWhat { WaitAny = false, WaitAll = true };
    enum { NoWait = 0, NoLimit = INFINITE };

    class _OWLCLASS TLock {
      public:
        // Assumes that set is not modified while locked
        //
        TLock(const TSemaphoreSet& set, TWaitWhat wait,
              ulong timeOut = NoLimit, bool alertable = false);
        TLock(ulong msgMask, const TSemaphoreSet& set, TWaitWhat wait,
              ulong timeOut = NoLimit);
       ~TLock();

        bool WasAquired() const;    // See if one or more was aquired
        enum {
          AllAquired = -1,   // All semaphores were aquired
          TimedOut   = -2,   // None aquired: timed out
          IoComplete = -3,   //               IO complate alert
          MsgWaiting = -4,   //               Msg waiting
        };
        int  WhichAquired() const;  // See which was aquired >=0, or enum
       
        void Release(bool relinquish = false);

      protected:
        bool InitLock(int count, TWaitWhat wait, int index);

      private:
        const TSemaphoreSet* Set;
        int   Locked;    // Which one got locked, or wait code
    };
    friend class TLock;

  private:
    void Release(int index = -1);

    typedef TSemaphore* TSemaphorePtr;
    typedef HANDLE THandle;

    const TSemaphore** Sems;    // Array of ptrs to semaphores, packed at head

    int   Size;   // Size of array, i.e. maximum object count
    int   Count;  // Count of objects currently in array
};

//
// class TCriticalSection
// ~~~~~ ~~~~~~~~~~~~~~~~
// Lightweight intra-process thread synchronization. Can only be used with
// other critical sections, and only within the same process.
//
class TCriticalSection{
  public:
    TCriticalSection();
   ~TCriticalSection();

    class TLock {
      public:
        TLock(const TCriticalSection&);
       ~TLock();
      private:
        const TCriticalSection& CritSecObj;
    };
    friend class TLock;
    typedef TLock Lock;  // For compatibility with old T-less typename

  private:
    CRITICAL_SECTION CritSec;

    TCriticalSection(const TCriticalSection&);
    const TCriticalSection& operator =(const TCriticalSection&);
};

//
// class TMRSWSection
// ~~~~~ ~~~~~~~~~~~~
// MultiRead SingleWrite section only exist in Win NT
// uses Critical section if Windows 95
//
class _OWLCLASS TMRSWSection{
public:
    TMRSWSection();
    ~TMRSWSection();

    class _OWLCLASS TLock {
      public:
        TLock(const TMRSWSection&,bool shared=true,bool wait=false);
       ~TLock();
      private:
        const TMRSWSection& MRSWSecObj;
    };
    friend class TLock;

    // Function to dump the current status of the
    // resource to the debugger output.
    void Dump();

  //private:
    // RTL_RWLOCK structure.  This is the
    // structure used by ntdll.dll to handle
    // the MRSW resource.
    struct RTL_RWLOCK {
      CRITICAL_SECTION  CS;
      HANDLE            Sem1;
      uint32            SharedWaiters;
      HANDLE            Sem2;
      uint32            ExclusiveWaiters;
      uint32            NumberActive;
      uint32            OwningThreadId;
      uint32            U2;
      void*             DebugInfo;
    };
private:
    union {
      CRITICAL_SECTION CritSec;
      RTL_RWLOCK       RWSec;
#if !defined(_ANONYMOUS_STRUCT)
    }u;
#else
    };
#endif
    friend class TNtDll;
};



//
// class TSync
// class TSync::TLock
// ~~~~~ ~~~~~~~~~~~
// TSync provides a system-independent interface to build classes that act like
// monitors, i.e., classes in which only one member can execute on a particular
// instance at any one time. TSync uses TCriticalSection, so it is portable to
// all platforms that TCriticalSection has been ported to.
//
// TSync Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~
// None. TSync can only be a base class.
//
// TSync Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//     TSync(const TSync&);
//                             Copy constructor. Does not copy the
//                             TCriticalSection object.
//
//     const TSync& operator =(const TSync&);
//                             Assignment operator. Does not copy the
//                             TCriticalSection object.
//
//     class TLock;            Handles locking and unlocking of member
//                             functions.
//
// Example
// ~~~~~~~
//     class ThreadSafe : private TSync
//     {
//       public:
//         void f();
//         void g();
//       private:
//         int i;
//     };
//
//     void ThreadSafe::f()
//     {
//       TLock lock(this);
//       if (i == 2)
//         i = 3;
//     }
//
//     void ThreadSafe::g()
//     {
//       TLock lock(this);
//       if (i == 3)
//         i = 2;
//     }
//
class TSync{
  protected:
    TSync();
    TSync(const TSync&);
    const TSync& operator =(const TSync&);

    class TLock : private TCriticalSection::TLock  {
      public:
        TLock(const TSync*);
      private:
        static const TCriticalSection& GetRef(const TSync*);
    };
    friend class TLock;
    typedef TLock Lock;  // For compatibility with old T-less typename

  private:
    TCriticalSection CritSec;
};

//
// template <class T> class TStaticSync
// template <class T> class TStaticSync::TLock
// ~~~~~~~~ ~~~~~~~~~ ~~~~~ ~~~~~~~~~~~~~~~~~
// TStaticSync provides a system-independent interface to build sets of classes
// that act somewhat like monitors, i.e., classes in which only one member
// function can execute at any one time regardless of which instance it is
// being called on. TStaticSync uses TCriticalSection, so it is portable to all
// platforms that TCriticalSection has been ported to.
//
// TStaticSync Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// None. TStaticSync can only be a base class.
//
// TStaticSync Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     TStaticSync<T>(const TStaticSync<T>&);
//                             Copy constructor. Does not copy the
//                             TCriticalSection object.
//
//     const TStaticSync<T>& operator =(const TStaticSync<T>&);
//                             Assignment operator. Does not copy the
//                             TCriticalSection object.
//
//     class TLock;            Handles locking and unlocking of member
//                             functions.
//
// Example
// ~~~~~~~
//     class ThreadSafe : private TStaticSync<ThreadSafe>
//     {
//       public:
//         void f();
//         void g();
//       private:
//         static int i;
//     };
//
//     void ThreadSafe::f()
//     {
//       TLock lock;
//       if (i == 2)
//         i = 3;
//     }
//
//     void ThreadSafe::g()
//     {
//       TLock lock;
//       if (i == 3)
//         i = 2;
//     }
//
template <class T> class TStaticSync{
  protected:
    TStaticSync();
    TStaticSync(const TStaticSync<T>&);
    const TStaticSync<T>& operator =(const TStaticSync<T>&);
   ~TStaticSync();

    class TLock : private TCriticalSection::TLock
    {
      public:
        TLock() : TCriticalSection::TLock(*TStaticSync<T>::CritSec) {}
    };
    friend class TLock;
    typedef TLock Lock;  // For compatibility with old T-less typename

  private:
    static TCriticalSection* CritSec;
    static ulong Count;
};

//
// class TThread
// ~~~~~ ~~~~~~~
// TThread provides a system-independent interface to threads. With
// suitable underlying implementations the same code can be used under
// OS/2 and Win32.
//
// TThread Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~
//     Start();                Begins execution of the thread. Returns
//                             the handle of the thread.
//
//     Suspend();              Suspends execution of the thread.
//     Resume();               Resumes execution of a suspended thread.
//
//     Terminate();            Sets an internal flag that indicates that the 
//                             thread should exit. The derived class can check
//                             the state of this flag by calling
//                             ShouldTerminate().
//
//     WaitForExit(ulong timeout = NoLimit);
//                             Blocks the calling thread until the internal
//                             thread exits or until the time specified by
//                             timeout, in milliseconds,expires. A timeout of
//                             NoLimit says to wait indefinitely.
//
//     TerminateAndWait(ulong timeout = NoLimit);
//                             Combines the behavior of Terminate() and
//                             WaitForExit(). Sets an internal flag that
//                             indicates that the thread should exit and blocks
//                             the calling thread until the internal thread
//                             exits or until the time specified by timeout, in
//                             milliseconds, expires. A timeout of NoLimit says
//                             to wait indefinitely.
//
//     GetStatus();            Gets the current status of the thread.
//                             See TThread::Status for possible values.
//
//     GetPriority();          Gets the priority of the thread.
//     SetPriority();          Sets the priority of the thread.
//
//     enum TStatus;           Identifies the states that the class can be in.
//
//         Created             The class has been created but the thread has 
//                             not been started.
//         Running             The thread is running.
//
//         Suspended           The thread has been suspended.
//
//         Finished            The thread has finished execution.
//
//         Invalid             The object is invalid. Currently this happens
//                             only when the operating system is unable to
//                             start the thread.
//
//     class TThreadError;     The error class that defines the objects that 
//                             are thrown when an error occurs.
//
// TThread::ThreadError Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     enum TErrorType;        Identifies the type of error that occurred.
//
//         SuspendBeforeRun    The user called Suspend() on an object
//                             before calling Start().
//
//         ResumeBeforeRun     The user called Resume() on an object
//                             before calling Start().
//
//         ResumeDuringRun     The user called Resume() on a thread that
//                             was not Suspended.
//
//         SuspendAfterExit    The user called Suspend() on an object
//                             whose thread had already exited.
//
//         ResumeAfterExit     The user called Resume() on an object
//                             whose thread had already exited.
//
//         CreationFailure     The operating system was unable to create
//                             the thread.
//
//         DestroyBeforeExit   The object's destructor was invoked
//                             its thread had exited.
//
//         AssignError         An attempt was made to assign to an
//                             object that was not in either the
//                             Created or the Finished state.
//
//     TErrorType GetErrorType() const;
//                             Returns a code indicating the type of
//                             error that occurred.
//
//     string why();           Returns a string that describes the
//                             error. Inherited from xmsg.
//
// TThread Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     TThread();              Creates an object of type TThread.
//     virtual ~TThread();     Destroys the object.
//
//     const TThread& operator =(const TThread&);
//                             The target object must be in either the
//                             Created state or the Finished state. If
//                             so, puts the object into the Created
//                             state. If the object is not in either the
//                             Created state or the Finished state it
//                             is an error and an exception will be
//                             thrown.
//
//     TThread(const TThread&);
//                             Pts the object into the Created
//                             state, just like the default constructor.
//                             Does not copy any of the internal details
//                             of the thread being copied.
//
//     virtual ulong Run() = 0;
//                             The function that does the work. Calling
//                             Start() creates a thread that begins
//                             executing Run() with the 'this' pointer
//                             pointing to the TThread-based object.
//
//     int ShouldTerminate() const;
//                             Returns a non-zero value to indicate
//                             that Terminate() or TerminateAndWait()
//                             has been called. If this capability is
//                             being used, the thread should call
//                             ShouldTerminate() regularly, and if it
//                             returns a non-zero value the thread
//                             finish its processing and exit.
//
// Example
// ~~~~~~~
//     class TimerThread : public TThread
//     {
//       public:
//         TimerThread() : Count(0) {}
//       private:
//         int Run();
//         int Count;
//     };
//
//     int TimerThread::Run()
//     {
//       // loop 10 times
//       while (Count++ < 10) {
//         Sleep(1000);    // delay 1 second
//         cout << "Iteration " << Count << endl;
//       }
//       return 0;
//     }
//
//     int main()
//     {
//       TimerThread timer;
//       timer.Start();
//       Sleep(20000);     // delay 20 seconds
//       return 0;
//     }
//
// Internal States
// ~~~~~~~~~~~~~~~
// Created:    the object has been created but its thread has not been
//             started. The only valid transition from this state is
//             to Running, which happens on a call to Start(). In
//             particular, a call to Suspend() or Resume() when the
//             object is in this state is an error and will throw an
//             exception.
//
// Running:    the thread has been started successfully. There are two
//             transitions from this state:
//
//                 When the user calls Suspend() the object moves into
//                 the Suspended state.
//
//                 When the thread exits the object moves into the
//                 Finished state.
//
//             Calling Resume() on an object that is in the Running
//             state is an error and will throw an exception.
//
// Suspended:  the thread has been suspended by the user. Subsequent
//             calls to Suspend() nest, so there must be as many calls
//             to Resume() as there were to Suspend() before the thread
//             actually resumes execution.
//
// Finished:   the thread has finished executing. There are no valid
//             transitions out of this state. This is the only state
//             from which it is legal to invoke the destructor for the
//             object. Invoking the destructor when the object is in
//             any other state is an error and will throw an exception.
//
class _OWLCLASS TThread{
  public:
    enum { NoLimit = INFINITE };
    typedef HANDLE THandle;

    // Attach to the current running (often primary) thread
    //
    enum TCurrent {Current};
    TThread(TCurrent);

    enum TStatus { Created, Running, Suspended, Finished, Invalid };

    THandle Start();
    ulong   Suspend();
    ulong   Resume();
    bool    Sleep(long timeMS, bool alertable = false);

    virtual void    Terminate();
    ulong   WaitForExit(ulong timeout = NoLimit);
    ulong   TerminateAndWait(ulong timeout = NoLimit);

    TStatus GetStatus() const;
    uint32  GetExitCode() const;

    enum TPriority {
      Idle         = THREAD_PRIORITY_IDLE,          // -15
      Lowest       = THREAD_PRIORITY_LOWEST,        //  -2
      BelowNormal  = THREAD_PRIORITY_BELOW_NORMAL,  //  -1
      Normal       = THREAD_PRIORITY_NORMAL,        //   0
      AboveNormal  = THREAD_PRIORITY_ABOVE_NORMAL,  //   1
      Highest      = THREAD_PRIORITY_HIGHEST,       //   2
      TimeCritical = THREAD_PRIORITY_TIME_CRITICAL, //  15
    };
      
    int GetPriority() const;
    int SetPriority(int);   // Can pass a TPriority for simplicity

    class TThreadError : public TXBase  {
      public:
        enum TErrorType {
          SuspendBeforeRun,
          ResumeBeforeRun,
          ResumeDuringRun,
          SuspendAfterExit,
          ResumeAfterExit,
          CreationFailure,
          DestroyBeforeExit,
          AssignError,
          NoMTRuntime,
        };
        typedef TErrorType ErrorType;
        TErrorType GetErrorType() const;

      private:
        TThreadError(TErrorType type);
        static owl_string MakeString(ErrorType type);
        TErrorType Type;
      friend class _OWLCLASS TThread;
    };

    typedef TStatus Status;     // For compatibility with old T-less typenames
    typedef TThreadError ThreadError;

  protected:
    TThread();          // Create a thread. Derived class overrides Run()

    // Copying a thread puts the target into the Created state
    //
    TThread(const TThread&);
    const TThread& operator =(const TThread&);

    virtual ~TThread();

    bool    ShouldTerminate() const;
    void    Exit(ulong code);  // Alternative to returning from Run()

  private:
    virtual int Run();

    TStatus CheckStatus() const;

#if defined(BI_MULTI_THREAD_RTL)
#if  defined(BI_COMP_BORLANDC)
    static void _USERENTRY Execute(void* thread);
#else
    static uint __stdcall Execute(void* thread);
#endif
#elif defined(BI_PLAT_WIN32)
    static DWORD __stdcall Execute(void* thread);
#endif

    DWORD ThreadId;
    THandle Handle;
    mutable TStatus Stat;
    bool TerminationRequested;
    bool Attached;
};

////////////////////////////////////////////////////////////////////////////
// Created by Yura Bidus
// Multithread OWL internal support
//
// Thread Local Storage
// class TTlsAllocator
// ~~~~~ ~~~~~~~~~~~~~
// class intended to be static.
//
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class TTlsAllocator {
  public:
    TTlsAllocator()              { index = ::TlsAlloc();}
    ~TTlsAllocator()            { ::TlsFree(index);    }

    bool  IsOk()                { return index != 0xFFFFFFFF; }
    bool  SetValue(void* value)  { return ::TlsSetValue(index, value); }
    void* GetValue()            { return ::TlsGetValue(index);        }

  private:
    uint32 index;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

#if defined(BI_MULTI_THREAD_RTL)

///
// All classes used in local/tls process data have to be derived from here
//
// class TLocalObject
// ~~~~~ ~~~~~~~~~~~~
//
class TLocalObject : public TStandardAllocator /*TLocalAllocator*/ {
  public:
    TLocalObject(){}
    virtual ~TLocalObject(){}
};


//
class TLocalData: public TLocalObject {
  typedef TMIPtrArray<TLocalObject*,TLocalObject*,TLocalObject>  TLocalObjectArray;
  public:
    TLocalData(){}
    virtual ~TLocalData(){}

    TLocalObject* GetData(int index)  
      { 
        return index < (int)Data.Size() ? Data[index] : 0;
      }
    void SetData(int index, TLocalObject* obj) 
      { 
        while(index >= (int)Data.Size())
          Data.Add(0);
        Data[index] = obj;
      }
    int  AddData(TLocalObject* data) { return Data.Add(data); }
  protected:
    TLocalObjectArray Data;
};

//
// class TTlsDataManager
// ~~~~~ ~~~~~~~~~~~~~~~
//
class TTlsDataManager : public TLocalObject {
  typedef TMIPtrArray<TLocalData*,TLocalData*,TLocalObject>  TLocalDataArray;
  public:
    TTlsDataManager()
      { 
        CHECK(Allocator.IsOk()); 
      }
    virtual ~TTlsDataManager()
      {
        TMRSWSection::TLock __lock(Lock,false,true);
        Data.Flush(true);
      }

    TLocalObject* GetData(int index)
      { 
        TLocalData* data = (TLocalData*)Allocator.GetValue();
        return (data ? data : CreateData())->GetData(index);
      }
    void SetData(int index, TLocalObject* obj)
      {
        TLocalData* data = (TLocalData*)Allocator.GetValue();
        (data ? data : CreateData())->SetData(index, obj);
      }
    int AddData(TLocalObject* obj)
      {
        TLocalData* data = (TLocalData*)Allocator.GetValue();
        return (data ? data : CreateData())->AddData(obj);
      }
  protected:
    TLocalData* CreateData()
      {
        TMRSWSection::TLock __lock(Lock,false,true);
        TLocalData* data = new TLocalData();
        Data.Add(data);
        Allocator.SetValue(data); 
        return data;
      }

  protected: // global static data 
    TTlsAllocator    Allocator;
    TLocalDataArray  Data;
    TMRSWSection    Lock;
};

//
//
//
_OWLFUNC(TTlsDataManager*) GetTlsDataMng();


//
// class TProcessContainer<T> 
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~
// Process global data container
//
// Usage
//  struct MyProsessData{
//    ... some data
//  };
//  static MyProsessData& GetProcessData()
//  {
//      static TProcessContainer<MyProsessData> __Data
//      return __Data;
//  }
//
template <class T> class TProcessContainer: public TLocalObject{
  public:
    TProcessContainer():Object(0){}
    virtual ~TProcessContainer(){ delete Object;}

    inline T* GetData()
      {
        if(!Object) // throw bad_alloc if error ??
          Object = new T;
        // check that T derived from TLocalObject
        CHECK(dynamic_cast<TLocalObject*>(Object));
        return Object;
      }
    inline T* QueryData()  {  return Object;    }
    inline operator T*()  { return GetData(); }
    inline operator T&()  { return *GetData();}
    inline T* operator->(){ return GetData(); }

  protected:
    T* Object;
};

//
//
// class TTlsContainer<T>
// ~~~~~ ~~~~~~~~~~~~~~~~
//
// Thread local storage container
//
// Usage
//  struct MyTreadData{
//    ... some data
//  };
//  static MyTreadData& GetThreadData()
//  {
//      static TTlsContainer<MyTreadData> __Data
//      return __Data;
//  }
//
template <class T> class TTlsContainer : public TLocalObject{
  public:
    TTlsContainer():Index(-1){}
    virtual ~TTlsContainer(){/* TIPtrArray will destroy our Object */  }

    inline T* GetData()
      {
        if(Index == -1){
          T* t = CreateData();
          Index = GetTlsDataMng()->AddData(t);
          return t;
        }
        else{
          T* t = (T*)GetTlsDataMng()->GetData(Index);
          if(t==0){
            t = CreateData();
            GetTlsDataMng()->SetData(Index, t);
          }
          return t;
        }
      }
    inline T* QueryData()
      {
        return Index == -1 ? (T*)0 : (T*)GetTlsDataMng()->GetData(Index);
      }
    inline operator T*()      { return GetData();    }
    inline operator T&()      { return *GetData();  }
    inline T* operator->()    { return GetData();    }

  protected:
    T* CreateData()
      {
        T* t = new T;
        CHECK(dynamic_cast<TLocalObject*>(t));
        return t;
      }

  protected:
    int  Index;
};
#endif

//
// Internal use
// ~~~~~~~~~~~~
//long TlsAddRefs();
//long TlsRelease();


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementation
//


//----------------------------------------
// TSemaphore Win32

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSemaphore::~TSemaphore()
{
  ::CloseHandle(Handle);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
inline TSemaphore::operator TSemaphore::THandle() const
{
  return Handle;
}

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSemaphore::TLock::TLock(const TSemaphore& sem, ulong timeOut, bool alertable)
:
  Sem(0)
{
  if (::WaitForSingleObjectEx(sem, timeOut, alertable) == 0)
    Sem = &sem;
}

//
inline TSemaphore::TLock::~TLock()
{
  Release();
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
inline bool TSemaphore::TLock::WasAquired() const
{
  return Sem != 0;
}

//
// Release but hang on to the semaphore
//
inline void TSemaphore::TLock::Release(bool relinquish)
{
  if (Sem) {
    CONST_CAST(TSemaphore*,Sem)->Release();
    if (relinquish)
      Sem = 0;
  }
}

//----------------------------------------
// TMutex Win32

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TMutex::TMutex(LPCTSTR name, LPSECURITY_ATTRIBUTES sa)
 : Shared(false)
{
  Handle = ::CreateMutex(sa, false, name);  // don't aquire now
  if (Handle && (GetLastError() == ERROR_ALREADY_EXISTS))
    Shared = true;
}

//
// Open an existing mutex. Fails if not there.
//
inline TMutex::TMutex(LPCTSTR name, bool inherit, uint32 access)
  : Shared(false)
{
  Handle = ::OpenMutex(access, inherit, name);
  if (Handle)
    Shared = true;
}

//
//
//
inline TMutex::TMutex(THandle handle)
  : Shared(false)
{
  if (::DuplicateHandle(::GetCurrentProcess(), handle,
                        ::GetCurrentProcess(), &Handle,
                        0, false, DUPLICATE_SAME_ACCESS))
  {
    Shared = true;
  }
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
//
//
inline bool TMutex::IsShared()
{
  return Shared;
}

//----------------------------------------
// TCountedSemaphore Win32

//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TCountedSemaphore::TCountedSemaphore(int initialCount, int maxCount,
                                            LPCTSTR name,
                                            LPSECURITY_ATTRIBUTES sa)
{
  Handle = ::CreateSemaphore(sa, initialCount, maxCount, name);
}

//
inline TCountedSemaphore::TCountedSemaphore(LPCTSTR name, bool inherit,
                                            uint32 access)
{
  Handle = ::OpenSemaphore(access, inherit, name);
}

//
inline TCountedSemaphore::TCountedSemaphore(THandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle,
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

//----------------------------------------
// TEventSemaphore Win32

inline TEventSemaphore::TEventSemaphore(bool manualReset, bool initialState,
                                        LPCTSTR name,
                                        LPSECURITY_ATTRIBUTES sa)
{
  Handle = ::CreateEvent(sa, manualReset, initialState, name);
}

//
inline TEventSemaphore::TEventSemaphore(LPCTSTR name, bool inherit,
                                        uint32 access)
{
  Handle = ::OpenEvent(access, inherit, name);
}

//
inline TEventSemaphore::TEventSemaphore(THandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle,
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

inline void TEventSemaphore::Set()
{
  ::SetEvent(*this);
}

inline void TEventSemaphore::Reset()
{
  ::ResetEvent(*this);
}

inline void TEventSemaphore::Pulse()
{
  ::PulseEvent(*this);
}

//----------------------------------------
// TWaitableTimer Win NT or Win98
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TWaitableTimer::TWaitableTimer(THandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle, ::GetCurrentProcess(), 
                    &Handle, 0, false, DUPLICATE_SAME_ACCESS);
}

inline  TWaitableTimer::~TWaitableTimer()
{
  ::CloseHandle(Handle);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif
//----------------------------------------
// TSemaphoreSet Win32

inline int TSemaphoreSet::GetCount() const
{
  return Count;
}

inline const TSemaphore* TSemaphoreSet::operator [](int index) const
{
  return (index >= 0 && index < Count) ? Sems[index] : 0;
}

//
inline bool TSemaphoreSet::TLock::WasAquired() const
{
  return Set != 0;
}

//
// Which one was locked, all locked code, or lock fail code
//
inline int TSemaphoreSet::TLock::WhichAquired() const
{
  return Locked;
}
       

//----------------------------------------------------------------------------
// TCriticalSection Win32
//

//
// Use system call to initialize the CRITICAL_SECTION object.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TCriticalSection::TCriticalSection()
{
  ::InitializeCriticalSection(CONST_CAST(CRITICAL_SECTION*,&CritSec));
}

//
// Use system call to destroy the CRITICAL_SECTION object.
//
inline TCriticalSection::~TCriticalSection()
{
  ::DeleteCriticalSection(CONST_CAST(CRITICAL_SECTION*,&CritSec));
}

//
// Use system call to lock the CRITICAL_SECTION object.
//
inline TCriticalSection::TLock::TLock(const TCriticalSection& sec)
:
  CritSecObj(sec)
{
  ::EnterCriticalSection(CONST_CAST(CRITICAL_SECTION*,&CritSecObj.CritSec));
}

//
// Use system call to unlock the CRITICAL_SECTION object.
//
inline TCriticalSection::TLock::~TLock()
{
  ::LeaveCriticalSection(CONST_CAST(CRITICAL_SECTION*,&CritSecObj.CritSec));
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// TCriticalSection OS2
//


//----------------------------------------------------------------------------

//
// Copy constructor does not copy the TCriticalSection object,
// since the new object is not being used in any of its own
// member functions. This means that the new object must start
// in an unlocked state.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSync::TSync()
{
}

//
inline TSync::TSync(const TSync&)
{
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Does not copy the TCriticalSection object, since the new  object is not
// being used in any of its own member functions.
// This means that the new object must start in an unlocked state.
//
inline const TSync& TSync::operator =(const TSync&)
{
  return *this;
}

//
// Locks the TCriticalSection object in the TSync object.
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TSync::TLock::TLock(const TSync* sync)
:
  TCriticalSection::TLock(GetRef(sync))
{
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Returns a reference to the TCriticalSection object contained in the TSync
// object.
//
// In the diagnostic version, checks for a null pointer.
//
inline const TCriticalSection& TSync::TLock::GetRef(const TSync* sync)
{
  CHECK(sync != 0);
  return sync->CritSec;
}

//----------------------------------------------------------------------------

//
// Instantiate the data members.
//
template <class T> TCriticalSection* TStaticSync<T>::CritSec;
template <class T> ulong TStaticSync<T>::Count;

//
// If this is the first TStaticSync<T> object to be constructed, create the
// semaphore.
//
// The copy constructor only has to increment the count, since there will
// already be at least one TStaticSync<T> object, namely, the one being copied.
//
template <class T> inline TStaticSync<T>::TStaticSync()
{
  if (Count++ == 0)
    CritSec = new TCriticalSection;
}

template <class T> inline TStaticSync<T>::TStaticSync(const TStaticSync<T>&)
{
  Count++;
}

//
// TStaticSync<T> assignment operator
//
template <class T>
inline const TStaticSync<T>& TStaticSync<T>::operator =(const TStaticSync<T>&)
{
  return *this;
}

//
// If this is the only remaining TStaticSync<T> object, destroy the semaphore.
//
template <class T> inline TStaticSync<T>::~TStaticSync()
{
  if (--Count == 0)
    delete CritSec;
}

//----------------------------------------------------------------------------

//
// Put this tread to sleep for a given number of milliseconds, with an optional
// wakeup on an IO completion. Return true if wakeup is due to IO completion.
//
inline bool TThread::Sleep(long timeMS, bool alertable)
{
  return ::SleepEx(timeMS, alertable) == WAIT_IO_COMPLETION;
}

//
// If the thread is marked as Running it may have terminated without our
// knowing it, so we have to check.
//
inline TThread::TStatus TThread::GetStatus() const
{
  if (Stat == Running)
    Stat = CheckStatus();
  return Stat;
}

//
inline uint32  TThread::GetExitCode() const
{
  uint32 code;
  ::GetExitCodeThread(Handle, (DWORD*)&code);
  return code;
}

//
// Direct OS call under Win32. 
//
inline int TThread::GetPriority() const
{
  return ::GetThreadPriority(Handle);
}

//
inline bool TThread::ShouldTerminate() const
{
  return TerminationRequested;
}

//
inline TThread::TThreadError::TErrorType TThread::TThreadError::GetErrorType() const
{
  return Type;
}

__OWL_END_NAMESPACE

#if defined(BI_OWL_NO_po)
# pragma option -po.
#endif

#endif  // OWL_THREAD_H
