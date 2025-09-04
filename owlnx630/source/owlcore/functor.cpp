//-----------------------------------------------------------------------------
// Visual Database Tools
// Copyright (c) 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// functor.cpp
// C++ Functor template implementation
//
// Based on Functors described in
//  Rich Hickey's 'Callbacks in C++ using template functors' Feb 95 C++ Report
//
//-----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_FUNCTOR_H)
#include <owl/functor.h>
#endif
//#include <memory.h>

__OWL_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
//
// Functor base class constructor implentation
//
TFunctorBase::TFunctorBase(const void* c, const void* f, size_t sz)
{
  if (c) {
    Callee = const_cast<void*>(c);
#if defined(_ANONYMOUS_STRUCT)
    memcpy(MemberFunc, f, sz);
#else
    memcpy(u.MemberFunc, f, sz);
#endif
  }
  else {
#if defined(_ANONYMOUS_STRUCT)
    Func = f;
#else
    u.Func = f;
#endif
  }
}

__OWL_END_NAMESPACE

