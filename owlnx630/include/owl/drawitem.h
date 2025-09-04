//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998, by Yura Bidus, All Rights Reserved
//
//$Revision: 1.25 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:33 $
//
// Definitions of classes TDrawItemProxy, TDrawItem,TButtonTextDrawItem,
// TDrawMenuItem, TDrawMenuItemProxy. Owner draw family.
// 
// Note this classes stil under development!!!!!!!!!!!!!!!
// 
//----------------------------------------------------------------------------

#if !defined(OWL_DRAWITEM_H)              // Sentry, use file only if it's not already included.
#define OWL_DRAWITEM_H

#include <owl/gadgetwi.h>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if defined(__HPUX_SOURCE)
typedef const char * Type_id;
//FMM Need it for TToolbarRes definition
#include <owl/resource.h>
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TDrawItemProxy
// ~~~~~ ~~~~~~~~~~~~~~
//
//
class _OWLCLASS TDrawItemProxy {
  public:
    typedef TIPtrArray<TDrawItem*> TDrawItemArray;
  public:
    TDrawItemProxy(TFont* font = new TGadgetWindowFont);
    virtual ~TDrawItemProxy();

            void        SetFont(TFont* font);
    virtual TFont*      GetFont()      { return Font; }
            void        SetCelArray(TCelArray* celarray, TAutoDelete del=AutoDelete);
            void        SetCelArray(TResId resId, uint count, TModule* module = gModule);
    virtual TCelArray*  GetCelArray()  { return CelArray; }
            void        RegisterItem(TDrawItem* item); // For automatic deletion of items
            TDrawItemArray& GetItems();

  // Data members -- will become private
  //
  protected_data:
    TFont*          Font;
    TCelArray*      CelArray;
    bool            DeleteCel;
    TDrawItemArray*  ItemArray;
};

//
//
// class TButtonTextDrawItem
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
//
class _OWLCLASS TButtonTextDrawItem: public TDrawItem{
  public:
    enum TAlign {aLeft, aCenter, aRight};
    enum TStyle {sNone=0,sBitmap=0x001, sText=0x002, sBitmapText=0x003};
    enum TLayoutStyle {lTextLeft,lTextTop,lTextRight,lTextBottom};
    enum TSeparator {sepNoBitmap=-2,sepUseBitmap=-1};

    TButtonTextDrawItem(TDrawItemProxy* proxy=0);
    ~TButtonTextDrawItem();

    // get/set
    LPCTSTR   GetText() const;
    void      SetText(LPCTSTR text);
    int        GetIndex() const;
    void      SetIndex(int index);
    TStyle     GetStyle() const;
    void      SetStyle(const TStyle style);
    TAlign    GetAlign() const;
    void      SetAlign(const TAlign align);
    TLayoutStyle GetLayoutStyle() const;
    void      SetLayoutStyle(const TLayoutStyle style);

    // usually it uses Proxy CelArray and Font
            void        SetFont(TFont* font);
    virtual TFont*      GetFont()      { return Font; }
            void        SetCelArray(TCelArray* celarray, TAutoDelete del=AutoDelete);
            void        SetCelArray(TResId resId, uint count, TModule* module = gModule);
    virtual TCelArray*  GetCelArray()  { return CelArray; }

    // overriden virtuals
    virtual void Draw(DRAWITEMSTRUCT & drawItem);
    virtual void Measure(MEASUREITEMSTRUCT & drawItem);
    virtual int  Compare(COMPAREITEMSTRUCT & drawItem);
    
    // new virtuals
    virtual void Paint(TDC& rc, TRect& rect);
    virtual void Layout(const TRect& src, TRect& textRect, TRect& bmpRect);
    virtual void PaintBorder(TDC& rc, TRect& rect);
    virtual void PaintText(TDC& rc, TRect& rect);
    virtual void PaintBitmap(TDC& rc, TRect& rect);
    virtual TColor GetBkColor();
    virtual TColor GetTextColor();
    
    bool IsEnabled()    { return (Flags&ODS_DISABLED)==0; }
    bool IsSelected()    { return (Flags&ODS_SELECTED)!=0;  }
    bool IsFocused()    { return (Flags&ODS_FOCUS)!=0;    }
    bool IsGrayed()      { return (Flags&ODS_GRAYED)!=0;    }
    bool IsChecked()    { return (Flags&ODS_CHECKED)!=0;  }

    // helpers
    virtual void GetTextSize(TSize& size);
    virtual void GetButtonSize(TSize& btnSize);

