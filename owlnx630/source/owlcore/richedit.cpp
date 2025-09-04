//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.2 $
//$Author: jogybl $
//$Date: 2009-12-16 09:10:46 $
//
// Implementation of class TRichEdit.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif
#if !defined(OWL_EDIT_H)
# include <owl/edit.h>
#endif
#if !defined(OWL_EDITSEAR_H)
# include <owl/editsear.h>
#endif
#if !defined(OWL_EDITFILE_H)
# include <owl/editfile.h>
#endif
#if !defined(OWL_RICHEDIT_H)
# include <owl/richedit.h>
#endif
#if !defined(OWL_FILENAME_H)
# include <owl/filename.h>
#endif
/*
#if defined(BI_STD_IO)
#  if defined(UNICODE)
    typedef std::wifstream _tifstream;
    typedef std::wofstream _tofstream;
#  else
    typedef std::ifstream _tifstream;
    typedef std::ofstream _tofstream;
#  endif
#else
  typedef ifstream _tifstream;
  typedef ofstream _tofstream;
#endif
*/
__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

// Range of the editor's font size
//
const int MinPointSize = 6;
const int MaxPointSize = 128;

// System DLL providing underlying support for RichEdit controls
//
const TCHAR RichEditDllName[]    = _T("RICHED32.DLL");
const TCHAR RichEdit20DllName[]  = _T("RICHED20.DLL");

////////////////////////////////////////////////////////////////////////////////
//
//

//
// Constructs a CharacterFormat structure from the current character
// attributes of a RICHEDIT control. NOTE: Specifying 'true' for selection
// will return the attribute of the character at the current location if
// there are no block of data selected in the control.
//
TCharFormat::TCharFormat(const TRichEdit& edit, bool selection, ulong mask)
{
  PRECONDITION((HWND)edit);

  memset(this, 0, sizeof(CHARFORMAT2));

  cbSize = sizeof(CHARFORMAT);
  if(TRichEditDll::Dll()->GetVersion() >= 2)
     cbSize = sizeof(CHARFORMAT2);

  dwMask = mask;
  edit.GetCharFormat(*this, selection);
}

//
// Toggles the 'bold' character attribute according to boolean parameter
// specified.
//
void
TCharFormat::EnableBold(bool flag)
{
  dwMask |= CFM_BOLD;
  if (flag) 
    dwEffects |= CFE_BOLD;
  else 
    dwEffects &= ~CFE_BOLD;
}

//
// Toggles 'italic' character attribute based on the boolean parameter
// specified.
//
void
TCharFormat::EnableItalic(bool flag)
{
  dwMask |= CFM_ITALIC;
  if (flag) 
    dwEffects |= CFE_ITALIC;
  else 
    dwEffects &= ~CFE_ITALIC;
}

//
// Toggle the 'underline' character attribute based on the boolean parameter
// specified.
//
void
TCharFormat::EnableUnderline(bool flag)
{
  dwMask |= CFM_UNDERLINE;
  if (flag) 
    dwEffects |= CFE_UNDERLINE;
  else 
    dwEffects &= ~CFE_UNDERLINE;
}

//
// Toggle the 'strike-out' character attribute based on the boolean parameter
// specified.
//
void
TCharFormat::EnableStrikeOut(bool flag)
{
  dwMask |= CFM_STRIKEOUT;
  if (flag) 
    dwEffects |= CFE_STRIKEOUT;
  else 
    dwEffects &= ~CFE_STRIKEOUT;
}

//
// Toggle the 'protected' character attribute based on the boolean parameter
// specified.
//
void
TCharFormat::EnableProtected(bool flag)
{
  dwMask |= CFM_PROTECTED;
  if (flag) 
    dwEffects |= CFE_PROTECTED;
  else 
    dwEffects &= ~CFE_PROTECTED;
}

//
// Retrieve the character color stored in the CHARFORMAT structure.
// NOTE: Default to system text color if no explicit color was set
//       in the CHARFORMAT structure.
//
TColor
TCharFormat::GetTextColor() const
{
  if ((dwMask & CFM_COLOR) && !(dwEffects & CFE_AUTOCOLOR))
    return TColor(crTextColor);
  return TColor::SysWindowText;
}

//
// Update the CHARFORMAT structure with the specified color.
// NOTE: If 'TColor::None' is specified, enable the flag specifying that the
//       color should default to the system text color .
//
void
TCharFormat::SetTextColor(const TColor& color)
{
  dwMask |= CFM_COLOR;
  if (color == TColor::None) 
    dwEffects |= CFE_AUTOCOLOR;
  else {
    dwEffects &= ~CFE_AUTOCOLOR;
    crTextColor = color;
  }
}

//
// Set the face name of the font.
//
void
TCharFormat::SetFaceName(LPCTSTR name)
{
  //!BB No unicode support!!!!!!!!!!
  ::_tcscpy(szFaceName, name);
}

//
// Set the character set of the font. Valid values include the following:
// ANSI_CHARSET, OEM_CHARSET and SYMBOL_CHARSET.
//
void
TCharFormat::SetCharSet(uint8 charSet)
{
  bCharSet = charSet;
}

//
// Set the pitch and family of the font. The two lower-bits specify the pitch
// of the font and can be one of the following values:
// DEFAULT_PITCH, FIXED_PITCH and VARIABLE_PITCH.

// Bits 4 through 7 of the member specify the font family and can be
// one of the following values:
// FF_DECORATIVE, FF_DONTCARE, FF_MODERN, FF_ROMAN, FF_SCRIPT & FF_SWISS.
//
void
TCharFormat::SetPitchAndFamily(uint8 pitchFam)
{
  bPitchAndFamily = pitchFam;
}

//
// Set the character height
//
void
TCharFormat::SetHeight(long height)
{
  dwMask |= CFM_SIZE;
  yHeight = height;
}

//
// Set the character offset from the baseline. If the parameter is positive,
// the character is a superscript; if it is negative, the character is a
// subscript
//
void
TCharFormat::SetOffset(long offset)
{
  dwMask |= CFM_OFFSET;
  yOffset = offset;
}

