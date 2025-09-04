//----------------------------------------------------------------------------
// ObjectWindows
// (C) Copyright 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2008-05-13 09:56:33 $
//
//   Diagnostic macros for checking:
//
//     PRECONDITION[X]  Assert that a precondition is met
//     CHECK[X]         Check that a state meets assumptions
//     TRACE[X]         Output a tracing text stream
//     WARN[X]          Output a warning text stream
//     _OWL_BREAK       Macro
//     IFDBG(a)         Execute if Debug
//     IFNDBG(a)        Execute if NotDebug  
//     VALIDPTR(p)      Check Pointer
//     VALIDORNULLPTR(p)Check Pointer 
//     COMPILE_ASSERT(B) This guy generates a compiler error such as
//                       'Array must have at least one element'
//                        if the boolean condition argument is false:
//     VERIFY(p)        Verify Output  
//     VERIFYX(p,s)      Verify Output and print message 
//     IFDIAG(a)         Execute for Diagnostic build
//     IFNDIAG(a)       Execute for Non Diagnostic build
//
//     TRACE_FUNCTIONX(g,f)
//     TRACE_FUNCTIONXP(g,f,p)
//     TRACE_FUNCTION(f)      
//     TRACE_FUNCTIONP(f,p)    
//
//   Based on the following switches:
//     __DEBUG = 0   PRECONDITION and CHECK are nops
//     __DEBUG = 1   PRECONDITION is active
//     __DEBUG = 2   PRECONDITION and CHECK are active
//     __TRACE       When defined enables TRACE
//     __WARN        When defined enables WARN
//----------------------------------------------------------------------------
#if defined(UNIX)
#include <assert.h>  // Needed for OWL_BREAK definition under UNIX
#endif

#if !defined(OWL_PRIVATE_CHECKS_H)
#define OWL_PRIVATE_CHECKS_H

#if defined(SERVICES_CHECKS_H)
#error <services\checks.h> included before OWL one.
#endif
#if defined(__CHECKS_H)
#error <checks.h> included before OWL one.
#endif

#define SERVICES_CHECKS_H
#define __CHECKS_H
#if !defined(OWL_PRIVATE_EXCEPT_H)
# include <owl/private/except.h>
#endif
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
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

class _OWLCLASS TDiagBase;

typedef  _tostrstream  diag_stream;
typedef  owl_string    diag_string;

//
// Static Global Redirector
//
struct TDiagBaseHook {
  virtual void Output(TDiagBase* group, LPCTSTR str) = 0;
};

class _OWLCLASS TModule;
class _OWLCLASS TDiagFunction;
//
// TDiagBase - this class forms the base for TDiagGroup classes and
// handles basic message output.
//
class _OWLCLASS TDiagBase{
  public:
    void _OWLENTRY Trace(LPCTSTR msg, LPCSTR fname, uint32 line);
    
    void _OWLENTRY Warn(LPCTSTR msg, LPCSTR fname, uint32 line);

    void _OWLENTRY    Enable(uint8 enabled)  
                          { Flags.Enabled = uint8(enabled ? 1 : 0); }
    int  _OWLENTRY    IsEnabled()            
                          { return Flags.Enabled; }
    void  _OWLENTRY    SetLevel( uint8 level ) 
                          { Flags.Level = level; }
    uint8 _OWLENTRY    GetLevel() 
                          { return Flags.Level; }
    void  _OWLENTRY    SetLocalHook(TDiagBaseHook* hook ) 
                          { LocalHook = hook; }

    LPCSTR _OWLENTRY  GetName()                    
                          { return Name;}
    
    virtual TModule* _OWLENTRY GetDiagModule()  = 0;

    static TDiagBase* _OWLENTRY GetDiagGroup(TDiagBase* curr = 0);
    
    static TDiagBaseHook*  _OWLENTRY SetGlobalHook(TDiagBaseHook* hook = 0);

    virtual _OWLENTRY ~TDiagBase();

  protected:
    // name have to be static string
    _OWLENTRY TDiagBase(LPCSTR name, uint8 e, uint8 l);

#if defined(BI_COMP_MSC)
  public:
#endif

    struct TFlags {
       uint8 Enabled : 1;
      uint8 Level   : 7;
    };
  
  protected:
    TFlags          Flags;
    LPCSTR           Name;
    TDiagBaseHook*  LocalHook;        // second redirector  

