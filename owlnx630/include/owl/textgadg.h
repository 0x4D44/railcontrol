//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//
// Definition of text gadget class TGadget.
// Definition for TDynamicTextGadget.
//----------------------------------------------------------------------------

#if !defined(OWL_TEXTGADG_H)
#define OWL_TEXTGADG_H

#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
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

//
// class TTextGadget
// ~~~~~ ~~~~~~~~~~~
// When constructing the text gadget specify how many characters you want
// room for and how the text should be aligned horizontally.
//
// The inner bounds are computed by multiplying the number of characters by
// the maximum character width.
//
class _OWLCLASS TTextGadget : public TGadget {
  public:
    enum TAlign {Left, Center, Right};

    TTextGadget(int id = 0, TBorderStyle = Recessed, TAlign = Left,
                uint numChars = 10, LPCTSTR text = 0,
                TFont* font = 0 /*new TGadgetWindowFont*/);
   ~TTextGadget();

    // Return a copy of gadget's text
    //
    LPCTSTR   GetText() const;

    // Makes a copy of the text
    //
    void      SetText(LPCTSTR text);

    // Added 6/22/98 to allow setting/unsetting of text/background color
    void SetBkgndColor(TColor& color);
    void SetTextColor(TColor& color);
    void ResetBkgndColor();
    void ResetTextColor();

  protected:
    // Override virtual methods defined in TGadget
    //
    void            Paint(TDC& dc);
    virtual TColor   GetEnabledColor() const;
    void            GetDesiredSize(TSize &size);
    void            Invalidate();

  // Data members -- will become private
  //
  protected_data:
    LPTSTR    Text;       // new'd copy of the text for this gadget
    uint      TextLen;    // strlen of the above text
    TAlign    Align;      // Alignment: left, center or right
    uint      NumChars;   // Number of chars to reserve space for
    TFont*    Font;       // display font
    TColor    BkgndColor; // background color of the gadget
    TColor    TextColor;  // text color of the gadget

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TTextGadget(const TTextGadget&);
    TTextGadget& operator =(const TTextGadget&);
};

//
// class TDynamicTextGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// Small class will send EvEnable to window, 
// You can set text by handling EvCommandEnable message
//
class _OWLCLASS TDynamicTextGadget: public TTextGadget{
  public:
    TDynamicTextGadget(int id = 0, 
                       TBorderStyle = Recessed, 
                       TAlign = Left,
                       uint numChars = 10, 
                       LPCTSTR text = 0,
                       TFont* font = 0);
    public:
      virtual void CommandEnable();
}; 

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


// -----------------------------------------------------------------------------
// Inline implementation
//

//
inline LPCTSTR TTextGadget::GetText() const {
  return Text;
}

//
inline TColor TTextGadget::GetEnabledColor() const {
  return TColor::SysBtnText;
}

//------------------------------------------------------------------------------

__OWL_END_NAMESPACE


#endif  // OWL_TEXTGADG_H
