//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.34.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_DEFS_H)
#define OWL_PRIVATE_DEFS_H
#define SERVICES_DEFS_H

#if !defined(__cplusplus)
#error Must compile ObjectWindows as C++
#endif

// List of definnes that must be do
//      BI_PLAT_MSW         Microsoft Windows, all versions including console
//
//  Specify the Windows API if we're compiling for Windows. Must
//  define one of:
//
//      BI_PLAT_WIN32       Microsoft Win32
//
//      WINVER && WINVER < 0x030A   for Win 3.0
//
//  Define the memory model when compiling for Windows 16 or DOS. Must
//  define one of:
//
//      BI_MODEL_TINY       DOS/Windows tiny model
//      BI_MODEL_SMALL      DOS/Windows small model
//      BI_MODEL_COMPACT    DOS/Windows compact model
//      BI_MODEL_MEDIUM     DOS/Windows medium model
//      BI_MODEL_LARGE      DOS/Windows large model
//      BI_MODEL_HUGE       DOS/Windows huge model
//
//  Define the pointer size. Must define one of:
//
//      BI_PTR_16_16        Intel 16:16 pointer
//      BI_PTR_0_32         32-bit flat pointer (including Intel 0:32)
//
//  Specify the size of data pointers and code
//  pointers if applicable. Must define one of:
//
//  Define the type of application being built. Must be one of:
//
//      BI_APP_EXECUTABLE
//      BI_APP_DLL
//
//  Only enable multi-thread classes when the OS supports them, and/or when the
//  runtime library support them
//
// BI_MULTI_THREAD
// BI_MULTI_THREAD_RTL
//
// Specific name of compiler, BI_COMP_xxxx
//
// BI_COMP_BORLANDC  or BI_COMP_MSC or BI_COMP_GNU
//
//
// BC v5.0 introduces the ability to import/export templates
// BI_IMPEXP_TEMPLATES // Import and export template instances
//
//
// BI_NO_ARRAYNEW      // operators new[] & delete[]   -
// BI_NO_COVAR_RET     // Covariant returns in overrides -
// BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
// BI_NO_THROWSPEC     // Function throw specifiers    EXCEPT.H
// BI_NO_LVAL_CASTS    // Can cast lvalues
//
// BI_NO_BOOL          // Standard bool type           DEFS.H
// BI_NO_MUTABLE       // Mutable keyword              DEFS.H
// BI_NO_NAMESPACE     // Namespaces
//
// BI_NO_NEW_CASTS     // New style casts              CASTABLE.H
//
// _WCHAR_T_DEFINED  // done already in win32/windows.h ?
// _WIN32
//
// THROW_BAD_CAST Bad_cast(), bad_cast()
// THROW_TYPE_ID  Bad_typeid(), bad_typeid()
//
// BI_XBASE_DEFINED
// BI_XALLOC_DEFINED
// XALLOC xalloc, bad_alloc
// XBASE xmsg, exception
// XWHY_STR why().c_str(), what()
//
//
// BI_NO_MINMAX_TMPL   //1=> min and max defined trought STL, 0=>using OWL min max template
//
//  BI_NO_RTTI                  if not support for rtti
//  _RTTI                       -> define _RTTI => nothing if not support for keyword __rtti
//                              else leave it undefined
//  EMULATE_BOOL
//
//  BI_NOTHUNK                  -> don't use thunks (WARNING! Not safe! Deprecated.)
//  NEED_EXPL_TPL               -> issue with template function instances 
//
// Include the appropriate compiler-specific header based on known compiler
// defined preprocessor constants
//
// NO_AFTER_RETPARAM            -> if defined _export/_import keyword placed
//                                 before return parameter :
//                                 for Borland:    void __export MFunc()
//                                 for Microsoft:  __export void MFunc()
// BI_NEED_EXPL_TPL             -> need explicit instantiate template?????
// BI_NOSTATIC_FUNCCALL         -> if compiler has problems with this :
//                                 static int Flag = GetFlags();
// BI_VIRTUAL_BUG               -> virtual bug  with micro$oft, no bug but different 
//                                 handling of C++ ???
// BI_64BIT_NATIVE              -> native 64 bit math
// BI_128BIT_NATIVE             -> native 128 bit math
// BI_OWL_PACKING               -> aligment if not defined default 1
// BI_TYPECASTOPERATOR_BUG      -> if problem with type cast operator like: operator HWND()
// BI_NEW_GMODULE               -> new name: gModule for BC also (not ::Module)
// BI_STD_RTL                   -> Use STD run time i/o library -> implies EXCEPTIONS,IO,STRINNG
// BI_STD_EXCEPTIONS            -> Use STD exceptions
// BI_STD_IO                    -> Use new template based iostreams
// BI_STD_RTTI                  -> Use new rtti
// BI_STD_STRING                -> Use STL string 
// BI_NO_CHECK_ARRAY            -> Use COUNTOF() macro without parameter checking 
// BI_NO_DEFAULT_TEMPLATES      -> No suport for default templates e.g: template<class T = int> ...
// BI_NO_TMPL_ALLOCATOR         -> No use memory allocator
// BI_SUPPORT_TMPLFNC_FRIEND    -> Support declaration template funciton as friend
// BI_OWL_NO_DEBUG_CRT          -> Compiler provided Debug RTL 
// BI_HAS_MEMBER_TEMPLATE       -> Member template functions and classes
// BI_USE_LONG_HNAMES           -> Using long names for header
// BI_GLOBALNM_BUG              -> Global namespace bug in MSVC like mfunc(int name = ::someName);
// BI_COMMCTRL_OLDNAMES         -> Old names for common control structures like MWing32
// BI_NOTEMPLATE_H              -> No include <owl/template.h> -> not defined
// BI_STD_SPECIALIZATION        -> Specialization of templates needs "template<>"
// BI_SUPPRESS_OLE              -> Suppress including OLE
// BI_DEFINE_HUGE               -> define HUGE if nessesary
// BI_ARRAY_NEW_DEFINED         -> not define operator new[](unsigned,void*)
// BI_STDIO_HASFILECNT          -> STL IO has constructor thas gets file_id 
// file_id                      -> STL IO, will be FILE* or int its depends on STL
// BI_STDIO_CTRFSI              -> STL IO has constructor ( file_id f, LPSTR b, int len )
// BI_STD_NEWSSTREAM            -> new STL use sstream and not strstream  
// BI_HAS_PRAGMA_ONCE            -> support for pragma once
//----------------------------------------------------------------------------
// We must use all libraries in DLLs if we are using Owl in a DLL
//
// Define _OWLDLL with -WSE (-WE for 32bit) to use Owl, bids & RTL in dlls.
//
// Be sure _BUILDOWLDLL is defined when building an actual owl dll
//

