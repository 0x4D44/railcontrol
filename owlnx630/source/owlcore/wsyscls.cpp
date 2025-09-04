//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:20 $
//
// Implementation of window system structure and type encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_WSYSCLS_H)
#include <owl/wsyscls.h>
#endif
#if !defined(OWL_PRIVATE_MEMORY_H)
#include <owl/private/memory.h>
#endif
#if !defined(OWL_SHELLITM_H)
# include <owl/shellitm.h>
#endif
#if !defined(OWL_TIME_H)
#include <owl/time.h>
#endif

__OWL_BEGIN_NAMESPACE

//
// Drag / Drop support
//
TFileDroplet::TFileDroplet(LPCTSTR fileName, const TPoint& p, bool inClient)
:
  FileName(strnewdup(fileName)),
  Point(p),
  InClientArea(inClient)
{
}

//
// Construct a TFileDroplet given a DropInfo and a file index
//
// The location is relative to the client coordinates, and will have negative
// values if dropped in the non client partsof the window.
//
// DragQueryPoint copies that point where the file was dropped and returns
// whether or not the point is in the client area.  Regardless of whether or
// not the file is dropped in the client or non-client area of the window,
// you will still receive the file name.
//
TFileDroplet::TFileDroplet(TDropInfo& drop, int i)
{
  // Tell DragQueryFile the file wanted (i) and the length of the buffer.
  //
  int  namelen = drop.DragQueryFileNameLen(i) + 1;
  FileName = new _TCHAR[namelen];

  drop.DragQueryFile(i, FileName, namelen);

  InClientArea = drop.DragQueryPoint(Point);
}

//
// Clean up the new'd filename
//
TFileDroplet::~TFileDroplet()
{
  delete[] FileName;
}

//
//
//
// Information access
//
uint 
TDropInfo::DragQueryFile(uint index, LPTSTR name, uint nameLen)
{
  PRECONDITION(name && nameLen);
  return TShell::DragQueryFile(Handle, index, name, nameLen);
}
//
//
//
uint 
TDropInfo::DragQueryFileCount() 
{
  return TShell::DragQueryFile(Handle, uint(-1), 0, 0);
}
//
//
//
uint 
TDropInfo::DragQueryFileNameLen(uint index)
{
  return TShell::DragQueryFile(Handle, index, 0, 0);
}
//
//
//
bool 
TDropInfo::DragQueryPoint(TPoint& point)
{
  return TShell::DragQueryPoint(Handle, &point);
}
//
//
//
void 
TDropInfo::DragFinish() 
{
  TShell::DragFinish(Handle);
}
//
//
//
//void __MakeDosTime(const TTime& ctm, uint16& fdm, uint16& ftm);
TFileTime::TFileTime(const TTime& tm)
{
  *this = tm.GetFileTime();
  //uint16 fdate, ftime;
  //__MakeDosTime(tm, fdate, ftime);
  //DosDateTimeToFileTime(fdate,ftime,this)
}
//
//
//
TFileTime& 
TFileTime::operator=(const TTime& tm)
{
  *this = tm.GetFileTime();
  //uint16 fdate, ftime;
  //__MakeDosTime(tm, fdate, ftime);
  //DosDateTimeToFileTime(fdate,ftime,this)
  return *this;
}
//
//
//
bool TFileTime::ToLocalTime()
{ 
  TFileTime tm; 
  bool retval = ::FileTimeToLocalFileTime(this, &tm); 
  *this = tm;
  return retval; 
}
//
//
//
bool TFileTime::ToUniversalTime()
{ 
  TFileTime tm; 
  bool retval = ::LocalFileTimeToFileTime(this, &tm); 
  *this = tm;
  return retval; 
}
//
//
//

__OWL_END_NAMESPACE
/* ========================================================================== */
