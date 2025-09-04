//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// CALCEDIT.H
//
// Header file for TCalcEdit Class
//
// Original code by Steve Carr (Compuserve: 100251,1571)
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//------------------------------------------------------------------------------
#if !defined(__OWLEXT_CALCEDIT_H)
#define __OWLEXT_CALCEDIT_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

#include <owlext/popedit.h>

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

class OWLEXTCLASS TCalcEdit : public TPopupEdit {
	public:
		TCalcEdit(__OWL TWindow* parent, int resourceId,
						uint width, uint decimals = 0, double min = 0, double max = 0,
						__OWL TModule* module = 0);
		TCalcEdit(__OWL TWindow* parent, int id, double value, int x, int y,
					  int w, int h, uint width,
					  uint decimals= 0, double min = 0, double max = 0,
					  __OWL TModule* module = 0);

		void Clicked();

		void SetMax( double d );
		void SetMin( double d );
		void operator = (double d);
	  operator double();


		uint GetDecimals() { return decimals; }

	private:
		__OWL TDialog*  aCalc;
   	uint		 	decimals;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif  //__OWLEXT_CALCEDIT_H