//
// Transfer the information currently in the underlying CHARFORMAT structure
// to a LOGFONT structure.
// This is useful when changing the editor's font as initialized LOGFONT
// structure can subsequently be used when invoking the FONT Common
// Dialog, i.e. TChooseFontDialog
//
void
TCharFormat::SetFontInfo(LOGFONT& lf) const
{
  memset(&lf, 0, sizeof(LOGFONT));

  if (dwMask & CFM_SIZE)                //  (1 Point == 20 twips)
    lf.lfHeight = yHeight/-20;

  if (dwMask & CFM_BOLD)
    lf.lfWeight = dwEffects & CFE_BOLD ? FW_BOLD : FW_NORMAL;

  if (dwMask & CFM_ITALIC)
    lf.lfItalic = (uint8)(dwEffects & CFE_ITALIC ? TRUE : FALSE);

  if (dwMask & CFM_UNDERLINE)
    lf.lfUnderline = (uint8)(dwEffects & CFE_UNDERLINE ? TRUE : FALSE);

  if (dwMask & CFM_FACE) {
    lf.lfPitchAndFamily = bPitchAndFamily;
    ::_tcscpy(lf.lfFaceName, szFaceName);
  }

  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfQuality = DEFAULT_QUALITY;
}

//
// Initialize the underlying CHARFORMAT structure using the information
// stored in a LOGFONT structure.
//
void
TCharFormat::GetFontInfo(const LOGFONT& lf)
{
  dwMask = (CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_OFFSET|
            CFM_STRIKEOUT | CFM_UNDERLINE | CFM_FACE);

  yHeight = lf.lfHeight * -20;

  if (FW_BOLD == lf.lfWeight)
    dwEffects |= CFE_BOLD;
  if (lf.lfItalic)
    dwEffects |= CFE_ITALIC;
  if (lf.lfStrikeOut)
    dwEffects |= CFE_STRIKEOUT;
  if (lf.lfUnderline)
    dwEffects |= CFE_UNDERLINE;

  bPitchAndFamily = lf.lfPitchAndFamily;
  ::_tcscpy(szFaceName, lf.lfFaceName);
}

//
// Font weight (LOGFONT value)
// RichEdit 2.0 specific
void
TCharFormat::SetWeight(uint16 weigh)
{
  dwMask |= CFM_WEIGHT;
  wWeight = weigh;
}

//
// Amount to space between letters
// RichEdit 2.0 specific
void
TCharFormat::SetSpacing(int16 spacing)
{
  dwMask |= CFM_SPACING;
  wWeight = spacing;
}

//
// Background color
// RichEdit 2.0 specific
void
TCharFormat::SetBkColor(const TColor& clr)
{
  dwMask |= CFM_COLOR;
  crBackColor = clr;
}

//
// Locale ID
// RichEdit 2.0 specific
void
TCharFormat::SetLCID(LCID _lcid)
{
  dwMask |= CFM_LCID;
  lcid = _lcid;
}

//
// Style handle
// RichEdit 2.0 specific
void
TCharFormat::SetStyle(int16 style)
{
  dwMask |= CFM_STYLE;
  sStyle = style;
}

//
// Twip size above which to kern char pair
// RichEdit 2.0 specific
void
TCharFormat::SetKerning(uint16 kern)
{
  dwMask |= CFM_STYLE;
  wKerning = kern;
}

//
// Underline type
// RichEdit 2.0 specific
void
TCharFormat::SetUnderlineType(uint8 utype)
{
  dwMask |= CFM_UNDERLINETYPE;
  bUnderlineType = utype;
}

//
// Animated text like marching ants
// RichEdit 2.0 specific
void
TCharFormat::SetAnimation(uint8 anim)
{
  dwMask |= CFM_ANIMATION;
  bAnimation = anim;
}

//
// Revision author index
// RichEdit 2.0 specific
void
TCharFormat::SetRevAuthor(uint8 revav)
{
  dwMask |= CFM_REVAUTHOR;
  bRevAuthor = revav;
}

////////////////////////////////////////////////////////////////////////////////
//
//

//
// Construct a TFormatRange object initializing data members with the
// specified parameters.
//
TFormatRange::TFormatRange(HDC renderDC, HDC devDC, const TRect& renderArea,
                           const TRect& entireArea, const TCharRange& range)
{
  SetRenderDC(renderDC);
  SetTargetDC(devDC);
  SetRenderRect(renderArea);
  SetPageRect(entireArea);
  SetRange(range);
}

//
// Set the device context of the device to render to.
//
void
TFormatRange::SetRenderDC(HDC renderDC)
{
  hdc = renderDC;
}

//
// Set the device context of the target device to format for.
//
void
TFormatRange::SetTargetDC(HDC targetDC)
{
  hdcTarget = targetDC;
}

//
// Set the area to render to.
// NOTE: The specified units are in TWIPS.
//
void
TFormatRange::SetRenderRect(const TRect& renderRect)
{
  rc = renderRect;
}

//
// Set the entire area of the rendering device.
// NOTE: The specified units are in TWIPS.
//
void
TFormatRange::SetPageRect(const TRect& pgRect)
{
  rcPage = pgRect;
}

//
// Set the range of text to format.
//
void
TFormatRange::SetRange(const TCharRange& charRange)
{
  chrg = charRange;
}

//
// Set the range of text to format specifying the starting and ending
// character offsets.
//
void
TFormatRange::SetRange(long start, long end)
{
  chrg.cpMin = start;
  chrg.cpMax = end;
}

////////////////////////////////////////////////////////////////////////////////
//
// Construct a default TParaFormat structure.
//
TParaFormat::TParaFormat(ulong mask)
{
  memset(this, 0, sizeof(PARAFORMAT2));

  cbSize = sizeof(PARAFORMAT);
  if(TRichEditDll::Dll()->GetVersion() >= 2)
     cbSize = sizeof(PARAFORMAT2);

  dwMask = mask;
}

//
// Construct a TParaFormat structure whose members are initialized with the
// paragraph formatting attributes of the current selection of a rich edit
// control.
//
TParaFormat::TParaFormat(const TRichEdit& edit, ulong mask)
{
  PRECONDITION((HWND)edit);

  memset(this, 0, sizeof(PARAFORMAT2));

  cbSize = sizeof(PARAFORMAT);
  if(TRichEditDll::Dll()->GetVersion() >= 2)
     cbSize = sizeof(PARAFORMAT2);

  dwMask = mask;
  edit.GetParaFormat(*this);
}

//
// Toggle the specified flag in the member which describes which information
// of the PARAFORMAT structures is valid.
//
void
TParaFormat::ToggleMaskBit(ulong flag)
{
  dwMask ^= flag;
}

//
// Set the numbering options. The only valid parameter is '0' or PFN_BULLET.
//
void
TParaFormat::SetNumbering(uint16 opt)
{
  PRECONDITION(opt == 0 || opt == PFN_BULLET);

  dwMask |= PFM_NUMBERING;
  wNumbering = opt;
}

