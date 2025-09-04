//----------------------------------------------------------------------------
// Borland BIDS Container Library Compatibility header
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.1 $
//
//----------------------------------------------------------------------------

#ifdef _MSC_VER 
#   pragma once
#endif

#if !defined(CLASSLIB_COMPAT_H)
#define CLASSLIB_COMPAT_H

#include <classlib/defs.h>

#if defined(BI_NO_EMULATESTL)

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


template<class T> class TArrayAsVectorIterator : public TArrayIterator<T, TObjectArray<T> > {
	public:
		typedef TArrayIterator<T, TObjectArray<T> > Base;

  	_OWLCTOR TArrayAsVectorIterator(TObjectArray<T>& array):Base(array){}
};

template<class T> class TIArrayAsVector : public TIPtrArray<T*> {
	public:
		typedef TIPtrArray<T*> Base;
    _OWLCTOR TIArrayAsVector(int upper, int lower=0, int delta=0):Base(upper,lower,delta){}
		_OWLCTOR TIArrayAsVector(){}
};

template<class T> class TIArrayAsVectorIterator : public TPtrArrayIterator<T*, TIPtrArray<T*> > {
	public:
		typedef TPtrArrayIterator<T*, TIPtrArray<T*> > Base;

  	_OWLCTOR TIArrayAsVectorIterator(TIPtrArray<T*>& array):Base(array){}
};

template<class T> class TSArrayAsVectorIterator : public TArrayIterator<T, TSortedObjectArray<T> > {
	public:
		typedef TArrayIterator<T*, TSortedObjectArray<T> > Base;

  	_OWLCTOR TSArrayAsVectorIterator(TSortedObjectArray<T>& array):Base(array){}
};


template<class T> class TISArrayAsVector : public TISortedPtrArray<T*> {
	public:
		typedef TISortedPtrArray<T*> Base;
    _OWLCTOR TISArrayAsVector(int upper, int lower=0, int delta=0):Base(upper,lower,delta){}
		_OWLCTOR TISArrayAsVector(){}
};

template<class T> class TISArrayAsVectorIterator : public TPtrArrayIterator<T*, TISortedPtrArray<T*> > {
	public:
		typedef TPtrArrayIterator<T*, TISortedPtrArray<T*> > Base;

  	_OWLCTOR TISArrayAsVectorIterator(TIPtrArray<T*>& array):Base(array){}
};

#define TSArrayAsVector 				TSortedObjectArray
#define TSArray									TSortedObjectArray
#define TArrayAsVector 					TObjectArray
#define TArray         					TObjectArray
#define TArrayIterator          TArrayAsVectorIterator    
#define TSArrayIterator         TSArrayAsVectorIterator


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

#endif


#endif  // OWL_COMPAT_H
