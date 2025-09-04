//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30.2.2 $
//$Author: jogybl $ 
//$Date: 2009-11-23 08:45:18 $
// Implementation of classes TFileDocument, TFileInStream, TFileOutStream,
// TFileBuf
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_FILEDOC_H)
# include <owl/filedoc.h>
#endif
#include <stdio.h>

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

//
// class TFileBuf
// ~~~~~ ~~~~~~~~
class _OWLCLASS TFileBuf : public _tstreambuf {
#if defined(BI_STD_IO)
    bool buffered_;
    _TCHAR *base_;
    _TCHAR *ebuf_;
#endif
  public:
    enum { shDefault = shReadWrite };  // default file sharing

   ~TFileBuf() {
#if defined(BI_STD_IO)
      if (base_) delete[] base_;
#endif
   }
#if !defined(BI_STD_IO)
   int eof(){
     return EOF;
#else
   int_type eof(){
     return traits_type::eof();
#endif
   }
#if !defined(BI_STD_IO)
    TFileBuf(int fhdl, int omode, long seekpos);

    virtual int overflow(int = EOF);
    virtual streampos  seekoff(streamoff, ios::seek_dir, int);
    virtual int sync();
    virtual int underflow();

#else //defined BI_STD_IO
    TFileBuf(int fhdl, int omode, streampos seekpos);


    virtual int_type overflow(int_type c = _tstreambuf::traits_type::eof());
    virtual int_type underflow();

    virtual _tstreambuf* setbuf(char_type *s, streamsize n);

    virtual pos_type seekoff(off_type,
                             ios_base::seekdir, ios_base::openmode which =
                             ios_base::in | ios_base::out);

    virtual pos_type seekpos(pos_type sp,
                             ios_base::openmode which =
                                       ios_base::in | ios_base::out);

    virtual int sync();

    int out_waiting(){
      return pptr() ? (int)(pptr() - pbase()) : 0;
    }

    void setb(_TCHAR* beg, _TCHAR* end, int = 0){
      int size_buf = end - beg;
      buffered_ = beg && size_buf;
      setbuf(beg, size_buf);
    }

    int unbuffered(){
      return !buffered_;
    }

    void unbuffered(int buf){
      if (buf){
        buffered_ = false;
        setb(0, 0, 0);
      }
    }

    _TCHAR *base(){
      return base_;
    }

    int blen(){
      return (int) (ebuf_ - base_);
    }
#endif //BI_STD_IO

    int       xfd;       // the file descriptor, EOF if closed
    int       mode;      // the opened mode
    streampos last_seek;
    _TCHAR    lahead[2]; // current input char if unbuffered
};

//
// class TFileStreamBase
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLCLASS TFileStreamBase : virtual public _tios {
  public:
    TFileStreamBase(int fhdl, int omode, streampos seekpos);
   ~TFileStreamBase() {}
    TFileBuf buf;
};

//
// class TFileInStream
// ~~~~~ ~~~~~~~~~~~~~
//JJH gcc-3.1 doesn't like original inheritance order - it refuses to
//    correctly fill rdbuf variable - effect is that you cannot save
//    anything... (all this just becaus of MI!).
#if (__GNUC__ >= 3)
class _OWLCLASS TFileInStream : public TInStream, public TFileStreamBase {
#else
class _OWLCLASS TFileInStream : public TFileStreamBase, public TInStream {
#endif
  public:
    TFileInStream(TFileDocument& doc, HFILE fhdl, int omode, streampos seekpos)
               : TInStream (doc,0,omode), TFileStreamBase(fhdl,omode,seekpos){}
   ~TFileInStream();
};

//
// class TFileOutStream
// ~~~~~ ~~~~~~~~~~~~~~
//JJH the same as above, you have to change inheritance order in order
//    TFileOutStream to work correctly...
#if (__GNUC__ >= 3)
class _OWLCLASS TFileOutStream : public TOutStream, public TFileStreamBase {
#else
class _OWLCLASS TFileOutStream : public TFileStreamBase, public TOutStream {
#endif
  public:
    TFileOutStream(TFileDocument& doc, HFILE fhdl, int omode, streampos seekpos)
              : TOutStream (doc,0,omode), TFileStreamBase(fhdl,omode,seekpos){}
   ~TFileOutStream();
};


//------------------------------------------------------------------------------

//
//
//
HFILE
TFileDocument::OpenThisFile(int omode, LPCTSTR name, streampos* pseekpos)
{
  int how;
  HFILE fhdl;
  bool exists = false;
  int share = omode & shMask;
  _USES_CONVERSION;

  if (share < shCompat)
    share = (omode & ofWrite) ? shRead : shReadWrite;

  how = ((unsigned)(share-shCompat)) >> 5;

  if (omode & ofWrite) {
    if (!(omode & (ofAtEnd | ofAppend | ofRead)))
      omode |= ofTruncate; // output implies truncate unless in, app, or ate

    if (omode & ofRead)
      how |= OF_READWRITE;
    else
      how |= OF_WRITE;
    if (!((omode & ofNoCreate) && !(omode & ofTruncate))) {
      if (!((omode & ofTruncate) && !(omode & (ofNoCreate | ofNoReplace)))) {
        if ((fhdl=::_lopen(_W2A(name),OF_READ|OF_SHARE_COMPAT))   !=HFILE_ERROR
        ||  (fhdl=::_lopen(_W2A(name),OF_READ|OF_SHARE_DENY_NONE))!=HFILE_ERROR) {
          ::_lclose(fhdl);
          exists = true;
          }
        }
      if (!(exists && !(omode & (ofNoCreate | ofNoReplace)))) {
        if ((exists && (omode & ofNoReplace))
        || (!exists && (omode & ofNoCreate)) )
//            return 0; // ? Jogy - this function should return either valid handle or HFILE_ERROR
            return HFILE_ERROR;
        if ((fhdl = _lcreat(_W2A(name), 0)) == HFILE_ERROR)
            return HFILE_ERROR;
        ::_lclose(fhdl);   // close in order to open with share mode
        }
      }
    }
  else if (omode & ofRead)
    how |= OF_READ;
  else
    return HFILE_ERROR;   // must specfify in, out, or in/out

  if ((fhdl = ::_lopen(_W2A(name), how)) != HFILE_ERROR) {
    if ((*pseekpos = ::_llseek(fhdl, 0, (omode & (ofAtEnd|ofAppend))
                             ? SEEK_END : SEEK_SET)) == streampos(HFILE_ERROR)) {
      ::_lclose(fhdl);
      return HFILE_ERROR;
    }
  }
  FileLength = GetFileSize((HANDLE)fhdl, 0);
  InfoPresent = GetFileTime((HANDLE)fhdl, &FileCreateTime, &FileAccessTime,
                            &FileUpdateTime);

  NotifyViews(vnDocOpened,omode);
  return fhdl;
}


//
//
//
void
TFileDocument::CloseThisFile(HFILE fhdl, int omode)
{
  if (!IS_PREV_OPEN(omode)) {
    ::_lclose(fhdl);
    NotifyViews(vnDocClosed, omode);
  }
}

//
//
//
bool
TFileDocument::Open(int omode, LPCTSTR path)
{
  if (FHdl != HFILE_ERROR)
    return false;    // if already open at document level

  if (path)
    SetDocPath(path);
  if (omode != 0)
    SetOpenMode(omode);

// gcc2.95.2-1 doesn't use template iostreams, so don't turn on BI_STD_IO;
// but it does want streampos instead of long here
#if defined (BI_STD_IO) || defined (__GNUG__)
  streampos seekpos;
#else
  long seekpos;
#endif
  if ((FHdl = OpenThisFile(GetOpenMode(),GetDocPath(),&seekpos)) == HFILE_ERROR)
    return false;
  return true;
}

//
//
//
bool
TFileDocument::Open(HFILE fhdl)
{
  SetOpenMode(PREV_OPEN | ofReadWrite); //!CQ can we determine open mode?
  SetDocPath(0);  //!CQ can we get path name?
  FHdl = fhdl;
  return true;
}

//
//
//
bool
TFileDocument::Close()
{
  if (!TDocument::Close())     // close all children first
    return false;
  if (FHdl != HFILE_ERROR) {   // if open at document level
    if (TDocument::IsOpen())   // cannot close document if streams open
      return false;       // ?should we close streams here?
    CloseThisFile(FHdl, GetOpenMode());
    FHdl = HFILE_ERROR;
  }
  return true;
}

//
//
//
bool
TFileDocument::Commit(bool force)
{
  if (!TDocument::Commit(force))      // flush views and child docs
    return false;
  SetDirty(false);
  return true;
}

//
//
//
bool
TFileDocument::Revert(bool clear)
{
  if (!TDocument::Revert(clear))
    return false;
  SetDirty(false);
  return true;
}

static LPCTSTR PropNames[] = {
  _T("Create Time"),   // CreateTime
  _T("Modify Time"),   // ModifyTime
  _T("Access Time"),   // AccessTime
  _T("Storage Size"),  // StorageSize
  _T("File Handle"),   // FileHandle
};

static int PropFlags[] = {
  pfGetBinary|pfGetText,   // CreateTime
  pfGetBinary|pfGetText,   // ModifyTime
  pfGetBinary|pfGetText,   // AccessTime
  pfGetBinary|pfGetText,   // StorageSize
  pfGetBinary,             // FileHandle
};

//
//
//
LPCTSTR
TFileDocument::PropertyName(int index)
{
  if (index <= PrevProperty)
    return TDocument::PropertyName(index);
  else if (index < NextProperty)
    return PropNames[index-PrevProperty-1];
  else
    return 0;
}

//
//
//
int
TFileDocument::PropertyFlags(int index)
{
  if (index <= PrevProperty)
    return TDocument::PropertyFlags(index);
  else if (index < NextProperty)
    return PropFlags[index-PrevProperty-1];
  else
    return 0;
}

//
//
//
int
TFileDocument::FindProperty(LPCTSTR name)
{
  int i;
  for (i=0; i < NextProperty-PrevProperty-1; i++)
    if (_tcscmp(PropNames[i], name) == 0)
      return i+PrevProperty+1;
  return TDocument::FindProperty(name);
}

//
//
//
int
TFileDocument::GetProperty(int prop, void * dest, int textlen)
{
  switch(prop) {
    case FileHandle:
      if (textlen)
        return 0;
      *(HFILE *)dest = FHdl;
      return sizeof(FHdl);
    default:
      if (InfoPresent) {
        switch(prop) {
          case StorageSize:
            if (!textlen) {
              *(unsigned long *)dest = FileLength;
              return sizeof(FileLength);
            }
            else {
              TCHAR buf[10];
              int len = _stprintf(buf, _T("%ld"), FileLength);
              if (textlen > len)
                textlen = len;
              memcpy(dest, buf, textlen*sizeof(_TCHAR));
              *((LPTSTR)dest + textlen) = 0;
              return len;
            }
          case CreateTime:
            return FormatFileTime(&FileCreateTime, dest, textlen);
          case ModifyTime:
            return FormatFileTime(&FileUpdateTime, dest, textlen);
          case AccessTime:
            return FormatFileTime(&FileAccessTime, dest, textlen);
        }
      }
      return TDocument::GetProperty(prop, dest, textlen);
  }
}

//
//
//
bool
TFileDocument::SetProperty(int prop, const void * src)
{
  // File properties currently not settable
  //
  return TDocument::SetProperty(prop, src);
}

//
//
//
TInStream*
TFileDocument::InStream(int omode, LPCTSTR /*strmId*/)
{
  HFILE fhdl;
  streampos seekpos;
  if (omode == ofParent)
    omode = GetOpenMode();
  if (!(omode & ofRead))
    return 0;
  if ((fhdl = FHdl) == HFILE_ERROR) {   // if file not open at document level
    if ((fhdl=OpenThisFile (omode, GetDocPath(), &seekpos)) == HFILE_ERROR)
      return 0;
  }
  else {
#if 0 // suggested by Greg Chicares
    omode = GetOpenMode() ? (GetOpenMode() & ~ofBinary) | (omode & ofBinary)
                          : omode;
    omode |= PREV_OPEN;
#else
     ::_lclose(FHdl);
     FHdl = HFILE_ERROR;
    if ((fhdl=OpenThisFile (omode, GetDocPath(), &seekpos)) == HFILE_ERROR)
       return 0;
#endif
  }
  return new TFileInStream(*this, fhdl, omode, seekpos);
}

//
//
//
TOutStream*
TFileDocument::OutStream(int omode, LPCTSTR /*strmId*/)
{
  HFILE fhdl;
  streampos seekpos;
  if (omode == ofParent)
    omode = GetOpenMode();
  if (!(omode & ofWrite))
    return 0;
  if ((fhdl = FHdl) == HFILE_ERROR) {   // if file not open at document level
    if ((fhdl=OpenThisFile (omode, GetDocPath(), &seekpos)) == HFILE_ERROR)
      return 0;
  }
  else {
#if 0 // suggested by Greg Chicares
    omode = GetOpenMode() ? (GetOpenMode() & ~ofBinary) | (omode & ofBinary)
                          : omode;
    omode |= PREV_OPEN;
#else
    ::_lclose(FHdl);
    FHdl = HFILE_ERROR;
    if ((fhdl=OpenThisFile (omode, GetDocPath(), &seekpos)) == HFILE_ERROR)
       return 0;
#endif
  }
  return new TFileOutStream(*this, fhdl, omode, seekpos);
}

//----------------------------------------------------------------------------

//
//
//
TFileInStream::~TFileInStream()
{
  ((TFileDocument&)Doc).CloseThisFile(buf.xfd, GetOpenMode());
}

//
//
//
TFileOutStream::~TFileOutStream()
{
  if (buf.out_waiting())
    buf.overflow(EOF);
  ((TFileDocument&)Doc).CloseThisFile(buf.xfd, GetOpenMode());
}

//----------------------------------------------------------------------------
// class TFileBuf
//

const int B_size = 516; // natural size for a file buffer, plus 4 for putback;

//
// Make a TFileBuf attached to an open fd
//
#if defined(BI_STD_IO)
TFileBuf::TFileBuf(int fhdl, int omode, streampos seekpos)
#else
TFileBuf::TFileBuf(int fhdl, int omode, long seekpos)
#endif
{
#if defined(BI_STD_IO)
  base_ = ebuf_ = 0;
#endif
  xfd = fhdl;     // assumed to be valid
  mode = omode;   // this may not represent the actual mode opened previously
  last_seek = seekpos;
  _TCHAR* p = new _TCHAR[B_size];
  if (p) {
    setb(p, p+B_size, 1);   // ~streambuf() will delete buffer
    setp(p+4, p+4);
    setg(p, p+4, p+4);
  }
}

//
// Seek file to position.
// We take a simple approach, and don't check for small position changes
// within the current buffer.
//
#if !defined(BI_STD_IO)
streampos TFileBuf::seekoff(streamoff off, ios::seek_dir dir, int /*mode*/)
#else
TFileBuf::pos_type TFileBuf::seekoff(off_type off, ios_base::seekdir dir,
                                     ios_base::openmode /*which*/)
#endif
{
  long loff = off;
  if (out_waiting()) {       // flush the output
     if (sync() == eof())
        return eof();
  }
  else if (dir == ios::cur) {
    int count = in_avail();
    if (count != 0) {
      loff -= count;

      //  if we're in text mode, need to allow for newlines
      //  in the buffer
      if ((mode & ofBinary) == 0) {
        _TCHAR *tptr = gptr();
        while (tptr != egptr())
          if (*tptr++ == _T('\n'))
            loff--;
      }
    }
  }
  int w = (dir==ios::beg) ? SEEK_SET : ((dir==ios::cur) ? SEEK_CUR:SEEK_END);
  last_seek = ::_llseek(xfd, loff, w);
  if (!unbuffered() && base()) {      // set up get and put areas
    int pb = (blen() > 8) ? 4 : 1;  // putback area size
    _TCHAR *b = base();
    setp(b+pb, b+pb);
    setg(b, b+pb, b+pb);
  }
  return (last_seek == streampos(HFILE_ERROR)) ?   streampos(eof()) : last_seek;
}

//
//
//
int TFileBuf::sync()
{
  uint count = out_waiting();
  if (count) {
    _USES_CONVERSION;
    _TCHAR* curp;
    _TCHAR* srcp = pbase();
    _TCHAR* endp = srcp + count;

    // Convert LF's to CR/LF if text mode
    //
    if ((mode & ofBinary) == 0) {
      for (curp = srcp; curp < endp; curp++) {
        if (*curp == _T('\n')){
          *curp = _T('\r');
          count = (int)(curp - srcp) + 1;
          if (::_lwrite(xfd, _W2A(srcp), count) != count)
            return eof();
          *(srcp = curp) = _T('\n');
        }
      }
      count = uint(curp - srcp);  // write what remains in the buffer below
    }
    if (::_lwrite(xfd, _W2A(srcp), count) != count)
      return eof();

    // reset get and put areas
    int     pb = (blen() > 8) ? 4 : 1;  // putback area size
    _TCHAR* b = base();
    setp(b+pb, b+blen());
    setg(b, b+pb, b+pb);
  }
  else if (in_avail()) {
    last_seek = ::_llseek(xfd, long(-in_avail()), SEEK_CUR);
    setg(eback(), gptr(), gptr());
    setp(gptr(), gptr());
    if (last_seek == streampos(HFILE_ERROR))
      return eof();
  }
  return 0;
}

//
//
//
#if !defined(BI_STD_IO)
int 
#else
TFileBuf::int_type 
#endif
TFileBuf::underflow()
{
  if ((mode & (ofRead | ofWrite)) == ofWrite)
    return eof();

  if (in_avail())    // no action needed
    return (_TCHAR)*gptr();

  int c = 0;  // the return value
  int count;  // input character count

  if (!unbuffered() && base()) {     // this is buffered
    if (sync() != 0)
      return eof();

    // find buffer data
    int pb = (blen() > 8) ? 4 : 1;  // putback area size
    _TCHAR* begp = base() + pb;

    // read in a new buffer
    count = ::_lread(xfd, begp, blen()-pb);
    if (count == HFILE_ERROR)
      return eof();

    // remove CR's if text mode
    if ((mode & ofBinary) == 0) {
      _TCHAR* endp = begp + count;
      _TCHAR* dstp = 0;
      _TCHAR* srcp = 0; // initialized only to prevent compiler warning
      _TCHAR* curp;

      for (curp = begp;  curp < endp; curp++) {
        if (*curp == _T('\r')) {
          if (dstp) {
            memcpy(dstp, srcp, (int)(curp - srcp));
            dstp += (int)(curp - srcp);
          }
          else
            dstp = curp;
          srcp = curp + 1;
        }
      }
      if (dstp) {
        endp = dstp + (int)(curp - srcp);
        if (curp != srcp)
          memcpy(dstp, srcp, (int)(curp - srcp));
      }
      count = (int)(endp - begp);
    }
    // set up get and put areas
    setg(base(), begp, begp + count);
    setp(begp, begp);

    if (count)
      c = (uchar) *gptr();
  }
  else {     // this is not buffered
    for (;;) {
      count = ::_lread(xfd, lahead, 1);
      if (count == HFILE_ERROR) {
        c = eof();
        setg(0, 0, 0);
      }
      else {
        c = (uchar)lahead[0];
        if ((mode & ofBinary) == 0  && c == _T('\r'))
          continue;
        setg(lahead, lahead, lahead+1);
      }
      break;
    }
  }
  if (!count)
    c = eof();    // end of file
  return c;
}

//
// Always flush
//
#if !defined(BI_STD_IO)
int TFileBuf::overflow(int c)
#else
TFileBuf::int_type TFileBuf::overflow(int_type c)
#endif
{
  if ((mode & (ofRead | ofWrite)) == ofRead)
    return eof();

  if (unbuffered() || !base()) {
    if (c != eof()) {
      _USES_CONVERSION;
      uint count;
      _TCHAR b[2];
      if (c == _T('\n') && (mode & ofBinary) == 0) {
        b[0] = _T('\r');
        b[1] = (_TCHAR)c;
        count = 2;
      }
      else {
        b[0] = (_TCHAR)c;
        count = 1;
      }
      if (::_lwrite(xfd, _W2A(b), count) != count)
        return eof();
    }
  }
  else {   // now we know this is buffered and state is not bad

    // resets get and put areas
    if (sync() != 0)
      return eof();

    // reset get and put areas
    int pb = (blen() > 8) ? 4 : 1;  // putback area size
    _TCHAR *b = base();
    setp(b+pb, b+blen());
    setg(b, b+pb, b+pb);

    if (c != eof()) {
      sputc(static_cast<_TCHAR>(c));
      gbump(1);       // pptr and gptr must be the same
    }
  }
  return 1;
}

#if defined(BI_STD_IO)
_tstreambuf * TFileBuf::setbuf(char_type *s, streamsize n)
{
  delete[] base_;
  base_ = s;
  ebuf_ = s + n;
  return this;
}

TFileBuf::pos_type TFileBuf::seekpos(pos_type sp,
                 ios_base::openmode which){
  return seekoff(streamoff(sp), ios::beg, which);
}
#endif

//----------------------------------------------------------------------------
// class TFileStreamBase
//

//
//
//
TFileStreamBase::TFileStreamBase(int fhdl, int omode, streampos seekpos)
:
  buf(fhdl, omode, seekpos)
{
  _tios::init(&buf);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TFileDocument, TDocument);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TFileDocument::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TFileDocument* o = GetObject();
  o->FHdl = HFILE_ERROR;    // initialize to closed file
  o->InfoPresent = false;
  ReadBaseObject((TDocument*)o, is);
  return o;
}

//
//
//
void
TFileDocument::Streamer::Write(opstream& os) const
{
  //  assumed that document is committed at this point
  WriteBaseObject((TDocument*)GetObject(), os);
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
//=========================================================================================
