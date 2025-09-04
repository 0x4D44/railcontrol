//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// TRegItem and TRegList members that need to be linked into user app and not
// the WinSys DLL
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_REGISTRY_H)
#include <owl/registry.h>
#endif
#include <stdio.h>   // sprintf()

__OWL_BEGIN_NAMESPACE

//
// Initialize the data members to 0.
//
TRegFormatHeap::TRegFormatHeap()
:
  Head(0)
{
}

//
// Walk the heap chain & delete the memory blocks
//
TRegFormatHeap::~TRegFormatHeap()
{
  while (Head) {
    TBlock* next = Head->Next;
    delete[] (_TCHAR*)Head;
    Head = next;
  }
}

//
// Allocate a block of memory of a given size & link it into the heap chain
//
LPTSTR TRegFormatHeap::Alloc(int spaceNeeded)
{
  TBlock* newblock = (TBlock*) new _TCHAR[sizeof(TBlock) + spaceNeeded];
  newblock->Next = Head;
  Head = newblock;

  return newblock->Data;
}

//
// Lookup the key and return the associated value.
// Returns an empty string if there is no associated value.
// If the key does not exist, it returns a null pointer.
//
LPCTSTR TRegList::Lookup(LPCSTR key, TLangId lang)
{
  if (key) {
    for (TRegItem* regItem = Items; regItem->Key != 0; regItem++) {
      if (strcmp(regItem->Key, key) == 0)
        if (regItem->Value.Private)  // current can't test Value directly
          return regItem->Value.Translate(lang);
        else
          return _T("");
    }
  }
  return 0;
}

//
//
// Return a reference to the locale string object associated with the key.
//
TLocaleString& TRegList::LookupRef(LPCSTR key)
{
  for (TRegItem* regItem = Items; regItem->Key != 0; regItem++) {
    if (strcmp(regItem->Key, key) == 0)
       return regItem->Value;
  }
  return TLocaleString::Null;
}

//
// Maximum string length for REGFORMAT w/ string arg. String is clipped if too
// long.
//
const int MaxFormatLen = 40;

//
// Register data formats for the object.
//
//
LPTSTR TRegItem::RegFormat(int f, int a, int t, int d, TRegFormatHeap& heap)
{
  // sprintf into sized auto buffer
  // ints have a max of 11 digits: -2000000000. Add pad of 8 just in case
  //
  TCHAR temp[11+1+11+1+11+1+11+1+8];
  int len = _stprintf(temp, _T("%d,%d,%d,%d"), f, a, t, d);

  // Copy into real heap buffer & return it
  //
  return ::_tcscpy(heap.Alloc(len + 1), temp);
}

//
//
// Register data formats for the object.
//
LPTSTR TRegItem::RegFormat(LPCTSTR f, int a, int t, int d, TRegFormatHeap& heap)
{
  // _stprintf into sized auto buffer
  //
  TCHAR temp[MaxFormatLen+1+11+1+11+1+11+1+8];
  int len = _stprintf(temp, _T("%.*s,%d,%d,%d"), MaxFormatLen, (LPTSTR)f, a, t, d);

  // Copy into real heap buffer & return it
  //
  return ::_tcscpy(heap.Alloc(len + 1), temp);
}

//
// Register the flag.
//
//
LPTSTR TRegItem::RegFlags(long flags, TRegFormatHeap& heap)
{
  // _stprintf into sized auto buffer
  //
  TCHAR temp[11+1+8];
  int len = _stprintf(temp, _T("%ld"), flags);

  // Copy into real heap buffer & return it
  //
  return ::_tcscpy(heap.Alloc(len + 1), temp);
}

//
// Register the verb option.
//
//
LPTSTR TRegItem::RegVerbOpt(int mf, int sf, TRegFormatHeap& heap)
{
  // _stprintf into sized auto buffer
  //
  TCHAR temp[11+1+11+1+8];
  int len = _stprintf(temp, _T("%d,%d"), mf, sf);

  // Copy into real heap buffer & return it
  //
  return ::_tcscpy(heap.Alloc(len + 1), temp);
}

__OWL_END_NAMESPACE
/* ========================================================================== */
