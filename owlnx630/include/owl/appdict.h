//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// Definition of class TAppDictionary. This class manages associations between
// processes/tasks and TApplication pointers.
//----------------------------------------------------------------------------

#if !defined(OWL_APPDICT_H)
#define OWL_APPDICT_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TApplication;

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TAppDictionary
// ~~~~~ ~~~~~~~~~~~~~~
class _OWLCLASS TAppDictionary {
  public:
    struct TEntry {
      uint           Pid;
      TApplication*  App;
    };
    typedef void (*TEntryIterator)(TEntry&);

  public:
    TAppDictionary();
   ~TAppDictionary();

    TApplication* GetApplication(uint pid = 0);  // default to current pid

    void          Add(TApplication* app, uint pid = 0);
    void          Remove(TApplication* app);
    void          Remove(uint pid);
    void          Condemn(TApplication* app);

    bool          DeleteCondemned();
    void          Iterate(TEntryIterator iter);

  private:
    class TAppDictImp*  Imp;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


// Global exported TAppDictionary in Owl. User Component DLLs should have a
// similar 'AppDictionary'.
//
extern _OWLFUNC(TAppDictionary&) OWLGetAppDictionary();

//
// Global function that calls GetApplication() on owl's app-dictionary.
// Used by EXEs, or DLLs statically linking Owl. Never returns 0, will make
// an alias app if needed. Primarily for compatibility
//
_OWLFUNC(TApplication*) GetApplicationObject(uint pid = 0);


__OWL_END_NAMESPACE


//
// Convenient macro to define a 'AppDictionary' ref and object as needed
// for use in component DLLs and EXEs
//
#if defined(BI_APP_DLL) && defined(_OWLDLL)
# define DEFINE_APP_DICTIONARY(AppDictionary)    \
  NS_OWL::TAppDictionary  AppDictionary
#else
# define DEFINE_APP_DICTIONARY(AppDictionary)    \
  NS_OWL::TAppDictionary& AppDictionary = NS_OWL::OWLGetAppDictionary()
#endif


#endif  // OWL_APPDICT_H
