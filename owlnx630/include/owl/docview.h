//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// Definition of classes TDocument, TView, TWindowView, TStream, TInStream,
// TOutStream
//----------------------------------------------------------------------------

#if !defined(OWL_DOCVIEW_H)
#define OWL_DOCVIEW_H

#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif
#if !defined(_INC_COMMDLG)
#if defined(BI_NEED_COMMDLG_H)
# include <api_upd/commdlg.h>
#else
# include <commdlg.h>
#endif
#endif
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//#if defined(BI_STD_IO) 
//#  if defined(BI_COMP_BORLANDC)
//#    include <rw/iosbase.h>
//#    define IOS_OMODE_LAST 0x80
//#  else
//#    include <xiosbase>
//#    define IOS_OMODE_LAST 0x80 //??????????????
//#  endif
//#include <ios>
//#  define IOS_OMODE_LAST 0x80
//#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TDocManager;
class _OWLCLASS TDocTemplate;
class _OWLCLASS TMenuDescr;
class _OWLCLASS TBarDescr;

_OWLFUNC(int)  FormatFileTime(FILETIME* pft, void * dest, int textlen);

//
// Document open and sharing modes - used in storage and stream constructors
// Note: the bits values are those of file streams, not same as RTL or OLE
//
enum TDocMode {
#if !defined(BI_STD_IO)
  ofParent    = 0,      // use open mode of parent storage
  ofRead      = 0x0001, // ios::in,  open for reading
  ofWrite     = 0x0002, // ios::out, open for writing
  ofReadWrite = (ofRead|ofWrite),
  ofAtEnd     = 0x0004, // ios::ate, seek to eof upon original open
  ofAppend    = 0x0008, // ios::app, append mode: all additions at eof
  ofTruncate  = 0x0010, // ios::trunc, truncate file if already exists
  ofNoCreate  = 0x0020, // ios::nocreate,  open fails if file doesn't exist
  ofNoReplace = 0x0040, // ios::noreplace, open fails if file already exists
  ofBinary    = 0x0080, // ios::binary, binary (not text) file, no CR stripping
  ofIosMask   = 0x00FF, // all of the above bits as used by class ios
#else
//  ofParent    = (IOS_OMODE_LAST << 1),      // use open mode of parent storage
  ofParent    = 0,        // use open mode of parent storage  
  ofRead      = NS_OWL_STREAMS::ios::in,  // ios::in,  open for reading
  ofWrite     = NS_OWL_STREAMS::ios::out, // ios::out, open for writing
  ofReadWrite = (ofRead|ofWrite),
  ofAtEnd     = NS_OWL_STREAMS::ios::ate, // ios::ate, seek to eof upon original open
  ofAppend    = NS_OWL_STREAMS::ios::app, // ios::app, append mode: all additions at eof
  ofTruncate  = NS_OWL_STREAMS::ios::trunc, // ios::trunc, truncate file if already exists
//  ofNoCreate  = (IOS_OMODE_LAST << 2), // ios::nocreate,  open fails if file doesn't exist
//  ofNoReplace = (IOS_OMODE_LAST << 3), // ios::noreplace, open fails if file already exists
#if defined(_IOS_Nocreate)
  ofNoCreate = _IOS_Nocreate, // ios::nocreate,  open fails if file doesn't exist
#else
  ofNoCreate  = 0x40, // ios::nocreate,  open fails if file doesn't exist
#endif
#if defined(_IOS_Noreplace)
  ofNoReplace = _IOS_Noreplace, // ios::noreplace, open fails if file already exists
#else
  ofNoReplace = 0x80, // ios::noreplace, open fails if file already exists
#endif
  ofBinary    = NS_OWL_STREAMS::ios::binary, // ios::binary, binary (not text) file, no CR stripping
  ofIosMask   = 0x00FF, // all of the above bits as used by class ios
#endif

  ofTransacted= 0x1000, // STGM_TRANSACTED, supports commit and revert
  ofPreserve  = 0x2000, // STGM_CONVERT, backup old data of same name
  ofPriority  = 0x4000, // STGM_PRIORITY, temporary efficient peeking
  ofTemporary = 0x8000, // STGM_DELETEONRELEASE, delete when destructed

