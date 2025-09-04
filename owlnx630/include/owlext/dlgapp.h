//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
//
// Thanks to Ron Reynolds of Access Health, Inc. for the assist.
//
// TDialogApp
//-------------------------------------------------------------------
#if !defined(__OWLEXT_DLGAPP_H)
#define __OWLEXT_DLGAPP_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif

#include <assert.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/functor.h>		// Make sure you link with VDBT

OWLEXT_BEGIN_NAMESPACE

// DialogAppFunctor is a ptr to a fn taking nothing and returning a TDialog*
typedef __OWL TFunctor0<__OWL TDialog*> DialogAppFunctor;

#if defined(BI_HAS_MEMBER_TEMPLATE)
#  define DialogApp_FUNCTOR(func) TFunctionTranslator0<TDialog*,TDialog* (*)()>(func)
#else
#define DialogApp_FUNCTOR(func) \
		Functor((DialogAppFunctor*)0, func)
#endif


// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// TDialogApp
//
class OWLEXTCLASS TDialogApp : public __OWL TApplication
{
  // Object lifetime methods
  //
public:
  TDialogApp(DialogAppFunctor dlgCreator, LPCTSTR title = 0 )
    : __OWL TApplication(title), m_creatorFunctor(dlgCreator)
  { }

  // OWL mutators
  //
protected:
  virtual void InitMainWindow()
  {
    // Create the dialog
    __OWL TDialog* dlg = m_creatorFunctor();

    // Create our main window
    SetMainWindow( new __OWL TFrameWindow(0, GetName(), dlg, true));
  }

  // Internal data
  //
protected:
  DialogAppFunctor m_creatorFunctor;

  // Explicitly disallowed methods
  //
private:
  TDialogApp(const TDialogApp&);        // DISALLOWED METHOD
  void operator=(const TDialogApp&);    // DISALLOWED METHOD
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif