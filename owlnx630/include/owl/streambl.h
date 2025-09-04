//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Class definitions for object streaming
//
//
//                 +----------------+  +-------------+
//                 |TStreamableTypes|  |ObjectBuilder|
//                 +--+----------+--+  +-------------+
//                    |class name|     |  BUILDER    |
//                    +----------+     |  delta      |
//                          |          +-------------+
//                          |                 |
//                          |                 /\ .
//                          |         +--------
//                          |         |
//                          |         |
//                        +----------------+
//                        |TStreamableClass|
//                        +----------------+
//                        |  Module ID     |
//                        +----------------+
//
//----------------------------------------------------------------------------

#if !defined(OWL_STREAMBL_H)
#define OWL_STREAMBL_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(__STRING_H) && !defined(_INC_STRING)
# include <string.h>
#endif
#if defined(BI_PLAT_MSW) && !defined(OWL_PRIVATE_WSYSINC_H)
# include <owl/private/wsysinc.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if defined(_FASTTHIS)
#  define _OWLFASTTHIS __fastthis
#else
#  define _OWLFASTTHIS
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

#if defined( BI_OWL_NO_po )
# pragma option -po-
#endif

#if defined( BI_NO_PER_INSTANCE_DATA ) && defined(_OWLDLL)

  typedef unsigned ModuleId;
  inline ModuleId GetModuleId() { return (unsigned)_hInstance; }

#else

  typedef unsigned ModuleId;
  inline ModuleId GetModuleId() { return 1; }

#endif

class _OWLCLASS _OWLFASTTHIS _RTTI TStreamer;
class _OWLCLASS _OWLFASTTHIS _RTTI TStreamableBase;

#if defined(NO_AFTER_RETPARAM)
typedef TStreamer* (*_CALLCNVN BUILDER)( TStreamableBase * );
#else
typedef TStreamer* _CALLCNVN (*BUILDER)( TStreamableBase * );
#endif

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif

struct _OWLCLASS ObjectBuilder {

    enum { NoDelta = -1 };

    _OWLCTOR ObjectBuilder( BUILDER b, int d ) : Builder( b ), Delta( d ) {}

    BUILDER Builder;
    int Delta;

};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

#define __DELTA( d ) (FP_OFF((TStreamable *)(d *)1)-1)

class _OWLCLASS TStreamableClass : public ObjectBuilder {
  public:

    _OWLCTOR TStreamableClass( LPCSTR n,
                             BUILDER b,
                             int d = NoDelta,
                             ModuleId mid = GetModuleId() );

    _OWLCTOR ~TStreamableClass();

    int _OWLENTRY operator == ( const TStreamableClass& n ) const
        {
        if( strcmp( ObjectId, n.ObjectId ) != 0 )
            return 0;
        else
            return (ModId == 0 || n.ModId == 0 || ModId == n.ModId);
        }

    int _OWLENTRY operator < ( const TStreamableClass& n ) const
        {
        int res = strcmp( ObjectId, n.ObjectId );
        if( res < 0 )
            return 1;
        else if( res > 0 )
            return 0;
        else if( ModId == 0 || n.ModId == 0 || ModId == n.ModId )
            return 0;
        else
            return ModId < n.ModId;
        }

  private:

    const char* ObjectId;
    ModuleId ModId;

};
class TStreamableClassArray;
class _OWLCLASS TStreamableTypes {
  public:
    _OWLCTOR TStreamableTypes();
    _OWLCTOR ~TStreamableTypes();

    void _OWLENTRY RegisterType( ModuleId id, TStreamableClass& );
    void _OWLENTRY UnRegisterType( ModuleId id, TStreamableClass& );
    const ObjectBuilder * _OWLENTRY Lookup( ModuleId id, LPCSTR name ) const;

  private:
    TStreamableClassArray*  Types;
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE


#endif  // OWL_STREAMBLE_H