//JJH use only if not using msvcrt's stdlib.h
#if defined(BI_NEEDS_PRIVATE_OWLPORT) //for Syncad boys, probably should be obsolote right now.
#include <port/owl-port.h>
#endif

#if defined(_OWLDLL) || defined(_BUILDOWLDLL)
# if !defined(_OWLDLL)
#   define _OWLDLL
# endif

# if !defined(_RTLDLL)
#   define _RTLDLL
# endif
#endif

#if defined(_DEBUG)
#if !defined(__DEBUG)
# define __DEBUG 1
#endif
#endif

#if defined(__DEBUG) && __DEBUG > 0
# if !defined(_DEBUG)
#   define _DEBUG
# endif
#endif

#if defined(UNICODE)
# if !defined(_UNICODE)
#   define _UNICODE
# endif
#endif

#if defined(_UNICODE)
# if !defined(UNICODE)
#   define UNICODE
# endif
#endif

#if !defined(__FORTE__)
#define __endexcept
#endif

//Jogy - should not have such defines here
//#define HPCOMPNOW

#if defined(__BORLANDC__)
# include <owl/private/borlandc.h>
#elif defined(_MSC_VER)
# include <owl/private/msc.h>
#elif defined(__GNUC__)
# include <owl/private/gnuc.h>
#elif defined(__WATCOMC__)
# include <owl/private/watcom.h>
#elif defined(__FORTE__)
# include <owl/private/forte.h>
#elif defined(HPCOMPNOW)
# include <owl/private/acchpux.h>
#else
#error Unknown compiler
#endif