//
// Set the indentation of the first line in the paragraph. If the paragraph
// formatting is being set and the 'relative' parameter is true, the 'start'
// value is  treated as a relative value that is added to the starting
// indentation of each affected paragraph.
//
void
TParaFormat::SetStartIndent(long start, bool relative)
{
  dwMask |= PFM_STARTINDENT;
  if (relative)
    dwMask |= PFM_OFFSETINDENT;
  else
    dwMask &= ~PFM_OFFSETINDENT;

  dxStartIndent = start;
}

//
// Set the size of the right identation, relative to the right margin.
//
void
TParaFormat::SetRightIndent(long indent)
{
  dwMask |= PFM_RIGHTINDENT;
  dxRightIndent = indent;
}

//
// Set the indentation of the second line and subsequent lines,
// relative to the starting indentation. The first line is indented if
// the 'offset' parameter is negative, or outdented if it is positive.
//
void
TParaFormat::SetOffset(long offset)
{
  dwMask |= PFM_OFFSET;
  dxOffset = offset;
}

//
// Set the alignment option. The 'opt' parameter can be one of the following
// values:
//    PFA_LEFT        Paragraphs are aligned with the left margin.
//    PFA_RIGHT       Paragraphs are aligned with the right margin.
//    PFA_CENTER      Paragraphs are centered.
//
void
TParaFormat::SetAlignment(uint16 opt)
{
  dwMask |= PFM_ALIGNMENT;
  wAlignment = opt;
}

//
// Set the number and absolute positions of the tab stops.
//
void
TParaFormat::SetTabCount(short cnt, long* tabs)
{
  PRECONDITION(tabs || !cnt);

  dwMask |= PFM_TABSTOPS;
  cTabCount = cnt;
  for (int i=0; i<cnt; i++)
    rgxTabs[i] = *tabs++;
}

//
// Vertical spacing before para
// RichEdit 2.0 specific
void
TParaFormat::SetSpaceBefore(long space)
{
  dwMask |= PFM_SPACEBEFORE;
  dySpaceBefore = space;
}

//
// Vertical spacing after para
// RichEdit 2.0 specific
void
TParaFormat::SetSpaceAfter(long space)
{
  dwMask |= PFM_SPACEAFTER;
  dySpaceAfter = space;
}

//
// Line spacing depending on Rule
// RichEdit 2.0 specific
void
TParaFormat::SetLineSpacing(long space, uint8 rule)
{
  dwMask |= PFM_LINESPACING;
   dyLineSpacing      = space;
  bLineSpacingRule  = rule;
}

//
// Style handle
// RichEdit 2.0 specific
void
TParaFormat::SetStyle(int16 style)
{
  dwMask |= PFM_STYLE;
  sStyle = style;
}

//
// Shading in hundredths of a per cent
// Nibble 0: style, 1: cfpat, 2: cbpat
// RichEdit 2.0 specific
void
TParaFormat::SetShading(uint16 wight,uint16 style)
{
  dwMask |= PFM_SHADING;
   wShadingWeight  = wight;
  wShadingStyle   = style;
}

//
// Starting value for numbering
// RichEdit 2.0 specific
void
TParaFormat::SetNumStart(uint16 start)
{
  dwMask |= PFM_NUMBERINGSTART;
   wNumberingStart  = start;
}

//
// Alignment, roman/arabic, (), ), ., etc.
// RichEdit 2.0 specific
void
TParaFormat::SetNumStyle(uint16 style)
{
  dwMask |= PFM_NUMBERINGSTYLE;
   wNumberingStyle  = style;
}

//
// Space bet 1st indent and 1st-line text
// RichEdit 2.0 specific
void
TParaFormat::SetNumTab(uint16 tab)
{
  dwMask |= PFM_NUMBERINGTAB;
   wNumberingTab  = tab;
}

//
// Space between border and text (twips)
// RichEdit 2.0 specific
void
TParaFormat::SetBorder(uint16 space, uint16 width, uint16 border)
{
  dwMask |= PFM_BORDER;
  wBorderSpace  = space;
  wBorderWidth  = width;
  wBorders      = border;
}

////////////////////////////////////////////////////////////////////////////////
//
//

DEFINE_RESPONSE_TABLE1(TRichEdit, TEditFile)
  EV_COMMAND(CM_EDITCUT, CmEditCut),
  EV_COMMAND(CM_EDITCOPY, CmEditCopy),
  EV_COMMAND(CM_EDITPASTE, CmEditPaste),
  EV_COMMAND(CM_EDITDELETE, CmEditDelete),
  EV_COMMAND(CM_EDITCLEAR, CmEditClear),
  EV_COMMAND(CM_EDITUNDO, CmEditUndo),
  EV_COMMAND_ENABLE(CM_EDITCUT, CeHasSelect),
  EV_COMMAND_ENABLE(CM_EDITCOPY, CeHasSelect),
  EV_COMMAND_ENABLE(CM_EDITDELETE, CeHasSelect),
  EV_COMMAND_ENABLE(CM_EDITPASTE, CeEditPaste),
  EV_COMMAND_ENABLE(CM_EDITCLEAR, CeEditClear),
  EV_COMMAND_ENABLE(CM_EDITUNDO, CeEditUndo),
  EV_WM_CHAR,
  EV_WM_KEYDOWN,
  EV_WM_GETDLGCODE,
  EV_WM_SETFOCUS,
  EV_WM_KILLFOCUS,
END_RESPONSE_TABLE;

//
// constructor for a TRichEdit object
//
// by default, edit control has a border and its text is left-justified
//
// multiline edit control has horizontal vertical scroll bars
//
TRichEdit::TRichEdit(TWindow*        parent,
                     int             id,
                     LPCTSTR         text,
                     int x, int y, int w, int h,
                     LPCTSTR         fileName,
                     TModule*        module)
:
  TEditFile(parent, id, text, x, y, w, h, fileName, module)
{
  // Make sure the RichEdit DLL is available
  //
  if (!TRichEditDll::IsAvailable())
    TXCommCtrl::Raise();  //!BB Do we need a richedit-specific exception

  // Default to RTF data format
  //
  SetFormat(SF_RTF);
  // hide stuff that RichEdit doesn't support
  // Richedit support it now
  //GetSearchData().Flags |= FR_HIDEUPDOWN;

  // Undo the styles specific to "EDIT" controls; add richedit styles
  //
  ModifyStyle(ES_LOWERCASE|ES_PASSWORD|ES_OEMCONVERT|ES_UPPERCASE|ES_AUTOHSCROLL,
              ES_LEFT|WS_BORDER|WS_TABSTOP);
}

//
// constructor for TRichEdit associated with a MS-Windows interface element
// created by MS-Windows from a resource definition
//
// by default, data transfer is enabled
//
TRichEdit::TRichEdit(TWindow*   parent,
                     int        resourceId,
                     TModule*   module)
