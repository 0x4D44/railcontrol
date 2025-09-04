//------------------------------------------------------------------------------
// ObjectWindows. Copyright (c) 1998 by Yura Bidus  
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:35 $
//
// TUrlLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
//
//------------------------------------------------------------------------------

#if !defined(OWL_URLLINK_H)
#define OWL_URLLINK_H

// Macro defining class name (usable in resource definition)
#if !defined(OWL_URLLINK)
#  if defined(UNICODE)
#    define OWL_URLLINK L"OWL_UrlLink"
#  else
#    define OWL_URLLINK "OWL_UrlLink"
#  endif
#endif

#if !defined(RC_INVOKED)

#if !defined(OWL_STATIC_H)
# include <owl/static.h>
#endif
#if !defined(OWL_COLOR_H)
# include <owl/color.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class _OWLCLASS TTooltip;

//
// class MTUrlLink
// ~~~~~ ~~~~~~~~
//
class _OWLCLASS TUrlLink : public TStatic {
  // Construction/destruction
  public:
    TUrlLink(TWindow* parent, int id,
             LPCTSTR title,
             int x, int y, int w, int h,
             uint textLimit = 0,
             TModule* module = 0);

    TUrlLink(TWindow* parent, int resourceId, 
             uint textLimit = 0, TModule* module = 0);
    virtual ~TUrlLink();

  // Operations
  public:
    void     SetURL(LPCTSTR str);
    LPCTSTR GetURL() const;

    void      SetLinkColor(TColor& color);
    TColor  GetLinkColor() const;

    void     SetVisitedColor(TColor& color);
    TColor  GetVisitedColor() const;

    void     SetHoverColor(TColor& color);
    TColor  GetHoverColor() const;

    void     SetVisited(bool bVisited = true);
    bool     GetVisited() const;

    void     SetUnderline(bool bUnderline = true);
    bool     GetUnderline() const;

    // Retrieves/assigns tooltip of/to window
    //
    TTooltip* GetTooltip() const;
    void       EnableTooltip(bool enable=true);
    void      SetTooltip(TTooltip* tooltip);

  protected:
    // Override TWindow virtual member functions
    LPCTSTR GetClassName();
    bool    PreProcessMsg(MSG& msg);
    void    SetupWindow();
    void    GetWindowClass(WNDCLASS& wndClass);
    void    Paint(TDC& dc, bool erase, TRect& rect);

    // Message Handlers
    void    EvPaint();
    void    EvLButtonDown(uint modKeys, TPoint& point);
    void    EvMouseMove(uint modKeys, TPoint& point);
    uint    EvGetDlgCode(MSG * msg);
    void    EvSetFont(HFONT hFont, bool redraw);
    HFONT   EvGetFont();
    HBRUSH  EvCtlColor(HDC hDC, HWND hWndChild, uint ctlType);

    // implementation
    virtual void SetupCursor();
    virtual HINSTANCE GotoURL(LPCTSTR url, int showcmd);
    virtual void ReportError(int nError);

  public:
    void    PositionWindow();

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TUrlLink(const TUrlLink&);
    TUrlLink& operator =(const TUrlLink&);

  protected_data:
    TFont*    LinkFont;
    TTooltip*  Tooltip;
    TColor     LinkColor;    // Link color, defaults to blue
    TColor     VisitedColor; // Visited color, defaults to blue
    TColor     HoverColor;   // Hover color, defaults to blue
    owl_string UrlAddress;
    bool      bUnderline;
    bool      bVisited;
    bool      bOverControl;

  DECLARE_RESPONSE_TABLE(TUrlLink);
  //DECLARE_STREAMABLE(_OWLCLASS, TUrlLink, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TUrlLink, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TUrlLink );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------
// inlines
inline TTooltip* TUrlLink::GetTooltip() const{
  return Tooltip;
}
inline LPCTSTR TUrlLink::GetURL() const{
  return UrlAddress.c_str();
}
inline bool TUrlLink::GetUnderline() const{
  return bUnderline;
}
inline bool TUrlLink::GetVisited() const{
  return bVisited;
}
inline TColor TUrlLink::GetLinkColor() const {
  return LinkColor;
}
inline TColor TUrlLink::GetVisitedColor() const{
  return VisitedColor;
}
inline TColor TUrlLink::GetHoverColor() const{
  return HoverColor;
}

__OWL_END_NAMESPACE

#endif  // !RC_INVOKED

#endif // !defined(OWL_URLLINK_H)
