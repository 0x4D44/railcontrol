//----------------------------------------------------------------------------
// ObjectWindwos
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Fully rewritten by Yura Bidus
//----------------------------------------------------------------------------

#if !defined(OWL_FILE_H)
#define OWL_FILE_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(OWL_TIME_H)
# include <owl/time.h>
#endif
#if !defined(OWL_DATE_H)
# include <owl/date.h>
#endif
#if defined(BI_HAS_HREADWRITE) && !defined(OWL_PRIVATE_WSYSINC_H)
# include <owl/private/wsysinc.h>
#endif
#ifndef OWL_GDIBASE_H
#include <owl/gdibase.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WINELIB
//#include <msvcrt/share.h>
//#include <msvcrt/dos.h>
#include <sys/io.h>

#else

#if !defined(__CYGWIN__)
#include <share.h>
#include <dos.h>
#endif
#include <io.h>

#endif //WINELIB

#if defined(BI_COMP_MSC) 
# include <sys/locking.h>
#endif
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif
#if !defined(BI_STD_IO)
#  include <fstream.h>
#endif

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

//------------------------------------------------------------------------------
//
// struct TFileStatus
// ~~~~~~ ~~~~~~~~~~~
// 
struct TFileStatus {
    TTime   createTime;
    TTime   modifyTime;
    TTime   accessTime;
    uint32  size;
    uint    attribute;
    TCHAR   fullName[_MAX_PATH];
};

_OWLCFUNC(_tostream &)  operator << (_tostream &, const TFileStatus &);

//
enum TByteOrderType {
  boLittle_Endian,  // LSB at lowest address: Intel    //
  boBig_Endian,     // MSB at lowest address: Motorola //
};

//
//
//
inline TByteOrderType EndianType()
{
  uint16 w=0x0001;
  uint8 *b=(uint8*)&w;
  return (b[0] ? boLittle_Endian : boBig_Endian);
}

//------------------------------------------------------------------------------
//  DESCRIPTION                                                               
//  These identify host program variable types and let the binary             
//  I/O package read and write routines know how to store or retrieve bytes   
//  from host buffer arrays and when to sign-extend.                          
// 
enum TVarType{
  varEnd,
  varPOINTER,
  varCHAR,
  varUCHAR,
  varSHORT,
  varLONG,
  varINT8,
  varINT16,
  varINT32,
  varUSHORT,
  varULONG,
  varUINT8,
  varUINT16,
  varUINT32,
  varFLOAT,
  varDOUBLE,
  varINT64,
  varUINT64,
  varLastMember,
};
#define BINNTYPE  (varLastMember+1)


//----------------------------------------------------------------------------
//  TBinField  -  Binary I/O Structure Field Description                       
//                                                                            
//  DESCRIPTION                                                               
//  The TBinField struct describes a group of like-typed fields in a          
//  structure to be read or written using the Binary I/O Struct calls.        
//
struct TBinField {
  TVarType   Type;    // Declared type of struct field  /
  int       Bytes;  // # of bytes occupied in file    /
  int       Count;  // # of repeatitions of this item /
};


#define TFILE_ERROR uint32(-1)
#define FileNull 0
#define TFILE64_ERROR uint64(0xFFFFFFFF)

class _OWLCLASS TFileHandle; 

//------------------------------------------------------------------------------
//
// class TFile
// ~~~~~ ~~~~~
//
class _OWLCLASS TFile {
  public:
    enum seek_dir {
      beg = 0,
      cur = 1,
      end = 2
    };

    // Open mode -> remapped into OS specific value internally
    enum OpenMode{
      ReadOnly      =   0x0001,
      WriteOnly     =   0x0002,
      ReadWrite     =   WriteOnly|ReadOnly,

      PermWrite     =   0x0010, //
      PermRead      =   0x0020, //
      PermReadWrite =   PermWrite|PermRead, //
      PermExclusive =   0x0040, //
      PermNone      =   0x0080, //

      CreateNew     =   0x0100,
      CreateAlways  =   0x0200,
      OpenExisting  =   0x0400,
      TruncateExist =   0x0800,

      Text          =   0x1000, // typeText are used in derived classes only
                                // default type Binary
    };

    // file attributes -> internally remapped into OS values
    enum Attribute {
      Normal =    0x00,
      RdOnly =    0x01,
      Hidden =    0x02,
      System =    0x04,
      Volume =    0x08,
      Directory = 0x10,
      Archive =   0x20,
      Temporary = 0x40,
    };

    //
    enum BinType {
      TypeChar,
      TypeShort,
      TypeLong,
      TypeFloat,
      TypeDouble,
      TypeLong64,
      TypePoint, // == TypeSize,
      TypeRect,
    };

    TFile();
    TFile(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);
    TFile(TFileHandle* handle, bool shouldClose);
    // if TFileHandle not support Clone() will throw TXNotSupportedCall()
    TFile(const TFile & file);
     virtual ~TFile();

    virtual TFileHandle* GetHandle() const;
    const owl_string GetName() const;

    virtual bool    Open(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);
    virtual bool    Close();

    virtual bool    Length(uint64 newLen);
    virtual uint64   Position64()   const;
    virtual uint64   Length64()   const;
    virtual bool     Length(uint32 newLen);
    virtual uint32   Position() const;
    virtual uint32   Length()   const;

    virtual uint64   Seek(int64 offset, seek_dir origin = beg);
    virtual uint32   Seek(long offset, seek_dir origin = beg);

    uint64          SeekToBegin64();
    uint64           SeekToEnd64();
    uint32           SeekToBegin();
    uint32           SeekToEnd();

    bool            IsOpen() const;
    uint32          GetOpenMode() const;
    uint32          LastError();

    virtual uint32   Read(void * buffer, uint32 numBytes);
    virtual bool      Write(const void * buffer, uint32 numBytes);
    virtual bool    Flush();

    uint            ReadStruct(void * buffer, BinType btype, TByteOrderType type);
    uint            ReadStruct(void * buffer, TBinField * fields,
                               TByteOrderType type);
    static   uint    ReadStruct(uint8 * readBuf, void * buffer,
                              TBinField * fields, TByteOrderType type);
    uint            WriteStruct(void * buffer, BinType btype, TByteOrderType type);
    uint             WriteStruct(void * buffer, TBinField * fields,
                                TByteOrderType type);
    static   uint    WriteStruct(uint8 * writeBuf, void * buffer,
                               TBinField * fields, TByteOrderType type);

    static  uint    StructSize(TBinField * fields);

    virtual bool    LockRange(uint32 position, uint32 count);
    virtual bool    UnlockRange(uint32 position, uint32 count);
    virtual bool    LockRange(uint64 position, uint64 count);
    virtual bool    UnlockRange(uint64 position, uint64 count);

#if defined(OWL5_COMPAT) // must be moved to TFileName
    // don't use, this functions call right to TFileName
    static bool     GetStatus(LPCTSTR name, TFileStatus & status);
    // don't use, this functions call right to TFileName
    static bool     SetStatus(LPCTSTR name, const TFileStatus & status);
#endif
    bool            GetStatus(TFileStatus & status) const;

    // streaming support (internal functions)
    virtual uint8  _OWLENTRY  readUint8();
    virtual uint16 _OWLENTRY   readUint16();
    virtual uint32 _OWLENTRY   readUint32();
    virtual uint64 _OWLENTRY   readUint64();
    virtual float  _OWLENTRY   readFloat();
    virtual double _OWLENTRY   readDouble();
    virtual LPTSTR _OWLENTRY   readString( _TCHAR  *);

    virtual void _OWLENTRY writeUint8( const uint8 );
    virtual void _OWLENTRY writeUint16( const uint16 );
    virtual void _OWLENTRY writeUint32( const uint32 );
    virtual void _OWLENTRY writeUint64( const uint64 );
    virtual void _OWLENTRY writeFloat( const float );
    virtual void _OWLENTRY writeDouble( const double );
    virtual void _OWLENTRY writeString( const _TCHAR  *);

  protected:
    TFileHandle*  Handle;      // Low-level C file handle
    bool           ShouldClose; // Should C++ object close file on dtor

    // read/write structure helpers
    uint8*    Buffer;
    uint      BufSize;
};

//------------------------------------------------------------------------------
// Abstract class
//
// class TFileHandle
// ~~~~~ ~~~~~~~~~~~
//
class _OWLCLASS TFileHandle {
  friend class _OWLCLASS TFile;
  protected:
    TFileHandle(){}
    virtual ~TFileHandle(){}
  public:
    virtual uint    GetOpenMode() = 0;
    virtual const owl_string GetName()  = 0;
    virtual uint32  LastError() = 0;

    virtual bool    IsOpen()  = 0;
    virtual TFileHandle* Clone() const = 0;
    virtual bool     Close() = 0;
    virtual uint32   Read(void * buffer, uint32 numBytes) = 0;
    virtual bool     Write(const void * buffer, uint32 numBytes) = 0;
    virtual bool    Length(uint64 newLen) = 0;
    virtual uint64   Position64()   const = 0;
    virtual uint64   Length64()   const = 0;
    virtual bool     Length(uint32 newLen) = 0;
    virtual uint32   Position() const = 0;
    virtual uint32   Length()   const = 0;
    virtual uint64   Seek(int64 offset, TFile::seek_dir origin = TFile::beg) = 0;
    virtual uint32   Seek(long offset, TFile::seek_dir origin = TFile::beg) = 0;
    virtual bool     Flush() = 0;
    virtual bool     LockRange(uint32 position, uint32 count) = 0;
    virtual bool     UnlockRange(uint32 position, uint32 count) = 0;
    virtual bool     LockRange(uint64 position, uint64 count) = 0;
    virtual bool     UnlockRange(uint64 position, uint64 count) = 0;
    virtual bool    GetStatus(TFileStatus  & status) const = 0;

  private: 
    TFileHandle(const TFileHandle& handle);
};
///////////////////////////////////
//
// class TDiskFileHandle
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLCLASS TDiskFileHandle: public  TFileHandle {
  friend class _OWLCLASS TFileName;
  public:
    TDiskFileHandle(LPCTSTR name, uint32 mode);

#if defined(BI_NO_COVAR_RET)
    TFileHandle* Clone() const;
#else
    TDiskFileHandle* Clone() const;
#endif
    virtual uint    GetOpenMode() { return OpenMode; }
    virtual const owl_string GetName() { return FileName; }
    virtual uint32 LastError();
    virtual bool    IsOpen()  { return Handle != INVALID_HANDLE_VALUE; }
    
    virtual bool     Close();
    virtual uint32   Read(void * buffer, uint32 numBytes);
    virtual bool     Write(const void * buffer, uint32 numBytes);
    virtual bool    Length(uint64 newLen);
    virtual uint64   Position64()   const;
    virtual uint64   Length64()   const;
    virtual bool     Length(uint32 newLen);
    virtual uint32   Position() const;
    virtual uint32   Length()   const;
    virtual uint64   Seek(int64 offset, TFile::seek_dir origin = TFile::beg);
    virtual uint32   Seek(long offset, TFile::seek_dir origin = TFile::beg);
    virtual bool     Flush();
    virtual bool     LockRange(uint32 position, uint32 count);
    virtual bool     UnlockRange(uint32 position, uint32 count);
    virtual bool     LockRange(uint64 position, uint64 count);
    virtual bool     UnlockRange(uint64 position, uint64 count);
    virtual bool    GetStatus(TFileStatus  & status) const;

  protected:
    TDiskFileHandle(HANDLE handle, LPCTSTR name, uint mode);
    HANDLE      Handle;
    uint        OpenMode;
    owl_string  FileName;

  private:
    TDiskFileHandle(const TDiskFileHandle& handle);
};

//------------------------------------------------------------------------------
//
// class TBufferedFile
// ~~~~~ ~~~~~~~~~~~~~
class _OWLCLASS TBufferedFile : public TFile{
  public:
    enum { DefaultBufferSize = 2048};

    TBufferedFile(); 
    TBufferedFile(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);
    // if TFileHandle isn't support Clone() will be throw
    TBufferedFile(const TBufferedFile & file);
    TBufferedFile(TFileHandle * handle, bool shouldClose);
     
    virtual ~TBufferedFile();

    virtual bool Close();
    virtual bool    Length(uint64 newLen);
    virtual uint64   Position64()   const;
    virtual uint64   Length64()   const;
    virtual uint32   Position() const;
    virtual uint32   Length()   const;
    virtual bool    Length(uint32 newLen);

