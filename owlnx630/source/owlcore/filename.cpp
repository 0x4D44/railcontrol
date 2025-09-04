//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31.2.1 $
//$Author: jogybl $
//$Date: 2009-12-16 09:10:46 $
//
// TFileName class implementation.
// 22/06/2002 added path only processing (suggestion by Jogy)
//----------------------------------------------------------------------------
#if defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#include <owl/filename.h>
#include <owl/private/memory.h>
#if !defined(OWL_COMMDIAL_H)
# include <owl/commdial.h>
#endif


__OWL_BEGIN_NAMESPACE


#define WIN95_LONGFILENAME_MAX 260

#if defined(BI_PLAT_MSW)
# if !defined(SECTION)
      static _TCHAR serverNamePrefix[] = _T("\\\\");
      const int serverNamePrefixLen = sizeof serverNamePrefix / sizeof(_TCHAR) - 1;
//      static _TCHAR devTerminator[] = "\\/:";
      static _TCHAR ldevTerminatorCh = _T(':');
      static _TCHAR ldevTerminatorStr[] = _T(":");
#    if defined(UNIX)
      static _TCHAR dirSeparator[] = _T("/");
//      static _TCHAR dirSeparatorCh = _T('/');
      static _TCHAR dirSeparatorStr[] = _T("/");
#    else
      static _TCHAR dirSeparator[] = _T("\\/");
//      static _TCHAR dirSeparatorCh = _T('\\');
      static _TCHAR dirSeparatorStr[] = _T("\\");
#    endif //if UNIX
      static _TCHAR extSeparatorCh = _T('.');
      static _TCHAR extSeparatorStr[] = _T(".");
# else
#   if SECTION == 1
      _TCHAR serverNamePrefix[] = _T("\\\\");
      const int serverNamePrefixLen = sizeof serverNamePrefix / sizeof(_TCHAR) - 1;
      _TCHAR ldevTerminatorCh = _T(':');
      _TCHAR ldevTerminatorStr[] = _T(":");
      _TCHAR dirSeparator[] = _T("\\/");
      _TCHAR dirSeparatorStr[] = _T("\\");
      _TCHAR extSeparatorCh = _T('.');
      _TCHAR extSeparatorStr[] = _T(".");
#   else
      extern _TCHAR serverNamePrefix[];
      extern int  serverNamePrefixLen;
      extern _TCHAR ldevTerminatorCh;
      extern _TCHAR ldevTerminatorStr[];
      extern _TCHAR dirSeparator[];
      extern _TCHAR dirSeparatorStr[];
      extern _TCHAR extSeparatorCh;
      extern _TCHAR extSeparatorStr[];
#   endif
# endif
#endif

#if defined(SECTION) 
owl_string __formServer(const _TCHAR* serverName);
owl_string __formDevice(const _TCHAR* deviceName, bool isUNC);
owl_string __formPath(const _TCHAR* path);
owl_string __formFile(const _TCHAR* file);
owl_string __formExt(const _TCHAR* ext);
#endif

//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 1

//
// Some Win32 APIs that are available under Win16 via DOS calls
//

//
// overloaded functions to perform string scanning
//
static const _TCHAR* fn_strend(const _TCHAR* str)
{
  while (*str)
    str++;
  return str;
}

static const _TCHAR* fn_find(const _TCHAR* start, const _TCHAR* end, int match)
{
  while (start != end && *start != match)
    start++;
  return start;
}

static const _TCHAR* fn_find(const _TCHAR* start, const _TCHAR* end, const _TCHAR* set)
{
  const _TCHAR* setend = set + ::_tcslen(set);
  while (start != end && fn_find(set, setend, *start) == setend)
    start++;
  return start;
}

static const _TCHAR* fn_findl(const _TCHAR* start, const _TCHAR* end, int match)
{
  while (start != end && *start != match)
    start--;
  return start;
}

static const _TCHAR* fn_findl(const _TCHAR* start, const _TCHAR* end, const _TCHAR* set)
{
  const _TCHAR* setend = set + ::_tcslen(set);
  while (start != end && fn_find(set, setend, *start) == setend)
    start--;
  return start;
}

static const _TCHAR* fn_skipstr(const _TCHAR* start, const _TCHAR* end,
                               const _TCHAR* skip)
{
  while (start != end && *skip && *start == *skip)
    start++, skip++;

  return start;

/*
// TODO: Verify this does not break anything -jhh
  if ((start[0] == '\\') && (start[1] == '\\'))
    return &start[2];
  else
    return start;
//jhh  while (*skip && *start == *skip && start != end)
//jhh    start++, skip++;

//jhh
//jhh  return start;
*/
}

//-----
// Low level helper functions for parsing filename parts
//

//
//
//
#if !defined(SECTION) 
static 
#endif
owl_string __formServer(const _TCHAR* serverName)
{
  if (!serverName)
    return serverName;

  const _TCHAR* serverNameEnd = fn_strend(serverName);

  if (_tcsncmp(serverName, serverNamePrefix, serverNamePrefixLen) == 0)
    serverName += serverNamePrefixLen;               // skip "\\"

  const _TCHAR* ds = fn_find(serverName, serverNameEnd, dirSeparator);
  return owl_string(serverName, 0, (int)(ds - serverName));
}

//
//
//
#if !defined(SECTION) 
static 
#endif
owl_string __formDevice(const _TCHAR* deviceName, bool isUNC)
{
  if (!deviceName)
    return deviceName;

  const _TCHAR* deviceNameEnd = fn_strend(deviceName);


  if (isUNC)
    deviceNameEnd = fn_find(deviceName, deviceNameEnd, dirSeparator);
  else
    deviceNameEnd = fn_find(deviceName, deviceNameEnd, ldevTerminatorCh);

  return owl_string(deviceName, 0, (int)(deviceNameEnd - deviceName));
}

//
// Return a well formed path from any kind of filename part. A lone name
// is assumed to be just the path
//
#if !defined(SECTION) 
static 
#endif
owl_string __formPath(const _TCHAR* path)
{
  if (!path)
    return path;

  const _TCHAR* pathEnd = fn_strend(path);

  // Find the start of the path, skipping logical device or server+share
  //
  const _TCHAR* dvs = fn_find(path, pathEnd, ldevTerminatorCh);
  if (dvs < pathEnd) {
    path = dvs + 1;  // skip logical device
  }

  return owl_string(path, 0, (int)(pathEnd - path));
}

//#endif
//----------------------------------------------------------------------------
//#if !defined(SECTION) || SECTION == 2

//
// Return a well formed file from any kind of filename part. A lone name
// is assumed to be just the file
//
//   "dir.x\file.ext"  -> "file"
//   "dir.x\"          -> ""
//   "file"            -> "file"
//
#if !defined(SECTION) 
static 
#endif
owl_string __formFile(const _TCHAR* file)
{
  if (!file)
    return file;

  const _TCHAR* fileEnd = fn_strend(file);

  const _TCHAR* ds = fn_findl(fileEnd-1, file-1, dirSeparator);
  if (ds >= file)
    file = ds + 1;

  const _TCHAR* es = fn_findl(fileEnd-1, file-1, extSeparatorCh);
  if (es >= file)
    fileEnd = es;

  return owl_string(file, 0, (int)(fileEnd - file));
}