:
  TEditFile(parent, resourceId, module ? *module : *parent->GetModule())
{
  // Make sure the RichEdit DLL is available
  //
  if (!TRichEditDll::IsAvailable())
    TXCommCtrl::Raise();  //!BB Do we need a richedit-specific exception
}


//
// Retrieve the current character formatting in an edit control. If
// 'selection' parameter is 'true', the attribute of the current selection is
// retrieved. Otherwise, the default formatting attribute is retrieved.
//
ulong
TRichEdit::GetCharFormat(TCharFormat & cf, bool selection) const
{
  return CONST_CAST(TRichEdit*,this)->SendMessage(
                         EM_GETCHARFORMAT, selection ? SCF_SELECTION : 0,
                                                  TParam2(&cf));
}

//
// Retrieve the paragraph formatting of the current selection of the rich
// edit control.
//
// NOTE: If more than one paragraph is selected, the structure
//       receives the attributes of the first paragraph, and the dwMask
//       member specifies which attributes are consistent throughout the
//       entire selection.
//
ulong
TRichEdit::GetParaFormat(TParaFormat & pf) const
{
  return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETPARAFORMAT, 0,
                                                    TParam2(&pf));
}

//
// Set the character formatting of a rich edit control. The 'flags' parameter
// can be one of the following:
//
//    SCF_SELECTION  Applies the formatting to the current selection, or sets
//                   the default formatting if the selection is empty.
//    SCF_WORD       Applies the formatting to the selected word or words. If
//                   the selection is empty but the insertion point is inside
//                   a word, the formatting is applied to the word. This value
//                   must be used in conjunction with the SCF_SELECTION value.
//
bool
TRichEdit::SetCharFormat(const TCharFormat & cf, uint flags)
{
  return SendMessage(EM_SETCHARFORMAT, TParam1(flags), TParam2(&cf)) != 0;
}

//
// Set the paragraph formatting of the current selection of the rich edit
// control.
//
bool
TRichEdit::SetParaFormat(const TParaFormat & pf)
{
  return SendMessage(EM_SETPARAFORMAT, 0, TParam2(&pf)) != 0;
}

//
// Set the background color of the rich edit control.
// NOTE: If 'TColor::None' is specified, the color is set to the window
//       background system color.
//
TColor
TRichEdit::SetBkgndColor(const TColor& bkColor)
{
  TParam1 p1 = bkColor == TColor::None ? true : false;
  TParam2 p2 = bkColor == TColor::None ? 0    : (COLORREF)bkColor;
  return TColor((COLORREF)SendMessage(EM_SETBKGNDCOLOR, p1, p2));
}

//
// Return whether the current selection has a particular attribute.
// The 'mask' identifies the attribute we're interested in.
// The 'effects' contains the state of the attributes.
// The function returns one of the following values:
//
//    TFmtStatus::Yes           The attribute is enabled
//    TFmtStatus::No            The attribute is absent
//    TFmtStatus::Partly        The attribute is partly present
//
uint
TRichEdit::HasCharAttribute(ulong mask, uint32 effects)
{
  TCharFormat cf(*this);
  if (cf.dwMask & mask) {
    if (cf.dwEffects & effects)
      return Yes;
    else
      return No;
  }
  else
    return Partly;
}

//
// Toggle a set of character attributes. The 'mask' identifies
// the attributes we're interested in while 'effects' identifies the state
// of the attributes.
//
bool
TRichEdit::ToggleCharAttribute(ulong mask, uint32 effects)
{
  TCharFormat cf(*this);
  cf.dwMask = mask;
  cf.ToggleEffectsBit(effects);
  return SetCharFormat(cf);
}

//
// Increase or decrease (using a positive or negative value respectively) the
// point size of the current selection.
//
bool
TRichEdit::ChangeCharPointSize(int pointSizeDelta)
{
  TCharFormat cf(*this);
  cf.dwMask = CFM_SIZE;
  if (((cf.yHeight + 20*pointSizeDelta) <= (MaxPointSize*20)) &&
      ((cf.yHeight + 20*pointSizeDelta) >= (MinPointSize*6))) {
    cf.yHeight += 20*pointSizeDelta;
    return SetCharFormat(cf);
  }
  return false;
}

//
// Return true if the rich edit control has an active selection. Return false
// otherwise.
//
bool
TRichEdit::HasSelection() const
{
  TCharRange chRange;
  GetSelRange(chRange);
  return chRange.cpMin != chRange.cpMax;
}

//
// Retrieve the starting and ending character position of the selection
// in the rich edit control.
//
void
TRichEdit::GetSelection(uint& startPos, uint& endPos) const
{
  TCharRange cr;
  GetSelRange(cr);
  startPos = cr.cpMin;
  endPos = cr.cpMax;
}

//
// Retrieve the starting and ending character positions of the selection of
// the richedit control.
//
void
TRichEdit::GetSelRange(TCharRange & cr) const
{
  CONST_CAST(TRichEdit*,this)->SendMessage(EM_EXGETSEL, 0, TParam2(&cr));
}

//
// Select a range of characters in the rich edit control.
//
bool
TRichEdit::SetSelection(uint startPos, uint endPos)
{
  TCharRange cr(startPos, endPos);
  return SetSelRange(cr) >= 0;
}

//
// Select a range of characters in the rich edit control.
//
int
TRichEdit::SetSelRange(const TCharRange & cr)
{
  return (int)SendMessage(EM_EXSETSEL, 0, TParam2(&cr));
}

//
// Show or hide the selection in the rich edit control.
// The 'hide' parameter specifies whether to hide or show the selection. If the
// is 'false' the selection is shown. Otherwise, the selection is hidden.
// The 'changeStyle' parameter specifies whether to change the control's
// ES_NOHIDESEL window style. If this parameter is 'false', the selection is
// temporarily shown or hidden. Otherwise, the style is changed. If this
// parameter is 'true' and the control has the focus, the selection is hidden
// or shown as appropriate.
//
void
TRichEdit::HideSelection(bool hide, bool changeStyle)
{
  SendMessage(EM_HIDESELECTION, TParam1(hide), TParam2(changeStyle));
}

//
// Return the selection type of the rich edit control. Returns SEL_EMPTY if
// the selection is empty, or one or more of the following values:
//
//    Value                 Contents of the selection
//    -----                 ------------------------
//    SEL_TEXT              Text
//    SEL_OBJECT            At least one OLE object
//    SEL_MULTICHAR         More than one character of text
//    SEL_MULTIOBJECT       More than one OLE object
//
ulong
TRichEdit::GetSelectionType() const
{
  return CONST_CAST(TRichEdit*,this)->SendMessage(EM_SELECTIONTYPE);
}


//
// Return the lenght of the text in the richedit control
//   for RichEdit 2.0 uses EM_GETTEXTLENGTH
//
int
TRichEdit::GetTextLength() const
{
  if (TRichEditDll::Dll()->GetVersion() >= 2)
  {
    GETTEXTLENGTHEX gtl;
    gtl.flags = GTL_DEFAULT;

//Jogy ???  From MSDN: Code page used in the translation. It is CP_ACP for ANSI Code Page and 1200 for Unicode.
#if defined(UNICODE)
    gtl.codepage = 1200;
#else
    gtl.codepage = CP_ACP;
#endif

    return SendMessage(EM_GETTEXTLENGTHEX, TParam1(&gtl), TParam2(0));
  }
  else
  {
    return GetWindowTextLength();
  }
}


//
// Retrieve a specified range of text from the rich edit control
//
int
TRichEdit::GetTextRange(TTextRange & tr) const
{
  return (int)CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETTEXTRANGE, 0,
                                                         TParam2(&tr));
}

//
// Retrieve a specified range of text from the rich edit control
//
int
TRichEdit::GetTextRange(const TCharRange & cr, LPTSTR buffer) const
{
  TTextRange tr(cr, buffer);
  return GetTextRange(tr);
}

//
// Retrieve a specified range of text from the rich edit control
//
void
TRichEdit::GetSubText(LPTSTR str, uint startPos, uint endPos) const
{
  TTextRange tr(startPos, endPos, str);
  GetTextRange(tr);
}

//
// Retrieve the currently selected text of the rich edit control.
//
int
TRichEdit::GetSelectedText(LPTSTR buffer) const
{
  return (int)CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETSELTEXT, 0,
                                                         TParam2(buffer));
}

//
// Set an upper limit to the amount of text in the richedit control.
//
void
TRichEdit::LimitText(long max)
{
  SendMessage(EM_EXLIMITTEXT, 0, TParam2(max));
}

//
// Find text within the rich edit control. The 'flags' parameter can be a
// combination of the following values:
// i.   FT_MATCHCASE  - Perform a case sensitiv search
// ii.  FT_MATCHWORD  - Match whole words
//
int
TRichEdit::FindText(uint flags, const TFindText & ft)
{
  return (int)SendMessage(EM_FINDTEXT, TParam1(flags), TParam2(&ft));
}

//
// Find text within the rich edit control. The 'flags' parameter can be a
// combination of the following values:
// i.   FT_MATCHCASE  - Perform a case sensitiv search
// ii.  FT_MATCHWORD  - Match whole words
//
int
TRichEdit::FindText(uint flags, const TCharRange & cr, LPCTSTR text)
{
  TFindText ft(cr, text);
  return FindText(flags, ft);
}

//
// Search for the specified text in the rich edit control. If found, select
// the text and return the offset of the text. Otherwise, return -1.
//
// NOTE: If the 'startPos' is -1, it is assumed that the starting position is
// the end [or beginning, depending on the direction parameter, 'up'] of the
// current selection
//
int
TRichEdit::Search(uint startPos, LPCTSTR text, bool caseSensitive,
                  bool wholeWord, bool up)
{
  if (!text || !text[0])
    return -1;

  if (startPos == (uint)-1) {
    uint sBeg, sEnd;
    GetSelection(sBeg, sEnd);
    startPos = up ? sBeg : sEnd;
  }

  // The current docs. mention the FT_MATCHCASE and FT_WHOLEWORD flags which
  // are not defined currently. I suspect they meant the FR_xxxx flags (used
  // in CommDlg API).
  // Yes it is FR_MATCHCASE and FR_WHOLEWORD
  TFindText findText(startPos, -1, text);
  uint flags = (caseSensitive ? FR_MATCHCASE : 0) |
               (wholeWord ? FR_WHOLEWORD : 0) |
               (up ? 0 : FR_DOWN);
  int index = FindText(flags, findText);

  //
  // If we've got a match, select the text
  //
  if (index >= 0) {
    int len = ::_tcslen(text);
    SetSelection(index+len, index);
  }
  return index;
}

//
// Find the next work break before or after the specified character position,
// or retrieve information about the character at that position. The 'code'
// parameter can be one of the following:
//
//  WB_CLASSIFY       Returns the character class and word break flags of the
//                    character at the specified position.
//  WB_ISDELIMITER    Returns TRUE if the character at the specified position
//                    is a delimiter, or FALSE otherwise.
//  WB_LEFT           Finds the nearest character before the specified
//                    position that begins a word.
//  WB_LEFTBREAK      Finds the next word end before the specified position.
//  WB_MOVEWORDLEFT   Finds the next character that begins a word before the
//                    specified position. This value is used during CTRL+LEFT key processing.
//  WB_MOVEWORDRIGHT  Finds the next character that begins a word after the
//                    specified position. This value is used during
//                    CTRL+RIGHT key processing.
//  WB_RIGHT          Finds the next character that begins a word after the
//                    specified position.
//  WB_RIGHTBREAK     Finds the next end-of-word delimiter after the
//                    specified position.
//
// The return value is the character index of the word break, unless the
// 'code' parameter is WB_CLASSIFY or WB_ISDELIMETER
int
TRichEdit::FindWordBreak(uint code, long start)
{
  return (int)SendMessage(EM_FINDWORDBREAK, TParam1(code), TParam2(start));
}

//
// Determine which line contains the specified character in the richedit
// control.
// NOTE: The return value is zero-based.
//
int
TRichEdit::GetLineFromPos(uint charPos) const
{
  return (int)CONST_CAST(TRichEdit*,this)->SendMessage(EM_EXLINEFROMCHAR,
                                                         0, TParam2(charPos));
}


//
//   Enable/Disable Auto URL detection
// RichEdit 2.0 specific
bool
TRichEdit::EnableAutoURL(bool enable)
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return ToBool(SendMessage(EM_AUTOURLDETECT, TParam1(enable)));
  return false;
}

//
//
// RichEdit 2.0 specific
int
TRichEdit::GetIMEMode() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETIMECOMPMODE);
  return ICM_NOTOPEN;
}

//
//
// RichEdit 2.0 specific
void
TRichEdit::SetLangOptions(int options)
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    SendMessage(EM_SETLANGOPTIONS,0,TParam2(options));
}

//
//
// RichEdit 2.0 specific
int
TRichEdit::GetLangOptions() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETLANGOPTIONS);
  return 0;
}

//
//
// RichEdit 2.0 specific
void
TRichEdit::SetTextMode(int mode)
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    SendMessage(EM_SETTEXTMODE,TParam1(mode));

  if(mode & TM_PLAINTEXT)
    Format = SF_TEXT;
  else if(mode & TM_RICHTEXT)
    Format = SF_RTF;
}