    virtual uint64   Seek(int64 offset, seek_dir origin = beg);
    virtual uint32   Seek(long offset, seek_dir origin = beg);

    virtual uint32   Read(void * buffer, uint32 numBytes);
    virtual bool      Write(const void* buffer, uint32 numBytes);
    virtual bool     Flush();

    // if buffer == 0, allocate memory
    virtual void     SetBuffer(uint8* buffer, uint size, TAutoDelete autodelete = AutoDelete);

  protected:

    virtual void InitBuffer(uint size = DefaultBufferSize);
    virtual bool FlushBuffer();

  protected:
    // read/write buffer data
    uint8*    FileBuffer;
    bool      ShouldDelete;
    uint      FileBufSize;

    uint8*    CurByte;
    uint32    CurPos;
    uint32    StartPos;
    uint32    EndPos;
    bool       BufferEmpty;
};

//------------------------------------------------------------------------------
//
// TTextFile
//
class _OWLCLASS TTextFile : public TBufferedFile{
  public:
    TTextFile();
    TTextFile(const TTextFile & file);
    TTextFile(TFileHandle* handle, bool shouldClose);
    TTextFile(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting|Text);

    virtual LPTSTR   GetString(LPTSTR buffer, uint32 size);
    virtual bool     WriteString(LPCTSTR str);

    // streaming support (internal functions)
    virtual uint8  _OWLENTRY  readUint8();
    virtual uint16 _OWLENTRY   readUint16();
    virtual uint32 _OWLENTRY   readUint32();
    virtual uint64 _OWLENTRY   readUint64();
    virtual float  _OWLENTRY   readFloat();
    virtual double _OWLENTRY   readDouble();
    virtual LPTSTR _OWLENTRY   readString( _TCHAR  *);

    virtual void _OWLENTRY writeUint8( const uint8 );
    virtual void _OWLENTRY writeUint16( const uint16 );
    virtual void _OWLENTRY writeUint32( const uint32 );
    virtual void _OWLENTRY writeUint64( const uint64 );
    virtual void _OWLENTRY writeFloat( const float );
    virtual void _OWLENTRY writeDouble( const double );
    virtual void _OWLENTRY writeString( const _TCHAR  *);
};

//------------------------------------------------------------------------------
//
// TFileLineIterator
//
class _OWLCLASS TFileLineIterator {
  public:
    TFileLineIterator(TTextFile& file, uint bufsize = 260);
    virtual ~TFileLineIterator();

    const TCHAR* operator *() const;
    operator const TCHAR*() const;
    const TCHAR* operator ++();
    const TCHAR* Current() const;
    const uint    Line() const;

  protected:
    TFileLineIterator();
    // all work do here -> must fill LineBuffer
    virtual bool NextLine();

  protected:
    TTextFile*  File;
    LPTSTR       LineBuffer;
    uint        BuffSize;
    uint        LineNumber;
    bool        Done;
};

//------------------------------------------------------------------------------
//
// class TXBadFormat
// ~~~~~ ~~~~~~~~~~~~~
class _OWLCLASS TXBadFormat : public TXOwl {
  public:
    TXBadFormat();

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXBadFormat* Clone();
#endif
    void   Throw();

    static void Raise();
};

// structures for manipulating RIFF headers
#if !defined(owlFCC)
#  if 0
#    define owlFCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |    \
                       (((DWORD)(ch4) & 0xFF00) << 8) |    \
                       (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                       (((DWORD)(ch4) & 0xFF000000) >> 24))
#  else
//#  define extFCC(ch4) ((DWORD)(ch4))
#    define owlFCC(ch0, ch1, ch2, ch3)                                \
      ((uint32)(uint8)(ch0) | ((uint32)(uint8)(ch1) << 8) |          \
      ((uint32)(uint8)(ch2) << 16) | ((uint32)(uint8)(ch3) << 24 ))
#  endif
#endif

// standard four character codes //
#if !defined(FOURCC_RIFF)
#  define FOURCC_RIFF     owlFCC('R','I','F','F')
#  define FOURCC_LIST     owlFCC('L','I','S','T')
#endif

