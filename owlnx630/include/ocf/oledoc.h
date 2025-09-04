//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.13 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:47 $
//
//----------------------------------------------------------------------------

#if !defined(OCF_OLEDOC_H)
#define OCF_OLEDOC_H

#if !defined(OCF_STGDOC_H)
# include <ocf/stgdoc.h>
#endif
#if !defined(OCF_OCFFEVENT_H)
# include <ocf/ocfevent.h>
#endif
#if !defined(OCF_OCDOC_H)
# include <ocf/ocdoc.h>
#endif


#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OCF_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class _OCFCLASS TOleWindow;

//
// class TOleDocument
// ~~~~~ ~~~~~~~~~~~~
// The ole document class.
//
class _OCFCLASS TOleDocument : public TStorageDocument {
  public:
    TOleDocument(__OWL TDocument* parent = 0);
   ~TOleDocument();

    // Accessors
    //
    void          SetOcDoc(TOcDocument* doc);
    TOcDocument*  GetOcDoc();
    TOcApp*       GetOcApp();

    // Overridables:
    //   __OWL TDocument:
    //
    bool          Commit(bool force);
    bool          Open(int mode, LPCTSTR path = 0);
    bool          Close();
    bool          InitDoc();
    bool          CanClose();
    bool          Revert(bool clear);

    //   TStorageDocument:
    bool          SetStorage(IStorage* stg, bool remember = true);
    bool          RestoreStorage();
    bool          ReleaseDoc();

    //   TOleDocument:
    virtual void  PreOpen();
    virtual bool  Read();
    virtual bool  Write();
    virtual bool  CommitSelection(TOleWindow& oleWin, void* userData);

    // Utilities
    //
    bool PathChanged();
    void OleViewClose();

  private:
    TOcDocument*      OcDoc;    // OCF document partner
    bool              Closing;  // shutdown in progress
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline void TOleDocument::SetOcDoc(TOcDocument* doc)
{
  OcDoc = doc;
}

//
inline TOcDocument* TOleDocument::GetOcDoc()
{
  return OcDoc;
}

//
// Virtual function to be overriden in TOleDocument-derived class which
// serve or support linking to portions of a document's data.
//
inline bool TOleDocument::CommitSelection(TOleWindow& oleWin, void* userData)
{
  return true;
}


__OCF_END_NAMESPACE


#endif
