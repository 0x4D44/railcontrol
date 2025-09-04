//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//
// Definition of TRecentFiles class
//----------------------------------------------------------------------------

#if !defined(OWL_RECENTFILE_H)
#define OWL_RECENTFILE_H

#if !defined(OWL_EVENTHAN_H)
# include <owl/eventhan.h>
#endif
#if !defined(OWL_FILENAME_H)
#include <owl/filename.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

//
// Derived classes should catch the following registered message to know
// when an item in the recent list has been selected.
//
#define MruFileMessage _T("MRUFILEMESSAGE")

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TRecentFiles
// ~~~~~ ~~~~~~~~~~~~
// This class implements a most-recent files list, designed to be mixed-in with
// TApplication.
// The list is appended to the menu with CM_EXIT options.
//
class _OWLCLASS TRecentFiles : virtual public TEventHandler
#if defined (BI_NO_RTTI)
  , virtual public TStreamableBase
#endif
{
  public:
    enum { MaxMenuItems = 10 };

    TRecentFiles(LPCTSTR iniOrKeyName, int numSavedFiles = MaxMenuItems,
                 int namelen = 30, bool useRegistry=false);
    virtual ~TRecentFiles();

    void          SaveMenuChoice(LPCTSTR text);
    void          RemoveMenuChoice(LPCTSTR text);
    bool          GetMenuText(int id, LPTSTR text, int maxTextLen);
    owl_string    GetMenuText(int id);
    void          SetMaxMruItems(int max);
    void           SetDispLength(const int);
    void           EnableRegistry(bool enable = true);
    int           GetMruCount();

  protected:
    void          CeExit(TCommandEnabler& ce);
    void          CmFile(uint id);

    // Do the MRU items already exists in the menu?
    //
    bool          MruItemsInsertedIntoMenu(HMENU hMenu);
    void          RemoveMruItemsFromMenu(HMENU hMenu);
    void          InsertMruItemsToMenu(HMENU hMenu);
    int           GetMenuPos(HMENU hMenu, uint id);
    int           GetExitMenuPos(HMENU hMenu);

    // Profile methods
    //
    //int           GetMruCount();
    void          RemoveMruIndex(int index);
    void          AddMruItem(LPCTSTR text);
    bool          ExistMruItem(LPCTSTR text);
    int           GetMruItemIndex(LPCTSTR text);

  private:
    // Read information from the .INI file or Registry
    //
    void          Read();
    // Write information from the .INI file or Registry
    //
    void          Write();

    static uint           MruMessage;
    int                   MaxFilesToSave;
    int                    MaxDispLen;
    bool                  AddedSeparator;
    bool                  UseRegistry;
    owl_string             MruName;
    HMENU                 LastHMenu;
    TAPointer<TFileName>   MruNames;
    int                   MruCount;

  DECLARE_RESPONSE_TABLE(TRecentFiles);
  DECLARE_CASTABLE;
};
// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//-------------------
// inlines
inline void  TRecentFiles::SetDispLength(const int newLen){
  MaxDispLen = newLen;
}
inline void TRecentFiles::EnableRegistry(bool enable){
  UseRegistry = enable;
}
inline int TRecentFiles::GetMruCount(){
  return MruCount;
}

__OWL_END_NAMESPACE


#endif  // OWL_RECENTFILE_H