//------------------------------------------------------------------------------
//
// RIFF chunk information data structure
//
struct TCkInfo {
  enum ckFlags {
          ckDirty = 0x0001,
          ckList   = 0x0002,
          ckRiff   = 0x0004,
        };

  TCkInfo();

  uint32    CkId;    // chunk ID                         //
  uint32    Size;   // chunk size                       //
  uint32    Type;   // form type or list type           //
  uint32    Offset; // offset of data portion of chunk   //
  uint32    Flags;  // flags used by MMIO functions     //
};

//------------------------------------------------------------------------------
//
// TRiffFile        read/write RIFF files
// ~~~~~~~~~
//
class _OWLCLASS TRiffFile : public TBufferedFile {
  public:
    enum TFindFlags {
      ffReset      = 0x0001,
      ffFindAny    = 0x0002,
      ffFindChunk  = 0x0004,
      ffFindList   = 0x0008,
      ffFindRiff   = 0x0010,
    };
    enum TCreateFlags{
      cfCreateChunk,
      cfCreateList,
      cfCreateRiff,
    };

  public:
    TRiffFile();
    TRiffFile(const TRiffFile & file);
    TRiffFile(TFileHandle* handle, bool shouldClose);
    TRiffFile(const _TCHAR * name, const uint32 acess = ReadOnly|PermRead|OpenExisting);

    bool  CreateChunk(TCkInfo& info, const TCreateFlags = cfCreateChunk);
    bool  Ascent(TCkInfo& info);
    bool  Descent(TCkInfo& info, TCkInfo* parent = 0, const TFindFlags = ffFindAny);
};

//------------------------------------------------------------------------------
//
// QuickTime atom information data structure
//
struct TQtInfo {
  enum qtFlags {qtDirty  = 0x0001,};  // only dirty flag currently (for internal using)

  TQtInfo();

  uint32  Size;        //
  uint32   Type;        // chunk identifier
  uint32   Offset;      //
  uint32   Flags;       //
};

//------------------------------------------------------------------------------
//
// TQtFile
// ~~~~~~~
//
class _OWLCLASS TQtFile: public TBufferedFile {
  public:
    enum TFindFlags {
      ffFindAny   = 0x0000,
      ffReset     = 0x0001,
      ffFindChunk = 0x0002,
    };
  public:
    TQtFile();
    TQtFile(const TQtFile & file);
    TQtFile(TFileHandle * handle, bool shouldClose);
    TQtFile(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);

    bool Ascent(TQtInfo & info);
    bool Descent(TQtInfo & info, TQtInfo* parent = 0, 
                 const TFindFlags = TFindFlags(ffFindChunk|ffReset));
    bool CreateChunk(TQtInfo & info);
};


//------------------------------------------------------------------------------
// Not finished
// class TStreamFile
// ~~~~~ ~~~~~
#if defined(BI_STD_IO)
#  if defined(UNICODE)
    //using std::wfstream;
    typedef std::wfstream _tfstream;
#  else
    //using std::fstream;
    typedef std::fstream _tfstream;
#  endif
#else
  typedef fstream _tfstream;
#endif

// class TFileHandle
// ~~~~~ ~~~~~~~~~~~
//
class TStreamFile;
class _OWLCLASS TStreamHandle: public TFileHandle {
  friend class TStreamFile;
  protected:
    TStreamHandle(TStreamFile* parent, LPCTSTR name, uint32 mode);
    virtual ~TStreamHandle(){}

  public:
    virtual uint GetOpenMode() { return OpenMode; }
    virtual const owl_string GetName() { return FileName; }
    virtual uint32  LastError();

    virtual bool    IsOpen();
#if defined(BI_NO_COVAR_RET)
    virtual TFileHandle* Clone() const;
#else
    TStreamHandle* Clone() const;
#endif
    virtual bool     Close();
    virtual uint32   Read(void * buffer, uint32 numBytes);
    virtual bool     Write(const void * buffer, uint32 numBytes);
    virtual bool    Length(uint64 newLen);
    virtual uint64   Position64()   const;
    virtual uint64   Length64()   const;
    virtual bool     Length(uint32 newLen);
    virtual uint32   Position() const;
    virtual uint32   Length()   const;
    virtual uint64   Seek(int64 offset, TFile::seek_dir origin = TFile::beg);
    virtual uint32   Seek(long offset, TFile::seek_dir origin = TFile::beg);
    virtual bool     Flush();
    virtual bool     LockRange(uint32 position, uint32 count);
    virtual bool     UnlockRange(uint32 position, uint32 count);
    virtual bool     LockRange(uint64 position, uint64 count);
    virtual bool     UnlockRange(uint64 position, uint64 count);
    virtual bool    GetStatus(TFileStatus  & status) const;

