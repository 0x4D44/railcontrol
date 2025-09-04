//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Definition of TOcPart class
//----------------------------------------------------------------------------

#if !defined(OCF_OCPART_H)
#define OCF_OCPART_H

#if !defined(OWL_GEOMETRY_H)
# include <owl/geometry.h>
#endif
#if !defined(OWL_CONTAIN_H)
# include <owl/contain.h>
#endif

#if !defined(OCF_OCBOCOLE_H)
# include <ocf/ocbocole.h>
#endif
#if !defined(OCF_OCOBJECT_H)
# include <ocf/ocobject.h>
#endif
#if !defined(OCF_AUTODEFS_H)
# include <ocf/autodefs.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OCF_BEGIN_NAMESPACE

//
// Classes referenced
//
class _ICLASS TOcStorage;
class _ICLASS TOcDocument;
class TOcVerb;

//
// class TOcPart
// ~~~~~ ~~~~~~~
// OC part class represents an embeded or linked part in a document
//
class _ICLASS TOcPart : public TUnknown, protected IBSite2 {
  public:
    TOcPart(TOcDocument& document, int id = 0);
    TOcPart(TOcDocument& document, LPCTSTR name);
    TOcPart(TOcDocument& document, TOcInitInfo & initInfo, __OWL TRect pos,
            int id = 0);

    virtual bool Init(TOcInitInfo * initInfo, __OWL TRect pos);

    void    Delete();

    bool    operator ==(const TOcPart& other) {return __OWL ToBool(&other == this);}

    // Delegated doc functions from TOcDocument
    //
    IBPart*     GetBPartI()  {return BPartI;}
    TOcStorage* GetStorage() {return Storage;}
    void        SetStorage(IStorage* storage, bool remember = true);
    bool    Save(bool SameAsLoad = true, bool remember = true);
    bool    Load();
    int     Detach();
    void    FinishLoading();

    // Position & size information
    //
    __OWL TPoint  GetPos() const {return Pos;}
    void    SetPos(const __OWL TPoint& pos);
    __OWL TSize   GetSize() const;
    void    SetSize(const __OWL TSize& size);
    __OWL TRect   GetRect() const {return __OWL TRect(Pos, Size);}
    void    UpdateRect();
    bool    IsVisible(const __OWL TRect& logicalRect) const
              {return __OWL ToBool(IsVisible() && logicalRect.Touches(GetRect()));}

    // Flag accessor functions
    //
    void    Select(bool select)
              {if (select) Flags |= Selected; else Flags &= ~Selected;}
    bool    IsSelected() const {return __OWL ToBool(Flags&Selected);}
    void    SetVisible(bool visible);
    bool    IsVisible() const {return __OWL ToBool(Flags&Visible);}
    bool    IsActive() const {return __OWL ToBool(Flags&Active);}
    void    SetActive() {Flags |= Active;}
    bool    IsLink() const {return __OWL ToBool(Flags&Link);}

    // Naming
    //
    // Changed the following line as there seems to be a problem with the 
    // destruction of TString objects when they are constructed from an 
    // owl_string. This causes an exception when the TString returned from 
    // GetName() goes out of scope. MH - 19/06/02
    //
    // TString GetName() {return Name;}
    __OWL TString GetName() {return Name.c_str();}
    void    Rename();

    // Pass thru functions to corresponding IBPart
    //
    bool    Save(IStorage* storage, bool sameAsLoad, bool remember);
    bool    Draw(HDC dc, const __OWL TRect& pos, const __OWL TRect& clip, TOcAspect aspect = asDefault);
    bool    Activate(bool activate);
    bool    Show(bool show) {return HRSucceeded(BPartI->Show(show));}
    bool    Open(bool open) {return HRSucceeded(BPartI->Open(open));}
    HWND    OpenInPlace(HWND hwnd) {return BPartI->OpenInPlace(hwnd);}
    bool    CanOpenInPlace() {return HRSucceeded(BPartI->CanOpenInPlace());}
    bool    Close();
    bool    SetHost(IBContainer * container)
              {return HRSucceeded(BPartI->SetHost(container));}

    bool    EnumVerbs(const TOcVerb&);
    bool    DoVerb(uint);
    LPCOLESTR    GetServerName(TOcPartName partName)
        {return const_cast<LPOLESTR>(BPartI->GetName((TOcPartName)partName));}
    HRESULT QueryServer(const IID & iid, void * * iface);

    bool    SetFormatData(TOcFormatInfo * fmt, HANDLE data, bool release);

    // Object reference & lifetime managment
    // For internal OCF use only
    //
    ulong   _IFUNC AddRef() {return GetOuter()->AddRef();}
    ulong   _IFUNC Release() {return GetOuter()->Release();}
    HRESULT _IFUNC QueryInterface(const GUID & iid, void ** iface)
                     {return GetOuter()->QueryInterface(iid, iface);}

  protected:
   ~TOcPart();

    virtual bool InitObj (TOcInitInfo * initInfo);

    // TUnknown virtual overrides
    //
    HRESULT      QueryObject(const IID & iid, void * * iface);

    // IBSite implementation for BOle to use
    //
    HRESULT   _IFUNC SiteShow(BOOL);
    HRESULT   _IFUNC DiscardUndo();
    HRESULT   _IFUNC GetSiteRect(__OWL TRect *, __OWL TRect *);
    HRESULT   _IFUNC SetSiteRect(const __OWL TRect *);
    HRESULT   _IFUNC SetSiteExtent(const __OWL TSize *);
    HRESULT   _IFUNC GetZoom(TOcScaleInfo * scale);
    void      _IFUNC Invalidate(TOcInvalidate);
    void      _IFUNC OnSetFocus(BOOL set);
    HRESULT   _IFUNC Init(IBDataProvider *, IBPart *, LPCOLESTR, BOOL);
    void      _IFUNC Disconnect();

    // IBSite2 implementation for BOle to use
    //
    HRESULT   _IFUNC GetParentWindow(HWND *);

  protected:
    IUnknown*			BPart;      // Our corresponding helper object & interfaces
    IBPart*				BPartI;
    IBLinkable*		BLPartI;

    TOcDocument&	OcDocument; // The OC document we are imbeded in
    TOcStorage*		Storage;    // The storage we are in

		owl_string    Name;
    __OWL TPoint				Pos;
    __OWL TSize					Size;
    enum TFlag { Visible=0x01, Selected=0x02, Active=0x04, Link=0x08,
                 Closing=0x10, OcxDefault=0x20, OcxCancel=0x40 };
    uint16       Flags;      // Is this part Selected/Visible/Active/ALink?

  friend class TOcPartCollection;
};

//
// class TOcPartCollection
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Container of parts with iterator
//
class _OCFCLASS TOcPartCollection : public __OWL TPtrArray<TOcPart*> {
  private:
  	typedef __OWL TPtrArray<TOcPart*> Base;
  public:
    TOcPartCollection();
   ~TOcPartCollection();

    void 	Clear();
    int 	Add(TOcPart* const& part)
              { return Base::Add(part); }
    int IsEmpty() const {return Base::Empty();}
    int Find(TOcPart* const& part)
             { return Base::Find(part);}
    virtual unsigned Count() const {return Base::Size();}
    int Detach(TOcPart* const& part, int del = 0);

    TOcPart*   Locate(__OWL TPoint& point);
    bool       SelectAll(bool select = false);
};

//
// class TOcPartCollectionIter
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~
typedef TOcPartCollection::Iterator TOcPartCollectionIter;

//
// class TOcVerb
// ~~~~~ ~~~~~~~
class _OCFCLASS TOcVerb {
  public:
    TOcVerb();

  public:
    LPCOLESTR   TypeName;
    LPCOLESTR   VerbName;
    uint        VerbIndex;
    bool        CanDirty;
};

//
// class TOcPartChangeInfo
// ~~~~~ ~~~~~~~~~~~~~~~~~
// View/Data change info
//
class _OCFCLASS TOcPartChangeInfo {
  public:
    TOcPartChangeInfo(TOcPart* part, TOcInvalidate type = invView)
      : Part(part), Type(type) {}

    bool     IsDataChange() {return Type & invData;}
    bool     IsViewChange() {return Type & invView;}

    void     SetDataChange() {Type |= invData;}
    void     SetViewChange() {Type |= invView;}

    TOcPart* GetPart() {return Part;}
    void     SetPart(TOcPart* part) {Part = part;}

    int      GetType() {return Type;}

  protected:
    int         Type;  // what type of change
    TOcPart*    Part;
};


__OCF_END_NAMESPACE

#endif  // OCF_OCPART_H
