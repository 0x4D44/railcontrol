//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2010-03-24 11:54:28 $
//
// Internal window object instance proc creation & maintenance.
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include <owl/window.h>
#include <owl/private/memory.h>
#include <stddef.h>

#if !defined(__CYGWIN__) && !defined(WINELIB)
# include <dos.h>
#endif

#if defined(BI_MULTI_THREAD_RTL)
# include <owl/thread.h>
#endif

__OWL_BEGIN_NAMESPACE

//------------------------------------------------------------------------------

//
// Diagnostics
//
OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlWin);
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlThunk, 1, 1);  // diag. group for thunks [VH-25MAR2005]

//
// Returns the internal version number. See "owl/version.h".
//
_OWLFUNC(uint32)
OWLGetVersion()
{
  return OWLInternalVersion;
}

//
// Returns a pointer to the global OWL module object.
//
_OWLFUNC(TModule*)
OWLGetModule()
{
  return gModule;
}

//----------------------------------------------------------------------------

//
// This internal singleton class encapsulates the global "creation window" pointer.
// The creation window is the window currently being constructed/initialized.
// A thread-safe implementation is used for multi-threaded builds.
//
class TCreationWindow
{
  TCreationWindow() {} // Private to prohibit instantiation.

#if defined(BI_MULTI_THREAD_RTL)

  //
  // Thread-safe encapsulation of a creation window pointer.
  //
  struct TInstance : public TLocalObject
  {
    TInstance() : Window(0) {}
    TWindow* Window;
  };

public:
  //
  // Get a reference to the pointer to the creation window.
  // Returns the pointer to the current window being created in this thread.
  //
  static TWindow*& GetInstance() 
  {
    static TTlsContainer<TInstance> container;
    TInstance& instance = container; // implicit accessor
    return instance.Window;
  };

#else // Non-threaded code.

public:
  //
  // Get a reference to the pointer to the current window being created.
  //
  static TWindow*& GetInstance() 
  {
    static TWindow* creationWindow = 0;
    return creationWindow;
  };

#endif
};

//
// Accessor of the "creation window", the global pointer to the current window being created.
// Internal function.
//
_OWLFUNC(TWindow*)
GetCreationWindow()
{
  return TCreationWindow::GetInstance();
}

//
// Set the "creation window", the global pointer to the current window being created.
// Internal function.
//
_OWLFUNC(void)
SetCreationWindow(TWindow* w)
{
  // Theoretically, there should always be only one non-zero
  // "creation window" pointer. i.e. we never want to have a case where
  // we're holding on to a pointer waiting to be thunked, and this function
  // is invoked with yet another valid pointer (Otherwise, we'll fail to
  // thunk a HWND<->TWindow* pair.
  //
  PRECONDITION(GetCreationWindow() == 0 || w == 0);
  TRACEX(OwlWin, 1, _T("SetCreationWindow: Old(") << (void*)GetCreationWindow() <<\
                    _T("), New(") << (void*)w << _T(')'));
  TCreationWindow::GetInstance() = w;
}

//
// Global GetWindowPtr() message ID used for registered message
//
_OWLDATA(uint) GetWindowPtrMsgId = 0;


//----------------------------------------------------------------------------

//
// Returns the correct default window proc, depending on build options.
// Internal function.
//
WNDPROC 
GetDefWindowProc()
{
  // For Win32, find the default window proc in the right DLL for this build.

# if defined(UNICODE)
#   define DEFWINDOWPROC "DefWindowProcW"
# else
#   define DEFWINDOWPROC "DefWindowProcA"
# endif

# if defined(UNICODE) && defined(USE_UNICOWS)
#   define DEFWINDOWPROC_MODULE _T("UNICOWS")
# else
#   define DEFWINDOWPROC_MODULE _T("USER32")
# endif

  HMODULE module = GetModuleHandle(DEFWINDOWPROC_MODULE);
  return reinterpret_cast<WNDPROC>(GetProcAddress(module, DEFWINDOWPROC));
}

