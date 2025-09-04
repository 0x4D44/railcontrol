//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// TDirDialog v. 1.0 - (C) Copyright 1996 by Kent Reisdorph, All Rights Reserved
// Copyright © 1998 by Yura Bidus. All Rights Reserved.
//
// Original code by Kent Reisdorph, 75522,1174
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//
//  OVERVIEW
//  ~~~~~~~~
// Header for class TDirDialog - A dialog class that allows
// directory selection similar to Win95's Explorer
//
//----------------------------------------------------------------------------
#if !defined(OWLEXT_DIRDIALG_H) // Sentry, use file only if it's not already included.
#define OWLEXT_DIRDIALG_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
# include <owlext/core.h>
#endif

#include <owl/dialog.h>

__OWL_DECLCLASS(TTreeWindow);
__OWL_DECLCLASS(TImageList);
__OWL_DECLCLASS(TStatic);
__OWL_DECLCLASS(TTwNotify);
__OWL_DECLCLASS(TTreeNode);

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class OWLEXTCLASS TDirDialog : public __OWL TDialog {
	public:
		class OWLEXTCLASS TData {
			public:
					TData();
					~TData();

				LPTSTR DirPath;
		};

		TDirDialog(__OWL TWindow* parent, TData& data, __OWL TModule* module = 0);
		~TDirDialog();

		void SetupWindow();

		void CmOk();

		void TvnSelChanged(__OWL TTwNotify &);
		void TvnItemExpanded(__OWL TTwNotify &);
		void BuildPath(owl_string&,__OWL  TTreeNode&);
		void LoadTree();
		void AddNode(LPCTSTR, const __OWL TTreeNode&, int);

	protected:
		__OWL TTreeWindow* 	DirTree;
		__OWL TImageList* 	Images;
		__OWL TStatic* 			CurrentDir;
		TData& 							Data;
		int 								level;

	DECLARE_RESPONSE_TABLE(TDirDialog);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif     // OWLEXT_DIRDIALG_H sentry.

