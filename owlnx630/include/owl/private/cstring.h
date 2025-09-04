//----------------------------------------------------------------------------
// ObjectWindows
// (C) Copyright 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
//   Standard C++ string class
//
//  This class uses a technique called "copy-on-write".
//  Multiple instances of a string can refer to the same piece of data
//  so long as it is in a "readonly" situation.  If a string writes to
//  the data, then a copy is automatically made if more than one string
//  is referring to it.
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_CSTRING_H)
#define OWL_PRIVATE_CSTRING_H
#define __REF_H
#define __CSTRING_H

#if !defined(OWL_PRIVATE_EXCEPT_H)
# include <owl/private/except.h>
#endif
#if !defined(OWL_PRIVATE_WSYSINC_H)
# include <owl/private/wsysinc.h>
#endif
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS string;
class _OWLCLASS TRegexp;
class _OWLCLASS TSubString;
class _OWLCLASS TStringRef;

__OWL_END_NAMESPACE

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Since inline functions that use throw specifiers                      */
/*  currently end up rather large, we don't use throw                     */
/*  specifiers on them unless you #define USE_THROW_SPECIFIERS.           */
/*                                                                        */
/*------------------------------------------------------------------------*/

#if defined( USE_THROW_SPECIFIERS ) && !defined(BI_NO_THROWSPEC)
#define THROW_NONE                  throw()
#define THROW_XALLOC                throw(XALLOC)
#define THROW_OUTOFRANGE            throw( string::outofrange )
#define THROW_XALLOC_LENGTHERROR    throw( XALLOC, string::lengtherror )
#define THROW_XALLOC_OUTOFRANGE     throw( XALLOC, string::outofrange )
#define THROW_XALLOC_RANGE_LENGTH   \
        throw( xalloc, string::lengtherror, string::outofrange )
#else
#define THROW_NONE
#define THROW_XALLOC
#define THROW_OUTOFRANGE
#define THROW_XALLOC_LENGTHERROR
#define THROW_XALLOC_OUTOFRANGE
#define THROW_XALLOC_RANGE_LENGTH
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class _OWLCLASS TReference {
  public:
    _OWLENTRY TReference(unsigned short initRef = 0) : Refs(initRef) { }
    void _OWLENTRY AddReference() { Refs++; }
    unsigned short _OWLENTRY References() { return Refs; }
    unsigned short _OWLENTRY RemoveReference() { return --Refs; }

  private:
    unsigned short Refs;    // Number of references to this block
};

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  string                                                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

const size_t NPOS = size_t(-1);

class _OWLCLASS string {
  public:

    //
    // Exceptions
    //
    class _OWLCLASS outofrange : public XBASE    {
      public:
        _OWLENTRY outofrange();
    };

    class _OWLCLASS lengtherror : public XBASE    {
      public:
        _OWLENTRY lengtherror();
    };

    // additional conversion flags
    enum {
      dec         = 0x000,
      oct         = 0x002,
      hex         = 0x004,
      showbase     = 0x008,
      uppercase   = 0x010,
      showpos     = 0x020,
      fixed       = 0x040,
      scientific  = 0x080,
      showpoint   = 0x100,
    };
    //
    // Constructors
    //

    _OWLENTRY string() THROW_XALLOC;

    _OWLENTRY string( const string &s ) THROW_XALLOC;
    _OWLENTRY string( const string &s, size_t orig, size_t n = NPOS )
        THROW_XALLOC;

    _OWLENTRY string( LPCTSTR cp ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( LPCTSTR cp, size_t orig, size_t n = NPOS )
        THROW_XALLOC_LENGTHERROR;

    _OWLENTRY string( _TCHAR c ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( _TCHAR c, size_t n ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( _TSCHAR c ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( _TSCHAR c, size_t n ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( _TUCHAR c ) THROW_XALLOC_LENGTHERROR;
    _OWLENTRY string( _TUCHAR c, size_t n ) THROW_XALLOC_LENGTHERROR;

    // non-standard constructors
    _OWLENTRY string( const TSubString &ss ) THROW_XALLOC;


    // Ctor to make a string from a resource
#if defined(BI_PLAT_MSW)
    _OWLENTRY string( HINSTANCE instance, uint id, int len = 255 )
        THROW_XALLOC_LENGTHERROR;
#endif

    //
    // Destructor
    //
    _OWLENTRY ~string() THROW_NONE;

    //
    // Assignment
    //
    string & _OWLENTRY operator = ( const string &s ) THROW_XALLOC;
    string & _OWLENTRY assign( const string &s ) THROW_XALLOC;
    string & _OWLENTRY assign( const string &s,
                                    size_t orig,
                                    size_t n = NPOS ) THROW_XALLOC;

    //
    // Concatenation
    //
    string & _OWLENTRY operator += ( const string &s )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( const string &s )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( const string &s,
                                    size_t orig,
                                    size_t n = NPOS ) THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( LPCTSTR cp )
        THROW_XALLOC_LENGTHERROR;
    friend string _OWLENTRY _FARFUNC operator + ( const string &s, LPCTSTR cp )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( LPCTSTR cp )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( LPCTSTR p, size_t orig, size_t n = NPOS )
        THROW_XALLOC_LENGTHERROR;

    // additional helpers parameters
    string & _OWLENTRY operator += ( const TCHAR & c )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const bool & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const int & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const unsigned int & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const short & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const unsigned short & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const long & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const unsigned long & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const float & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY operator += ( const double & number )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( const signed long cp ) const
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( const unsigned long cp ) const
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY append( const double cp ) const
        THROW_XALLOC_LENGTHERROR;


    string & _OWLENTRY prepend( const string &s )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY prepend( const string &s, size_t orig, size_t n = NPOS )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY prepend( LPCTSTR cp )
        THROW_XALLOC_LENGTHERROR;
    string & _OWLENTRY prepend( LPCTSTR cp, size_t orig, size_t n = NPOS )
        THROW_XALLOC_LENGTHERROR;

    //
    // Comparison
    //
    int _OWLENTRY compare(const string &s) const THROW_NONE;
    int _OWLENTRY compare(const string &s, size_t orig, size_t n = NPOS ) const
        THROW_NONE;

    friend int _OWLENTRY operator == ( const string &s1, const string &s2 )
        THROW_NONE;

    friend int _OWLENTRY operator != ( const string &s1, const string &s2 )
        THROW_NONE;

    friend int _OWLENTRY operator == ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator == ( LPCTSTR cp, const string &s )
        THROW_NONE;

    friend int _OWLENTRY operator != ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator != ( LPCTSTR cp, const string &s )
        THROW_NONE;

    friend int _OWLENTRY operator <  ( const string &s1, const string &s2 )
        THROW_NONE;
    friend int _OWLENTRY operator <  ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator <  ( LPCTSTR cp, const string &s )
        THROW_NONE;

    friend int _OWLENTRY operator <= ( const string &s1, const string &s2 )
        THROW_NONE;
    friend int _OWLENTRY operator <= ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator <= ( LPCTSTR cp, const string &s )
        THROW_NONE;

    friend int _OWLENTRY operator >  ( const string &s1, const string &s2 )
        THROW_NONE;
    friend int _OWLENTRY operator >  ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator >  ( LPCTSTR cp, const string &s )
        THROW_NONE;

    friend int _OWLENTRY operator >= ( const string &s1, const string &s2 )
        THROW_NONE;
    friend int _OWLENTRY operator >= ( const string &s, LPCTSTR cp )
        THROW_NONE;
    friend int _OWLENTRY operator >= ( LPCTSTR cp, const string &s )
        THROW_NONE;

    //
    // Insertion at some position
    //
    string & _OWLENTRY insert( size_t pos, const string &s )
        THROW_XALLOC_RANGE_LENGTH;
    string & _OWLENTRY insert( size_t pos, const string &s, size_t orig,
                                    size_t n = NPOS ) THROW_XALLOC_RANGE_LENGTH;

    //
    // Removal
    //
    string & _OWLENTRY remove( size_t pos ) THROW_XALLOC_OUTOFRANGE;
    string & _OWLENTRY remove( size_t pos, size_t n )
        THROW_XALLOC_OUTOFRANGE;

    //
    // Replacement at some position
    //
    string & _OWLENTRY replace( size_t pos, size_t n, const string &s )
        THROW_XALLOC_RANGE_LENGTH;
    string & _OWLENTRY replace( size_t pos, size_t n1, const string &s,
                                     size_t orig, size_t n2 = NPOS )
        THROW_XALLOC_RANGE_LENGTH;

    //
    // Subscripting
    //
    TCHAR _OWLENTRY get_at( size_t pos ) const THROW_OUTOFRANGE;
    void _OWLENTRY put_at( size_t pos, char c ) THROW_OUTOFRANGE;

    _TCHAR & _OWLENTRY operator[]( size_t pos ) THROW_OUTOFRANGE;
    _TCHAR & _OWLENTRY operator()( size_t pos ) THROW_OUTOFRANGE;
    TSubString _OWLENTRY operator()( size_t start, size_t len ) THROW_NONE;
    TSubString _OWLENTRY operator()( const TRegexp &re ) THROW_NONE;
    TSubString _OWLENTRY operator()( const TRegexp &re, size_t start ) THROW_NONE;

    TCHAR _OWLENTRY operator[]( size_t pos ) const THROW_OUTOFRANGE;
    TCHAR _OWLENTRY operator()( size_t pos ) const THROW_OUTOFRANGE;
    const TSubString _OWLENTRY operator()( size_t start, size_t len ) const THROW_NONE;
    const TSubString _OWLENTRY operator()( const TRegexp &pat ) const THROW_NONE;
    const TSubString _OWLENTRY operator()( const TRegexp &pat, size_t start )
        const THROW_NONE;

    //
    // Searching
    //
    size_t _OWLENTRY find( const string &s ) const THROW_NONE;
    size_t _OWLENTRY find( const string &s, size_t pos ) const THROW_NONE;
    size_t _OWLENTRY rfind( const string &s ) const THROW_NONE;
    size_t _OWLENTRY rfind( const string &s, size_t pos ) const THROW_NONE;

    int _OWLENTRY contains( LPCTSTR pat ) const THROW_NONE;
    int _OWLENTRY contains(const string &s) const THROW_NONE;
    size_t _OWLENTRY find( const TRegexp &pat, size_t i = 0 ) const THROW_NONE;
    size_t _OWLENTRY find( const TRegexp &pat, size_t *ext, size_t i = 0 )
        const THROW_NONE;

    //
    // Substring
    //
    string _OWLENTRY substr( size_t pos ) const
        THROW_XALLOC_OUTOFRANGE;
    string _OWLENTRY substr( size_t pos, size_t n ) const
        THROW_XALLOC_OUTOFRANGE;

    TSubString _OWLENTRY substring( LPCTSTR cp ) THROW_NONE;
    const TSubString _OWLENTRY substring( LPCTSTR cp )
        const THROW_NONE;
    TSubString _OWLENTRY substring( LPCTSTR cp, size_t start ) THROW_NONE;
    const TSubString _OWLENTRY substring( LPCTSTR cp, size_t start )
        const THROW_NONE;

    //
    // Character set searching
    //
    size_t _OWLENTRY find_first_of( const string &s ) const THROW_NONE;
    size_t _OWLENTRY find_first_of( const string &s, size_t pos ) const THROW_NONE;
    size_t _OWLENTRY find_first_not_of( const string &s ) const THROW_NONE;
    size_t _OWLENTRY find_first_not_of( const string &s, size_t pos )
        const THROW_NONE;
    size_t _OWLENTRY find_last_of( const string &s ) const THROW_NONE;
    size_t _OWLENTRY find_last_of( const string &s, size_t pos ) const THROW_NONE;
    size_t _OWLENTRY find_last_not_of( const string &s ) const THROW_NONE;
    size_t _OWLENTRY find_last_not_of( const string &s, size_t pos )
        const THROW_NONE;

    //
    // Miscellaneous
    //
    size_t _OWLENTRY length() const THROW_NONE;
    size_t _OWLENTRY copy( LPTSTR cb, size_t n ) THROW_OUTOFRANGE;
    size_t _OWLENTRY copy( LPTSTR cb, size_t n, size_t pos ) THROW_OUTOFRANGE;
    LPCTSTR _OWLENTRY c_str() const THROW_XALLOC;
    size_t _OWLENTRY reserve() const THROW_NONE;
    void _OWLENTRY reserve( size_t ic ) THROW_XALLOC_OUTOFRANGE;

    string _OWLENTRY copy() const THROW_XALLOC;   // Distinct copy of self.


    // Static member functions:
    static int _OWLENTRY set_case_sensitive( int tf = 1 );
    static int _OWLENTRY set_paranoid_check( int ck = 1 );
    static int _OWLENTRY skip_whitespace( int sk = 1 );
    static size_t _OWLENTRY initial_capacity( size_t ic = 63 );
    static size_t _OWLENTRY resize_increment( size_t ri = 64 );
    static size_t _OWLENTRY max_waste( size_t mw = 63 );

    static int _OWLENTRY get_case_sensitive_flag();
    static int _OWLENTRY get_paranoid_check_flag();
    static int _OWLENTRY get_skip_whitespace_flag();
    static size_t _OWLENTRY get_initial_capacity();
    static size_t _OWLENTRY get_resize_increment();
    static size_t _OWLENTRY get_max_waste();

    static unsigned _OWLENTRY flags();
    static void _OWLENTRY set_bit(const unsigned  bit);
    static void _OWLENTRY clear_bit(const unsigned  bit);
    static size_t _OWLENTRY precision();
    static void _OWLENTRY precision(size_t prec);

    enum StripType { Leading, Trailing, Both };

    // Non-static member functions:
    unsigned _OWLENTRY hash() const;
    int      _OWLENTRY is_null() const;
    int      _OWLENTRY empty() const;
    _tistream & _OWLENTRY read_file( _tistream &is );
    _tistream & _OWLENTRY read_string( _tistream &is );
    _tistream & _OWLENTRY read_line( _tistream &is );
    _tistream & _OWLENTRY read_to_delim( _tistream &is, TCHAR delim = _T('\n') );
    _tistream & _OWLENTRY read_token( _tistream &is );
    void       _OWLENTRY resize( size_t m );
    TSubString _OWLENTRY strip( StripType s = Trailing, TCHAR c = _T(' ') );
    void       _OWLENTRY to_lower();
    void       _OWLENTRY to_upper();

#  if defined( BI_PLAT_MSW )
    void _OWLENTRY ansi_to_oem() THROW_NONE;
    void _OWLENTRY oem_to_ansi() THROW_NONE;
#  endif

protected:

    int  _OWLENTRY valid_element( size_t pos ) const THROW_NONE;
    int  _OWLENTRY valid_index( size_t pos ) const THROW_NONE;

    void _OWLENTRY assert_element( size_t pos ) const THROW_OUTOFRANGE;
    void _OWLENTRY assert_index( size_t pos ) const THROW_OUTOFRANGE;

    _OWLENTRY string( const string &s, LPCTSTR cb );
    void _OWLENTRY cow();

private:

    TStringRef *p;

    static int case_sensitive;
    static int paranoid_check;
    static int skip_white;
    static size_t initial_capac;
    static size_t resize_inc;
    static size_t freeboard;
    static unsigned s_flags;
    static size_t precis;

private:

    friend class _OWLCLASS TSubString;
    friend class _OWLCLASS TStringRef;

    void _OWLENTRY clone();
    size_t _OWLENTRY find_case_index( LPCTSTR cb, size_t start, size_t &patl) const;
    size_t _OWLENTRY rfind_case_index( LPCTSTR cb, size_t start, size_t &patl) const;
    size_t _OWLENTRY find_index(LPCTSTR, size_t start, size_t & patl) const;
    size_t _OWLENTRY rfind_index(LPCTSTR, size_t start, size_t & patl) const;
    unsigned _OWLENTRY hash_case() const;

};


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Related global functions                                              */
/*                                                                        */
/*------------------------------------------------------------------------*/

_OWLFUNC(_tistream &)
operator >> ( _tistream &is, string &s );

_OWLFUNC(_tostream &)
operator << ( _tostream &os, const string &s );

_OWLFUNC(_tistream &)
getline( _tistream &is, string &s );

_OWLFUNC(_tistream &)
getline( _tistream &is, string &s, char c );

_OWLFUNC(string)
to_lower( const string &s ) THROW_NONE;
_OWLFUNC(string)
to_upper( const string &s ) THROW_NONE;
_OWLFUNC(string)
operator + ( LPCTSTR cp, const string & s)
             THROW_XALLOC_LENGTHERROR;
_OWLFUNC(string)
operator + ( const string &s1, const string &s2 )
             THROW_XALLOC_LENGTHERROR;

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TStringRef                                                            */
/*                                                                        */
/*  This is the dynamically allocated part of a string.                   */
/*  It maintains a reference count.                                       */
/*  There are no public member functions.                                 */
/*                                                                        */
/*------------------------------------------------------------------------*/

class _OWLCLASS TStringRef : public TReference {

    friend class _OWLCLASS string;
    friend class _OWLCLASS TSubString;

    //
    // Data
    //
    TCHAR *array;
    size_t nchars;
    size_t capacity;

    //
    // State flags
    //
    enum {
        MemReserved = 1     // indicates that reserve() has been
                            // called on this string
        };
    unsigned flags;

    //
    // Constructors
    //
    _OWLENTRY TStringRef( TCHAR c, size_t n );
    _OWLENTRY TStringRef( LPCTSTR str1, size_t count1,
                          LPCTSTR str2, size_t count2,
                          size_t extra );


    //
    // Ctor to make a TStringRef from a resource
    //
#if defined(BI_PLAT_MSW)
    _OWLENTRY TStringRef( HINSTANCE instance, UINT id, int len = 256 )
         THROW_XALLOC_LENGTHERROR;
#endif

    //
    // Destructor
    //
    _OWLENTRY ~TStringRef() THROW_NONE;

    //
    // Miscellaneous
    //
    void _OWLENTRY reserve( size_t ic ) THROW_XALLOC_OUTOFRANGE;
    void _OWLENTRY check_freeboard() THROW_NONE;
    void _OWLENTRY grow_to( size_t n ) THROW_XALLOC_LENGTHERROR;
    void _OWLENTRY read_to_delim( _tistream &is, char delim ) THROW_XALLOC;
    void _OWLENTRY read_token( _tistream &is ) THROW_XALLOC;
    static size_t _OWLENTRY round_capacity( size_t cap ) THROW_NONE;
    void _OWLENTRY splice( size_t start, size_t extent, LPCTSTR cp, size_t n )
        THROW_XALLOC_LENGTHERROR;

};


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TSubString                                                            */
/*                                                                        */
/*  The TSubString class allows selected elements to be addressed.        */
/*  There are no public constructors.                                     */
/*                                                                        */
/*------------------------------------------------------------------------*/

class _OWLCLASS TSubString{

  public:

    //
    // Assignment
    //
    TSubString & _OWLENTRY operator = ( const string &s ) THROW_NONE;

    //
    // Comparison
    //
    int _OWLENTRY operator == ( LPCTSTR cp ) const THROW_NONE;
    int _OWLENTRY operator == ( const string &s ) const THROW_NONE;
    int _OWLENTRY operator != ( LPCTSTR cp ) const THROW_NONE;
    int _OWLENTRY operator != ( const string & str ) const THROW_NONE;

    //
    // Subscripting
    //
    char _OWLENTRY get_at( size_t pos ) const THROW_OUTOFRANGE;
    void _OWLENTRY put_at( size_t pos, char c ) THROW_OUTOFRANGE;

    TCHAR & _OWLENTRY operator[]( size_t pos ) THROW_OUTOFRANGE;
    TCHAR & _OWLENTRY operator()( size_t pos ) THROW_OUTOFRANGE;
    TCHAR _OWLENTRY operator[]( size_t pos ) const THROW_OUTOFRANGE;
    TCHAR _OWLENTRY operator()( size_t pos ) const THROW_OUTOFRANGE;
    size_t _OWLENTRY length() const THROW_NONE;
    int _OWLENTRY start() const THROW_NONE;
    void _OWLENTRY to_lower() THROW_NONE;
    void _OWLENTRY to_upper() THROW_NONE;

    //
    // Detecting empty strings
    //
    int _OWLENTRY is_null() const THROW_NONE;
    int _OWLENTRY operator!() const THROW_NONE;

protected:

    void _OWLENTRY assert_element( size_t pos ) const THROW_OUTOFRANGE;
    int _OWLENTRY valid_element( size_t pos ) const;

private:

    friend class _OWLCLASS string;

    //
    // Data
    //
    string *s;
    size_t begin;
    size_t extent;

    //
    // Constructor
    //
    _OWLENTRY TSubString( const string *cp, size_t start, size_t len );

};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//__OWL_USING_NAMESPACE
//#if !defined(BI_NO_NAMESPACE)
//using OWL::string;
//#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  string inlines                                                        */
/*                                                                        */
/*------------------------------------------------------------------------*/

inline _OWLENTRY string::outofrange::outofrange() :
#if defined(BI_STD_EXCEPTIONS)
    XBASE()
#else
    XBASE( _T("String reference out of range") )
#endif
{
}

inline _OWLENTRY string::lengtherror::lengtherror() :
#if defined(BI_STD_EXCEPTIONS)
    XBASE()
#else
    XBASE( _T("String length error") )
#endif
{
}

inline _OWLENTRY string::string( TCHAR c ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,1);
}

inline _OWLENTRY string::string( TCHAR c, size_t n ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,n);
}
#if !defined(_UNICODE)
inline _OWLENTRY string::string( signed char c ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,1);
}

inline _OWLENTRY string::string( signed char c, size_t n ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,n);
}

inline _OWLENTRY string::string( unsigned char c ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,1);
}

