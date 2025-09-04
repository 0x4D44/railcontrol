//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2007-05-11 15:29:18 $                                                    //
//
// Implementation of TGdiBase, base abstract class for all GDI objects that
// have Handles.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_GDIBASE_H)
# include <owl/gdibase.h>
#endif
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// TGdiBase constructors
//
TGdiBase::TGdiBase()
:
  Handle(0),
  ShouldDelete(true)
{
  // Handle must be set by derived class
}

//
//
//
TGdiBase::TGdiBase(HANDLE handle, TAutoDelete autoDelete)
:
  Handle(handle),
  ShouldDelete(autoDelete==AutoDelete)
{
  PRECONDITION(handle);
}

#if !defined(BI_COMP_GNUC)
#pragma warn -par   // resId is never used in small model
#endif
//
//
//
void
TGdiBase::CheckValid(uint resId)
{
  _CheckValid(Handle, resId);
}

//
//
//
void
TGdiBase::_CheckValid(HANDLE handle, uint resId)
{
  if (!handle)
    TXGdi::Raise(resId);
}
#if !defined(BI_COMP_GNUC)
#pragma warn .par
#endif

//----------------------------------------------------------------------------

//
// For backward compatibility
//
owl_string
TXGdi::Msg(uint resId, HANDLE handle)
{
  return MakeMessage(resId, uint(handle));
}

//
// Construct the object with a string resource and the handle
//
TXGdi::TXGdi(uint resId, HANDLE handle)
:
  TXOwl(MakeMessage(resId, uint(handle)), resId)
{
}

//
// Clone the exception object for safe-throwing across Windows.
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXGdi*
#endif
TXGdi::Clone()
{
  return new TXGdi(*this);
}

//
// Throw the exception.
//
void
TXGdi::Throw()
{
  THROW( *this );
}

//
// Throw the exception.
//
void
TXGdi::Raise(uint resId, HANDLE handle)
{
  TXGdi(resId, handle).Throw();
}

__OWL_END_NAMESPACE
/* ========================================================================== */

