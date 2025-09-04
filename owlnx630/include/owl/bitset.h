//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of a bit set and a character set
//----------------------------------------------------------------------------

#if !defined(OWL_BITSET_H)
#define OWL_BITSET_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


__OWL_BEGIN_NAMESPACE

class _OWLCLASS opstream;
class _OWLCLASS ipstream;

//
// class TBitSet
// ~~~~~ ~~~~~~~
// Simplifies setting and testing bits in a 32 count array of uint8 (32 bytes).
//

template <class T = uint8>
class _OWLCLASS TBitSet {
  public:
    TBitSet();
    TBitSet(const TBitSet& src);

    bool Has(T item) const;

    TBitSet operator ~() const;

    void DisableItem(T item);
    void EnableItem(T item);
    TBitSet& operator +=(T item);
    TBitSet& operator -=(T item);

    void DisableItem(const TBitSet& bs);
    void EnableItem(const TBitSet& bs);
    TBitSet& operator +=(const TBitSet& bs);
    TBitSet& operator |=(const TBitSet& bs);
    TBitSet& operator -=(const TBitSet& bs);
    TBitSet& operator &=(const TBitSet& bs);

    bool   IsEmpty() const;

    friend TBitSet operator &(const TBitSet& bs1, const TBitSet& bs2)
    {
      TBitSet<T> temp(bs1);
      temp &= bs2;
      return temp;
    }
    friend TBitSet operator |(const TBitSet& bs1, const TBitSet& bs2)
    {
      TBitSet<T> temp(bs1);
      temp |= bs2;
      return temp;
    }

    friend int operator ==(const TBitSet& bs1, const TBitSet& bs2)
    {
      for (int i = 0; i < sizeof(bs1.Bits); i++)
        if (bs1.Bits[i] != bs2.Bits[i])
          return false;
      return true;
    }
    friend int operator !=(const TBitSet& bs1, const TBitSet& bs2)
    {
      return !operator ==(bs1, bs2);
    }

    friend opstream& operator <<(opstream& out, const TBitSet& bs)
    {
      out.fwriteBytes(bs.Bits, sizeof(bs.Bits));
      return out;
    }
    friend ipstream& operator >>(ipstream& in, TBitSet& bs)
    {
      in.freadBytes(bs.Bits, sizeof(bs.Bits));
      return in;
    }

  private:
    int    Loc(T item) const;
    uint8  Mask(T item) const;

    uint8  Bits[1 << (sizeof(T) * 8 - 3)];
};


//
// class CharSet
// ~~~~~ ~~~~~~~~
class _OWLCLASS TCharSet : public TBitSet<TCHAR> {
  public:
    TCharSet();
    TCharSet(const TBitSet<TCHAR>& src);
    TCharSet(LPCTSTR str);
};

//
// TBitFlags is a *lightweight* class for setting, clearing and querrying
// bit flags. It's intenteded to be used with a 'short' or 'long' type
// allowing an easy method to handle 16 and 32 bit flags respectively.
//
// For example:
//
//    class TMyClass : public TMyBase, public TBitFlags<short> {
//
template <class T> class /*_OWLCLASS*/ TBitFlags {
  public:
    TBitFlags(T  t = 0);

    // Query, Clear and Set flag bits
    //
    T       Clear(T t);
    T       Set(T t);
    bool    IsSet(T t) const;

  protected:
    T       Bits;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
// Initialize the Flags to the specified parameter
//
template <class T>
TBitFlags<T>::TBitFlags(T t)
:
  Bits(t)
{
}

//
// Activate the bits that are enabled in the specified parameter
//
template <class T>
T TBitFlags<T>::Set(T t)
{
  return Bits |= t;
}

//
// Clear the bits that are enabled in the specified parameter
//
template <class T>
T TBitFlags<T>::Clear(T t)
{
  return Bits &= ~t;
}

//
// Return true of the ON bits of the parameter are currently enabled.
// Return false otherwise.
//
template <class T>
bool TBitFlags<T>::IsSet(T t) const
{
  return (Bits & t) != 0;
}


__OWL_END_NAMESPACE


#endif  // OWL_BITSET_H
