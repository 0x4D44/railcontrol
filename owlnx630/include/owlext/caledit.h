//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// CALEDIT.H
//
// Header file for TCalendarEdit Class
//
// Original code by Daniel BERMAN (dberman@inge-com.fr)
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//------------------------------------------------------------------------------
#if !defined(__OWLEXT_CALEDIT_H)
#define __OWLEXT_CALEDIT_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

#include <owl/validate.h>

#include <owl/date.h>
#include <owlext/popedit.h>


OWLEXT_BEGIN_NAMESPACE

class OWLEXTCLASS TPopupCalWin;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

class OWLEXTCLASS TCalendarEdit : public TPopupEdit {
	public:
		TCalendarEdit(__OWL TWindow* parent, int id, LPCTSTR text, int x, int y,
						      int w, int h, uint textLen = 0, __OWL TModule* module = 0);
		TCalendarEdit(__OWL TWindow* parent, int resourceId, uint textLen = 0,
						      __OWL TModule* module = 0);
		~TCalendarEdit();

		void					Clicked();
		const __OWL TDate& 	GetDate();
	  void	SetDate(const __OWL TDate& date);
	  void	SetDate(LPCTSTR str);

	protected:
		TPopupCalWin* CalWin;

		void EvChar(uint key, uint repeatCount, uint flags);
	  void EvKeyDown(uint key, uint repeatCount, uint flags);
		void EvKillFocus(HWND hWndGetFocus);

	private:

		void Init();

		DECLARE_RESPONSE_TABLE(TCalendarEdit);
};


class OWLEXTCLASS TCalendarEditGadget : public TPopupEditGadget{
	public:
		TCalendarEditGadget(__OWL TWindow* gadgetWnd, int id, int width = 70,
								  			LPCTSTR text = 0);
};


class OWLEXTCLASS TDateValidator : public __OWL TValidator{
	public:
		TDateValidator();

		virtual void Error(__OWL TWindow* p=0);
		virtual bool IsValid(LPCTSTR str);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif // __OWLEXT_CALENDAREDIT_H