  shCompat    = 0x0600, // for non-compliant applications, avoid if possible
  shNone      = 0x0800, // EXCLUSIVE functionality
  shRead      = 0x0A00, // DENY_WRITE functionality
  shWrite     = 0x0C00, // DENY_READ functionality
  shReadWrite = 0x0E00, // DENY_NONE functionality
  shDefault   = 0,      // use stream implementation default value
  shMask      = (shCompat|shNone|shRead|shWrite)
};
#define PREV_OPEN           (ofNoCreate|ofNoReplace)
#define IS_PREV_OPEN(omode) ((omode & PREV_OPEN)==PREV_OPEN)

//
// Definitions of vnXxxx view notification event IDs
// event ID's up to vnCustomBase reserved for general doc-view notifications
//
const int vnViewOpened  = 1;   // a new view has just been constructed
const int vnViewClosed  = 2;   // another view is about to be destructed
const int vnDocOpened   = 3;   // document has just been opened
const int vnDocClosed   = 4;   // document has just been closed
const int vnCommit      = 5;   // document is committing, flush cached changes
const int vnRevert      = 6;   // document has reverted, reload data from doc
const int vnIsDirty     = 7;   // respond true if uncommitted changes present
const int vnIsWindow    = 8;   // respond true if passed HWND is that of view
const int vnCustomBase = 100;  // base of document class specific notifications

//
// Document and view property access flags
//
const int pfGetText   =  1;   // property accessible as text format
const int pfGetBinary =  2;   // property accessible as native non-text format
const int pfConstant  =  4;   // property is invariant for object instance
const int pfSettable  =  8;   // property settable as native format
const int pfUnknown   = 16;   // property defined but unavailable in object
const int pfHidden    = 32;   // property should be hidden from normal browse
const int pfUserDef   =128;   // property has been user-defined at run time

//
// Classes defined later in this file
//
class _OWLCLASS TStream;

//
// This causes
// fatal error C1001: INTERNAL COMPILER ERROR  (compiler file 'f:\vs70builds\3077\vc\Compiler\Utc\src\P2\p2symtab.c', line 4533)
//class _OWLCLASS TInStream;
//class _OWLCLASS TOutStream;
class TInStream;
class TOutStream;

class _OWLCLASS TDocument;
class _OWLCLASS TView;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TDocument
// ~~~~~ ~~~~~~~~~
// Abstract base class for document/view management
//
class _OWLCLASS TDocument : public TStreamableBase {
  public:

    // Enumeration describing the basic properties of a document.
    // These can be used to update or query the attributes of the 
    // document. See GetProperty and SetProperty for more information.
    //
    enum TDocProp {      
      PrevProperty = 0,  // Index of last property in base class (none in this case)
      DocumentClass,     // text Property: Name of C++ class encapsulating document
      TemplateName,      // text property: Name of template attached to document
      ViewCount,         // int  property: Number of views displaying this document
      StoragePath,       // text property: Identifies object holding data of this document
      DocTitle,          // text property: Caption of this document
      NextProperty,      // Next index to be used by derived class
    };

    // Document list class
    //
    class _OWLCLASS TList {
      public:
        TList() : DocList(0) {}
       ~TList() {Destroy();}
        bool Insert(TDocument* doc);  // Insert new document, fails if there
        bool Remove(TDocument* doc);  // Remove document, fails if not there
        TDocument* Next(const TDocument* doc); // Returns first if doc=0
        void Destroy();               // Deletes all documents

      private:
        TDocument* DocList;
    };
    typedef TList List;  // for compatibility

    // Document constructor / destructor
    //
    TDocument(TDocument* parent = 0);
    virtual ~TDocument();

    virtual TInStream*  InStream(int mode, LPCTSTR strmId=0);
    virtual TOutStream* OutStream(int mode, LPCTSTR strmId=0);

    virtual bool   Open(int mode, LPCTSTR path=0);
    virtual bool   Close();         // close document, does not delete or detach
    virtual bool   Commit(bool force=false); // save current data, force write
    virtual bool   Revert(bool clear=false); // abort changes, no reload if true
    virtual TDocument& RootDocument();

    TDocManager&   GetDocManager();
    void           SetDocManager(TDocManager& dm);
    TDocument*     GetParentDoc();

    TDocTemplate*  GetTemplate();
    bool           SetTemplate(TDocTemplate* tpl);

    LPCTSTR GetDocPath() const;
    virtual bool    SetDocPath(LPCTSTR path);

    LPCTSTR GetTitle() const;
    virtual void   SetTitle(LPCTSTR title);

    virtual bool   IsDirty();           // Also queries doc and view hierarchy
    void           SetDirty(bool dirty = true);

    virtual bool   IsOpen();
    virtual bool   CanClose();           // Returns false if unable to close
    virtual bool   HasFocus(HWND hwnd);  // Document (or child doc) has Focus
    virtual TDocument* DocWithFocus(HWND hwnd); // Doc/ChildDoc with focus

    bool           NotifyViews(int event, long item=0, TView* exclude=0);
    TView*         QueryViews(int event, long item=0, TView* exclude=0);
    virtual uint   PostError(uint sid, uint choice = MB_OK);

    // Property access and info
    //
    virtual int    PropertyCount();
    virtual int    FindProperty(LPCTSTR name);    // return property index
    virtual int    PropertyFlags(int index);      // pfXxxxx bit array
    virtual const _TCHAR* PropertyName(int index);  // locale invariant name
    virtual int    GetProperty(int index, void * dest, int textlen=0);
    virtual bool   SetProperty(int index, const void * src); // native type

    TView*         GetViewList() const;
    TView*         NextView(const TView* view);

    TStream*       GetStreamList() const;
    TStream*       NextStream(const TStream* strm);

    void *      GetTag() const;
    void           SetTag(void* * tag);

    int            GetOpenMode() const;
    void           SetOpenMode(int mode);

    TView*         InitView(TView* view);    // called from template InitView

    bool           IsEmbedded() const;
    void           SetEmbedded(bool embed);

    virtual bool   InitDoc();

  public:
    TDocument(TDocManager* docMan);  // create a dummy document to hold docmgr

  protected:
    virtual void  AttachStream(TStream& strm);// called from TStream constructor
    virtual void  DetachStream(TStream& strm);// called from TStream destructor

  public_data:
    void *     Tag;          // application hook, not used internally
    TList         ChildDoc;     // linked child document chain

  protected_data:
    bool          DirtyFlag;    // document changed, might not represent views
    bool          Embedded;     // document is an embedding
//  static int    UntitledIndex;// last used index for Untitled document

  private:
    TDocManager*  DocManager;   // pointer back to document manager
    TDocument*    ParentDoc;    // parent document, 0 if this is root document
    TDocument*    NextDoc;      // next in linked chain of active documents
    int           OpenMode;     // mode and protection flags
    _TCHAR *   Title;        // current document title, 0 if untitled
    TDocTemplate* Template;     // template associated with this document
    TView*        ViewList;     // head of linked view chain, 0 if no views
    TStream*      StreamList;   // head of linked stream chain, 0 if no streams
    _TCHAR *   DocPath;      // path used to open/save document

    void   ReindexFrames();          // force view title and index update
    void   AttachView(TView& view);  // called from TView constructor
    bool   DetachView(TView& view);  // called from TView destructor


  friend class _OWLCLASS TDocTemplate;  // access to InitView()
  friend class _OWLCLASS TView;         // access to Attach/DetatchView()
  friend class _OWLCLASS TStream;   // access to Attach/DetachStream()
  friend class _OWLCLASS TDocManager;
  friend class _OWLCLASS TList;         // access to NextDoc

