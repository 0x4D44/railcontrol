//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:32 $
//
// Definition of class TCheckList, an ownerdrawn listbox to select multiple
// items
// Portions Copyright (C) 1997, 98 by Dieter Windau from TCheckListBox class
// EMail: dieter.windau@kvwl.de
// Web:   http://www.members.aol.com/SoftEngage
//
// Added new functionality from V 1.1 class of Dieter Windau   --  07/22/1998
// Added Constructors to work with TCheckListItemArray         --  07/16/1998
// Merged with OWL class and revised by Yura Bidus             --  06/17/1998
//------------------------------------------------------------------------------

#if !defined(OWL_CHECKLST_H)
#define OWL_CHECKLST_H

#if !defined(OWL_LISTBOX_H)
# include <owl/listbox.h>
#endif
#if !defined(OWL_CHECKBOX_H)
# include <owl/checkbox.h>
#endif
#if !defined(OWL_TEMPLATE_H)
# include <owl/template.h>
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


const int CheckList_BoxWidth = 15;

//
//!Y.B Must be revised once
// Add stream support, Transfer support
//

//
// class TCheckListItem
// ~~~~~ ~~~~~~~~~~~~~~
// Each item displayed and manipulated by TCheckList.
//
class _OWLCLASS TCheckListItem {
  public:
    TCheckListItem();
    TCheckListItem(LPCTSTR text, uint state = BF_UNCHECKED, bool enabled = true);
    TCheckListItem(const TCheckListItem& item);

    TCheckListItem& operator =(const TCheckListItem& d);

    // State management
    //
    bool IsChecked() const;
    bool IsUnchecked() const;
    bool IsIndeterminate() const;
    bool IsEnabled() const;
    uint GetState() const;
    bool GetThreeStates() const;

    void Toggle();
    void Check();
    void Uncheck();
    void SetIndeterminate();
    void SetThreeStates(bool);
    void Enable();
    void Disable();

    // Text management
    //
    void SetText(LPCTSTR text);
    void GetText(LPTSTR buffer, int len);

    // Data management
    //
    void    SetData(uint32 data);
    uint32  GetData();

    // Comparison operators
    //
    bool operator ==(const TCheckListItem& d) const;
    bool operator !=(const TCheckListItem& d) const;
    bool operator < (const TCheckListItem& d) const;
    bool operator > (const TCheckListItem& d) const;

  private:
    owl_string  Text;
    uint        State;
    bool        HasThreeStates;
    bool        Enabled;
    uint32      UserData;

    friend class _OWLCLASS TCheckList;
    friend int SortCheckList(const void *a, const void *b);
};


#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class TCheckListArray: public TPtrArray<TCheckListItem*> {
  public:
    TCheckListArray(int upper, int lower=0, int delta=0)
      :TPtrArray<TCheckListItem*>(upper,lower,delta){}
    TCheckListArray():TPtrArray<TCheckListItem*>(){}
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif
typedef TCheckListArray::Iterator   TCheckListArrayIter;

//
// class TCheckList
// ~~~~~ ~~~~~~~~~~
// An owner drawn listbox to select multiple items.
//
class _OWLCLASS TCheckList : public TListBox {
  public:
    // The constructors expect to be passed an array of TCheckListItems.
    // The memory is owned by the callee.
    //
    TCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListItem* items=0, int numItems=0,
               TModule* module = 0);
    TCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListArray& items, TModule* module = 0);
    TCheckList(TWindow* parent, int resourceId,
               TCheckListItem* items=0, int numItems=0,
               TModule* module = 0);
    TCheckList(TWindow* parent, int resourceId, TCheckListArray& items,
               TModule* module = 0);
   ~TCheckList();

  public:
    // The memory is owned by the callee.
    //
    int   AddItem(TCheckListItem* item);
    int   InsertItem(TCheckListItem* item, int idx);
    TCheckListItem* DetachItem(int idx);
    TCheckListItem* GetItem(int idx);

    virtual void ClearList();

  protected:
    virtual void SetupWindow();
    virtual uint Transfer(void* buffer, TTransferDirection direction);

    // Owner draw messages
    //
    void ODAFocus(DRAWITEMSTRUCT & drawInfo);
    void ODASelect(DRAWITEMSTRUCT & drawInfo);
    void ODADrawEntire(DRAWITEMSTRUCT & drawInfo);
    void PaintItem(DRAWITEMSTRUCT & drawInfo);
    virtual void PaintText(TDC& dc, const TRect& textRect, LPCTSTR text);

    // Event handlers
    //
    void EvLButtonDown(uint modKeys, TPoint& point);
    void EvChar(uint key, uint repeatCount, uint flags);
    void Update();

  private:
    friend class TCheckListArray;
    TCheckListArray Items;

  DECLARE_RESPONSE_TABLE(TCheckList);
};

