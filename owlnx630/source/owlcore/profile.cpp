//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of TProfile class
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_PROFILE_H)
#include <owl/profile.h>
#endif
#if !defined(OWL_PRIVATE_MEMORY_H)
#include <owl/private/memory.h>
#endif

__OWL_BEGIN_NAMESPACE

//
// Use system profile for filename==0. Use system to search in normal places
// if no path is included in filename.
//
TProfile::TProfile(LPCTSTR section, LPCTSTR filename)
{
  Section  = section ? strnewdup(section) : 0;
#if defined(UNICODE)
  USES_CONVERSION;
#endif
  // Use OpenFile to track down the given filename
  //   if can't find it, use copy of original name,
  //   if found, use copy of full path
  //
  if (filename) {
    OFSTRUCT ofs;
    ofs.cBytes = sizeof ofs;
#if defined(UNICODE)
    FileName = strnewdup(
      (OpenFile(W2A(filename), &ofs, OF_EXIST) == HFILE_ERROR) ? filename : A2W(ofs.szPathName)
    );
#else
    FileName = strnewdup(
    //JJH
    #if defined(__GNUC__)
      (OpenFile(filename, &ofs, OF_EXIST) == HFILE_ERROR) ? filename : (LPCTSTR)ofs.szPathName
    #else
      (OpenFile(filename, &ofs, OF_EXIST) == HFILE_ERROR) ? filename : ofs.szPathName
    #endif
    );
#endif
  }
  else {
    FileName = 0;
  }
}

//
// Clean up buffers
//
TProfile::~TProfile()
{
  delete[] FileName;
  delete[] Section;
}

//
// Retrieve an integer value from the profile
//
int
TProfile::GetInt(LPCTSTR key, int defaultInt)
{
  return FileName
    ? ::GetPrivateProfileInt(Section, key, defaultInt, FileName)
    : ::GetProfileInt(Section, key, defaultInt);
}

//
// Return all section values if key==0
//
bool
TProfile::GetString(LPCTSTR key, LPTSTR buff, unsigned buffSize,
                    LPCTSTR defaultString)
{
  return FileName
    ? ::GetPrivateProfileString(Section, key, defaultString?defaultString:_T(""), buff, buffSize, FileName)
    : ::GetProfileString(Section, key, defaultString?defaultString:_T(""), buff, buffSize);
}

//
// Write an integer value to the profile
//
bool
TProfile::WriteInt(LPCTSTR key, int value)
{
  TCHAR buf[16];
#if defined __GNUC__
  _tprintf(buf, "%d", value);
#else
  _itot(value, buf, 10);
#endif
  return WriteString(key, buf);
}

//
// Write an string value to the profile
//
bool
TProfile::WriteString(LPCTSTR key, LPCTSTR str)
{
  return FileName
    ? ::WritePrivateProfileString(Section, key, str, FileName)
    : ::WriteProfileString(Section, key, str);
}

//
// Make sure that all written profile values are flushed to the actual file
//
void
TProfile::Flush()
{
  if (FileName)
    ::WritePrivateProfileString(0, 0, 0, FileName);
}

__OWL_END_NAMESPACE
/* ========================================================================== */