inline _OWLENTRY string::string( unsigned char c, size_t n ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,n);
}
#endif

inline __OWL string & _OWLENTRY string::operator = ( const __OWL string &s )
    THROW_NONE
{
    return assign( s, 0, NPOS );
}

inline __OWL string & _OWLENTRY string::assign( const __OWL string &s )
    THROW_NONE
{
    return assign( s, 0, NPOS );
}

inline __OWL string & _OWLENTRY string::operator += ( const __OWL string &s )
    THROW_XALLOC_LENGTHERROR
{
    return append( s, 0, NPOS );
}

inline __OWL string & _OWLENTRY string::append( const __OWL string &s )
    THROW_XALLOC_LENGTHERROR
{
    return append(s, 0, NPOS);
}
inline __OWL string & _OWLENTRY string::prepend( LPCTSTR cp )
    THROW_XALLOC_LENGTHERROR
{
    return prepend( cp, 0, strlen(cp) );
}

inline int _OWLENTRY operator == ( const string &s1, const string &s2 )
    THROW_NONE
{
    return s1.compare( s2 ) == 0;
}

inline int _OWLENTRY operator != ( const string &s1, const string &s2 )
    THROW_NONE
{
    return !(s1==s2);
}

inline __OWL string & _OWLENTRY string::remove( size_t pos )
    THROW_XALLOC_OUTOFRANGE
{
    return remove( pos, length() );
}

inline __OWL string & _OWLENTRY string::replace( size_t pos, size_t n,
                                                const __OWL string &s )
    THROW_XALLOC_RANGE_LENGTH
{
    return replace( pos, n, s, 0, NPOS );
}

inline TCHAR _OWLENTRY string::get_at( size_t pos ) const THROW_OUTOFRANGE{
    return (*this)[pos];
}

inline void _OWLENTRY string::put_at( size_t pos, TCHAR c ) THROW_OUTOFRANGE{
    (*this)[pos] = c;
}

inline TCHAR & _OWLENTRY string::operator[]( size_t pos ) THROW_OUTOFRANGE{
    return (*this)(pos);    // use operator()
}

inline TSubString _OWLENTRY string::operator()( size_t start, size_t len ) THROW_NONE{
    return TSubString( this, start, len );
}

inline size_t _OWLENTRY string::find( const __OWL string &s ) const THROW_NONE{
    return find( s, 0 );
}

inline size_t _OWLENTRY string::rfind( const __OWL string &s ) const THROW_NONE{
    return rfind( s, length() );
}

inline size_t _OWLENTRY string::length() const THROW_NONE {
    return p->nchars;
}

inline LPCTSTR _OWLENTRY string::c_str() const THROW_NONE{
  return p->array;
}
/*
inline _OWLENTRY string::operator LPCTSTR() const THROW_NONE{
  return p->array;
}
*/
inline size_t _OWLENTRY string::reserve() const THROW_NONE
{
    return p->capacity;
}

