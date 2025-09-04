//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// ObjectWindows exception class & function definitions
//----------------------------------------------------------------------------

#if !defined(OWL_EXCEPT_H)
#define OWL_EXCEPT_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(OWL_PRIVATE_EXCEPT_H)
# include <owl/private/except.h>
#endif
#if !defined(OWL_EXBASE_H)
# include <owl/exbase.h>
#endif
#if !defined(OWL_GMODULE_H)
# include <owl/private/gmodule.h>
#endif
#include <owl/except.rh>
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


//
// Global function to handle exceptions in OWL as a last resort
//
_OWLFUNC(int) HandleGlobalException(XBASE& x, LPCTSTR caption, LPCTSTR canResume=0);

//
// class TXOwl
// ~~~~~ ~~~~~
// TXOwl is root class of the ObjectWindows exception hierarchy.
//
class _OWLCLASS TXOwl : public TXBase {
  public:
    TXOwl(const owl_string& msg, uint resId = 0);
    TXOwl(uint resId, TModule* module = gModule);
#if defined(BI_STD_EXCEPTIONS)
   ~TXOwl() THROW_SPEC0;
#else
   ~TXOwl();
#endif

    // Override TXBase's virtuals
    //
#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXOwl* Clone();
#endif
    void Throw();

    // Per-exception class unhandled-handler, will default to the per-module
    // unhandled-handler
    //
    virtual int Unhandled(TModule* appModule, uint promptResId);

    uint GetErrorCode() const;

    static owl_string ResourceIdToString(bool* found, uint resId,
                                     TModule* module = gModule);
    static owl_string MakeMessage(uint resId, LPCTSTR infoStr, TModule* module = gModule);
#if defined(UNICODE)
    static owl_string MakeMessage(uint resId, LPCSTR infoStr, TModule* module = gModule);
#endif
    static owl_string MakeMessage(uint resId, uint infoNum, TModule* module = gModule);

    static void Raise(const owl_string& msg, uint resId = 0);
    static void Raise(uint resId, TModule* module = gModule);

  public:
    uint ResId;
};

//
// class TXOutOfMemory
// ~~~~~ ~~~~~~~~~~~~~
class _OWLCLASS TXOutOfMemory : public TXOwl {
  public:
    TXOutOfMemory();

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXOutOfMemory* Clone();
#endif
    void   Throw();

    static void Raise();
};

//
// class TXNotSupportedCall
// ~~~~~ ~~~~~~~~~~~~~~~~~~
class _OWLCLASS TXNotSupportedCall : public TXOwl {
  public:
    TXNotSupportedCall();

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXNotSupportedCall* Clone();
#endif
    void   Throw();

    static void Raise();
};


//
// class TXEndSession
// ~~~~~ ~~~~~~~~~~~~
// TXEndSession is thrown from TWindow's handler for WM_ENDSESSION.
// Since it is not derived from TXOwl, it does not fall into the same
// catch handler.
// It is used exclusively for shutting down the application.
//

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class _OWLCLASS_RTL TXEndSession {
  public:
    TXEndSession() {}
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

#if defined(OWL1_COMPAT)

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//----------------------------------------------------------------------------
// Owl 1.0 compatibility exception classes
//

//
// class TXCompatibility
// ~~~~~ ~~~~~~~~~~~~~~~
class _OWLCLASS TXCompatibility : public TXOwl {
  public:
    TXCompatibility(int statusCode);
    TXCompatibility(const TXCompatibility& src);

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXCompatibility* Clone();
#endif
    void             Throw();
    int              Unhandled(TModule* app, uint promptResId);

    static owl_string MapStatusCodeToString(int statusCode);

    // Member containing an OWL-1 compatible status code representing
    // an error condition.
    //
    int Status;
};

//
// class TStatus
// ~~~~~ ~~~~~~~
class _OWLCLASS TStatus {
  public:
    TStatus();
    TStatus& operator =(int statusCode);
    operator int() const;

  private:
    void Set(int statusCode);
    int  StatusCode;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline TStatus::TStatus() {
  StatusCode = 0;
}

//
inline TStatus& TStatus::operator =(int statusCode) {
  Set(statusCode);
  return *this;
}

//
inline TStatus::operator int() const {
  return StatusCode;
}

__OWL_END_NAMESPACE

#endif  // if defined(OWL1_COMPAT)

__OWL_BEGIN_NAMESPACE

//
inline uint TXOwl::GetErrorCode() const {
  return ResId;
}

__OWL_END_NAMESPACE


#endif  // OWL_EXCEPT_H