//
//
//
class _OWLCLASS TCheckListData{
  public:
    TCheckListData();
    ~TCheckListData();

    TCheckListItem* GetItem(int index);
    TIntArray&      GetSelIndices();
    
    int   AddString(LPCTSTR str, bool isSelected = false);
    int   AddStringItem(LPCTSTR str, uint32 itemData, bool isSelected = false);
    void Clear(bool del = true);

    void Select(int index);
    void SelectString(LPCTSTR str);
    int  GetSelCount() const;
    void ResetSelections();

    bool IsChecked(int index) const;
    bool IsUnchecked(int index) const;
    bool IsIndeterminate(int index) const;
    bool IsEnabled(int index) const;
    uint GetState(int index) const;
    bool GetThreeStates(int index) const;

    void Toggle(int index);
    void Check(int index);
    void Uncheck(int index);
    void SetIndeterminate(int index);
    void SetThreeStates(int index, bool enable = true);
    void Enable(int index);
    void Disable(int index);


    int AddItem(TCheckListItem* item);
    uint ItemCount();

  protected_data:
    TIPtrArray<TCheckListItem*> Items;       //
    TIntArray*                  SelIndices;  // Contains all selection indices
};

//
// class TTabCheckList
// ~~~~~ ~~~~~~~~~~~~~
//
class _OWLCLASS TTabCheckList : public TCheckList {
  public:
    // The constructors expect to be passed an array of TCheckListItems.
    // The memory is owned by the callee.
    //
    TTabCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListItem* items=0, int numItems=0,
               TModule* module = 0);
    TTabCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListArray& items, TModule* module = 0);
    TTabCheckList(TWindow* parent, int resourceId,
               TCheckListItem* items=0, int numItems=0,
               TModule* module = 0);
    TTabCheckList(TWindow* parent, int resourceId, TCheckListArray& items,
               TModule* module = 0);

    // Don't send the LB_SETTABSTOP, because the Text is ownerdrawn
    // If numTabs > 0 and tabs == 0 the function return false
    //
    bool SetTabStops(int numTabs=0, int * tabs=0);

  protected:
    void   SetupWindow();
    virtual void PaintText(TDC& dc, const TRect& textRect, LPCTSTR text);
    int   GetAverageCharWidths();

  protected:
    TIntArray TabStops;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline Implementations
//

inline void
TCheckList::PaintText(TDC& dc, const TRect& textRect, LPCTSTR text)
{
  dc.DrawText(text, -1, textRect, DT_SINGLELINE | DT_VCENTER);
}


//
// TTabCheckList
//

//
//
//
inline
TTabCheckList::TTabCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListItem* items, int numItems,
               TModule* module)
:
  TCheckList(parent,id,x,y,w,h,items,numItems,module)
{
  ModifyStyle(0,LBS_USETABSTOPS);
}

//
//
//
inline
TTabCheckList::TTabCheckList(TWindow* parent, int id, int x, int y, int w, int h,
               TCheckListArray& items, TModule* module)
:
  TCheckList(parent,id,x,y,w,h,items,module)
{
  ModifyStyle(0,LBS_USETABSTOPS);
}

//
//
//
inline
TTabCheckList::TTabCheckList(TWindow* parent, int resourceId,
               TCheckListItem* items, int numItems,
               TModule* module)
:
  TCheckList(parent,resourceId,items,numItems,module)
{
  ModifyStyle(0,LBS_USETABSTOPS);
}

//
//
//
inline
TTabCheckList::TTabCheckList(TWindow* parent, int resourceId,
                             TCheckListArray& items, TModule* module)
:
  TCheckList(parent,resourceId,items,module)
{
  ModifyStyle(0,LBS_USETABSTOPS);
}

//
// Initialize the state of TCheckListItem.
//
inline
TCheckListItem::TCheckListItem()
:
  State(BF_UNCHECKED),
  HasThreeStates(false),
  Enabled(true),
  UserData(0)
{
}

inline
TCheckListItem::TCheckListItem(const TCheckListItem& item)
:
  Text(item.Text),
  State(item.State),
  HasThreeStates(item.HasThreeStates),
  Enabled(item.Enabled),
  UserData(item.UserData)
{
}

