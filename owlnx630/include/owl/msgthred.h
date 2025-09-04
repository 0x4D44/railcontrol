//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Author Yura Bidus
//
// Definition of message queue oriented thread class support. Degenerates to a
// simple message queue owner under non-threaded environments
//----------------------------------------------------------------------------

#if !defined(OWL_MSGTHRED_H)
#define OWL_MSGTHRED_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if defined(BI_MULTI_THREAD) && !defined(OWL_THREAD_H)
# include <owl/thread.h>
#endif
#if !defined(OWL_WSYSINC_H)
#  include <owl/wsysinc.h>
#endif
# if !defined(OWL_SYSTEM_H)
#   include <owl/system.h>
# endif
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

class _OWLCLASS TMsgThread;

//
// class TMsgThread
// ~~~~~ ~~~~~~~~~~
#if defined(BI_MULTI_THREAD_RTL)
class _OWLCLASS TMsgThread :  public TThread
{
#else
class _OWLCLASS TMsgThread
{
  public:
    enum TCurrent {Current};
#endif

  public:
    TMsgThread(TCurrent);

    // Message queue functions
    //
    virtual int      MessageLoop();
    virtual bool     IdleAction(long idleCount);
    virtual bool     ProcessMsg(MSG& msg);

    bool             PumpWaitingMessages();
    void             FlushQueue();

    // Is this queue thread running its message loop?
    //
    bool             IsRunning() const;

    // Message queue thread synchronization mechanism. Real multi-threading
    // requires compiler & RTL support (-MT)
    //
#if defined(BI_MULTI_THREAD_RTL)
# if defined(BI_USE_CRITICAL_SECTION)
    // Using a critical section is much faster, but prevents
    // cross-process locking and potentially breaks existing code.
    typedef TCriticalSection TMsgMutex;
# else
    typedef TMutex TMsgMutex;
# endif
    void    EnableMultiThreading(bool enable);
    TMsgMutex* GetMutex();

    class _OWLCLASS TQueueLock {
      public:
        TQueueLock(TMsgThread& app);
       ~TQueueLock();
        void Release(bool relinquish = false);
      private:
#if defined(BI_COMP_BORLAND)
        TMsgThread::TMsgMutex::TLock *Lock;
#else
        TMsgMutex::TLock *Lock;
#endif
    };
#endif

  protected:
#if defined(BI_MULTI_THREAD_RTL)  // No real threads without RTL support
    TMsgThread();
#endif

    // Override this for created threads, app thread is Run() from OwlMain
    //
    virtual int      Run();

    virtual void     InitInstance();
    virtual int      TermInstance(int status);

    bool          BreakMessageLoop;     // Message loop is broken via WM_QUIT
    int           MessageLoopResult;    // Return value from message loop

    //!CQ still need this with thread state?
    bool          LoopRunning;          // Track if the loop is running

#if defined(BI_MULTI_THREAD_RTL)
    //!CQ Just use a TMutex directly???
    class _OWLCLASS TAppMutex {
      public:
        TAppMutex();
       ~TAppMutex();
        TMsgMutex* GetMutex();
      private:
        TMsgMutex *Mutex;
    };

    TAppMutex     Mutex;                // Mutex to prevent multiple threads
                                        // from processing messages at the
                                        // same time.
  private:
    bool          UseMutex;
#endif
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations

//
// Return true if the thread is currently running.
//
inline bool TMsgThread::IsRunning() const
{
  return LoopRunning;
}

#if defined(BI_MULTI_THREAD_RTL)

//
// Enable/disable the use of the mutex to synchronize access to the msg queue.
// Only when enabled, will the TMsgThread lock its msg queue mutex.
//
inline void TMsgThread::EnableMultiThreading(bool enable)
{
  UseMutex = enable ? TSystem::SupportsThreads() : false;
}

//
// Get this msg thread's mutex. Return 0 if mutexes are not supported, or are
// not enabled for this thread.
//
inline TMsgThread::TMsgMutex* TMsgThread::GetMutex()
{
  return UseMutex ? Mutex.GetMutex() : 0;
}

//
// The mutex is always constructed/destructed if the system supports threads
//
inline TMsgThread::TAppMutex::TAppMutex()
:
  Mutex(NULL)
{
  if (TSystem::SupportsThreads())
    Mutex = new TMsgThread::TMsgMutex;
}

//
inline TMsgThread::TAppMutex::~TAppMutex()
{
  if (TSystem::SupportsThreads())
    delete Mutex;
}

//
// Get the apps mutex. Return 0 if mutexs are not supported, or are not enabled
// for this app.
//
inline TMsgThread::TMsgMutex* TMsgThread::TAppMutex::GetMutex()
{
  return Mutex;
}

//
// Only lock if the mutex is available. Do nothing otherwise.
//
inline TMsgThread::TQueueLock::TQueueLock(TMsgThread& app)
  : Lock(NULL)
{
  TMsgMutex* appMutex = app.GetMutex();
  if (appMutex)
    Lock = new TMsgMutex::TLock(*appMutex);
}

//
// Destruct the real lock if it was constructed (locked)
//
inline TMsgThread::TQueueLock::~TQueueLock()
{
  delete Lock;
}

//
// Release the real lock if it was locked
//
inline void TMsgThread::TQueueLock::Release(bool relinquish)
{
  if (Lock) {
    CHECK(relinquish == true);
    delete Lock;
    Lock = NULL;
  }
}

#endif  // BI_MULTI_THREAD_RTL

__OWL_END_NAMESPACE



#endif  // OWL_MSGTHRED_H
