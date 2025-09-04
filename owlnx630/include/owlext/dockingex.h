// ****************************************************************************
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// dockingex.h:  header file
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
#ifndef __OWLEXT_DOCKINGEX_H
#define __OWLEXT_DOCKINGEX_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
  #include <owlext/core.h>
#endif

#ifndef   OWL_DOCKING_H
#include <owl/docking.h>
#endif
#ifndef   OWL_CONTAIN_H
#include <owl/contain.h>
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
class OWLEXTCLASS TFloatingSlipEx;
class OWLEXTCLASS TEdgeSlipEx;

struct TGridItem;
struct TGridLine;
struct TGridLayout;

// ****************** TDockableControlBarEx ***********************************

class OWLEXTCLASS TDockableControlBarEx : public __OWL TDockableControlBar
{
  friend class THarborEx;

  public:
    TDockableControlBarEx(
      __OWL TWindow* parent = 0,
      TTileDirection direction = Horizontal,
      __OWL TFont* font = new __OWL TGadgetWindowFont,
      __OWL TModule* module = 0);
    // default constructor

    TDockableControlBarEx(
      uint id,              // id of toolbar, this should be a unique identifier
      LPCTSTR title,        // name of toolbar, should not be empty
      __OWL TWindow* parent = 0,  // see default constructor
      bool _Default = true, // true, for toolbars, that comes out with your app
      bool hasHelp = false, // true, if there exist a helppage with helpid = id
      TTileDirection direction = Horizontal, // see default constructor 
      __OWL TFont* font = new __OWL TGadgetWindowFont,   // see default constructor
      __OWL TModule* module = 0                    // see default constructor
    );
    // new constructor

    virtual ~TDockableControlBarEx();

    void SetDefaultGadgets(const __OWL TIntArray& defaultGadgets);
    void SetDefaultGadgets(const int iArray[], uint numGadgets);
    // Set the dafault gadgets with the id. All id's should be added in
    // GadgetsDescriptors. Best place to set the default gadgets is after
    // the toolbar is inserted in harbor

    void InsertDefaultGadgets();
    // Remove all existing gadgets and insert the default gadgets in the toolbar

#if (OWLInternalVersion < 0x06000000L)
    static void EnableFlatStyle(bool enable) { FlatStyle = enable; }
    static bool GetFlatStyle() { return FlatStyle; }
#else
    virtual bool Create();
#endif

    virtual void Hide();
    virtual void Show();
    virtual void PreRemoved();
    // Called by TFloatingSlipEx or TEdgeSlipEx if the conrolbar will be removed

    __OWL TResult WindowProc(uint msg, __OWL TParam1 p1, __OWL TParam2 p2);
    // Processes incoming messages when toolbar is hide

    void PaintGadgets(__OWL TDC& dc, bool b, __OWL TRect& rect);
    // Overridden to paint the actual customize gadget with a black rectangle

    void Paint(__OWL TDC& dc, bool erase, __OWL TRect& rect);
    // Overridden to paint the etched toolbar border and the gripper

    void ToggleSlip();
    // toggles it from docked position to floating position and back again

    bool IsDefault() { return Default; }
    void SetDefault(bool b) { Default = b; }

    bool GetHasHelp() { return HasHelp; }
    void SetHasHelp(bool b) { HasHelp = b; }

    virtual __OWL TGadget* RemoveEx(__OWL TGadget* gadget, bool del);
    virtual void InsertEx(__OWL TGadget& gadget,
                          TPlacement placement = After,
                          __OWL TGadget* sibling = 0);

    __OWL TGadget* PrevGadget(__OWL TGadget* gadget) const;
    // Returns previous Gadget

  protected:
    virtual void SetupWindow();

    void RemoveAllGadgets();
    // Remove all gadgets from toolbar

    __OWL TGadget* GetAndSetCustomizeGadget(__OWL TPoint& point);
		// Get the actual customize gadget and set them in harbor or returns 0

    void CheckOnLastGadget();
    // If the controlbar has no visible gadgets a invisible gadget is inserted

    void CheckOnNeighbourSeparatorGadgets(__OWL TGadget* actGadget);
    // Check the neighbours gadgets of actGadget. If they are seperator gadgets
    // that are not used they will be removed