  protected:
    TStreamFile*  Parent;
    uint          OpenMode;
    owl_string    FileName;
};

///////////////////////////////////////////////////////////////
// class TStreamFile
// ~~~~~ ~~~~~~~~~~~
//
// class _OWLCLASS TStreamFile
// fatal error C1001: INTERNAL COMPILER ERROR  (compiler file 'f:\vs70builds\3077\vc\Compiler\Utc\src\P2\p2symtab.c', line 4533)
class _OWLCLASS TStreamFile : public TFile, public _tfstream {
  public:
    TStreamFile();
    TStreamFile(TStreamFile & file);
    TStreamFile(TStreamHandle * handle, bool shouldClose);
    TStreamFile(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);
    virtual ~TStreamFile(){}

    virtual bool Open(LPCTSTR name, const uint32 mode = ReadOnly|PermRead|OpenExisting);
};
//------------------------------------------------------------------------------

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//---------------------------------------------------------------------------
// Inlines
//
inline TFile::TFile()
: 
  Handle(FileNull),ShouldClose(false),Buffer(0),BufSize(0)
{
}
inline TFile::TFile(LPCTSTR name, const uint32 mode)
:
  Handle(FileNull),  ShouldClose(true), Buffer(0), BufSize(0)
{
  Open(name, mode);
}
inline TFile::TFile(TFileHandle* handle, bool shouldClose)
:  
  Handle(handle), ShouldClose(shouldClose), Buffer(0), BufSize(0)
{
}
//
inline TFileHandle* TFile::GetHandle() const {
  return Handle;
}
//
inline const owl_string TFile::GetName() const {
  PRECONDITION(Handle);
  return Handle->GetName();
}
//
inline bool TFile::IsOpen() const {
  return Handle != FileNull;
}
//
inline uint32 TFile::GetOpenMode() const {
  PRECONDITION(Handle);
  return Handle->GetOpenMode();
}
//
inline uint32 TFile::SeekToBegin() {
  return Seek(0l, beg);
}
//
inline uint32 TFile::SeekToEnd(){
  return Seek(0l, end);
}
//
inline uint64 TFile::SeekToBegin64(){
  return Seek((int64)0, beg);
}
//
inline uint64 TFile::SeekToEnd64(){
  return Seek((int64)0, end);
}

//
inline uint32 TFile::LastError(){
  PRECONDITION(Handle);
  return Handle->LastError();
}

//
inline uint32 TFile::Read(void* buffer, uint32 numBytes){
  PRECONDITION(Handle);
  return Handle->Read(buffer, numBytes);
}

//
inline bool TFile::Write(const void* buffer, uint32 numBytes){
  PRECONDITION(Handle);
  return Handle->Write(buffer, numBytes);
}

//
inline uint32 TFile::Position() const{
  PRECONDITION(Handle);
  return Handle->Position();
}

//
inline uint32 TFile::Length() const{
  PRECONDITION(Handle);
  return Handle->Length(); 
}

//
inline bool TFile::Length(uint32 newLen){
  PRECONDITION(Handle);
  return Handle->Length(newLen);
}

//
inline uint32 TFile::Seek(long offset, seek_dir origin){
  PRECONDITION(Handle);
  return Handle->Seek(offset, origin);
}