//
//
// RichEdit 2.0 specific
int
TRichEdit::GetTextMode() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETTEXTMODE);

  if(IsRTF())
    return TM_RICHTEXT;
  else
    return TM_PLAINTEXT;
}

//
//
// RichEdit 2.0 specific
void
TRichEdit::SetUndoLimit(int maxnum)
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    SendMessage(EM_SETUNDOLIMIT,TParam1(maxnum));
}

//
//
// RichEdit 2.0 specific
void
TRichEdit::StopGroupTyping()
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    SendMessage(EM_STOPGROUPTYPING);
}

//
// Return true if the richedit can paste the specified clipboard format, or
// false otherwise.
//
bool
TRichEdit::CanPaste(uint format) const
{
  return ToBool(CONST_CAST(TRichEdit*,this)->SendMessage(EM_CANPASTE,TParam1(format)));
}

//
//  RichEdit 2.0 specific
//
bool
TRichEdit::CanRedo() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return ToBool(CONST_CAST(TRichEdit*,this)->SendMessage(EM_CANREDO));
  return CanUndo();
}

//
//  RichEdit 2.0 specific
// The value returned is an UNDONAMEID enumeration
int
TRichEdit::GetUndoName() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETUNDONAME);
  return 0;
}

//
//  RichEdit 2.0 specific
// The value returned is an UNDONAMEID enumeration
int
TRichEdit::GetRedoName() const
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETREDONAME);
  return 0;
}

//
//  RichEdit 2.0 specific
//
void
TRichEdit::Redo()
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    SendMessage(EM_REDO);
  else
    Undo();
}

//
// Paste the specified clipboard format in the rich edit control.
//
void
TRichEdit::PasteSpecial(uint format)
{
  SendMessage(EM_PASTESPECIAL, TParam1(format));
}

//
// Paste a compatible clipboard format in the rich edit control.
//
void
TRichEdit::Paste()
{
  SendMessage(WM_PASTE);
//!BB  //
//!BB  // Iterator through clipboard to locate 'pastable' format
//!BB  //
//!BB  TClipboard clip(*this);
//!BB  for (TClipboardFormatIterator iter(clip); iter; iter++) {
//!BB    if (CanPaste(iter.Current())) {
//!BB      PasteSpecial(iter.Current());
//!BB      return;
//!BB    }
//!BB  }

}

//
// Replace the contents of the rich edit control with the specified data
// stream. The 'format' parameter can be one of the following data formats,
// optionally combined with the SFF_SELECTION flag:
//
//    Value               Meaning
//    -----               -------
//    SF_TEXT             Text
//    SF_RTF              Rich-text format
//
// If the SFF_SELECTION flag is specified, the stream replaces the contents of
// the current selection. Otherwise, the stream replaces the entire contents
// of the control.
//
ulong
TRichEdit::StreamIn(uint format, TEditStream & es)
{
  return SendMessage(EM_STREAMIN, TParam1(format), TParam2(&es));
}

//
// Write the contents of the rich edit control to the specified data stream.
// The 'format' parameter can be one of the following values, optionally
// combined with the SFF_SELECTION flag:
//
//    Value               Meaning
//    -----               -------
//    SF_TEXT             Text with spaces in place of OLE objects
//    SF_RTF              Rich-text format (RTF)
//    SF_RTFNOOBJS        RTF with spaces in place of OLE object.
//    SF_TEXTIZED         Text with a text representation of OLE objects.
//
// NOTE: The SF_RTFNOOBJS option is useful if an application stores OLE
//       objects itself, as RTF representation of OLE objects is not very
//       compact.
//       If the SFF_SELECTION flag is specified, only the contents of the
//       current selection are streamed out. Otherwise, the entire contents of
//       the control are streamed out.
//
ulong
TRichEdit::StreamOut(uint format, TEditStream & es)
{
  return SendMessage(EM_STREAMOUT, TParam1(format), TParam2(&es));
}

//
// Display a portion of the richedit control's content within the specified
// rectangle.
// NOTE: The content of the control must first be formatted via a call to the
//       'FormatRange' method.
//
bool
TRichEdit::DisplayBand(TRect & rc)
{
  return SendMessage(EM_DISPLAYBAND, 0, TParam2(&rc)) != 0;
}

//
// Formats a range of text (specified via the 'chrg' member of the
// specified TFormatRange) for the device(s) specified via the 'hdcTarget'
// and 'hdc' members of the TFormatRange structure.
//
int
TRichEdit::FormatRange(const TFormatRange & fr, bool render)
{
  return (int)SendMessage(EM_FORMATRANGE, TParam1(render), TParam2(&fr));
}

//
// Frees the Formatting information cached by the RichEdit control...
//
int
TRichEdit::FormatRange()
{
  return (int)SendMessage(EM_FORMATRANGE, TParam1(TRUE), 0);
}

//
// Set the target device and line width used for WYSIWYG (what you see is
// what you get) formatting of the rich edit control.
//
bool
TRichEdit::SetTargetDevice(HDC dc, int lineWidth)
{
  return SendMessage(EM_SETTARGETDEVICE, TParam1(dc),
                       TParam2(lineWidth)) != 0;
}

//
// Force the rich edit control to send an EN_REQUESTRESIZE notification
// message to its parent window.
//
// NOTE: This message is useful during WM_SIZE processing for the parent of a
//       bottomless rich edit control.
//
void
TRichEdit::RequestResize()
{
  SendMessage(EM_REQUESTRESIZE);
}

#if !defined(BI_SUPPRESS_OLE)
//
// Retrieve an IRichEditOle object that a client can use to access a rich edit
// control's OLE functionality. Returns 'true' if successful, or false
// otherwise.
//
bool
TRichEdit::GetOleInterface(IRichEditOle * &pInterface) const
{
  return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETOLEINTERFACE,
                                0, TParam2((void * *)&pInterface)) != 0;
}

//
// Set an IRichEditOleCallback object that the rich edit control uses to get
// OLE-related resources and information from the client. Returns 'true' if
// successful, or false otherwise.
//
bool
TRichEdit::SetOleInterface(IRichEditOleCallback * pInterface)
{
#if 0
  // EM_SETOLEINTERFACE: Documented but missing...
  return SendMessage(EM_SETOLEINTERFACE, 0, TParam2(pInterface)) != 0;
#else
  // Y.B 06/16/98 MFC uses EM_SETOLECALLBACK not EM_SETOLEINTERFACE ?????
  // afxxmn.inl line 620. func CRichEditCtrl::SetOLECallback(...)
  // #define EM_SETOLECALLBACK    (WM_USER + 70)  // not documented !!!!!!
  return SendMessage(EM_SETOLECALLBACK, 0, TParam2(pInterface)) != 0;
#endif
}
#endif