    __OWL TGadget* GetDragDropGadget(const __OWL TPoint& pt, __OWL TPoint& p1, __OWL TPoint& p2,
	    TPlacement& placement);


#if (OWLInternalVersion >= 0x06000000L)
    void EvOwlWindowDocked(uint pos, __OWL TDockingSlip* slip);
#endif

    void EvLButtonDown(uint modKeys, __OWL TPoint& point);
    void EvLButtonDblClk(uint modKeys, __OWL TPoint& point);
    void EvRButtonDown(uint modKeys, __OWL TPoint& point);

    void StorePosAndLocation();
    void LoadSettings(THarborManagement& harborMan);
    void SaveSettings(THarborManagement& harborMan);

    void CmOk();
    void CmCancel();

    // Some internal draw functions
    void DrawVerticalDividers(__OWL TDC& dc, __OWL TGadget* firstgadg, __OWL TGadget* lastgadg,
      int miny, int maxy, bool skipEndOfRowCheck = true);
    void DrawHorizontalDividers(__OWL TDC& dc, __OWL TGadget* firstgadg, __OWL TGadget* lastgadg,
      int minx, int maxx);
    void DrawCustomizeRect(__OWL TDC& dc, __OWL TRect& rect);

  protected:
#if (OWLInternalVersion < 0x06000000L)
    static bool  FlatStyle;      // Draw gadgets in flat style (similar as OWL6)
#endif

    __OWL TRect        Rect;           // Last position. Save when hide
    __OWL TAbsLocation Location;       // Last location. Save when hide
    __OWL TRect        ToggleRect;     // Position when toggle the slip view
    __OWL TAbsLocation ToggleLocation; // Location when toggle the slip view
    bool         Default;        // Controlbar is a default bar
                                 // You can't delete or rename a default bar
    bool         HasHelp;        // Exist a context help for the controlbar
    __OWL TIntArray    DefaultGadgets; // this gadgets will be shown by default in bar

  DECLARE_RESPONSE_TABLE(TDockableControlBarEx);
};

// ****************** TFloatingSlipEx *****************************************

class OWLEXTCLASS TFloatingSlipEx: public __OWL TFloatingSlip
{
public:
  TFloatingSlipEx(__OWL TWindow* parent, int x, int y, __OWL TWindow* clientWnd = 0,
    bool shrinkToClient = true, int captionHeight = DefaultCaptionHeight,
    bool popupPalette = true, __OWL TModule* module = 0);
  virtual ~TFloatingSlipEx();

  void DockableInsert(__OWL TDockable& dockable, const __OWL TPoint* topLeft,
    __OWL TRelPosition position, __OWL TDockable* relDockable);
  void DockableRemoved(const __OWL TRect& orgRect);

protected:
  void EvNCLButtonDown(uint hitTest, __OWL TPoint& point);
  void EvNCRButtonDown(uint hitTest, __OWL TPoint& point);  
  void EvLButtonDown(uint hitTest, __OWL TPoint& point);
  void EvClose();
  void EvNCLButtonDblClk(uint modKeys, __OWL TPoint& point);

  DECLARE_RESPONSE_TABLE(TFloatingSlipEx);
};

// ****************** TEdgeSlipEx *********************************************

class OWLEXTCLASS TEdgeSlipEx : public __OWL TWindow, public __OWL TDockingSlip
{
  public:
    TEdgeSlipEx(__OWL TDecoratedFrame& parent, __OWL TAbsLocation location, __OWL TModule* module = 0);
    virtual ~TEdgeSlipEx();

    void     SetGripperMargins(TDockableControlBarEx* cb);
    // Set the margins for the gripper

    // Overridden TWindow virtuals
    __OWL TResult  EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    void     EvCommandEnable(__OWL TCommandEnabler& commandEnabler);
    void     SetupWindow();

    // These are overriden for any window that wants to be notified on dockable
    // windows being moved or removed.
    void DockableInsert(__OWL TDockable& dockable, const __OWL TPoint* topLeft = 0,
      __OWL TRelPosition position = __OWL rpNone, __OWL TDockable* relDockable = 0);
    void DockableMove(__OWL TDockable& dockable, const __OWL TPoint* topLeft = 0,
      __OWL TRelPosition position = __OWL rpNone, __OWL TDockable* relDockable = 0);
    void DockableRemoved(const __OWL TRect& orgRect);
    __OWL TAbsLocation GetLocation() const;

