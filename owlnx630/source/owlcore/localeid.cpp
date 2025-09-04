//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// TLocaleString default native language for symbols, only if not user-defined
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
#include <owl/lclstrng.h>
#endif

__OWL_BEGIN_NAMESPACE

TLangId TLocaleString::NativeLangId = TLocaleString::GetSystemLangId();

__OWL_END_NAMESPACE

