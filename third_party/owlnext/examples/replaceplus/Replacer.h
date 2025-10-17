#ifndef REPLACER_H
#define REPLACER_H

#include <owl/wsyscls.h>
#include <owl/window.h>

//
// This struct represents the parameters of the replace command.
// It may be set by the command line or by a user dialog.
//
struct TReplaceData
{
  int Action; // 0 = find-and-replace, 1 = touch. TODO: Use an enumeration type.
  owl::tstring Folder;
  owl::tstring Filter;
  owl::tstring SearchTerm;
  owl::tstring Replacement;
  bool RecurseFlag;
  owl::TSystemTime Time; // TODO: Use a single member of type TFileTime.
  owl::TSystemTime Date;

  TReplaceData() 
    : Action(0), Folder(), Filter(), SearchTerm(), Replacement(), RecurseFlag(false), Time(), Date() 
  {}
};

bool PerformCommand(const TReplaceData& d, owl::TWindow* resultParent = 0);

#endif