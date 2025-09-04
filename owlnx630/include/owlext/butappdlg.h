// ****************************************************************************
// OWL Extensions (OWLEXT) Class Library
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// butappdlg.h:  header file
// Version:      1.6
// Date:         08.11.1998
// Author:       Dieter Windau
//
// Freeware OWL classes that extents the dockable and gadget system
//
// You are free to use/modify this code but leave this header intact.
// May not be sold for profit.
//
// Tested with Borland C++ 5.02, OWL 5.02, OWL6 patch #3 and with Windows
// NT 4.0 SP3 but I think the class should work with Windows 95 too.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk.
//
// This package contains many classes and source that are based on other OWL
// developers work. Very special thanks to Alan Chambers, Christopher Kohlhoff,
// Jo Parrello, Mark Hatsell, Michael Mogensen and Yura Bidus
//
// Please send me bug reports, bug fixes, enhancements, requests, etc., and
// I'll try to keep this in next versions:
//   EMail: dieter.windau@usa.net
//   Web:   http://members.aol.com/softengage/index.htm
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
// ****************************************************************************
#if !defined(__OWLEXT_BUTAPPDLG_H)
#define __OWLEXT_BUTAPPDLG_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
  #include <owlext/core.h>
#endif

#ifndef   OWL_EDIT_H
#include <owl/edit.h>
#endif
#ifndef   OWL_LISTBOX_H
#include <owl/listbox.h>
#endif

#ifndef __OWLEXT_GADGDESC_H
#include <owlext/gadgdesc.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// ****************** Forward declaration *************************************
class OWLEXTCLASS TButtonGadgetEx;
class OWLEXTCLASS TButtonTextGadgetEx;

class OWLEXTCLASS TImageListBox;
class OWLEXTCLASS TButtonAppearanceDialog;

// ****************** TImageListbox *******************************************

class OWLEXTCLASS TImageListBox : public __OWL TListBox
{
  public:
    TImageListBox(__OWL TWindow* parent, int resourceId, __OWL TCelArray* array=0, 
			            __OWL TIntArray* idsaray = 0, __OWL TModule* module = 0);
    virtual ~TImageListBox();

    virtual void BuildIdsArray(__OWL TResId& resId, int maxitems=0);
    virtual void BuildCelArray();
    // Set the  bitmaps by DWORD data. Call this function if all items are added

  protected:
    bool         EvEraseBkgnd(HDC hdc);
		virtual void DrawItem(DRAWITEMSTRUCT & drawInfo);

  protected:
    __OWL TCelArray*	CelArray;
		__OWL TIntArray*	IdsArray;

  DECLARE_RESPONSE_TABLE(TImageListBox);
};

// ****************** TButtonAppearanceDialog *********************************

class OWLEXTCLASS TButtonAppearanceDialog : public __OWL TDialog {
  public:
    TButtonAppearanceDialog(__OWL TWindow* parent,
			                      TButtonTextGadgetEx* gadget,
														__OWL TCelArray* celaray=0, 
														__OWL TIntArray* idsaray = 0, 
														__OWL TModule* module=0);

  protected:
    virtual void SetupWindow();

    void CmOk();
    void CmImage();
    void CmText();
    void CmImageText();
    void CmButtonText();

    void EnableControls();

  protected:

    TImageListBox*       ImageListBox;
    __OWL TEdit*               ButtonEdit;
    TButtonTextGadgetEx* ButtonTextGadget;
    TDisplayType         DisplayType;
    owl_string           ButtonText;
    __OWL TResId               BmpResId;

  DECLARE_RESPONSE_TABLE(TButtonAppearanceDialog);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif
