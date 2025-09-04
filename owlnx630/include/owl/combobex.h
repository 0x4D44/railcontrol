//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2007-11-20 09:10:29 $
//
// Definition of class TComboBoxEx.
//----------------------------------------------------------------------------

#if !defined(OWL_COMBOBEX_H)
#define OWL_COMBOBEX_H

#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#if !defined(OWL_COMBOBOX_H)
# include <owl/combobox.h>
#endif
#if !defined(OWL_PROPERTY_H)
# include <owl/property.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TImageList;
class _OWLCLASS TComboBoxEx;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TComboBoxExItem
// ~~~~~ ~~~~~~~~~~~~~
//
class _OWLCLASS TComboBoxExItem {
  friend class _OWLCLASS TComboBoxEx;
  public:
    TComboBoxExItem();
    TComboBoxExItem(LPCTSTR str, int item=-1,int image = -1);
    TComboBoxExItem(LPCTSTR str, int item, int image, int selectedImage, int overlayImage = -1, int indent = -1, LPARAM param = 0);
    TComboBoxExItem(const TComboBoxExItem& item);
    ~TComboBoxExItem();

    // for templates
    bool operator < (const TComboBoxExItem& item) { return this < &item; }
    bool operator ==(const TComboBoxExItem& item) { return this == &item;}

    TProperty<int>        Mask;       // Band image index (into rebar image list): don't use -1
    TProperty<int>        Item;       // == -1 to add at end
    TObjProperty<owl_string>   Text;       // Band text label
    TProperty<int>         Image;      // The item image
    TProperty<int>        Selected;   // The item selected image
    TProperty<int>        Overlay;    // Band colors -
    TProperty<int>        Indent;     // Band colors -
    TProperty<LPARAM>      Param;      // additional param


  protected:
    // Initialise all data members to zero
    void Init();

    // Initialize native item with this.
    void InitItem(COMBOBOXEXITEM& item, uint tsize = 0) const;

    // Initialize this with native item.
    void ReFillItem(const COMBOBOXEXITEM& item);
};

//
// class TComboBoxEx
// ~~~~~ ~~~~~~~~~~~
//
class _OWLCLASS TComboBoxEx : public TComboBox {
  // Constructors
  public:
    TComboBoxEx(TWindow* parent,
                int id,
                int x, int y, int w, int h,
                uint32 style,
                uint textLimit,
                TModule* module = 0);
    TComboBoxEx(TWindow* parent,
                int ResourceId,
                uint textLen = 0,
                TModule* module = 0);
    ~TComboBoxEx();


    HWND GetEditControl();
    HWND GetComboBoxCtrl();
    bool HasEditChanged();

    uint32 GetExtendedStyle() const;
    uint32 SetExtendedStyle(uint32 mask, uint32 style);

    TImageList* GetImageList();
    void SetImageList(TImageList* list, TAutoDelete = AutoDelete);

    void  SetUnicode(bool unicode);
    bool  GetUnicode();

    // Operations
    int   DeleteItem(int index);
    bool   GetItem(TComboBoxExItem& item, int index=-1);
    int   InsertItem(const TComboBoxExItem& item);
    bool   SetItem(const TComboBoxExItem& item, int index=-1);

    // Override TWindow virtual member functions
    //
    uint   Transfer(void* buffer, TTransferDirection direction);

  protected:
    // Override TWindow virtual member functions
    //
    LPCTSTR       GetClassName();
    void          SetupWindow();
    void            DeleteItem (DELETEITEMSTRUCT & deleteInfo);
    // Do nothing handlers
    TResult       EvCompareItem(uint ctrlId, COMPAREITEMSTRUCT & comp);
    void          EvDeleteItem(uint ctrlId, DELETEITEMSTRUCT & del);
    void          EvDrawItem(uint ctrlId, DRAWITEMSTRUCT & draw);
    void          EvMeasureItem(uint ctrlId, MEASUREITEMSTRUCT & meas);

  protected_data:
    TImageList* ImageList;
    bool        ShouldDelete;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TComboBoxEx(const TComboBoxEx&);
    TComboBoxEx& operator =(const TComboBoxEx&);

  DECLARE_RESPONSE_TABLE(TComboBoxEx);

//!YB Need to be consistent - Common Controls are not streamable for the
//!YB the most part. This guy only invokes TControl's version anyway.
//!YB
//!YB DECLARE_STREAMABLE_FROM_BASE(_OWLCLASS, __OWL TComboBoxEx, 1);
};


typedef TObjectArray<TComboBoxExItem> TComboBoxExItemArray;
typedef TComboBoxExItemArray::Iterator TComboBoxExItemArrayIter;
//
//  class TComboBoxExData
//  ----- -------------
//
class _OWLCLASS TComboBoxExData {
  public:
    TComboBoxExData();
   ~TComboBoxExData();

    int   AddItem(const TComboBoxExItem& item);
    int   DeleteItem(int index);
    TComboBoxExItem& GetItem(int index);

    void  Clear();
    uint  Size();
    void  Select(int index);
    void  SelectString(LPCTSTR str);
    int    GetSelIndex();
    owl_string& GetSelection();
    int   GetSelCount() const;
    void   ResetSelections();

    int   GetSelStringLength() const;
    void  GetSelString(LPTSTR buffer, int bufferSize) const;

  protected:
    TComboBoxExItemArray* Items;
    owl_string            Selection;
    int                    SelIndex;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// inlines
//

//
inline HWND TComboBoxEx::GetEditControl(){
  PRECONDITION(GetHandle());
  return (HWND)SendMessage(CBEM_GETEDITCONTROL,0);
}

//
inline HWND TComboBoxEx::GetComboBoxCtrl(){
  PRECONDITION(GetHandle());
  return (HWND)SendMessage(CBEM_GETCOMBOCONTROL, 0);
}

//
inline uint32 TComboBoxEx::GetExtendedStyle() const{
  PRECONDITION(GetHandle());
  return (uint32)CONST_CAST(TComboBoxEx*,this)->SendMessage(CBEM_GETEXTENDEDSTYLE, 0);
}

//
inline uint32 TComboBoxEx::SetExtendedStyle(uint32 mask, uint32 style){
  PRECONDITION(GetHandle());
  return (uint32)SendMessage(CBEM_SETEXTENDEDSTYLE, TParam1(mask), TParam2(style));
}

//
inline int TComboBoxEx::DeleteItem(int index){
  PRECONDITION(GetHandle());
  return (int)SendMessage(CBEM_DELETEITEM, TParam1(index));
}

//
inline bool TComboBoxEx::HasEditChanged(){
  PRECONDITION(GetHandle());
  return SendMessage(CBEM_HASEDITCHANGED,0)==0;
}

//
inline TImageList* TComboBoxEx::GetImageList(){
  return ImageList;
}

//
inline void TComboBoxEx::SetUnicode(bool unicode){
  PRECONDITION(GetHandle());
  SendMessage(CBEM_SETUNICODEFORMAT,TParam1(unicode));
}

//
inline bool TComboBoxEx::GetUnicode(){
  PRECONDITION(GetHandle());
  return SendMessage(CBEM_GETUNICODEFORMAT,0)!=0;
}

//
inline void TComboBoxEx::DeleteItem (DELETEITEMSTRUCT & deleteInfo){
  DefaultProcessing();
}

//
inline TResult TComboBoxEx::EvCompareItem(uint, COMPAREITEMSTRUCT&){
  return DefaultProcessing();
}

//
inline void TComboBoxEx::EvDeleteItem(uint, DELETEITEMSTRUCT&){
  DefaultProcessing();
}

//
inline void TComboBoxEx::EvDrawItem(uint, DRAWITEMSTRUCT&){
  DefaultProcessing();
}

//
inline void TComboBoxEx::EvMeasureItem(uint, MEASUREITEMSTRUCT&){
  DefaultProcessing();
}

//
inline void TComboBoxExData::Select(int index){
  SelIndex = index;
  Selection = SelIndex >= 0 ? GetItem(SelIndex).Text() : owl_string();
}

//
inline int TComboBoxExData::GetSelIndex(){
  return SelIndex;
}

//
inline owl_string& TComboBoxExData::GetSelection(){
  return Selection;
}

//
inline int TComboBoxExData::GetSelCount() const{
  return SelIndex == CB_ERR ? 0 : 1;
}

//
inline void TComboBoxExData::ResetSelections(){
  SelIndex = CB_ERR;
  Selection = _T("");
}

//
//
inline int TComboBoxExData::GetSelStringLength() const{
  return Selection.length();
}

//
//
inline void TComboBoxExData::GetSelString(LPTSTR buffer, int bufferSize) const{
  _tcsncpy(buffer, Selection.c_str(), bufferSize);
}


__OWL_END_NAMESPACE

#endif  // OWL_COMBOBEX_H
