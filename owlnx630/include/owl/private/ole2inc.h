//----------------------------------------------------------------------------
// Borland Services Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//
// Includes OLE2 headers in the right order with necessary macros defined
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_OLE2INC_H)
#define OWL_PRIVATE_OLE2INC_H

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
#pragma read_only_file
#endif

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif

//
// If wsysinc.h hasn't been included yet, then let it pull in OLE2 if it can
//
#if !defined(OWL_PRIVATE_WSYSINC_H)
# if !defined(INC_OLE2)
#   define INC_OLE2
# endif
# include <owl/private/wsysinc.h>
#endif

//
// Well, if wsysinc.h already was included, or it didn't get OLE2 for us, then
// try to get the headers now.
// NOTE: If 'BOOL to bool' remapping was requested, OLE2 headers *must* be
//       included via 'services/wsysinc.h'; i.e. define 'INC_OLE2' before
//       including <services/wsysinc.h>
//
//
#if !defined(__OLE2_H) && !defined(_OLE2_H_) && !defined(_OLE2_H)
#error Win32 Ole2 headers cannot be included after Windows.h. Define INC_OLE2 first.
#endif

//
// OLECHAR & BSTR is char under Win16, and wchar_t under Win32
//
# define BI_OLECHAR_WIDE    // OLECHAR is wide (wchar_t)
# define BI_HAS_WCHAR       // platform has wide char functions available

#endif  // SERVICES_OLE2INC_H
