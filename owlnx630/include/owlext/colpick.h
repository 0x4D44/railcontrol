// ****************************************************************************
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// colpick.h:    header file
// Version:      1.6
// Date:         08.11.1998
// Author:       Dieter Windau
//
// Freeware OWL classes that extents the dockable and gadget system
//
// TColorPicker is based on the JPColorPicker class written by Jo Parrello
//
// You are free to use/modify this code but leave this header intact.
// May not be sold for profit.
//
// Tested with Borland C++ 5.02, OWL 5.02, OWL6 patch #3 and with Windows
// NT 4.0 SP3 but I think the class should work with Windows 95 too.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk.
//
// This package contains many classes and source that are based on other OWL
// developers work. Very special thanks to Alan Chambers, Christopher Kohlhoff,
// Jo Parrello, Mark Hatsell, Michael Mogensen and Yura Bidus
//
// Please send me bug reports, bug fixes, enhancements, requests, etc., and
// I'll try to keep this in next versions:
//   EMail: dieter.windau@usa.net
//   Web:   http://members.aol.com/softengage/index.htm
// ****************************************************************************
#ifndef __OWLEXT_COLPICK_H
#define __OWLEXT_COLPICK_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
  #include <owlext/core.h>
#endif

#include <owlext/colpick.rh>

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// ****************** Forward declaration *************************************
class OWLEXTCLASS TColorPickerData;
class OWLEXTCLASS TColorPicker;

// ******************** TColorPickerData **************************************

class OWLEXTCLASS TColorPickerData {
  public:
    static COLORREF ColorTable8[8];
    static COLORREF ColorTable16[16];
    static COLORREF ColorTable40[40];

  public:
    __OWL TColor*	Colors;
    uint		NumColors;
    uint		NumColumn;
    bool		UsePalette;
    bool		AutoParentNotify;
    bool		ShowDefaultField;
    uint		DefaultFieldResId; // string resid for the field
                                  // +1 for statusbar message
                                  // +2 for tooltip (if not present use +0)
		COLORREF DefaultFieldColor; // need if ShowDefaultField true

    bool		ShowCustomField;
    uint		CustomFieldResId;  // string resid for the field
                                  // +1 for statusbar message
                                  // +2 for tooltip (if not present use +0)
  public:
    TColorPickerData(
      COLORREF colors[],
      uint     numColors,
      uint     numColumn = 8,
      bool     usePalette = false,
		  bool     autoParentNotify = false,
      bool     showDefaultField = true,
      uint     defaultFieldResId = IDS_AUTOMATIC,
      COLORREF defaultFieldColor = 0xFF000000L,  // Color is black, first 0xFF
                                                 // identifies user defined
      bool     showCustomField = true,
      uint     customFieldResId = IDS_MORECOLORS);

    TColorPickerData(const TColorPickerData& data)
      { *this = data; }

    ~TColorPickerData();

    TColorPickerData& operator =(const TColorPickerData& d);

	  bool operator ==(const TColorPickerData& d) const;
	  bool operator !=(const TColorPickerData& d) const;
};

// ******************** TColorPicker ******************************************

#define TColorPickerMessage _T("TCOLORPICKERMESSAGE")

const int NumCustColors = 16;
const int NumMapColors  = 43; // Num of colors where a resorce Id is mapped
const int BoxSize       = 18; // Size in pixel of a color rectangle
const int DefaultHeight = 22; // Height of the first field
const int DistToColor   = 4;  // Distance in pixel between first field and colors
const int DistToCustom  = 8;  // Distance in pixel between colors and custom field
const int CustomHeight  = 20; // Height of the custom field

typedef struct {
  COLORREF Color;
  uint     ResId;
} ColorTableEntry;

class OWLEXTCLASS TColorPicker : public __OWL TWindow {
  public:
    TColorPicker(__OWL TWindow* parent,
                 const TColorPickerData& data,
                 __OWL TColor   startColor,
                 int      refId,
                 LPCTSTR  title = 0,
                 __OWL TModule* module = 0);
    virtual ~TColorPicker();

  public:
#if (OWLInternalVersion >= 0x06000000L)
    virtual LPCTSTR GetClassName();
#else
    virtual char * GetClassName();
#endif

    virtual void GetWindowClass(WNDCLASS& wndClass);
    virtual void Paint(__OWL TDC& dc, bool erase, __OWL TRect& rect);
    virtual void SetupWindow();
    virtual void ShowPickerWindow(__OWL TPoint& pt, __OWL TRect& rect);
    virtual bool PreProcessMsg(MSG& msg);
    virtual __OWL TColor GetCurrentChosenColor();
    virtual void SetCurrentChosenColor(__OWL TColor color);
    virtual void NotifyAtParent();

  public:
    static __OWL TColor GetCustomColorsValue(int index);
    static void SetCustomColorsValue(int index, __OWL TColor color);

  protected:
    void   ExitPicker();
    void   SetStatusBarMessage(int id);
    void   PaintBorder(__OWL TDC& dc, int index);
    int    GetIndexFromPoint(__OWL TPoint point);
    int    GetIndexByColor(__OWL TColor color);
    int    GetFlatState(int index);
    void   GetCellRect(int index, __OWL TRect& rect);
    void   DrawCell(__OWL TDC& dc, int index);
    void   ChosenColorChanged(int index);
    void   ColorSelectionChanged(int index);
    void   EvActivateApp(bool active, HTASK hTask);
    void   EvKillFocus(THandle hWndGetFocus );
    void   EvMouseMove(uint modKeys, __OWL TPoint& point);
    void   EvLButtonDown(uint modKeys, __OWL TPoint& point);
    void   EvPaletteChanged(THandle hWndPalChg);
    bool   EvQueryNewPalette();
    void   EvKeyDown(uint key, uint repeatCount, uint flags);

  protected:
    static COLORREF        CustColors[NumCustColors];
    static ColorTableEntry MapColorTable[NumMapColors];
    static uint            ColorPickerMessage;

  protected:
    TColorPickerData Data;
    __OWL TTooltip*        PickerTooltip;
    int              RefId;
    bool             ColorDialogOpened;
    bool             PaletteRealized;
    __OWL TColor           CurrentChosenColor;
    __OWL TColor           CurrentCustomColor;
    int              CurrentSel;
    int              ChosenColorSel;
    int              NumRows;
    int              BoxMargin;
    __OWL TSize            WindowSize;
    owl_string           DefaultText;
    owl_string           CustomText;
    LOGPALETTE*      MyLogPalette;

	DECLARE_RESPONSE_TABLE(TColorPicker);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
OWLEXT_END_NAMESPACE

#endif
