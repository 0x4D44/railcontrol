//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TFileDocument
//----------------------------------------------------------------------------

#if !defined(OWL_FILEDOC_H)
#define OWL_FILEDOC_H

#if !defined(OWL_DOCVIEW_H)
# include <owl/docview.h>
#endif
#if !defined(BI_STD_IO)
#  if !defined(__IOSTREAM_H)
#   include <iostream.h>
#  endif
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

//
// class TFileDocument
// ~~~~~ ~~~~~~~~~~~~~
class _OWLCLASS TFileDocument : public TDocument {
  public:
    enum TFileDocProp {
      PrevProperty = TDocument::NextProperty-1,
      CreateTime,        // FILETIME
      ModifyTime,        // FILETIME
      AccessTime,        // FILETIME
      StorageSize,       // ulong
      FileHandle,        // platform file handle (HFILE if Windows)
      NextProperty,
    };

    TFileDocument(TDocument* parent = 0);
   ~TFileDocument();

    // Implement virtual methods of TDocument
    //
    bool        Open(int mode, LPCTSTR path=0);
    bool        Close();
    TInStream*  InStream(int mode, LPCTSTR strmId=0);
    TOutStream* OutStream(int mode, LPCTSTR strmId=0);
    bool        Commit(bool force = false);
    bool        Revert(bool clear = false);
    bool        IsOpen();

    int         FindProperty(LPCTSTR name);  // return index
    int         PropertyFlags(int index);
    LPCTSTR      PropertyName(int index);
    int         PropertyCount();
    int         GetProperty(int index, void * dest, int textlen=0);
    bool        SetProperty(int index, const void * src);

    // Additional methods for file document
    //
    bool        Open(HFILE fhdl);     // open on existing file handle

  protected:
    HFILE OpenThisFile(int omode, LPCTSTR name, NS_OWL_STREAMS::streampos* pseekpos);
    void  CloseThisFile(HFILE fhdl, int omode);

  protected_data:
    HFILE    FHdl;         // file handle if held open at the document level

  private:                 // cached info for property access
    bool     InfoPresent;
    ulong    FileLength;

    FILETIME FileCreateTime;
    FILETIME FileAccessTime;
    FILETIME FileUpdateTime;

  friend class _OWLCLASS TFileInStream;
  friend class _OWLCLASS TFileOutStream;
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TFileDocument,1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TFileDocument,1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TFileDocument );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline TFileDocument::TFileDocument(TDocument* parent)
:
  TDocument(parent), FHdl(HFILE_ERROR), InfoPresent(false)
{
}

//
inline TFileDocument::~TFileDocument()
{
}

//
inline bool TFileDocument::IsOpen()
{
  return FHdl != HFILE_ERROR || TDocument::IsOpen();
}

//
// Return the number of property support by this document.
// NOTE: The property count includes properties inherited from base document
//       classes.
//
inline int TFileDocument::PropertyCount()
{
  return NextProperty - 1;
}


__OWL_END_NAMESPACE


#endif  // OWL_FILEDOC_H
