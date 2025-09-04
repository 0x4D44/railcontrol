//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
// $Revision: 1.1.2.1 $
//----------------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop

#include <owlfx/version.h>


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
	OWLFXInternalVersion
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
char WhoAmI[] = "OWLFX, Codeguarded, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Byte aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Word aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Word aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Dword aligned";
#        endif
#      endif    
#    else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment";
#        endif
#      endif
#    endif    // end of "if   sizeof(__dummyStructure) == 5"
#  else
#    if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +TRACE";
#      endif
#    else
#      if defined(__WARN)
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment, +WARN";
#      else
char WhoAmI[] = "OWLFX, Codeguarded, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#else
#if defined(BI_COMP_BORLANDC)
#  if   sizeof(__dummyStructure) == 5
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Byte aligned";
#        endif
#      endif  // end if for "if defined(__TRACE)"    
#  elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Word aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Word aligned";
#        endif
#      endif    
#  elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OWLFX, Dword aligned";
#        endif
#      endif    
#  else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLFX, Unknown alignment, +TRACE";
#  endif
#else
#        if defined(__WARN)
char WhoAmI[] = "OWLFX, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OWLFX, Unknown alignment";
#        endif
#endif
#    endif  // end of "if sizeof(__dummyStructure == 5"
#  else
#if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OWLFX, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OWLFX, Unknown alignment, +TRACE";
#endif
#    else
#if defined(__WARN)
char WhoAmI[] = "OWLFX, Unknown alignment, +WARN";
#      else
//#        if defined(WINELIB)
//static
//#        endif //WINELIB
char WhoAmI[] = "OWLFX, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#endif    // end of "if defined(__CODEGUARD__)"


/* Jogy  Old code

#if	!defined(WHOAMI)
# define WHOAMI "OWLFX6"
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
{ OWLFXInternalVersion },
WHOAMI,
};

*/
