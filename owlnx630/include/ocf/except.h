//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.13 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:47 $
//
// OCF Exception classes
//     NOTE: All OCF Exception classes are defined here except for TXAuto,
//           the exception used in automation failures. [See AUTODEFS.H
//           for TXAuto].
//----------------------------------------------------------------------------

#if !defined(OCF_EXCEPT_H)
#define OCF_EXCEPT_H

#if !defined(OCF_DEFS_H)
# include <ocf/defs.h>
#endif
#if !defined(OWL_EXBASE_H)
# include <owl/exbase.h>
#endif

#if !defined(OCF_DEFS_H)
# include <ocf/defs.h>
#endif
#if !defined(OWL_PRIVATE_OLE2INC_H)
# include <owl/private/ole2inc.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OCF_BEGIN_NAMESPACE

//
// class TXOle
// ~~~~~ ~~~~~
// OLE API related exception object
//
class _OCFCLASS TXOle : public __OWL TXBase {
  public:
    TXOle(const owl_string& msg, HRESULT stat);
    TXOle(const TXOle& src);
 #if defined(BI_STD_RTL)
    ~TXOle() throw();
 #else
    ~TXOle();
 #endif

#if defined(BI_NO_COVAR_RET)
    __OWL TXBase*	Clone();
#else
    TXOle*	Clone();
#endif
    void    Throw();

    static void Check(HRESULT stat, LPCTSTR msg);
    static void Check(HRESULT stat);
    static void Throw(HRESULT stat, LPCTSTR msg);
    static void OleErrorFromCode(HRESULT stat, LPTSTR buffer, int size);

    const long  Stat;
};

//
//
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline
TXOle::TXOle(const owl_string& msg, HRESULT stat)
      : __OWL TXBase(msg), Stat((long)stat)
{}

//
//
//
inline TXOle::TXOle(const TXOle& src)
             : __OWL TXBase(src), Stat(src.Stat)
{}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Macro to simply error checking of OLE calls
//
#if (__DEBUG > 0) || defined(__WARN)
# define OLECALL(func, msg) TXOle::Check(func, msg)
#else
# define OLECALL(func, msg) TXOle::Check(func)
#endif


//
// class TXObjComp
// ~~~~~ ~~~~~~~~~
// Base OC exception class
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class _OCFCLASS TXObjComp : public TXOle {
  public:
    enum TError {

      // Application Errors
      //
      xNoError,
      xBOleLoadFail,
      xBOleVersFail,
      xBOleBindFail,
      xDocFactoryFail,

      // Doc & Part Errors
      //
      xMissingRootIStorage,
      xInternalPartError,
      xPartInitError,

      // Storage Errors
      //
      xRootStorageOpenError,
      xStorageOpenError,
      xStorageILockError,
      xStreamOpenError,
    };

    TXObjComp(TError err, const owl_string& msg, HRESULT stat = HR_FAIL)
      : TXOle(msg, stat), ErrorCode(err) {}
    TXObjComp(const TXObjComp& src)
      : TXOle(src), ErrorCode(src.ErrorCode) {}
#	if defined(BI_STD_RTL)
   	~TXObjComp() throw();
#	else
   	~TXObjComp();
#	endif

#if defined(BI_NO_COVAR_RET)
    __OWL TXBase*	Clone();
#else
    TXObjComp*  Clone();
#endif
    void        Throw();

    static void Check(HRESULT stat, TError err, LPCTSTR msg = 0);
    static void Throw(TError err, HRESULT stat = HR_FAIL, LPCTSTR msg = 0);

    const TError ErrorCode;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

__OCF_END_NAMESPACE

#endif  //  OCF_EXCEPT_H
