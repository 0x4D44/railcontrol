//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Module used to explicit expand/export templates used by ObjectWindows
//----------------------------------------------------------------------------
#include <owl/pch.h>

#undef __MINMAX_DEFINED
#undef max
#undef min

#if !defined(OWL_TEMPLATE_H)
# include <owl/template.h>
#endif
#if !defined(OWL_CONTAIN_H)
# include <owl/contain.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

// Generate the necessary instances. ?????????????
#if !defined(__GNUC__) //JJH added pragma option removal
#pragma option -Jgd
#endif

#if defined(BI_COMP_BORLANDC)

/*
typedef _OWLCLASS TObjectArray<string> fake_type1;
typedef _OWLCLASS TSortedObjectArray<string> fake_type2;
typedef _OWLCLASS TObjectArray<int> fake_type4;
typedef _OWLCLASS TObjectArray<uint32> fake_type5;
*/

#if defined(BI_IMPEXP_TEMPLATES)
# if defined(_BUILDOWLDLL)
// The template instances only need to be generated when building the
// ObjectWindows DLL - These instances are exported by OWL and imported
// by user code.
template class _OWLCLASS TObjectArray<string>;
template class _OWLCLASS TSortedObjectArray<string>;
template class _OWLCLASS TObjectArray<int>;
template class _OWLCLASS TObjectArray<uint32>;
# endif
#endif
#endif

__OWL_END_NAMESPACE