//
// Initial WndProc called when an OWL window is first created.
// Subclasses the window function by installing the window proc then calls the
// window proc to get this first message to the window.
//
TResult CALLBACK OWL_EXPORT16
TWindow::InitWndProc(TWindow::THandle hWnd, uint msg, TParam1 param1,TParam2 param2)
{
  // Get the creation window (i.e. the current TWindow being initialized at this point).
  // If there's no creation window, i.e. we'll be aliasing a resource, then we
  // can't do anything now.  Create() will take care of it later.
  //
  TWindow* w = GetCreationWindow();
  if (!w)
    return ::DefWindowProc(hWnd, msg, param1, param2);

  // Reset the creation window so that it is not inadvertently used again.
  //
  SetCreationWindow(0);

  // Initialize the  window.
  // Assign a default window proc before we subclass,
  // otherwise we would subclass InitWndProc which would be bad.
  //
  w->SetHandle(hWnd);
  w->SetWindowProc(GetDefWindowProc());
  w->SubclassWindowFunction(); // Install the instance window proc.

  // Call the instance window proc.
  //
  WNDPROC wndproc = w->GetWindowProc();
  return (*wndproc)(hWnd, msg, param1, param2);
}

//----------------------------------------------------------------------------

//
// Implementation of the instance window proc interface using lookup
//
#if defined(BI_NOTHUNK)

//
// The name of the Window Property used to associate a HWND and a TWindow object.
//
const char OwlObjectProperty [] = "OWL_TWINDOW"; // NB! Should be an ANSI string, not UNICODE.

//
// Returns StdWndProc as the instance proc for all windows. 
//
WNDPROC
TWindow::CreateInstanceProc()
{
  return TWindow::StdWndProc;
}

//
// Creates an association between the window and its handle.
//
void
TWindow::InitInstanceProc()
{
  PRECONDITION(GetHandle());
  BOOL r = ::SetPropA(GetHandle(), OwlObjectProperty, reinterpret_cast <HANDLE> (this));
  WARNX(OwlThunk, !r, 0, _T("InitInstanceProc: Handle registration failed! GetLastError: ") << ::GetLastError());
  CHECK(r);
}

// 
// Removes the association between the window and its handle.
//
void
TWindow::FreeInstanceProc()
{
  HANDLE h = ::RemovePropA(GetHandle(), OwlObjectProperty);
  WARNX(OwlThunk, !h, 0, _T("FreeInstanceProc: Handle removal failed! GetLastError: ") << ::GetLastError());
}

//
// Window function called for normal, initialized, OWL windows.
// Forwards the call to the target window via a window pointer lookup.
//
_OWLFUNC(TResult CALLBACK)
TWindow::StdWndProc(HWND hWnd, uint msg, TParam1 param1, TParam2 param2)
{
  // Lookup the window pointer.
  //
  HANDLE hObj = ::GetPropA(hWnd, OwlObjectProperty);
  WARNX(OwlThunk, !hObj, 0, _T("StdWndProc: Handle lookup failed! GetLastError: ") << ::GetLastError());
  TWindow* w = reinterpret_cast <TWindow*> (hObj);
  CHECK(w);

  // Dispatch the message to the window.
  //
  return w->ReceiveMessage(msg, param1, param2);
}

#else // nothunk/thunk

//----------------------------------------------------------------------------

//
// This singelton class provides a memory heap for thunks. 
// The implementation is compatible with DEP (Data Execution Prevention).
//
class TThunkAllocator 
{
public:
  //
  // Provides access to the singleton allocator.
  //
  static TThunkAllocator& GetInstance()
  {
    static TThunkAllocator TheAllocator;
    return TheAllocator;
  }

  //
  // Destructor - destroys the heap.
  //
  ~TThunkAllocator()
  {
    PRECONDITION(Handle);
    Dead = true;
    BOOL r = ::HeapDestroy(Handle);
    WARNX(OwlThunk, !r, 0, _T("HeapDestroy failed! GetLastError: ") << ::GetLastError());
    TRACEX(OwlThunk, 1, _T("TThunkAllocator destructed @") << Handle);
  }

