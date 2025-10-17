#pragma once
#include <owl/draglist.h>
#include <owl/radiobut.h>

class TExampleDragList : public owl::TDragList
{
public:
  // Constructors
  //
  TExampleDragList(owl::TWindow* parent, int id, int x, int y, int w, int h,
    owl::TModule* module = 0)
    :
  owl::TDragList(parent, id, x, y, w, h, module)
  {
  Attr.Style &= ~LBS_SORT;
  Copy = true;
  }

  TExampleDragList(owl::TWindow* parent, int resourceId, owl::TModule* module = 0)
  :
    owl::TDragList(parent, resourceId, module)
  {
    Copy = true;
  }

  bool BeginDrag(int item, const owl::TPoint&);
  TCursorType Dragging(int item, const owl::TPoint&);
  void Dropped(int item, const owl::TPoint& point);
  void CancelDrag(int item, const owl::TPoint&);
  void SetCopy(bool newValue)
  {
    Copy = newValue;
  }

private:
  int StartItem;
  bool Copy;
};

class TDragListWindow : public owl::TWindow, public owl::TDragListEventHandler {
public:
  TDragListWindow();
  void SetupWindow();
  // EV_COMMAND_AND_ID function- void ButtonClicked(WPARAM id);
  void CopyClicked();
  void MoveClicked();

private:
  TExampleDragList* DragList1;
  TExampleDragList* DragList2;
  owl::TRadioButton* CopyButton;
  owl::TRadioButton* MoveButton;

  DECLARE_RESPONSE_TABLE(TDragListWindow);
};