inline void _OWLENTRY string::cow()
{
    if( p->References() > 1 )
        clone();
}

inline __OWL string & _OWLENTRY string::operator += ( LPCTSTR cp )
    THROW_XALLOC_LENGTHERROR
{
    return append( cp, 0, strlen(cp) );
}
// additions
inline __OWL string & _OWLENTRY string::operator += ( const TCHAR & c )
        THROW_XALLOC_LENGTHERROR
{
    return append(string(c));
}
inline __OWL string & _OWLENTRY string::operator += ( const bool & number )
        THROW_XALLOC_LENGTHERROR
{
    return append((const signed long)number);
}
inline __OWL string & _OWLENTRY string::operator += ( const int & number )
        THROW_XALLOC_LENGTHERROR
{
    return append( (const signed long)number);
}
inline __OWL string & _OWLENTRY string::operator += ( const unsigned int & number )
        THROW_XALLOC_LENGTHERROR
{
    return append( (unsigned long)number);
}
inline __OWL string & _OWLENTRY string::operator += ( const short & number )
        THROW_XALLOC_LENGTHERROR
{
    return append( (long)number);
}
inline __OWL string & _OWLENTRY string::operator += ( const unsigned short & number )
        THROW_XALLOC_LENGTHERROR
{
    return append( (unsigned long)number);
}
inline __OWL string & _OWLENTRY string::operator += ( const long & number )
        THROW_XALLOC_LENGTHERROR
{
    return append(number);
}
inline __OWL string & _OWLENTRY string::operator += ( const unsigned long & number )
        THROW_XALLOC_LENGTHERROR
{
    return append(number);
}
inline __OWL string & _OWLENTRY string::operator += ( const float & number )
        THROW_XALLOC_LENGTHERROR
{
    return append((double)number);
}

inline __OWL string & _OWLENTRY string::operator += ( const double & number )
        THROW_XALLOC_LENGTHERROR
{
    return append( number);
}

inline __OWL string & _OWLENTRY string::prepend( const __OWL string &s )
    THROW_XALLOC_LENGTHERROR
{
    return prepend( s.c_str() );
}

inline __OWL string & _OWLENTRY string::prepend( const __OWL string &s,
                                                     size_t orig, size_t n )
  THROW_XALLOC_LENGTHERROR
{
    return prepend( s.c_str(), orig, n );
}

inline int _OWLENTRY operator == ( const string &s1, LPCTSTR s2 ) THROW_NONE
{
    return s1.compare(s2) == 0;
}

inline int _OWLENTRY operator == ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return string(cp).compare(s) == 0;
}

inline int _OWLENTRY operator != ( const string &s, LPCTSTR cp ) THROW_NONE
{
    return !(s==cp);
}

inline int _OWLENTRY operator != ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return !(cp==s);
}

inline int _OWLENTRY operator <  ( const string &s1, const string &s2 )
    THROW_NONE
{
    return s1.compare(s2) < 0;
}

inline int _OWLENTRY operator <  ( const string &s1, LPCTSTR s2 ) THROW_NONE
{
    return s1.compare(s2) < 0;
}

inline int _OWLENTRY operator <  ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return string(cp).compare(s) < 0;
}

inline int _OWLENTRY operator <= ( const string &s1, const string &s2 )
    THROW_NONE
{
    return s1.compare(s2) <= 0;
}

inline int _OWLENTRY operator <= ( const string &s, LPCTSTR cp ) THROW_NONE
{
    return s.compare(string(cp)) <= 0;
}

inline int _OWLENTRY operator <= ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return string(cp).compare(s) <= 0;
}

inline int _OWLENTRY operator >  ( const string &s1, const string &s2 )
    THROW_NONE
{
    return s1.compare(s2) > 0;
}

inline int _OWLENTRY operator >  ( const string &s, LPCTSTR cp ) THROW_NONE
{
    return s.compare(cp) > 0;
}

inline int _OWLENTRY operator >  ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return string(cp).compare(s) > 0;
}

inline int _OWLENTRY operator >= ( const string &s1, const string &s2 )
    THROW_NONE
{
    return s1.compare(s2) >= 0;
}

inline int _OWLENTRY operator >= ( const string &s, LPCTSTR cp ) THROW_NONE
{
    return s.compare(cp) >= 0;
}

inline int _OWLENTRY operator >= ( LPCTSTR cp, const string &s ) THROW_NONE
{
    return string(cp).compare(s) >= 0;
}

inline TCHAR _OWLENTRY string::operator[]( size_t pos ) const THROW_OUTOFRANGE
{
    assert_element(pos);
    return p->array[pos];
}

inline char _OWLENTRY string::operator()( size_t pos ) const THROW_OUTOFRANGE
{
#if defined( BOUNDS_CHECK )
    assert_element(pos);
#endif
    return p->array[pos];
}

inline int _OWLENTRY string::contains( const __OWL string &s ) const THROW_NONE
{
    return contains( s.c_str() );
}

inline TSubString _OWLENTRY string::substring( LPCTSTR cp ) THROW_NONE
{
    return substring( cp, 0 );
}

inline const TSubString _OWLENTRY string::substring( LPCTSTR cp ) const
    THROW_NONE
{
    return substring( cp, 0 );
}

inline size_t _OWLENTRY string::find_first_of( const __OWL string &s ) const THROW_NONE
{
    return find_first_of( s, 0 );
}

inline size_t _OWLENTRY string::find_first_not_of( const __OWL string &s ) const THROW_NONE
{
    return find_first_not_of( s, 0 );
}

inline size_t _OWLENTRY string::find_last_of( const __OWL string &s ) const THROW_NONE
{
    return find_last_of( s, NPOS );
}

inline size_t _OWLENTRY string::find_last_not_of( const __OWL string &s ) const THROW_NONE
{
    return find_last_not_of( s, NPOS );
}

inline int _OWLENTRY string::get_case_sensitive_flag(){
    return case_sensitive;
}

inline int _OWLENTRY string::get_paranoid_check_flag(){
    return paranoid_check;
}

inline int _OWLENTRY string::get_skip_whitespace_flag(){
    return skip_white;
}

inline size_t _OWLENTRY string::get_initial_capacity(){
    return initial_capac;
}

inline size_t _OWLENTRY string::get_resize_increment(){
    return resize_inc;
}

inline size_t _OWLENTRY string::get_max_waste(){
    return freeboard;
}
inline unsigned _OWLENTRY string::flags(){
  return s_flags;
}
inline void _OWLENTRY string::set_bit(const unsigned bit){
  s_flags |= bit;
}
inline void _OWLENTRY string::clear_bit(const unsigned bit){
  s_flags &= ~bit;
}
inline size_t _OWLENTRY string::precision(){
  return precis;
}
inline void _OWLENTRY string::precision(size_t prec){
  precis = prec;
}

inline int _OWLENTRY string::is_null() const
{
    return *p->array==0;
}

inline int _OWLENTRY string::empty() const
{
    return *p->array==0;
}

