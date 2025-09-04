//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
//
//Original code by David Foringer
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_VALIDATE_H)
#define __OWLEXT_VALIDATE_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

#include <owl/validate.h>

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

//
// class TDoubleValidator
//

class OWLEXTCLASS TDoubleValidator : public __OWL TFilterValidator {
  public:
    TDoubleValidator(double min, double max);

    // Override TValidator's virtuals
    //
    void	Error(__OWL TWindow* owner);
    bool  IsValid(LPCTSTR str);
    bool 	IsValidInput(LPTSTR str, bool /*suppressFill*/);
    uint  Transfer(LPTSTR str, void* buffer, __OWL TTransferDirection direction);
		int   Adjust(owl_string& text, uint& begPos, uint& endPos, int amount);

	protected:
		double	GetMin()						{return Min;};
		void   	SetMin(double min)	{Min=min;};
    double  GetMax()						{return Max;}
    void   	SetMax(double max)	{Max=max;};

    double Min;
    double Max;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif  // __OWLEXT_VALIDATE_H

