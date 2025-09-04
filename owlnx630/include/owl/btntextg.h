//----------------------------------------------------------------------------//
// ObjectWindows 1998 Copyright by Yura Bidus                                 //
//                                                                            //
// Used code and ideas from Dieter Windau and Joseph Parrello                 //
//                                                                            //
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:12 $
//                                                                            //
// THIS CLASS AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF         //
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO        //
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A             //
// PARTICULAR PURPOSE.                                                        //
//                                                                            //
// Definition of classes TButtonTextGadget.                                   //
//----------------------------------------------------------------------------//

#if !defined(OWL_BTNTEXTG_H) // Sentry, use file only if it's not already included.
#define OWL_BTNTEXTG_H

#if !defined(OWL_BUTTONGA_H)
# include <owl/buttonga.h>
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
//
#include <owl/preclass.h>

//
// class TButtonTextGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~
class _OWLCLASS TButtonTextGadget : public TButtonGadget{
  public:
    enum TAlign {aLeft, aCenter, aRight};
    enum TStyle {sBitmap=0x001, sText=0x002, sBitmapText=0x003};
    enum TLayoutStyle {lTextLeft,lTextTop,lTextRight,lTextBottom};

    TButtonTextGadget(int      id,
                      TResId   glyphResIdOrIndex,
                      TStyle   style = sBitmapText,
                      TType    type = Command,
                      bool    enabled = false,
                      TState   state = Up,
                      bool    sharedGlyph = false,
                      uint     numChars = 4);

    virtual ~TButtonTextGadget();

    LPCTSTR   GetText() const;
    void      SetText(LPCTSTR text, bool repaint=true);
    TStyle     GetStyle() const;
    void      SetStyle(const TStyle style, bool   repaint=true);
    TAlign    GetAlign() const;
    void      SetAlign(const TAlign align, bool repaint=true);
    TLayoutStyle GetLayoutStyle() const;
    void      SetLayoutStyle(const TLayoutStyle style, bool repaint=true);

    TFont*     GetFont()  const;
    void      SetFont(TFont* font, bool repaint = true);

    // Override virtual methods defined in TGadget
    //
    virtual void  GetDesiredSize(TSize &size);
    virtual void   SetBounds(const TRect& rect);

    // Override and initiate a WM_COMMAND_ENABLE message
    //
    virtual void  CommandEnable();

  protected:
    // Override Paint
    virtual void  Paint(TDC& dc);
    virtual void   Created();
    virtual void  Layout(TRect& srcRect, TRect& textRect, TRect& btnRect);
    virtual void   PaintText(TDC& dc, TRect& rect, LPCTSTR text);
    virtual void  SysColorChange();

    void GetTextSize(TSize& size);

  // Data members -- will become private
  //
  protected_data:
    LPTSTR        Text;       // new'd copy of the text for this gadget
    uint          TextLen;    // strlen of the above text
    uint          NumChars;   // Number of chars to reserve space for
    TAlign        Align;      // Alignment: left, center or right
    TStyle        Style;      // Style Bitmap, Text, Bitmap and Text
    TLayoutStyle  LayoutStyle;// Layout style
    TFont*         Font;       // display font

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TButtonTextGadget(const TButtonTextGadget&);
    TButtonTextGadget& operator =(const TButtonTextGadget&);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// --------------------------------------------------------------------------
// Inline implementation
//

inline TButtonTextGadget::TStyle TButtonTextGadget::GetStyle() const{
  return Style;
}

//
inline LPCTSTR TButtonTextGadget::GetText() const {
  return Text;
}

//
inline TFont* TButtonTextGadget::GetFont() const {
  return Font;
}

//
inline TButtonTextGadget::TAlign TButtonTextGadget::GetAlign() const {
  return Align;
}

//
inline TButtonTextGadget::TLayoutStyle TButtonTextGadget::GetLayoutStyle() const{
  return LayoutStyle;
}


__OWL_END_NAMESPACE

#endif  // OWL_BTNTEXTG_H