  private:
    _OWLENTRY TDiagBase(const TDiagBase& base); // prohibite
    TDiagBase& operator=(const TDiagBase& base); // prohibite
    
    void _OWLENTRY Message(LPCTSTR type, LPCTSTR msg,
                           LPCSTR fname, uint32 line);
    void _OWLENTRY Output(LPCTSTR msg);
    
    TDiagBase*            NextGroup;

    static void AddDiagGroup(TDiagBase* group);
    static void RemoveDiagGroup(TDiagBase* group);

    friend class TDiagFunction;
};

//
// TDiagBase::Trace - Formats a trace message using the specified parameters
// and outputs it using the output() function.
//
inline void TDiagBase::Trace(LPCTSTR msg, LPCSTR fname, uint32 line)
{
  Message( _T("Trace"), msg, fname, line);
}

//
// TDiagBase::Warn - Formats a warning message using the specified parameters
// and outputs it using the output() function.
//
inline void TDiagBase::Warn(LPCTSTR msg, LPCSTR fname, uint32 line)
{
  Message( _T("Warning"), msg, fname, line);
}

//
// TraceFunction - idea from Don Griffin
//
class _OWLCLASS TDiagFunction{
  public:
    TDiagFunction(TDiagBase& group, int level, LPCTSTR name,
                  LPCSTR file, int line);
    TDiagFunction(TDiagBase& group, int level, LPCTSTR name, 
                  LPCTSTR params, LPCSTR file, int line);
    ~TDiagFunction();

  private:
    _OWLENTRY TDiagFunction(const TDiagFunction& f); // prohibite
    TDiagFunction& operator=(const TDiagFunction& f); // prohibite

    TDiagBase&  Group;
    LPCTSTR      Name;
    LPCSTR      File;
    int          Line;
    bool        Enable;

    void Trace(LPCTSTR ch, LPCTSTR param, LPCSTR file, int line);
};

//FMM Added the define below to check if it was redefined anywhere, but
//FMM it wasn't and it seems to have fixed the problem without breaking
//FMM anything so i'm leaving it here. for now anyway.
#define THROW_SPEC0 throw()

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif

class _OWLCLASS xerror : public XBASE {
  public:
     _OWLENTRY xerror( LPCTSTR type, LPCTSTR txt, LPCSTR file, uint32 line ) 
                : str(MakeString(type, txt, file, line)) //????????????????????????????
#if defined(BI_STD_EXCEPTIONS)
                  {}
     virtual _OWLENTRY ~xerror() THROW_SPEC0 {}
     virtual const char* what () const  THROW_SPEC0
    {
        return (LPCSTR)str.c_str();
    }
    
#else //BI_STD_EXCEPTIONS

             ,XBASE((LPCSTR)str.c_str()) {}
#endif

#if defined(UNICODE)
     _OWLENTRY xerror( LPCTSTR type, LPCSTR txt, LPCSTR file, uint32 line ) 
                : str(MakeString(type,txt,file,line))//????????????????????????????
#  if defined(BI_STD_EXCEPTIONS)
                  {}
#  else
             ,XBASE((LPCSTR)str.c_str()) {}
#  endif

#endif

  private:
    static diag_string _OWLENTRY MakeString( LPCTSTR type, LPCTSTR txt, 
                                             LPCSTR file, uint32 line );
#if defined(UNICODE)
    static diag_string _OWLENTRY MakeString( LPCTSTR type, LPCSTR txt, 
                                             LPCSTR file, uint32 line );
#endif
  protected:
    diag_string str;
};

class _OWLCLASS precondition : public xerror {
  public:
    _OWLENTRY precondition( LPCTSTR txt, LPCSTR file, uint32 line ) 
                : xerror( _T("Precondition"), txt, file, line )
            {}
#if defined(UNICODE)
    _OWLENTRY precondition( LPCSTR txt, LPCSTR file, uint32 line ) 
                : xerror( _T("Precondition"), txt, file, line )
            {}
#endif
};

class _OWLCLASS check : public xerror {
  public:
    _OWLENTRY check( LPCTSTR txt, LPCSTR file, uint32 line ) 
                : xerror( _T("Check"), txt, file, line )
             {}
#if defined(UNICODE)
    _OWLENTRY check( LPCSTR txt, LPCSTR file, uint32 line ) 
                : xerror( _T("Check"), txt, file, line )
             {}
#endif
};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

