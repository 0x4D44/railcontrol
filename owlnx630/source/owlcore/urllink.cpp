//------------------------------------------------------------------------------
// ObjectWindows
// Bidus Yura 1998
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TUrlLink
//
//------------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_REGIRTRY_H)
#  include <owl/registry.h>
#endif
#if !defined(OWL_TOOLTIP_H)
# include <owl/tooltip.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#if !defined(OWL_SHELLITM_H)
# include <owl/shellitm.h>
#endif

#if !defined(OWL_URLLINK_H)
#include <owl/urllink.h>
#endif

#include <owl/urllink.rh>

#define TOOLTIP_ID 1003

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlControl);        // General Controls diagnostic group


//----------------------------------------------------------------------------
DEFINE_RESPONSE_TABLE1(TUrlLink, TStatic)
  EV_WM_PAINT,
  EV_WM_LBUTTONDOWN,
  EV_WM_MOUSEMOVE,
  EV_WM_CTLCOLOR,
  EV_WM_GETFONT,
  EV_WM_SETFONT,
  EV_WM_GETDLGCODE,
END_RESPONSE_TABLE;


TUrlLink::TUrlLink(TWindow* parent, int id, LPCTSTR title,
             int x, int y, int w, int h, uint textLimit/* = 0*/,
             TModule* module/* = 0*/)
:
  TStatic(parent, id, title,x, y, w, h, textLimit,module),
  LinkFont(0),
  Tooltip(0),
  LinkColor(TColor::LtBlue),//
  VisitedColor(85,  26, 139),
  HoverColor(TColor::LtRed),
  bUnderline(true),
  bVisited(false),
  bOverControl(false)
{
  SetupCursor();
}

TUrlLink::TUrlLink(TWindow* parent, int resourceId, uint textLimit/* = 0*/,
                   TModule* module/* = 0*/)
:
  TStatic(parent, resourceId, textLimit, module),
  LinkFont(0),
  Tooltip(0),
  LinkColor(TColor::LtBlue),
  VisitedColor(85,  26, 139),
  HoverColor(TColor::LtRed),
  bUnderline(true),
  bVisited(false),
  bOverControl(false)
{
  SetupCursor();
}

TUrlLink::~TUrlLink()
{
  delete LinkFont;
}

//
// Return the Window class name of the notetab control object
//
LPCTSTR
TUrlLink::GetClassName()
{
  return OWL_URLLINK;
}

// Overriden virtual of TWindow - Initialize font used by control and resize
// accordingly.
//
void
TUrlLink::SetupWindow()
{
  TStatic::SetupWindow();

  // Try to get our parent's font
  HFONT font = GetParentO()->GetWindowFont();
  // Settle for the ANSI proportional one
  LOGFONT logFont;
  if(font){
    if(TFont(font).GetObject(logFont)){
      logFont.lfUnderline = bUnderline;
      LinkFont = new TFont(&logFont);
    }
  }
  else{
    if(TDefaultGUIFont().GetObject(logFont)){
      logFont.lfUnderline = bUnderline;
      LinkFont = new TFont(&logFont);
    }
  }
  CHECK(LinkFont != 0);
   // Set the URL as the window text
   TTmpBuffer<_TCHAR> Buffer(MAX_PATH);
   GetText(Buffer, MAX_PATH);
#if !defined(BI_STD_STRING)
  if (UrlAddress.is_null())
#else
  if (UrlAddress.empty())
#endif
    UrlAddress = (_TCHAR*)Buffer;

   if(!::_tcslen(Buffer)){
#if !defined(BI_STD_STRING)
    CHECK(!UrlAddress.is_null());
#else
    CHECK(!UrlAddress.empty());
#endif
     SetText(UrlAddress.c_str());
  }

   PositionWindow();

  if (Tooltip && !Tooltip->GetHandle()){
    // Make sure tooltip is disabled so it does not take input focus
    Tooltip->ModifyStyle(0,WS_DISABLED);
    Tooltip->Create();
  }
  if(Tooltip){
    TRect rect;
     GetClientRect(rect);
    TToolInfo ti(*this, rect, TOOLTIP_ID);
    ti.SetText(UrlAddress.c_str());
    Tooltip->AddTool(ti);
  }
}
//
void       
TUrlLink::EnableTooltip(bool enable)
{
  if (!Tooltip) {
    // Create and initialize tooltip
    //
    SetTooltip(new TTooltip(this));
  }
  else {
    if (Tooltip->GetHandle())
      Tooltip->Activate(enable);
  }
}
//
void      
TUrlLink::SetTooltip(TTooltip* tooltip)
{
  // Cleanup; via Condemned list if tooltip was created
  //
  if (Tooltip) {
    if (Tooltip->GetHandle())
      Tooltip->SendMessage(WM_CLOSE);
    else
      delete Tooltip;
  }

  // Store new tooltip and create if necessary
  //
  Tooltip = tooltip;
  if (Tooltip) {
    if(GetHandle() && !Tooltip->GetHandle()) {
      // Make sure tooltip is disabled so it does not take input focus
      Tooltip->ModifyStyle(0,WS_DISABLED);
      Tooltip->Create();
      TRect rect;
       GetClientRect(rect);
      TToolInfo ti(*this, rect, TOOLTIP_ID);
      ti.SetText(UrlAddress.c_str());
      Tooltip->AddTool(ti);
    }
  }
}

