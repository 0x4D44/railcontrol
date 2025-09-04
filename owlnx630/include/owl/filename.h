//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
//----------------------------------------------------------------------------
#if !defined(OWL_FILENAME_H)
#define OWL_FILENAME_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif

#ifndef NO_OWL

#if !defined(OWL_FILE_H)
# include <owl/file.h>
#endif

#endif

#include <owl/private/dir.h>  // struct ffblk
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

// Copied from old owl (04-16-01 JAM)
#ifdef UNIX
#include <owl/system.h> 
#endif

//#include <string>
//using std::string;

struct TFileStatus;

//
// class TFileName
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TFileName {
  protected:
    void   Parse(LPCTSTR freeform, bool pathOnly = false);

    owl_string ServerStr;
    owl_string DeviceStr;
    mutable owl_string PathStr; //had to do this to make Squeezed function const
    owl_string FileStr;
    owl_string ExtStr;

    owl_string FullCanonicalName;
    mutable _TCHAR NameBuffer[_MAX_PATH];
    bool   Unc;

  public:
    // Construct an empty filename
    //
    TFileName();

    // Construct a filename from its parts. OK to pass 0 to skip part
    //
    TFileName(LPCTSTR serverName, LPCTSTR shareOrDeviceName, LPCTSTR path,
              LPCTSTR file, LPCTSTR ext);

    // Construct a filename from a freeform string
    //
    TFileName(LPCTSTR  freeform, bool pathOnly = false);
    TFileName(const owl_string& freeform, bool pathOnly = false);

    // Construct a filename from another filename
    //
    TFileName(const TFileName& src);

    //virtual ~TFileName(){} ??

    // Assign new filename or freeform to this name
    //
    TFileName& operator =(const TFileName& src);
    TFileName& operator =(const owl_string& src);
    TFileName& operator =(LPCTSTR src);

    // Construct a filename representing a special file or directory
    //
    enum TSpecialType {
      TempFile,     // A temporary filename
      TempDir,      // Location of temporary files
      CurrentDir,   // Current working directory if any
      BootDir,      // Root dir of boot device (LDID_BOOT)
      MachineDir,   // (LDID_MACHINE)
      HomeDir,      // Home directory for OS (LDID_WIN)
      SharedDir,    // Shared home directory for OS (LDID_SHARED)
      SysDir,       // Location of system files (LDID_SYS)
      ComputerName,
    };
    TFileName(TSpecialType type);

    // Normal fully qualified path string, & short filename version of it
    // Convert a possibly logical drive based name to a UNC name if indicated
    //
        //PERL API USE ONLY
    _TCHAR* GetNameNoExt();
    _TCHAR* GetNameAndExt() const;
    _TCHAR* GetFullFileName();
    _TCHAR* GetFullFileNameNoExt();

    void FormCanonicalName();
    const owl_string& Canonical(bool forceUNC = false) const;
    const _TCHAR* CanonicalStr(bool forceUNC = false) const;
    const _TCHAR* ShortName(bool forceUNC = false) const;

    // Nice, human readable rendering of the filename using system conventions
    // (Returns null string for directory names or malformed names.  For
    // directories, try Canonical() instead.)
    //
    const _TCHAR* Title() const;

    // Human readable form of the filename squeezed to fit in 'maxLen' chars
    //
    const _TCHAR* Squeezed(int maxLen, bool keepName = true) const;

    // Obtain any combination of various filename parts. Seperators inserted
    // only as appropriate
    //
    enum TPart {
      Server = 1,  // Server name
      Device = 2,  // Logical device or sharename
      Path   = 4,  // Directory path to the file
      File   = 8,  // Filename part without the extension
      Ext    =16,  // Extension
    };
    const _TCHAR* GetParts(uint p) const;      // Return assembled string of parts
    bool   HasParts(uint p) const;      // Does name have an given part
    void   SetPart(uint p, const owl_string& partStr);    // Modify sub part
    void   MergeParts(uint p, const TFileName& source); // Modify sub parts
    void   StripParts(uint p);         // Remove indicated parts

    // Information about the filename
    //
    bool IsValid() const;       // Is a valid name of any kind
    bool IsUNC() const;         // Is UNC vs logical drive based name
    bool Exists() const;        // Does device/dir/file exist?

    // Is another filename equivalent to this filename?
    //
    bool operator ==(const TFileName& other) const;

    // Remove the file or dir associated with this filename.
    //
    bool Remove() const;

    // Move (rename) the file associated with this filename, and change this
    // name to the new name
    //
    enum { ReplaceExisting=1, CopyAllowed=2, DelayUntilReboot=4 };
    bool Move(const TFileName& newName, uint32 how = CopyAllowed);

    // Copy the file associated with this filename to a new file
    //
    bool Copy(const TFileName& newName, bool failIfExists) const;

    int ChangeDir() const;
    int CreateDir() const;

    //<Jogy
    bool AddSubDir(LPCTSTR subdir);
    bool AddSubDir(const owl_string& subdir);
    //Jogy>

    // Get and Set the file status struct for the item associated with this
    // filename
    //
    int GetStatus(TFileStatus& status) const;
    int SetStatus(const TFileStatus& status);

    // Other ideas...
    //
    static LPCTSTR WildName() {return _T("*");}       // General wildstring
    static LPCTSTR WildPart(uint /*p*/) {return _T("*");} // Part specific?

    // ***********************************************************************************    
    // (JAM 03-12-01) Added GetNameAndExt for compliance with old code

};

//
// class TFileNameIterator
// ~~~~~ ~~~~~~~~~~~~~~~~~
class _OWLCLASS TFileNameIterator {
  public:
    TFileNameIterator(const owl_string& wildName);
    virtual ~TFileNameIterator();

    operator const TFileStatus&() const;
    const TFileStatus& operator *() const;
    operator const _TCHAR*() const;
    const TFileStatus& operator ++();
    const TFileStatus& Current() const;

    bool IsDir() const;
    bool IsSysDir() const;//true if it is the "." or ".." system directory; 
    unsigned long  GetAttributes() const { return Status.attribute; }
  protected:
    TFileNameIterator(){}
    // all work do here -> must fill TFileStatus structure return true on success
    virtual bool FindNext(TFileStatus& status, void* findData);

  protected:
    bool            Done;
    void*           Data;
#ifndef NO_OWL
    TFileStatus     Status;
#endif

};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inlines
//

inline TFileName& TFileName::operator =(const owl_string& src)
{
  Parse(src.c_str());
  return *this;
}

inline TFileName& TFileName::operator =(LPCTSTR src)
{
  Parse(src);
  return *this;
}

inline bool TFileName::operator ==(const TFileName& other) const
{
  return _tcsicmp(CanonicalStr(), other.CanonicalStr()) == 0;
}

inline bool TFileName::IsUNC() const
{
  return Unc;
}

inline const _TCHAR* TFileName::CanonicalStr(bool forceUNC) const
{
  return Canonical(forceUNC).c_str();
}


#ifndef NO_OWL

#if defined(OWL5_COMPAT)

  inline int TFileName::GetStatus(TFileStatus& status) const
  {
    return TFile::GetStatus(CanonicalStr(), status);
  }
  
  inline int TFileName::SetStatus(const TFileStatus& status)
  {
    return TFile::SetStatus(CanonicalStr(), status);
  }
#endif // #if defined(OWL5_COMPAT)
  
// filename
inline TFileNameIterator::operator const TFileStatus&() const 
{
  return Status;
}
inline TFileNameIterator::operator const TCHAR*() const 
{
  return Done ? 0 : Status.fullName;
}
inline const TFileStatus& TFileNameIterator::operator *()const
{
  return Status;
}
inline const TFileStatus& TFileNameIterator::Current() const
{
  return Status;
}
inline const TFileStatus& TFileNameIterator::operator ++() 
{
  if(!FindNext(Status, Data))
    Done = true;
  return Status;
}

#endif // #ifndef NO_OWL

inline bool TFileNameIterator::IsDir() const
{ 
  return Status.attribute & FILE_ATTRIBUTE_DIRECTORY; 
}

//Returns true if it is a directory & the directory is "." or ".."; 
inline bool TFileNameIterator::IsSysDir() const
{
  LPCTSTR p = Status.fullName;
  return IsDir() && p[0]==_T('.') &&
          (p[1]==_T('\0') || p[1]==_T('.') && p[2]==_T('\0'));
}    

__OWL_END_NAMESPACE


#endif  // OWL_FILENAME_H