#if defined(BI_COMP_WATCOM)
#pragma read_only_file
#endif

//------------------------------------------------------------------------------
// default BI_OWL_PACKING = 1
#if !defined(BI_OWL_PACKING)
#define BI_OWL_PACKING 1
#endif

//----------------------------------------------------------------------------
// OS-specific flags.
//
// no WIN16 from now !!!!!!!!!!!!!!!!!!!!!!!!!!!
#if !defined(OWL_NATIVECTRL_ALWAYS) && !defined(NO_OWL_NATIVECTRL_ALWAYS)
#define OWL_NATIVECTRL_ALWAYS
#endif


//Jogy 23.08.2007 - behavior changed, by default the mode is NO_OWL5_COMPAT unless OWL5_COMPAT, OWL2_COMPAT or OWL1_COMPAT is defined

#if defined(OWL1_COMPAT)
#define OWL2_COMPAT
#endif

#if defined(OWL2_COMPAT)
#define OWL5_COMPAT
#endif

#if !defined(OWL5_COMPAT)
#define NO_OWL5_COMPAT
#endif

#if !defined(OWL2_COMPAT)
#define NO_OWL2_COMPAT
#endif

#if !defined(OWL1_COMPAT)
#define NO_OWL1_COMPAT
#endif

#if defined(NO_OWL5_COMPAT)
# define NO_OWL2_COMPAT
# define NO_OWL1_COMPAT
#endif

#if defined(NO_OWL2_COMPAT)
# define NO_OWL1_COMPAT
#endif


//----------------------------------------------------------------------------
// _RTTI provides a convenient macro for switching on Borland's __rtti
// keyword for finer grained control over generation of runtime type
// information.
//

#if defined(BI_NO_RTTI)
# if !defined(_RTTI)
#   define _RTTI
# endif
#else
# if !defined(_RTTI)
#   define _RTTI __rtti
# endif
#endif

#if !defined(BI_NO_NAMESPACE)
# define BI_NAMESPACE
# define __OWL OWL::
# define NS_OWL OWL
# define __OWL_GLOBAL ::OWL
# define __OWL_BEGIN_NAMESPACE namespace OWL {
# define __OWL_USING_NAMESPACE  using namespace OWL;
# define __OWL_END_NAMESPACE };
# define __OWL_DECLCLASS(cls) namespace OWL { class _OWLCLASS cls; }
# define __STL stl::
#else
# define __OWL
# define NS_OWL
# define __OWL_GLOBAL
# define __OWL_BEGIN_NAMESPACE
# define __OWL_END_NAMESPACE
# define __OWL_USING_NAMESPACE
# define __OWL_DECLCLASS(cls) class _OWLCLASS cls
# define __STL 

#endif


//----------------------------------------------------------------------------
//  These CAST macros encapsulate the new cast syntax in the ANSI/ISO
//  working paper. Note that TYPESAFE_DOWNCAST isn't as general as
//  dynamic_cast -- it only works on pointers.
//
//  Usage:
//
//  TYPESAFE_DOWNCAST(object,toClass)
//      Converts the pointer referred to by 'object' into a pointer to
//      an object of type 'toClass'. Note that the macro parameters to
//      TYPESAFE_DOWNCAST are in the opposite order from the rest of
//      the macros here. When using a compiler that supports new style
//      casts and runtime type information this is done with
//      dynamic_cast<> and will return 0 if the cast cannot be done.
//      When using a compiler that does not support new style casts and
//      runtime type information this is done with fake runtime type
//      information generated by the IMPLEMENT_CASTABLE macro.
//
//  STATIC_CAST(targetType,object)
//      Converts the data object referred to by 'object' into the type
//      referred to by 'targetType'. When using a compiler that supports
//      new style casts, this is done with static_cast<> and will fail
//      if the cast cannot be done without runtime type information.
//      When using a compiler that does not support new style casts, this
//      is done with an old style dangerous cast.
//
//  CONST_CAST(targetType,object)
//      Converts the data object referred to by 'object' into the type
//      referred to by 'targetType'. When using a compiler that supports
//      new style casts, this is done with const_cast<> and will fail
//      if the cast changes the type of the object in any way other than
//      adding or removing const and volatile qualifiers.
//      When using a compiler that does not support new style casts, this
//      is done with an old style dangerous cast.
//
//  REINTERPRET_CAST(targetType,object)
//      Converts the data object referred to by 'object' into the type
//      referred to by 'targetType'. When using a compiler that supports
//      new style casts, this is done with reinterpret_cast<>.
//      When using a compiler that does not support new style casts, this
//      is done with an old style dangerous cast.
//