//
// Relay 'interesting' messages to the tooltip window
//
bool
TUrlLink::PreProcessMsg(MSG& msg)
{
  // Check if this message might be worth relaying to the tooltip window
  //
  if (Tooltip && Tooltip->IsWindow()) {
    if (msg.hwnd == *this || IsChild(msg.hwnd)) {
      if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
        Tooltip->RelayEvent(msg);
      }
    }
  }

  // Always let event go through.
  //
  return TStatic::PreProcessMsg(msg);
}

//
// Window proc. of control to handle messages sent before OWL thunks
//
LRESULT CALLBACK OWL_EXPORT16
UrlLinkProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg) {
    case WM_GETDLGCODE:
      return DLGC_STATIC|DLGC_WANTMESSAGE;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}


// Overriden virtual of TWindow - Fills out information about the Window
// class associated with a glyph button.
// NOTE: The class information is based on the system's "BUTTON" class.
//
void
TUrlLink::GetWindowClass(WNDCLASS& wndClass)
{
  // Grab a the attributes of the native "STATIC" control
  if (::GetClassInfo(0, _T("STATIC"), &wndClass)){
    wndClass.hInstance       = *GetModule();
    wndClass.style           = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
    wndClass.lpszClassName   = GetClassName();
    wndClass.lpfnWndProc     = UrlLinkProc;
    wndClass.hCursor         = HCursor;
    wndClass.hbrBackground  = HBRUSH(COLOR_BTNFACE+1);
  }
  else {
    TStatic::GetWindowClass(wndClass);
    wndClass.style           = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
    wndClass.hbrBackground  = HBRUSH(COLOR_BTNFACE+1);
    wndClass.hCursor         = HCursor;
  }
}

