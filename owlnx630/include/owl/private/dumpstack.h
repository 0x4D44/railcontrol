//----------------------------------------------------------------------------
// ObjectWindows
// (C) Copyright 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:48 $
//
//----------------------------------------------------------------------------
#if !defined(OWL_PRIVATE_DUMPSTACK_H)
#define OWL_PRIVATE_DUMPSTACK_H

#if !defined(OWL_PRIVATE_EXCEPT_H)
# include <owl/private/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

#define OWL_STACK_DUMP_TARGET_TRACE     0x0001
#define OWL_STACK_DUMP_TARGET_CLIPBOARD 0x0002
#define OWL_STACK_DUMP_TARGET_BOTH      0x0003
#define OWL_STACK_DUMP_TARGET_ODS       0x0004
#ifdef _DEBUG
#define OWL_STACK_DUMP_TARGET_DEFAULT   OWL_STACK_DUMP_TARGET_TRACE
#else
#define OWL_STACK_DUMP_TARGET_DEFAULT   OWL_STACK_DUMP_TARGET_CLIPBOARD
#endif

_OWLFUNC(void) OwlDumpStack(uint32 target = OWL_STACK_DUMP_TARGET_DEFAULT);


__OWL_END_NAMESPACE

#endif  // OWL_PRIVATE_DUMPSTACK_H
