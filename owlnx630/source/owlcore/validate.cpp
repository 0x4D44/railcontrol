//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:44 $
//
// Implementation of TValidator, user input validator abstract base class
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_VALIDATE_H)
# include <owl/validate.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#include <stdlib.h>
#include <ctype.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

//
// Construct a validator
//
TValidator::TValidator()
{
  Options = 0;
}

//
//
//
TValidator::~TValidator()
{
}

//
// Validator error display. Overridden in derived classes
//
void
TValidator::Error(TWindow* /*owner*/)
{
}

//
// Checks current input against validator. May adjust input if suppressFill
// isn't set & validator has the voFill option set.
//
bool
TValidator::IsValidInput(_TCHAR *, bool /*suppressFill*/)
{
  return true;
}

//
// Checks input against validator for completeness. Never modifies input.
//
bool
TValidator::IsValid(LPCTSTR)
{
  return true;
}

//
//
//
uint
TValidator::Transfer(_TCHAR *, void*, TTransferDirection)
{
  return 0;
}

//
// Adjust the 'value' of the text, given a cursor position & an amount
// Return the actual amount adjusted.
//
int
TValidator::Adjust(owl_string& /*text*/, uint& /*begPos*/, uint& /*endPos*/, int /*amount*/)
{
  return 0;
}

//----------------------------------------------------------------------------

//
//
//
TXValidator::TXValidator(uint resId)
:
  TXOwl(resId)
{
}


//
// Copies the exception so it can be rethrown at a safer time.
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXValidator*
#endif
TXValidator::Clone()
{
  return new TXValidator(*this);
}

//
// Throws the exception.
//
void
TXValidator::Throw()
{
  THROW( *this );
}

//
// Creates an instance of TXValidator and throws it.
//
void
TXValidator::Raise()
{
  TXValidator().Throw();
}

#endif
#if !defined(SECTION) || SECTION == 2

#if !defined(BI_NO_OBJ_STREAMING)

IMPLEMENT_STREAMABLE(TValidator);

//
//
//
void*
TValidator::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  is >> GetObject()->Options;
  return GetObject();
}

//
//
//
void
TValidator::Streamer::Write(opstream& os) const
{
  os << GetObject()->Options;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

