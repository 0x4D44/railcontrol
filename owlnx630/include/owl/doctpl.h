//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.30 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TDocTemplate, and class template TDocTemplateT<D,V>
//----------------------------------------------------------------------------
#if !defined(OWL_DOCTPL_H)
#define OWL_DOCTPL_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_REGISTRY_H)
# include <owl/registry.h>
#endif
#if !defined(OWL_OBJSTRM_H)
# include <owl/objstrm.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


# define _DOCVIEWENTRY

//FMM Need it for gModule definition
#if defined(__HPUX_SOURCE)
#include <owl/module.h>
#include <owl/docview.h>
#endif

__OWL_DECLCLASS(TDocTemplate);


//
// Templates constructed before app get linked onto this head pointer
//
extern __OWL TDocTemplate* DocTemplateStaticHead;

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TDocManager;
class _OWLCLASS TModule;
class _OWLCLASS TDocument;
class _OWLCLASS TView;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TDocTemplate
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TDocTemplate : public TRegLink, public TStreamableBase {
  public:
    virtual TDocument*      ConstructDoc(TDocument* parent = 0) = 0;
    virtual TView*          ConstructView(TDocument& doc) = 0;
    virtual TDocument*      IsMyKindOfDoc(TDocument& doc) = 0;
    virtual TView*          IsMyKindOfView(TView& view) = 0;
    virtual LPCTSTR         GetViewName() = 0;

    bool                    IsVisible(); // displayable in file select dialogs
#if !defined (BI_GLOBALNM_BUG)
    virtual TDocTemplate*   Clone(TModule* module,
                                  TDocTemplate*& phead=::DocTemplateStaticHead)=0;
#else
    virtual TDocTemplate*   Clone(TModule* module,
                                  TDocTemplate*& phead=DocTemplateStaticHead)=0;
#endif

    TDocManager*            GetDocManager() const;
    void                    SetDocManager(TDocManager* dm);
    LPCTSTR                 GetDirectory() const;
    void                    SetDirectory(LPCTSTR);
    void                    SetDirectory(LPCTSTR, int len);
    TDocTemplate*           GetNextTemplate() const;
    bool                    IsFlagSet(long flag) const;
    long                    GetFlags() const;
    void                    SetFlag(long flag);
    void                    ClearFlag(long flag);
    bool                    IsStatic() const;
    TModule*&               GetModule();
    void                    SetModule(TModule* module);
    int                     GetRefCount() const;

    LPCTSTR                 GetFileFilter() const;
    LPCTSTR                 GetDescription() const;
    LPCTSTR                 GetDefaultExt() const;
    void                    SetFileFilter(LPCTSTR);
    void                    SetDescription(LPCTSTR);
    void                    SetDefaultExt(LPCTSTR);


  protected:                // Called only from parameterized derived class
    TDocTemplate(TRegList& regList, TModule*& module, TDocTemplate*& phead);
   ~TDocTemplate();

  private:
    _TCHAR *    Directory;     // Current directory path, 0 until changed
    long          Flags;         // dtXxx bits for dialogs and creation modes
    TDocManager*  DocManager;    // Pointer back to document manager
    int            RefCnt;        // Documents attached + 1 if attached to docmgr
    TModule**      ModulePtr;     // Would be TModule*&, except for streaming
    TModule*      ModuleDeref;   // Used only when setting a new module

#if defined(OWL2_COMPAT)
  // The following functions are maintained for backward compatability
  // with code generated for versions prior to enhanced document templates
  //
  public:
    virtual TDocument* CreateDoc(LPCTSTR path, long flags = 0) = 0;
    virtual TView*     CreateView(TDocument& doc, long flags = 0) = 0;
    TDocument*         InitDoc(TDocument* doc,LPCTSTR path,long flags);
    TView*             InitView(TView* view);
    bool               SelectSave(TDocument& doc);

//<Jogy
#endif
  protected:                // Called only from parameterized derived class
    TDocTemplate(const _TCHAR* desc, const _TCHAR* filt,
                 const _TCHAR* dir,  const _TCHAR* ext, long flags,
                 TModule*& module, TDocTemplate*& phead);
#if defined(OWL2_COMPAT)
//Jogy>

  private:
    static bool       _CALLCNVN (*SelectSave_)(TDocTemplate* tpl, TDocument& doc);
    static TView*     _CALLCNVN (*InitView_)(TView* view);
    static TDocument* _CALLCNVN (*InitDoc_)(TDocTemplate& tpl, TDocument* doc,
                                  LPCTSTR path, long flags);
#endif

  friend class TDocument;   // access to RefCnt
  friend class TDocManager;
  //DECLARE_ABSTRACT_STREAMABLE(_OWLCLASS, TDocTemplate, 1);
  DECLARE_ABSTRACT_STREAMABLE_OWL(__OWL TDocTemplate, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( __OWL TDocTemplate );

#define DECLARE_STREAMABLE_OPSDV( cls )                             \
static ipstream& readRef( ipstream& is, cls& cl );                  \
friend inline ipstream& operator >> ( ipstream& is, cls& cl )       \
    { return cls::readRef( is, cl ); }                              \
static ipstream& readPtr( ipstream& is, cls*& cl );                 \
friend inline ipstream& operator >> ( ipstream& is, cls*& cl )      \
    { return cls::readPtr( is, cl ); }                              \
static opstream& writeRef( opstream& is, const cls& cl );           \
friend inline opstream& operator << ( opstream& os, const cls& cl ) \
    { return cls::writeRef( os, cl ); }                             \
static opstream& writePtr( opstream& is, const cls* cl );           \
friend inline opstream& operator << ( opstream& os, const cls* cl ) \
    { return cls::writePtr( os, cl ); }

//
// class TDocTemplateT<D,V>
// ~~~~~ ~~~~~~~~~~~~~~~~~~
template<class D, class V>
class _DOCVIEWENTRY TDocTemplateT : public TDocTemplate {
  public:
    TDocTemplateT(TRegList& regList, TModule*& module = gModule,
                  TDocTemplate*& phead = DocTemplateStaticHead);
    TDocTemplateT(LPCTSTR desc, LPCTSTR filt,
                  LPCTSTR dir, LPCTSTR ext, long flags = 0,
                  TModule*& module = gModule,
                  TDocTemplate*& phead = DocTemplateStaticHead);
#if !defined(BI_NO_COVAR_RET)
#if !defined (BI_GLOBALNM_BUG)
    TDocTemplateT* Clone(TModule* module,
                         TDocTemplate*& phead=::DocTemplateStaticHead);
#else
    TDocTemplateT* Clone(TModule* module,
                         TDocTemplate*& phead=DocTemplateStaticHead);
#endif
    D* ConstructDoc(TDocument* parent = 0);
    V* ConstructView(TDocument& doc);
    D* IsMyKindOfDoc(TDocument& doc);  // Returns 0 if template can't support
    V* IsMyKindOfView(TView& view);    // Returns 0 if template incompatible
#else
#if !defined (BI_GLOBALNM_BUG)
    TDocTemplate* Clone(TModule* module,
                         TDocTemplate*& phead=::DocTemplateStaticHead);
#else
    TDocTemplate* Clone(TModule* module,
                         TDocTemplate*& phead=DocTemplateStaticHead);
#endif

    TDocument*  ConstructDoc(TDocument* parent = 0);
    TView*      ConstructView(TDocument& doc);
    TDocument*  IsMyKindOfDoc(TDocument& doc);  // Returns 0 if template can't support
    TView*      IsMyKindOfView(TView& view);    // Returns 0 if template incompatible
#endif
    virtual LPCTSTR GetViewName();

#if defined(OWL2_COMPAT)
    // The following functions are maintained for backward compatability
    //
#if !defined(BI_NO_COVAR_RET)
    D*          CreateDoc(LPCTSTR path, long flags = 0);
#else
    TDocument*  CreateDoc(LPCTSTR path, long flags = 0);
#endif
    TView* CreateView(TDocument& doc, long flags = 0);
#endif

#if !defined(BI_NO_OBJ_STREAMING)
  // Explicit expansion of DECLARE_STREAMABLE for use with parameterized class
  // DECLARE_STREAMABLE_FROM_BASE(TDocTemplateT<D,V>);
  //
  #define TEMPLATECLASS TDocTemplateT<D,V>
  DECLARE_CASTABLE;
  DECLARE_STREAMABLE_CTOR(TDocTemplateT);
  DECLARE_STREAMER_FROM_BASES(TEMPLATECLASS, TDocTemplate);
  DECLARE_STREAMABLE_OPSDV(TEMPLATECLASS);
  #undef TEMPLATECLASS
#endif
};

// define streameable inlines (VC)
//DECLARE_STREAMABLE_INLINESDV( D,V );
//DECLARE_STREAMABLE_INLINEST( TDocTemplateT );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Template flags used in reg parameter "docflags"
//

//
// Definitions of dtXxx document/view flags for templates and CreateDoc()
//
const long dtNewDoc     = 0x80000000L; // create new document, no path used
const long dtAutoDelete = 0x40000000L; // delete doc when last view is deleted
const long dtNoAutoView = 0x20000000L; // no automatic create of default view
const long dtSingleView = 0x10000000L; // only a single view per document
const long dtAutoOpen   = 0x08000000L; // open document upon creation
const long dtUpdateDir  = 0x04000000L; // syncronize directory with dialog dir
const long dtHidden     = 0x02000000L; // hide template from user selection
const long dtSelected   = 0x01000000L; // indicates template last selected
const long dtDynRegInfo = 0x00800000L; // reginfo table is dynamic, not static
const long dtSingleUse  = 0x00400000L; // to be registered as single use
const long dtRegisterExt= 0x00200000L; // register extension with this app
const long dtForbidNew  = 0x00100000L; // forbids creating new document

//
// Flags defined by Windows in <commdlg.h> - use the low order 17 bits
//
const long dtReadOnly       = 0x0001; //OFN_READONLY;         // initial check readonly box
const long dtOverwritePrompt= 0x0002; //OFN_OVERWRITEPROMPT;  // prompt before overwrite
const long dtHideReadOnly   = 0x0004; //OFN_HIDEREADONLY;     // no show readonly box
const long dtPathMustExist  = 0x0800; //OFN_PATHMUSTEXIST;    // path must be valid
const long dtFileMustExist  = 0x1000; //OFN_FILEMUSTEXIST;    // file must exist
const long dtCreatePrompt   = 0x2000; //OFN_CREATEPROMPT;     // prompt before create
const long dtNoReadOnly     = 0x8000; //OFN_NOREADONLYRETURN; // return writable file
const long dtNoTestCreate   = 0x10000L;//OFN_NOTESTFILECREATE; // skip create tests;


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline TDocManager* TDocTemplate::GetDocManager() const
{
  return DocManager;
}

//
inline void TDocTemplate::SetDocManager(TDocManager* dm)
{
  DocManager = dm;
}

inline TDocTemplate* TDocTemplate::GetNextTemplate() const 
{ 
  return (TDocTemplate*)Next;
}

//
inline bool TDocTemplate::IsFlagSet(long flag) const
{
  return (GetFlags() & flag) != 0;
}

//
inline long TDocTemplate::GetFlags() const
{
  return Flags;
}

//
inline bool TDocTemplate::IsStatic() const
{
  return (RefCnt & 0x8000) != 0;
}

//
inline TModule*& TDocTemplate::GetModule()
{
  return *ModulePtr;
}

//
// Return the number of reference count of the template.
// NOTE: The reference count of static templates has the high bit set.
//
inline int TDocTemplate::GetRefCount() const
{
  return RefCnt;
}

//
inline void TDocTemplate::SetModule(TModule* module)
{
  ModuleDeref = module;
  ModulePtr = &ModuleDeref;
}

//
inline bool
TDocTemplate::IsVisible()
{
  return (GetFlags() & dtHidden) == 0;
}

//
template<class D, class V> inline
TDocTemplateT<D,V>::TDocTemplateT(TRegList& regList, TModule*& module,
                                  TDocTemplate*& phead)
:
  TDocTemplate(regList, module, phead)
{
}

//
template<class D, class V> inline
TDocTemplateT<D,V>::TDocTemplateT(LPCTSTR desc, LPCTSTR filt,
                        LPCTSTR dir,  LPCTSTR ext, long flags,
                        TModule*& module, TDocTemplate*& phead)
:
  TDocTemplate(desc, filt, dir, ext, flags, module, phead)
{
}

//
#if !defined(BI_NO_COVAR_RET)
template<class D, class V> inline TDocTemplateT<D,V>*
#else
template<class D, class V> inline TDocTemplate*
#endif
TDocTemplateT<D,V>::Clone(TModule* module, TDocTemplate*& phead)
{
  TDocTemplateT<D,V>* tpl = new TDocTemplateT<D,V>(GetRegList(),
                                                   GetModule(), phead);
  tpl->SetModule(module);
  return tpl;
}

//
// 'Factory' method to create a new document of type 'D' using the specified
// parameter as the parent document.
//
#if !defined(BI_NO_COVAR_RET)
template<class D, class V> inline D*
#else
template<class D, class V> inline TDocument*
#endif
TDocTemplateT<D,V>::ConstructDoc(TDocument* parent/* = 0*/)
{
  return new D(parent);
}

//
// 'Factory' method to create a new view of type 'V' from the specified
// document parameter.
//
#if !defined(BI_NO_COVAR_RET)
template<class D, class V> inline V*
#else
template<class D, class V> inline TView*
#endif
TDocTemplateT<D,V>::ConstructView(TDocument& doc)
{
  return new V((D&)doc);
}

//
template<class D, class V> inline LPCTSTR
TDocTemplateT<D,V>::GetViewName()
{
  return V::StaticName();
}

//----------------------------------------------------------------------------

#if !defined(BI_NO_COVAR_RET)
template<class D, class V> D*
#else
template<class D, class V> TDocument*
#endif
TDocTemplateT<D,V>::IsMyKindOfDoc(TDocument& doc)
{
  return dynamic_cast<D*>(&doc);
}

#if !defined(BI_NO_COVAR_RET)
template<class D, class V> V*
#else
template<class D, class V> TView*
#endif
TDocTemplateT<D,V>::IsMyKindOfView(TView& view)
{
  return dynamic_cast<V*>(&view);
}

// check maybe suffucient define BI_STD_SPECIALIZATION for MSVC ???
#if defined(BI_COMP_MSC) && !defined(BI_STD_SPECIALIZATION) && !defined(BI_NO_OBJ_STREAMING)
#  define DEFINE_DOC_TEMPLATE_CLASS(docClass, viewClass, tplClass) \
    typedef NS_OWL::TDocTemplateT<docClass, viewClass> tplClass;   \
    IMPLEMENT_STREAMABLE_CLASS( tplClass );                                  \
    NS_OWL::TDocTemplateT<docClass, viewClass>::TDocTemplateT(NS_OWL::StreamableInit) : NS_OWL::TDocTemplate( streamableInit) {} \
    IMPLEMENT_STREAMABLE_POINTER( tplClass )
#else   //#if defined(BI_COMP_MSC) && !defined(BI_STD_SPECIALIZATION) && !defined(BI_NO_OBJ_STREAMING)
#  if defined(BI_STD_SPECIALIZATION)
#    define DEFINE_DOC_TEMPLATE_CLASS(docClass, viewClass, tplClass) \
      typedef NS_OWL::TDocTemplateT<docClass, viewClass> tplClass;   \
      IMPLEMENT_STREAMABLE_CLASS( tplClass );                        \
      template<> tplClass::tplClass( StreamableInit ) : NS_OWL::TDocTemplate( streamableInit ) {}\
      template<> NS_OWL::ipstream& tplClass::readPtr( NS_OWL::ipstream& is, tplClass*& cl )      \
        {                                                                           \
          __OWL_USING_NAMESPACE;                                                    \
          TStreamableBase *temp = 0;                                                \
          is.readObjectPointer( temp );                                             \
          cl = TYPESAFE_DOWNCAST(temp,tplClass);                                    \
          return is;                                                                \
        }                                                                           \
      template<> NS_OWL::ipstream& tplClass::readRef( NS_OWL::ipstream& is, tplClass& cl )       \
        {                                                                           \
          __OWL_USING_NAMESPACE;                                                    \
          TStreamableBase *ptr = &cl;                                               \
          is.readObject( ptr );                                                     \
          return is;                                                                \
        }                                                                           \
      template<> NS_OWL::opstream& tplClass::writeRef( NS_OWL::opstream& os, const tplClass& cl )\
        {                                                                           \
          __OWL_USING_NAMESPACE;                                                    \
          os.writeObject( &cl );                                                    \
          return os;                                                                \
        }                                                                           \
      template<> NS_OWL::opstream& tplClass::writePtr( NS_OWL::opstream& os, const tplClass* cl )\
        {                                                                           \
          __OWL_USING_NAMESPACE;                                                    \
          os.writeObjectPointer( cl );                                              \
          return os;                                                                \
        }
#  else 	//#if defined(BI_STD_SPECIALIZATION)
#    define DEFINE_DOC_TEMPLATE_CLASS(docClass, viewClass, tplClass) \
	  typedef NS_OWL::TDocTemplateT<docClass, viewClass> tplClass;     \
	  IMPLEMENT_STREAMABLE_FROM_BASE(tplClass, TDocTemplate);
#  endif	//#if defined(BI_STD_SPECIALIZATION)
#endif		//#if defined(BI_COMP_MSC) && !defined(BI_STD_SPECIALIZATION) && !defined(BI_NO_OBJ_STREAMING)


//----------------------------------------------------------------------------
#if defined(OWL2_COMPAT)

//
inline TDocument*
TDocTemplate::InitDoc(TDocument* doc, LPCTSTR path, long flags)
{
  return InitDoc_(*this, doc, path, flags);
}

//
inline TView*
TDocTemplate::InitView(TView* view)
{
  return InitView_(view);
}

//
inline bool
TDocTemplate::SelectSave(TDocument& doc)
{
  return SelectSave_(this, doc);
}

//
#if !defined(BI_NO_COVAR_RET)
template<class D, class V> inline D*
#else
template<class D, class V> inline TDocument*
#endif
TDocTemplateT<D,V>::CreateDoc(LPCTSTR path, long flags)
{
  return (D*)InitDoc(new D((TDocument*)0), path, flags);
}

//
template<class D, class V> inline TView*
TDocTemplateT<D,V>::CreateView(TDocument& doc, long /*flags*/)
{
  return (V*)InitView(new V((D&)doc));
}
#endif  // defined(OWL2_COMPAT)


__OWL_END_NAMESPACE


#endif  // OWL_DOCTPL_H