  //
  // Allocates memory from the private heap.
  //
  LPVOID Allocate(size_t size)
  {
    PRECONDITION(Handle);
    LPVOID p = ::HeapAlloc(Handle, 0, size);
    WARNX(OwlThunk, !p, 0, _T("HeapAlloc failed!"));
    if (!p) TXOutOfMemory().Throw();
    WARNX(OwlThunk, (++Count > Maximum ? (Maximum = Count) : 0), 1, _T("New thunk count record: ") << Maximum);
    return p;
  }

  //
  // Frees memory previously allocated from the private heap.
  //
  void Free(LPVOID p)
  {
    PRECONDITION(Handle);
    BOOL r = ::HeapFree(Handle, 0, p);
    WARNX(OwlThunk, !r, 0, _T("HeapFree failed! GetLastError: ") << ::GetLastError());
    WARNX(OwlThunk, (--Count < 0 ? (Count = 0, true) : false), 1, _T("Negative thunk count!"));
  }

  //
  // This predicate indicates whether the allocator has been destructed.
  // It is used for handling the special case when the allocator singleton is
  // destructed at the end of a program before the last window is destructed.
  // This can happen when the application is a static (global) object.
  //
  static bool IsDead()
  {
    return Dead;
  }

private:
  static bool Dead; // Destruction flag
  HANDLE Handle; // Handle for the private heap (returned by HeapCreate)

  //
  // Diagnostics
  //
#if defined(__WARN)
  unsigned Count; // The current number of thunks currently allocated.
  unsigned Maximum; // The max number of thunks allocated so .
#endif

  //
  // Heap creation parameters
  //
  enum
  {
    Flags = 0x00040000, // = HEAP_CREATE_ENABLE_EXECUTE, // Allow thunks to run.
    InitialSize = 32768, // Room for 1024 32-byte thunks.
    MaxSize = 0, // Set no limits, i.e. dynamic growth. (Any benefits to limiting it?)
  };

  //
  // Private constructor - creates the heap.
  //
  TThunkAllocator()
    : Handle(::HeapCreate(Flags, InitialSize, MaxSize))
#if defined(__WARN)
    , Count (0), Maximum (0) // Diagnostics
#endif
  {
    WARNX(OwlThunk, !Handle, 0, _T("HeapCreate failed! GetLastError: ") << ::GetLastError());
    if (!Handle) TXOutOfMemory().Throw();
    TRACEX(OwlThunk, 1, _T("TThunkAllocator constructed @") << Handle);
  }

  //
  // Prohibit copying (i.e. not implemented)
  //
  TThunkAllocator(const TThunkAllocator&);
  TThunkAllocator& operator=(const TThunkAllocator&);
};

//
// Destruction flag
//
bool TThunkAllocator::Dead = false;

//
// This is a template for the code generated for each thunk.
// The code simply puts a windows pointer in EAX and forwards it to a window proc.
// Each thunk inserts its own window pointer and proc displacement.
//
#if 0
__declspec(naked) static void ThunkTemplate ()
{
  __asm 
  {
    mov eax, 0xBAADF00D ; // Placeholder for window pointer.
    jmp offset TWindow::StdWndProc ; // Placeholder for proc displacement.
  }
}
#endif

//
// The thunk struct requires packing.
//
#include <pshpack1.h>     

//
// This class defines a thunk that contains an associated TWindow pointer.
// NB! This struct MUST be byte packed!
//
struct TInstanceThunk 
{
  const uint8 MOV_EAX; // Instruction - move immediate value into EAX.
  TWindow* Window; // Pointer moved into EAX.
  const uint8 JMP; // Instruction - immediate relative jump.
  int32 ProcDisplacement; // Jump distance to the window proc.

  //
  // Constructor - generates the thunking code.
  //
  TInstanceThunk(TWindow* w, WNDPROC proc)
    : MOV_EAX (0xB8), JMP (0xE9)
  {
    Window = w;
    ProcDisplacement = reinterpret_cast<uint8*>(proc) -
      reinterpret_cast<uint8*>(this) -
      sizeof(TInstanceThunk);
  }

