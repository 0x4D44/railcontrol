//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.15 $
//$Author: jogybl $
//$Date: 2007-11-20 09:10:27 $
//
// Definition of TOcLinkView Class
//----------------------------------------------------------------------------

#if !defined(OCF_OCLINK_H)
#define OCF_OCLINK_H

#if !defined(OWL_GEOMETRY_H)
# include <owl/geometry.h>
#endif

#if !defined(OWL_CONTAIN_H)
# include <owl/contain.h>
#endif
#if !defined(OWL_TEMPLATE_H)
# include <owl/template.h>
#endif

#if !defined(OCF_OCBOCOLE_H)
# include <ocf/ocbocole.h>
#endif
#if !defined(OCF_OCREG_H)
# include <ocf/ocreg.h>
#endif
#if !defined(OCF_OCOBJECT_H)
# include <ocf/ocobject.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif



__OCF_BEGIN_NAMESPACE

class _ICLASS TOcView;

//
// class TOcLinkView
// ~~~~~ ~~~~~~~~~~~
// Link Viewer object for a server document
//
class _ICLASS TOcLinkView : public TUnknown,
//                            public IBPart2 {
                            public IBPart {
  public:
    TOcLinkView(TOcView* ocView, __OWL TRegList* regList = 0, IUnknown* outer = 0);
    int Detach();

    // IBSite pass-thrus
    //
    void    Invalidate(TOcInvalidate);
    void    Disconnect();

    // Misc status accessors, etc.
    //
    void     GetLinkRect();
    void     SetMoniker(LPCTSTR name);
    __OWL TString& GetMoniker() {return Moniker;};

    // Object reference & lifetime managment
    //
    ulong   _IFUNC AddRef() {return GetOuter()->AddRef();}
    ulong   _IFUNC Release() {return GetOuter()->Release();}
    HRESULT _IFUNC QueryInterface(const GUID & iid, void ** iface)
                     {return GetOuter()->QueryInterface(iid, iface);}

  protected:
    // TUnknown virtual overrides
    //
    HRESULT      QueryObject(const IID & iid, void * * iface);

    // IBDataNegotiator implementation
    //
    uint     _IFUNC CountFormats();
    HRESULT  _IFUNC GetFormat(uint index, TOcFormatInfo * fmt);

    // IBDataProvider2 implementation
    //
    HANDLE  _IFUNC  GetFormatData(TOcFormatInfo *);
    HRESULT _IFUNC  Draw(HDC, const RECTL *, const RECTL *, TOcAspect, TOcDraw bd);
    HRESULT _IFUNC  GetPartSize(__OWL TSize *);
    HRESULT _IFUNC  Save(IStorage*, BOOL sameAsLoad, BOOL remember);
//    HRESULT _IFUNC  SetFormatData(TOcFormatInfo * fmt, HANDLE data, bool release);

    // IBPart2 implementation
    //
    HRESULT _IFUNC  Init(IBSite *, TOcInitInfo *);
    HRESULT _IFUNC  Close();
    HRESULT _IFUNC  CanOpenInPlace();
    HRESULT _IFUNC  SetPartSize(__OWL TSize *);
    HRESULT _IFUNC  SetPartPos(__OWL TRect *);
    HRESULT _IFUNC  Activate(BOOL);
    HRESULT _IFUNC  Show(BOOL);
    HRESULT _IFUNC  Open(BOOL);
    HRESULT _IFUNC  EnumVerbs(TOcVerb *);
    HRESULT _IFUNC  DoVerb(uint);
    HWND    _IFUNC  OpenInPlace(HWND);
    HRESULT _IFUNC  InsertMenus(HMENU, TOcMenuWidths *);
    HRESULT _IFUNC  ShowTools(BOOL);
    void    _IFUNC  FrameResized(const __OWL TRect *, BOOL);
    HRESULT _IFUNC  DragFeedback(__OWL TPoint *, BOOL);
    HRESULT _IFUNC  GetPalette(LOGPALETTE * *);
    HRESULT _IFUNC  SetHost(IBContainer * objContainer);
    HRESULT _IFUNC  DoQueryInterface(const IID & iid, void * * pif);
    LPOLESTR _IFUNC GetName(TOcPartName);

    __OWL TPoint    Origin;
    __OWL TSize     Extent;

  private:
   ~TOcLinkView();

    TOcView*        OcView;          // Our hosting view that link is on
    IUnknown*       BSite;           // In-place site helper object
    IBSite*         BSiteI;          // Site interface
    IBApplication*  BAppI;           // Site's application interface

    __OWL TString					Moniker;

  friend class TOcLinkCollection;
};

//
// class TOcLinkCollection
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Container of link views with iterator
//
class _OCFCLASS TOcLinkCollection : public __OWL TPtrArray<TOcLinkView*> {
  private:
    typedef __OWL TPtrArray<TOcLinkView*> Base;
    //friend class TOcLinkCollectionIter;
  public:
    TOcLinkCollection();
   ~TOcLinkCollection();
    //void operator delete(void* ptr) {TStandardAllocator::operator delete(ptr);}

    void Clear();
    int Add(TOcLinkView* const& View)
            { return Base::Add(View);}
    int IsEmpty() const {return Base::Empty();}
    int Find(TOcLinkView* const& view)
            { return Base::Find(view);}
    virtual unsigned Count() const {return Base::Size();}
    int Detach(TOcLinkView* const& view, int del = 0);
    TOcLinkView* Find(__OWL TString const moniker) const;
};
typedef TOcLinkCollection TOcViewCollection;  // For compatibility only

// class TOcLinkCollectionIter
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~
// Iterator for view collection
typedef TOcLinkCollection::Iterator TOcLinkCollectionIter;
typedef TOcLinkCollectionIter TOcViewCollectionIter;  // For compatibility only

__OCF_END_NAMESPACE

#endif  // OCF_OCLINK_H
