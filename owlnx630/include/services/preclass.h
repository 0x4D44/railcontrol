//----------------------------------------------------------------------------
// Borland Services Library
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1 $
//
// Provides generic definitions preceeding the declaration the classes of
// the ClassLibrary, ObjectComponents and ObjectWindows
// NOTE: This header must *NOT* contain guards
//----------------------------------------------------------------------------

#ifdef _MSC_VER 
#   pragma once
#endif

#pragma nopackwarning
#pragma message ("Warning!!! Included obsolete file: <services/preclass.h> use <owl/preclass.h instead>")
#if ! (defined(lint) || defined(_lint))
#  ifndef RC_INVOKED
#    if ( _MSC_VER >= 800 ) || (defined(__BORLANDC__))
#      pragma warning(disable:4103)
#      pragma pack(BI_OWL_PACKING)
#    endif
#  endif // ndef RC_INVOKED
#endif // ! (defined(lint) || defined(_lint))

