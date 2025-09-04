//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//
// TExpandableComboBox class; original code by:
//----------------------------------------------------------------
// Copyright © Antony Corbett 1995. All Rights Reserved
// Author:	Antony Corbett
//				Compuserve	100277,330
//				INTERNET		antony@corbetteng.co.uk
//				Tel +44 (1926) 856131,  Fax +44 (1926) 856727
// Updated: Sergio G. Olivas
//          Parker Hannifin
//          Converted to ComboBox Class
//          06/10/97
//
// Description:
//		Header file for TExpandableComboBox classes.
//
//
//		TComboBox derivatives that provide disabling of selected
//		list items, horizontal scrolling
//
//-------------------------------------------------------------------
#if !defined(__OWLEXT_COMBOEXP_H)
#define __OWLEXT_COMBOEXP_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif


// Grab necessary OWL headers
//
#if !defined(OWL_COMBOBOX_H)
	#include <owl/combobox.h>
#endif

// Grab necessary OWL headers
//
#if !defined(OWL_TEMPLATE_H)
	#include <owl/template.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


// nUnitWidth
//  if +X  Fixed
//  if 0  No Change in Size
//  if -1 AutoSize to largest string, but not larger than dialog width
//  if -X Size given is maximum Width allowable, will resize to less



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                   TCBItemData
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Used to store enable status of an item in the list. Also retains 32-bit item
// data. A ptr to a TCBItemData object is associated with each item added to the
// list using SeExpItemData.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct OWLEXTCLASS TCBItemData {
	bool enabled_;
	bool useSysColor_;
	uint32 data_;
	__OWL TColor textColor_;

	TCBItemData()
    : enabled_(true), data_(0), useSysColor_(true)
	{ }
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                           TExpandableComboBox
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Provides horizontal scrolling, disabling of selected items. Color Highlights
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TExpandableComboBox : public __OWL TComboBox {
	// Enumerations
	//
	public:
		enum eExpAlign{
			eExpAlignDefault,  // Default Left Alignment
			eExpAlignAuto,     // Not Implemented
			eExpAlignLeft,     // Aligns (fixes) left side
			eExpAlignRight,    // Aligns (fixes) right side
			eExpAlignCenter,   // Not Implementd
		};

	// Object lifetime methods
	//
	public:
		TExpandableComboBox(__OWL TWindow* parent, int resourceId,
												eExpAlign fAlign = eExpAlignLeft,
												int nUnitWidth = -1,
												__OWL TModule* module = 0);
		~TExpandableComboBox();

	// Accessors
	//
	public:
		// Combobox item operations
		int						GetStringX(LPTSTR str, int nIndex) const;
		uint32				GetItemData(int index) const;
		bool					GetSel(int index) const;
		int						GetSelCount() const;
		bool					GetEnabled(int index) const;
		TCBItemData*	GetItemDataPtr(int index) const;
		// Combobox operations
		int						GetSelIndex() const;

	// Mutators
	//
	public:
		// Combobox item operations
		void	ResizeToFit();
		int		AddString(LPCTSTR str);
		int		InsertString(LPCTSTR str, int index);
		int		DeleteString(int index);
		int		AddDisabledString(LPCTSTR str);
		void	Enable(int index, bool enable = true);
		int		SetItemData(int index, uint32 data);
		int		SetSel(int index, bool select);
		int		SetSelIndex(int index);
		void	SetTextColor(int index, __OWL TColor color);
		void	ResetTextColor(int index);
		// ComboBox operations
		void	ClearList();
		bool	SetTabStops(int numTabs, int* tabs, bool bShowCompleteLine = false);
		bool	MakeSingleSel(bool single = true);

		// Misc "helper" methods
		//
	public:
		int		UnitsToPixels(int nUnits);
		int		PixelsToUnits(int nPixels);

		// OWL overridden methods
		//
	protected:
		void	SetupWindow();
		void	CleanupWindow();
		LRESULT DefWindowProc(uint msg, WPARAM wParam, LPARAM lParam);
		void	ODADrawEntire(DRAWITEMSTRUCT &drawInfo);
		void	ODAFocus(DRAWITEMSTRUCT &drawInfo);
		void	ODASelect(DRAWITEMSTRUCT &drawInfo);
		DECLARE_RESPONSE_TABLE(TExpandableComboBox);
		void	EvMeasureItem(UINT ctrlId, MEASUREITEMSTRUCT &measure);
		void	CmKillFocus();
		void	CmSelChange();
		void	CmSelEndOk();

		virtual void		DrawListItem(DRAWITEMSTRUCT& drawInfo, LPCTSTR str);
		virtual int			FindTextExtent(LPCTSTR str);
		virtual __OWL TColor	QueryTextColor(DRAWITEMSTRUCT& drawInfo) const;
		virtual __OWL TColor	QueryBkColor(DRAWITEMSTRUCT& drawInfo) const;
		void						StoreStrExtent(LPCTSTR str);
		virtual void		DrawText(DRAWITEMSTRUCT& drawInfo,const __OWL TRect& rc, LPCTSTR str);
		void						UpdateHorizontalExtent();
		bool						IsMultipleSel() const;

		// Internal data
		//
	private:
		HWND 		hwndList;
		__OWL TRect 	RectList;
		int     greatestExtent;
		int   	nUnitWidth_;
		int     nItemHeight;
		eExpAlign fTCBAlign_;
		bool  	bShowCompleteLine_;
		int   	nPasses;
		bool 		bFirstRedraw;
		bool 		hasHScroll_;
		int 		numTabs_;
		int* 		tabs_;

		typedef __OWL TSortedObjectArray<int> SortedIntegers;
		SortedIntegers& textExtents_;

	protected:
		bool pseudoSingleSel_;	// forced into single-selection mode?
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations

inline bool TExpandableComboBox::IsMultipleSel() const
{
	return false;
}

OWLEXT_END_NAMESPACE

#endif	//__OWLEXT_COMBOEXP_H
