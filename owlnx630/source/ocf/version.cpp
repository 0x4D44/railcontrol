//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
// $Revision: 1.9 $
//----------------------------------------------------------------------------
#define INC_OLE2
#include <ocf/pch.h>
#pragma hdrstop

#include <ocf/version.h>


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
  OCFInternalVersion
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
char WhoAmI[] = "OCF, Codeguarded, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Byte aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Word aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Word aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Dword aligned";
#        endif
#      endif    
#    else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment";
#        endif
#      endif
#    endif    // end of "if   sizeof(__dummyStructure) == 5"
#  else
#    if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +TRACE";
#      endif
#    else
#      if defined(__WARN)
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment, +WARN";
#      else
char WhoAmI[] = "OCF, Codeguarded, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#else
#if defined(BI_COMP_BORLANDC)
#  if   sizeof(__dummyStructure) == 5
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Byte aligned";
#        endif
#      endif  // end if for "if defined(__TRACE)"    
#  elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Word aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Word aligned";
#        endif
#      endif    
#  elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OCF, Dword aligned";
#        endif
#      endif    
#  else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OCF, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OCF, Unknown alignment, +TRACE";
#  endif
#else
#        if defined(__WARN)
char WhoAmI[] = "OCF, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OCF, Unknown alignment";
#        endif
#endif
#    endif  // end of "if sizeof(__dummyStructure == 5"
#  else
#if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OCF, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OCF, Unknown alignment, +TRACE";
#endif
#    else
#if defined(__WARN)
char WhoAmI[] = "OCF, Unknown alignment, +WARN";
#      else
//#        if defined(WINELIB)
//static
//#        endif //WINELIB
char WhoAmI[] = "OCF, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#endif    // end of "if defined(__CODEGUARD__)"


/* Jogy  Old code

#if	!defined(WHOAMI)
# define WHOAMI "OCF6"
#endif

struct VersionHeader {
  // The next three fields must be consecutive for VPRINT to locate
  // the info
	// Therefore, watch for 'improper' alignment settings.
	//
  char          VprintSignature[6];
  unsigned long InternalVersion;
  char          ComponentIDstring[ sizeof( WHOAMI )];
};

// The following is for VPRINT's usage
//
VersionHeader HeaderVersion = {
  {'$', '*', '$', '*', '$', '*'},
  { OCFInternalVersion },
  WHOAMI,
};

*/
