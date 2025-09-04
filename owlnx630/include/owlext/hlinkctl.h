// ****************************************************************************
// OWL Extensions (OWLEXT) Class Library
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// HLinkCtl.h:   header file
// Version:      1.0
// Date:         17.06.1998
// Author:       Dieter Windau
//
// THLinkCtrl is a freeware OWL class that supports Internet Hyperlinks from
// standard Windows applications just like they are displayed in Web browsers.
//
// You are free to use/modify this code but leave this header intact.
// May not be sold for profit.
//
// Tested with Borland C++ 5.02, OWL 5.02 under Windows NT 4.0 SP3 but I think
// the class would work with Windows 95 too.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk.
//
// This code is based on MFC class CHLinkCtrl by PJ Naughter
// Very special thanks to PJ Naughter:
//   EMail: pjn@indigo.ie
//   Web:   http://indigo.ie/~pjn
//
// Please send me bug reports, bug fixes, enhancements, requests, etc., and
// I'll try to keep this in next versions:
//   EMail: dieter.windau@usa.net
//   Web:   http://www.members.aol.com/SoftEngage
// ****************************************************************************
#if !defined(__OWLEXT_HLINKCTL_H)
#define __OWLEXT_HLINKCTL_H
#define __HLINKCTL_H

#ifndef   OWL_EDIT_H
#include <owl/edit.h>
#endif
#ifndef   OWL_PROPSHT_H
#include <owl/propsht.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// ***************************** THLinkCtrl ***********************************

class OWLEXTCLASS THLinkCtrl: public __OWL TEdit
{
public:
	//Constructors / Destructors
	THLinkCtrl(__OWL TWindow* parent, int Id, LPCTSTR text, int x, int y, int w,
						 int h, uint textLimit = 0, bool multiline = false,
						 __OWL TModule* module = 0);
	THLinkCtrl(__OWL TWindow* parent, int resourceId, uint textLimit = 0,
	           __OWL TModule* module = 0);
	virtual ~THLinkCtrl();

	//Set or get the hyperlink to use
	void SetHyperLink(const owl_string& sActualLink);
	void SetHyperLink(const owl_string& sActualLink,
										const owl_string& sLinkDescription,
										bool bShrinkToFit=true);
	owl_string GetActualHyperLink() const { return m_sActualLink; };

	//Set or get the hyperlink description (really just the window text)
	void SetHyperLinkDescription(const owl_string& sDescription);
	owl_string GetHyperLinkDescription() const;

	//Set or get the hyperlink color
	void SetLinkColor(const __OWL TColor& color);
	__OWL TColor GetLinkColor() { return m_Color; };

	//Set or get the hyperlink color for visited links
	void SetVisitedLinkColor(const __OWL TColor& color);
	__OWL TColor GetVisitedLinkColor() { return m_VisitedColor; };

	//Set or get the hyperlink color for highlighted links
	void SetHighlightLinkColor(const __OWL TColor& color);
	__OWL TColor GetHighlightLinkColor() { return m_HighlightColor; };
	void SetUseHighlightColor(bool bUseHighlight) { m_bUseHighlight = bUseHighlight; };

	//Set or get whether the hyperlink should use an underlined font
	void SetUseUnderlinedFont(bool bUnderline);
	bool GetUseUnderlinedFont() { return m_bUnderline; };

	//Set or get whether the hyperlink should use a drop shadow
	void SetUseShadow(bool bUseShadow);
	bool GetUseShadow() { return m_bUseShadow; };

	//Set or get whether the hyperlink should show a popup menu on right button click
	void SetShowPopupMenu(bool bShowPopupMenu) { m_bShowPopupMenu = bShowPopupMenu; }
	bool GetShowPopupMenu() { return m_bShowPopupMenu; }

	//Gets whether the hyperlink has been visited
	bool GetVisited() { return m_State == ST_VISITED; };

	//Gets whether the window is automatically adjusted to
	//the size of the description text displayed
	bool GetShrinkToFit()	{ return m_bShrinkToFit; }

	//Saves the hyperlink to an actual shortcut on file
#ifndef HLINK_NOOLE
	bool Save(const owl_string& sFilename) const;
#endif

	//Displays the properties dialog for this URL
	void ShowProperties() const;

	//Connects to the URL
	bool Open() const;

protected:
	enum State
	{
		ST_NOT_VISITED,
		ST_VISITED,
		ST_HIGHLIGHTED
	};

	void Init();

	bool EvSetCursor(THandle hWndCursor, uint hitTest, uint mouseMsg);
	void EvLButtonDown(uint modKeys, __OWL TPoint& point);
	void EvMouseMove(UINT modKeys, __OWL TPoint& point);
	bool EvEraseBkgnd(HDC hdc);
	void EvPaint();
	void EvCopyShortcut();
	void EvProperties();
	void EvOpen();
	void EvSetFocus(HWND hWndLostFocus);
	void EvContextMenu(HWND childHwnd, int x, int y);

#ifndef HLINK_NOOLE
	void EvAddToFavorites();
	void EvAddToDesktop();
#endif

	void SetActualHyperLink(const owl_string& sActualLink);
	void ShrinkToFitEditBox();
	void HighLight(bool state);

#ifndef HLINK_NOOLE
	bool AddToSpecialFolder(int nFolder) const;
	bool OpenUsingCom() const;
#endif
	bool OpenUsingShellExecute() const;

	owl_string   m_sLinkDescription;
	owl_string   m_sActualLink;
	HCURSOR  m_hLinkCursor;
	HCURSOR  m_hArrowCursor;
	__OWL TColor 	 m_Color;
	__OWL TColor 	 m_VisitedColor;
	bool     m_bShrinkToFit;
	bool     m_bUseHighlight;
	__OWL TColor 	 m_HighlightColor;
	State    m_State;
	bool     m_bUnderline;
	bool     m_bUseShadow;
	State    m_OldState;
	bool     m_bShowingContext;
	bool     m_bShowPopupMenu;

	DECLARE_RESPONSE_TABLE(THLinkCtrl);
};

class OWLEXTCLASS THLinkPage: public __OWL TPropertyPage {
	public:
		THLinkPage(__OWL TPropertySheet* parent);
		~THLinkPage();
		void SetBuddy(const THLinkCtrl* pBuddy)
			{ m_pBuddy = pBuddy; };

	protected:
		void SetupWindow();

	protected:
		const THLinkCtrl* m_pBuddy;
};

class OWLEXTCLASS THLinkSheet: public __OWL TPropertySheet {
	public:
		THLinkSheet(__OWL TWindow* parent);
		virtual ~THLinkSheet();
		void SetBuddy(const THLinkCtrl* pBuddy)
			{ m_page1->SetBuddy(pBuddy); }

	protected:
		THLinkPage* m_page1;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif //__OWLEXT_HLINKCTL_H