#if defined( BI_PLAT_MSW )
inline void _OWLENTRY string::ansi_to_oem() THROW_NONE
{
    ::AnsiToOem( p->array, p->array );
}

inline void _OWLENTRY string::oem_to_ansi() THROW_NONE
{
    ::OemToAnsi( p->array, p->array );
}
#endif

// Check to make sure a string index refers to a valid element
inline int _OWLENTRY string::valid_element( size_t n ) const THROW_NONE
{
    return n < length();
}

// Check to make sure a string index is in range
inline int _OWLENTRY string::valid_index( size_t n ) const THROW_NONE
{
    return n <= length();
}

// Constructor for internal use only
inline _OWLENTRY string::string( const __OWL string &s, LPCTSTR cp )
{
    p = new TStringRef( s.c_str(), s.length(), cp, cp?strlen(cp):0, 0 );
}

inline string _OWLENTRY operator + ( const string &s,
                                     LPCTSTR cp )
    THROW_XALLOC_LENGTHERROR
{
    return string(s,cp);
}

inline string _OWLENTRY operator + ( const string &s1,
                                     const string &s2 )
    THROW_XALLOC_LENGTHERROR
{
    return s1 + s2.c_str();
}

// string additions
#pragma warn -inl
inline string _OWLENTRY operator + ( const string &s,
                                     const TCHAR c )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append(string(c));
}
inline string _OWLENTRY operator + ( const TCHAR c,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(c).append(s);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const bool number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const bool number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const int number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const signed long)number);
}
inline string _OWLENTRY operator + ( const int number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const signed long)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const unsigned int number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const unsigned int number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const long number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append(number);
}
inline string _OWLENTRY operator + ( const long number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const signed long)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const unsigned long  number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const unsigned long number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const unsigned long)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const float &number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const double)number);
}
inline string _OWLENTRY operator + ( const float &number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const double)number);
}
inline string _OWLENTRY operator + ( const string &s,
                                     const double &number )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append(number);
}
inline string _OWLENTRY operator + ( const double &number,
                                     const string &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(s).append((const double)number);
}
#pragma warn .inl
/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TSubString inlines                                                    */
/*                                                                        */
/*------------------------------------------------------------------------*/

inline int _OWLENTRY TSubString::operator == ( const __OWL string &s ) const THROW_NONE
{
    return operator==(s.c_str());
}

inline int _OWLENTRY TSubString::operator != ( LPCTSTR cp ) const THROW_NONE
{
    return !operator==(cp);
}

inline int _OWLENTRY TSubString::operator != ( const __OWL string &s ) const THROW_NONE
{
    return !operator==(s.c_str());
}

inline TCHAR _OWLENTRY TSubString::get_at( size_t pos ) const THROW_OUTOFRANGE
{
    return (*this)[pos];
}

inline void _OWLENTRY TSubString::put_at( size_t pos, TCHAR c ) THROW_OUTOFRANGE
{
    (*this)[pos] = c;
}

inline TCHAR & _OWLENTRY TSubString::operator[]( size_t pos ) THROW_OUTOFRANGE
{
    return (*this)(pos);    // use operator()
}

inline TCHAR _OWLENTRY TSubString::operator[]( size_t pos ) const
    THROW_OUTOFRANGE
{
    assert_element(pos);
    return s->p->array[begin+pos];
}

inline TCHAR _OWLENTRY TSubString::operator()( size_t pos ) const
    THROW_OUTOFRANGE
{
#if defined( BOUNDS_CHECK )
    assert_element(pos);
#endif
    return s->p->array[begin+pos];
}

inline size_t _OWLENTRY TSubString::length() const THROW_NONE
{
    return extent;
}

inline int _OWLENTRY TSubString::start() const THROW_NONE
{
    return begin;
}

inline int _OWLENTRY TSubString::is_null() const THROW_NONE
{
    return begin == NPOS;
}

inline int _OWLENTRY TSubString::operator!() const THROW_NONE
{
    return begin == NPOS;
}

inline int _OWLENTRY TSubString::valid_element( size_t n ) const THROW_NONE
{
    return n < length();
}

// Private constructor
inline _OWLENTRY TSubString::TSubString(const __OWL string *sp,
                              size_t start, size_t len )
:
    begin(start),
    extent(len),
    s((string *)sp)
{
}

inline TSubString _OWLENTRY string::operator()( const TRegexp & r ) THROW_NONE
{
    return (*this)(r,0);
}

inline const TSubString _OWLENTRY string::operator()( const TRegexp &r ) const THROW_NONE
{
    return (*this)(r,0);
}

__OWL_END_NAMESPACE

#endif  // OWL_PRIVATE_CSTRING_H
