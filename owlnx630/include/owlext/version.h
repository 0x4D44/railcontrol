//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1.2.7 $
//$Author: jogybl $
//$Date: 2010-03-29 07:36:47 $
//
//----------------------------------------------------------------------------

#if !defined(OWLEXT_VERSION_H)
#define OWLEXT_VERSION_H


// OWLEXT Version Number (Major, Minor, Revision, Build)
// All other version numbers and strings should be defined in terms of this.
// NB! No leading zeroes in the numbers! (leading zero = octal)
//
#define _OWLEXT_VERSION(v) v(6,30,13,4622)

// OWLEXT File Version (major, minor, revision, build)
//
#define V_4WORD(major,minor,revision,build) major, minor, revision, build
#define OWLEXTFileVersion _OWLEXT_VERSION(V_4WORD)

// OWLEXT Product version (major, minor, revision, build)
//
#define OWLEXTProductVersion OWLEXTFileVersion

// OWLEXT File Version String ("major.minor.revision.build")
//
#define V_STRING(major,minor,revision,build) #major "." #minor "." #revision "." #build
#define OWLEXTFileVersionString _OWLEXT_VERSION(V_STRING)

// OWLEXT Product Version String ("major.minor")
//
#define V_MAJORMINORSTRING(major,minor,revision,build) #major "." #minor
#define OWLEXTProductVersionString _OWLEXT_VERSION(V_MAJORMINORSTRING)

// 16 bit internal version number
// 0xMmNn, where M=Major/10, m=Major%10, N=Minor/10, n=Minor%10
// E.g. 0x0620 (6.20)
//
#define V_16BIT(major,minor,revision,build) (((major/10) << 12) | ((major%10) << 8) | ((minor/10) << 4) | (minor%10))
#define OWLEXTVersion _OWLEXT_VERSION(V_16BIT)

// 32-bit internal version number
// V_32BIT - 32 bit internal version number
// 0xMmNnRbbb, where M=Major/10, m=Major%10, N=Minor/10, n=minor%10, R=Revision, bbb=Build
// E.g. 0x06201193 (6.20.1.0403)
//
#define V_32BIT(major,minor,revision,build) (((V_16BIT(major, minor, 0, 0)) << 16) | ((revision%10) << 12) | build)
#define OWLEXTInternalVersion _OWLEXT_VERSION(V_32BIT)

// OWLEXT DLL name base (e.g. "OWLEXT620")
//
#define V_OWLEXTDLL(major,minor,revision,build) "OWLEXT" #major #minor
#define OWLEXTDLLNameBase _OWLEXT_VERSION(V_OWLEXTDLL) 

// OWLEXT DLL naming
//
#if defined(USERBUILD)
#  define OWLEXTDLLName USERNAME
#else

#  if defined(__WIN32__)  || defined (_WIN32)

#    if defined(__BORLANDC__) || defined(WORKSHOP_INVOKED)
#      if defined(_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL)
#           if defined(_UNICODE)
#              define OWLEXTDLLName OWLEXTDLLNameBase "DTVU"
#           else
#              define OWLEXTDLLName OWLEXTDLLNameBase "DTV"
#           endif
#         else
#           if defined(_UNICODE)
#              define OWLEXTDLLName OWLEXTDLLNameBase "DTU"
#           else
#              define OWLEXTDLLName OWLEXTDLLNameBase "DT"
#           endif 
#         endif 
#        else
#          if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "DFU"
#          else 
#            define OWLEXTDLLName OWLEXTDLLNameBase "DF"
#          endif 
#        endif
#      else // defined (_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL) 
#           if defined(_UNICODE) 
#              define OWLEXTDLLName OWLEXTDLLNameBase "TVU"
#           else 
#              define OWLEXTDLLName OWLEXTDLLNameBase "TV"
#           endif 
#         else 
#           if defined(_UNICODE) 
#              define OWLEXTDLLName OWLEXTDLLNameBase "TU"
#           else 
#              define OWLEXTDLLName OWLEXTDLLNameBase "T"
#           endif 
#         endif 
#        else
#         if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "FU"
#         else 
#            define OWLEXTDLLName OWLEXTDLLNameBase "F"
#         endif 
#        endif
#      endif // defined(_DEBUG)

#    elif defined(__GNUC__)
#      if defined(_DEBUG)
#        if defined(__MT__)
#          if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "GDTU"
#          else
#            define OWLEXTDLLName OWLEXTDLLNameBase "GDT"
#          endif
#        else
#          if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "GDFU"
#          else
#            define OWLEXTDLLName OWLEXTDLLNameBase "GDF"
#          endif
#        endif
#      else // defined(_DEBUG)
#        if defined(__MT__)
#         if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "GTU"
#          else
#            define OWLEXTDLLName OWLEXTDLLNameBase "GT"
#          endif
#        else
#         if defined(_UNICODE) 
#            define OWLEXTDLLName OWLEXTDLLNameBase "GU"
#          else
#            define OWLEXTDLLName OWLEXTDLLNameBase "G"
#          endif
#        endif
#      endif // defined(_DEBUG)

#    elif defined(MAINWIN)
#      if defined(_DEBUG)
#        define OWLEXTDLLNAME OWLEXTDLLNameBase "D"
#      else
#        define OWLEXTDLLNAME OWLEXTDLLNameBase
#      endif

#    elif defined(_MSC_VER) || defined(RC_INVOKED)
#      if defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OWLEXTDLLName OWLEXTDLLNameBase "VDU"
#        else
#          define OWLEXTDLLName OWLEXTDLLNameBase "VD"
#        endif
#      else // defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OWLEXTDLLName OWLEXTDLLNameBase "VU"
#        else
#          define OWLEXTDLLName OWLEXTDLLNameBase "V"
#        endif
#      endif // defined(_DEBUG)

#    else
#     error must be defined OWLEXTDLLName
#    endif

#  else // defined(__WIN32__)

#    if defined(_DEBUG)
#      define OWLEXTDLLName OWLEXTDLLNameBase "D"
#    else
#      define OWLEXTDLLName OWLEXTDLLNameBase
#    endif
#  endif // defined(__WIN32__)

#endif  // defined(USERBUILD)


#endif  //  OWLEXT_VERSION_H
