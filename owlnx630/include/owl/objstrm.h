//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
// Revision by Yura Bidus
//
//$Revision: 1.31 $
//$Author: jogybl $
//$Date: 2008-05-13 09:56:33 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_OBJSTRM_H)
#define OWL_OBJSTRM_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if !defined(OWL_STREAMBL_H)
# include <owl/streambl.h>
#endif

//FMM
#if defined(__HPUX_SOURCE)
#include <owl/exbase.h>
#endif


#if !defined(BI_STD_IO)
#  include <iostream.h>
#  include <fstream.h>
#  define NS_OWL_STREAMS 
#else
# include <iostream>
# include <fstream>
#  define NS_OWL_STREAMS std 
#endif

#if !defined(BI_NO_RTTI) 
#  if !defined(BI_STD_RTTI)
#    if !defined(__TYPEINFO_H) && !defined(_INC_TYPEINFO)
#      include <typeinfo.h>
#    endif
#  else
#    if !defined(__TYPEINFO__)
#      include <typeinfo>
#    endif
#  endif
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  The __link() macro forces the compiler to link in a static instance   */
/*  of class TStreamableClass, which in turn registers its associated     */
/*  TStreamable object with the stream manager.  Applications that do     */
/*  not use streaming won't use __link(), and that will reduce the amount */
/*  of code that is linked into the application.                          */
/*                                                                        */
/*------------------------------------------------------------------------*/

struct fLink
{
    struct fLink *f;
    class TStreamableClass *t;
};

