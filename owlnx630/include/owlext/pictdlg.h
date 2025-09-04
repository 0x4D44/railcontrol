//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// TPictDialog
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#ifndef __OWLEXT_PICTDLG_H
#define __OWLEXT_PICTDLG_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif

#include <owl/dialog.h>

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                   TPictDialog
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TPictDialog : public __OWL TDialog
{
  // Object lifetime methods
  //
public:
  TPictDialog(__OWL TDib* dib, __OWL TWindow* parent, __OWL TResId resId, __OWL TModule* module = 0);
  virtual ~TPictDialog();

  // OWL overrides
  //
protected:
  LPCTSTR GetClassName(){ return _T("PictDlg"); }
  DECLARE_RESPONSE_TABLE(TPictDialog);
  virtual bool EvEraseBkgnd(HDC hdc);

  // Accessors
  //
public:
  __OWL TDib* GetDib()
    { return m_pdib; }

  // Mutators
  //
public:
  __OWL TDib* SetDib(__OWL TDib* newDib);

  // Internal data
  //
private:
  __OWL TDib* m_pdib;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif
