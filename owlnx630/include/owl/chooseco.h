//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-07-09 07:50:15 $
//
// Definition of Choose Color Common Dialog class
//----------------------------------------------------------------------------

#if !defined(OWL_CHOOSECO_H)
#define OWL_CHOOSECO_H

#if !defined(OWL_COMMDIAL_H)
# include <owl/commdial.h>
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


//
// class TChooseColorDialog
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// Wrapper for the Choose-Color common dialog.
//
class _OWLCLASS TChooseColorDialog : public TCommonDialog {
  public:
    class _OWLCLASS TData {
      public:
        TData(uint32 flags = 0, bool allocCustColor = false);
        TData(uint32 flags, const TColor& initColor, 
              bool allocCustColors = false);

        uint32              Flags;
        uint32              Error;
        TColor              Color;
        TColor*             CustColors;

      protected:
        TAPointer<TColor>   CustColorsArray;  
    };

    TChooseColorDialog(TWindow* parent,
                       TData&   data,
                       TResId   templateId = 0,
                       LPCTSTR   title = 0,
                       TModule* module = 0);
   ~TChooseColorDialog();

    // Set the current RGB color in this dialog
    //
    void    SetRGBColor(const TColor& color);

  protected:
    TData&  GetData();
    void    SetData(TData& data);

    CHOOSECOLOR&  GetCC();
    void   SetCC(const CHOOSECOLOR& cc);

    int     DoExecute();
    bool    DialogFunction(uint message, TParam1, TParam2);

    // Default behavior inline for message response functions
    //
    //!TH Is the return type really a TParam2?
    TParam2  EvSetRGBColor(TParam1, TParam2);      // EV_REGISTERED(SETRGBSTRING,

    // Registered messages this class sends (to itself)
    //
    static uint SetRGBMsgId;

  protected_data:
    union {
      CHOOSECOLOR  Cc;  // New name
      CHOOSECOLOR  cc;  // Old name
    };
    TData&       Data;

  private:
    TChooseColorDialog(const TChooseColorDialog&);
    TChooseColorDialog& operator=(const TChooseColorDialog&);

  DECLARE_RESPONSE_TABLE(TChooseColorDialog);
  DECLARE_CASTABLE;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
// Set the current selection to a specific RGB color.
//
inline void TChooseColorDialog::SetRGBColor(const TColor& color) {
  SendMessage(SetRGBMsgId, 0, color);
}

//
// Return the data object for this common dialog.
//
inline TChooseColorDialog::TData& TChooseColorDialog::GetData() {
  return Data;
}

//
// Set the data for this common dialog.
// Use this function with caution!
//
inline void TChooseColorDialog::SetData(TData& data) {
  Data = data;
}

//
// Return the CHOOSECOLOR data structure for this dialog.
//
inline CHOOSECOLOR& TChooseColorDialog::GetCC() {
  return Cc;
}

//
// Set the CHOOSECOLOR data structure for this dialog.
// Use this function with caution!
//
inline void TChooseColorDialog::SetCC(const CHOOSECOLOR& cc) {
  Cc = cc;
}

//
// The user has clicked on a color.
//
inline TParam2 TChooseColorDialog::EvSetRGBColor(TParam1, TParam2) {
  return DefaultProcessing();
}

//
//
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TChooseColorDialog::TData::TData(uint32 flags, bool allocCustColor) {
//Jogy???  Flags = 0;
  Flags = flags;
  if (allocCustColor) 
  {
    CustColorsArray = new TColor[16];
    CustColors = CustColorsArray;
  }
  else
  {
    CustColors = 0;
  }
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
//
//
inline TChooseColorDialog::TData::TData(uint32 flags, 
                                        const TColor& initColor, 
                                        bool allocCustColor) {
  Flags = flags;
  Color = initColor;  
  if (allocCustColor) 
  {
    CustColorsArray = new TColor[16];
    CustColors = CustColorsArray;
  }
  else
  {
    CustColors = 0;
  }
}


__OWL_END_NAMESPACE

#endif  // OWL_CHOOSECO_H
