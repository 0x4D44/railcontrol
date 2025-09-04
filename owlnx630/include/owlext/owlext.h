//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//
// OWL Extensions TModule-derivative header
//-------------------------------------------------------------------
#ifndef __OWLEXT_OWLEXT_H
#define __OWLEXT_OWLEXT_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                    TOWLEXTDll
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TOWLEXTDll : public __OWL TModule {
	// Class friendships
	//
	friend int OWLEXTEntryPoint(HINSTANCE, uint32, LPVOID);

	// Object lifetime methods
	//
	public:
		TOWLEXTDll(bool shouldLoad = true, bool mustLoad = false);

	private:
		TOWLEXTDll(HINSTANCE hInstance);      // Only this source module needs this

	public:
		virtual ~TOWLEXTDll();

	// Accessors
	//
	public:
		__OWL TModuleProc0<long> GetOWLEXTVersion;
		__OWL TModuleVersionInfo& GetVersionInfo();
		static TOWLEXTDll& Instance();

	// Mutators
	//
	public:
		int ThreadAttach();
		int ThreadDetach();

	// Internal data
	//
	private:
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// Singleton accessor method; does the same as TOWLEXTDll::Instance(), but
// some may find the global function syntax to be cleaner
//
TOWLEXTDll* GetOWLEXTDll();


OWLEXT_END_NAMESPACE


#endif
