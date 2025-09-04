//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//
//  Description:  An editable listbox. Double clicking an item lets you edit
//                it. An EN_CHANGE notification is sent to the parent when
//                the user finishes editing.
//
// Original code by Chris Kohloff; used with permission:
//  Author:       Christopher Kohlhoff (chris@tenermerx.com)
//
//  Date:         30 May 1997
//
//-------------------------------------------------------------------
#ifndef __OWLEXT_EDITLIST_H
#define __OWLEXT_EDITLIST_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
# include <owlext/core.h>
#endif


// Grab necessary OWL headers
//
#if !defined(OWL_LISTBOX_H)
# include <owl/listbox.h>
#endif


__OWL_DECLCLASS(TEdit);  // forward declaration

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                     TEditList
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class OWLEXTCLASS TEditList : public __OWL TListBox {
	// Object lifetime methods
	//
	public:
		TEditList(__OWL TWindow* parent, int id, int x, int y, int w, int h, __OWL TModule* module = 0);
		TEditList(__OWL TWindow* parent, int id, __OWL TModule* module = 0);
		virtual ~TEditList();

	// Accessors
	//
	public:
		__OWL TEdit* GetEdit();

	// Mutators
	//
	public:
		void BeginEdit();
		void EndEdit();

	// OWL overridden methods
	//
	public:
		virtual void SetupWindow();
		void EvLButtonDblClk(uint modKeys, __OWL TPoint& point);
		void EditKillFocus();

	// Internal data
	//
	protected:
		__OWL TEdit* Edit;

	DECLARE_RESPONSE_TABLE(TEditList);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations

inline __OWL TEdit* TEditList::GetEdit(){
	return Edit;
}

OWLEXT_END_NAMESPACE

#endif	//__OWLEXT_COMBOEXP_H

