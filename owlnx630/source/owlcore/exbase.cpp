//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All rights
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:58 $
//
// TXBase class implementation.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/exbase.h>


//------------------------------------------------------------------------------
#if !defined(BI_COMP_BORLANDC) 
#  if !defined(BI_XALLOC_DEFINED)
_OWLENTRY xalloc::xalloc( const owl_string &msg, size_t size)
:
  XBASE((LPCSTR)msg.c_str())
{
  str = msg;
}
_OWLENTRY xalloc::xalloc()
:
  XBASE("")
{
  str = owl_string();
}
_OWLENTRY xalloc::xalloc(const xalloc & msg)
{
  str = msg.str;
}
void _OWLENTRY xalloc::raise() THROW_SPEC1( xalloc )
{
  throw *this;
}
_OWLENTRY xalloc::~xalloc()
{
}
LPCSTR _OWLENTRY xalloc::what() const THROW_SPEC1( xalloc )
{
  //return (LPCSTR)str->c_str();
  //Fix by Mark Hatsell - convert from Unicode

  _USES_CONVERSION;
  return (LPCSTR)_W2A(str.c_str());
}
xalloc & _OWLENTRY xalloc::operator=(const xalloc & src)
{
  if(this != &src){
    str = src.str;
  }
  return(*this);
}
#  endif // BI_XALLOC_DEFINED
#endif // BI_COMP_BORLANDC

__OWL_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
int TXBase::InstanceCount = 0;

TXBase::TXBase(const owl_string& msg)
#if defined(BI_COMP_BORLANDC)
#  if !defined(BI_STD_EXCEPTIONS)
:
  XBASE(msg)
#  endif
#else
#  if !defined(BI_STD_EXCEPTIONS)
#    if defined(UNICODE)
#      error This code is not supported under Unicode when no standard exceptions are available
#    else
:
  XBASE((LPCSTR)msg.c_str())
#    endif
#  endif
#endif
{
  InstanceCount++;
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
  str = msg;
#endif
}

TXBase::TXBase(const TXBase& src)
#if defined(BI_COMP_BORLANDC)
:  XBASE(src)
#else
#  if !defined(BI_STD_EXCEPTIONS)
:  XBASE(src.what())
# endif
#endif
{
  InstanceCount++;
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
  str = src.str;
#endif
}

#if defined(BI_STD_EXCEPTIONS)
TXBase::~TXBase() THROW_SPEC0
#else
TXBase::~TXBase() 
#endif
{
  InstanceCount--;
}

TXBase*
TXBase::Clone()
{
  return new TXBase(*this);
}

void
TXBase::Throw()
{
  THROW( *this );
}

//
// Construct a TXBase exception from scratch, and throw it
//
void
TXBase::Raise(const owl_string& msg)
{
  TXBase(msg).Throw();
}
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
const char * _OWLENTRY TXBase::what() const THROW_SPEC0 
{
  //return (LPCSTR)str->c_str();
  //Fix by Mark Hatsell - convert from Unicode
	
  _USES_CONVERSION;
  return (LPCSTR)_W2A(str.c_str());
}
TXBase& _OWLENTRY TXBase::operator=(const TXBase &src)
{
  if (this != &src)
  {
    str = src.str;
  }
  return(*this);
}
#endif
//------------------------------------------------------------------------------

__OWL_END_NAMESPACE