#define __link( s )             \
  extern TStreamableClass s;    \
  static fLink force ## s =     \
    { (fLink *)&force ## s, (TStreamableClass *)&s };

typedef unsigned P_id_type;

class _OWLCLASS _RTTI TStreamable;
class _OWLCLASS TStreamableTypes;
class _OWLCLASS opstream;
class _OWLCLASS ipstream;



_OWLCFUNC(ipstream &) operator >> ( ipstream & is, owl_string & str );

_OWLCFUNC(opstream &) operator << ( opstream & os,
                                          const owl_string & str );

/* -----------------------------------------------------------------------*/
/*                                                                        */
/*  _OBJ_TYPENAME(obj) provides a macro for getting the type name from a  */
/*  pointer to an object. If runtime type information is available, this  */
/*  macro uses the typeid of the object to get its name. If runtime type  */
/*  information is not available, it uses the CastableID() for the object.*/
/*                                                                        */
/* -----------------------------------------------------------------------*/


#if defined( BI_NO_RTTI )
#   define _OBJ_TYPENAME(obj) (obj)->CastableID()
#   define _OBJ_FULLTYPENAME(obj) (obj)->CastableID()
#else
#   define _OBJ_FULLTYPENAME(obj) typeid(*obj).name()
#   if defined(BI_COMP_MSC) && !defined(UNIX) && !defined(__EDG__)
#      if defined(BI_COMPATIBLE_STREAMING)
#         define _OBJ_TYPENAME(obj) typeid(*obj).name() + 6
#     else
#        define _OBJ_TYPENAME(obj) typeid(*obj).raw_name()
#     endif
#   else
#      define _OBJ_TYPENAME(obj) typeid(*obj).name()
#   endif
#endif
/* -----------------------------------------------------------------------*/
/*                                                                        */
/*  class TStreamable                                                     */
/*                                                                        */
/*  This is the base class from which all streamable objects must be      */
/*  derived.                                                              */
/*                                                                        */
/* -----------------------------------------------------------------------*/


enum StreamableInit { streamableInit };

class _OWLCLASS _RTTI TStreamableBase {
  public:
    virtual _OWLCTOR ~TStreamableBase();

#if defined( BI_NO_RTTI )
    typedef const char * /*_OWLENTRY*/ Type_id;
    virtual void *  /*_OWLENTRY*/ FindBase( Type_id id ) const;

  public:

    virtual Type_id /*_OWLENTRY*/ CastableID() const = 0;
    virtual void * /*_OWLENTRY*/ MostDerived() const = 0;
#endif

    // debug support
#if defined(_DEBUG) && !defined(BI_OWL_NO_DEBUG_CRT)
  // for file name/line number tracking using DEBUG_NEW
  void* PASCAL operator new(size_t nSize)
    {  return _malloc_dbg(nSize, _CLIENT_BLOCK, __FILE__, __LINE__);  }
  void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
    {  return _malloc_dbg(nSize, _CLIENT_BLOCK, lpszFileName, nLine); }
  void* PASCAL operator new(size_t, void *p) // placement new for STL
    {  return p; }

  void PASCAL operator delete(void* p)
    {  _free_dbg(p, _CLIENT_BLOCK); }

  void PASCAL operator delete(void*, void*) // placement delete for STL
    {  ; }
#if _MSC_VER >= 1200
  void PASCAL operator delete(void *p, LPCSTR /*lpszFileName*/, int /*nLine*/)
    {  _free_dbg(p, _CLIENT_BLOCK); }
#endif

#endif

};


class _OWLCLASS _RTTI TStreamable : public TStreamableBase {

    friend class _OWLCLASS _RTTI TOldStreamer;

  protected:

    virtual LPCSTR _OWLENTRY streamableName() const = 0;

    virtual void * _OWLENTRY read( ipstream& ) = 0;
    virtual void _OWLENTRY write( opstream& )  = 0;

#if defined( BI_NO_RTTI )
  public:
    virtual void * /*_OWLENTRY*/ FindBase( Type_id id ) const;
  protected:
    virtual Type_id /*_OWLENTRY*/ CastableID() const { return streamableName(); }
    virtual void * /*_OWLENTRY*/ MostDerived() const { return 0; }
#endif
};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif

class _OWLCLASS _RTTI TStreamer {

    friend class ipstream;
    friend class opstream;

  public:

    TStreamableBase * _OWLENTRY GetObject() const { return object; }

  protected:

    TStreamer( TStreamableBase *obj ) : object(obj) {}

    virtual LPCSTR _OWLENTRY StreamableName() const = 0;

    virtual void * _OWLENTRY Read( ipstream&, uint32 ) const = 0;
    virtual void _OWLENTRY Write( opstream& ) const = 0;

  private:

    virtual uint32 _OWLENTRY ClassVersion() const = 0;

    TStreamableBase *object;

};

class _OWLCLASS _RTTI TOldStreamer : public TStreamer {

  public:

    _OWLCTOR TOldStreamer( TStreamable *obj ) : TStreamer(obj) {};

  protected:

    virtual LPCSTR _OWLENTRY StreamableName() const
        {
        return STATIC_CAST(TStreamable *,GetObject())->streamableName();
        }

    virtual void * _OWLENTRY Read( ipstream& is, uint32 ) const
        {
        return STATIC_CAST(TStreamable *,GetObject())->read( is );
        }

    virtual void _OWLENTRY Write( opstream& os ) const
        {
        STATIC_CAST(TStreamable *,GetObject())->write( os );
        }

  private:

    virtual uint32 _OWLENTRY ClassVersion() const
        {
        return 0;
        }

};

class _OWLCLASS _RTTI TNewStreamer : public TStreamer {

  public:

    _OWLCTOR TNewStreamer( TStreamableBase *obj ) : TStreamer(obj) {};

  protected:

    virtual LPCSTR _OWLENTRY StreamableName() const
        {
        return _OBJ_TYPENAME(GetObject());
        }

};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPWrittenObjects                                                */
/*                                                                         */
/*   Maintains a database of all objects that have been written to the     */
/*   current persistent stream.                                            */
/*                                                                         */
/*   Used by opstream when it writes a pointer from a stream to save the   */
/*   address and ID of the object being written.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class  TSortedTPWObjObjectArray;

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif

class _OWLCLASS TPWrittenObjects {

    friend class opstream;

  public:

    void _OWLENTRY RemoveAll();

    class _OWLCLASS TPWObj {
      public:

        _OWLCTOR TPWObj() : Address(0), Ident(0) {}
        _OWLCTOR TPWObj( const void *adr, P_id_type id ) :
                        Address(adr), Ident(id) {}

        bool _OWLENTRY operator == ( const TPWObj& o) const
            { return TAddrInt(Address) == TAddrInt(o.Address); }

        bool _OWLENTRY operator < ( const TPWObj& o ) const
            { return TAddrInt(Address) < TAddrInt(o.Address); }

        const void *Address;
        P_id_type Ident;
        typedef uint32 TAddrInt;
    };

  private:

    _OWLCTOR TPWrittenObjects();
    _OWLCTOR ~TPWrittenObjects();

    void _OWLENTRY RegisterObject( TStreamableBase *adr );
    void _OWLENTRY RegisterVB( const TStreamableBase *adr );
    P_id_type _OWLENTRY FindObject( TStreamableBase *adr );
    P_id_type _OWLENTRY FindVB( TStreamableBase *adr );

    P_id_type                   CurId;
    TSortedTPWObjObjectArray*   Data;
};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPReadObjects                                                   */
/*                                                                         */
/*   Maintains a database of all objects that have been read from the      */
/*   current persistent stream.                                            */
/*                                                                         */
/*   Used by ipstream when it reads a pointer from a stream to determine   */
/*   the address of the object being referred to.                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class TStreamableBaseArray;

class _OWLCLASS TPReadObjects {
    friend class ipstream;
  public:

    void _OWLENTRY RemoveAll();

  private:
    _OWLCTOR TPReadObjects();
    _OWLCTOR ~TPReadObjects();

    void _OWLENTRY RegisterObject( TStreamableBase *adr );
    TStreamableBase * _OWLENTRY Find( P_id_type id );

    TStreamableBaseArray* Data;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class pstream                                                         */
/*                                                                         */
/*   Base class for handling streamable objects.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class _OWLCLASS pstream {
    friend class TStreamableTypes;
    friend class TStreamableClass;
  public:

    enum PointerTypes { ptNull, ptIndexed, ptObject };

    _OWLCTOR pstream( NS_OWL_STREAMS::streambuf * );
    virtual _OWLCTOR ~pstream();

    int _OWLENTRY rdstate() const;
    int _OWLENTRY eof() const;
    int _OWLENTRY fail() const;
    int _OWLENTRY bad() const;
    int _OWLENTRY good() const;
    void _OWLENTRY clear( int = 0 );
    _OWLENTRY operator void *() const;
    int _OWLENTRY operator ! () const;

    NS_OWL_STREAMS::streambuf * _OWLENTRY rdbuf() const;

    static void _OWLENTRY initTypes();
    static void _OWLENTRY releaseTypes();

    static void _OWLENTRY registerType( TStreamableClass *ts );

  protected:

    _OWLCTOR pstream();

    NS_OWL_STREAMS::streambuf *bp;
    int state;

    void _OWLENTRY init( NS_OWL_STREAMS::streambuf * );
    void _OWLENTRY setstate( int );

//    static TStreamableTypes *types;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ipstream                                                        */
/*                                                                         */
/*   Base class for reading streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/
inline ipstream& _OWLENTRY operator >> ( ipstream&, int8& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, uint8& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, char& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, signed short& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, unsigned short& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, signed int& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, unsigned int& );
#if defined(BI_UNIQUE_BOOL)
inline ipstream& _OWLENTRY operator >> ( ipstream&, bool& );
#endif
inline ipstream& _OWLENTRY operator >> ( ipstream&, signed long& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, unsigned long& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, float& );
inline ipstream& _OWLENTRY operator >> ( ipstream&, double&);
inline ipstream& _OWLENTRY operator >> ( ipstream&, long double&);


class _OWLCLASS ipstream : virtual public pstream {
    friend class TStreamableClass;
  public:

    _OWLCTOR ipstream( NS_OWL_STREAMS::streambuf * );

    NS_OWL_STREAMS::streampos _OWLENTRY tellg();
    ipstream& _OWLENTRY seekg( NS_OWL_STREAMS::streampos );
    ipstream& _OWLENTRY seekg( NS_OWL_STREAMS::streamoff, NS_OWL_STREAMS::ios::seek_dir );

    uint8 _OWLENTRY readByte();
    void _OWLENTRY readBytes( void *, size_t );
    void _OWLENTRY freadBytes( void * data, size_t sz );

    uint32 _OWLENTRY readWord();
    uint16 _OWLENTRY readWord16();
    uint32 _OWLENTRY readWord32();

    LPSTR _OWLENTRY readString();
    LPSTR _OWLENTRY readString( LPSTR , unsigned );
    char  * _OWLENTRY freadString();
    char  * _OWLENTRY freadString( char *buf,
                                            unsigned maxLen );

    friend ipstream& _OWLENTRY operator >> ( ipstream&, int8& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, uint8& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, char& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, signed short& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, unsigned short& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, signed int& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, unsigned int& );
#if defined(BI_UNIQUE_BOOL)
    friend ipstream& _OWLENTRY operator >> ( ipstream&, bool& );
#endif
    friend ipstream& _OWLENTRY operator >> ( ipstream&, signed long& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, unsigned long& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, float& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, double& );
    friend ipstream& _OWLENTRY operator >> ( ipstream&, long double& );
    friend ipstream &   operator >> ( ipstream&, owl_string&);

    uint32 _OWLENTRY getVersion() const;

    TStreamableBase * _OWLENTRY readObject( TStreamableBase *&mem, ModuleId mid = GetModuleId() );
    TStreamableBase * _OWLENTRY readObjectPointer( TStreamableBase *&mem, ModuleId mid = GetModuleId() );

    TStreamableBase * _OWLENTRY find( P_id_type );
    void _OWLENTRY registerObject( TStreamableBase *adr );

  protected:

    _OWLCTOR ipstream();

    const ObjectBuilder * _OWLENTRY readPrefix( ModuleId mid );
    void _OWLENTRY readData( const ObjectBuilder *,
                          TStreamableBase *& );
    void _OWLENTRY readSuffix();

    void _OWLENTRY readVersion();

private:

    uint32 readStringLength();
    TPReadObjects objs;
    uint32 version;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/
inline opstream& _OWLENTRY operator << ( opstream&, int8 );
inline opstream& _OWLENTRY operator << ( opstream&, uint8 );
inline opstream& _OWLENTRY operator << ( opstream&, char );
inline opstream& _OWLENTRY operator << ( opstream&, signed short );
inline opstream& _OWLENTRY operator << ( opstream&, unsigned short );
inline opstream& _OWLENTRY operator << ( opstream&, signed int );
inline opstream& _OWLENTRY operator << ( opstream&, unsigned int );
#if defined(BI_UNIQUE_BOOL)
inline opstream& _OWLENTRY operator << ( opstream&, bool );
#endif
inline opstream& _OWLENTRY operator << ( opstream&, signed long );
inline opstream& _OWLENTRY operator << ( opstream&, unsigned long );
inline opstream& _OWLENTRY operator << ( opstream&, float );
inline opstream& _OWLENTRY operator << ( opstream&, double );
inline opstream& _OWLENTRY operator << ( opstream&, long double );

class _OWLCLASS opstream : virtual public pstream {
  public:

    _OWLCTOR opstream( NS_OWL_STREAMS::streambuf * );
    virtual _OWLCTOR ~opstream();

    NS_OWL_STREAMS::streampos _OWLENTRY tellp();
    opstream& _OWLENTRY seekp( NS_OWL_STREAMS::streampos );
    opstream& _OWLENTRY seekp( NS_OWL_STREAMS::streamoff, NS_OWL_STREAMS::ios::seek_dir );
    opstream& _OWLENTRY flush();

    void _OWLENTRY writeByte( uint8 );
    void _OWLENTRY writeBytes( const void *, size_t );
    void _OWLENTRY fwriteBytes( const void *data, size_t sz );

    void _OWLENTRY writeWord( uint32 );
    void _OWLENTRY writeWord16( uint16 );
    void _OWLENTRY writeWord32( uint32 );

    void _OWLENTRY writeString( const char * );
    void _OWLENTRY fwriteString( const char * str );

    friend opstream& _OWLENTRY operator << ( opstream&, int8 );
    friend opstream& _OWLENTRY operator << ( opstream&, uint8 );
    friend opstream& _OWLENTRY operator << ( opstream&, char );
    friend opstream& _OWLENTRY operator << ( opstream&, signed short );
    friend opstream& _OWLENTRY operator << ( opstream&, unsigned short );
    friend opstream& _OWLENTRY operator << ( opstream&, signed int );
    friend opstream& _OWLENTRY operator << ( opstream&, unsigned int );
#if defined(BI_UNIQUE_BOOL)
    friend opstream& _OWLENTRY operator << ( opstream&, bool );
#endif
    friend opstream& _OWLENTRY operator << ( opstream&, signed long );
    friend opstream& _OWLENTRY operator << ( opstream&, unsigned long );
    friend opstream& _OWLENTRY operator << ( opstream&, float );
    friend opstream& _OWLENTRY operator << ( opstream&, double );
    friend opstream& _OWLENTRY operator << ( opstream&, long double );

    void _OWLENTRY writeObject( const TStreamableBase *t, int isPtr = 0, ModuleId mid = GetModuleId() );
    void _OWLENTRY writeObjectPointer( const TStreamableBase *t, ModuleId mid = GetModuleId() );

    P_id_type _OWLENTRY findObject( TStreamableBase *adr );
    void _OWLENTRY registerObject( TStreamableBase *adr );

    P_id_type _OWLENTRY findVB( TStreamableBase *adr );
    void _OWLENTRY registerVB( TStreamableBase *adr );

  protected:

    _OWLCTOR opstream();

    void _OWLENTRY writePrefix( const TStreamableBase * );
    void _OWLENTRY writeData( const TStreamableBase *, ModuleId mid );
    void _OWLENTRY writeSuffix( const TStreamableBase * );

    void _OWLENTRY writeVersion();

  private:

    TPWrittenObjects* objs;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class _OWLCLASS fpbase : virtual public pstream {
  public:

    enum { openprot = 0666 }; // default open mode
    _OWLCTOR fpbase();
    _OWLCTOR fpbase( LPCSTR, int, int = openprot );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    _OWLCTOR fpbase( LPCWSTR, int, int = openprot );
#endif
    
#if defined(BI_STDIO_HASFILECNT)
    _OWLCTOR fpbase( __file_id );
#endif
#if defined(BI_STDIO_CTRFSI)
    _OWLCTOR fpbase(__file_id, LPSTR, int );
#endif

    void _OWLENTRY open( LPCSTR, int, int = openprot );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    void _OWLENTRY open( LPCWSTR, int, int = openprot );
#endif    
#if defined(BI_COMP_BORLANDC)
    void _OWLENTRY attach( __file_id );
#endif
    void _OWLENTRY close();
    void _OWLENTRY setbuf( LPSTR, int );
    NS_OWL_STREAMS::filebuf * _OWLENTRY rdbuf();

  private:

    NS_OWL_STREAMS::filebuf buf;
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ifpstream                                                       */
/*                                                                         */
/*   Base class for reading streamable objects from file streams           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class _OWLCLASS ifpstream : public fpbase, public ipstream {
  public:

    _OWLCTOR ifpstream();
    _OWLCTOR ifpstream( LPCSTR,
                      int = NS_OWL_STREAMS::ios::in,
                      int = fpbase::openprot
                    );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    _OWLCTOR ifpstream( LPCWSTR,
                      int = NS_OWL_STREAMS::ios::in,
                      int = fpbase::openprot
                    );
#endif                    
#if defined(BI_STDIO_HASFILECNT)
    _OWLCTOR ifpstream( __file_id );
#endif
#if defined(BI_STDIO_CTRFSI)
    _OWLCTOR ifpstream( __file_id, LPSTR, int );
#endif

    NS_OWL_STREAMS::filebuf * _OWLENTRY rdbuf();
    	
    void _OWLENTRY open( LPCSTR ,
                      int = NS_OWL_STREAMS::ios::in,
                      int = fpbase::openprot
                    );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    void _OWLENTRY open( LPCWSTR ,
                      int = NS_OWL_STREAMS::ios::in,
                      int = fpbase::openprot
                    );
#endif                   
};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class _OWLCLASS ofpstream : public fpbase, public opstream {
  public:

    _OWLCTOR ofpstream();
    _OWLCTOR ofpstream( LPCSTR ,
                      int = NS_OWL_STREAMS::ios::out,
                      int = fpbase::openprot
                    );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    _OWLCTOR ofpstream( LPCWSTR ,
                      int = NS_OWL_STREAMS::ios::out,
                      int = fpbase::openprot
                    );
#endif                    
#if defined(BI_STDIO_HASFILECNT)
    _OWLCTOR ofpstream( __file_id );
#endif
#if defined(BI_STDIO_CTRFSI)
    _OWLCTOR ofpstream( __file_id, LPSTR , int );
#endif

    NS_OWL_STREAMS::filebuf * _OWLENTRY rdbuf();

    void _OWLENTRY open( LPCSTR,
                      int = NS_OWL_STREAMS::ios::out,
                      int = fpbase::openprot
                    );
#if defined(BI_HAS_STREAMWCHAROPEN)    
    void _OWLENTRY open( LPCWSTR,
                      int = NS_OWL_STREAMS::ios::out,
                      int = fpbase::openprot
                    );
#endif
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   Inline functions                                                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR pstream::pstream( NS_OWL_STREAMS::streambuf *sb ){
  init( sb );
}

inline int _OWLENTRY pstream::rdstate() const {
  return state;
}

inline int _OWLENTRY pstream::eof() const {
  return state & NS_OWL_STREAMS::ios::eofbit;
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



#if !defined(hardfail) && (defined(BI_COMP_MSC) || defined(BI_COMP_GNUC) || defined(BI_STD_IO))
# define hardfail    goodbit   // MSC ios does not support hardfail, use goodbit (0)
#endif

inline int _OWLENTRY pstream::fail() const {
  return state & (NS_OWL_STREAMS::ios::failbit | NS_OWL_STREAMS::ios::badbit | NS_OWL_STREAMS::ios::hardfail);
}

inline int _OWLENTRY pstream::bad() const {
  return state & (NS_OWL_STREAMS::ios::badbit | NS_OWL_STREAMS::ios::hardfail);
}

inline int _OWLENTRY pstream::good() const {
  return state == 0;
}

inline void _OWLENTRY pstream::clear( int i ){
  state = (i & 0xFF) | (state & NS_OWL_STREAMS::ios::hardfail);
}

#if defined(BI_COMP_MSC) || defined(BI_STD_IO) || defined(BI_COMP_GNUC) 
# undef hardfail
#endif

inline _OWLENTRY pstream::operator void *() const {
  return fail() ? 0 : (void *)this;
}

inline int _OWLENTRY pstream::operator! () const {
  return fail();
}

inline NS_OWL_STREAMS::streambuf * _OWLENTRY pstream::rdbuf() const {
  return bp;
}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR pstream::pstream(){
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

inline void _OWLENTRY pstream::init( NS_OWL_STREAMS::streambuf *sbp ){
  state = 0;
  bp = sbp;
}

inline void _OWLENTRY pstream::setstate( int b ){
  state |= (b&0xFF);
}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR ipstream::ipstream( NS_OWL_STREAMS::streambuf *sb ){
  pstream::init( sb );
  readVersion();
}

inline _OWLCTOR ipstream::ipstream(){
  if( bp != 0 )
    readVersion();
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

inline TStreamableBase * _OWLENTRY ipstream::find( P_id_type id ){
  return objs.Find( id );
}

inline void _OWLENTRY ipstream::registerObject( TStreamableBase *adr ){
  objs.RegisterObject( adr );
}

inline uint32 _OWLENTRY ipstream::getVersion() const {
  return version;
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR opstream::~opstream(){
  delete objs;
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

inline void _OWLENTRY opstream::writeWord( uint32 word32 ){
  writeWord32( word32 );
}

inline void _OWLENTRY opstream::writeSuffix( const TStreamableBase * ){
  writeByte( ']' );
}

inline P_id_type _OWLENTRY opstream::findObject( TStreamableBase *adr ){
  return objs->FindObject( adr );
}

inline void _OWLENTRY opstream::registerObject( TStreamableBase *adr ){
  objs->RegisterObject( adr );
}

inline P_id_type _OWLENTRY opstream::findVB( TStreamableBase *adr ){
  return objs->FindVB( adr );
}

inline void _OWLENTRY opstream::registerVB( TStreamableBase *adr ){
  objs->RegisterVB( adr );
}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR fpbase::fpbase(){
  pstream::init( &buf );
}

inline _OWLCTOR fpbase::fpbase( LPCSTR name, int omode, int prot ){
  pstream::init( &buf );
  open( name, omode, prot );
}

#if defined(BI_HAS_STREAMWCHAROPEN)
inline _OWLCTOR fpbase::fpbase( LPCWSTR name, int omode, int prot ){
  pstream::init( &buf );
  open( name, omode, prot );
}
#endif

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

#if defined(BI_STDIO_HASFILECNT)
inline _OWLCTOR fpbase::fpbase( __file_id f )  : buf( f )
{
  pstream::init( &buf );
}
#endif


#if defined(BI_STDIO_CTRFSI)
inline _OWLCTOR fpbase::fpbase( __file_id f, LPSTR b, int len )
 : buf( f, b, len )
{
  pstream::init( &buf );
}
#endif

inline NS_OWL_STREAMS::filebuf * _OWLENTRY fpbase::rdbuf(){
  return &buf;
}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR ifpstream::ifpstream(){
}

inline _OWLCTOR ifpstream::ifpstream( LPCSTR name, int omode, int prot )
:
  fpbase( name, omode | NS_OWL_STREAMS::ios::in | NS_OWL_STREAMS::ios::binary, prot )
{
}

#if defined(BI_HAS_STREAMWCHAROPEN)
inline _OWLCTOR ifpstream::ifpstream( LPCWSTR name, int omode, int prot )
:
  fpbase( name, omode | NS_OWL_STREAMS::ios::in | NS_OWL_STREAMS::ios::binary, prot )
{
}
#endif

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

#if defined(BI_STDIO_HASFILECNT)
inline _OWLCTOR ifpstream::ifpstream( __file_id f ) : fpbase( f )
{}
#endif

#if defined(BI_STDIO_CTRFMSI)
inline _OWLCTOR ifpstream::ifpstream(__file_id f, LPSTR b, int len)
:
  fpbase(f, b, len)
{
}
#endif

inline NS_OWL_STREAMS::filebuf * _OWLENTRY ifpstream::rdbuf(){
  return fpbase::rdbuf();
}

inline void _OWLENTRY ifpstream::open( LPCSTR name,
                                    int omode,
                                    int prot )
{
  fpbase::open( name, omode | NS_OWL_STREAMS::ios::in | NS_OWL_STREAMS::ios::binary, prot );
  readVersion();
}

#if defined(BI_HAS_STREAMWCHAROPEN)
inline void _OWLENTRY ifpstream::open( LPCWSTR name,
                                    int omode,
                                    int prot )
{
  fpbase::open( name, omode | NS_OWL_STREAMS::ios::in | NS_OWL_STREAMS::ios::binary, prot );
  readVersion();
}
#endif

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline _OWLCTOR ofpstream::ofpstream(){
}

inline _OWLCTOR ofpstream::ofpstream( LPCSTR name, int omode, int prot )
:
  fpbase( name, omode | NS_OWL_STREAMS::ios::out | NS_OWL_STREAMS::ios::binary, prot )
{
}

#if defined(BI_HAS_STREAMWCHAROPEN)
inline _OWLCTOR ofpstream::ofpstream( LPCWSTR name, int omode, int prot )
:
  fpbase( name, omode | NS_OWL_STREAMS::ios::out | NS_OWL_STREAMS::ios::binary, prot )
{
}
#endif

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

#if defined(BI_STDIO_HASFILECNT)
inline _OWLCTOR ofpstream::ofpstream( __file_id f ) : fpbase(f)
{}
#endif

#if defined(BI_STDIO_CTRFSI)
inline _OWLCTOR ofpstream::ofpstream(int f, LPSTR b, int len)
:
  fpbase(f, b, len)
{
}
#endif

inline NS_OWL_STREAMS::filebuf * _OWLENTRY ofpstream::rdbuf(){
  return fpbase::rdbuf();
}

inline void _OWLENTRY ofpstream::open( LPCSTR name,
                                    int omode,
                                    int prot )
{
  fpbase::open( name, omode | NS_OWL_STREAMS::ios::out | NS_OWL_STREAMS::ios::binary, prot );
  writeVersion();
}

#if defined(BI_HAS_STREAMWCHAROPEN)
inline void _OWLENTRY ofpstream::open( LPCWSTR name,
                                    int omode,
                                    int prot )
{
  fpbase::open( name, omode | NS_OWL_STREAMS::ios::out | NS_OWL_STREAMS::ios::binary, prot );
  writeVersion();
}
#endif

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, int8& ch )
{
  ch = ps.readByte();
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, uint8& ch ){
  ch = ps.readByte();
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, char &ch ){
  ch = ps.readByte();
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, signed short &sh ){
  sh = ps.readWord16();
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, unsigned short &sh ){
  sh = ps.readWord16();
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, signed int &i ){
  i = (int)(ps.readWord());
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, unsigned int &i ){
  i = (unsigned int)(ps.readWord());
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, signed long &l ){
  ps.readBytes( &l, sizeof(l) );
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, unsigned long &l ){
  ps.readBytes( &l, sizeof(l) );
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, float &f ){
  ps.readBytes( &f, sizeof(f) );
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, double &d ){
  ps.readBytes( &d, sizeof(d) );
  return ps;
}

inline ipstream& _OWLENTRY operator >> ( ipstream& ps, long double &l ){
  ps.readBytes( &l, sizeof(l) );
  return ps;
}

#if defined(BI_UNIQUE_BOOL)
inline ipstream& _OWLENTRY operator >> ( ipstream& ps, bool &b ){
  b = static_cast<bool>(ps.readWord32());
  return ps;
}
#endif

inline opstream& _OWLENTRY operator << ( opstream& ps, int8 ch ){
  ps.writeByte( ch );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, uint8 ch ){
  ps.writeByte( ch );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, char ch ){
  ps.writeByte( ch );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, signed short sh ){
  ps.writeWord16( sh );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, unsigned short sh ){
  ps.writeWord16( sh );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, signed int i ){
  ps.writeWord32( i );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, unsigned int i ){
  ps.writeWord32( i );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, signed long l ){
  ps.writeBytes( &l, sizeof(l) );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, unsigned long l ){
  ps.writeBytes( &l, sizeof(l) );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, float f ){
  ps.writeBytes( &f, sizeof(f) );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, double d ){
  ps.writeBytes( &d, sizeof(d) );
  return ps;
}

inline opstream& _OWLENTRY operator << ( opstream& ps, long double l ){
  ps.writeBytes( &l, sizeof(l) );
  return ps;
}

#if defined(BI_UNIQUE_BOOL)
inline opstream& _OWLENTRY operator << ( opstream& ps, bool b ){
  ps.writeWord32( b );
  return ps;
}
#endif

#if defined(BI_NO_TYPENAME)
#define Base_Streamer Base::Streamer
#endif

template <class Base> void WriteBaseObject( Base *base, opstream& out )
{
#if !defined(BI_NO_TYPENAME)
 typedef typename Base::Streamer Base_Streamer;
#endif
  Base_Streamer strmr(base);
  out << strmr.ClassVersion();
  strmr.Write( out );
}

template <class Base> void ReadBaseObject( Base *base, ipstream& in )
{
#if !defined(BI_NO_TYPENAME)
 typedef typename Base::Streamer Base_Streamer;
#endif
  uint32 version = 0;
  if( in.getVersion() > 0 )
    in >> version;
  Base_Streamer(base).Read( in, version );
}

template <class Base> void WriteVirtualBase( Base *base, opstream& out )
{
#if !defined(BI_NO_TYPENAME)
 typedef typename Base::Streamer Base_Streamer;
#endif

 if( !out.good() )
    return;
  if( out.findVB( base ) != 0 ){
    out.writeByte( pstream::ptIndexed );    // use ptIndexed to indicate
                                            // that we've already seen
                                            // this virtual base. Don't
                                            // need to actually write it.
  }
  else{
    Base_Streamer strmr(base);
    out.registerObject( (TStreamableBase *)((char*)base + 1) );
    out.writeByte( pstream::ptObject );
    out.writeWord32( strmr.ClassVersion() );
    strmr.Write( out );
  }
}

template <class Base> void ReadVirtualBase( Base *base, ipstream& in )
{
#if !defined(BI_NO_TYPENAME)
 typedef typename Base::Streamer Base_Streamer;
#endif
  char ch;
  in >> ch;
  switch( ch ){
    case pstream::ptIndexed:
      break;      // We've already read this virtual base
    case pstream::ptObject: {
        uint32 ver = 0;
        if( in.getVersion() > 0 )
          ver = in.readWord32();
        Base_Streamer strmr(base);
        // register the address
        in.registerObject(strmr.GetObject());
        strmr.Read( in, ver );
      }
      break;
  }
}

//
//  Individual Components for Streamable Declarations
//

// When using namespaces, the friend declarations need to name
// their scope explicitly.
//

// Used to paste tokens when one argument is itself a macro
// that needs to be processed before the ## operation
//
#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#  define INNER_PASTE(a,b) a ## b
#  define PASTE(a, b) INNER_PASTE(a,b)
#endif

#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_STREAMERS(cls, ver )                                \
public:                                                             \
    class Streamer : public NS_OWL::TNewStreamer                    \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                   \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;              \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;      \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;                                          \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& );\
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& );\
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_STREAMERS(cls, ver )                                \
public:                                                             \
    class Streamer : public NS_OWL::TNewStreamer                    \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                   \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;              \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;      \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;                                          
#endif


#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_STREAMER( exp, cls, ver )                           \
public:                                                             \
    class exp Streamer : public NS_OWL::TNewStreamer                \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                   \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;              \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;      \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;                                      \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& ); \
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_STREAMER( exp, cls, ver )                           \
public:                                                             \
    class exp Streamer : public NS_OWL::TNewStreamer                \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                   \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;              \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;      \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;                                      
#endif

#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_STREAMER_FROM_BASES(cls, base )                     \
public:                                                             \
    class Streamer : public base::Streamer                          \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj ) : base::Streamer(obj){} \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;                                                \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& );   \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& );  \
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& );  \
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_STREAMER_FROM_BASES(cls, base )                     \
public:                                                             \
    class Streamer : public base::Streamer                          \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj ) : base::Streamer(obj){} \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj ) \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;
#endif


#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_STREAMER_FROM_BASE( exp, cls, base )                \
public:                                                             \
    class exp Streamer : public base::Streamer                      \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj ) : base::Streamer(obj){}    \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj )             \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;                                      \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& );\
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& );\
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_STREAMER_FROM_BASE( exp, cls, base )                \
public:                                                             \
    class exp Streamer : public base::Streamer                      \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj ) : base::Streamer(obj){}    \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        static NS_OWL::TStreamer *Build( NS_OWL::TStreamableBase *obj )             \
            {                                                       \
            return new Streamer( obj ? obj : new cls(NS_OWL::streamableInit) ); \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;
#endif

#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_ABSTRACT_STREAMERS(cls, ver )                       \
public:                                                             \
    class Streamer : public NS_OWL::TNewStreamer                            \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                           \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;                      \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;              \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;                                          \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& );\
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& );\
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_ABSTRACT_STREAMERS(cls, ver )                       \
public:                                                             \
    class Streamer : public NS_OWL::TNewStreamer                            \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                           \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;                      \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;              \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class Streamer;
#endif

#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_ABSTRACT_STREAMER( exp, cls, ver )                  \
public:                                                             \
    class exp Streamer : public NS_OWL::TNewStreamer                        \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                           \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;                      \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;              \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;                                      \
    friend void PASTE(__OWL, ReadBaseObject)( cls *, NS_OWL::ipstream& ); \
    friend void PASTE(__OWL, WriteBaseObject)( cls *, NS_OWL::opstream& );\
    friend void PASTE(__OWL, ReadVirtualBase)( cls *, NS_OWL::ipstream& );\
    friend void PASTE(__OWL, WriteVirtualBase)( cls *, NS_OWL::opstream& )
#else
#define DECLARE_ABSTRACT_STREAMER( exp, cls, ver )                  \
public:                                                             \
    class exp Streamer : public NS_OWL::TNewStreamer                        \
        {                                                           \
        public:                                                     \
                                                                    \
        Streamer( NS_OWL::TStreamableBase *obj );                           \
                                                                    \
        virtual uint32 ClassVersion() const                         \
            { return ver; }                                         \
                                                                    \
        virtual void Write( NS_OWL::opstream& ) const;                      \
        virtual void *Read( NS_OWL::ipstream&, uint32 ) const;              \
                                                                    \
        cls *GetObject() const                                      \
            {                                                       \
            return object;                                          \
            }                                                       \
                                                                    \
        private:                                                    \
            cls *object;                                            \
                                                                    \
        };                                                          \
    friend class exp Streamer;
#endif


#if defined(BI_SUPPORT_TMPLFNC_FRIEND)
#define DECLARE_STREAMABLE_OPS( cls )                               \
static NS_OWL::ipstream& readRef( NS_OWL::ipstream& is, cls& cl );                  \
friend inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls& cl )       \
    { return cls::readRef( is, cl ); }                              \
static NS_OWL::ipstream& readPtr( NS_OWL::ipstream& is, cls*& cl );                 \
friend inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls*& cl )      \
    { return cls::readPtr( is, cl ); }                              \
static NS_OWL::opstream& writeRef( NS_OWL::opstream& is, const cls& cl );           \
friend inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls& cl ) \
    { return cls::writeRef( os, cl ); }                             \
static NS_OWL::opstream& writePtr( NS_OWL::opstream& is, const cls* cl );           \
friend inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls* cl ) \
    { return cls::writePtr( os, cl ); }

#define DECLARE_STREAMABLE_INLINES_I( cls )

#else // else BI_SUPPORT_TMPLFNC_FRIEND

#define DECLARE_STREAMABLE_OPS( cls )                               \
static NS_OWL::ipstream& readRef( NS_OWL::ipstream& is, cls& cl );                  \
friend inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls& cl );      \
static NS_OWL::ipstream& readPtr( NS_OWL::ipstream& is, cls*& cl );                 \
friend inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls*& cl );     \
static NS_OWL::opstream& writeRef( NS_OWL::opstream& is, const cls& cl );           \
friend inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls& cl );\
static NS_OWL::opstream& writePtr( NS_OWL::opstream& is, const cls* cl );           \
friend inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls* cl );

#define DECLARE_STREAMABLE_INLINES_I( cls )                           \
inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls& cl )              \
    { return cls::readRef( is, cl ); }                              \
inline NS_OWL::ipstream& operator >> ( NS_OWL::ipstream& is, cls*& cl )             \
    { return cls::readPtr( is, cl ); }                              \
inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls& cl )        \
    { return cls::writeRef( os, cl ); }                             \
inline NS_OWL::opstream& operator << ( NS_OWL::opstream& os, const cls* cl )        \
    { return cls::writePtr( os, cl ); }

#endif

#define DECLARE_STREAMABLE_CTOR( cls )                              \
public:                                                             \
    cls ( NS_OWL::StreamableInit )

//
// Castable declaration macros
//
#if defined( BI_NO_RTTI )
# define DECLARE_CASTABLE                                           \
public:                                                             \
    virtual void *FindBase( Type_id id ) const;                     \
public:                                                             \
    virtual Type_id CastableID() const;                             \
    virtual void *MostDerived() const { return (void *)this; }      \
    static Type_id CastableIdent
#else
# define DECLARE_CASTABLE friend class __OWL_TYPEINFO
#endif

//
// Streamable declaration macros
//
#if !defined(BI_NO_OBJ_STREAMING)

#define DECLARE_STREAMABLE( exp, cls, ver )                         \
    DECLARE_CASTABLE ;                                              \
    DECLARE_STREAMER( exp, cls, ver );                              \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

#define DECLARE_STREAMABLES(cls, ver )                              \
    DECLARE_CASTABLE ;                                              \
    DECLARE_STREAMERS(cls, ver );                                   \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

#define DECLARE_STREAMABLE_FROM_BASE( exp, cls, base )              \
    DECLARE_CASTABLE;                                               \
    DECLARE_STREAMER_FROM_BASE( exp, cls, base );                   \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

#define DECLARE_STREAMABLE_FROM_BASES(cls, base )                   \
    DECLARE_CASTABLE;                                               \
    DECLARE_STREAMER_FROM_BASES(cls, base );                        \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

#define DECLARE_ABSTRACT_STREAMABLE( exp, cls, ver )                \
    DECLARE_CASTABLE;                                               \
    DECLARE_ABSTRACT_STREAMER( exp, cls, ver );                     \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

#define DECLARE_ABSTRACT_STREAMABLES(cls, ver )                     \
    DECLARE_CASTABLE;                                               \
    DECLARE_ABSTRACT_STREAMERS(cls, ver );                          \
    DECLARE_STREAMABLE_OPS( cls );                                  \
    DECLARE_STREAMABLE_CTOR( cls )

// only for OWL
#if defined(_BUILDOWLDLL) || defined(_OWLDLL)
#  define DECLARE_STREAMABLE_OWL( cls, ver )                     \
    DECLARE_STREAMABLE( _OWLCLASS, cls, ver )
# define DECLARE_STREAMABLE_FROM_BASE_OWL( cls, base )          \
    DECLARE_STREAMABLE_FROM_BASE( _OWLCLASS, cls, base )
#define DECLARE_ABSTRACT_STREAMABLE_OWL(cls, ver )              \
    DECLARE_ABSTRACT_STREAMABLE( _OWLCLASS, cls, ver )                

#define DECLARE_STREAMABLE_INLINES( cls )                       \
    DECLARE_STREAMABLE_INLINES_I( cls )

#else
#  define DECLARE_STREAMABLE_OWL(cls, ver )                      \
    DECLARE_STREAMABLES( cls, ver ) 
# define DECLARE_STREAMABLE_FROM_BASE_OWL( cls, base )          \
    DECLARE_STREAMABLE_FROM_BASES(cls, base )
#define DECLARE_ABSTRACT_STREAMABLE_OWL(cls, ver )              \
    DECLARE_ABSTRACT_STREAMABLES(cls, ver )                

#define DECLARE_STREAMABLE_INLINES( cls )                       \
    DECLARE_STREAMABLE_INLINES_I( cls )

#endif


#else  // if BI_NO_OBJ_STREAMING

#define DECLARE_STREAMABLE( exp, cls, ver )                         \
    DECLARE_CASTABLE

#define DECLARE_STREAMABLE_S(cls, ver )                             \
    DECLARE_CASTABLE

#define DECLARE_STREAMABLE_FROM_BASE( exp, cls, base )              \
    DECLARE_CASTABLE

#define DECLARE_STREAMABLE_FROM_BASE_S( cls, base )                 \
    DECLARE_CASTABLE

#define DECLARE_ABSTRACT_STREAMABLE( exp, cls, ver )                \
    DECLARE_CASTABLE

#define DECLARE_ABSTRACT_STREAMABLE_S( cls, ver )                   \
    DECLARE_CASTABLE

#define DECLARE_STREAMABLE_OWL(cls, ver )                           \
    DECLARE_CASTABLE

# define DECLARE_STREAMABLE_FROM_BASE_OWL( cls, base )              \
    DECLARE_CASTABLE

#define DECLARE_ABSTRACT_STREAMABLE_OWL( cls, ver )                 \
    DECLARE_CASTABLE

#define DECLARE_STREAMABLE_INLINES( cls )

#endif  // if/else BI_NO_OBJ_STREAMING

//
// Castable implementation macros
//
#if !defined( BI_NO_RTTI )

#define IMPLEMENT_CASTABLE( cls )
#define IMPLEMENT_CASTABLE1( cls, base1 )
#define IMPLEMENT_CASTABLE2( cls, base1, base2 )
#define IMPLEMENT_CASTABLE3( cls, base1, base2, base3 )
#define IMPLEMENT_CASTABLE4( cls, base1, base2, base3, base4 )
#define IMPLEMENT_CASTABLE5( cls, base1, base2, base3, base4, base5 )

#else   // BI_NO_RTTI

#define IMPLEMENT_CASTABLE_ID( cls )                                \
NS_OWL::TStreamableBase::Type_id cls::CastableIdent = #cls;                 \
NS_OWL::TStreamableBase::Type_id cls::CastableID() const                    \
{                                                                   \
    return cls::CastableIdent;                                      \
}                                                                   \

#define IMPLEMENT_CASTABLE( cls )                                   \
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
  return (_tcscmp( id, CastableIdent ) == 0) ? (void *)this : 0;    \
}                                                                   \

                                                                    \
#define IMPLEMENT_CASTABLE1( cls, base1 )                           \
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
    if(_tcscmp( id, CastableIdent ) == 0)                           \
        return (void *)this;                                        \
    else                                                            \
        return base1::FindBase(id);                                 \
}                                                                   \

#define IMPLEMENT_CASTABLE2( cls, base1, base2 )                    \
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
    void *res = 0;                                                  \
    if(_tcscmp( id, CastableIdent ) == 0)                            \
        return (void *)this;                                        \
    else if( (res = base1::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base2::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else                                                            \
        return 0;                                                   \
}                                                                   \

#define IMPLEMENT_CASTABLE3( cls, base1, base2, base3 )             \
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
    void *res = 0;                                                  \
    if(_tcscmp( id, CastableIdent ) == 0)                            \
        return (void *)this;                                        \
    else if( (res = base1::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base2::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base3::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else                                                            \
        return 0;                                                   \
}                                                                   \

#define IMPLEMENT_CASTABLE4( cls, base1, base2, base3, base4 )      \
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
    void *res = 0;                                                  \
    if(_tcscmp( id, CastableIdent ) == 0)                           \
        return (void *)this;                                        \
    else if( (res = base1::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base2::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base3::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base4::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else                                                            \
        return 0;                                                   \
}                                                                   \

#define IMPLEMENT_CASTABLE5( cls, base1, base2, base3, base4, base5 )\
IMPLEMENT_CASTABLE_ID( cls );                                       \
void *cls::FindBase( Type_id id ) const                             \
{                                                                   \
    void *res = 0;                                                  \
    if(_tcscmp( id, CastableIdent ) == 0)                           \
        return (void *)this;                                        \
    else if( (res = base1::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base2::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base3::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base4::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else if( (res = base5::FindBase(id)) != 0 )                     \
        return res;                                                 \
    else                                                            \
        return 0;                                                   \
}                                                                   \

#endif  // BI_NO_RTTI


//
// Streamable implementation mactos
//
#if !defined(BI_NO_OBJ_STREAMING)
# if defined( BI_NO_RTTI )
#   define IMPLEMENT_STREAMABLE_CLASS( cls )    \
    NS_OWL::TStreamableClass r ## cls( cls::CastableIdent, &cls::Streamer::Build )
# else
#      if defined(BI_COMP_MSC) && !defined(UNIX) && !defined(__EDG__)
#         if defined(BI_COMPATIBLE_STREAMING)
#            define IMPLEMENT_STREAMABLE_CLASS( cls )    \
               NS_OWL::TStreamableClass r ## cls( typeid(cls).name() + 6, &cls::Streamer::Build )
#        else
#            define IMPLEMENT_STREAMABLE_CLASS( cls )    \
               NS_OWL::TStreamableClass r ## cls( typeid(cls).raw_name(), &cls::Streamer::Build )
#        endif
#      else
#         define IMPLEMENT_STREAMABLE_CLASS( cls )    \
            NS_OWL::TStreamableClass r ## cls( typeid(cls).name(), &cls::Streamer::Build )
#      endif
# endif
#else
# define IMPLEMENT_STREAMABLE_CLASS( cls )
#endif

#if !defined(BI_NO_OBJ_STREAMING)
#define IMPLEMENT_STREAMABLE_POINTER( cls )                         \
NS_OWL::ipstream& cls::readPtr( NS_OWL::ipstream& is, cls*& cl )                    \
    {                                                               \
    NS_OWL::TStreamableBase *temp = 0;                                      \
    is.readObjectPointer( temp );                                   \
    cl = TYPESAFE_DOWNCAST(temp,cls);                               \
    return is;                                                      \
    }                                                               \
NS_OWL::ipstream& cls::readRef( NS_OWL::ipstream& is, cls& cl )                     \
    {                                                               \
    NS_OWL::TStreamableBase *ptr = &cl;                                     \
    is.readObject( ptr );                                           \
    return is;                                                      \
    }                                                               \
NS_OWL::opstream& cls::writeRef( NS_OWL::opstream& os, const cls& cl )              \
    {                                                               \
    os.writeObject( &cl );                                          \
    return os;                                                      \
    }                                                               \
NS_OWL::opstream& cls::writePtr( NS_OWL::opstream& os, const cls* cl )              \
    {                                                               \
    os.writeObjectPointer( cl );                                    \
    return os;                                                      \
    }
#else
#define IMPLEMENT_STREAMABLE_POINTER( cls )
#endif

#if !defined(BI_NO_OBJ_STREAMING)

#define IMPLEMENT_STREAMER( cls )                                   \
cls::Streamer::Streamer( NS_OWL::TStreamableBase *obj ) :                   \
    NS_OWL::TNewStreamer(obj), object(TYPESAFE_DOWNCAST(obj,cls)){}

#define IMPLEMENT_STREAMABLE_CTOR( cls )                            \
cls::cls ( NS_OWL::StreamableInit ) {}

#define IMPLEMENT_STREAMABLE_CTOR1( cls, base1 )                    \
cls::cls ( NS_OWL::StreamableInit ) : base1( NS_OWL::streamableInit ) {}

#define IMPLEMENT_STREAMABLE_CTOR2( cls, base1, base2 )             \
cls::cls ( NS_OWL::StreamableInit ) :                                       \
    base1 ( NS_OWL::streamableInit ),                                       \
    base2 ( NS_OWL::streamableInit ) {}

#define IMPLEMENT_STREAMABLE_CTOR3( cls, base1, base2, base3 )      \
cls::cls ( NS_OWL::StreamableInit ) :                                       \
    base1 ( NS_OWL::streamableInit ),                                       \
    base2 ( NS_OWL::streamableInit ),                                       \
    base3 ( NS_OWL::streamableInit ) {}

#define IMPLEMENT_STREAMABLE_CTOR4( cls, base1, base2, base3, base4 )\
cls::cls ( NS_OWL::StreamableInit ) :                                       \
    base1 ( NS_OWL::streamableInit ),                                       \
    base2 ( NS_OWL::streamableInit ),                                       \
    base3 ( NS_OWL::streamableInit ),                                       \
    base4 ( NS_OWL::streamableInit ) {}

#define IMPLEMENT_STREAMABLE_CTOR5( cls, base1,base2,base3,base4,base5)\
cls::cls ( NS_OWL::StreamableInit ) :                                       \
    base1 ( NS_OWL::streamableInit ),                                       \
    base2 ( NS_OWL::streamableInit ),                                       \
    base3 ( NS_OWL::streamableInit ),                                       \
    base4 ( NS_OWL::streamableInit ),                                       \
    base5 ( NS_OWL::streamableInit ) {}

#else  // if !defined(BI_NO_OBJ_STREAMING)
# define IMPLEMENT_STREAMER( cls )
# define IMPLEMENT_STREAMABLE_CTOR( cls )
# define IMPLEMENT_STREAMABLE_CTOR1( cls, base1 )
# define IMPLEMENT_STREAMABLE_CTOR2( cls, base1, base2 )
# define IMPLEMENT_STREAMABLE_CTOR3( cls, base1, base2, base3 )
# define IMPLEMENT_STREAMABLE_CTOR4( cls, base1, base2, base3, base4 )
# define IMPLEMENT_STREAMABLE_CTOR5( cls, base1,base2,base3,base4,base5)
#endif
                                                                    \

//
//  Standard Combinations of Streamable Implementations
//

#if !defined(BI_NO_OBJ_STREAMING)
#define IMPLEMENT_ABSTRACT_STREAMABLE( cls )                        \
IMPLEMENT_CASTABLE( cls );                                          \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR( cls );                                   \
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_ABSTRACT_STREAMABLE1( cls, base1 )                \
IMPLEMENT_CASTABLE1( cls, base1 );                                  \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR1( cls, base1 );                           \
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_ABSTRACT_STREAMABLE2( cls, base1, base2 )         \
IMPLEMENT_CASTABLE2( cls, base1, base2 );                    \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR2( cls, base1, base2 );                    \
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_ABSTRACT_STREAMABLE3( cls, base1, base2, base3 )  \
IMPLEMENT_CASTABLE3( cls, base1, base2, base3 );                    \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR3( cls, base1, base2, base3 );             \
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_ABSTRACT_STREAMABLE4( cls, base1, base2, base3, base4 )\
IMPLEMENT_CASTABLE4( cls, base1, base2, base3, base4 );             \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR4( cls, base1, base2, base3, base4 );      \
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_ABSTRACT_STREAMABLE5( cls, base1, base2, base3, base4, base5 )\
IMPLEMENT_CASTABLE5( cls, base1, base2, base3, base4, base5 );      \
IMPLEMENT_STREAMER( cls );                                          \
IMPLEMENT_STREAMABLE_CTOR5( cls, base1, base2, base3, base4, base5 );\
IMPLEMENT_STREAMABLE_POINTER( cls )

#define IMPLEMENT_STREAMABLE( cls )                                 \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE( cls )

#define IMPLEMENT_STREAMABLE1( cls, base1 )                         \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE1( cls, base1 )

#define IMPLEMENT_STREAMABLE2( cls, base1, base2 )                  \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE2( cls, base1, base2 )

#define IMPLEMENT_STREAMABLE3( cls, base1, base2, base3 )           \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE3( cls, base1, base2, base3 )

#define IMPLEMENT_STREAMABLE4( cls, base1, base2, base3, base4 )    \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE4( cls, base1, base2, base3, base4 )

#define IMPLEMENT_STREAMABLE5( cls, base1, base2, base3, base4, base5 )\
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_ABSTRACT_STREAMABLE5( cls, base1, base2, base3, base4, base5 )

#define IMPLEMENT_STREAMABLE_FROM_BASE( cls, base1 )                \
IMPLEMENT_STREAMABLE_CLASS( cls );                                  \
IMPLEMENT_STREAMABLE_CTOR1( cls, base1 );                           \
IMPLEMENT_STREAMABLE_POINTER( cls )

#else  // if BI_NO_OBJ_STREAMING

#define IMPLEMENT_ABSTRACT_STREAMABLE( cls )                        \
IMPLEMENT_CASTABLE( cls );
#define IMPLEMENT_ABSTRACT_STREAMABLE1( cls, base1 )                \
IMPLEMENT_CASTABLE1( cls, base1 );
#define IMPLEMENT_ABSTRACT_STREAMABLE2( cls, base1, base2 )         \
IMPLEMENT_CASTABLE2( cls, base1, base2 )
#define IMPLEMENT_ABSTRACT_STREAMABLE3( cls, base1, base2, base3 )  \
IMPLEMENT_CASTABLE3( cls, base1, base2, base3 );
#define IMPLEMENT_ABSTRACT_STREAMABLE4( cls, base1, base2, base3, base4 ) \
IMPLEMENT_CASTABLE4( cls, base1, base2, base3, base4 );
#define IMPLEMENT_ABSTRACT_STREAMABLE5( cls, base1, base2, base3, base4, base5 ) \
IMPLEMENT_CASTABLE5( cls, base1, base2, base3, base4, base5 );
#define IMPLEMENT_STREAMABLE( cls )                                \
IMPLEMENT_CASTABLE( cls );
#define IMPLEMENT_STREAMABLE1( cls, base1 )                        \
IMPLEMENT_CASTABLE1( cls, base1 );
#define IMPLEMENT_STREAMABLE2( cls, base1, base2 )                 \
IMPLEMENT_CASTABLE2( cls, base1, base2 )
#define IMPLEMENT_STREAMABLE3( cls, base1, base2, base3 )          \
IMPLEMENT_CASTABLE3( cls, base1, base2, base3 );
#define IMPLEMENT_STREAMABLE4( cls, base1, base2, base3, base4 )   \
IMPLEMENT_CASTABLE4( cls, base1, base2, base3, base4 );
#define IMPLEMENT_STREAMABLE5( cls, base1, base2, base3, base4, base5 ) \
IMPLEMENT_CASTABLE5( cls, base1, base2, base3, base4, base5 );
#define IMPLEMENT_STREAMABLE_FROM_BASE( cls, base1 )

#endif  // if/else BI_NO_OBJ_STREAMING


__OWL_END_NAMESPACE


#endif  // OWL_OBJSTRM_H
