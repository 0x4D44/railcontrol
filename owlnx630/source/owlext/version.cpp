//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
// $Revision: 1.1.2.1 $
//----------------------------------------------------------------------------
#include <owlext/pch.h>
#pragma hdrstop

#include <owlext/version.h>


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
	OWLEXTInternalVersion
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
char WhoAmI[] = "OWLEXT, Codeguarded, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Byte aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Word aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Word aligned";
#        endif
#      endif    
#    elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Dword aligned";
#        endif
#      endif    
#    else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment";
#        endif
#      endif
#    endif    // end of "if   sizeof(__dummyStructure) == 5"
#  else
#    if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +TRACE";
#      endif
#    else
#      if defined(__WARN)
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment, +WARN";
#      else
char WhoAmI[] = "OWLEXT, Codeguarded, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#else
#if defined(BI_COMP_BORLANDC)
#  if   sizeof(__dummyStructure) == 5
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Byte aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Byte aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Byte aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Byte aligned";
#        endif
#      endif  // end if for "if defined(__TRACE)"    
#  elif sizeof(__dummyStructure) == 6
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Word aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Word aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Word aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Word aligned";
#        endif
#      endif    
#  elif sizeof(__dummyStructure) == 8
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Dword aligned, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Dword aligned, +TRACE";
#        endif
#      else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Dword aligned, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Dword aligned";
#        endif
#      endif    
#  else
#      if defined(__TRACE)
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Unknown alignment, +TRACE, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Unknown alignment, +TRACE";
#  endif
#else
#        if defined(__WARN)
char WhoAmI[] = "OWLEXT, Unknown alignment, +WARN";
#        else
char WhoAmI[] = "OWLEXT, Unknown alignment";
#        endif
#endif
#    endif  // end of "if sizeof(__dummyStructure == 5"
#  else
#if defined(__TRACE)
#      if defined(__WARN)
char WhoAmI[] = "OWLEXT, Unknown alignment, +TRACE, +WARN";
#      else
char WhoAmI[] = "OWLEXT, Unknown alignment, +TRACE";
#endif
#    else
#if defined(__WARN)
char WhoAmI[] = "OWLEXT, Unknown alignment, +WARN";
#      else
//#        if defined(WINELIB)
//static
//#        endif //WINELIB
char WhoAmI[] = "OWLEXT, Unknown alignment";
#      endif
#endif
#  endif    // end of "if defined(BI_COMP_BORLANDC)
#endif    // end of "if defined(__CODEGUARD__)"


/* Jogy  Old code

#if	!defined(WHOAMI)
# define WHOAMI "OWLEXT6"
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
{ OWLEXTInternalVersion },
WHOAMI,
};

*/
