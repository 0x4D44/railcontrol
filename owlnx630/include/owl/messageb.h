//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Definition of class TMessageBar.
//----------------------------------------------------------------------------

#if !defined(OWL_MESSAGEB_H)
#define OWL_MESSAGEB_H

#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
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

const int IDW_STATUSBAR     = 32040;  // Window ID used to locate status bars

//
// class TMessageBar
// ~~~~~ ~~~~~~~~~~~
// Implements a message bar with one text gadget as wide as the window and no
// border. positions itself at the bottom of the window and uses the default
// decoration bar font
//
// The message bar optionally draws a highlight line at the top
//
class _OWLCLASS TMessageBar : public TGadgetWindow {
  public:
    TMessageBar(TWindow*   parent = 0,
                TFont*     font = new TGadgetWindowFont,
                TModule*   module = 0);
   ~TMessageBar();

    // Set the text for the default text message gadget or a text gadget by id
    //
    void    SetText(LPCTSTR text);
    void    SetMessageText(int id, LPCTSTR text);

    // Set (or clear if 0) menu/command item hint text displayed in/on bar
    //
    virtual void    SetHintText(LPCTSTR text);

  protected:
    // Compensate for highlight line
    //
    void   GetInnerRect(TRect& rect);
    void   GetDesiredSize(TSize& rect);

    // Draw highlight line, then HintText if any, or gadgets
    //
    void   PaintGadgets(TDC& dc, bool erase, TRect& rect);

    bool   GetHighlightLine() const;
    void   SetHighlightLine(bool highlightline);

    LPCTSTR  GetHintText() const;

  protected_data:
    bool   HighlightLine;  // does this bar have a upper highlight line?
    TCHAR* HintText;       // current command hint being displayed, if any

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TMessageBar(const TMessageBar&);
    TMessageBar& operator =(const TMessageBar&);

  DECLARE_CASTABLE;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
// Returns true if the message bar has an upper highlight line.
//
inline bool TMessageBar::GetHighlightLine() const {
  return HighlightLine;
}

//
// Sets the flag for the message bar to have an upper highlight line.
//
inline void TMessageBar::SetHighlightLine(bool highlightline) {
  HighlightLine = highlightline;
}

//
// Return the cached hint text for the current message.
//
inline LPCTSTR TMessageBar::GetHintText() const {
  return HintText;
}


__OWL_END_NAMESPACE


#endif  // OWL_MESSAGEB_H