  protected:
    // Response functions
    void     EvLButtonDown(uint modKeys, __OWL TPoint& point);
    void     EvLButtonDblClk(uint modKeys, __OWL TPoint& point);
    uint     EvNCCalcSize(bool calcValidRects, NCCALCSIZE_PARAMS & calcSize);
    void     EvNCPaint(HRGN);
    bool     EvEraseBkgnd(HDC);
    void     EvParentNotify(uint event, uint childHandleOrX, uint childIDOrY);
    void     EvWindowPosChanging(WINDOWPOS & windowPos);

    // Internal tiling functions
    void ArrangeAllDockables(__OWL TWindow* NewDockable);
    void LayoutSlip(int Height);

  protected:
    // Internal tiling data
    __OWL TAbsLocation Location;
    __OWL TGridType    GridType;
    bool InsertingDockable; // To prevent parent notify layout during other layout

  DECLARE_RESPONSE_TABLE(TEdgeSlipEx);
};

// ****************** TGridItem ***********************************************

// Represents a single dockable in the slip
struct TGridItem
{
  // Constructor
  TGridItem(__OWL TWindow* Window, __OWL TAbsLocation Location);

  // Tiling functions - used to eliminate overlaps along a gridline
  void ArrangePrecedingItems();
  void ArrangeFollowingItems(int SlipWidth);
  void ArrangeLeft();
  void ArrangeRight(bool Limit, int SlipWidth);

  // Data members
  __OWL TWindow*   Dockable;      // Dockable window which this item represents
  int        LeftEdge;      // Left or Top depending on orientation
  int        TopEdge;       // Top or Left depending on orientation
  int        Width;         // Width or Height depending on orientation
  int        Height;        // Height or Width depending on orientation
  TGridLine* GridLine;      // Used to retieve the top edge for this grid item
  TGridItem* NextItem;      // Pointer to next item on the same gridline
  TGridItem* PrevItem;      // Pointer to previous item on the same gridline
};

// ****************** TGridLine ***********************************************

// Represents a row of one or more dockables in the slip (linked list)
struct TGridLine
{
  // Constructor and destructor
  TGridLine(uint32 Top);
  ~TGridLine();           // Delete all the grid items in the linked list

  // Tiling functions
  void ComputeHeight();   // Find height of the tallest grid item
  TGridItem* LayoutGridItems(TGridItem* Anchor, int SlipWidth);  // Horizontal layout - sets left edges mainly

  // List management functions
  void InsertGridItem(TGridItem* NewItem);  // Insert into a sorted linked list
  void RemoveGridItem(TGridItem* NewItem);  // Remove from the linked list

  // Data members
  int        TopEdge;     // All grid items share this top edge
  int        Height;      // Height of the tallest grid item
  TGridItem* FirstItem;   // Start of the linked list of grid items
  TGridLine* NextLine;    // Pointer to the next gridline
  TGridLine* PrevLine;    // Pointer to the previous gridline
};

// ****************** TGridLayout *********************************************

// Represents all rows of dockables in the slip (linked list)
struct TGridLayout
{
  // Constructor and destructor
  TGridLayout(__OWL TAbsLocation Loc);
  ~TGridLayout();           // Delete all the grid lines in the linked list

  // Tiling functions
  void CreateLists(__OWL TWindow* parent, __OWL TWindow* dockable);  // Build linked lists
  int  LayoutGridLines();  // Vertical layout - sets top edge mainly
  void LayoutDockables();  // Move actual windows about

  // List management functions
  void InsertGridLine(TGridLine* NewLine);       // Insert into a sorted linked list
  TGridLine* InsertGridItem(TGridItem* NewItem); // Insert into a sorted linked list
  TGridLine* InsertDockableItem();               // Insert into a sorted linked list

  // Data members
  TGridLine* FirstLine;    // Start of the linked list of grid lines
  __OWL TWindow*   Parent;       // The TBorderSlip which owns this object
  __OWL TWindow*   Dockable;     // The toolbar just dropped on this edge slip
  TGridItem* DockableItem; // The grid item representing Dockable
  __OWL TAbsLocation Location;   // The position of the TBorderSlip
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
OWLEXT_END_NAMESPACE

#endif