//
// Invoke 'PaintButton' to display glyph and/or text.
//
void
TUrlLink::Paint(TDC& dc, bool /*erase*/, TRect& /*rect*/)
{
  TPointer<TFont>  tmpFnt; // Object wrapping font handle
  TAPointer<TCHAR> text;   // Pointer to caption dynamic buffer
  // Select the font
  if(!LinkFont){
    HFONT hFont = HFONT(::SendMessage(::GetParent(*this), WM_GETFONT, 0, 0));
    if (!hFont)
      hFont = HFONT(GetStockObject(ANSI_VAR_FONT));
    if (hFont)
      tmpFnt = new TFont(hFont);
  }
  if (LinkFont) {
    CHECK(LinkFont->IsGDIObject());
    dc.SelectObject(*LinkFont);
  }
  else if (tmpFnt) {
    CHECK(((TFont&)tmpFnt).IsGDIObject());
    dc.SelectObject((TFont&)tmpFnt);
  }
  int len = GetWindowTextLength();
  text = new TCHAR[len+1];
  GetWindowText(text, len+1);
  TRect textRect;
  GetClientRect(textRect);

  TColor textColor = LinkColor;
  if (bOverControl)
    textColor = HoverColor;
  else if (bVisited)
    textColor = VisitedColor;

  TColor oldColor = dc.SetTextColor(textColor);
  TColor oldBkColor = dc.SetBkColor(TColor::Sys3dFace);
  dc.DrawText(text, len, textRect, 0);
  dc.SetBkColor(oldBkColor);
  dc.SetTextColor(oldColor);

  // Restore font
  if (LinkFont || tmpFnt)
    dc.RestoreFont();
}


//
// WM_PAINT handler - Invokes 'Paint' method to display glyph and/or text.
//
void
TUrlLink::EvPaint()
{
  TPaintDC dc(*this);
  TRect&   rect = *(TRect*)&dc.Ps.rcPaint;
  Paint(dc, dc.Ps.fErase, rect);
}
//
// Respond to WM_CTLCOLOR if we have a bkgnd color set & pass that to the
// control
//
HBRUSH
TUrlLink::EvCtlColor(HDC hDC, HWND hWndChild, uint ctlType)
{
  TDC dc(hDC);
  if (bOverControl)
    dc.SetTextColor(HoverColor);
  else if (bVisited)
    dc.SetTextColor(VisitedColor);
  else
    dc.SetTextColor(LinkColor);
  return TStatic::EvCtlColor(hDC, hWndChild, ctlType);
}

// WM_LBUTTONDOWN handler -
//
void
TUrlLink::EvLButtonDown(uint modKeys, TPoint& point)
{
  TStatic::EvLButtonDown(modKeys, point);
  // set url !!!!!!!!!!!!!!!!!!1
  int result = (int)GotoURL(UrlAddress.c_str(), SW_SHOW);
  if (result > (int)HINSTANCE_ERROR){
    ReleaseCapture();
    bOverControl = false;
    Invalidate();
    SetVisited();                        // Repaint to show visited colour
  }
  else{
    MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
    ReportError(result);
  }
}

//
// WM_MOUSEMOVE handler. Update state of button if we're in 'capture' mode.
//
void
TUrlLink::EvMouseMove(uint modKeys, TPoint& point)
{
  TStatic::EvMouseMove(modKeys, point);
  if(bOverControl){
    TRect rect;
    GetClientRect(rect);
    if(!rect.Contains(point)){
      ReleaseCapture();
      bOverControl = false;
      Invalidate();
    }
  }
  else{
     SetCapture();
    bOverControl = true;
    Invalidate();
  }
}

//
// WM_GETFONT handler - Returns font used by control if one was specified
// earlier. Otherwise, returns 0.
//
HFONT
TUrlLink::EvGetFont()
{
  PRECONDITION(!LinkFont || LinkFont->IsGDIObject());
  return LinkFont ? HFONT(*LinkFont) : HFONT(0);
}


//
// WM_SETFONT Handler. Deletes any cached font and stores copy of new one.
//
void
TUrlLink::EvSetFont(HFONT hFont, bool redraw)
{
  TStatic::EvSetFont(hFont, redraw);
  delete LinkFont;
  LOGFONT logFont;
  if(TFont(hFont).GetObject(logFont)){
     logFont.lfUnderline = bUnderline;
     LinkFont = new TFont(&logFont);
  }
  if (redraw)
    Invalidate();
}

// WM_GETDLGCODE handler.
//
uint
TUrlLink::EvGetDlgCode(MSG* /*msg*/)
{
  return DLGC_STATIC|DLGC_WANTMESSAGE;
}