  //DECLARE_ABSTRACT_STREAMABLE(_OWLCLASS,TDocument,1);
  DECLARE_ABSTRACT_STREAMABLE_OWL(NS_OWL::TDocument,1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TDocument );

//
// class TView
// ~~~~~ ~~~~~
// Abstract base class for view access from document
//
class _OWLCLASS TView : virtual public TEventHandler,
                        virtual public TStreamableBase {
  public:
    enum {
      PrevProperty = 0,
      ViewClass,                  // text
      ViewName,                   // text
      NextProperty,
    };

    TView(TDocument& doc);
    virtual ~TView();

    TDocument&  GetDocument();
    void        SetDocument(TDocument&); // Added by Vidar Hasfjord, 2007-08-27.

    uint        GetViewId();

    virtual TMenuDescr* GetViewMenu();
    virtual TBarDescr*  GetViewBar();
    void        SetViewMenu(TMenuDescr* menu);
    void        SetViewBar(TBarDescr* bar);

    bool        IsOK();           // true if successfully created

    static uint GetNextViewId();  // Next global ID to assign
    static void BumpNextViewId();

    TView*      GetNextView();

    // Must implement, used by template manager for selection
    // static LPCTSTR StaticName() {return "name of view";}
    //
    virtual LPCTSTR GetViewName() = 0;    // return static name of view

    virtual TWindow* GetWindow();                 // if not derived from TWindow
    virtual bool   SetDocTitle(LPCTSTR docname, int index);

    // Property access and info
    //
    virtual int    PropertyCount();
    virtual int    FindProperty(LPCTSTR name);// return property index
    virtual int    PropertyFlags(int index);          // pfXxxxx bit array
    virtual LPCTSTR  PropertyName(int index);     // locale invariant name
    virtual int    GetProperty(int index, void * dest, int textlen=0);
    virtual bool   SetProperty(int index, const void * src);

    void *      GetTag() const;
    void           SetTag(void* * tag);

  protected:
    void         NotOK();       // To flag errors in creation

  protected_data:
    TDocument*   Doc;

  public_data:
    void *    Tag;           // Application hook, not used internally

  private:
    TView*       NextView;      // Linked view chain, 0 if no more views
    uint         ViewId;        // Unique ID for this view, used for controls
    TMenuDescr*  ViewMenu;      // Menu descriptor specific for this view or 0
    TBarDescr*   ViewBar;      // Bar descriptor specific for this view or 0
    static uint  NextViewId;    // Next view ID to be assigned to a view

  friend class _OWLCLASS TDocument;   // needs access to NextView
#if !defined(BI_NO_OBJ_STREAMING)
  friend class TDocument::Streamer;   // needs access to NextView
#endif
  //DECLARE_ABSTRACT_STREAMABLE(_OWLCLASS,TView,1);
  DECLARE_ABSTRACT_STREAMABLE_OWL(__OWL TView,1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( __OWL TView );

//
// class TWindowView
// ~~~~~ ~~~~~~~~~~~
class _OWLCLASS TWindowView : public TWindow, public TView {
  public:
    TWindowView(TDocument& doc, TWindow* parent = 0);
   ~TWindowView();

    static LPCTSTR StaticName();  // put in resource

    // Override virtuals from TWindow
    //
    bool     CanClose();

    // Override virtuals from TView
    //
    LPCTSTR  GetViewName();
    TWindow* GetWindow();
    bool     SetDocTitle(LPCTSTR docname, int index);

  private:
    // Event handlers
    //
    bool     VnIsWindow(HWND hWnd);

  DECLARE_RESPONSE_TABLE(TWindowView);
  //DECLARE_STREAMABLE (_OWLCLASS, __OWL TWindowView,1);
  DECLARE_STREAMABLE_OWL(__OWL TWindowView, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( __OWL TWindowView );


//
// class TDialogView
// ~~~~~ ~~~~~~~~~~~
// must be overriden, derived class have to define resId
class _OWLCLASS TDialogView : public TDialog, public TView {
  public:
    TDialogView(TDocument& doc, TWindow* parent, TResId resId, TModule* module = gModule);
    TDialogView(TDocument& doc, TWindow* parent, const DLGTEMPLATE& dlgTemplate, TAutoDelete = AutoDelete, TModule* module = gModule);
    TDialogView(TDocument& doc, TWindow* parent, TModule* module, HGLOBAL hTemplate, TAutoDelete = AutoDelete);
   ~TDialogView();

    static LPCTSTR StaticName();  // put in resource

    // Override virtuals from TWindow
    //
    bool     CanClose();

    // Override virtuals from TView
    //
    LPCTSTR  GetViewName();
    TWindow* GetWindow();
    bool     SetDocTitle(LPCTSTR docname, int index);

  private:
    // Event handlers
    //
    bool     VnIsWindow(HWND hWnd);

  DECLARE_RESPONSE_TABLE(TDialogView);
  //DECLARE_STREAMABLE (_OWLCLASS, __OWL TDialogView,1);
  DECLARE_STREAMABLE_OWL(__OWL TDialogView,1);

};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( __OWL TDialogView );

//
// class TStream
// ~~~~~ ~~~~~~~
// Abstract base class for storage stream access
//
class _OWLCLASS TStream {
  public:
    TDocument& GetDocument();
   ~TStream();
    int  GetOpenMode();
    LPCTSTR GetStreamName();

  protected:
    TDocument& Doc;        // document owning this stream
    TStream* NextStream;   // next stream in linked list of active streams

    TStream(TDocument& doc, LPCTSTR name, int mode);

  private:
    int     OpenMode;
    LPCTSTR StreamName;

  friend class TDocument;
};

//
// class TInStream
// ~~~~~ ~~~~~~~~~
// Base Getclass for input streams
//
class TInStream : public TStream, public _tistream {
  public:
    TInStream(TDocument& doc, LPCTSTR name, int mode);
};

//
// class TOutStream
// ~~~~~ ~~~~~~~~~~
// Base class for output streams
//
class TOutStream : public TStream, public _tostream {
  public:
    TOutStream(TDocument& doc, LPCTSTR name, int mode);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// View Notification Handler Definitions
//

//
// DocView aliases to actual dispatchers
//
#define B_void_Dispatch     B_Dispatch            // No parameters
#define B_int_Dispatch      B_I2_Dispatch         // LPARAM as int
#define B_pointer_Dispatch  B_POINTER_Dispatch    // LPARAM as void*
#define B_long_Dispatch     B_LPARAM_Dispatch     // LPARAM as long

//
// Define a DocView notification signature
//   'id' is the vnXxxXxx name of the notification
//   'arg' is the type of the arg passed to the handler
//
#define NOTIFY_SIG(id, arg) \
  template <class T> \
  inline bool _CALLCNVN (T::*id##_Sig(bool _CALLCNVN (T::*pmf)(arg)))(arg) {return pmf;}

NOTIFY_SIG(vnViewOpened,  TView*)
NOTIFY_SIG(vnViewClosed,  TView*)
NOTIFY_SIG(vnDocOpened,   int)
NOTIFY_SIG(vnDocClosed,   int)
NOTIFY_SIG(vnCommit,      bool)
NOTIFY_SIG(vnRevert,      bool)
NOTIFY_SIG(vnIsDirty,     void)
NOTIFY_SIG(vnIsWindow,    HWND)

//
// Define a DocView notification response entry
//   'id' is the id from NOTIFY_SIG above
//   'method' is the method name called by the notification
//   'disp' is the type of the lParam dispacher to use, and can be:
//     'void'    lParam not passed
//     'int'     integer size (16bit for win16, or 32bit on win32)
//     'pointer' pointer size (16bit for small & medium, or 32bit all other)
//     'long'    32 bit passed always
//
#define VN_DEFINE(id, method, disp) \
  {{WM_OWLNOTIFY}, id, \
  (TAnyDispatcher)::B_##disp##_Dispatch, \
  (TMyPMF)id##_Sig(&TMyClass::method)}

#define EV_VN_VIEWOPENED  VN_DEFINE(NS_OWL::vnViewOpened,  VnViewOpened,  pointer)
#define EV_VN_VIEWCLOSED  VN_DEFINE(NS_OWL::vnViewClosed,  VnViewClosed,  pointer)
#define EV_VN_DOCOPENED   VN_DEFINE(NS_OWL::vnDocOpened,   VnDocOpened,   int)
#define EV_VN_DOCCLOSED   VN_DEFINE(NS_OWL::vnDocClosed,   VnDocClosed,   int)
#define EV_VN_COMMIT      VN_DEFINE(NS_OWL::vnCommit,      VnCommit,      int)
#define EV_VN_REVERT      VN_DEFINE(NS_OWL::vnRevert,      VnRevert,      int)
#define EV_VN_ISDIRTY     VN_DEFINE(NS_OWL::vnIsDirty,     VnIsDirty,     void)
#define EV_VN_ISWINDOW    VN_DEFINE(NS_OWL::vnIsWindow,    VnIsWindow,    int)

//----------------------------------------------------------------------------
// Inline implementations
//

//
inline TInStream* TDocument::InStream(int /*mode*/, LPCTSTR /*strmId*/) {
  return 0;
}

//
inline TOutStream* TDocument::OutStream(int /*mode*/, LPCTSTR /*strmId*/) {
  return 0;
}

//
inline bool TDocument::Open(int /*mode*/, LPCTSTR /*path*/) {
  return true;
}

//
inline TDocManager& TDocument::GetDocManager() {
  return *DocManager;
}

//
inline TDocument* TDocument::GetParentDoc() {
  return ParentDoc;
}

//
inline TDocTemplate* TDocument::GetTemplate() {
  return Template;
}

//
inline LPCTSTR TDocument::GetDocPath() const {
  return DocPath;
}

//
inline LPCTSTR TDocument::GetTitle() const {
  return Title;
}

//
// Update the document's dirty flag using the specified parameter.
//
inline void TDocument::SetDirty(bool dirty) {
  DirtyFlag = dirty;
}

//
inline bool TDocument::IsOpen() {
  return StreamList != 0;
}

//
inline int TDocument::PropertyCount() {
  return NextProperty-1;
}

//
inline bool TDocument::IsEmbedded() const {
  return Embedded;
}

//
inline void TDocument::SetEmbedded(bool embed) {
  Embedded = embed;
}

//
inline bool TDocument::InitDoc() {
  return true;
}

//
// Return pointer to the head of the link list of views associated with this
// document.
// NOTE: To iterate through all the views, use the 'NextView' method with
//       the pointer obtained from this method.
//
inline TView* TDocument::GetViewList() const {
  return ViewList;
}

//
// Return head of the link list of streams associated with this document.
// NOTE: To iterate through all the streams, use the 'NextStream' method 
// with the pointer returned from this method.
//
inline TStream* TDocument::GetStreamList() const {
  return StreamList;
}

//
// Return pointer to user-defined data [i.e. tag] attached to this document.
//
inline void * TDocument::GetTag() const {
  return Tag;
}

//
// Attach an arbitrary (user-defined) pointer with this document.
// NOTE: The 'Tag' is a place-holder. It is not used by the Doc/View 
//       subsystem.
//
inline void TDocument::SetTag(void* * tag) {
  Tag = tag;
}

//
inline TDocument::TDocument(TDocManager* docMan)
:
  DocManager(docMan), ParentDoc(this)
{}

//
inline int
TDocument::GetOpenMode() const
{
  return OpenMode;
}

//
inline void
TDocument::SetOpenMode(int mode)
{
  OpenMode = mode;
}

//
inline TDocument& TView::GetDocument() {
  return *Doc;
}

//
inline uint TView::GetViewId() {
  return ViewId;
}

//
inline TMenuDescr* TView::GetViewMenu() {
  return ViewMenu;
}

//
inline TBarDescr* TView::GetViewBar() {
  return ViewBar;
}

//
inline bool TView::IsOK() {
  return ViewId != 0;
}
//
inline uint TView::GetNextViewId() {
  return NextViewId;
}

//
inline TView* TView::GetNextView() {
  return NextView;
}

//
inline TWindow* TView::GetWindow() {
  return 0;
}

//
// Retrieve the user-defined pointer attached to this view.
//
inline void * TView::GetTag() const {
  return Tag;
}

//
// Associate an arbitrary (user-defined) pointer with this view.
// NOTE: The 'Tag' is not unsed by the Doc/View subsystem.
//
inline void TView::SetTag(void* * tag) {
  Tag = tag;
}

//
inline bool TView::SetDocTitle(LPCTSTR /*docname*/, int /*index*/) {
  return false;
}

//
inline int TView::PropertyCount() {
  return NextProperty - 1;
}

//
inline bool TView::SetProperty(int /*index*/, const void * /*src*/) {
  return false;
}

//
inline void TView::NotOK() {
  ViewId = 0;
}

//
// class TWindowView
// ~~~~~ ~~~~~~~~~~~
inline TWindowView::~TWindowView() {
}

//
inline LPCTSTR TWindowView::StaticName() {
  return _T("Window View");
}

//
// Does a given HWND belong to this view? Yes if it is us, or a child of us
//
inline bool TWindowView::VnIsWindow(HWND hWnd){
  return hWnd == GetHandle() || IsChild(hWnd);
}
//
// Only query document if this is the last view open to it.
//
inline bool TWindowView::CanClose() {
  return TWindow::CanClose() && 
         (Doc->NextView(this) || 
          Doc->NextView(0) != this ||
          Doc->CanClose());
}

//
inline LPCTSTR TWindowView::GetViewName() {
  return StaticName();
}

//
inline TWindow* TWindowView::GetWindow() {
  return (TWindow*)this;
}

//
inline bool TWindowView::SetDocTitle(LPCTSTR docname, int index) {
  return TWindow::SetDocTitle(docname, index);
}

//
// class TDialogView
// ~~~~~ ~~~~~~~~~~~
inline TDialogView::~TDialogView() {
}

//
inline LPCTSTR TDialogView::StaticName(){
  return _T("Dialog View");
}

//
// Only query document if this is the last view open to it.
//
inline bool TDialogView::CanClose() {
  return TDialog::CanClose() && 
         (Doc->NextView(this) || 
          Doc->NextView(0) != this ||
          Doc->CanClose());
}

//
inline LPCTSTR TDialogView::GetViewName() {
  return StaticName();
}

//
inline TWindow* TDialogView::GetWindow() {
  return (TWindow*)this;
}

//
// Does a given HWND belong to this view? Yes if it is us, or a child of us
//
inline bool TDialogView::VnIsWindow(HWND hWnd){
  return hWnd == GetHandle() || IsChild(hWnd);
}

//
inline bool TDialogView::SetDocTitle(LPCTSTR docname, int index) {
  return TDialog::SetDocTitle(docname, index);
}

//
// class TStream
// ~~~~~ ~~~~~~~
//
inline TDocument& TStream::GetDocument() {
  return Doc;
}

//
inline TStream::~TStream() {
  Doc.DetachStream(*this);
}

//
inline TStream::TStream(TDocument& doc, LPCTSTR name, int mode)
:
  Doc(doc),
  OpenMode(mode),
  StreamName(name)
{
  Doc.AttachStream(*this);
}

//
inline LPCTSTR TStream::GetStreamName() {
  return StreamName;
}

//
inline int TStream::GetOpenMode() {
  return OpenMode;
}

//
inline TInStream::TInStream(TDocument& doc, LPCTSTR name, int mode)
:
  TStream(doc, name, mode) 
#if defined(BI_COMP_MSC)
# if _MSC_VER == 1200 && _STLPORT_VERSION == 0x461
  ,_tistream(0) // SIR June 19th 2007 _Noinit does not work with STLport 4.6.2.
# else
  ,_tistream(NS_OWL_STREAMS::_Noinit)
# endif
#elif defined(BI_STD_NEWSSTREAM)
# if defined(BI_HAS_STREAM_NOINIT)
  ,_tistream(NS_OWL_STREAMS::_Noinit)
# else
  ,_tistream(0)  // init base to null, don't forget to call this->init()
# endif
#endif
{}

//
inline TOutStream::TOutStream(TDocument& doc, LPCTSTR name, int mode)
:
  TStream(doc, name, mode)
#if defined(BI_COMP_MSC)
# if _MSC_VER == 1200 && _STLPORT_VERSION == 0x461
  ,_tostream(0) // SIR June 19th 2007 _Noinit does not work with STLport 4.6.2.
# else
  ,_tostream(NS_OWL_STREAMS::_Noinit)
# endif
#elif defined(BI_STD_NEWSSTREAM)
# if defined(BI_HAS_STREAM_NOINIT)
  ,_tostream(NS_OWL_STREAMS::_Noinit)
# else
  ,_tostream(0)  // init base to null, don't forget to call this->init()
# endif
#endif
{}


__OWL_END_NAMESPACE


#endif  // OWL_DOCVIEW_H
