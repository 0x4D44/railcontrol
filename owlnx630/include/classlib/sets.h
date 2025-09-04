//----------------------------------------------------------------------------
// Borland BIDS Container Library Compatibility header
// Copyright (c) 2010, Sebastian Ledesma-OWLNext, All Rights Reserved
//
//$Revision: 2.0 $
//
//----------------------------------------------------------------------------

#ifdef BI_HAS_PRAGMA_ONCE
#   pragma once
#endif

#if !defined(CLASSLIB_SETS_H)
#define CLASSLIB_SETS_H

#pragma message ("Included file: <classlib/sets.h>, partial implementation")

#include <classlib/defs.h>
//#include <classlib/resource.h> //original location for DEFAULT_SET_SIZE

#include <set>

__OWL_BEGIN_NAMESPACE

const DEFAULT_SET_SIZE          =  29;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


/// \addtogroup utility
/// @{
/// \class TSetAsVector
// ~~~~~ ~~~~~~~
/// Implements a set of objects of type T,
/// using a std::set as the underlying implementation.
/// TStandardAllocator is used to manage memory.
///
/// Example: 	TSetAsVector<int> alreadyChanged;


template<class T> class TSetAsVector : public std::set<T> {
public:
	typedef void (*IterFunc)(T&, void*);
	typedef bool (*CondFunc)(const T&, void*);

	TSetAsVector(unsigned int init_size = DEFAULT_SET_SIZE) : std::set<T>()
	{
	
    }

	bool Add(const T& t) {
		if (find(t)== end()) {
			insert(t);
			return true;
		}
		return false;
	}

	int Detach( const T& t ) {
		erase (find(t));
	}

	bool IsEmpty() const
	{
		return empty();
	}

	bool IsFull() const
	{
		return false;
	}

	unsigned int GetItemsInContainer() const
	{
		return size();
	}

	bool HasMember(const T& t) const
	{
		return find(t) != end();
	}

	int Find( const T& t ) const
	{
		return find(t)!= end();
	}

	int FindMember( const T& t ) const
	{
		return Find( t ) ;
	}

	void Flush()
	{
		clear ( );
	}

	void ForEach(IterFunc iter, void* args)
	{
		for (int i = 0; i < size(); ++i)
			(iter)(at(i), args);
	}



};


/// \class TSet
// ~~~~~ ~~~~~~~
/// Easy names for TSetAsVector.
/// Implements a set of objects of type T,
/// using a std::set as the underlying implementation.
/// TStandardAllocator is used to manage memory.
///
/// Example: 	TSet<int> alreadyChanged;

template <class T> class TSet :
    public TSetAsVector<T>
{

public:

    TSet( unsigned sz = DEFAULT_SET_SIZE ) :
		TSetAsVector<T>( sz )
        {
        }

};
/// @}

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

#endif  // CLASSLIB_SETS_H