//
// Retrieve the event mask for the rich edit control. The event mask specifies
// which notification messages the control sends to its parent window.
//
ulong
TRichEdit::GetEventMask() const
{
  return CONST_CAST(TRichEdit*,this)->SendMessage(EM_GETEVENTMASK);
}

//
// Set the event mask for a rich edit control. The event mask specifies which
// notification messages the control sends to its parent window. The 'msk'
// parameter can be zero or more of the following values:
//
//  Value               Meaning
//  -----               -------
//  ENM_CHANGE          Sends EN_CHANGE notifications.
//  ENM_CORRECTTEXT     Sends EN_CORRECTTEXT notifications.
//  ENM_DROPFILES       Sends EN_DROPFILES notifications.
//  ENM_KEYEVENTS       Sends EN_MSGFILTER  notifications for keyboard events.
//  ENM_MOUSEEVENTS     Sends EN_MSGFILTER notifications for mouse events.
//  ENM_PROTECTED       Sends EN_PROTECTED notifications.
//  ENM_RESIZEREQUEST   Sends EN_REQUESTRESIZE notifications.
//  ENM_SCROLL          Sends EN_HSCROLL notifications.
//  ENM_SELCHANGE       Sends EN_SELCHANGE notifications.
//  ENM_UPDATE          Sends EN_UPDATE notifications
//
ulong
TRichEdit::SetEventMask(ulong msk)
{
  return SendMessage(EM_SETEVENTMASK, 0, TParam2(msk));
}

//
// WM_GETDLGCODE handler to bypass TEdit's handler which caters for
// validators.
//
uint
TRichEdit::EvGetDlgCode(MSG * msg)
{
  return TWindow::EvGetDlgCode(msg);
}

//
// WM_CHAR handler to bypass TEdit's handler which caters for
// validators.
//
void
TRichEdit::EvChar(uint /*key*/, uint /*repeatCount*/, uint /*flags*/)
{
  DefaultProcessing();
}

//
// WM_KEYDOWN handler to bypass TEdit's handler which caters for
// validators.
//
void
TRichEdit::EvKeyDown(uint /*key*/, uint /*repeatCount*/, uint /*flags*/)
{
  DefaultProcessing();
}

//
// WM_KILLFOCUS handler to bypass TEdit's handler which caters for
// validators.
//
void
TRichEdit::EvKillFocus(THandle hGetFocus)
{
  TControl::EvKillFocus(hGetFocus);
}

//
// WM_SETFOCUS handler to bypass TEdit's handler which caters for
// validators.
//
void
TRichEdit::EvSetFocus(THandle /*hLostFocus*/)
{
  DefaultProcessing();
}

//
// This function is called for Cut/Copy/Delete menu items to determine
// whether or not the item is enabled.
//
void
TRichEdit::CeHasSelect(TCommandEnabler& commandHandler)
{
  uint sPos, ePos;

  GetSelection(sPos, ePos);
  commandHandler.Enable(sPos != ePos);
}

//
// This function is called for the Paste menu item to determine whether or
// not the item is enabled.
//
void
TRichEdit::CeEditPaste(TCommandEnabler& ce)
{
/*
  return (CountClipboardFormats() != 0) &&
    (IsClipboardFormatAvailable(CF_TEXT) ||
    IsClipboardFormatAvailable(_oleData.cfRichTextFormat) ||
    IsClipboardFormatAvailable(_oleData.cfEmbedSource) ||
    IsClipboardFormatAvailable(_oleData.cfEmbeddedObject) ||
    IsClipboardFormatAvailable(_oleData.cfFileName) ||
    IsClipboardFormatAvailable(_oleData.cfFileNameW) ||
    IsClipboardFormatAvailable(CF_METAFILEPICT) ||
    IsClipboardFormatAvailable(CF_DIB) ||
    IsClipboardFormatAvailable(CF_BITMAP) ||
    GetRichEditCtrl().CanPaste());
*/
  TClipboard clip(*this, false);
  if (clip &&
      (clip.CountClipboardFormats()!=0) &&
      (  clip.IsClipboardFormatAvailable(CF_TEXT) ||
        clip.IsClipboardFormatAvailable(CF_OEMTEXT) ||
        clip.IsClipboardFormatAvailable(CF_UNICODETEXT)
//        || clip.IsClipboardFormatAvailable(CF_METAFILEPICT) ||  //?? check
//        clip.IsClipboardFormatAvailable(CF_ENHMETAFILE) ||   //??
//        clip.IsClipboardFormatAvailable(CF_DIB)              //???
      )
      ){
    ce.Enable(true);
  }
  else
    ce.Enable(false);
}

//
// This function is called for the Clear menu item to determine whether or
// not the item is enabled.
//
void
TRichEdit::CeEditClear(TCommandEnabler& commandHandler)
{
  commandHandler.Enable(!(GetNumLines() == 1 && GetLineLength(0) == 0));
}

//
// This function is called for the Undo menu item to determine whether or
// not the item is enabled.
//
void
TRichEdit::CeEditUndo(TCommandEnabler& commandHandler)
{
  commandHandler.Enable(CanUndo());
}

//
// Return name of predefined Windows edit class
//
LPCTSTR
TRichEdit::GetClassName()
{
  if(TRichEditDll::Dll()->GetVersion() >= 2)
    return RICHEDIT_CLASS;
  else
    return _T("RICHEDIT");
}

//
// Updates the list of filters describing files which can be opened by
// the rich edit control.
//
//
void
TRichEdit::SetupWindow()
{
  TEditFile::SetupWindow();

  //Load RichEdit's filter
  //
  FileData.SetFilter(GetModule()->LoadString(IDS_RICHEDITFILTER).c_str());
}


inline uint32
TRichEdit::CharFromPos(int16 x, int16 y)
{
  PRECONDITION(TWindow::GetHandle());
  POINTL pt;
  pt.x = x;
  pt.y = y;

  return (uint32)SendMessage(EM_CHARFROMPOS, 0, TParam2(&pt));
}

//
inline uint32
TRichEdit::PosFromChar(uint charIndex)
{
  PRECONDITION(TWindow::GetHandle());

  if (TRichEditDll::Dll()->GetVersion() == 2)
	  return (uint32)SendMessage(EM_POSFROMCHAR, charIndex);
  else
  {
    POINTL pt;
    SendMessage(EM_POSFROMCHAR, (TParam1)&pt, charIndex);
    return MkUint32((uint16)pt.y, (uint16)pt.x);
  }
}