#if defined(BI_COMP_BORLANDC)
#  define _OWL_BREAK {__emit__(0xCC); }   // force debugg trap
#elif defined(BI_COMP_GNUC)
# define _OWL_BREAK __asm__ ("int3");
#elif defined(UNIX)
# define _OWL_BREAK assert(NULL);
#else
#  define _OWL_BREAK _asm { int 3 }       // force debugg trap
#endif

_OWLFUNC(int) BreakMessage(LPCTSTR type, LPCTSTR msg, LPCSTR file, int line);
#if defined(UNICODE)
_OWLFUNC(int) BreakMessage(LPCTSTR type, LPCSTR msg, LPCSTR file, int line);
#endif

__OWL_END_NAMESPACE

#if !defined( __DEBUG )
# define __DEBUG 0
#endif


#if __DEBUG < 1

#  define IFDBG(a)
#  define IFNDBG(a) a

#  ifndef NDEBUG
#    define NDEBUG 
#  endif
#  define  VALIDPTR(p)        ((p) != 0)              // quicker than IsBadReadPtr

#else // _DEBUG < 1

      //  If we don't turn these on, frequent use of ASSERT will fill DGROUP:
      //
#if !defined(BI_COMP_GNUC)
#  pragma option -d    // merge duplicate strings ON
#endif

#  define IFDBG(a) a
#  define IFNDBG(a)

#  define  VALIDPTR(p) (!::IsBadReadPtr((p),1))  // more thorough

#endif

# define  VALIDORNULLPTR(p)  (!(p) || VALIDPTR(p))

//  This guy generates a compiler error such as
//          'Array must have at least one element'
//  if the boolean condition argument is false:
# define COMPILE_ASSERT(B)   typedef char _Assert##__LINE__ [2*(!!(B)) - 1]


#undef PRECONDITION
#undef PRECONDITIONX

#define PRECONDITION(p) PRECONDITIONX(p,#p)

#if __DEBUG < 1

# define PRECONDITIONX(p,s)   ((void)0)

#else // __DEBUG < 1

# define PRECONDITIONX(p,s)                                               \
    if(!(p)){                                                             \
      int retval = NS_OWL::BreakMessage(_T("Precondition"),s,__FILE__,__LINE__);  \
      if(retval==IDRETRY)                                                 \
        { _OWL_BREAK }                                                    \
      else if(retval==IDABORT)                                            \
        { throw NS_OWL::precondition(s,__FILE__,__LINE__); }                      \
     }

#endif

#undef CHECK
#undef CHECKX

#define CHECK(p) CHECKX(p,#p)

#if __DEBUG < 2
# define CHECKX(p,s)    ((void)0)
#  define VERIFY(p)      ((void)(p))
#  define VERIFYX(p,s)    ((void)(p))
#else
# define CHECKX(p,s)                                                      \
     if(!(p)){                                                            \
      int retval = NS_OWL::BreakMessage(_T("Check"),s,__FILE__,__LINE__);          \
      if(retval==IDRETRY)                                                 \
        { _OWL_BREAK }                                                    \
      else if(retval==IDABORT)                                            \
      { throw NS_OWL::check(s,__FILE__,__LINE__); }                                \
     }

#  define  VERIFY(p)      CHECK(p)
#  define  VERIFYX(p,s)  CHECKX(p,s)
#endif

#if defined(__TRACE) || defined(__WARN)

#if defined( _OWLDLL ) || defined( _BUILDOWLDLL )
#  if !defined(DIAG_IMPORT)
#    define DIAG_IMPORT __import
#  endif
#  if !defined(DIAG_EXPORT)
#    define DIAG_EXPORT __export
#  endif
#else
# define DIAG_IMPORT /**/
# define DIAG_EXPORT /**/
#endif

//#define  DIAG_NONE   /**/

//#define DIAG_VAR_NAME(g) __OwlDiagGroup##g
//#define DIAG_GROUP_NAME(g) TDiagGroup##g

#define DECLARE_DIAG_GROUP_COMMON(g)                                      \
  public:                                                                  \
    TDiagGroup##g(LPCSTR name, uint8 e, uint8 l);                         \
    virtual NS_OWL::TModule* _OWLENTRY GetDiagModule();                            \
}

#define DECLARE_DIAG_GROUP(g, qual)                                       \
  class qual TDiagGroup##g : public NS_OWL::TDiagBase {                           \
  DECLARE_DIAG_GROUP_COMMON(g)

