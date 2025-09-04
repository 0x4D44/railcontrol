//----------------------------------------------------------------------------
// ObjectComponents
// Copyright 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of TOcStorage & TOcStream classes
//----------------------------------------------------------------------------

#if !defined(OCF_OCSTORAG_H)
#define OCF_OCSTORAG_H

#if !defined(OCF_DEFS_H)
# include <ocf/defs.h>
#endif


#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//
// Classes referenced
//
interface _ICLASS IRootStorage;
interface _ICLASS IStorage;
interface _ICLASS IStream;
interface _ICLASS ILockBytes;
interface _ICLASS IEnumSTATSTG;

__OCF_BEGIN_NAMESPACE

//
// Classes defined
//
class _ICLASS TOcStream;
class _ICLASS TOcStorage;

//
// class TOcStream
// ~~~~~ ~~~~~~~~~
class _OCFCLASS TOcStream {
  public:
    TOcStream(TOcStorage& storage, LPCTSTR name, bool create,
              uint32 mode = STGM_READWRITE);
    TOcStream(TOcStream& stream);
    TOcStream(IStream* stream);
   ~TOcStream();

    IStream* GetIStream();

    HRESULT Read(void * pv, ulong cb, ulong * read = 0);
    HRESULT Write(void const * pv, ulong cb, ulong * written = 0);
    HRESULT Seek(int64 move, uint32 origin= STREAM_SEEK_SET,
                 uint64 * newPosition = 0);
    HRESULT SetSize(uint64 newSize);
    HRESULT CopyTo(TOcStream& stream, uint64 cb, uint64 * read = 0,
                   uint64 * written = 0);
    HRESULT Commit(uint32 commitFlags);
    HRESULT Revert();
    HRESULT LockRegion(uint64 offset, uint64 cb, uint32 lockType);
    HRESULT UnlockRegion(uint64 offset, uint64 cb, uint32 lockType);
    HRESULT Stat(STATSTG * statstg, uint32 statFlag);

  protected:
    HRESULT Clone(IStream * * ppstm);

    IStream* StreamI;
};

//
// class TOcStorage
// ~~~~~ ~~~~~~~~~~
class _OCFCLASS TOcStorage {
  public:
    TOcStorage(LPCTSTR fileName, bool create,
               uint32 mode = STGM_READWRITE|STGM_TRANSACTED);
    TOcStorage(ILockBytes * lkbyt, bool create,
               uint32 mode = STGM_READWRITE|STGM_TRANSACTED);
    TOcStorage(TOcStorage& parent, LPCTSTR name, bool create,
               uint32 mode = STGM_READWRITE);
    TOcStorage(IStorage* storage);
   ~TOcStorage();

    IStorage* GetIStorage();

    HRESULT CopyTo(uint32 ciidExclude, IID const * rgiidExclude,
                   SNB snbExclude, TOcStorage& dest);
    HRESULT MoveElementTo(LPCTSTR name, TOcStorage& dest,
                          LPCTSTR newName, uint32 grfFlags);
    HRESULT Commit(uint32 grfCommitFlags);
    HRESULT Revert();
    HRESULT EnumElements(uint32 reserved1, void * reserved2,
                         uint32 reserved3, IEnumSTATSTG ** ppenm);
    HRESULT DestroyElement(LPCTSTR name);
    HRESULT RenameElement(LPCTSTR oldName, LPCTSTR newName);
    HRESULT SetElementTimes(LPCTSTR name, FILETIME const * pctime,
                            FILETIME const * patime,
                            FILETIME const * pmtime);
    HRESULT SetClass(const IID & clsid);
    HRESULT SetStateBits(uint32 grfStateBits, uint32 grfMask);
    HRESULT Stat(STATSTG  *pstatstg, uint32 grfStatFlag);

    HRESULT SwitchToFile(LPCTSTR newPath);

    static HRESULT IsStorageFile(LPCTSTR pwcsName);
    static HRESULT IsStorageILockBytes(ILockBytes * plkbyt);
    static HRESULT SetTimes(LPCTSTR lpszName,
                            FILETIME const * pctime,
                            FILETIME const * patime,
                            FILETIME const * pmtime);

  protected:
    HRESULT CreateStream(LPCTSTR name, uint32 mode, uint32 rsrvd1,
                         uint32 rsrvd2, IStream * * stream);
    HRESULT OpenStream(LPCTSTR name, void  *rsrvd1, uint32 grfMode,
                       uint32 rsrvd2, IStream  * *stream);
    HRESULT CreateStorage(LPCTSTR name, uint32 mode, uint32 rsrvd1,
                          uint32 rsrvd2, IStorage ** storage);
    HRESULT OpenStorage(LPCTSTR name, IStorage * stgPriority,
                        uint32 mode, SNB snbExclude, uint32 rsrvd,
                        IStorage ** storage);
    ulong   AddRef();
    ulong   Release();

#if 0  // not currently implemented
//    TOcStorage*    GetParent() const {return Parent;}
//    int            GetOpenMode() const {return OpenMode;}
//    void           SetOpenMode(int mode) const {OpenMode = mode;}

  protected:
//    int         ThisOpen;     // actual mode bits used for opening storage

  private:
//    int         OpenMode;     // mode and protection flags
//    int         OpenCount;
//    TOcStorage* Parent;
#endif
    IStorage*   StorageI;

  friend TOcStream;
};

__OCF_END_NAMESPACE

#endif  // OCF_OCSTORAG_H