//
// Return a well formed extension from any kind of filename part. A lone name
// is assumed to be just the extension and a '.' is prepended
//
//   "dir.x\file.ext"  -> ".ext"
//   "dir.x\"          -> "."
//   "ext"             -> ".ext"
//
#if !defined(SECTION) 
static 
#endif
owl_string __formExt(const _TCHAR* ext)
{
  if (!ext)
    return ext;  // or extSeparatorCh?

  const _TCHAR* extEnd = fn_strend(ext);

  const _TCHAR* ds = fn_findl(extEnd-1, ext-1, dirSeparator);
  if (ds >= ext)
    ext = ds + 1;

  const _TCHAR* es = fn_findl(extEnd-1, ext-1, extSeparatorCh);
  if (es >= ext)
    return es;

  return owl_string(extSeparatorStr) + ext;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2

//
//
//
TFileName::TFileName()
{
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 3

//
//
//
TFileName::TFileName(const TFileName& src)
:
  Unc(src.Unc),
  ServerStr(src.ServerStr),
  DeviceStr(src.DeviceStr),
  PathStr(src.PathStr),
  FileStr(src.FileStr),
  ExtStr(src.ExtStr)
{
  FormCanonicalName();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 4

//
// Construct a filename in the form:
//
//   filename := [[servername]devicename][path][file[ext]]
//
//  where:
//
//   servername := \\multiname\         //
//   devicename := name(: | \ | :\)
//   sharename := name
//   logicaldevice := name
//
//   path := multiname\[...]
//   file := multiname
//   ext := [.[name]]
//
//   name := (filecharset)[...]
//   multiname := (filecharset | .)[...]
//
TFileName::TFileName(const _TCHAR* serverName, const _TCHAR* deviceName,
                     const _TCHAR* path, const _TCHAR* file,
                     const _TCHAR* ext)
{
  ServerStr = __formServer(serverName);

  Unc = serverName != 0;

  DeviceStr = __formDevice(deviceName, Unc);
  PathStr += __formPath(path);
  FileStr += __formFile(file);
  ExtStr += __formExt(ext);
  FormCanonicalName();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 1

//
//
//
void
TFileName::Parse(const _TCHAR* freeform, bool pathOnly)
{
  const _TCHAR* nextp;
  const _TCHAR* freeformEnd = fn_strend(freeform);

  if (*freeform == '\0')
    {
    Unc = false;
    ServerStr = _T("");
    DeviceStr = _T("");
    ExtStr = _T("");
    PathStr = _T("");
    FileStr = _T("");
    FormCanonicalName();
    return;
    }
  // Parse from the left, the server name and device if UNC "\\", or the
  // logical device name if one.
  //
  nextp = fn_skipstr(freeform, freeformEnd, serverNamePrefix);
  if (nextp >= freeform+serverNamePrefixLen) {
    Unc = true;
    freeform = nextp;               // skip "\\"
    nextp = fn_find(freeform, freeformEnd, dirSeparator);
    ServerStr = owl_string(freeform, 0, (int)(nextp-freeform));
    freeform = nextp + 1;               // skip past "\"

    nextp = fn_find(freeform, freeformEnd, dirSeparator);
    DeviceStr = owl_string(freeform, 0, (int)(nextp-freeform));
    freeform = nextp + 1;               // skip past "\"
  }
  else {
    Unc = false;
    ServerStr = _T("");

    nextp = fn_find(freeform, freeformEnd, ldevTerminatorCh);
    if (nextp != freeformEnd) {
      DeviceStr = owl_string(freeform, 0, (int)(nextp-freeform));
      freeform = nextp + 1;               // skip past "\/:"
    }
    else
      DeviceStr = _T("");
  }
  
  if (!pathOnly){
    // Parse the extension and filename from the right.
    //
    nextp = fn_findl(freeformEnd-1, freeform-1, extSeparatorCh);
  //DLN freeform is just before first char in string, so boundschecker will
  //    report an erroneous error here
    const _TCHAR* dirp = fn_findl(freeformEnd-1, freeform-1, dirSeparator);

    if (nextp > dirp) {
      ExtStr = owl_string(nextp, 0, (int)(freeformEnd-nextp));
      freeformEnd = nextp;
    }
    else
      ExtStr = _T("");

    FileStr = owl_string(dirp+1, 0, (int)(freeformEnd - (dirp+1)));
    freeformEnd = dirp;
  }
  else{
    FileStr = _T("");
    ExtStr = _T("");
  }

  // What is left is the path
  //
  if (freeformEnd > freeform)
    PathStr = owl_string(freeform, 0, (int)(freeformEnd-freeform));
  else
    PathStr = _T("");
  FormCanonicalName();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 5

//
// Construct a filename given the full path in any form
//
TFileName::TFileName(const _TCHAR* freeform, bool pathOnly)
{
  Parse(freeform,pathOnly);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 6

//
// Construct a filename given the full path in any form
//
TFileName::TFileName(const owl_string& freeform, bool pathOnly)
{
  Parse(freeform.c_str(),pathOnly);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 7

//
// Construct a special filename
//
TFileName::TFileName(TFileName::TSpecialType type)
{
  switch (type) {
    case TempFile:
    case TempDir: {
      int tmpLen = ::GetTempPath(0,0);
      TTmpBuffer<TCHAR> __clnObj(tmpLen);
      TCHAR* tmpPath = __clnObj;
      ::GetTempPath(tmpLen, tmpPath);
      if (type == TempDir) {
        //_tcscat(tmpPath,dirSeparatorStr); // !BUG: Writes past EOB. Also, path already has trailing backslash. [VH]
        Parse(tmpPath,true);
      }
      else {
        TTmpBuffer<TCHAR> tmpFile(WIN95_LONGFILENAME_MAX);
        ::GetTempFileName((TCHAR*)tmpPath, _T("TFN"), 0, tmpFile);
        Parse((TCHAR*)tmpFile);
      }
    }
    break;
    case CurrentDir: {
      TTmpBuffer<TCHAR> buffer(WIN95_LONGFILENAME_MAX);
      ::GetCurrentDirectory(WIN95_LONGFILENAME_MAX, (TCHAR*)buffer);
      _tcscat((TCHAR*)buffer, dirSeparatorStr);
      Parse((TCHAR*)buffer,true);
    }
    break;
    case HomeDir: {
      TTmpBuffer<TCHAR> buffer(WIN95_LONGFILENAME_MAX);
      ::GetWindowsDirectory((TCHAR*)buffer, WIN95_LONGFILENAME_MAX);
      _tcscat((TCHAR*)buffer,dirSeparatorStr);
      Parse((TCHAR*)buffer,true);
    }
    break;
    case BootDir:{
        LPCTSTR path = _tgetenv(_T("wonbootdir"));
        if(path)
          Parse(path,true);
        break;
      }
    case MachineDir:
      //!Y.B ??????????????
    case SharedDir:
      //!Y.B ??????????????
    break;
    case SysDir: {
      TCHAR tmp[WIN95_LONGFILENAME_MAX];
      ::GetSystemDirectory(tmp, WIN95_LONGFILENAME_MAX);
      _tcscat(tmp,dirSeparatorStr);
      Parse(tmp,true);
    }
    break;
    case ComputerName: {
      TCHAR tmp[WIN95_LONGFILENAME_MAX];
      DWORD size = WIN95_LONGFILENAME_MAX;
      ::GetComputerName(tmp, &size);

      Parse(tmp);
    }
    break;
  }
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 8

//
//
//
TFileName&
TFileName::operator =(const TFileName& src)
{
  Unc = src.Unc;
  ServerStr = src.ServerStr;
  DeviceStr = src.DeviceStr;
  PathStr = src.PathStr;
  FileStr = src.FileStr;
  ExtStr = src.ExtStr;
  FormCanonicalName();
  return *this;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 9

//
//
//


// JJH this function is not used
//static void
//ForceFullPath(string& canonical)
//{
//  if (canonical != "")
//    {
//    _TCHAR buffer[WIN95_LONGFILENAME_MAX];
//    ::GetFullPathName(canonical.c_str(), WIN95_LONGFILENAME_MAX, buffer, 0);
//    canonical = buffer;
//    }
//}


_TCHAR* TFileName::GetFullFileName()
{
  return (_TCHAR*)CanonicalStr();
}

//
// Reassemble the filename parts into a canonical form & expand any relative
// dirs
//

//string TFileName::Canonical(bool forceUNC) const
void TFileName::FormCanonicalName()
{
//  if (forceUNC)
//    {
//    strcpy(NameBuffer,serverNamePrefix);
//    strcat(NameBuffer,DeviceStr);
//    strcat(NameBuffer, dirSeparatorStr);
//    strcat(NameBuffer,PathStr);
//    strcat(NameBuffer,dirSeparatorStr);
//    strcat(NameBuffer,FileStr);
//    strcat(NameBuffer,ExtStr);
//  else
  GetParts(Server|Device|Path|File|Ext); //fills NameBuffer

  if (_tcscmp(NameBuffer,_T(""))) //if file name is not blank
    {                        //  use it to get official file name
    _TCHAR buffer[WIN95_LONGFILENAME_MAX];
    ::GetFullPathName(NameBuffer, WIN95_LONGFILENAME_MAX, buffer, 0);
    FullCanonicalName = buffer;
    }
  else
    FullCanonicalName = NameBuffer;
//  return NameBuffer;
}

const owl_string& TFileName::Canonical(bool) const
{
  return FullCanonicalName;
}

_TCHAR* TFileName::GetNameNoExt()
{
  return (_TCHAR*)GetParts(File);
}

_TCHAR* TFileName::GetNameAndExt() const
{
  return (_TCHAR*)GetParts(File|Ext);  
}

_TCHAR* TFileName::GetFullFileNameNoExt()
{
  return (_TCHAR*)GetParts(Server|Device|Path|File); //fills NameBuffer
}


#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 10

//
//
//
const _TCHAR*
TFileName::ShortName(bool forceUNC) const
{
  _TCHAR tmp[WIN95_LONGFILENAME_MAX];
  ::GetShortPathName(CanonicalStr(forceUNC), tmp, WIN95_LONGFILENAME_MAX);

  _tcscpy(NameBuffer,tmp);
  return (_TCHAR*)NameBuffer;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 11

//
// Human readable form of the filename appropriate for use in titles
//
const _TCHAR* TFileName::Title() const
{
  owl_string canonical = Canonical();
  _TCHAR tmp[WIN95_LONGFILENAME_MAX];
  TCommDlg::GetFileTitle(canonical.c_str(), tmp, sizeof tmp);
  _tcscpy(NameBuffer,tmp);
  return (_TCHAR*)NameBuffer;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 12

//
// input  filemame.ext
// output file...
//
static owl_string& trimmStr(owl_string& s, uint len)
{
  if(s.length() < len)
    return s;
#if !defined(BI_STD_STRING)
  s.remove(len-3); 
#else
  s.erase(len-3);
#endif
  s += _T("...");
  return s;
}

//
// input  ProgrammFiles\Common File\New Files\Short
// output ProgrammFiles\...\Short
//
static owl_string& trimPath(owl_string& s, uint len)
{
  if(s.length() < len)
    return s;

  _TCHAR* addPath = _T("\\...");
  // find ?
  // 1. get last path  path1\path2\path3 -> \path3
  owl_string subpath = s;
  size_t pos = subpath.find_last_of(dirSeparatorStr);
  if(pos == NPOS || len <= 5){
    s = addPath;
    return s;
  }
    
  owl_string lastpath = subpath.substr(pos);
#if !defined(BI_STD_STRING)
  subpath.remove(pos); // path1\path2
#else
  subpath.erase(pos);
#endif

  // calculate minimum path length include "\\..."
  int length = len-4-lastpath.length();
  while(length < 0 && lastpath.length() > 0){
      lastpath = lastpath.substr(1); //Remove the first character
      length = len-4-lastpath.length();
  }
  uint iteration = 0;

  // while remainig string greater then our length
  while(static_cast<int>(subpath.length()) > length && subpath.length() > 0){
    size_t pos = subpath.find_last_of(dirSeparatorStr);
    if(pos != NPOS)
#if !defined(BI_STD_STRING)
      subpath.remove(pos); // path1\path2
#else
      subpath.erase(pos);
#endif
    else
      subpath = _T("");

    iteration++;
  }

  // if was only one path1 -> lastpath => path1, and subpath => 0
  if(iteration){
    s = subpath;
    s += addPath;
    s += lastpath;
  }
  else if(s.length() > len){
    s = subpath;
    s += addPath;
  }

  return s;
}
//
static owl_string getAttempt(const TFileName& This, const owl_string& name, 
                              int index)
{
  uint parts[] = {
    TFileName::Server | TFileName::Device | TFileName::Path,  // Whole thing
    TFileName::Server | TFileName::Device,
    TFileName::File | TFileName::Ext,
  };
  owl_string attempt = This.GetParts(parts[index]);
#if !defined(BI_STD_STRING)
  if(!attempt.is_null()){
#else
  if(!attempt.empty()){
#endif
    _TCHAR lastCh = attempt.c_str()[attempt.length()-1];
    if(lastCh != _T('\\') && lastCh != _T('/'))
      attempt += dirSeparatorStr;
  }
  attempt += name;
  return attempt;
}
//
// Human readable form of the filename squeezed to fit in 'maxLen' chars
//
// lpszCanon = C:\MYAPP\DEBUGS\C\TESWIN.C
//
// maxLen   b   Result
// ------   -   ---------
//  1-3     F   <empty>
//  4       F   T...
//  5       F   TE...
//  6       F   TES...
//  7       F   TESW...
//  4- 7    T   TESWIN.C
//  8-14    x   TESWIN.C
// 15-16    x   C:\...\TESWIN.C
// 17-23    x   C:\MYAPP\...\TESWIN.C
// 24-25    x   C:\MYAPP\...\C\TESWIN.C
// 26+      x   C:\MYAPP\DEBUGS\C\TESWIN.C
const _TCHAR* TFileName::Squeezed(int maxLen, bool keepName) const
{
  if (maxLen <= 3)
    return _T("");

  const uint n = static_cast<uint>(maxLen);
  owl_string fname = Title();
  owl_string attempt = getAttempt(*this, fname, 0); // full path

  // If maxLen is not enough to hold the full path name, then trim.
  // If maxLen is enough to hold at least the basename, and enough 
  // to hold at least server/device and "...\", trim the path,
  // otherwise trim (or keep) only the basename.

  if (n < attempt.length())
  {
    const owl_string dname = getAttempt(*this, fname, 1); // server-device/name
    if (n > dname.length() + 3)
    {
      owl_string p = PathStr;
      trimPath(p, n - dname.length()); 
      if (p.length() > 0 && p[p.length() - 1] != dirSeparatorStr[0])
        p += dirSeparatorStr;
      attempt = GetParts(Server | Device) + p + fname;
    }
    else 
    {
      attempt = keepName ? fname : trimmStr(fname, n);
    }
  }

  CHECK(attempt.length() < _MAX_PATH - 1);
  _tcscpy(NameBuffer, attempt.c_str());
  return NameBuffer;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 13


//
//
//
bool
TFileName::IsValid() const
{
  OFSTRUCT ofs;
  ofs.cBytes = sizeof ofs;
  _USES_CONVERSION; 
  return ::OpenFile(_W2A(CanonicalStr()), &ofs, OF_PARSE) != -1;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 14

//
//
//
bool
TFileName::Exists() const
{
#if 0
  OFSTRUCT ofs;
  ofs.cBytes = sizeof ofs;
  _USES_CONVERSION; 
  return ::OpenFile(_W2A(Canonical().c_str()), &ofs, OF_EXIST) != -1;
#else
  HANDLE h;
  {
    WIN32_FIND_DATA data;
    h = ::FindFirstFile(CanonicalStr(), &data);
    ::FindClose(h);
  }
  return (h != INVALID_HANDLE_VALUE);
#endif
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 15

//
// Reassemble any logical subset of filename parts
//
const _TCHAR*
TFileName::GetParts(uint p) const
{
  *NameBuffer = 0; //JRB This crashes GWANGI


  if (Unc) 
  {
    if (p & Server) 
    {
      _tcscat(NameBuffer,serverNamePrefix);
      _tcscat(NameBuffer,ServerStr.c_str());
      if (p & Device)
        _tcscat(NameBuffer,dirSeparatorStr);
    }
    if ((p & Device) && DeviceStr.length() > 0) {
      _tcscat(NameBuffer, DeviceStr.c_str());
      if ((p & Path) || (p & File))
        _tcscat(NameBuffer, dirSeparatorStr);
    }
  }
  else 
  {
    if ((p & Device) && DeviceStr.length() > 0) {
      _tcscat(NameBuffer, DeviceStr.c_str());
      _tcscat(NameBuffer, ldevTerminatorStr);
      if ((p & Path) && PathStr.length() == 0) {
        _tcscat(NameBuffer, dirSeparatorStr);
      }
    }
  }

  if ((p & Path) && PathStr.length() > 0)
  {
    _tcscat(NameBuffer,PathStr.c_str());
    _tcscat(NameBuffer, dirSeparatorStr);
  }

  if ((p & File) && FileStr.length() > 0)
    _tcscat(NameBuffer, FileStr.c_str());

  if ((p & Ext) && ExtStr.length() > 0)
    _tcscat(NameBuffer, ExtStr.c_str());
  return (_TCHAR*)NameBuffer;

}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 16

//
//
//
bool
TFileName::HasParts(uint p) const
{
  bool has = false;
  if (p & Server)
    has |= (ServerStr.length() > 0);
  if (p & Device)
    has |= (DeviceStr.length() > 0);
  if (p & Path)
    has |= (PathStr.length() > 0);
  if (p & File)
    has |= (FileStr.length() > 0);
  if (p & Ext)
    has |= (ExtStr.length() > 0);
  return has;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 17

//
//
//
void
TFileName::SetPart(uint p, const owl_string& partStr)
{
  if (p & Server)
    ServerStr = partStr;
  if (p & Device)
    DeviceStr = partStr;
  if (p & Path)
    PathStr = partStr;
  if (p & File)
    FileStr = partStr;
  if (p & Ext)
    ExtStr = partStr;
  FormCanonicalName();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 18

//
//
//
void
TFileName::MergeParts(uint p, const TFileName& source)
{
  // Walk thru bits & SetPart(p) from source.GetParts(p)
  //
  if (p & Server)
    ServerStr = source.ServerStr;//source.GetParts(Server);
  if (p & Device)
    DeviceStr = source.DeviceStr;//source.GetParts(Device);
  if (p & Path)
    PathStr = source.PathStr;//source.GetParts(Path);
  if (p & File)
    FileStr = source.FileStr;//source.GetParts(File);
  if (p & Ext)
    ExtStr = source.ExtStr;//source.GetParts(Ext);
    
  FormCanonicalName(); // BUGFIX: FullCanonicalName wasn't updated! (VH, 2005-07-26)
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 19

//
//
//
void
TFileName::StripParts(uint p)
{
  if (p & Server)
    ServerStr = _T("");
  if (p & Device)
    DeviceStr = _T("");
  if (p & Path)
    PathStr = _T("");
  if (p & File)
    FileStr = _T("");
  if (p & Ext)
    ExtStr = _T("");

  FormCanonicalName(); // BUGFIX: FullCanonicalName wasn't updated! (VH, 2005-07-26)
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 20

//
//
//
bool
TFileName::Remove() const
{
  if (!::DeleteFile(CanonicalStr()))
    if(!::RemoveDirectory(CanonicalStr()))
      return false;
  return true;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 21

//
//
bool 
TFileName::Move(const TFileName& newName, uint32 how)
{
  //
  // Contrary to the help\MSDN MoveFileEx fails under Win95. (as of 2/97)
  //
  bool retval;
  if( !TSystem::IsWin95() )
    retval = ::MoveFileEx(CanonicalStr(), newName.CanonicalStr(), how);
  else
    retval = ::MoveFile(CanonicalStr(), newName.CanonicalStr());

  if(retval){
    *this = newName;
    return true;
  }

  return false;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 22

//
//
//
bool
TFileName::Copy(const TFileName& newName, bool failIfExists) const
{
  return ::CopyFile(CanonicalStr(), newName.CanonicalStr(), 
                    failIfExists) == TRUE;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 23

//
//
//
int
TFileName::CreateDir() const
{
  return ::CreateDirectory(CanonicalStr(), 0) == TRUE;  //LPSECURITY_ATTRIBUTES
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 24

//
//
//
int
TFileName::ChangeDir() const
{
  return ::SetCurrentDirectory(CanonicalStr()) == TRUE;
}

#endif
//----------------------------------------------------------------------------

#if !defined(SECTION) || SECTION == 25
//<Jogy
bool
TFileName::AddSubDir(LPCTSTR subdir)
{
  if (_tcschr(dirSeparator, subdir[0]) == 0) // The subdir does not begin with the separator
    PathStr += dirSeparatorStr;

  PathStr += subdir;

  if (_tcschr(dirSeparator, PathStr[PathStr.length() - 1]) != 0)  // The resultring string ends with the separator
    PathStr = PathStr.substr(0, PathStr.length() - 1);

  FormCanonicalName();

  return true;
}

bool
TFileName::AddSubDir(const owl_string& subdir)
{
  return AddSubDir(subdir.c_str());
}
#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 26 || SECTION == 27 || SECTION == 28 || SECTION == 29

///////////////////////
// struct __TFindFileStr
// ~~~~~~ ~~~~~~~~~~~~~~
//
struct __TFindFileStr{
  HANDLE          Handle;
  WIN32_FIND_DATA Data;
  owl_string Direct;
};

TTime __MakeTTime(uint16 fdt, uint16 ftm);
void __MakeDosTime(const TTime& ctm, uint16& fdm, uint16& ftm);
uint __sys2Atr(uint32 attrib);
uint __attr2Sys( uint attrib);
#if !defined(SECTION)
static
#endif
void __FillFileStr( __TFindFileStr& data, TFileStatus& status);


#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 26

#if !defined(SECTION)
static
#endif
void __FillFileStr( __TFindFileStr& data, TFileStatus& status)
{
  ::_tcscpy(status.fullName, data.Data.cFileName);
  status.attribute  = __sys2Atr(data.Data.dwFileAttributes);
  status.createTime = TTime(TFileTime(data.Data.ftCreationTime));
  status.modifyTime = TTime(TFileTime(data.Data.ftLastWriteTime));
  status.accessTime = TTime(TFileTime(data.Data.ftLastAccessTime));
  status.size       = data.Data.nFileSizeLow;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 27

#if !defined(OWL5_COMPAT)

//
int TFileName::GetStatus(TFileStatus& status) const
{
  owl_string fileName = Canonical();
  __TFindFileStr findData;
  findData.Handle = ::FindFirstFile(fileName.c_str(), &findData.Data);
  if(findData.Handle == INVALID_HANDLE_VALUE)
    return false;

  __FillFileStr(findData, status);
  ::_tcscpy(status.fullName, fileName.c_str());


  ::FindClose(findData.Handle);

  return true;
}

#endif //#if defined(OWL5_COMPAT)

//

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 28

#if !defined(OWL5_COMPAT)
//
int TFileName::SetStatus(const TFileStatus& status)
{
  owl_string fileName = Canonical();
  {
    TFile File(fileName.c_str(), TFile::WriteOnly|TFile::PermExclusive|TFile::OpenExisting);
    if(!File.IsOpen())
      return false;
    if(!File.Length(status.size))
      return false;
    TFileTime ftCreation(status.createTime);
    TFileTime ftLastAccess(status.accessTime);
    TFileTime ftLastWrite(status.modifyTime);

    // will work only for TDiskFileHandle
    TDiskFileHandle* handle = TYPESAFE_DOWNCAST(File.GetHandle(),TDiskFileHandle);
    if(!::SetFileTime(handle->Handle, &ftCreation, &ftLastAccess, &ftLastWrite))
      return false;
  }
  uint attr = __attr2Sys(status.attribute);
  if(!::SetFileAttributes(fileName.c_str(), attr))
  {
# pragma warn -ccc
    WARN(1, "TFile::SetStatus() error ::SetFileAttributes(), line: " << __LINE__);
# pragma warn .ccc
    return false;
  }

  return true;
}
#endif //#if !defined(OWL5_COMPAT)

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 29
//
//
//
TFileNameIterator::TFileNameIterator(const owl_string& wildName)
:
  Done(false)
{
  __TFindFileStr* data = new __TFindFileStr;
  Data = data;
  _TCHAR* p = (_TCHAR*)_tcsrchr(wildName.c_str(),_T('\\'));
  if(p){
    _TCHAR c = *p;
    *p = _T('\0');
    data->Direct = owl_string(wildName.c_str()) + _T("\\");
    *p = c;
  }

  data->Handle = ::FindFirstFile(wildName.c_str(), &data->Data);
  if(data->Handle != INVALID_HANDLE_VALUE){
    __FillFileStr( *data, Status);
  }
  else
    Done = true;
}
//------------------------------------------------------------------------------
TFileNameIterator::~TFileNameIterator()
{
  if (Data && ((__TFindFileStr*)Data)->Handle != INVALID_HANDLE_VALUE)
    ::FindClose(((__TFindFileStr*)Data)->Handle);

  delete (__TFindFileStr*)Data;
}
//------------------------------------------------------------------------------
bool TFileNameIterator::FindNext(TFileStatus& status, void* findData)
{
  __TFindFileStr& data = *(__TFindFileStr*)findData;
  if (!::FindNextFile(data.Handle, &data.Data))
    return false;

  __FillFileStr(data, status);
  return true;
} 
#endif

//==============================================================================

__OWL_END_NAMESPACE
