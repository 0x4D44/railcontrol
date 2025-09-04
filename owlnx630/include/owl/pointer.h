//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26.4.1 $
//
// Various types of smart pointer templatized classes
//----------------------------------------------------------------------------

#if !defined(OWL_POINTER_H)
#define OWL_POINTER_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if defined(BI_STD_RTL)
# include <new>
#else
# include <new.h>
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

//
// A pair of smart pointer template classes. Provides basic conversion
// operator to T*, as well as dereferencing (*), and 0-checking (!).
// These classes assume that they alone are responsible for deleting the
// object or array unless Relinquish() is called.
//

//
// class TPointerBase
// ~~~~~ ~~~~~~~~~~~~
template<class T> class TPointerBase {
  public:
    T&   operator  *() {return *P;}
         operator T*() {return P;}
    int  operator  !() const {return P==0;}
    T*   Relinquish() {T* p = P; P = 0; return p;}

  protected:
    TPointerBase(T* pointer) : P(pointer) {}
    TPointerBase() : P(0) {}

    T* P;

  private:
    void* operator new(size_t) {return 0;}  // prohibit use of new
#if !defined(BI_COMP_GNUC)
    void operator delete(void* p) {((TPointerBase<T>*)p)->P=0;}
#endif
};

//
// class TPointer
// ~~~~~ ~~~~~~~~
// Pointer to a single object. Provides member access operator ->
//
template<class T> class TPointer : public TPointerBase<T> {
  public:
    TPointer() : TPointerBase<T>() {}
    TPointer(T* pointer) : TPointerBase<T>(pointer) {}
   ~TPointer() {delete this->P;}
    TPointer<T>& operator =(T* src) {delete this->P; this->P = src; return *this;}
    T* operator->() {return this->P;}  // Could throw exception if P==0
};

//
// class TAPointer
// ~~~~~ ~~~~~~~~~
// Pointer to an array of type T. Provides an array subscript operator and uses
// array delete[]
//
template<class T> class TAPointer : public TPointerBase<T> {
  public:
    TAPointer() : TPointerBase<T>() {}
    TAPointer(T array[]) : TPointerBase<T>(array) {}
   ~TAPointer() {delete[] this->P;}
    TAPointer<T>& operator =(T src[]) {delete[] this->P; this->P = src; return *this;}
    T& operator[](int i) {return this->P[i];}  // Could throw exception if P==0
    T& operator[](size_t i) {return this->P[i];}  // Could throw exception if P==0
};

//----------------------------------------------------------------------------

//
// Envelope-letter type of smart pointers. In this implementation the envelope
// acts as a smart pointer to a reference-counted internal letter. This allows
// a lot of flexibility & safety in working with a single object (associated
// with the letter) being assigned to numerous envelopes in many scopes. An 'A'
// version is provided for use with arrays.
//
// Use like:
//   TEnvelope<T> e1 = new T(x,y,x);   // e1 now owns a T in a letter
//   e1->Func();                       // invokes Func() on the new T
//   Func(*e1);                        // passing a T& or a T this way
//   TEnvelope<T> e2 = e1;             // e2 safely shares the letter with e1
//
//   TAEnvelope<T> e1 = new T[99];     // e1 now owns T[] in a letter
//   e1[i].Func();                     // invokes Func() on a T element
//   Func(e1[i]);                      // passing a T& or a T this way
//   TAEnvelope<T> e2 = e1;            // e2 safely shares the letter with e1
//

//
// class TEnvelope
// ~~~~~ ~~~~~~~~~
template<class T> class TEnvelope {
  public:
    TEnvelope(T* object) : Letter(new TLetter(object)) {}
    TEnvelope(const TEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}
   ~TEnvelope() {Letter->Release();}

    TEnvelope& operator =(const TEnvelope& src);
    TEnvelope& operator =(T* object);

    T* operator->() { return Letter->Object; }
    T& operator *() { return *Letter->Object; }

  private:
    struct TLetter {
      TLetter(T* object) : Object(object), RefCount(1) {}
     ~TLetter() { delete Object; }

      void AddRef() { RefCount++; }
      void Release() { if (--RefCount == 0) delete this; }

      T*  Object;
      int RefCount;
    };
    TLetter* Letter;
};

//
//
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class TVoidPointer {
  public:
    TVoidPointer( const void *p = 0 ) : Ptr(p) {}
    TVoidPointer( const TVoidPointer& rp ) : Ptr(rp.Ptr) {}

    const TVoidPointer& operator = ( const TVoidPointer& vp )
        { Ptr = vp.Ptr; return *this; }
    const TVoidPointer& operator = ( const void *p )
        { Ptr = p; return *this; }
    operator void *() const { return CONST_CAST(void *,Ptr); }

  private:
    const void *Ptr;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif


//
// class TComRefBase
// ~~~~~ ~~~~~~~~~~~
// Encapsulation of OLE interface pointers
//
template<class T> class TComRefBase {
  public:
            operator T*()         {return I;}
            operator T**()        {Clear(); return &I;}
    int     operator !() const    {return I==0;}
    void operator delete(void* p) {((TComRefBase<T>*)p)->I=0;}
  protected:
    TComRefBase(const TComRefBase<T>& i) : I(i.I) {if (I) I->AddRef();}
    TComRefBase(T* i) : I(i) {}
    TComRefBase()     : I(0) {}
    ~TComRefBase()   {Clear();}

    void Clear() {if(I){I->Release(); I = 0;}}
    T* I;
  private:

    // Prohibit use of operator new
    //
    void* operator new(size_t) {return 0;}
};

template<class T> class TComRef : public TComRefBase<T> {
  public:
    TComRef() : TComRefBase<T>() {}
    TComRef(T* iface) : TComRefBase<T>(iface) {}
    TComRef(const TComRef<T>& i) : TComRefBase<T>(i) {}
    TComRef<T>& operator =(T* iface) {this->Clear(); this->I = iface; return *this;}
    TComRef<T>& operator =(const TComRef<T>& i)
                 {this->Clear(); if (i.I) {this->I = i.I; this->I->AddRef();} return *this;}
    T* operator->()       {return this->I;}      // Could throw exception if I==0
    T* operator->() const {return this->I;}      // Could throw exception if I==0
    //void** operator&()    {this->Clear(); return (void**)&I;}
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

template<class T>
TEnvelope<T>& TEnvelope<T>::operator =(const TEnvelope<T>& src)
{
  Letter->Release();
  Letter = src.Letter;
  Letter->AddRef();
  return *this;
}

template<class T>
TEnvelope<T>& TEnvelope<T>::operator =(T* object)
{
  Letter->Release();
  Letter = new TLetter(object);  // Assumes non-null! Use with new
  return *this;
}

//
// class TAEnvelope
// ~~~~~ ~~~~~~~~~~
template<class T> class TAEnvelope {
  public:
    TAEnvelope(T array[]) : Letter(new TLetter(array)) {}
    TAEnvelope(const TAEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}
   ~TAEnvelope() { Letter->Release(); }

    TAEnvelope& operator =(const TAEnvelope& src);
    TAEnvelope& operator =(T array[]);

    T& operator[](int i) { return Letter->Array[i]; }
    T* operator *() { return Letter->Array; }

  private:
    struct TLetter {
      TLetter(T array[]) : Array(array), RefCount(1) {}
     ~TLetter() { delete[] Array; }

      void AddRef() { RefCount++; }
      void Release() { if (--RefCount == 0) delete this; }

      T*  Array;
      int RefCount;
    };
    TLetter* Letter;
};

template<class T>
TAEnvelope<T>& TAEnvelope<T>::operator =(const TAEnvelope<T>& src)
{
  Letter->Release();
  Letter = src.Letter;
  Letter->AddRef();
  return *this;
}

template<class T>
TAEnvelope<T>& TAEnvelope<T>::operator =(T array[])
{
  Letter->Release();
  Letter = new TLetter(array);  // Assumes non-null! Use with new
  return *this;
}

__OWL_END_NAMESPACE

#endif  // OWL_POINTER_H
