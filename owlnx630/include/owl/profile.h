//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-05-28 14:29:39 $
//
// Definition of TProfile class
//----------------------------------------------------------------------------

#if !defined(OWL_PROFILE_H)
#define OWL_PROFILE_H

// Jogy, 21.05.2007 - changed from including owl/private/defs.h
//                    to avoid a wint_t related compile error in Borland Developer Studio
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE
//FMM had to include LPCTSTR on HPUX for some reason
#ifndef LPCTSTR
#include <windows.h>
#endif

//
// class TProfile
// ~~~~~ ~~~~~~~~
class _OWLCLASS TProfile  {
  public:
    // Use system global profile for filename==0
    //
    TProfile(LPCTSTR section, LPCTSTR filename = 0);
   ~TProfile();

    int GetInt(LPCTSTR key, int defaultInt = 0);

    // Returns all section values if key==0
    //
    bool GetString(LPCTSTR key, LPTSTR buff,
                   unsigned buffSize, LPCTSTR defaultString = 0);

    bool WriteInt(LPCTSTR key, int value);
    bool WriteString(LPCTSTR key, LPCTSTR str);
    void Flush();

  protected:
    LPTSTR Section;     // Name of the section to use
    LPTSTR FileName;    // File name of the .INI file
};


__OWL_END_NAMESPACE


#endif  // OWL_PROFILE_H
