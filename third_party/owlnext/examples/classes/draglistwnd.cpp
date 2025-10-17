#include "pch.h"
#include "draglistwnd.h"

#include <owl/static.h>

using namespace owl;

const int DragListId1 = 100;
const int DragListId2 = 101;
const int CopyButtonId = 102;
const int MoveButtonId = 103;
static TStatic* Status;
static tchar Buffer[512];


bool
TExampleDragList::BeginDrag(int item, const TPoint&)
{
  StartItem = item;

  if (StartItem == 0)
    return false;
  wsprintf(::Buffer, _T("Start item %d"), StartItem);

  if (::Status)
    ::Status->SetText(::Buffer);
  return true;
}

TDragList::TCursorType
TExampleDragList::Dragging(int item, const TPoint& point)
{
  HWND hwnd = WindowFromPoint(point);
  if (hwnd == *(Parent->ChildWithId(DragListId1)) ||
    hwnd == *(Parent->ChildWithId(DragListId2))) {
    if (hwnd != *this) {
      if (Copy)
        wsprintf(::Buffer, _T("Copying to top of other ListBox"));
      else
        wsprintf(::Buffer, _T("Move to top of other ListBox"));
    }
    else if (item == 0) {
      wsprintf(::Buffer, _T("Cannot insert new item here"));
    }
    else {
      if (item == -1)
        wsprintf(::Buffer, _T("Move to end of list"));
      else
        wsprintf(::Buffer, _T("Moving to item %d"), item);
    }

    if (::Status)
      ::Status->SetText(::Buffer);

    if (Copy && (hwnd != *this))
      return TDragList::dlCopy;
    else if (item == 0)
      return TDragList::dlStop;
    else
      return TDragList::dlMove;
  }
  wsprintf(::Buffer, _T("Cancel"));
  if (::Status)
    ::Status->SetText(::Buffer);
  return TDragList::dlMove;
}

void
TExampleDragList::Dropped(int item, const TPoint& point)
{
  if ((StartItem != 0) && (StartItem != item) && (item != 0)) {
    int len = GetStringLen(StartItem);
    tchar *moveItem = new tchar[len + 1];
    if (moveItem) {
      GetString(moveItem, StartItem);
      HWND hwnd = WindowFromPoint(point);
      if (hwnd == *(Parent->ChildWithId(DragListId1)) ||
        hwnd == *(Parent->ChildWithId(DragListId2))) {
        if (!Copy || (hwnd == *this))
          DeleteString(StartItem);
        if (hwnd == *this) {
          if (item != -1)
            wsprintf(::Buffer, _T("Dragged from %d to %d"), StartItem, item);
          else
            wsprintf(::Buffer, _T("Dragged to end of list"));
          InsertString(moveItem, item);
        }
        else {
          wsprintf(::Buffer, _T("Dragged from %d to the top of the other ListBox"), StartItem);
          ::SendMessage(hwnd, LB_INSERTSTRING, 1, (LPARAM)((LPSTR)moveItem));
        }
      }
    }
    if (::Status)
      ::Status->SetText(::Buffer);

    delete[] moveItem;
  }
}

void
TExampleDragList::CancelDrag(int, const TPoint&)
{
  wsprintf(::Buffer, _T("Cancelled drag"));

  if (::Status)
    ::Status->SetText(::Buffer);
}



DEFINE_RESPONSE_TABLE2(TDragListWindow, TWindow, TDragListEventHandler)
// EV_COMMAND_AND_ID(CopyButtonId,ButtonClicked),
// EV_COMMAND_AND_ID(MoveButtonId,ButtonClicked),
EV_BN_CLICKED(CopyButtonId, CopyClicked),
EV_BN_CLICKED(MoveButtonId, MoveClicked),
END_RESPONSE_TABLE;

TDragListWindow::TDragListWindow()
  :
  TWindow(0, 0, 0)
{
  ::Status = new TStatic(this, -1, _T(""), 10, 0, 410, 35);
  DragList1 = new TExampleDragList(this, DragListId1, 10, 60, 210, 160);
  DragList2 = new TExampleDragList(this, DragListId2, 230, 60, 210, 160);
  CopyButton = new TRadioButton(this, CopyButtonId, _T("Copy Entry"), 115, 235, 100, 20);
  MoveButton = new TRadioButton(this, MoveButtonId, _T("Move Entry"), 255, 235, 100, 20);
}

void
TDragListWindow::SetupWindow()
{
  TWindow::SetupWindow();
  CopyButton->SetCheck(BF_CHECKED);
  DragList1->SetCopy(true);
  DragList2->SetCopy(true);

  // Add items to listbox
  //
  DragList1->AddString(_T("Cannot be dragged"));
  DragList1->AddString(_T("Amber"));
  DragList1->AddString(_T("Annie"));
  DragList1->AddString(_T("Anh"));
  DragList1->AddString(_T("Della"));
  DragList1->AddString(_T("Doreen"));
  DragList1->AddString(_T("Elizabeth"));
  DragList1->AddString(_T("Eva"));
  DragList1->AddString(_T("Della"));
  DragList1->AddString(_T("Jean"));
  DragList1->AddString(_T("Josephine"));
  DragList1->AddString(_T("Maya"));
  DragList1->AddString(_T("Soyeun"));
  DragList1->AddString(_T("Sally"));
  DragList1->AddString(_T("Valerie"));

  DragList2->AddString(_T("Cannot be dragged"));
  DragList2->AddString(_T("Chris"));
  DragList2->AddString(_T("Tommy"));
  DragList2->AddString(_T("Bruneau"));
  DragList2->AddString(_T("Jason"));
  DragList2->AddString(_T("Dave"));
  DragList2->AddString(_T("Carl"));
}

/* EV_COMMAND_AND_ID function-
void
TDragListWindow::ButtonClicked(WPARAM id)
{
if(id== CopyButtonId) {
DragList1->SetCopy(true);
DragList2->SetCopy(true);
}
else {
DragList1->SetCopy(false);
DragList2->SetCopy(false);
}
}  */

void
TDragListWindow::CopyClicked()
{
  DragList1->SetCopy(true);
  DragList2->SetCopy(true);
}

void
TDragListWindow::MoveClicked()
{
  DragList1->SetCopy(false);
  DragList2->SetCopy(false);
}