//
// Construct the object with a text string and a starting state.
//
inline
TCheckListItem::TCheckListItem(LPCTSTR text, uint state, bool enabled)
:
  Text(_T("")),
  State(state),
  HasThreeStates(ToBool(state == BF_GRAYED)),
  Enabled(enabled),
  UserData(0)
{
  SetText(text);
}

//
// Comparison operators
//
inline bool
TCheckListItem::operator !=(const TCheckListItem& d) const
{
  return !(*this == d);
}

//
//
//
inline bool
TCheckListItem::operator < (const TCheckListItem& d) const
{
  return Text < d.Text;
}

//
// Copy the text string.
//
inline void
TCheckListItem::SetText(LPCTSTR text)
{
  Text = text;
}

//
//
//
inline bool
TCheckListItem::operator > (const TCheckListItem& d) const
{
  return Text > d.Text;
}

//
// Returns true if the item has been checked.
//
inline bool
TCheckListItem::IsChecked() const
{
  return ToBool(State == BF_CHECKED);
}

//
// Returns true if the item has been unchecked.
//
inline bool
TCheckListItem::IsUnchecked() const
{
  return ToBool(State == BF_UNCHECKED);
}

//
// Returns true if the item is in the indeterminate state.
//
inline bool
TCheckListItem::IsIndeterminate() const
{
  return ToBool(State == BF_GRAYED);
}

//
// Returns the button state: BF_CHECKED, BF_UNCHECKED or BF_GRAYED
//
inline uint
TCheckListItem::GetState() const
{
  return State;
}

//
// Returns true if the item has tree states.
//
inline bool
TCheckListItem::GetThreeStates() const
{
  return HasThreeStates;
}

//
// Returns true if the item is disabled.
//
inline bool
TCheckListItem::IsEnabled() const
{
  return Enabled;
}

//
// NEW: Programmatically disable the item.
//
inline void
TCheckListItem::Disable()
{
  Enabled = false;
}

//
// NEW: Programmatically enable the item.
//
inline void
TCheckListItem::Enable()
{
  Enabled = true;
}

//
//
//
inline void TCheckListItem::SetData(uint32 data)
{
  UserData = data;
}

//
//
//
inline uint32 TCheckListItem::GetData(){
  return UserData;
}

//
//
//
inline TCheckListItem* TCheckListData::GetItem(int index)
{ 
  PRECONDITION(index < (int)Items.size()); return Items[index]; 
}

//
//
//
inline TIntArray& TCheckListData::GetSelIndices()       
{ 
  return *SelIndices;
}

//
//
//
inline bool TCheckListData::IsChecked(int index) const       
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->IsChecked();        
}

//
//
//
inline bool TCheckListData::IsUnchecked(int index) const     
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->IsUnchecked();      
}

//
//
//
inline bool TCheckListData::IsIndeterminate(int index) const 
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->IsIndeterminate();  
}

//
//
//
inline bool TCheckListData::IsEnabled(int index) const       
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->IsEnabled();        
}

//
//
//
inline uint TCheckListData::GetState(int index) const        
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->GetState();         
}

//
//
//
inline bool TCheckListData::GetThreeStates(int index) const  
{ 
  PRECONDITION(index < (int)Items.size()); 
  return Items[index]->GetThreeStates();   
}

//
//
//
inline void TCheckListData::Toggle(int index)                
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->Toggle();                  
}

//
//
//
inline void TCheckListData::Check(int index)                 
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->Check();                   
}

//
//
//
inline void TCheckListData::Uncheck(int index)               
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->Uncheck();                 
}

//
//
//
inline void TCheckListData::SetIndeterminate(int index)      
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->SetIndeterminate();        
}

//
//
//
inline void TCheckListData::SetThreeStates(int index, bool enable)  
{
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->SetThreeStates(enable);
}

//
//
//
inline void TCheckListData::Enable(int index)                
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->Enable();                  
}

//
//
//
inline void TCheckListData::Disable(int index)               
{ 
  PRECONDITION(index < (int)Items.size()); 
  Items[index]->Disable();                 
}

//
//
//
inline int TCheckListData::AddItem(TCheckListItem* item)
{
  return Items.Add(item);
}
//
//
//
inline uint TCheckListData::ItemCount()
{
  return Items.size();
}

//
//
//

__OWL_END_NAMESPACE

#endif  // OWL_CHECKLIST_H
