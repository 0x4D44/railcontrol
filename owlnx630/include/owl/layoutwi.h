//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of classes TLayoutMetrics & TLayoutWindow
//----------------------------------------------------------------------------

#if !defined(OWL_LAYOUTWI_H)
#define OWL_LAYOUTWI_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_LAYOUTCO_H)
# include <owl/layoutco.h>
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
// class TLayoutMetrics
// ~~~~~ ~~~~~~~~~~~~~~
// When specifying the layout metrics for a window, four layout constraints
// are needed.
//
class _OWLCLASS TLayoutMetrics {
  public:
    TEdgeConstraint         X;      // Horz1 can be lmLeft, lmCenter, lmRight
    TEdgeConstraint         Y;      // Vert1 can be lmTop, lmCenter, lmBottom
    TEdgeOrWidthConstraint  Width;  // Horz2 can be lmWidth, lmCenter, lmRight
    TEdgeOrHeightConstraint Height; // Vert2 can be lmHeight, lmCenter, lmBottom

    // Defaults each co: RelWin=0, MyEdge=(1st from above), Relationship=AsIs
    //
    TLayoutMetrics();
  
    void SetMeasurementUnits(TMeasurementUnits units);
};

//
// Private structs used by TLayoutWindow
//
struct TChildMetrics;
struct TConstraint;
struct TVariable;

//
// class TLayoutWindow
// ~~~~~ ~~~~~~~~~~~~~
// When specifying the layout metrics for a window, there are several options:
// e.g. in the horizontal direction,
//
//  Two Edge Constraints in X and Width
//   1. left edge and right edge
//   2. center edge and right edge
//   3. left edge and center edge
//
//  Edge Constraint and Size constraint in X and Width
//   4. left edge and size
//   5. right edge and size
//   6. center edge and size
//
// The same holds true in the vertical direction for Y and Height
//
// It is also possible to specify "lmAsIs" in which case we use the windows
// current value
//
// Specifying "lmAbsolute" means that we will use whatever is in data member
// "Value"
//
// We just name the fields "X" and "Width" and "Y" and "Height",
// although its okay to place a right or center edge constraint in the
// "Width" field and its also okay to place a right edge constraint in
// the "X" field (i.e. option #3)
//
// However, it's NOT okay to place a width constraint in the "X" or
// "Height" fields or a height constraint in the "Y" or "Width" fields.
//
class _OWLCLASS TLayoutWindow : virtual public TWindow {
  public:
    TLayoutWindow(TWindow* parent,
                  LPCTSTR  title = 0,
                  TModule* module = 0);

   ~TLayoutWindow();

    // Causes the receiver to size/position its children according to the
    // specified layout metrics
    //
    // If you change the layout metrics for a child window call Layout()
    // to have the changes take effect
    //
    //!BB
    //!BB Consider making Layout virtual. Beta users have requested so that
    //!BB they can enhance it: For example, one user wants Layout to use
    //!BB 'DeferWindowPos' API to minimize flicker and avoid dirty repaints
    //!BB in cases where the child windows overlap. Sounds like a fair
    //!BB request to me
    //!BB
    virtual void    Layout();

    void            SetChildLayoutMetrics(TWindow& child, TLayoutMetrics& metrics);
    bool            GetChildLayoutMetrics(TWindow& child, TLayoutMetrics& metrics);
    bool            RemoveChildLayoutMetrics(TWindow& child);

  protected:
    // Responds to a change in size by calling Layout()
    //
    void            EvSize(uint sizeType, TSize& size);

    // Override TWindow virtual to allow cleanup of child metrics
    //
    void            RemoveChild(TWindow* child);

  protected_data:
    TSize           ClientSize;

  private:
    enum TWhichConstraint {
      XConstraint,
      YConstraint,
      WidthConstraint,
      HeightConstraint
    };

    TChildMetrics*  ChildMetrics;
    TConstraint*    Constraints;
    TConstraint*    Plan;
    TVariable*      Variables;
    bool            PlanIsDirty;
    int             NumChildMetrics;
    int             FontHeight;

    TChildMetrics*  GetChildMetrics(TWindow& child);

    void            AddConstraint(TChildMetrics&     metrics,
                                  TLayoutConstraint* c,
                                  TWhichConstraint   whichContraint);
    void            BuildConstraints(TChildMetrics& childMetrics);
    void            RemoveConstraints(TChildMetrics& childMetrics);

    void            BuildPlan();
    void            ExecutePlan();
    void            ClearPlan();

    int             LayoutUnitsToPixels(int);
    void            GetFontHeight();

    // Hidden to prevent accidental copying or assignment
    //
    TLayoutWindow(const TLayoutWindow&);
    TLayoutWindow& operator =(const TLayoutWindow&);

  DECLARE_RESPONSE_TABLE(TLayoutWindow);
  //DECLARE_CASTABLE;
  //DECLARE_STREAMABLE(_OWLCLASS,TLayoutWindow,1); // Because VC++
  DECLARE_STREAMABLE_OWL(NS_OWL::TLayoutWindow,1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TLayoutWindow );

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_LAYOUTWIN_H
