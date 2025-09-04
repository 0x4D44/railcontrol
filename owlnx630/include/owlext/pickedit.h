//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// PICKEDIT.H
//
// Header file for TPickEditList Class
//
// Original code by Steve Carr (Compuserve: 100251,1571)
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//------------------------------------------------------------------------------
#if !defined(__OWLEXT_PICKEDIT_H)
#define __OWLEXT_PICKEDIT_H

#include <owlext/popedit.h>

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

class OWLEXTCLASS TPickEditList : public TPopupEdit {
	public:
		TPickEditList(__OWL TWindow* parent, int id, LPCTSTR fileName, LPCTSTR text,
				          int x, int y, int w, int h, __OWL TResId bmpId,
				          uint textLen = 0, __OWL TModule* module = 0);
	 	TPickEditList(__OWL TWindow* parent, int resourceId, LPCTSTR fileName,
				          __OWL TResId bmpId, uint textLen = 0, __OWL TModule* module = 0);

		~TPickEditList() ;

		void Clicked();

	protected:
		LPTSTR fileName;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif // __OWLEXT_PICKEDIT_H
