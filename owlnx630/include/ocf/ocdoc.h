//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.13 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:31 $
//
// Definition of TOcDocument Class
//----------------------------------------------------------------------------

#if !defined(OCF_OCDOC_H)
#define OCF_OCDOC_H

#if !defined(OCF_OCLINK_H)
# include <ocf/oclink.h>
#endif

#if !defined(OCF_OCPART_H)
# include <ocf/ocpart.h>
#endif


#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//
// Interfaces & Classes referenced
//
interface _ICLASS IStorage;

__OCF_BEGIN_NAMESPACE

//
// Interfaces & Classes referenced
//
class _ICLASS TOcStorage;
class _ICLASS TOcApp;
class _ICLASS TOcControl;
class _ICLASS TOcView;
class _ICLASS TOcRemVie;

//
// class TOcDocument
// ~~~~~ ~~~~~~~~~~~
// OC Document class, holds parts & is a owner of views
//
class _ICLASS TOcDocument {
  public:
    TOcDocument(TOcApp& app, LPCTSTR fileName = 0);
    TOcDocument(TOcApp& app, LPCTSTR fileName, IStorage * storageI);
   ~TOcDocument();

    // Collection management
    //
    TOcPartCollection& GetParts() {return PartCollection;}
    TOcLinkCollection& GetLinks() {return LinkCollection;}
    TOcLinkCollection& GetViews() {return LinkCollection;} // Compatibility

    // Storage & streaming related
    //
    TOcStorage* GetStorage() {return Storage;}
    void        SetStorage(IStorage* storage, bool remember = true);
    void        SetStorage(LPCTSTR path);
    bool        SaveToFile(LPCTSTR newName);
    bool        RestoreStorage();

    // Load/Save part information
    //
    bool        LoadParts();
    bool        SaveParts(IStorage* storage = 0, bool sameAsLoaded = true,
                          bool remember = true);
    void        RenameParts(IBRootLinkable * bLDocumentI);
    void        Close();

    // Get/Set active view
    //
    TOcView*    GetActiveView() {return ActiveView;}
    void        SetActiveView(TOcView* view);

    // Get/Set document name
    //
    owl_string  GetName() const {return Name;}
    void        SetName(const owl_string& newName);

    int         AllocPartID() {return ++PartID;}
    TOcApp&     GetOcApp() {return OcApp;}

  private:
    TOcView*       ActiveView;   // Active TOcView object
    TOcApp&        OcApp;        // Our OC application object
    TOcStorage*    Storage;      // root storage for embedded objects
    TOcStorage*    OrgStorage;   // original root storage for embedded objects

    int            PartID;
    owl_string     Name;
    TOcPartCollection  PartCollection;
    TOcLinkCollection  LinkCollection;

  //friend class TOcPartCollectionIter;      // To allow iterator access to collection
  //friend class TOcLinkCollectionIter;      // To allow iterator access to collection
  friend class _ICLASS TOcControl;
  friend class _ICLASS TOcView;
  friend class _ICLASS TOcRemVie;
};

__OCF_END_NAMESPACE

#endif  // OCF_OCDOC_H