//
// Callback used when reading data from a stream into a rich edit control.
//
DWORD CALLBACK
RichEditStrmInWithIStream(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  PRECONDITION(dwCookie);

  _tistream& is = *((_tistream*)dwCookie);

  // Return 0 if transfer is complete
  //
  if (is.eof()) {
   *pcb = 0;
    return 0;
  }

  // Read data in buffer
  //
#if defined(UNICODE)
  _TCHAR * tempBuff = new _TCHAR[cb];
  is.read((_TCHAR*)tempBuff, cb);
  ::WideCharToMultiByte(CP_ACP, 0, tempBuff, is.gcount(), (char*)pbBuff, cb, NULL, NULL);
  delete[] tempBuff;
#else
  is.read((char*)pbBuff, cb); 
#endif

  // Indicate amount of data read
  //
  *pcb = is.gcount();

  // Docs says to return the amount of bytes read in but that does not seem
  // to be correct!!! Returning 0, on the other side, seems to work - so
  // stick with it until...
  //
  return 0;
}

//
// Callback used when writing out the contents of a rich edit control to
// a data stream.
//
DWORD CALLBACK
RichEditStrmOutWithOstream(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  PRECONDITION(dwCookie);

  _tostream& os = *((_tostream*)dwCookie);

  // Save current stream location and write data to buffer
  //
  streampos pCnt = os.tellp();
  
#if defined(UNICODE)
  _TCHAR * tempBuff = new _TCHAR[cb];
  ::MultiByteToWideChar(CP_ACP, 0, (char*)pbBuff, cb, tempBuff, cb);  
  os.write(tempBuff, cb);
  delete[] tempBuff;
#else
  os.write((char*)pbBuff, cb);
#endif 

  // Indicate the number of bytes written to the file
  //
  *pcb = os.tellp() - pCnt;
  return 0;
}

//
// Overriden to bypass TEdit's 'Transfer' method.
// NOTE: There's no transfer-support for rich edit controls.
//
uint
TRichEdit::Transfer(void* /*buffer*/, TTransferDirection /*direction*/)
{
  // NOTE: No transfer support for rich edit control
  //
  return 0;
}

//
// Read the data from the specified stream into the rich edit control. The
// 'fmt' parameter can be one of the following data formats, optionally
// combined with the SFF_SELECTION flag:
//
//    Value               Meaning
//    -----               -------
//    SF_TEXT             Text
//    SF_RTF              Rich-text format
//
// If the SFF_SELECTION flag is specified, the stream replaces the contents of
// the current selection. Otherwise, the stream replaces the entire contents
// of the control.
//
bool
TRichEdit::ReadFromStream( _tistream& is, uint fmt)
{
  TEditStream edStrm(DWORD(( _tistream*)&is), RichEditStrmInWithIStream);
  StreamIn(fmt, edStrm);
  return edStrm.dwError == 0;
}

//
// Write the contents of the rich edit control to the specified data stream.
// The 'fmt' parameter can be one of the following values, optionally
// combined with the SFF_SELECTION flag:
//
//    Value               Meaning
//    -----               -------
//    SF_TEXT             Text with spaces in place of OLE objects
//    SF_RTF              Rich-text format (RTF)
//    SF_RTFNOOBJS        RTF with spaces in place of OLE object.
//    SF_TEXTIZED         Text with a text representation of OLE objects.
//
// NOTE: The SF_RTFNOOBJS option is useful if an application stores OLE
//       objects itself, as RTF representation of OLE objects is not very
//       compact.
//       If the SFF_SELECTION flag is specified, only the contents of the
//       current selection are streamed out. Otherwise, the entire contents of
//       the control are streamed out.
//
bool
TRichEdit::WriteToStream( _tostream& os, uint fmt)
{
  TEditStream edStrm(DWORD(( _tostream*)&os), RichEditStrmOutWithOstream);
  StreamOut(fmt, edStrm);
  return edStrm.dwError == 0;
}

//
// Read the contents of the specified file in the rich edit control. Returns
// 'true' if successful, or false otherwise.
//
bool
TRichEdit::Read(LPCTSTR fileName)
{
  if (!fileName)
    if (FileName)
      fileName = FileName;
    else
      return false;
  
  _USES_CONVERSION;
  _tifstream ifs(_W2A(fileName), ios::in|ios::binary);
  if (ifs) {
    // Could check for a valid file (eg. FileSize != 0)
    // before proceeding with a call to Clear() here.
    //
    Clear();

    // Stream in data from file
    //
    if (ReadFromStream(ifs, Format)) {
      ClearModify();
      return true;
    }
  }
  return false;
}

//
// Write the contents of the edit control to the specified file. Returns
// 'true' if successful, or false otherwise.
//
bool
TRichEdit::Write(LPCTSTR fileName)
{
  if (!fileName)
    if (FileName)
      fileName = FileName;
    else
      return false;

  _USES_CONVERSION;
  _tofstream ofs(_W2A(fileName), ios::out|ios::binary);
  if (ofs) {
    if (WriteToStream(ofs, Format)) {
      ClearModify();
      return true;
    }
  }
  return false;
}


IMPLEMENT_STREAMABLE1(TRichEdit, TEditFile);

#if !defined(BI_NO_OBJ_STREAMING)
//
//
void*
TRichEdit::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TEditFile*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TRichEdit::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TEditFile*)GetObject(), os);
}

#endif //BI_NO_OBJ_STREAMING
//
// Object wrapper which loads the RichEdit DLL
//
TRichEditModule::TRichEditModule()
:
  TModule(GetVersion() >= 2 ? RichEdit20DllName : RichEditDllName, true, true,false)
{
}

//
// check new rich edit library
//
static int CheckREVersion()
{
  TErrorMode loadMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
  HINSTANCE Handle = ::LoadLibrary(RichEdit20DllName);
  if (Handle <= HINSTANCE(HINSTANCE_ERROR))
    return 1;
  ::FreeLibrary(Handle);
  return 2;
}

//
//
//
int TRichEditModule::GetVersion(bool force_old)
{
  static int REVersion = force_old ? 1 : CheckREVersion();
  return REVersion;
}

#if defined(BI_IMPEXP_TEMPLATES)
# if defined(_BUILDOWLDLL)
// The template instances only need to be generated when building the
// ObjectWindows DLL - These instances are exported by OWL and imported
// by user code.

  //
  // Export template of TDllLoader<TRichEditModule> when building ObjectWindows
  // DLL and provide import declaration of DLL instance for users of the class.
  //
  template class _OWLCLASS TDllLoader<TRichEditModule>;
# endif
#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

