//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Include for ObjectWindows, gets owlcore.h or owlall.h when precompiled
// headers are enabled.
//----------------------------------------------------------------------------
//#ifdef _PCH_H
//#define _PCH_H


#if defined(_OWLVCLPCH) && !defined(OWL_OWLVCL_H)
# include <owl/owlvcl.h>
#endif

#if defined(_OWLALLPCH) && !defined(OWL_OWLALL_H)
#  include <owl/owlall.h>
#elif defined(_OWLPCH) && !defined(OWL_OWLCORE_H)
#  include <owl/owlcore.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

//#endif // _PCH_H
