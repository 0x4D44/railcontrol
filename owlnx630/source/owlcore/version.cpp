//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// ObjectWindows global version structure for vprint
//----------------------------------------------------------------------------

#include <owl/pch.h>

#if !defined(OWL_VERSION_H)
# include <owl/version.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

struct __dummyStructure {
 char   c;
 long   l;
};

struct TVersionHeader {
  char          Signature[6];
  unsigned long InternalVersionNumber;
  /*
  char          ComponentIdString[];
  */
};

// NOTE: The following variables must be contiguous in memory
//
//#if defined(WINELIB)
//static
//#endif
TVersionHeader VersionHeader = {
  {'$', '*', '$', '*', '$', '*'},
  OWLInternalVersion
};

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// JBC & JRS    
//  Rewrote the char WhoAmI[] to work with the OpenC++ compiler
//  OpenC++ had issues with #define in midst of a string  
//
//    Original Code commented out below with "//Orig <code line>"
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#if defined(__CODEGUARD__)
#  if defined(BI_COMP_BORLANDC)
#    if   sizeof(__dummyStructure) == 5
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Byte aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Byte aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Word aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Word aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Dword aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Dword aligned";
#        endif
#      endif    
#    else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment";
#        endif
#      endif
#    endif    // end of "if   sizeof(__dummyStructure) == 5"
#  else
#    if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +TRACE";
#      endif
#    else
#      if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment, +WARN";
#      else
char WhoAmI[] = "ObjectWindows, Codeguarded, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#else
#if defined(BI_COMP_BORLANDC)
#  if   sizeof(__dummyStructure) == 5
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Byte aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Byte aligned";
#        endif
#      endif  // end if for "if defined(__TRACE)"    
#  elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Word aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Word aligned";
#        endif
#      endif    
#  elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Dword aligned, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Dword aligned";
#        endif
#      endif    
#  else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Unknown alignment, +TRACE";
#  endif
#else
#        if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "ObjectWindows, Unknown alignment";
#        endif
#endif
#    endif  // end of "if sizeof(__dummyStructure == 5"
#  else
#if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "ObjectWindows, Unknown alignment, +TRACE";
#endif
#    else
#if defined(__WARN)
char WhoAmI[] = "ObjectWindows, Unknown alignment, +WARN";
#      else
//#        if defined(WINELIB)
//static
//#        endif //WINELIB
char WhoAmI[] = "ObjectWindows, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#endif    // end of "if defined(__CODEGUARD__)"

//Orig char WhoAmI[] = "ObjectWindows"
//Orig 
//Orig // Codeguard
//Orig //
//Orig #if defined(__CODEGUARD__)
//Orig   ", Codeguarded"
//Orig #endif
//Orig 
//Orig // Alignment
//Orig //
//Orig #if defined(BI_COMP_BORLANDC)
//Orig #  if   sizeof(__dummyStructure) == 5
//Orig     ", Byte aligned"
//Orig #  elif sizeof(__dummyStructure) == 6
//Orig     ", Word aligned"
//Orig #  elif sizeof(__dummyStructure) == 8
//Orig     ", Dword aligned"
//Orig #  else
//Orig     ", Unknown alignment"
//Orig #  endif
//Orig #else
//Orig     ", Unknown alignment"
//Orig #endif
//Orig 
//Orig // Diagnostics
//Orig //
//Orig #if defined(__TRACE)
//Orig   ", +TRACE"
//Orig #endif
//Orig #if defined(__WARN)
//Orig   ", +WARN"
//Orig #endif
//Orig ;

__OWL_END_NAMESPACE
/* ========================================================================== */