  // Data members -- will become private
  //
  protected_data:
    LPTSTR          Text;       // new'd copy of the text for this gadget
    int              Index;      // Index of Bitmap in Proxy CellArray if >= 0
    TAlign          Align;      // Alignment: left, center or right
    TStyle          Style;      // Style Bitmap, Text, Bitmap and Text
    TLayoutStyle    LayoutStyle;// Layout style

    //
    uint            Flags;

    //
    TFont*          Font;        // Local if overriding  ?? I need this? 
    TCelArray*      CelArray;   // Local if overriding  ?? I need this? 
    bool            DeleteCel;  // Local
    TDrawItemProxy* Proxy;      // Proxy
};


//
//
// class TDrawMenuItem
// ~~~~~ ~~~~~~~~~~~~~
//
//
class _OWLCLASS TDrawMenuItem: public TButtonTextDrawItem {
  public:
    typedef void (TDrawMenuItem::*TCheckPainter)(TDC& dc, TRect& rect);
  public:
    TDrawMenuItem(TDrawItemProxy* proxy, int cmdId, int index, LPCTSTR title=0);
//FMM DECLARE_CASTABLE line inserted
    DECLARE_CASTABLE;
    virtual void PaintBitmap(TDC& dc, TRect& rect);
    virtual void PaintText(TDC& rc, TRect& rect);
    virtual void GetTextSize(TSize& size);
    virtual void GetButtonSize(TSize& btnSize);
    virtual TColor GetBkColor();
    virtual TColor GetTextColor();
    virtual void PaintCheckFace(TDC& dc, TRect& rect);

            int  GetCmdId();
            void SetCheckPainter(TCheckPainter painter);

            void PaintCheck_None(TDC& dc, TRect& rect);
            void PaintCheck_3Angle(TDC& dc, TRect& rect);
            void PaintCheck_Arrow(TDC& dc, TRect& rect);
            void PaintCheck_Box(TDC& dc, TRect& rect);
            void PaintCheck_Diamond(TDC& dc, TRect& rect);
            void PaintCheck_Dot(TDC& dc, TRect& rect);
            void PaintCheck_Plus(TDC& dc, TRect& rect);
            void PaintCheck_V(TDC& dc, TRect& rect);
            void PaintCheck_X(TDC& dc, TRect& rect);

  // Data members -- will become private
  //
  protected_data:
    int            CmdId;
    TCheckPainter CheckPainter;
};

//
//
// class TDrawMenuItemProxy
// ~~~~~ ~~~~~~~~~~~~~~~~~~
//
class _OWLCLASS TDrawMenuItemProxy : public TDrawItemProxy {
  public:
    typedef TTypedArray<uint,uint,TStandardAllocator>  TUIntAray;
  public:
    TDrawMenuItemProxy(TResId resId, TModule* module=gModule);
    virtual ~TDrawMenuItemProxy();
    
    TDrawItem* FindItem(uint cmdId);
    void  RemapMenu(HMENU hMenu);
    bool  EvMenuChar(uint uChar, uint, HMENU hMenu, TParam2& param);

  protected:
    int  GetIndex(int cmdId);
    virtual TDrawItem*  CreateItem(uint itemId, LPCTSTR text);
    static TFont* CreateProxyMenuFont();

  protected:
    TUIntAray*  Array;
  private:
    int iRecurse;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//-------------------------------------------------------------------------
//
// Inlines
//
//
inline TDrawItemProxy::TDrawItemArray& TDrawItemProxy::GetItems()
{
  return *ItemArray;
}
//
inline LPCTSTR TButtonTextDrawItem::GetText() const
{
  return Text;
}
//
inline int TButtonTextDrawItem::GetIndex() const
{
  return Index;
}
//
inline TButtonTextDrawItem::TStyle TButtonTextDrawItem::GetStyle() const
{
  return Style;
}
//
inline void TButtonTextDrawItem::SetStyle(const TStyle style)
{
  Style = style;
}
//
inline TButtonTextDrawItem::TAlign TButtonTextDrawItem::GetAlign() const
{
  return Align;
}
//
inline void TButtonTextDrawItem::SetAlign(const TAlign align)
{
  Align = align;
}
//
inline TButtonTextDrawItem::TLayoutStyle TButtonTextDrawItem::GetLayoutStyle() const
{
  return LayoutStyle;
}
//
inline void TButtonTextDrawItem::SetLayoutStyle(const TLayoutStyle style)
{
  LayoutStyle = style;
}
//
inline void TDrawMenuItem::SetCheckPainter(TCheckPainter painter)
{
  CheckPainter = painter;
  Index = CheckPainter ? -3 : -1;
}
//
inline int TDrawMenuItem::GetCmdId()
{
  return CmdId;
}
//

__OWL_END_NAMESPACE

#endif  // OWL_DRAWITEM_H sentry.