#define DECLARE_DIAG_STATIC_GROUP(g)\
  class TDiagGroup##g : public NS_OWL::TDiagBase {\
  DECLARE_DIAG_GROUP_COMMON(g)

#define DIAG_DEFINE_GROUP_COMMON(g)\
TDiagGroup##g::TDiagGroup##g(LPCSTR name,uint8 e, uint8 l)\
    : NS_OWL::TDiagBase(name,e, l){}\
  NS_OWL::TModule* _OWLENTRY TDiagGroup##g::GetDiagModule(){\
  return gModule;\
}

//
// Define group macro
//
#define _DIAG_DEFINE_GROUP_(g,e,l,t)\
DECLARE_DIAG_GROUP(g,t);\
DIAG_DEFINE_GROUP_COMMON(g)

//
// Define static group macro
//
#define _DIAG_DEFINE_GROUP_STATIC(g,e,l)\
DECLARE_DIAG_STATIC_GROUP(g);\
DIAG_DEFINE_GROUP_COMMON(g);\
TDiagGroup##g __OwlDiagGroup##g(#g, e, l)

//DLN Define OwlDiagGroupDef without redefining TDiagGroupDef
#define OWL_DIAG_DEFINE_GROUP(g,e,l) \
  DIAG_DEFINE_GROUP_COMMON(g); \
  TDiagGroup##g __OwlDiagGroup##g(#g, e, l)

//
// Declare static group macro
//
#define DIAG_DECLARE_GROUP(g)\
DECLARE_DIAG_STATIC_GROUP(g);\
extern TDiagGroup##g __OwlDiagGroup##g


//
// Define group macro
//
#define DIAG_DEFINE_GROUP(g,e,l)\
_DIAG_DEFINE_GROUP_STATIC(g,e,l);

