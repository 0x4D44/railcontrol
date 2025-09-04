//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.24.2.8 $
//$Author: jogybl $
//$Date: 2010-03-29 07:33:49 $
//
//----------------------------------------------------------------------------

#if !defined(OCF_VERSION_H)
#define OCF_VERSION_H


// OCF Version Number (Major, Minor, Revision, Build)
// All other version numbers and strings should be defined in terms of this.
// NB! No leading zeroes in the numbers! (leading zero = octal)
//
#define _OCF_VERSION(v) v(6,30,13,4622)

// OCF File Version (major, minor, revision, build)
//
#define V_4WORD(major,minor,revision,build) major, minor, revision, build
#define OCFFileVersion _OCF_VERSION(V_4WORD)

// OCF Product version (major, minor, revision, build)
//
#define OCFProductVersion OCFFileVersion

// OCF File Version String ("major.minor.revision.build")
//
#define V_STRING(major,minor,revision,build) #major "." #minor "." #revision "." #build
#define OCFFileVersionString _OCF_VERSION(V_STRING)

// OCF Product Version String ("major.minor")
//
#define V_MAJORMINORSTRING(major,minor,revision,build) #major "." #minor
#define OCFProductVersionString _OCF_VERSION(V_MAJORMINORSTRING)

// 16 bit internal version number
// 0xMmNn, where M=Major/10, m=Major%10, N=Minor/10, n=Minor%10
// E.g. 0x0620 (6.20)
//
#define V_16BIT(major,minor,revision,build) (((major/10) << 12) | ((major%10) << 8) | ((minor/10) << 4) | (minor%10))
#define OCFVersion _OCF_VERSION(V_16BIT)

// 32-bit internal version number
// V_32BIT - 32 bit internal version number
// 0xMmNnRbbb, where M=Major/10, m=Major%10, N=Minor/10, n=minor%10, R=Revision, bbb=Build
// E.g. 0x06201193 (6.20.1.0403)
//
#define V_32BIT(major,minor,revision,build) (((V_16BIT(major, minor, 0, 0)) << 16) | ((revision%10) << 12) | build)
#define OCFInternalVersion _OCF_VERSION(V_32BIT)

// OCF DLL name base (e.g. "OCF620")
//
#define V_OCFDLL(major,minor,revision,build) "OCF" #major #minor
#define OCFDLLNameBase _OCF_VERSION(V_OCFDLL) 

// OCF DLL naming
//
#if defined(USERBUILD)
#  define OCFDLLName USERNAME
#else

#  if defined(__WIN32__)  || defined (_WIN32)

#    if defined(__BORLANDC__) || defined(WORKSHOP_INVOKED)
#      if defined(_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL)
#           if defined(_UNICODE)
#              define OCFDLLName OCFDLLNameBase "DTVU"
#           else
#              define OCFDLLName OCFDLLNameBase "DTV"
#           endif
#         else
#           if defined(_UNICODE)
#              define OCFDLLName OCFDLLNameBase "DTU"
#           else
#              define OCFDLLName OCFDLLNameBase "DT"
#           endif 
#         endif 
#        else
#          if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "DFU"
#          else 
#            define OCFDLLName OCFDLLNameBase "DF"
#          endif 
#        endif
#      else // defined (_DEBUG)
#        if defined(__MT__)
#         if defined(VCLRTL) 
#           if defined(_UNICODE) 
#              define OCFDLLName OCFDLLNameBase "TVU"
#           else 
#              define OCFDLLName OCFDLLNameBase "TV"
#           endif 
#         else 
#           if defined(_UNICODE) 
#              define OCFDLLName OCFDLLNameBase "TU"
#           else 
#              define OCFDLLName OCFDLLNameBase "T"
#           endif 
#         endif 
#        else
#         if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "FU"
#         else 
#            define OCFDLLName OCFDLLNameBase "F"
#         endif 
#        endif
#      endif // defined(_DEBUG)

#    elif defined(__GNUC__)
#      if defined(_DEBUG)
#        if defined(__MT__)
#          if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "GDTU"
#          else
#            define OCFDLLName OCFDLLNameBase "GDT"
#          endif
#        else
#          if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "GDFU"
#          else
#            define OCFDLLName OCFDLLNameBase "GDF"
#          endif
#        endif
#      else // defined(_DEBUG)
#        if defined(__MT__)
#         if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "GTU"
#          else
#            define OCFDLLName OCFDLLNameBase "GT"
#          endif
#        else
#         if defined(_UNICODE) 
#            define OCFDLLName OCFDLLNameBase "GU"
#          else
#            define OCFDLLName OCFDLLNameBase "G"
#          endif
#        endif
#      endif // defined(_DEBUG)

#    elif defined(MAINWIN)
#      if defined(_DEBUG)
#        define OCFDLLNAME OCFDLLNameBase "D"
#      else
#        define OCFDLLNAME OCFDLLNameBase
#      endif

#    elif defined(_MSC_VER) || defined(RC_INVOKED)
#      if defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OCFDLLName OCFDLLNameBase "VDU"
#        else
#          define OCFDLLName OCFDLLNameBase "VD"
#        endif
#      else // defined(_DEBUG)
#        if defined(_UNICODE) 
#          define OCFDLLName OCFDLLNameBase "VU"
#        else
#          define OCFDLLName OCFDLLNameBase "V"
#        endif
#      endif // defined(_DEBUG)

#    else
#     error must be defined OCFDLLName
#    endif

#  else // defined(__WIN32__)

#    if defined(_DEBUG)
#      define OCFDLLName OCFDLLNameBase "D"
#    else
#      define OCFDLLName OCFDLLNameBase
#    endif
#  endif // defined(__WIN32__)

#endif  // defined(USERBUILD)


#endif  //  OCF_VERSION_H