//
// Set new URL
//
void
TUrlLink::SetURL(LPCTSTR str)
{
  UrlAddress = str;

  if(Tooltip && GetHandle()){
    TRect rect;
     GetClientRect(rect);
    TToolInfo info(*this, rect, TOOLTIP_ID);
    Tooltip->GetToolInfo(info);
    info.SetText(str);
    Tooltip->UpdateTipText(info);
  }
}

//
// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void
TUrlLink::SetupCursor()
{
  SetCursor(GetModule(), IDC_HANDCURSOR);
  if(HCursor == 0){
    // if was problem try load cursor from winhlp32.exe
    // Get the windows directory
    TAPointer<TCHAR> Buffer = new TCHAR[MAX_PATH];
    ::GetWindowsDirectory(Buffer, MAX_PATH);
     _tcscat(Buffer,_T("\\winhlp32.exe"));
    // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
    HMODULE hModule = ::LoadLibrary(Buffer);
    if (hModule) {
      HCURSOR hHandCursor = ::LoadCursor(hModule, TResId(106));
      if (hHandCursor)
        HCursor = CopyCursor(hHandCursor); // it is a macro in Win32

      ::FreeLibrary(hModule);
    }
  }
}

//
//
//
void
TUrlLink::SetVisited(bool visited/* = true*/)
{
  bVisited = visited;
  if (GetHandle())
    Invalidate();
}

//
//
//
void
TUrlLink::SetUnderline(bool bUnderline/* = true */)
{
  if(bUnderline == bUnderline)
    return;
  bUnderline = bUnderline;
  if (GetHandle()){
    LOGFONT logFont;
    if(LinkFont->GetObject(logFont)){
       logFont.lfUnderline = bUnderline;
      delete LinkFont;
       LinkFont = new TFont(&logFont);
    }
    Invalidate();
  }
}

//
//
//
void
TUrlLink::SetLinkColor(TColor& color)
{
  LinkColor = color;
  if (GetHandle())
    Invalidate();
}

//
//
//
void
TUrlLink::SetVisitedColor(TColor& color)
{
  VisitedColor = color;
  if (GetHandle())
    Invalidate();
}

//
//
//
void
TUrlLink::SetHoverColor(TColor& color)
{
  HoverColor = color;
  if (GetHandle())
    Invalidate();
}

// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
// Suggested by Pål K. Tønder
void
TUrlLink::PositionWindow()
{
  // Get the current window position
  TRect rect;
  GetWindowRect(rect);

  TWindow* pParent = GetParentO();
  if(pParent){
    pParent->ScreenToClient(rect.TopLeft());
    pParent->ScreenToClient(rect.BottomRight());
  }

  // Get the size of the window text
  TTmpBuffer<_TCHAR> buffer(MAX_PATH);
  GetText(buffer, MAX_PATH);

  TSize size = LinkFont->GetTextExtent(buffer);

  // Get the text justification via the window style
  uint32 style = GetStyle(); //TWindow

  // Recalc the window size and position based on the text justification
  if (style & SS_CENTERIMAGE)
    rect.Inflate(0, -(rect.Height() - size.cy)/2);
  else
    rect.bottom = rect.top + size.cy;

   if (style & SS_CENTER)
    rect.Inflate( -(rect.Width() - size.cx)/2, 0);
  else if (style & SS_RIGHT)
    rect.left  = rect.right - size.cx;
  else // SS_LEFT = 0, so we can't test for it explicitly
    rect.right = rect.left + size.cx;

  // Move the window
  SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

//
//
//
HINSTANCE
TUrlLink::GotoURL(LPCTSTR url, int showcmd)
{
  // First try ShellExecute()
  HINSTANCE result = TShell::ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

  // If it failed, get the .htm regkey and lookup the program
   if ((uint)result <= (uint)HINSTANCE_ERROR){
    owl_string tmp_str = _T(".htm");
    owl_string key = TRegValue(TRegKey::GetClassesRoot(),tmp_str.c_str()).GetName();
#if !defined(BI_STD_STRING)
    if(!key.is_null()){
#else
    if(!key.empty()){
#endif
      tmp_str = key;
      tmp_str += _T("\\shell\\open\\command");
      key = TRegValue(TRegKey::GetClassesRoot(),tmp_str.c_str()).GetName();
#if !defined(BI_STD_STRING)
      if(!key.is_null()){
#else
      if(!key.empty()){
#endif
        size_t pos = key.find(owl_string(_T("\"%1\"")));
        if(pos == NPOS){
          pos = key.find(owl_string(_T("%1")));
          if(pos != NPOS)
#if !defined(BI_STD_STRING)
            key.remove(pos);
#else
            key.erase(pos);
#endif
        }
        else
#if !defined(BI_STD_STRING)
           key.remove(pos);
#else
          key.erase(pos);
#endif
        key += _T(" ");
        key += url;
        _USES_CONVERSION;
        result = (HINSTANCE) WinExec(_W2A(key.c_str()), showcmd);
      }
    }
  }
  return result;
}

//
//
//
void
TUrlLink::ReportError(int error)
{
  uint resId;
  switch (error) {
    case 0:                       resId = IDS_OUTOFMEMORY; break;
    case SE_ERR_PNF:              resId = IDS_PATHNOTFOUND; break;
    case SE_ERR_FNF:              resId = IDS_FILENOTFOUIND; break;
    case ERROR_BAD_FORMAT:        resId = IDS_FILEINVALID; break;
    case SE_ERR_ACCESSDENIED:     resId = IDS_ACCESSDENIED; break;
    case SE_ERR_ASSOCINCOMPLETE:  resId = IDS_INVALIDASSOC; break;  //  Win32/Win16
    case SE_ERR_DDEBUSY:          resId = IDS_DDEBISY; break;        //  Win32/Win16
    case SE_ERR_DDEFAIL:          resId = IDS_DDEFAILED; break;      //  Win32/Win16
    case SE_ERR_DDETIMEOUT:       resId = IDS_DDETYMEOUT; break;    //  Win32/Win16
    case SE_ERR_DLLNOTFOUND:      resId = IDS_FILENOTFOUIND; break;
    case SE_ERR_NOASSOC:          resId = IDS_NOINSTANCE; break;    //  Win32/Win16
    case SE_ERR_OOM:              resId = IDS_OUTOFMEMORY; break;
    case SE_ERR_SHARE:            resId = IDS_SHAREDERROR; break;    //  Win32/Win16
    default:{
       _TCHAR buffer[MAX_PATH];
      wsprintf(buffer, GetModule()->LoadString(IDS_UNCKNOWNERR).c_str(), error);
      MessageBox(buffer, 0, MB_ICONEXCLAMATION | MB_OK);
      return;
    }
  }
  //str = "Unable to open hyperlink:\n\n" + str;
  MessageBox(GetModule()->LoadString((uint)(LPCTSTR)resId).c_str(), 0, MB_ICONEXCLAMATION | MB_OK);
  return;
}

IMPLEMENT_STREAMABLE1(TUrlLink, TStatic);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TUrlLink::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TStatic*)GetObject(), is);

  GetObject()->LinkFont = 0;

  is >> GetObject()->LinkColor;
  is >> GetObject()->VisitedColor;
  is >> GetObject()->HoverColor;
  is >> GetObject()->bUnderline;
  is >> GetObject()->bVisited;
  is >> GetObject()->bOverControl;

  GetObject()->SetupCursor();

  return GetObject();
}

//
//
//
void
TUrlLink::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TStatic*)GetObject(), os);

  os << GetObject()->LinkColor;
  os << GetObject()->VisitedColor;
  os << GetObject()->HoverColor;
  os << GetObject()->bUnderline;
  os << GetObject()->bVisited;
  os << GetObject()->bOverControl;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

__OWL_END_NAMESPACE
/* ========================================================================== */

