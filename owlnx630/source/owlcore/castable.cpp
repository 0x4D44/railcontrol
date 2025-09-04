//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:17 $
//
//  RTTI replacement class
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
#include <owl/defs.h>
#endif
#if !defined(OWL_OBJSTRM_H)
#include <owl/objstrm.h>
#endif

__OWL_BEGIN_NAMESPACE

TStreamableBase::~TStreamableBase()
{
}

#if defined(BI_NO_RTTI)

void *TStreamableBase::FindBase( Type_id ) const
{
    return 0;
}

#endif

__OWL_END_NAMESPACE

