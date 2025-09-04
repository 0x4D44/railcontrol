//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.12 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:17 $
//
// Actual BOcOle GUIDs spit out here
//
// NOTE! This file must be compiled with the following flags for 16-bit:
//   -zE_TEXT -zFCODE    (this forces the GUIDs into the code segment)
//----------------------------------------------------------------------------
#define INC_OLE2
//#include <ocf/pch.h>
//#pragma hdrstop

#define INITGUID              // Force OLE GUIDs to be defined
#define INIT_BGUID            // Force Bolero GUIDs to be defined

//Krasi-> warning C4005: '_OWLPCH': macro redefinition
#if !defined(_OWLPCH)
# define _OWLPCH
#endif

#include <owl/pch.h>					
#include <ocf/defs.h>         // Ole basics & GUIDs
#if !defined(OCF_OCBOCOLE_H)
# include <ocf/ocbocole.h>    // Bolero GUIDs
#endif

#if !defined(_OLECTL_H)
#include <olectl.h>         // Ole control GUIDs
#endif

