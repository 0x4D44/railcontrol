//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_REGISTRY_H)
#include <owl/registry.h>
#endif
#include <string.h>

__OWL_BEGIN_NAMESPACE

//
// Construct a reglink pointing to a reglist, and add to end of list
//
TRegLink::TRegLink(TRegList& regList, TRegLink*& head)
:
  Next(0),
  RegList(&regList)
{
  AddLink(&head, this);
}

//
// Add a new link to the end of the link list
//
void TRegLink::AddLink(TRegLink** head, TRegLink* newLink)
{
#if !defined(BI_VIRTUAL_BUG)
  TRegLink** link = head;
  while (*link)                 // put new link at end of list
    link = &(*link)->Next;
  *link = newLink;
#else
  if(*head == 0)
    *head = newLink;
  else{
    TRegLink* link = *head;
    while (link->GetNext() != 0)                 // put new link at end of list
      link = link->GetNext();// = &(*link)->Next;
    link->SetNext(newLink);
  }
#endif

}

//
// Remove a link from the link list. Return true if link found & removed
//
bool TRegLink::RemoveLink(TRegLink** head, TRegLink* remLink)
{
#if !defined(BI_VIRTUAL_BUG)
  for (TRegLink** link = head; *link; link = &(*link)->Next){
    if (*link == remLink) {
      *link = (*link)->Next;     // remove from list
      return true;
    }
  }
#else
  if(*head == remLink){
    (*head) = (*head)->GetNext();
    return true;
  }
    
  for( TRegLink* link = *head; link->GetNext(); link = link->GetNext()){
    if(link->GetNext() == remLink){
      link->SetNext(link->GetNext());// remove from list
      return true;
    }
  }
  
#endif
  return false;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