# define TYPESAFE_DOWNCAST(object,toClass)\
        dynamic_cast<toClass * >(object)
# define STATIC_CAST(targetType,object)   \
        static_cast<targetType >(object)
# define CONST_CAST(targetType,object)    \
        const_cast<targetType >(object)
# define REINTERPRET_CAST(targetType,object) \
        reinterpret_cast<targetType >(object)

#if defined( BI_NO_RTTI )
#  define GET_CLASS_CASTABLE_ID(classtype) classtype::CastableIdent
#  define GET_OBJECT_CASTABLE_ID(object) ( object )->CastableID()
#else
#  define GET_CLASS_CASTABLE_ID(classtype) typeid( classtype ).name()
#  define GET_OBJECT_CASTABLE_ID(object)   typeid( object ).name()
#endif

//----------------------------------------------------------------------------
// Provide expansions for mutable and bool when appropriate.
//


__OWL_BEGIN_NAMESPACE

#if defined(BI_NO_BOOL)
enum TBool {
  false,
  true
};
# if defined(EMULATE_BOOL)
    typedef TBool bool;
#   define BI_UNIQUE_BOOL
# else
    typedef int bool;
#   undef BI_UNIQUE_BOOL
# endif

#else
  typedef bool TBool;
# define BI_UNIQUE_BOOL
#endif

#if defined(BI_NO_BOOL) && defined(EMULATE_BOOL)
  template<class T> inline bool ToBool(const T& t) {
    return STATIC_CAST(bool,(t!=0));
  }
#else
  template<class T> inline bool ToBool(const T& t) {
    return STATIC_CAST(bool,t);
  }
#endif

__OWL_END_NAMESPACE


//----------------------------------------------------------------------------
// Setup class, function and data modifier macros for OWL.
//
#if defined(NO_AFTER_RETPARAM)
# define _OWLEXPORTDATA(p)   __export p
# define _OWLEXPORTFUNC(p)   __export p _CALLCNVN  // owl _stddecl
# define _OWLEXPORTCFUNC(p)  __export p _CCONV    // owl _cdecl
# define _OWLIMPORTDATA(p)   __import p
# define _OWLIMPORTFUNC(p)   __import p _CALLCNVN
# define _OWLIMPORTCFUNC(p)  __import p _CCONV
# define _OWLSTATICDATA(p)   p
# define _OWLSTATICFUNC(p)   p _CALLCNVN
# define _OWLSTATICCFUNC(p)  p _CCONV
#else
# define _OWLEXPORTDATA(p)   p __export
# define _OWLEXPORTFUNC(p)   p __export _CALLCNVN
# define _OWLEXPORTCFUNC(p)  p __export _CCONV
# define _OWLIMPORTDATA(p)   p __import
# define _OWLIMPORTFUNC(p)   p __import _CALLCNVN
# define _OWLIMPORTCFUNC(p)  p __import _CCONV
# define _OWLSTATICDATA(p)   p
# define _OWLSTATICFUNC(p)   p _CCONV
# define _OWLSTATICCFUNC(p)  p _CCONV
#endif

