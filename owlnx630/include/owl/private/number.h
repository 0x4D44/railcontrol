//----------------------------------------------------------------------------
// Object Windows Library
// OWL NExt
// Copyright (c) 1998 Yura Bidus
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:36 $
//
// uint64, int64, uint128, int128 support
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_NUMBER_H)
#define OWL_PRIVATE_NUMBER_H

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
#pragma read_only_file
#endif

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif

#if !defined(BI_64BIT_NATIVE)

extern "C" {
// asm 64 bit assembler support
// unsigned
_OWLFUNC(void _stdcall) _u64add(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _u64sub(uint8* l, uint8* r);
_OWLFUNC(int  _stdcall) _u64cmp(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _u64mul(uint8* multipl, uint8* mult, uint8* u128);
_OWLFUNC(void _stdcall) _64div(uint8* divident, uint8* divisor, uint8* remainder);
_OWLFUNC(void _stdcall) _u64neg(uint8* l);
// signed
_OWLFUNC(int  _stdcall) _i64cmp(uint8* l, uint8* r);
// common
_OWLFUNC(void _stdcall) _64and(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _64or(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _64xor(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _64not(uint8* l);
_OWLFUNC(void _stdcall) _64shl(uint8* u128, uint32 count);
_OWLFUNC(void _stdcall) _64shr(uint8* u128, uint32 count);
}


//__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// Large, 64bit integer classes
//
class int64;

class uint64 {
  public:
    // constructors
    uint64(uint32 low, uint32 high=0) {LowPart = low; HighPart = high;}
    uint64() {LowPart = HighPart = 0;}
    uint64(const uint64& u64) {LowPart = u64.LowPart ; HighPart = u64.HighPart;}
    uint64(const int64& i64);

    operator uint32() const {return LowPart;}

    // Unary negation operator
    uint64 operator -() { uint64 v(*this); _u64neg((uint8*)&v); return v;}

    // Postfix increment/decrement operators
    void    operator ++(int) {uint64 un(1); _u64add((uint8*)this, (uint8*)&un);  }
    void    operator --(int) {uint64 un(1); _u64sub((uint8*)this, (uint8*)&un);  }

    // Bitwise logical operators
    uint64  operator <<(uint n)
      { uint64 v(*this); _64shl((uint8*)&v,n); return v;}
    uint64  operator >>(uint n)
      { uint64 v(*this); _64shr((uint8*)&v,n); return v;}
    uint64  operator &(const uint64& other) 
      { uint64 v(*this); _64and((uint8*)&v, (uint8*)&other); return v;}
    uint64  operator |(const uint64& other) 
      { uint64 v(*this); _64or((uint8*)&v, (uint8*)&other); return v;}
    uint64  operator ^(const uint64& other)
      { uint64 v(*this); _64xor((uint8*)&v, (uint8*)&other); return v;}
    uint64  operator ~() 
      { uint64 v(*this); _64not((uint8*)&v); return v;}

    // Assignment operators
    uint64& operator <<=(uint n)
      { _64shl((uint8*)this,n); return *this;}
    uint64& operator >>=(uint n)
      { _64shr((uint8*)this,n); return *this;}
    uint64& operator &=(const uint64& other)
      { _64and((uint8*)this, (uint8*)&other); return *this;}
    uint64& operator |=(const uint64& other)
      { _64or((uint8*)this, (uint8*)&other); return *this;}
    uint64&  operator ^=(const uint64& other) 
      { _64xor((uint8*)this, (uint8*)&other); return *this;}
//    uint64&  operator ~=(const uint64& other) const //?????????
//      { _64not((uint8*)&other); return *this;}


    uint64& operator +=(const uint64& f)
        { _u64add((uint8*)this,(uint8*)&f); return *this;}
    uint64& operator +=(const int l)
        { _u64add((uint8*)this,(uint8*)&uint64(l)); return *this;}
    uint64& operator +=(const int64& f);

    uint64& operator -=(const uint64& f)
        {  _u64sub((uint8*)this,(uint8*)&f); return *this;}
    uint64& operator -=(const int l)
        {  _u64sub((uint8*)this,(uint8*)&uint64(l)); return *this;}
    uint64& operator -=(const int64& f);

    uint64& operator *=(const uint64& f);
    uint64& operator *=(int s)              {return *this *= uint64(s);  }
    uint64& operator *=(const int64& f);

    uint64& operator /=(const uint64& f)  
        {uint64 rem;_64div((uint8*)this,(uint8*)&f,(uint8*)&rem);return *this;}
    uint64& operator /=(int s)
        { return *this /= uint64(s);}
    uint64& operator /=(const int64& f);

    // Binary arithmetic operators
    friend uint64 operator +(const uint64& l, const uint64& r)
        { return uint64(l) += r;   }
    friend uint64 operator +(const uint64& l, const int64& r)
        { return uint64(l) += r;   }
    friend uint64 operator +(const int64& l, const uint64& r)
        { return uint64(r) += l;   }
    friend uint64 operator +(int l, const uint64& r)
        { return uint64(l) += r;  }
    friend uint64 operator +(const uint64& l, int r)
        { return uint64(r) += l;    }

    friend uint64 operator -(const uint64& l, const uint64& r)
        { return uint64(l) -= r;       }
    friend uint64 operator -(const uint64& l, const int64& r)
        { return uint64(l) -= r;       }
    friend uint64 operator -(const int64& l, const uint64& r)
        { return uint64(r) -= l;       }
    friend uint64 operator -(int l, const uint64& r)
        { return uint64(l) -= r;   }
    friend uint64 operator -(const uint64& l, int r)
        {  return uint64(l) -= uint64(r);  }

    friend uint64 operator *(const uint64& l, const uint64& r)
        {return uint64(l) *= r;    }
    friend uint64 operator *(const uint64& l, const int64& r)
        {return uint64(l) *= r;    }
    friend uint64 operator *(const int64& l, const uint64& r)
        {return uint64(r) *= l;    }
    friend uint64 operator *(int l, const uint64& r)
        {return uint64(l) *= r;  }
    friend uint64 operator *(const uint64& l, int r)
        {return uint64(r) *= l;  }

    friend uint64 operator /(const uint64& l, const uint64& r)
        {return uint64(l) /= r;}
    friend uint64 operator /(const uint64& l, const int64& r)
        {return uint64(l) /= r;}
    friend uint64 operator /(const int64& l, const uint64& r)
        {return uint64(l) /= r;}
    friend uint64 operator /(int l, const uint64& r)
        {return uint64(l) /= r;}
    friend uint64 operator /(const uint64& l, int r)
        {return uint64(l) /= uint64(r); }

    // Equality operators
    friend bool operator ==(const uint64& l, const uint64& r)
          { return _u64cmp((uint8*)&l,(uint8*)&r) == 0;}
    friend bool operator !=(const uint64& l, const uint64& r)
           {  return _u64cmp((uint8*)&l,(uint8*)&r) != 0;}
    friend bool operator <(const uint64& l, const uint64& r)
          {  return _u64cmp((uint8*)&l,(uint8*)&r) < 0;}
    friend bool operator >(const uint64& l, const uint64& r)
          {  return _u64cmp((uint8*)&l,(uint8*)&r) > 0;}
    friend bool operator <=(const uint64& l, const uint64& r)
          {  return !(_u64cmp((uint8*)&l,(uint8*)&r) > 0);}
    friend bool operator >=(const uint64& l, const uint64& r)
          {  return !(_u64cmp((uint8*)&l,(uint8*)&r) < 0);}

    uint32 LowPart;
    uint32 HighPart;
};


// support for 64 bit integer
class int64 {
  public:
    // constructors
    int64(uint32 low, int32 high)
      {LowPart = low; HighPart = high;}
    int64(const int32 low)
      {LowPart = low; HighPart = low<0 ? -1 : 0;}
    int64()
      {LowPart = 0; HighPart = 0;}
    int64(const int64& i64)
      {LowPart = i64.LowPart; HighPart = i64.HighPart;}
    int64(const uint64& u64)
      {LowPart = u64.LowPart; HighPart = (int32)u64.HighPart;}

    operator uint32() const {return LowPart;}

    // Unary negation operator
    int64  operator -() { int64 v(*this); _u64neg((uint8*)&v); return v;}

    // Postfix increment/decrement operators
    void  operator ++(int)
       {_u64add((uint8*)this, (uint8*)&int64(1));  }
    void  operator --(int)
       {_u64sub((uint8*)this, (uint8*)&int64(1));  }

    // Bitwise logical operators
    int64  operator <<(uint n)
      { int64 v(*this); _64shl((uint8*)&v,n); return v;}
    int64  operator >>(uint n)
      { int64 v(*this); _64shr((uint8*)&v,n); return v;}
    int64  operator &(const int64& other) 
      { int64 v(*this); _64and((uint8*)&v, (uint8*)&other); return v;}
    int64  operator |(const int64& other) 
      { int64 v(*this); _64or((uint8*)&v, (uint8*)&other); return v;}
    int64  operator ^(const int64& other) 
      { int64 v(*this); _64xor((uint8*)&v, (uint8*)&other); return v;}
    int64  operator ~() 
      { int64 v(*this); _64not((uint8*)&v); return v;}

    // Assignment operators
    int64& operator <<=(uint n)
      { _64shl((uint8*)this,n); return *this;}
    int64& operator >>=(uint n)
      { _64shr((uint8*)this,n); return *this;}
    int64& operator &=(const int64& other)
      { _64and((uint8*)this, (uint8*)&other); return *this;}
    int64& operator |=(const int64& other)
      { _64or((uint8*)this, (uint8*)&other); return *this;}
    int64&  operator ^=(const int64& other)
      { _64xor((uint8*)this, (uint8*)&other); return *this;}
//    int64&  operator ~=(const int64& other) //?????????
//      { _64not((uint8*)&other); return *this;}

    int64& operator +=(const int64& f)
       {_u64add((uint8*)this,(uint8*)&f);return *this;}
    int64& operator +=(const int l)
       {_u64add((uint8*)this,(uint8*)&int64(l));return *this;}
    int64& operator +=(const uint64& f)
       {_u64add((uint8*)this,(uint8*)&f);return *this;}
    
    int64& operator -=(const int64& f)
       {_u64sub((uint8*)this,(uint8*)&f);return *this;}
    int64& operator -=(const int l)
       {_u64sub((uint8*)this,(uint8*)&int64(l));return *this;}
    int64& operator -=(const uint64& f)
       {_u64sub((uint8*)this,(uint8*)&f);return *this;}

    int64& operator *=(const int64& f);
    int64& operator *=(int s)
      {  return *this *= int64(s);  }
    int64& operator *=(const uint64& f);

    int64& operator /=(const int64& f) 
        {int64 rem;_64div((uint8*)this, (uint8*)&f, (uint8*)&rem); return *this;}
    int64& operator /=(const uint64& f)
        {int64 rem;_64div((uint8*)this, (uint8*)&f, (uint8*)&rem); return *this;}
    int64& operator /=(int s)   { return *this /= int64(s);}

    // Binary arithmetic operators
    friend int64 operator +(const int64& l, const int64& r)
      { return int64(l) += r;   }
    friend int64 operator +(int l, const int64& r)
      { return int64(l) += r;    }
    friend int64 operator +(const int64& l, int r)
      { return int64(r) += l;    }

    friend int64 operator -(const int64& l, const int64& r)
      { return int64(l) -= r;    }
    friend int64 operator -(int l, const int64& r)
      { return int64(l) -= r;   }
    friend int64 operator -(const int64& l, int r)
      {  return int64(l) -= int64(r);  }

    friend int64 operator *(const int64& l, const int64& r)
        {return int64(l) *= r;    }
    friend int64 operator *(int l, const int64& r)
        {return int64(l) *= r;  }
    friend int64 operator *(const int64& l, int r)
        {return int64(r) *= l;  }

    friend int64 operator /(const int64& l, const int64& r)
        {return int64(l) /= r;}
    friend int64 operator /(int l, const int64& r)
        {return int64(l) /= r;}
    friend int64 operator /(const int64& l, int r)
        {return int64(l) /= int64(r); }

    // Equality operators
    friend bool operator ==(const int64& l, const int64& r)
      { return _i64cmp((uint8*)&l,(uint8*)&r) == 0;}
    friend bool operator !=(const int64& l, const int64& r)
      {  return _i64cmp((uint8*)&l,(uint8*)&r) != 0;}
    friend bool operator <(const int64& l, const int64& r)
      {  return _i64cmp((uint8*)&l,(uint8*)&r) < 0;}
    friend bool operator >(const int64& l, const int64& r)
      {  return _i64cmp((uint8*)&l,(uint8*)&r) > 0;}
    friend bool operator <=(const int64& l, const int64& r)
      {  return !(_i64cmp((uint8*)&l,(uint8*)&r) > 0);}
    friend bool operator >=(const int64& l, const int64& r)
      {  return !(_i64cmp((uint8*)&l,(uint8*)&r) < 0);}

    uint32 LowPart;
    int32  HighPart;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//__OWL_END_NAMESPACE

#else
//__OWL_BEGIN_NAMESPACE
typedef __int64    int64;

//JJH to get rid of warning in basetsd.h in wine directory
#if defined(WINELIB)
typedef unsigned long long uint64;
#else
typedef unsigned  __int64  uint64;
#endif

//__OWL_END_NAMESPACE
#endif


#if !defined(BI_128BIT_NATIVE)

extern "C" {
// 128 bit assembler support
// unsigned
_OWLFUNC(void _stdcall) _u128add(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _u128sub(uint8* l, uint8* r);
_OWLFUNC(int _stdcall) _u128cmp(uint8* l, uint8* r); // return <0, ==0, >0
_OWLFUNC(int _stdcall) _i128cmp(uint8* l, uint8* r);  // return <0, ==0, >0
_OWLFUNC(void _stdcall) _u128mul(uint8* l, uint8* r, uint8* res);
_OWLFUNC(void _stdcall) _128div(uint8* divident, uint8* divisor, uint8* remainder);
_OWLFUNC(void _stdcall) _u128neg(uint8* n);

_OWLFUNC(void _stdcall) _128and(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _128div(uint8* l, uint8* r, uint8* rem);
_OWLFUNC(void _stdcall) _128and(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _128or(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _128xor(uint8* l, uint8* r);
_OWLFUNC(void _stdcall) _128not(uint8* l);
_OWLFUNC(void _stdcall) _128shl(uint8* u128, uint32 count);
_OWLFUNC(void _stdcall) _128shr(uint8* u128, uint32 count);
}

//__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

// 128 bits support
class int128;
class uint128;

// Binary arithmetic operators
inline uint128 operator +(const uint128& l, const uint128& r);
inline uint128 operator +(const uint128& l, const int128& r);
inline uint128 operator +(const int128& l, const uint128& r);
inline uint128 operator +(const uint128& l, const uint64& r);
inline uint128 operator +(const uint64& l, const uint128& r);
inline uint128 operator +(const uint128& l, const int64& r);
inline uint128 operator +(const int64& l, const uint128& r);
inline uint128 operator +(int l, const uint128& r);
inline uint128 operator +(const uint128& l, int r);

inline uint128 operator -(const uint128& l, const uint128& r);
inline uint128 operator -(const int128& l, const uint128& r);
inline uint128 operator -(const uint128& l, const int128& r);
inline uint128 operator -(const uint64& l, const uint128& r);
inline uint128 operator -(const uint128& l, const uint64& r);
inline uint128 operator -(const int64& l, const uint128& r);
inline uint128 operator -(const uint128& l, const int64& r);
inline uint128 operator -(int l, const uint128& r);
inline uint128 operator -(const uint128& l, int r);

inline uint128 operator *(const uint128& l, const uint128& r);
inline uint128 operator *(const int128& l, const uint128& r);
inline uint128 operator *(const uint128& l, const int128& r);
inline uint128 operator *(const uint64& l, const uint128& r);
inline uint128 operator *(const uint128& l, const uint64& r);
inline uint128 operator *(const int64& l, const uint128& r);
inline uint128 operator *(const uint128& l, const int64& r);
inline uint128 operator *(int l, const uint128& r);
inline uint128 operator *(const uint128& l, int r);

inline uint128 operator /(const uint128& l, const uint128& r);
inline uint128 operator /(const int128& l, const uint128& r);
inline uint128 operator /(const uint128& l, const int128& r);
inline uint128 operator /(const uint64& l, const uint128& r);
inline uint128 operator /(const uint128& l, const uint64& r);
inline uint128 operator /(const int64& l, const uint128& r);
inline uint128 operator /(const uint128& l, const int64& r);
inline uint128 operator /(int l, const uint128& r);
inline uint128 operator /(const uint128& l, int r);

// Equality operators
inline bool operator ==(const uint128& l, const uint128& r);
inline bool operator !=(const uint128& l, const uint128& r);
inline bool operator <(const uint128& l, const uint128& r);
inline bool operator >(const uint128& l, const uint128& r);
inline bool operator <=(const uint128& l, const uint128& r);
inline bool operator >=(const uint128& l, const uint128& r);
//////////////////
//    
class uint128{
  public:
    // constructors
    uint128(const uint32 part);
    uint128(const uint64& part);
    uint128(const int64& part);
    uint128(const int128& part);
    uint128(uint64& low, uint64& high);
    uint128(const uint128& u128);
    uint128();

    operator uint64() const;
    operator uint32() const;

    // Unary negation operator
    uint128  operator -();

    // Postfix increment/decrement operators
    void  operator ++(int);
    void  operator --(int);

    // Bitwise logical operators
    uint128  operator <<(uint n);
    uint128  operator >>(uint n);
    uint128  operator &(const uint128& other);
    uint128  operator |(const uint128& other); 
    uint128 operator ^(const uint128& other); 
    uint128  operator ~();

    // Assignment operators
    uint128& operator <<=(uint n);
    uint128& operator >>=(uint n);
    uint128& operator &=(const uint128& other);
    uint128& operator |=(const uint128& other);
    uint128& operator ^=(const uint128& other); 
//    uint128& operator ~=(const uint128& other) //?????????
//      { _128not((uint8*)&other); return *this;}


    uint128& operator +=(const uint128& f);   
    uint128& operator +=(const int128& f);   
    uint128& operator +=(const uint64& f);   
    uint128& operator +=(const int64& f);
    uint128& operator +=(int f);       

    uint128& operator -=(const uint128& f);
    uint128& operator -=(const int128& f);
    uint128& operator -=(const uint64& f);
    uint128& operator -=(const int64& f);
    uint128& operator -=(int l);

    uint128& operator *=(const uint128& f);
    uint128& operator *=(const int128& f);
    uint128& operator *=(const uint64& f);
    uint128& operator *=(const int64& f);
    uint128& operator *=(int s);

    uint128& operator /=(const uint128& f);
    uint128& operator /=(const int128& f);
    uint128& operator /=(const uint64& f);
    uint128& operator /=(const int64& f);
    uint128& operator /=(int f);  

    // Binary arithmetic operators
    friend uint128 operator +(const uint128& l, const uint128& r);
    friend uint128 operator +(const uint128& l, const int128& r);
    friend uint128 operator +(const int128& l, const uint128& r);
    friend uint128 operator +(const uint128& l, const uint64& r);
    friend uint128 operator +(const uint64& l, const uint128& r);
    friend uint128 operator +(const uint128& l, const int64& r);
    friend uint128 operator +(const int64& l, const uint128& r);
    friend uint128 operator +(int l, const uint128& r);
    friend uint128 operator +(const uint128& l, int r);

    friend uint128 operator -(const uint128& l, const uint128& r);
    friend uint128 operator -(const int128& l, const uint128& r);
    friend uint128 operator -(const uint128& l, const int128& r);
    friend uint128 operator -(const uint64& l, const uint128& r);
    friend uint128 operator -(const uint128& l, const uint64& r);
    friend uint128 operator -(const int64& l, const uint128& r);
    friend uint128 operator -(const uint128& l, const int64& r);
    friend uint128 operator -(int l, const uint128& r);
    friend uint128 operator -(const uint128& l, int r);

    friend uint128 operator *(const uint128& l, const uint128& r);
    friend uint128 operator *(const int128& l, const uint128& r);
    friend uint128 operator *(const uint128& l, const int128& r);
    friend uint128 operator *(const uint64& l, const uint128& r);
    friend uint128 operator *(const uint128& l, const uint64& r);
    friend uint128 operator *(const int64& l, const uint128& r);
    friend uint128 operator *(const uint128& l, const int64& r);
    friend uint128 operator *(int l, const uint128& r);
    friend uint128 operator *(const uint128& l, int r);

    friend uint128 operator /(const uint128& l, const uint128& r);
    friend uint128 operator /(const int128& l, const uint128& r);
    friend uint128 operator /(const uint128& l, const int128& r);
    friend uint128 operator /(const uint64& l, const uint128& r);
    friend uint128 operator /(const uint128& l, const uint64& r);
    friend uint128 operator /(const int64& l, const uint128& r);
    friend uint128 operator /(const uint128& l, const int64& r);
    friend uint128 operator /(int l, const uint128& r);
    friend uint128 operator /(const uint128& l, int r);

    // Equality operators
    friend bool operator ==(const uint128& l, const uint128& r);
    friend bool operator !=(const uint128& l, const uint128& r);
    friend bool operator <(const uint128& l, const uint128& r);
    friend bool operator >(const uint128& l, const uint128& r);
    friend bool operator <=(const uint128& l, const uint128& r);
    friend bool operator >=(const uint128& l, const uint128& r);


    uint64 LowPart;
    uint64 HighPart;

};
///////////////////////////////////////////
inline int128 operator +(const int128& l, const int128& r);
inline int128 operator +(const int128& l, const uint64& r);
inline int128 operator +(const uint64& l, const int128& r);
inline int128 operator +(const int128& l, const int64& r);
inline int128 operator +(const int64& l, const int128& r);
inline int128 operator +(const int32 l, const int128& r);
inline int128 operator +(const int128& l, const int32 r);


inline int128 operator -(const int128& l, const int128& r);
inline int128 operator -(const uint64& l, const int128& r);
inline int128 operator -(const int128& l, const uint64& r);
inline int128 operator -(const int64& l, const int128& r);
inline int128 operator -(const int128& l, const int64& r);
inline int128 operator -(const int32 l, const int128& r);
inline int128 operator -(const int128& l, const int32 r);


inline int128 operator *(const int128& l, const int128& r);
inline int128 operator *(const uint64& l, const int128& r);
inline int128 operator *(const int128& l, const uint64& r);
inline int128 operator *(const int64& l, const int128& r);
inline int128 operator *(const int128& l, const int64& r);
inline int128 operator *(const int32 l, const int128& r);
inline int128 operator *(const int128& l, const int32 r);

inline int128 operator /(const int128& l, const int128& r);
inline int128 operator /(const uint64& l, const int128& r);
inline int128 operator /(const int128& l, const uint64& r);
inline int128 operator /(const int64& l, const int128& r);
inline int128 operator /(const int128& l, const int64& r);
inline int128 operator /(const int32 l, const int128& r);
inline int128 operator /(const int128& l, const int32 r);

// Equality operators
inline bool operator ==(const int128& l, const int128& r);
inline bool operator !=(const int128& l, const int128& r);
inline bool operator <(const int128& l, const int128& r);
inline bool operator >(const int128& l, const int128& r);
inline bool operator <=(const int128& l, const int128& r);
inline bool operator >=(const int128& l, const int128& r);
///////////////////////////////////////////
// 128 bits support
class int128{
  public:
    // constructors
    int128(const int32 low);
    int128(const int64& low);
    int128(const uint64& low);
    int128(uint64& low, int64& high);
    int128(const uint128& i128);
    int128(const int128& i128);
    int128();

    operator uint64() const;
    operator uint32() const;

    int128  operator -();

    // Postfix increment/decrement operators
    void    operator ++(int); 
    void    operator --(int); 

    // Bitwise logical operators
    int128  operator <<(uint n);
    int128  operator >>(uint n);
    int128  operator &(const int128& other);
    int128  operator |(const int128& other); 
    int128 operator ^(const int128& other); 
    int128  operator ~();

    // Assignment operators
    int128& operator <<=(uint n);
    int128& operator >>=(uint n);
    int128& operator &=(const int128& other);
    int128& operator |=(const int128& other);
    int128& operator ^=(const int128& other);

    int128& operator +=(const int128& l);
    int128& operator +=(const uint128& l);
    int128& operator +=(const uint64& l);
    int128& operator +=(const int64& l);
    int128& operator +=(const int l);

    int128& operator -=(const int128& l);
    int128& operator -=(const uint128& l);
    int128& operator -=(const uint64& l);
    int128& operator -=(const int64& l);
    int128& operator -=(const int l);

    int128& operator *=(const int128& l);
    int128& operator *=(const uint128& l);
    int128& operator *=(const uint64& l);
    int128& operator *=(const int64& l);
    int128& operator *=(const int l);

    int128& operator /=(const int128& l);
    int128& operator /=(const uint128& l);
    int128& operator /=(const uint64& l);
    int128& operator /=(const int64& l);
    int128& operator /=(const int32 l);  

    // Binary arithmetic operators
    friend int128 operator +(const int128& l, const int128& r);
    friend int128 operator +(const int128& l, const uint64& r);
    friend int128 operator +(const uint64& l, const int128& r);
    friend int128 operator +(const int128& l, const int64& r);
    friend int128 operator +(const int64& l, const int128& r);
    friend int128 operator +(const int32 l, const int128& r);
    friend int128 operator +(const int128& l, const int32 r);


    friend int128 operator -(const int128& l, const int128& r);
    friend int128 operator -(const uint64& l, const int128& r);
    friend int128 operator -(const int128& l, const uint64& r);
    friend int128 operator -(const int64& l, const int128& r);
    friend int128 operator -(const int128& l, const int64& r);
    friend int128 operator -(const int32 l, const int128& r);
    friend int128 operator -(const int128& l, const int32 r);


    friend int128 operator *(const int128& l, const int128& r);
    friend int128 operator *(const uint64& l, const int128& r);
    friend int128 operator *(const int128& l, const uint64& r);
    friend int128 operator *(const int64& l, const int128& r);
    friend int128 operator *(const int128& l, const int64& r);
    friend int128 operator *(const int32 l, const int128& r);
    friend int128 operator *(const int128& l, const int32 r);

    friend int128 operator /(const int128& l, const int128& r);
    friend int128 operator /(const uint64& l, const int128& r);
    friend int128 operator /(const int128& l, const uint64& r);
    friend int128 operator /(const int64& l, const int128& r);
    friend int128 operator /(const int128& l, const int64& r);
    friend int128 operator /(const int32 l, const int128& r);
    friend int128 operator /(const int128& l, const int32 r);

    // Equality operators
    friend bool operator ==(const int128& l, const int128& r);
    friend bool operator !=(const int128& l, const int128& r);
    friend bool operator <(const int128& l, const int128& r);
    friend bool operator >(const int128& l, const int128& r);
    friend bool operator <=(const int128& l, const int128& r);
    friend bool operator >=(const int128& l, const int128& r);

    uint64   LowPart;
    int64   HighPart;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//__OWL_END_NAMESPACE

#else
//__OWL_BEGIN_NAMESPACE
typedef __int128            int128    //?????????
typedef unsigned __int128    uint128   //?????????
//__OWL_END_NAMESPACE
#endif

#if !defined(BI_64BIT_NATIVE)
//__OWL_BEGIN_NAMESPACE
// inlines
inline uint64::uint64(const int64& i64){
  LowPart = i64.LowPart;
  HighPart = (uint32)i64.HighPart;
}
inline uint64& uint64::operator +=(const int64& f){
  _u64add((uint8*)this,(uint8*)&f); return *this;
}
inline uint64& uint64::operator -=(const int64& f){
  _u64sub((uint8*)this,(uint8*)&f); return *this;
}
inline uint64& uint64::operator *=(const int64& f){
  int128 r;
  _u64mul((uint8*)this, (uint8*)&f, (uint8*)&r);
  LowPart   = r.LowPart.LowPart;
  HighPart   = r.LowPart.HighPart;
  return *this;
}
inline uint64& uint64::operator /=(const int64& f){
  int64 rem;
  _128div((uint8*)this, (uint8*)&f, (uint8*)&rem);
  return *this;
}
inline uint64& uint64::operator *=(const uint64& f){
  uint128 r;
  _u64mul((uint8*)this, (uint8*)&f, (uint8*)&r);
  LowPart   = r.LowPart.LowPart;
  HighPart   = r.LowPart.HighPart;
  return *this;
}
inline int64& int64::operator *=(const int64& f){
  int128 r;
  _u64mul((uint8*)this, (uint8*)&f, (uint8*)&r);
  LowPart   = r.LowPart.LowPart;
  HighPart   = r.LowPart.HighPart;
  return *this;
}
inline int64& int64::operator *=(const uint64& f){
  int128 r;
  _u64mul((uint8*)this, (uint8*)&f, (uint8*)&r);
  LowPart   = r.LowPart.LowPart;
  HighPart   = r.LowPart.HighPart;
  return *this;
}
//__OWL_END_NAMESPACE
#endif

#if !defined(BI_128BIT_NATIVE)

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif



//__OWL_BEGIN_NAMESPACE
inline uint128::uint128(const int128& part){
 LowPart  = part.LowPart;
 HighPart = part.HighPart;
}
inline uint128::uint128(const uint32 part) { 
  LowPart = uint64(part); HighPart = 0;
}
inline uint128::uint128(const uint64& part) { 
  LowPart = part; HighPart = 0;
}
inline uint128::uint128(const int64& part) { 
  LowPart = part; HighPart = 0;
}
inline uint128::uint128(uint64& low, uint64& high) {
  LowPart = low; HighPart = high;
}
inline uint128::uint128(const uint128& u128) {
  LowPart = u128.LowPart ; HighPart = u128.HighPart;
}
inline uint128::uint128() {
  LowPart = HighPart = 0;
}
inline uint128::operator uint64() const{
  return LowPart;
}
inline uint128::operator uint32() const{
  return (uint32)LowPart;
}
inline uint128  uint128::operator -(){ 
  uint128 v(*this); 
  _u128neg((uint8*)&v); 
  return v;
}
inline void  uint128::operator ++(int){
  uint128 ul(1ul); 
  _u128add((uint8*)this, (uint8*)&ul);  
}
inline void  uint128::operator --(int) {
  uint128 ul(1ul); 
  _u128sub((uint8*)this, (uint8*)&ul);  
}
inline uint128  uint128::operator <<(uint n){ 
  uint128 v(*this); 
  _128shl((uint8*)&v,n); 
  return v;
}
inline uint128  uint128::operator >>(uint n){ 
  uint128 v(*this); 
  _128shr((uint8*)&v,n); 
  return v;
}
inline uint128  uint128::operator &(const uint128& other){ 
  uint128 v(*this); 
  _128and((uint8*)&v, (uint8*)&other); 
  return v;
}
inline uint128  uint128::operator |(const uint128& other){ 
  uint128 v(*this); 
  _128or((uint8*)&v, (uint8*)&other); 
  return v;
}
inline uint128 uint128::operator ^(const uint128& other){ 
  uint128 v(*this); 
  _128xor((uint8*)&v, (uint8*)&other); 
  return v;
}
inline uint128  uint128::operator ~(){ 
  uint128 v(*this); 
  _128not((uint8*)&v); 
  return v;
}
inline uint128& uint128::operator <<=(uint n){ 
  _128shl((uint8*)this,n); 
  return *this;
}
inline uint128& uint128::operator >>=(uint n){ 
  _128shr((uint8*)this,n); 
  return *this;
}
inline uint128& uint128::operator &=(const uint128& other){ 
  _128and((uint8*)this, (uint8*)&other); 
  return *this;
}
inline uint128& uint128::operator |=(const uint128& other){ 
  _128or((uint8*)this, (uint8*)&other); 
  return *this;
}
inline uint128& uint128::operator ^=(const uint128& other){
  _128xor((uint8*)this, (uint8*)&other); 
  return *this;
}
inline uint128& uint128::operator +=(const uint128& f){ 
  _u128add((uint8*)this,(uint8*)&f); 
  return *this;
}
inline uint128& uint128::operator +=(const int128& f) { 
  _u128add((uint8*)this,(uint8*)&f); 
  return *this;
}
inline uint128& uint128::operator +=(const uint64& f) { 
  uint128 uf(f); 
  _u128add((uint8*)this,(uint8*)&uf); 
  return *this;
}
inline uint128& uint128::operator +=(const int64& f){
  uint128 uf(f); 
  _u128add((uint8*)this,(uint8*)&uf); 
  return *this;
}
inline uint128& uint128::operator +=(int l){ 
  uint128 ul((int64)l);
  _u128add((uint8*)this,(uint8*)&ul);
  return *this;
}
inline uint128& uint128::operator -=(const uint128& f){  
  _u128sub((uint8*)this,(uint8*)&f); 
  return *this;
}
inline uint128& uint128::operator -=(const int128& f){  
  _u128sub((uint8*)this,(uint8*)&f); 
  return *this;
}
inline uint128& uint128::operator -=(const uint64& f){  
  uint128 uf(f); 
  _u128sub((uint8*)this,(uint8*)&uf); 
  return *this;
}
inline uint128& uint128::operator -=(const int64& f){  
  uint128 uf(f); 
  _u128sub((uint8*)this,(uint8*)&uf); 
  return *this;
}
inline uint128& uint128::operator -=(int l){  
  uint128 ul((int64)l); 
  _u128sub((uint8*)this,(uint8*)&ul); 
  return *this;
}

inline uint128& uint128::operator *=(const uint128& f){  
  uint128 v(*this);
  _u128mul((uint8*)&v,(uint8*)&f,(uint8*)this);
  return *this; 
}
inline uint128& uint128::operator *=(const uint64& f){  
  uint128 v(*this); 
  uint128 uf(f); 
  _u128mul((uint8*)&v, (uint8*)&uf,(uint8*)this);
  return *this; 
}
inline uint128& uint128::operator *=(const int64& f){  
  uint128 v(*this); 
  uint128 uf(f); 
  _u128mul((uint8*)&v, (uint8*)&uf,(uint8*)this);
  return *this; 
}
inline uint128& uint128::operator *=(int s){  
  uint128 v(*this); 
  uint128 uf((int64)s); 
  _u128mul((uint8*)&v, (uint8*)&uf,(uint8*)this);
  return *this; 
}
inline uint128& uint128::operator /=(const uint128& f){  
  uint128 rem; 
  _128div((uint8*)this, (uint8*)&f, (uint8*)&rem); 
  return *this;
}
inline uint128& uint128::operator /=(const int128& f){  
  uint128 rem; 
  _128div((uint8*)this, (uint8*)&f, (uint8*)&rem); 
  return *this;
}
inline uint128& uint128::operator /=(const uint64& f){  
  uint128 rem; 
  uint128 uf(f); 
  _128div((uint8*)this, (uint8*)&uf, (uint8*)&rem); 
  return *this;
}
inline uint128& uint128::operator /=(const int64& f){
  uint128 rem; 
  uint128 uf(f); 
  _128div((uint8*)this, (uint8*)&uf, (uint8*)&rem); 
  return *this;
}
inline uint128& uint128::operator /=(int f)  {  
  uint128 rem; 
  uint128 uf((int64)f);
  _128div((uint8*)this, (uint8*)&uf, (uint8*)&rem); 
  return *this;
}
inline uint128& uint128::operator *=(const int128& f){
  uint128 v(*this);
  _u128mul((uint8*)&v,(uint8*)&f,(uint8*)this); 
  return *this;
}
inline uint128 operator +(const uint128& l, const uint128& r)
  { return uint128(l) += r;   }
inline uint128 operator +(const uint128& l, const int128& r)
  { return uint128(r) += l;   }
inline uint128 operator +(const int128& l, const uint128& r)
  { return uint128(l) += r;   }
inline uint128 operator +(const uint128& l, const uint64& r)
  { return uint128(r) += l;   }
inline uint128 operator +(const uint64& l, const uint128& r)
  { return uint128(l) += r;   }
inline uint128 operator +(const uint128& l, const int64& r)
  { return uint128(r) += l;   }
inline uint128 operator +(const int64& l, const uint128& r)
  { return uint128(l) += r;   }
inline uint128 operator +(int l, const uint128& r)
  { return uint128(int64(l)) += r;  }
inline uint128 operator +(const uint128& l, int r)
   { return uint128(int64(r)) += l;    }
inline uint128 operator -(const uint128& l, const uint128& r)
  { return uint128(l) -= r;       }
inline uint128 operator -(const int128& l, const uint128& r)
  { return uint128(l) -= r;       }
inline uint128 operator -(const uint128& l, const int128& r)
  { return uint128(r) -= l;       }
inline uint128 operator -(const uint64& l, const uint128& r)
  { return uint128(l) -= r;       }
inline uint128 operator -(const uint128& l, const uint64& r)
  { return uint128(r) -= l;       }
inline uint128 operator -(const int64& l, const uint128& r)
  { return uint128(l) -= r;       }
inline uint128 operator -(const uint128& l, const int64& r)
  { return uint128(r) -= l;       }
inline uint128 operator -(int l, const uint128& r)
  { return uint128(int64(l)) -= r;   }
inline uint128 operator -(const uint128& l, int r)
  {  return uint128(l) -= uint128(int64(r));  }
inline uint128 operator *(const uint128& l, const uint128& r)
  { return uint128(l) *= r;  }
inline uint128 operator *(const int128& l, const uint128& r)
  { return uint128(l) *= r;  }
inline uint128 operator *(const uint128& l, const int128& r)
  { return uint128(r) *= l;  }
inline uint128 operator *(const uint64& l, const uint128& r)
  { return uint128(l) *= r;  }
inline uint128 operator *(const uint128& l, const uint64& r)
  { return uint128(r) *= l;  }
inline uint128 operator *(const int64& l, const uint128& r)
  { return uint128(l) *= r;  }
inline uint128 operator *(const uint128& l, const int64& r)
  { return uint128(r) *= l;  }
inline uint128 operator *(int l, const uint128& r)
  { return uint128(int64(l)) *= r;  }
inline uint128 operator *(const uint128& l, int r)
  { return uint128(int64(r)) *= l;  }
inline uint128 operator /(const uint128& l, const uint128& r)
  { return uint128(l) /= r;  }
inline uint128 operator /(const int128& l, const uint128& r)
  { return uint128(l) /= r;  }
inline uint128 operator /(const uint128& l, const int128& r)
  { return uint128(l) /= uint128(r); }
inline uint128 operator /(const uint64& l, const uint128& r)
  { return uint128(l) /= r;  }
inline uint128 operator /(const uint128& l, const uint64& r)
  { return uint128(l) /= uint128(r); }
inline uint128 operator /(const int64& l, const uint128& r)
  { return uint128(l) /= r;  }
inline uint128 operator /(const uint128& l, const int64& r)
  { return uint128(l) /= uint128(r); }
inline uint128 operator /(int l, const uint128& r)
  { return uint128(int64(l)) /= r;  }
inline uint128 operator /(const uint128& l, int r)
  { return uint128(l) /= uint128(int64(r)); }
inline bool operator ==(const uint128& l, const uint128& r)
  { return _u128cmp((uint8*)&l,(uint8*)&r) == 0;}
inline bool operator !=(const uint128& l, const uint128& r)
   {  return _u128cmp((uint8*)&l,(uint8*)&r) != 0;}
inline bool operator <(const uint128& l, const uint128& r)
  {  return _u128cmp((uint8*)&l,(uint8*)&r) < 0;}
inline bool operator >(const uint128& l, const uint128& r)
  {  return _u128cmp((uint8*)&l,(uint8*)&r) > 0;}
inline bool operator <=(const uint128& l, const uint128& r)
  {  return !(_u128cmp((uint8*)&l,(uint8*)&r) > 0);}
inline bool operator >=(const uint128& l, const uint128& r)
  {  return !(_u128cmp((uint8*)&l,(uint8*)&r) < 0);}

////////////////////////////////////////////////////////////////////////
    // constructors
inline int128::int128(const int32 low)
{
#if !defined(BI_64BIT_NATIVE)
  LowPart = uint64(low,0);
  HighPart = (low<0 ? int64(0,-1) : int64(0));
#else
  LowPart = uint64(low);
  HighPart = (low<0 ? int64(-1) : int64(0));
#endif
}
inline int128::int128(const int64& low)
{
#if !defined(BI_64BIT_NATIVE)
  LowPart = uint64((uint32)low.LowPart, (uint32)low.HighPart);
  HighPart = (low<int64(0) ? int64(0,-1) : int64(0));
#else
  LowPart = uint64(low);
  HighPart = (low<int64(0) ? int64(-1) : int64(0));
#endif
}
inline int128::int128(const uint64& low)
  {  LowPart = low;  HighPart = 0; }
inline int128::int128(uint64& low, int64& high)
  {LowPart = low; HighPart = high;}
inline int128::int128(const uint128& i128)
  {LowPart = i128.LowPart; HighPart = int64(i128.HighPart);}
inline int128::int128(const int128& i128)
  {LowPart = i128.LowPart ; HighPart = i128.HighPart;}
inline int128::int128() {LowPart = 0; HighPart = 0;}
inline int128::operator uint64() const 
  {return LowPart;}
inline int128::operator uint32() const 
  {return (uint32)LowPart;}
inline int128  int128::operator -() { 
  int128 v(*this); 
  _u128neg((uint8*)&v); 
  return v;
}
// Postfix increment/decrement operators
inline void int128::operator ++(int){ 
  int128 il(1l); 
  _u128add((uint8*)this, (uint8*)&il);  
}
inline void int128::operator --(int){ 
  int128 il(1l); 
  _u128sub((uint8*)this, (uint8*)&il);  
}
inline int128  int128::operator <<(uint n)
{ 
  int128 v(*this); 
  _128shl((uint8*)&v,n); 
  return v;
}
inline int128  int128::operator >>(uint n)
{ 
  int128 v(*this); 
  _128shr((uint8*)&v,n); 
  return v;
}
inline int128  int128::operator &(const int128& other) 
{ 
  int128 v(*this); 
  _128and((uint8*)&v, (uint8*)&other); 
  return v;
}
inline int128  int128::operator |(const int128& other) 
{ 
  int128 v(*this); 
  _128or((uint8*)&v, (uint8*)&other); 
  return v;
}
inline int128 int128::operator ^(const int128& other) 
{ 
  int128 v(*this); 
  _128xor((uint8*)&v, (uint8*)&other); 
  return v;
}
inline int128  int128::operator ~()
{ 
  int128 v(*this); 
  _128not((uint8*)&v); 
  return v;
}
inline int128& int128::operator <<=(uint n)
  { _128shl((uint8*)this,n); return *this;}
inline int128& int128::operator >>=(uint n)
  { _128shr((uint8*)this,n); return *this;}
inline int128& int128::operator &=(const int128& other)
  { _128and((uint8*)this, (uint8*)&other); return *this;}
inline int128& int128::operator |=(const int128& other)
  { _128or((uint8*)this, (uint8*)&other); return *this;}
inline int128& int128::operator ^=(const int128& other) 
  { _128xor((uint8*)this, (uint8*)&other); return *this;}
inline int128& int128::operator +=(const int128& l)
  { _u128add((uint8*)this,(uint8*)&l); return *this; }
inline int128& int128::operator +=(const uint128& l)
  { _u128add((uint8*)this,(uint8*)&l); return *this; }
inline int128& int128::operator +=(const uint64& l)
{ 
  int128 il(l); 
  _u128add((uint8*)this,(uint8*)&il); 
  return *this; 
}
inline int128& int128::operator +=(const int64& l)
{ 
  int128 il(l); 
  _u128add((uint8*)this,(uint8*)&il); 
  return *this; 
}
inline int128& int128::operator +=(const int l)
{ 
  int128 il((int64)l); 
  _u128add((uint8*)this,(uint8*)&il); //?????????????????
  return *this; 
}
inline int128& int128::operator -=(const int128& l)   
  {  _u128sub((uint8*)this,(uint8*)&l); return *this;}
inline int128& int128::operator -=(const uint128& l)
  {  _u128sub((uint8*)this,(uint8*)&l); return *this;}
inline int128& int128::operator -=(const uint64& l)
{  
  int128 il(l); 
  _u128sub((uint8*)this,(uint8*)&il); 
  return *this;
}
inline int128& int128::operator -=(const int64& l)
{  
  int128 il(l); 
  _u128sub((uint8*)this,(uint8*)&il); 
  return *this;
}
inline int128& int128::operator -=(const int l)
{
  int128 il((int64)l); 
  _u128sub((uint8*)this,(uint8*)&il); //?????????????????
  return *this;
}
inline int128& int128::operator *=(const int128& l)
{  
  int128 v(*this); 
  _u128mul((uint8*)&v,(uint8*)&l,(uint8*)this);
  return *this; 
}
inline int128& int128::operator *=(const uint128& l)
{  
  int128 v(*this); 
  _u128mul((uint8*)&v,(uint8*)&l,(uint8*)this);
  return *this; 
}
inline int128& int128::operator *=(const uint64& l)
{  
  int128 v(*this); 
  int128 il(l);
  _u128mul((uint8*)&v,(uint8*)&il,(uint8*)this);
  return *this; 
}
inline int128& int128::operator *=(const int64& l)
{  
  int128 v(*this); 
  int128 il(l);
  _u128mul((uint8*)&v,(uint8*)&il,(uint8*)this);
  return *this; 
}
inline int128& int128::operator *=(const int l)
{  
  int128 v(*this); 
  //int128 il(int64(l));
  int128 il((int64)l);
  _u128mul((uint8*)&v,(uint8*)&il,(uint8*)this);//?????????????????
  return *this; 
}
inline int128& int128::operator /=(const int128& l)
{  
  int128 rem;
  _128div((uint8*)this, (uint8*)&l, (uint8*)&rem); 
  return *this;
}
inline int128& int128::operator /=(const uint128& l)
{  
  int128 rem;
  _128div((uint8*)this, (uint8*)&l, (uint8*)&rem); 
  return *this;
}
inline int128& int128::operator /=(const uint64& l)
{  
  int128 rem; 
  int128 il(l);
  _128div((uint8*)this, (uint8*)&il, (uint8*)&rem); 
  return *this;
}
inline int128& int128::operator /=(const int64& l)
{  
  int128 rem; 
  int128 il(l);
  _128div((uint8*)this, (uint8*)&il, (uint8*)&rem); 
  return *this;
}
inline int128& int128::operator /=(const int32 l)  
{  
  int128 rem; int128 il(l);
  _128div((uint8*)this, (uint8*)&il, (uint8*)&rem); 
  return *this;
}
// Binary arithmetic operators
inline int128 operator +(const int128& l, const int128& r)
  { return int128(l) += r;   }
inline int128 operator +(const int128& l, const uint64& r)
  { return int128(r) += l;   }
inline int128 operator +(const uint64& l, const int128& r)
  { return int128(l) += r;   }
inline int128 operator +(const int128& l, const int64& r)
  { return int128(r) += l;   }
inline int128 operator +(const int64& l, const int128& r)
  { return int128(l) += r;   }
inline int128 operator +(const int32 l, const int128& r)
  { return int128(l) += r;  }
inline int128 operator +(const int128& l, const int32 r)
  { return int128(r) += l;    }
inline int128 operator -(const int128& l, const int128& r)
  { return int128(l) -= r;       }
inline int128 operator -(const uint64& l, const int128& r)
  { return int128(l) -= r;       }
inline int128 operator -(const int128& l, const uint64& r)
  { return int128(r) -= l;       }
inline int128 operator -(const int64& l, const int128& r)
  { return int128(l) -= r;       }
inline int128 operator -(const int128& l, const int64& r)
  { return int128(r) -= l;       }
inline int128 operator -(const int32 l, const int128& r)
   { return int128(l) -= r;   }
inline int128 operator -(const int128& l, const int32 r)
  {  return int128(l) -= int128(r);  }
inline int128 operator *(const int128& l, const int128& r)
  { return int128(l) *= r;  }
inline int128 operator *(const uint64& l, const int128& r)
  { return int128(l) *= r;  }
inline int128 operator *(const int128& l, const uint64& r)
  { return int128(r) *= l;  }
inline int128 operator *(const int64& l, const int128& r)
  { return int128(l) *= r;  }
inline int128 operator *(const int128& l, const int64& r)
  { return int128(r) *= l;  }
inline int128 operator *(const int32 l, const int128& r)
  {return int128(l) *= r;  }
inline int128 operator *(const int128& l, const int32 r)
  {return int128(r) *= l;  }
inline int128 operator /(const int128& l, const int128& r)
  {return int128(l) /= r;}
inline int128 operator /(const uint64& l, const int128& r)
  { return int128(l) /= r;  }
inline int128 operator /(const int128& l, const uint64& r)
  { return int128(l) /= int128(r); }
inline int128 operator /(const int64& l, const int128& r)
  { return int128(l) /= r;  }
inline int128 operator /(const int128& l, const int64& r)
  { return int128(l) /= int128(r); }
inline int128 operator /(const int32 l, const int128& r)
  { return int128(l) /= r;  }
inline int128 operator /(const int128& l, const int32 r)
  { return int128(l) /= int128(r); }
// Equality operators
inline bool operator ==(const int128& l, const int128& r)
  { return _i128cmp((uint8*)&l,(uint8*)&r) == 0;}
inline bool operator !=(const int128& l, const int128& r)
  {  return _i128cmp((uint8*)&l,(uint8*)&r) != 0;}
inline bool operator <(const int128& l, const int128& r)
  {  return _i128cmp((uint8*)&l,(uint8*)&r) < 0;}
inline bool operator >(const int128& l, const int128& r)
  {  return _i128cmp((uint8*)&l,(uint8*)&r) > 0;}
inline bool operator <=(const int128& l, const int128& r)
  {  return !(_i128cmp((uint8*)&l,(uint8*)&r) > 0);}
inline bool operator >=(const int128& l, const int128& r)
  {  return !(_i128cmp((uint8*)&l,(uint8*)&r) < 0);}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//__OWL_END_NAMESPACE
#endif


#endif // OWL_PRIVATE_NUMBER_H