  //
  // Class-local new operator - allocates virtual memory with DEP support.
  //
  void* operator new(size_t n)
  {
    //
    // For robustness we test that the allocator is alive here.
    // See notes in TInstanceThunk::delete.
    //
    LPVOID p = TThunkAllocator::IsDead() ? 0 : TThunkAllocator::GetInstance().Allocate (n);
    if (!p)
      throw TXOutOfMemory();
    TRACEX(OwlThunk, 1, _T("TInstanceThunk allocated @") << p);
    return p;
  }

  //
  // Class-local delete operator. Frees virtual memory.
  //
  void operator delete(void* p, size_t)
  {
    WARNX(OwlThunk, !p, 0, _T("TInstanceThunk::delete called with null pointer."));
    if (!p) return;
    //
    // We test that the allocator is not already dead here.
    // This should not happen, but can do so if the application is a global object.
    // This is unsafe practice though. The application object should be stack or heap allocated.
    //
    if (!TThunkAllocator::IsDead()) 
    TThunkAllocator::GetInstance().Free(p);
    WARNX(OwlThunk, TThunkAllocator::IsDead(), 0, 
      _T("TInstanceThunk::delete called after destruction of TThunkAllocator."));
    TRACEX(OwlThunk, 1, _T("TInstanceThunk deallocated @") << p);
  }
};

//
// Restore packing settings.
//
#include <poppack.h>

//
// Creates a thunk for the given window. 
// When called the thunk will redirect to the thunk proc given.
// That thunk proc should redirect to TWindow::ReceiveMessage by
// using the TWindow pointer passed by the thunk in a register.
//
WNDPROC
TWindow::CreateInstanceProc()
{
  return reinterpret_cast<WNDPROC>(new TInstanceThunk(this, &TWindow::StdWndProc));
}

//
// Nothing to do.
//

void
TWindow::InitInstanceProc()
{
}

// 
// Releases the given thunk.
//
void
TWindow::FreeInstanceProc()
{
  delete reinterpret_cast<TInstanceThunk*>(GetInstanceProc ());
}

// NAKED function attribute macro
// Prevents function prolog/epilog with MSVC++.
// Useful for preventing register scratching.
//
#if defined(BI_COMP_MSC)
#define NAKED __declspec(naked) 
#else
#define NAKED 
#endif 

//
// Window function called for normal, initialized, OWL windows.
// Forwards the call to the target window via the per-window thunk. 
// The window pointer is set up in a register by the thunk before being
// passed to StdWndProc.
//

NAKED _OWLFUNC(TResult CALLBACK)
TWindow::StdWndProc(HWND, uint msg, TParam1 param1, TParam2 param2)
{
  TWindow* w;

#if defined(BI_COMP_BORLANDC)
  
  // Implementation for Borland C++
  //
  w = (TWindow*)_EAX;
//  CHECK(w);     //Jogy - this CHECK was messing things with diagnostic builds
  return w->ReceiveMessage(msg, param1, param2);

#elif defined(BI_COMP_MSC)

  // Implementation for MS Visual C++

  // Custom prolog
  // Preserve registers to be on the safe side.
  //
  _asm { 
    push ebp 
    mov  ebp,esp 
    sub  esp,__LOCAL_SIZE 
    push ebx               
    push edi 
    push esi 
  } 

  // Get the window pointer and dispatch the message.
  //
  _asm mov w, eax 
//  CHECK(w);     //Jogy - this CHECK may mess things with diagnostic builds
  w->ReceiveMessage(msg, param1, param2);

  // Custom epilog
  // Restore registers.
  //
  _asm {
    pop  esi
    pop  edi 
    pop  ebx 
    mov  esp,ebp 
    pop  ebp 
    ret  SIZE HWND+SIZE msg+SIZE param1+SIZE param2 
  } 

#elif defined(BI_COMP_GNUC)

  // Implementation for GNU C
  //
  __asm__ ("movl %%eax , %0" : "=g" (w) : );
//  CHECK(w);     //Jogy - this CHECK may mess things with diagnostic builds
  return w->ReceiveMessage(msg, param1, param2);

#else

# error TWindow::StdWndProc: Unable to generate code for this compiler.

#endif
}

#endif // nothunk/thunk


__OWL_END_NAMESPACE
// ==========================================================================

