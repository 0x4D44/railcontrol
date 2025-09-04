//----------------------------------------------------------------------------
// OWLNext
//
//$Revision: 1.2.2.1 $
//$Author: jogybl $
//$Date: 2010-05-17 07:12:34 $
//
// Microsoft UxTheme Library Encapsulation
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include <owl/theme.h>

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlTheme, 1, 0);

//
// Private constructor
// Loads the DLL and dynamically links the module functions.
//
TThemeModule::TThemeModule()
  : TModule(_T("uxtheme.dll"), true, true, false), // shouldLoad, mustLoad and !addToList

    // General
    //
    CloseThemeData(*this, "CloseThemeData"),
    DrawThemeParentBackground(*this, "DrawThemeParentBackground"),
    EnableThemeDialogTexture(*this, "EnableThemeDialogTexture"),
    EnableTheming(*this, "EnableTheming"),
    GetCurrentThemeName(*this, "GetCurrentThemeName"),
    GetThemeAppProperties(*this, "GetThemeAppProperties"),
    GetThemeDocumentationProperty(*this, "GetThemeDocumentationProperty"),
    GetWindowTheme(*this, "GetWindowTheme"),
    IsAppThemed(*this, "IsAppThemed"),
    IsThemeActive(*this, "IsThemeActive"),
    IsThemeDialogTextureEnabled(*this, "IsThemeDialogTextureEnabled"),
    OpenThemeData(*this, "OpenThemeData"),
    SetThemeAppProperties(*this, "SetThemeAppProperties"),
    SetWindowTheme(*this, "SetWindowTheme"),

    // Theme sys properties
    //
    GetThemeSysBool(*this, "GetThemeSysBool"),
    GetThemeSysColor(*this, "GetThemeSysColor"),
    GetThemeSysColorBrush(*this, "GetThemeSysColorBrush"),
    GetThemeSysFont(*this, "GetThemeSysFont"),
    GetThemeSysInt(*this, "GetThemeSysInt"),
    GetThemeSysSize(*this, "GetThemeSysSize"),
    GetThemeSysString(*this, "GetThemeSysString"),

    // Theme parts
    //
    DrawThemeBackground(*this, "DrawThemeBackground"),
//    DrawThemeBackgroundEx(*this, "DrawThemeBackgroundEx"),
    DrawThemeEdge(*this, "DrawThemeEdge"),
    DrawThemeIcon(*this, "DrawThemeIcon"),
    DrawThemeText(*this, "DrawThemeText"),
    GetThemeBackgroundContentRect(*this, "GetThemeBackgroundContentRect"),
    GetThemeBackgroundExtent(*this, "GetThemeBackgroundExtent"),
    GetThemeBackgroundRegion(*this, "GetThemeBackgroundRegion"),
    GetThemeBool(*this, "GetThemeBool"),
    GetThemeColor(*this, "GetThemeColor"),
    GetThemeEnumValue(*this, "GetThemeEnumValue"),
    GetThemeFilename(*this, "GetThemeFilename"),
    GetThemeFont(*this, "GetThemeFont"),
    GetThemeInt(*this, "GetThemeInt"),
    GetThemeIntList(*this, "GetThemeIntList"),
    GetThemeMargins(*this, "GetThemeMargins"),
    GetThemeMetric(*this, "GetThemeMetric"),
    GetThemePartSize(*this, "GetThemePartSize"),
    GetThemePosition(*this, "GetThemePosition"),
    GetThemePropertyOrigin(*this, "GetThemePropertyOrigin"),
    GetThemeRect(*this, "GetThemeRect"),
    GetThemeString(*this, "GetThemeString"),
    GetThemeTextExtent(*this, "GetThemeTextExtent"),
    GetThemeTextMetrics(*this, "GetThemeTextMetrics"),
    HitTestThemeBackground(*this, "HitTestThemeBackground"),
    IsThemeBackgroundPartiallyTransparent(*this, "IsThemeBackgroundPartiallyTransparent"),
    IsThemePartDefined(*this, "IsThemePartDefined")
{}

//
// Singleton accessor
//
TThemeModule& TThemeModule::GetInstance() 
{
  static TThemeModule instance;
  return instance;
}

// 
// Constructs a theme handler.
//
TTheme::TTheme(HWND w, LPCWSTR cls)
: m_handle(TThemeModule::GetInstance().OpenThemeData(w, cls))
{
  if (m_handle == NULL)
    TXTheme::Raise(_T("Unable to open theme")); // TODO: Load resource string.
}

// 
// Releases the theme handle.
//
TTheme::~TTheme()
{
  TThemeModule::GetInstance().CloseThemeData(m_handle);
}

//
// Constructs a theme part handler.
//
TThemePart::TThemePart(HWND w, LPCWSTR cls, int part, int state)
: TTheme(w, cls), m_part (part), m_state(state)
{}

//
// Paints the background of the themed part.
//
void TThemePart::DrawBackground(HDC dc, const TRect& dest)
{
  TThemeModule& m = TThemeModule::GetInstance();
  HRESULT r = m.DrawThemeBackground(GetHandle(), dc, m_part, m_state, &dest, NULL);
  if (r) TXTheme::Raise(_T("DrawThemeBackground failed"), r);
}

//
// Paints the background of the themed part using a clip rectangle.
//
void TThemePart::DrawBackground(HDC dc, const TRect& dest, const TRect& clip)
{
  TThemeModule& m = TThemeModule::GetInstance();
  HRESULT r = m.DrawThemeBackground(GetHandle(), dc, m_part, m_state, &dest, &clip);
  if (r) TXTheme::Raise(_T("DrawThemeBackground failed"), r);
}

//
// Paints the background of the themed part, properly composited with the parent background.
//
void TThemePart::DrawTransparentBackground(HWND parent, HDC dc, const TRect& dest)
{
  TThemeModule& m = TThemeModule::GetInstance();
  if (IsBackgroundPartiallyTransparent())
  {
    // The parent may or may not be themed so we just warn (level 1) if painting fails.
    //
    TRect rect = dest;
    HRESULT r = m.DrawThemeParentBackground(parent, dc, &rect);
    WARNX(OwlTheme, r, 1, _T("DrawThemeParentBackground failed"));
  }
  HRESULT r = m.DrawThemeBackground(GetHandle(), dc, m_part, m_state, &dest, NULL);
  if (r) TXTheme::Raise(_T("DrawThemeBackground failed"), r);
}

//
// Returns true if the theme-specified background for a part has 
// transparent pieces or alpha-blended pieces.
//
bool TThemePart::IsBackgroundPartiallyTransparent()
{
  TThemeModule& m = TThemeModule::GetInstance();
  return m.IsThemeBackgroundPartiallyTransparent(GetHandle(), m_part, m_state);
}

//
// Creates a theme exception.
//
TXTheme::TXTheme(LPCTSTR name, HRESULT r)
:
  TXOwl(name), result (r)
{
}

//
// Creates a copy of the exception (for 16-bit Windows).
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXTheme*
#endif
TXTheme::Clone()
{
  return new TXTheme(*this);
}

//
// Throws the exception.
//
void
TXTheme::Throw()
{
  THROW( *this );
}

//
// Throws the exception.
//
void
TXTheme::Raise(LPCTSTR name, HRESULT r)
{
  TXTheme(name, r).Throw();
}


__OWL_END_NAMESPACE
