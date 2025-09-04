//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1.2.7 $
//$Author: jogybl $
//$Date: 2010-03-29 07:36:47 $
//
//----------------------------------------------------------------------------

#if !defined(OWLFX_VERSION_H)
#define OWLFX_VERSION_H


// OWLFX Version Number (Major, Minor, Revision, Build)
// All other version numbers and strings should be defined in terms of this.
// NB! No leading zeroes in the numbers! (leading zero = octal)
//
#define _OWLFX_VERSION(v) v(6,30,13,4622)

// OWLFX File Version (major, minor, revision, build)
//
#define V_4WORD(major,minor,revision,build) major, minor, revision, build
#define OWLFXFileVersion _OWLFX_VERSION(V_4WORD)

// OWLFX Product version (major, minor, revision, build)
//
#define OWLFXProductVersion OWLFXFileVersion

// OWLFX File Version String ("major.minor.revision.build")
//
#define V_STRING(major,minor,revision,build) #major "." #minor "." #revision "." #build
#define OWLFXFileVersionString _OWLFX_VERSION(V_STRING)

// OWLFX Product Version String ("major.minor")
//
#define V_MAJORMINORSTRING(major,minor,revision,build) #major "." #minor
#define OWLFXProductVersionString _OWLFX_VERSION(V_MAJORMINORSTRING)

// 16 bit internal version number
// 0xMmNn, where M=Major/10, m=Major%10, N=Minor/10, n=Minor%10
// E.g. 0x0620 (6.20)
//
#define V_16BIT(major,minor,revision,build) (((major/10) << 12) | ((major%10) << 8) | ((minor/10) << 4) | (minor%10))
#define OWLFXVersion _OWLFX_VERSION(V_16BIT)

// 32-bit internal version number
// V_32BIT - 32 bit internal version number
// 0xMmNnRbbb, where M=Major/10, m=Major%10, N=Minor/10, n=minor%10, R=Revision, bbb=Build
// E.g. 0x06201193 (6.20.1.0403)
//
#define V_32BIT(major,minor,revision,build) (((V_16BIT(major, minor, 0, 0)) << 16) | ((revision%10) << 12) | build)
#define OWLFXInternalVersion _OWLFX_VERSION(V_32BIT)

// OWLFX DLL name base (e.g. "OWLFX620")
//
#define V_OWLFXDLL(major,minor,revision,build) "OWLFX" #major #minor
#define OWLFXDLLNameBase _OWLFX_VERSION(V_OWLFXDLL) 

// OWLFX DLL naming
//
#if defined(USERBUILD)
#  define OWLFXDLLName USERNAME
#else

#  if defined(__WIN32__)  || defined (_WIN32)

#    if defined(__BORLANDC__) || defined(WORKSHOP_INVOKED)
#      if defined(_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL)
#           if defined(_UNICODE)
#              define OWLFXDLLName OWLFXDLLNameBase "DTVU"
#           else
#              define OWLFXDLLName OWLFXDLLNameBase "DTV"
#           endif
#         else
#           if defined(_UNICODE)
#              define OWLFXDLLName OWLFXDLLNameBase "DTU"
#           else
#              define OWLFXDLLName OWLFXDLLNameBase "DT"
#           endif 
#         endif 
#        else
#          if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "DFU"
#          else 
#            define OWLFXDLLName OWLFXDLLNameBase "DF"
#          endif 
#        endif
#      else // defined (_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL) 
#           if defined(_UNICODE) 
#              define OWLFXDLLName OWLFXDLLNameBase "TVU"
#           else 
#              define OWLFXDLLName OWLFXDLLNameBase "TV"
#           endif 
#         else 
#           if defined(_UNICODE) 
#              define OWLFXDLLName OWLFXDLLNameBase "TU"
#           else 
#              define OWLFXDLLName OWLFXDLLNameBase "T"
#           endif 
#         endif 
#        else
#         if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "FU"
#         else 
#            define OWLFXDLLName OWLFXDLLNameBase "F"
#         endif 
#        endif
#      endif // defined(_DEBUG)

#    elif defined(__GNUC__)
#      if defined(_DEBUG)
#        if defined(__MT__)
#          if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "GDTU"
#          else
#            define OWLFXDLLName OWLFXDLLNameBase "GDT"
#          endif
#        else
#          if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "GDFU"
#          else
#            define OWLFXDLLName OWLFXDLLNameBase "GDF"
#          endif
#        endif
#      else // defined(_DEBUG)
#        if defined(__MT__)
#         if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "GTU"
#          else
#            define OWLFXDLLName OWLFXDLLNameBase "GT"
#          endif
#        else
#         if defined(_UNICODE) 
#            define OWLFXDLLName OWLFXDLLNameBase "GU"
#          else
#            define OWLFXDLLName OWLFXDLLNameBase "G"
#          endif
#        endif
#      endif // defined(_DEBUG)

#    elif defined(MAINWIN)
#      if defined(_DEBUG)
#        define OWLFXDLLNAME OWLFXDLLNameBase "D"
#      else
#        define OWLFXDLLNAME OWLFXDLLNameBase
#      endif

#    elif defined(_MSC_VER) || defined(RC_INVOKED)
#      if defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OWLFXDLLName OWLFXDLLNameBase "VDU"
#        else
#          define OWLFXDLLName OWLFXDLLNameBase "VD"
#        endif
#      else // defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OWLFXDLLName OWLFXDLLNameBase "VU"
#        else
#          define OWLFXDLLName OWLFXDLLNameBase "V"
#        endif
#      endif // defined(_DEBUG)

#    else
#     error must be defined OWLFXDLLName
#    endif

#  else // defined(__WIN32__)

#    if defined(_DEBUG)
#      define OWLFXDLLName OWLFXDLLNameBase "D"
#    else
#      define OWLFXDLLName OWLFXDLLNameBase
#    endif
#  endif // defined(__WIN32__)

#endif  // defined(USERBUILD)


#endif  //  OWLFX_VERSION_H
