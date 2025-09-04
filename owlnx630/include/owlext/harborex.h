// ****************************************************************************
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// harborex.h:   header file
// Version:      1.6
// Date:         08.11.1998
// Author:       Dieter Windau
//
// Freeware OWL classes that extents the dockable and gadget system
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

#ifndef __OWLEXT_HARBOREX_H
#define __OWLEXT_HARBOREX_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
  #include <owlext/core.h>
#endif

#if !defined(OWL_DOCKING_H)
#include <owl/docking.h>
#endif
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// ****************** Forward declaration *************************************
class OWLEXTCLASS THarborEx;
class OWLEXTCLASS THarborManagement;
class OWLEXTCLASS TDockableControlBarEx;
class OWLEXTCLASS TButtonTextGadgetEx;

// ***************** Section strings for use in registry **********************

extern const TCHAR* MainWindowSection;
extern const TCHAR* LeftSection;
extern const TCHAR* TopSection;
extern const TCHAR* RightSection;
extern const TCHAR* BottomSection;
extern const TCHAR* VersionSection;
extern const TCHAR* CmdShowSection;
extern const TCHAR* ToolbarsSection;
extern const TCHAR* TooltipsSection;
extern const TCHAR* AccKeysSection;
extern const TCHAR* DrawGripperSection;
extern const TCHAR* FlatStyleSection;
extern const TCHAR* HintModeSection;
extern const TCHAR* IDSection;
extern const TCHAR* ToolbarSection;
extern const TCHAR* NameSection;
extern const TCHAR* LocationSection;
extern const TCHAR* VisibleSection;
extern const TCHAR* DefaultSection;
extern const TCHAR* GadgetCountSection;
extern const TCHAR* CountSection;
extern const TCHAR* ButtonSection;
extern const TCHAR* BmpIdSection;
extern const TCHAR* DisplayTypeSection;
extern const TCHAR* TextSection;

// ******************** THarborManagement *************************************

class OWLEXTCLASS THarborManagement : virtual public __OWL TEventHandler
{
  public:
    THarborManagement(LPCTSTR registryName);
   ~THarborManagement();

    void InsertToolbarMenuItem(__OWL TCommandEnabler& ce, int offset);
    // Insert the "Toolbar >" menu item at the menu item specified by ce + offset

    owl_string GetRegistryName() { return RegistryName; }
    static int GetInternVersion() { return InternVersion; }
    THarborEx* GetHarbor() { return Harbor; }

    void LoadSettings();
    void SaveSettings();

  protected:

    // simple wrapper functions to harbor
    //
    void CmShowHideCtlBar(uint id);
    void CmCustomize();

    void CmGadgetReset();
    void CmGadgetDelete();
    void CmGadgetCopyImage();
    void CmGadgetPasteImage();
    void CmGadgetAppearance();
    void CmGadgetImage();
    void CmGadgetText();
    void CmGadgetImageText();
    void CmGadgetBeginGroup();

    void CeGadgetReset(__OWL TCommandEnabler& ce);
    void CeGadgetDelete(__OWL TCommandEnabler& ce);
    void CeGadgetCopyImage(__OWL TCommandEnabler& ce);
    void CeGadgetPasteImage(__OWL TCommandEnabler& ce);
    void CeGadgetAppearance(__OWL TCommandEnabler& ce);
    void CeGadgetImage(__OWL TCommandEnabler& ce);
    void CeGadgetText(__OWL TCommandEnabler& ce);
    void CeGadgetImageText(__OWL TCommandEnabler& ce);
    void CeGadgetBeginGroup(__OWL TCommandEnabler& ce);

  protected:
    static int InternVersion;

    THarborEx* Harbor;
    HMENU      PopupMenu;
    owl_string RegistryName;

  DECLARE_RESPONSE_TABLE(THarborManagement);
};

// ************************ TDCBData ******************************************

const int MaxTitleLen = 255;

// class that encapsulate relevant TDockableToolBarEx data
class OWLEXTCLASS TDCBData
{
public:
  TDockableControlBarEx* DCB;
  __OWL TDockingSlip*          Slip; // if DCB is not visible this is 0
  __OWL TRect                  Rect; // in screen coordinates
  TCHAR                  Title[MaxTitleLen];

public:
  TDCBData()
    { DCB=0; Slip=0; Rect=__OWL TRect(0,0,0,0); *Title = 0; }
  TDCBData(const TDCBData& data)
    { *this = data;  }
  ~TDCBData() { }

  TDCBData& operator =(const TDCBData& d);

  bool operator ==(const TDCBData& d) const
    { return memcmp(this, &d, sizeof(d)) == 0 ? true : false; }
  bool operator !=(const TDCBData& d) const
    { return !(*this == d); }
};

// ****************** THarborEx ***********************************************

class OWLEXTCLASS THarborEx : public __OWL THarbor
{
  friend class THarborManagement;
  friend class TButtonGadgetEx;
  friend class TToolbarCustomizeSheet;
  friend class TToolbarPage;

  public:
    enum TGadgetType {
      IsNoGadget,
      IsToolbarGadget,
      IsRefGadget
    };