//
// Declare group for export macro
//
#define DIAG_DECLARE_EXPORTGROUP(g)\
DECLARE_DIAG_GROUP(g,DIAG_EXPORT);\
extern _OWLEXPORTDATA( TDiagGroup##g ) __OwlDiagGroup##g

//
// Declare group for import macro
//
#define DIAG_DECLARE_IMPORTTGROUP(g)\
DECLARE_DIAG_GROUP(g,DIAG_IMPORT);\
extern _OWLIMPORTDATA( TDiagGroup##g ) __OwlDiagGroup##g

//JJH Define group for winelib export macro without redefining TDiagGroupDef
#define OWL_DIAG_DEFINE_EXPORTGROUP(g,e,l)\
DIAG_DEFINE_GROUP_COMMON(g)\
_OWLEXPORTDATA( TDiagGroup##g ) __OwlDiagGroup##g(#g,e,l)

//
// Define group for export macro
//
#define DIAG_DEFINE_EXPORTGROUP(g,e,l)\
_DIAG_DEFINE_GROUP_(g,e,l,DIAG_EXPORT);\
_OWLEXPORTDATA( TDiagGroup##g ) __OwlDiagGroup##g(#g,e,l)


#define DIAG_ENABLE(g,s)            __OwlDiagGroup##g.Enable(s)
#define DIAG_ISENABLED(g)           __OwlDiagGroup##g.IsEnabled()
#define DIAG_SETLEVEL(g,l)          __OwlDiagGroup##g.SetLevel(l)
#define DIAG_GETLEVEL(g)            __OwlDiagGroup##g.GetLevel()
#define DIAG_REDIRECT(g,s)          __OwlDiagGroup##g.SetLocalHook(s)

#define IFDIAG(a)   a
#define IFNDIAG(a)

#if !defined(_DONT_DECLARE_DEF_DIAG_GROUP) && !defined( _DEF_DECLARED )
__OWL_BEGIN_NAMESPACE
#  define _DEF_DECLARED
#  if defined( _BUILDOWLDLL )
    DIAG_DECLARE_EXPORTGROUP(Def);
#  elif defined( _OWLDLL )
    DIAG_DECLARE_IMPORTTGROUP(Def);
#  else
    DIAG_DECLARE_GROUP(Def);
#  endif
__OWL_END_NAMESPACE
#endif

#else   // #if defined(__TRACE) | defined(__WARN)

#define DIAG_DECLARE_GROUP(g)
#define DIAG_DECLARE_EXPORTGROUP(g)
#define DIAG_DECLARE_IMPORTTGROUP(g)

#define DIAG_DEFINE_GROUP(g,e,l)
#define OWL_DIAG_DEFINE_GROUP(g,e,l)
#define DIAG_DEFINE_EXPORTGROUP(g,e,l)

#define DIAG_ENABLE(g,s)            ((void)0)
#define DIAG_ISENABLED(g)           ((void)0)
#define DIAG_SETLEVEL(g,l)          ((void)0)
#define DIAG_GETLEVEL(g)            ((void)0)
#define DIAG_REDIRECT(g,s)          ((void)0)

#define IFDIAG(a)
#define IFNDIAG(a) a

#endif   // #if defined(__TRACE) | defined(__WARN)

#if defined(__TRACE)
# define TRACE(m)                    TRACEX(Def,0,m)
#if defined(BI_STD_NEWSSTREAM)
# define TRACEX(g,l,m)\
    do{\
      __OWL_USING_NAMESPACE;\
      if(__OwlDiagGroup##g.IsEnabled() &&\
         l <= __OwlDiagGroup##g.GetLevel()){\
        diag_stream  out;\
        out << m << NS_OWL_STREAMS::ends;\
        __OwlDiagGroup##g.Trace(out.str().c_str(),__FILE__,__LINE__);\
      }\
    }while(0)
#else
# define TRACEX(g,l,m)\
    do{\
      __OWL_USING_NAMESPACE;\
      if(__OwlDiagGroup##g.IsEnabled() &&\
         l <= __OwlDiagGroup##g.GetLevel()){\
        diag_stream  out;\
        out << m << NS_OWL_STREAMS::ends;\
        __OwlDiagGroup##g.Trace(out.str(),__FILE__,__LINE__);\
        out.rdbuf()->freeze(0);\
      }\
    }while(0)
#endif
// not suitable logically if trace must be warning will be thow
#  define TRACEX_IF(b,g,l,m)   WARNX(g,b,l,m)
# define TRACE_IF(b,msg)      WARNX(Def,b,0,msg)

#define TRACE_FUNCTIONX(g,l,f)  \
  NS_OWL::TDiagFunction __traceFunc((NS_OWL::TDiagBase&)__OwlDiagGroup##g, l, f,__FILE__,__LINE__)

#define TRACE_FUNCTIONXP(g,l,f,p)  \
  NS_OWL::TDiagFunction __traceFunc((NS_OWL::TDiagBase&)__OwlDiagGroup##g, l, f,p,__FILE__,__LINE__)

#define TRACE_FUNCTION(f)  \
  TRACE_FUNCTIONX(Def, 0, f)

#define TRACE_FUNCTIONP(f,p)  \
  TRACE_FUNCTIONXP(Def, 0, f, p)

#else
# define TRACE(m)                    ((void)0)
# define TRACEX(g,l,m)               ((void)0)
#  define TRACEX_IF(b,g,l,m)          ((void)0)
# define TRACE_IF(b,msg)             ((void)0)
#  define TRACE_FUNCTIONX(g,l, f)     ((void)0)
#  define TRACE_FUNCTIONXP(g,l,f,p)   ((void)0)
#  define TRACE_FUNCTION(f)           ((void)0)
#  define TRACE_FUNCTIONP(f,p)         ((void)0)
#endif

#if defined(__WARN)
# define WARN(c,m)                   WARNX(Def,c,0,m)
#if defined(BI_STD_NEWSSTREAM)  
# define WARNX(g,c,l,m) \
  do{\
    __OWL_USING_NAMESPACE;\
    if(c){ \
      if(__OwlDiagGroup##g.IsEnabled() &&\
         l <= __OwlDiagGroup##g.GetLevel()){\
        diag_stream  out;\
        out << m << NS_OWL_STREAMS::ends;\
        __OwlDiagGroup##g.Warn(out.str().c_str(),__FILE__,__LINE__);\
      }\
    }\
  }while(0)
#else
# define WARNX(g,c,l,m) \
  do{\
    __OWL_USING_NAMESPACE;\
    if(c){ \
      if(__OwlDiagGroup##g.IsEnabled() &&\
         l <= __OwlDiagGroup##g.GetLevel()){\
        diag_stream  out;\
        out << m << NS_OWL_STREAMS::ends;\
        __OwlDiagGroup##g.Warn(out.str(),__FILE__,__LINE__);\
        out.rdbuf()->freeze(0);\
      }\
    }\
  }while(0)
#endif
#else
# define WARN(c,m)                   ((void)0)
# define WARNX(g,c,l,m)              ((void)0)
#endif

#endif  // OWL_PRIVATE_CHECKS_H