//
inline bool TFile::Length(uint64 newLen){
  PRECONDITION(Handle);
  return Handle->Length(newLen);
}
//
inline uint64 TFile::Position64() const{
  PRECONDITION(Handle);
  return Handle->Position64();
}
//
inline uint64 TFile::Length64() const {
  PRECONDITION(Handle);
  return Handle->Length64();
}
//
inline uint64 TFile::Seek(int64 offset, seek_dir origin)
{
  PRECONDITION(Handle);
  return Handle->Seek(offset, origin);
}
//
inline bool TFile::LockRange(uint64 position, uint64 count)
{
  PRECONDITION(Handle);
  return Handle->LockRange(position, count);
}
//
inline bool TFile::UnlockRange(uint64 position, uint64 count)
{
  PRECONDITION(Handle);
  return Handle->UnlockRange(position, count);
}
//
inline bool TFile::GetStatus(TFileStatus  & status) const
{
  PRECONDITION(Handle);
  return Handle->GetStatus(status);
}
//
// streaming support
inline LPTSTR _OWLENTRY TFile::readString( LPTSTR str){
  // this version just read 256 characters ??? for now
  if(Read(str, 256) == TFILE_ERROR)
    return 0;
  return str;
}
//
inline void _OWLENTRY TFile::writeString( const _TCHAR  * str){
  Write(str, ::_tcslen(str));
}
// inline stream operators
inline TFile& _OWLENTRY operator >> ( TFile& file, int8& c){
  c = (int8)file.readUint8();
  return file;
}
//
inline TFile& _OWLENTRY operator >> ( TFile& file, uint8& c){
  c = (uint8)file.readUint8();
  return file;
}
//
inline TFile& _OWLENTRY operator >> ( TFile& file, int16& i){
  i = (int16)file.readUint16();
  return file;
}
//
inline TFile& _OWLENTRY operator >> ( TFile& file, uint16& i){
  i = file.readUint16();
  return file;
}
//
inline TFile& _OWLENTRY operator >> ( TFile& file, signed int& i){
  i = (signed int)file.readUint32();
  return file;
}
//
inline TFile& _OWLENTRY operator >> ( TFile& file, unsigned int& i){
  i = (uint)file.readUint32();
  return file;
}
//
#if defined(BI_UNIQUE_BOOL)
inline TFile& _OWLENTRY operator >> ( TFile& file, bool& b){
  b = static_cast<bool>(file.readUint32());
  return file;
}
#endif
//
inline TFile& _OWLENTRY operator >> ( TFile& file, int32& i){
  i = (int32)file.readUint32();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, uint32& i){
  i = file.readUint32();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, int64& i){
  i = (int64)file.readUint64();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, uint64& i){
  i = file.readUint64();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, float& f){
  f = file.readFloat();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, double& d){
  d = file.readDouble();
  return file;
}
inline TFile& _OWLENTRY operator >> ( TFile& file, _TCHAR * str){
  file.readString(str);
  return file;
}
// writing support
inline TFile& _OWLENTRY operator << ( TFile& file, int8 c){
  file.writeUint8((uint8)c);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, uint8 c){
  file.writeUint8(c);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, int16 i){
  file.writeUint16((int16)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, uint16 i){
  file.writeUint16((uint16)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, signed int i){
  file.writeUint32((signed int)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, unsigned int i){
  file.writeUint32((uint)i);
  return file;
}
#if defined(BI_UNIQUE_BOOL)
inline TFile& _OWLENTRY operator << ( TFile& file, bool b){
  file.writeUint32((uint32)b);
  return file;
}
#endif
inline TFile& _OWLENTRY operator << ( TFile& file, int32 i){
  file.writeUint32((int32)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, uint32 i){
  file.writeUint32((uint32)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, int64 i){
  file.writeUint64(*(uint64*)&i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, uint64 i){
  file.writeUint64((uint64)i);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, float f){
  file.writeFloat(f);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, double d){
  file.writeDouble(d);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, const _TCHAR  * s){
  file.writeString(s);
  return file;
}
inline TFile& _OWLENTRY operator << ( TFile& file, const owl_string& s){
  size_t size = s.length();
  file.writeUint32(size);
  file.Write(s.c_str(), size);
  return file;
}

//
// TBufferedFile
//
inline TBufferedFile::TBufferedFile(){
  InitBuffer();
}
inline TBufferedFile::TBufferedFile(const TBufferedFile & file)
:  TFile(file)
{
  InitBuffer();
}
inline TBufferedFile::TBufferedFile(TFileHandle* handle, bool shouldClose)
:  TFile(handle, shouldClose)
{
  InitBuffer();
}
inline TBufferedFile::TBufferedFile(LPCTSTR name, const uint32 mode)
:  TFile(name, mode)
{
  InitBuffer();
}
//
inline bool 
TBufferedFile::Close()
{
  if(FlushBuffer())
    return TFile::Close();
  return false;
}
inline bool  TBufferedFile::Length(uint64 newLen){
  if(FlushBuffer())
    return TFile::Length(newLen);
  return false;
}
inline uint64 TBufferedFile::Length64() const{
  return TFile::Length64()+uint64(CurPos-StartPos);
}
inline uint64  TBufferedFile::Position64() const{
  return uint64(CurPos);
}
inline uint32  TBufferedFile::Position() const{
  return CurPos;
}
inline uint32  TBufferedFile::Length() const{
  return TFile::Length()+(CurPos-StartPos);
}
inline bool  TBufferedFile::Length(uint32 newLen){
  if(FlushBuffer())
    return TFile::Length(newLen);
  return false;
}
inline bool  TBufferedFile::Flush(){
  if(FlushBuffer())
    return TFile::Flush();
  return false;
}

//
// TTextFile
//
inline TTextFile::TTextFile(){
}
inline TTextFile::TTextFile(TFileHandle* handle, bool shouldClose)
: TBufferedFile(handle, shouldClose)
{
}
inline TTextFile::TTextFile(LPCTSTR name, const uint32 mode)
: TBufferedFile(name, mode)
{
}
inline TTextFile::TTextFile(const TTextFile & file)
: TBufferedFile(file)
{
}
inline LPTSTR _OWLENTRY TTextFile::readString( _TCHAR * str){
  return GetString(str, MAX_PATH);
}
inline void _OWLENTRY TTextFile::writeString( const _TCHAR * str){
  Write(str,::_tcslen(str));
}

//
// TFileLineIterator
//
inline TFileLineIterator::TFileLineIterator()
: File(0),LineBuffer(0),LineNumber(0),Done(false)
{
}
inline const TCHAR* TFileLineIterator::operator *() const{
  return Done ? 0 : LineBuffer;
}
inline TFileLineIterator::operator const TCHAR*() const{
  return Done ? 0 : LineBuffer;
}
inline const TCHAR* TFileLineIterator::Current() const{
  return Done ? 0 : LineBuffer;
}
inline const TCHAR* TFileLineIterator::operator ++()
{
  Done = !NextLine();
  if(!Done){
    LineNumber++;
    return LineBuffer;
  }
  return 0;
}
inline const uint TFileLineIterator::Line() const{
  return LineNumber;
}

//
// TCkInfo
//
inline TCkInfo::TCkInfo()
: CkId(0),Size(0),Type(0),Offset(0),Flags(0)
{
}
//
// TRiffFile
//

inline TRiffFile::TRiffFile()
: TBufferedFile()
{
}
inline TRiffFile::TRiffFile(const _TCHAR * name, const uint32 mode)
: TBufferedFile(name, mode)
{
}
inline TRiffFile::TRiffFile(const TRiffFile & file)
: TBufferedFile(file)
{
}
inline TRiffFile::TRiffFile(TFileHandle* handle, bool shouldClose)
: TBufferedFile(handle,shouldClose)
{
}
//
// TQtInfo
//
inline TQtInfo::TQtInfo()
: Size(0),Type(0),Offset(0),Flags(0)
{
}

//
// TQtFile
//
inline TQtFile::TQtFile()
: TBufferedFile()
{
}
inline TQtFile::TQtFile(LPCTSTR name, const uint32 openMode)
: TBufferedFile(name, openMode)
{
}
inline TQtFile::TQtFile(const TQtFile & file)
: TBufferedFile(file)
{
}
inline TQtFile::TQtFile(TFileHandle* handle, bool shouldClose)
: TBufferedFile(handle,shouldClose)
{
}
//------------------------------------------------------------------------------
inline TStreamFile::TStreamFile()
{
}
inline TStreamFile::TStreamFile(TStreamHandle* handle, bool shouldClose)
: TFile(handle, shouldClose)
{
}
inline TStreamFile::TStreamFile(LPCTSTR name, const uint32 mode)
{
  Open(name, mode);
}
//------------------------------------------------------------------------------

__OWL_END_NAMESPACE


#endif  // OWL_FILE_H