#define _OWLEXPORTCLASS       __export _CALLCNVN
#define _OWLEXPORTCLASS_RTL   __export
#define _OWLIMPORTCLASS       __import _CALLCNVN
#define _OWLIMPORTCLASS_RTL   __import
#define _OWLSTATICCLASS       _CALLCNVN
#define _OWLSTATICCLASS_RTL

#if defined(_BUILDOWLDLL)
//JJH following guard for _OWLCLASS is here for the sake of 
//ow/include/private/winelib.h:19
# if !defined(_OWLCLASS)
# define _OWLCLASS      _OWLEXPORTCLASS
# endif
//JJH the same as for _OWLCLASS
# if !defined(_OWLCLASS_RTL)
# define _OWLCLASS_RTL  _OWLEXPORTCLASS_RTL
# endif
# define _OWLDATA       _OWLEXPORTDATA
# define _OWLFUNC       _OWLEXPORTFUNC
# define _OWLCFUNC      _OWLEXPORTCFUNC
#elif defined(_OWLDLL)
# define _OWLCLASS     _OWLIMPORTCLASS
# define _OWLCLASS_RTL _OWLIMPORTCLASS_RTL
# define _OWLDATA     _OWLIMPORTDATA
# define _OWLFUNC     _OWLIMPORTFUNC
# define _OWLCFUNC    _OWLIMPORTCFUNC
#else
# define _OWLCLASS
# define _OWLCLASS_RTL
# define _OWLDATA(p)   p
# define _OWLFUNC(p)   p _CALLCNVN
# define _OWLCFUNC(p)  p _CCONV
#endif

#define _OWLENTRY _CALLCNVN
#if defined(BI_COMP_MSC)
# define _OWLCTOR
#else
# define _OWLCTOR _CALLCNVN
#endif

#if defined(NO_AFTER_RETPARAM)
#define DECLARE_FUNC(rparm,ctype,fname)  rparm (ctype fname)
#else
#define DECLARE_FUNC(rparm,ctype,fname)  rparm ctype (fname)
#endif


//
// Setup modifier for callback function
//
# define OWL_EXPORT    __export
# define OWL_EXPORT16

//----------------------------------------------------------------------------
// Setup class modifier for user classes derived from Owl classes.
//  Force data segment load on method entry in case called back from Owl for
//  16bit windows DLLs using Owl in a DLL
//
# define _USERCLASS

//----------------------------------------------------------------------------
// Define macro for user code to force inclusion of stream registration
// objects when statically linking to OWL. Assumes that owl/objstrm.h has
// been #included at the point where it is used.
//
#if defined(_OWLDLL)
# define _OWLLINK(s)
#else
# define _OWLLINK(s) __link(s)
#endif


//
// Commonly used fixed-size integer types
//
#if defined(BI_COMP_BORLANDC)
# include <systypes.h>
#else
//__OWL_BEGIN_NAMESPACE
  typedef signed char    int8;
  typedef signed short   int16;
  typedef signed long    int32;

  typedef unsigned char  uint8;
  typedef unsigned short uint16;
  typedef unsigned long  uint32;
//__OWL_END_NAMESPACE
#endif

#if (defined(BI_COMP_BORLANDC) && (__BORLANDC__ <= 0x530)) || \
  (_MSC_VER == 1200)
# if !defined(UINT_PTR)
#  define UINT_PTR uint32
# endif
# if !defined(DWORD_PTR)
#  define DWORD_PTR uint32
# endif
#endif  // (defined(BI_COMP_BORLANDC) && (__BORLANDC__ <= 0x530)) ||
        //  (_MSC_VER == 1200)

#if !defined(STRICT)
#define STRICT
#endif
#if !defined(__TCHAR_H)
#include <tchar.h>
#endif

//
//__OWL_BEGIN_NAMESPACE
// Some nonfixed-size types defined similar to the fixedsize in systypes.h
//
  typedef unsigned long  ulong;

//FMM That line was having conflicts with namespacing in HPUX
#if !defined(__HPUX_SOURCE)
  typedef unsigned int   uint;
  typedef unsigned short ushort;
#endif
  typedef _TUCHAR        uchar;
//__OWL_END_NAMESPACE

//__OWL_BEGIN_NAMESPACE
//
// Integer byte, word and long word manipulation
//
inline uint16 MkUint16(uint8 lo, uint8 hi) {
  return uint16(lo | (uint16(hi) << 8));
}
inline uint32 MkUint32(uint16 lo, uint16 hi) {
  return lo | (uint32(hi) << 16);
}
inline uint16 LoUint16(uint32 u32) {
  return uint16(u32);
}
inline int16 LoInt16(uint32 u32) {
  return int16(u32);
}
inline uint16 HiUint16(uint32 u32) {
  return uint16(u32 >> 16);
}
inline int16 HiInt16(uint32 u32) {
  return int16(u32 >> 16);
}
inline uint8 LoUint8(uint16 u16) {
  return uint8(u16);
}
inline int8 LoInt8(uint16 u16) {
  return int8(u16);
}
inline uint8 HiUint8(uint16 u16) {
  return uint8(u16 >> 8);
}
inline int8 HiInt8(uint16 u16) {
  return int8(u16 >> 8);
}
inline uint16 SwapUint16(uint16 u16){
  return uint16((u16 >> 8) | (u16 << 8));
}
inline uint32 SwapUint32(uint32 u32){
  return MkUint32(SwapUint16(HiUint16(u32)), SwapUint16(LoUint16(u32)));
}

//__OWL_END_NAMESPACE
//----------------------------------------------------------------------------

//
// Define library-wide function calling conventions. Must rebuild everything
// if changed, and must use appropriate modifier on derived classes.
//
#if !defined(_CALLCNVN)
#   define _CALLCNVN //__stdcall
#endif

#if !defined(_CCONV)
#define _CCONV __cdecl
#endif

// also exist from STL
#if !defined(BI_NO_MINMAX_TMPL)

//
// Give the RTL a chance to define min and max
//
#include <stdlib.h>

# if !defined( __MINMAX_DEFINED) && defined(__cplusplus)
#   define __MINMAX_DEFINED
#   undef min
#   undef max
template <class T> inline const T &min( const T &t1, const T &t2 ){
  return t2 < t1 ? t2 : t1;
}
template <class T> inline const T &max( const T &t1, const T &t2 ){
  return  t1 < t2 ? t2 : t1;
}
# endif
#endif

#if !defined(BI_NO_SWAP_TMPL)
template <class T> inline void Swap( T &a, T &b ) {
  a ^= b;  b ^= a;  a ^= b;
}
#endif

/* Minimum and maximum macros */
#if !defined(__max)
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(__min)
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

// find offset 
#if !defined(_OFFSETOF)
#define _OFFSETOF(s,m)   (size_t)&(((s*)0)->m)
#endif

#if !defined(BI_NO_CHECK_ARRAY)
  // The number of entries in an array, available at compile time
  // with check
# ifndef __GNUC__
    template <class T> void __a_size_chk(T*, T**) {}
    template <class T> void __a_size_chk(T* const, T* const *) {}
    template <class T> int  __a_size_chk(const void*, const T&) { return 0;}
    //template <class T> int  __a_size_chk(const void*, T) { return 0;}
#   define COUNTOF(a) (sizeof(__a_size_chk(a, &(a)))?sizeof(a)/sizeof((a)[0]):0)
# else // __GNUC__
#   include <cstddef>
    using std::size_t;
    template <typename T, size_t S> const char (& owl_lengthof(const T (&)[S])) [S];
    template <typename T, size_t S> char (& owl_lengthof(T (&)[S])) [S];
#   define owl_lengthof(a) sizeof(owl_lengthof(a))
#   define COUNTOF(a) (owl_lengthof(a))
# endif // __GNUC__
#else
# define COUNTOF(s)   (sizeof(s) / sizeof((s)[0]))
#endif

#if defined(BI_NOTHUNK)
#pragma message "Warning! BI_NOTHUNK is unsafe and deprecated!"
#endif

#include <limits.h>

#endif  // OWL_PRIVATE_DEFS_H