  public:
    THarborEx(__OWL TDecoratedFrame& df,
              bool leftSlip   = true,
              bool topSlip    = true,
              bool rightSlip  = true,
              bool bottomSlip = false);
    // If you create one of the four TEdgeSlips in THarbor before you insert
    // decorations in TDecoratedFrame all the TDockable's in THarbor insert near
    // the main window borders. Some examples:
    // - if you don't create top and left slip, e.g. a inserted ruler
    //   is not near the client window of the decorated frame
    // - if you create bottom slip here, e.g. a inserted statusbar
    //   is not always at bottom of the decorated frame
    virtual ~THarborEx();

    // Called by dockable control bars
    //
    bool GadgetDraggingBegin();

    bool IsShowTooltips() { return ShowTooltips; }
    void SetShowTooltips(bool showTooltips) { ShowTooltips = showTooltips; }
    bool IsWithAccel() { return WithAccel; }
    void SetWithAccel(bool b) { WithAccel = b; }
    __OWL TGadgetWindow::THintMode GetHintMode() { return HintMode; }
    void SetHintMode(__OWL TGadgetWindow::THintMode HintMode);
    bool IsDrawGripper() { return DrawGripper; }
    void SetDrawGripper(bool drawGripper);
    // Get/set options for all dockable toolbars in harbor

    virtual bool PreProcessAppMsg(MSG& msg);
    // Translate some messages for DockingEx system
    // To be called from TApplication ProcessAppMsg(...)
    // return true if the message is processed

    virtual int PostGetHintText(uint id, LPTSTR buf, int size, __OWL THintText hintType);
    // Retrieves the hint text associated with a particular Id
    // To be called from TDecoratedFrame GetHintText(...)
    // If ShowTooltips == false, the tooltips will be deleted
    // If WithAccel == false, the accelerator keys defined by resource in style:
    // " (key)" will be hidden.

    bool IsDCBTitleValid(LPCTSTR title);
    // return true, if the title don't exist. Title should be a unique identifier

    int  GetFreeDCBId();
    // returns a unused id for a new dockable controlbar. -1 if an error occurs

    bool IsCustomizeMode() { return CustomizeMode; }
    // returns true if the system is in customize mode

    void SetCustGadget(__OWL TGadgetWindow* gw, __OWL TGadget* gadget);
    void SetCustGadget(int refId);
    void SetCustGadgetNull();

    TGadgetType    GetCustGadgetType() { return CustGadgetType; }
    __OWL TGadgetWindow* GetCustWindow()     { return CustWindow; }
    __OWL TGadget*       GetCustGadget()     { return CustGadget; }
    int            GetCustGadgetRef()  { return CustGadgetRef; }

    void CheckBeforeInsertDefaultGadget(int id);
    void InsertDefaultGadgets();
    void InsertDefaultGadget(int idx);

    HMENU GetToolbarMenu(bool getDCBData = true);
    HMENU GetGadgetMenu();
    void  UpdateShowHideCtlBar();

  protected:

    virtual __OWL TEdgeSlip* ConstructEdgeSlip(__OWL TDecoratedFrame& df, __OWL TAbsLocation location);
    // Factory function overridden to use TEdgeSlipEx in lace of TEdgeSlip

    virtual __OWL TFloatingSlip* ConstructFloatingSlip(__OWL TDecoratedFrame& df,
      int x, int y, __OWL TWindow* dockableWindow);
    // Factory function overridden to use TFloatingSlipEx in lace of TFloatingSlip

    virtual TToolbarCustomizeSheet* CreateCustomizeDlg(__OWL TDecoratedFrame& frame);
		virtual void ExecuteGadgetDialog(__OWL TDecoratedFrame& frame, TButtonTextGadgetEx* btg);

    // Mouse handlers - invoked when harbor grabs capture
    //
    void     EvMouseMove(uint modKeys, __OWL TPoint& point);
    void     EvLButtonUp(uint modKeys, __OWL TPoint& point);
    void     EvLButtonDblClk(uint modKeys, __OWL TPoint& point);

    void  SetCustomizeMode(bool b);
    void  GetDCBData();
    TDockableControlBarEx* GetHitDCB(const __OWL TPoint& screenPoint);
    void  ShowDragDropLine(bool show);
    void  EraseCustomizeFrame();
    void  InsertDCBData(TDockableControlBarEx* cb, __OWL TDockingSlip* slip);

    void  CmShowHideCtlBar(uint id);
    void  CmCustomize();

    void  CmGadgetReset();
    void  CmGadgetDelete();
    void  CmGadgetCopyImage();
    void  CmGadgetPasteImage();
    void  CmGadgetAppearance();
    void  CmGadgetImage();
    void  CmGadgetText();
    void  CmGadgetImageText();
    void  CmGadgetBeginGroup();

    void  CeGadgetReset(__OWL TCommandEnabler& ce);
    void  CeGadgetDelete(__OWL TCommandEnabler& ce);
    void  CeGadgetCopyImage(__OWL TCommandEnabler& ce);
    void  CeGadgetPasteImage(__OWL TCommandEnabler& ce);
    void  CeGadgetAppearance(__OWL TCommandEnabler& ce);
    void  CeGadgetImage(__OWL TCommandEnabler& ce);
    void  CeGadgetText(__OWL TCommandEnabler& ce);
    void  CeGadgetImageText(__OWL TCommandEnabler& ce);
    void  CeGadgetBeginGroup(__OWL TCommandEnabler& ce);

    void  LoadSettings(THarborManagement& harborMan);
    void  SaveSettings(THarborManagement& harborMan);

  protected:

    typedef enum tagGadgetState {
      IsNone,
      IsCut,
      IsCopy,
      IsDelete
    } TGadgetState;

    __OWL TDecoratedFrame& DecoratedFrame;

    bool             ShowTooltips;  // Show tooltips for all dockable controlbars
    bool             WithAccel;     // Show accelerator key in tooltips
    bool             DrawGripper;   // Draw gripper for all edge slips
    __OWL TGadgetWindow::THintMode
                     HintMode;      // Hint mode for all dockable controlbars
    bool             CustomizeMode; // True, if the customize dialog is open
//  bool             LargeButtons;  // Future: draw button gadgets in large style
//  bool             GrayButtons;   // Future: draw button gadgets in gray colors

    bool             GadgetDragging;// True, if we are in gadget dragging mode
    HCURSOR          CutPasteCursor;
    HCURSOR          CopyCursor;
    HCURSOR          DeleteCursor;
    HCURSOR          OldCursor;     // Cursor, before gadget dragging started

    __OWL TMemoryDC*       MemDC;
    __OWL TBitmap*         MemBmp;
    __OWL TScreenDC*       DragScreenDC;
    __OWL TRect            DragDropRect;
    __OWL TPoint           DragDropP1;    // P1 of line that is drawn for drag & drop
    __OWL TPoint           DragDropP2;    // P2 of line that is drawn for drag & drop
    bool             DragDropLineVisible;
    __OWL TGadget*         DragDropGadget;
    __OWL TGadgetWindow*   DragDropWindow;
    __OWL TGadgetWindow::TPlacement
                     DragDropPlacement;
    TGadgetState     DragDropGadgetState;

    TGadgetType      CustGadgetType;  // State of the actual customize gadget
    __OWL TGadget*         CustGadget;      // Actual customize gadget
    __OWL TGadgetWindow*   CustWindow;      // GadgetWindow of CustGadget
    int              CustGadgetRef;   // Id of reference CustGadget

    __OWL TResId           CopiedBmpResId;

    TToolbarCustomizeSheet* CustomizeDlg;

    __OWL TIPtrArray<TDCBData*> DCBData;

  DECLARE_RESPONSE_TABLE(THarborEx);
};

// ******************** Inline Implementations ********************************

inline void THarborManagement::CmShowHideCtlBar(uint id)
  { PRECONDITION(Harbor); Harbor->CmShowHideCtlBar(id); }
inline void THarborManagement::CmCustomize()
  { PRECONDITION(Harbor); Harbor->CmCustomize(); }

inline void THarborManagement::CmGadgetReset()
   { PRECONDITION(Harbor); Harbor->CmGadgetReset(); }
inline void THarborManagement::CmGadgetDelete()
   { PRECONDITION(Harbor); Harbor->CmGadgetDelete(); }
inline void THarborManagement::CmGadgetCopyImage()
  { PRECONDITION(Harbor); Harbor->CmGadgetCopyImage(); }
inline void THarborManagement::CmGadgetPasteImage()
  { PRECONDITION(Harbor); Harbor->CmGadgetPasteImage(); }
inline void THarborManagement::CmGadgetAppearance()
  { PRECONDITION(Harbor); Harbor->CmGadgetAppearance(); }
inline void THarborManagement::CmGadgetImage()
   { PRECONDITION(Harbor); Harbor->CmGadgetImage(); }
inline void THarborManagement::CmGadgetText()
  { PRECONDITION(Harbor); Harbor->CmGadgetText(); }
inline void THarborManagement::CmGadgetImageText()
   { PRECONDITION(Harbor); Harbor->CmGadgetImageText(); }
inline void THarborManagement::CmGadgetBeginGroup()
   { PRECONDITION(Harbor); Harbor->CmGadgetBeginGroup(); }

inline void THarborManagement::CeGadgetReset(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetReset(ce); }
inline void THarborManagement::CeGadgetDelete(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetDelete(ce); }
inline void THarborManagement::CeGadgetCopyImage(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetCopyImage(ce); }
inline void THarborManagement::CeGadgetPasteImage(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetPasteImage(ce); }
inline void THarborManagement::CeGadgetAppearance(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetAppearance(ce); }
inline void THarborManagement::CeGadgetImage(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetImage(ce); }
inline void THarborManagement::CeGadgetText(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetText(ce); }
inline void THarborManagement::CeGadgetImageText(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetImageText(ce); }
inline void THarborManagement::CeGadgetBeginGroup(__OWL TCommandEnabler& ce)
   { PRECONDITION(Harbor); Harbor->CeGadgetBeginGroup(ce); }

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
OWLEXT_END_NAMESPACE

#endif
