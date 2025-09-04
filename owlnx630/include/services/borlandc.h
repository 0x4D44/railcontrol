//----------------------------------------------------------------------------
// Borland Services Library
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1 $
//
// Compiler-specific defines for Borland C/C++. Included only by compiler.h
//----------------------------------------------------------------------------

#ifdef _MSC_VER 
#   pragma once
#endif

#if !defined(SERVICES_BORLANDC_H)
#define SERVICES_BORLANDC_H

#pragma message ("services/borland.h has been replaced by owl/private/borland.h")
#include <owl/private/borland.h>
#endif  // SERVICES_BORLANDC_H
