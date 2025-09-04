//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:49 $
//
// Implementation of classes TView & TWindowView
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_DOCMANAG_H)
# include <owl/docmanag.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_BARDESCR_H)
# include <owl/bardescr.h>
#endif
#if !defined(OWL_DOCVIEW_RH)
# include <owl/docview.rh>
#endif
//#include <string.h>
//#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlDocView);        // General Doc/View diagnostic group

#if !defined(SECTION) || SECTION == 1

const uint MinUniqueViewId = 0x8000;
uint TView::NextViewId = MinUniqueViewId;

//
//
//
TView::TView(TDocument& doc)
:
  Tag(0),
  ViewMenu(0),
  ViewBar(0)
{
  ViewId = NextViewId;
  doc.AttachView(*this);
}

//
//
//
TView::~TView()
{
  delete ViewMenu;
  delete ViewBar;

  CHECK(Doc);
  if (Doc->DetachView(*this)) {
    delete Doc;
  }
}

//
// Detach the view from the current document and attach it to another. 
// (Added by Vidar Hasfjord, 2007-08-27.)
//
void 
TView::SetDocument(TDocument& new_doc)
{
  if (&new_doc == Doc) return; // Guard against reassignment.

  // Detach and register whether the document should be deleted.
  // The actual deletion is postponed to later to avoid any side-effects 
  // from the deletion that may affect the validity of the new document.

  TDocument* delete_doc = 0;
  if (Doc && Doc->DetachView(*this))
    delete_doc = Doc;

  // Update the new document and existing views as if this was a new view.

  new_doc.AttachView(*this);
  CHECK(Doc == &new_doc);
  Doc->ReindexFrames();

  // Deletion of the old doc should be safe now.

  if (delete_doc)
    delete delete_doc;
}

//
//
//
void
TView::SetViewMenu(TMenuDescr* menu)
{
  delete ViewMenu;
  ViewMenu = menu;
  TDocTemplate* tpl = Doc->GetTemplate();

  //if (tpl && ViewMenu && *ViewMenu->GetModule() == *tpl->GetModule())
  // must check also if template is static becouse as module = 0;
  if (tpl && ViewMenu && !tpl->IsStatic() && *ViewMenu->GetModule() == *tpl->GetModule())
    ViewMenu->SetModule(tpl->GetModule());// force same module alias as template
}
//
void
TView::SetViewBar(TBarDescr* bar)
{
  delete ViewBar;
  ViewBar = bar;
}

static _TCHAR* TView_PropertyNames[] = {
  _T("View Class"),      // ViewClass
  _T("View Name"),       // ViewName
};

static int TView_PropertyFlags[] = {
  pfGetText|pfConstant,  // ViewClass
  pfGetText|pfConstant,  // ViewName
};

//
//
//
const _TCHAR*
TView::PropertyName(int index)
{
  if (index <= PrevProperty) {
    TRACEX(OwlDocView, 0, _T("PropertyName(): index <= PrevProperty!"));
    return 0;
  }
  else if (index < NextProperty)
    return TView_PropertyNames[index-PrevProperty-1];
  else {
    TRACEX(OwlDocView, 0, _T("PropertyName(): index >= NextProperty!"));
    return 0;
  }
}

//
//
//
int
TView::PropertyFlags(int index)
{
  if (index <= PrevProperty) {
    TRACEX(OwlDocView, 0, _T("PropertyFlags(): index <= PrevProperty!"));
    return 0;
  }
  else if (index < NextProperty)
    return TView_PropertyFlags[index-PrevProperty-1];
  else {
    TRACEX(OwlDocView, 0, _T("PropertyFlags(): index >= NextProperty!"));
    return 0;
  }
}

//
//
//
int
TView::FindProperty(LPCTSTR name)
{
  int i;
  for (i=0; i < NextProperty-PrevProperty-1; i++)
    if (_tcscmp(TView_PropertyNames[i], name) == 0)
      return i+PrevProperty+1;

  TRACEX(OwlDocView, 0, _T("FindProperty(): ") \
        _T("index of [") << owl_string(name).c_str() << _T("] not found") );
  return 0;
}
//
//
//
int
TView::GetProperty(int prop, void * dest, int textlen)
{
  LPCTSTR src;
  switch (prop) {

    case ViewClass:{
        _USES_CONVERSION;
        src = _A2W(_OBJ_FULLTYPENAME(this));
      }
      break;

    case ViewName:
      src = GetViewName();
      break;

    default:
      TRACEX(OwlDocView, 0, _T("GetProperty(): ") \
            _T("invalid property [") << prop << _T("] specified!") );
      return 0;
  }

  if (!textlen) {
    TRACEX(OwlDocView, 0, _T("GetProperty(): 0-Length buffer specified!"));
    return 0;
  }

  int srclen = src ? ::_tcslen(src) : 0;
  if (textlen > srclen)
    textlen = srclen;
  if (textlen)
    memcpy(dest, src, textlen*sizeof(_TCHAR));
  *((_TCHAR *)dest + textlen) = 0;
  return srclen;
}

//
// Increment an internal count used by the Doc/View subsystem to identify
// each view.
//
void
TView::BumpNextViewId()
{
  if (++NextViewId < MinUniqueViewId)
    NextViewId = MinUniqueViewId;
}

IMPLEMENT_ABSTRACT_STREAMABLE(TView);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TView::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TView* o = GetObject();
  bool hasViewMenu = is.readByte();
  if (hasViewMenu) {
    o->ViewMenu = new TMenuDescr;
    is >> *o->ViewMenu;
  }
  else
    o->ViewMenu = 0;
  o->ViewBar = 0;
  is >> o->ViewId;
  is >> o->Doc;
  is >> o->NextView;
  return o;
}

//
//
//
void
TView::Streamer::Write(opstream& os) const
{
  TView* o = GetObject();
  os.writeByte(uint8(o->ViewMenu ? 1 : 0));
  if (o->ViewMenu)
    os << *o->ViewMenu;
  os << o->ViewId;
  os << o->Doc;
  os << o->NextView;
}
#endif // BI_NO_OBJ_STREAMING

#endif // #if !defined(SECTION) || SECTION == 1

#if !defined(SECTION) || SECTION == 2
//----------------------------------------------------------------------------
// TWindowView Implementation
//

DEFINE_RESPONSE_TABLE1(TWindowView, TWindow)
  EV_VN_ISWINDOW,
END_RESPONSE_TABLE;

//
//
//
TWindowView::TWindowView(TDocument& doc, TWindow* parent)
:
  TWindow(parent, 0, doc.GetDocManager().GetApplication()),
  TView(doc)
{
}
//
IMPLEMENT_STREAMABLE2(TWindowView, TWindow, TView);

#if !defined(BI_NO_OBJ_STREAMING)
//
//
//
void*
TWindowView::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TWindow*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TWindowView::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TWindow*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
}
#endif // BI_NO_OBJ_STREAMING

#endif // #if !defined(SECTION) || SECTION == 2

#if !defined(SECTION) || SECTION == 3
//----------------------------------------------------------------------------
// TDialogView Implementation
//

DEFINE_RESPONSE_TABLE1(TDialogView, TDialog)
  EV_VN_ISWINDOW,
END_RESPONSE_TABLE;

//
//
//
TDialogView::TDialogView(TDocument& doc, TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module),
  TView(doc)
{
}
//
TDialogView::TDialogView(TDocument& doc, TWindow* parent, const DLGTEMPLATE& dlgTemplate, 
                         TAutoDelete del, TModule* module)
:
  TDialog(parent, dlgTemplate, del, module),
  TView(doc)
{
}
//
TDialogView::TDialogView(TDocument& doc, TWindow* parent, TModule* module, 
                         HGLOBAL hTemplate, TAutoDelete del)
:
  TDialog(hTemplate, parent, del, module),
  TView(doc)
{
}
//
IMPLEMENT_STREAMABLE2(TDialogView, TDialog, TView);

#if !defined(BI_NO_OBJ_STREAMING)
//
//
//
void*
TDialogView::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TDialog*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TDialogView::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TDialog*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
}

#endif  // BI_NO_OBJ_STREAMING

#endif // #if !defined(SECTION) || SECTION == 3
//----------------------------------------------------------------------------

__OWL_END_NAMESPACE
/* ========================================================================== */

