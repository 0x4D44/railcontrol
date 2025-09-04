//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// POPEDIT.H
//
// Header file for TPopupEdit Class
//
// Original code by Daniel BERMAN (dberman@inge-com.fr)
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
//------------------------------------------------------------------------------
#if !defined(__OWLEXT_POPEDIT_H)
#define __OWLEXT_POPEDIT_H

#include <owl/edit.h>
#include <owl/gdiobjec.h>
#include <owl/controlb.h>
#include <owl/controlg.h>

OWLEXT_BEGIN_NAMESPACE

class TPopupButton;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

class OWLEXTCLASS TPopupEdit : public __OWL TEdit {
	public:

		TPopupEdit(__OWL TWindow* parent, int id, LPCTSTR text, int x, int y,
				       int w, int h, __OWL TResId bmpId, uint textLen = 0,
               __OWL TModule* module = 0);
		TPopupEdit(__OWL TWindow* parent, int resourceId, __OWL TResId bmpId,
               uint textLen = 0, __OWL TModule* module = 0);

		virtual ~TPopupEdit();

	  virtual void	Clicked() {};

		void 					SysColorChange();

	protected:

		TPopupButton* PopupBtn;

		virtual void	SetupWindow();

		void	EvEnable(bool enabled);
		void	EvKeyDown(uint key, uint repeatCount, uint flags);
		void	EvMouseMove(uint modKeys, __OWL TPoint& point);
		void	EvShowWindow(bool show, uint status);
		void	EvSize(uint sizeType, __OWL TSize& size);
		void	EvSysKeyDown(uint key, uint repeatCount, uint flags);

	private:

		__OWL TFont* Font;

		void Init(__OWL TResId bmpId);

		DECLARE_RESPONSE_TABLE(TPopupEdit);
};


class OWLEXTCLASS TToolBar : public __OWL TControlBar {
	public:
		TToolBar(__OWL TWindow* parent = 0, TTileDirection direction = Horizontal,
						 __OWL TFont* font = new __OWL TGadgetWindowFont,
						 __OWL TModule* module = 0);
	protected:
		bool IdleAction(long idleCount);
};


class OWLEXTCLASS TPopupEditGadget : public __OWL TControlGadget{
	public:
		TPopupEditGadget(__OWL TWindow& control, TBorderStyle = None);

	protected:
		void MouseEnter(uint modKeys, __OWL TPoint& pt);
		void MouseLeave(uint modKeys, __OWL TPoint& pt);
		void SysColorChange();
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

bool DrawMaskedBmp(HDC hDC, int x, int y, HBITMAP hbm, int XOrigin, int nWidth);


OWLEXT_END_NAMESPACE

#endif  // __OWLEXT_POPEDIT_H
