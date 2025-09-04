//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// General Registry access & registration implementation
//   TRegKey, TRegValue, TRegKeyIterator, TRegValueIterator
//   TXRegistry
//   TRegItem, TRegList, TRegLink - associative array of localizable string parameters
//   OC registry functions
//----------------------------------------------------------------------------

#if !defined(OWL_REGISTRY_H)
#define OWL_REGISTRY_H

#if !defined(OWL_EXBASE_H)
# include <owl/exbase.h>
#endif
#if !defined(OWL_OWLINC_H)
# include <owl/wsysinc.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
# include <owl/lclstrng.h>
#endif
#if !defined(OWL_PRIVATE_MEMORY_H)
# include <owl/private/memory.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


//----------------------------------------------------------------------------
// Direct registry access classes. Can read & iterate as well as create and
// write keys and value-data pairs
//

//
// class TRegKey
// ~~~~~ ~~~~~~~
// Encapsulation of a registration key
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class _OWLCLASS TRegKeyIterator;
class _OWLCLASS TRegKey {
  private:
    //BV, Sirma, Engview: Copy constructor is essential, it is implemented by default and default
    //implementation is not appropriate, we would better disable it until we write a good one:
    TRegKey(const TRegKey&){}
  public:
    typedef HKEY THandle;

    // Enumeration used to specify whether a key should be created (or 
    // simply opened).
    //
    enum TCreateOK {
      CreateOK,             // Create key if it does not exist
      NoCreate              // Don't create, simply open
    };

    // Create or open a key given a base key and a subkeyname. Security
    // information is ignored in 16bit (and under Win95)
    // Can also provide an ok-to-create or open-only indicator.
    //
    TRegKey(THandle baseKey,
            LPCTSTR keyName,
            REGSAM samDesired = KEY_ALL_ACCESS,
            TCreateOK createOK = CreateOK);

    // Construct a key give the current position of a regkey iterator
    //
    TRegKey(const TRegKeyIterator& iter, REGSAM samDesired = KEY_ALL_ACCESS);

    // Contruct a key that is an alias to an existing HKEY
    //
    TRegKey(THandle aliasKey, bool shouldClose=false, LPCTSTR keyName = 0);
   ~TRegKey();

    operator THandle() const;

    long DeleteKey(LPCTSTR subKeyName);
    long NukeKey(LPCTSTR subKeyName);
    long Flush() const;

    long GetSecurity(SECURITY_INFORMATION secInf, PSECURITY_DESCRIPTOR secDesc,
                     DWORD* secDescSize);
    long SetSecurity(SECURITY_INFORMATION secInf, PSECURITY_DESCRIPTOR secDesc);

    // Saving & loading of this key & subKeys
    //
    long Save(LPCTSTR fileName);
    long LoadKey(LPCTSTR subKeyName, LPCTSTR fileName);
    long ReplaceKey(LPCTSTR subKeyName, LPCTSTR newFileName,
                    LPCTSTR oldFileName);
    long Restore(LPCTSTR fileName, uint32 options=0);
    long UnLoadKey(LPCTSTR subKeyName);

    //long NotifyChangeKeyValue();

    // Some of these are initialized at ctor
    //
    long QueryInfo(LPTSTR class_, DWORD* classSize, DWORD* subkeyCount,
                   DWORD* maxSubkeySize, DWORD* maxClassSize,
                   DWORD* valueCount, DWORD* maxValueName,
                   DWORD* maxValueData, DWORD* secDescSize,
                   FILETIME * lastWriteTime);

    LPCTSTR GetName() const;
    uint32 GetSubkeyCount() const;
    uint32 GetValueCount() const;

    // Special predefined root keys
    //

  #if 0
    //
    // The following static objects cause a problem under system where
    // the keys do no exist: instead of simply indicating a failure to
    // open the key, the OS causes an access violation. The previous
    // implementation did contain a __try/__except clause to handle these
    // (see TRegKey's constructor in REGISTRY.CPP). Unfortunately, the
    // exceptions are still caught by debuggers; furthermore, sometimes they
    // render the system unstable ?!?!
    //
    // So, we'll replace the static objects with accessors - These keys
    // should only be accessed on versions of the OS that supports them.
    // You can make use of TSystem to determine the OS at runtime.
    //
    // NOTE: ClassesRoot and ClassesRootClsid are left as static objects
    //       since they are commonly used keys which are supported by all
    //       current versions of Windows. Changing these is likely to
    //       break a fair amount of code. However, we recommend that all
    //       new code use the accessors.
    //
    // NOTE: Jogy - accessing ClassesRoot caused problems in OCFNext applications,
    //       so I removed it as a static object

    //
    static TRegKey CurrentUser;
    static TRegKey LocalMachine;
    static TRegKey Users;
    static TRegKey CurrentConfig;
    static TRegKey DynData;
    static TRegKey PerformanceData;

  #else

    static TRegKey& ClassesRoot()      { return GetClassesRoot();      }
    static TRegKey& ClassesRootClsid() { return GetClassesRootClsid(); }
    static TRegKey& CurrentUser()      { return GetCurrentUser();      }
    static TRegKey& LocalMachine()     { return GetLocalMachine();     }
    static TRegKey& Users()            { return GetUsers();            }
    static TRegKey& CurrentConfig()    { return GetCurrentConfig();    }
    static TRegKey& DynData()          { return GetDynData();          }
    static TRegKey& PerformanceData()  { return GetPerformanceData();  }

  #endif

    // Accessors common/predefined registry keys
    //
    static TRegKey& GetClassesRoot();
    static TRegKey& GetClassesRootClsid();
    static TRegKey& GetCurrentUser();
    static TRegKey& GetLocalMachine();
    static TRegKey& GetUsers();
    static TRegKey& GetCurrentConfig();
    static TRegKey& GetDynData();
    static TRegKey& GetPerformanceData();

    // Friend iterators only...?
    //
    long EnumKey(int index, LPTSTR subKeyName, int subKeyNameSize) const;

    // Older, nameless subkey+(Default) value access
    //
    long SetDefValue(LPCTSTR subkeyName, uint32 type,
                     LPCTSTR data, uint32 dataSize);
    long QueryDefValue(LPCTSTR subkeyName, LPTSTR data,
                       uint32* dataSize) const;

    // Newer, named value set & get functions. Subkeys must be opened
    //
    long SetValue(LPCTSTR valName, uint32 type, const uint8* data,
                  uint32 dataSize) const;
    long SetValue(LPCTSTR valName, uint32 data) const;
    long QueryValue(LPCTSTR valName, uint32* type, uint8* data,
                    uint32* dataSize) const;
    long DeleteValue(LPCTSTR valName) const;

    // Friend iterators only...?
    //
    long EnumValue(int index, LPTSTR valueName, uint32& valueNameSize,
                   uint32* type = 0, uint8* data = 0, uint32* dataSize = 0) const;

  protected:
    THandle   Key;          // This Key's Handle
    LPTSTR    Name;         // This Key's Name
    DWORD     SubkeyCount;  // Number of subkeys
    DWORD     ValueCount;   // Number of value entries
    bool      ShouldClose;  // Should this key be closed on destruction
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif


//
// class TRegValue
// ~~~~~ ~~~~~~~~~
// Encapsulation of a value-data entry within one registration key.
//
class _OWLCLASS TRegValueIterator;
class _OWLCLASS TRegValue {
  public:
    TRegValue(const TRegKey& key, LPCTSTR name);
    TRegValue(const TRegValueIterator& iter);
   ~TRegValue();

    LPCTSTR      GetName() const;
    const uint32 GetDataType() const;
    const uint32 GetDataSize() const;
    const uint8* GetData() const;
    operator uint32() const;
    operator LPCTSTR() const;

    // Set the data and possibly type of this value
    //
    long Set(uint32 type, uint8* data, uint32 dataSize);
    long Set(uint32 data);
    long Set(LPCTSTR data);
    void operator =(uint32 v);
    long Delete();
    void operator =(LPCTSTR v);

    void RetrieveOnDemand() const;

  private:
    const TRegKey&  Key;      // Key that this value lives in
    LPCTSTR         Name;     // Working name pointer
    LPTSTR          AName;    // Allocated space for name if needed

    // Data information. //!CQ allocate only on demand?
    uint32          DataType; // Type code for value data
    mutable uint8*  Data;     // Value data itself, allocated
    uint32          DataSize; // Size in bytes of Data
};

//
// class TRegKeyIterator
// ~~~~~ ~~~~~~~~~~~~~~~
// Iterator for walking thru the subkeys of a key
//
class _OWLCLASS TRegKeyIterator {
  public:
    TRegKeyIterator(const TRegKey& key);

    operator bool();

    uint32 operator ++();
    uint32 operator ++(int);
    uint32 operator --();
    uint32 operator --(int);
    uint32 operator [](int index);

    int             Current() const;
    const TRegKey&  BaseKey() const;

    void Reset();

  private:
    const TRegKey& Key;
    int            Index;
};

//
// class TRegValueIterator
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Iterator for walking thru the values of a key
//
class _OWLCLASS TRegValueIterator {
  public:
    TRegValueIterator(const TRegKey& regKey);

    operator bool();

    uint32 operator ++();
    uint32 operator ++(int);
    uint32 operator --();
    uint32 operator --(int);
    uint32 operator [](int index);

    int            Current() const;
    const TRegKey& BaseKey() const;

    void Reset();

  private:
    const TRegKey& Key;
    int            Index;
};

//----------------------------------------------------------------------------

//
// class TXRegistry
// ~~~~~ ~~~~~~~~~~
// Registry related exception class
//
class _OWLCLASS TXRegistry : public TXBase {
  public:
    TXRegistry(const owl_string& msg, const _TCHAR* key);
    TXRegistry(const TXRegistry& copy);

    static void Check(long stat, const _TCHAR* key);

    const _TCHAR* Key;
};


//----------------------------------------------------------------------------
// Registration parameter structures and formatting functions
//

//
// class TRegFormatHeap
// ~~~~~ ~~~~~~~~~~~~~~
// Used internally to provide buffers for formating registration strings
//
class _OWLCLASS TRegFormatHeap {
  public:
    TRegFormatHeap();
   ~TRegFormatHeap();
    _TCHAR*  Alloc(int spaceNeeded);
  private:
    struct TBlock {
      TBlock*  Next;     // Next heap in chain
      _TCHAR   Data[1];  // Allocated memory starts here
    };
    TBlock* Head;
};

//
// struct TRegItem
// ~~~~~~ ~~~~~~~~
// A single registration list entry
//
struct _OWLCLASS TRegItem {
  char*         Key;           // Non-localized parameter or registry subkey
  TLocaleString Value;         // Localizable value for parameter or subkey

  // Used privately by REGFORMAT, REGSTATUS macros
  //
  static _TCHAR* RegFormat(int f, int a, int t, int d, TRegFormatHeap& heap);
  static _TCHAR* RegFormat(const _TCHAR* f, int a, int t, int d, TRegFormatHeap& heap);
  static _TCHAR* RegFlags(long flags, TRegFormatHeap& heap);
  static _TCHAR* RegVerbOpt(int mf, int sf, TRegFormatHeap& heap);
  static void    OverflowCheck();
};

//
// class TRegList
// ~~~~~ ~~~~~~~~
// A registration parameter table, composed of a list of TRegItems
//
class _OWLCLASS TRegList {
  public:
    TRegList(TRegItem* _list);
    LPCTSTR Lookup(LPCSTR key,
                       TLangId lang = TLocaleString::UserDefaultLangId);
    TLocaleString& LookupRef(LPCSTR key);
    LPCTSTR operator[](LPCSTR key);

    TRegItem* Items;
};

//
// class TRegLink
// ~~~~~ ~~~~~~~~
// Registration link node, holding a pointer to a TRegList. Can be one in a
// linked list.
//
class _OWLCLASS TRegLink {
  public:
    TRegLink(TRegList& regList, TRegLink*& head);
    virtual ~TRegLink();
    TRegLink* GetNext() const;
    void      SetNext(TRegLink* link);
    TRegList& GetRegList() const ;

    static void AddLink(TRegLink** head, TRegLink* newLink);
    static bool RemoveLink(TRegLink** head, TRegLink* remLink);

  protected:
    TRegLink();           // Derived class must fill in ptrs
    TRegLink*  Next;      // Next RegLink
    TRegList*  RegList;   // Pointer to registration parameter table
};

//
// Registration parameter table macro definitions
//
#define BEGIN_REGISTRATION(regname) extern NS_OWL::TRegItem regname##_list[]; \
                                    extern NS_OWL::TRegFormatHeap __regHeap; \
                                    NS_OWL::TRegList regname(regname##_list); \
                                    static NS_OWL::TRegItem regname##_list[] = {
#define END_REGISTRATION {0,{0}} };

#define REGDATA(var,val) {#var, {val}},
#define REGXLAT(var,val) {#var, {AUTOLANG_XLAT val}},
#define REGITEM(key,val) {" " key, {val}},
#define REGFORMAT(i,f,a,t,d) {"format" #i,{TRegItem::RegFormat(f,a,t,d,__regHeap)}},
#define REGSTATUS(a,f) {"aspect" #a, {TRegItem::RegFlags(f,__regHeap)}},
#define REGVERBOPT(v,mf,sf) {#v "opt",{TRegItem::RegVerbOpt(mf,sf,__regHeap)}},
#define REGICON(i) {"iconindex",{TRegItem::RegFlags(i,__regHeap)}},
#define REGDOCFLAGS(i) {"docflags",{TRegItem::RegFlags(i,__regHeap)}},

//
// Buffer size is no longer needed, use the macro below
//
#define REGISTRATION_FORMAT_BUFFER(n) \
  NS_OWL::TRegFormatHeap __regHeap;

#define REGISTRATION_FORMAT_HEAP \
  NS_OWL::TRegFormatHeap __regHeap;

//----------------------------------------------------------------------------
// Clipboard and registry data transfer format definitions
//
//!CQ from OCF. Really put all these here?

const int ocrVerbLimit   = 8;  // maximum number of verbs registered per class
const int ocrFormatLimit = 8;  // maximum number of data formats per class

//
// Format: standard clipboard numeric format, or name of custom format
//
enum ocrClipFormat {
  ocrText           =  1,  // CF_TEXT
  ocrBitmap         =  2,  // CF_BITMAP
  ocrMetafilePict   =  3,  // CF_METAFILEPICT
  ocrSylk           =  4,  // CF_SYLK
  ocrDif            =  5,  // CF_DIF
  ocrTiff           =  6,  // CF_TIFF
  ocrOemText        =  7,  // CF_OEMTEXT
  ocrDib            =  8,  // CF_DIB
  ocrPalette        =  9,  // CF_PALETTE
  ocrPenData        = 10,  // CF_PENDATA
  ocrRiff           = 11,  // CF_RIFF
  ocrWave           = 12,  // CF_WAVE
  ocrUnicodeText    = 13,  // CF_UNICODETEXT  Win32 only
  ocrEnhMetafile    = 14,  // CF_ENHMETAFILE  Win32 only
};
#define ocrRichText          _T("Rich Text Format")
#define ocrEmbedSource       _T("Embed Source")
#define ocrEmbeddedObject    _T("Embedded Object")
#define ocrLinkSource        _T("Link Source")
#define ocrObjectDescriptor  _T("Object Descriptor")
#define ocrLinkSrcDescriptor _T("Link Source Descriptor")

//
// Aspect: view types supported by transfer
//
enum ocrAspect {
  ocrContent   = 1,  // DVASPECT_CONTENT    normal display representation
  ocrThumbnail = 2,  // DVASPECT_THUMBNAIL  picture appropriate for browser
  ocrIcon      = 4,  // DVASPECT_ICON       iconized representation of object
  ocrDocPrint  = 8,  // DVASPECT_DOCPRINT   print preview representation
};

//
// Medium: means of data transfer
//
enum ocrMedium {
  ocrNull       = 0,
  ocrHGlobal    = 1,  // TYMED_HGLOBAL  global memory handle
  ocrFile       = 2,  // TYMED_FILE     data as contents of file
  ocrIStream    = 4,  // TYMED_ISTREAM  instance of an IStream object
  ocrIStorage   = 8,  // TYMED_ISTORAGE streams within an instance of IStorage
  ocrGDI        = 16, // TYMED_GDI      GDI object in global handle
  ocrMfPict     = 32, // TYMED_MFPICT   CF_METAFILEPICT containing global handle
  ocrStaticMed  = 1024 // OLE 2 static object
};

//
// Direction: transfer directions supported
//
enum ocrDirection {
  ocrGet    = 1,
  ocrSet    = 2,
  ocrGetSet = 3,
};

//----------------------------------------------------------------------------
// Miscellaneous registry definitions
//

//
// IOleObject miscellaneous status flags, defined for each or all aspects
//
enum ocrObjectStatus {
  ocrRecomposeOnResize           = 1,   // request redraw on container resize
  ocrOnlyIconic                  = 2,   // only useful context view is Icon
  ocrInsertNotReplace            = 4,   // should not replace current select.
  ocrStatic                      = 8,   // object is an OLE static object
  ocrCantLinkInside              = 16,  // should not be the link source
  ocrCanLinkByOle1               = 32,  // only used in OBJECTDESCRIPTOR
  ocrIsLinkObject                = 64,  // set by OLE2 link for OLE1 compat.
  ocrInsideOut                   = 128, // can be activated concurrently
  ocrActivateWhenVisible         = 256, // hint to cntnr when ocrInsideOut set
  ocrRenderingIsDeviceIndependent= 512, // no decisions made based on target
  ocrNoSpecialRendering          = 512, // older enum for previous entry
};

//
// IOleObject verb menu flags
//
enum ocrVerbMenuFlags {
  ocrGrayed      = 1,   // MF_GRAYED
  ocrDisabled    = 2,   // MF_DISABLED
  ocrChecked     = 8,   // MF_CHECKED
  ocrMenuBarBreak= 32,  // MF_MENUBARBREAK
  ocrMenuBreak   = 64,  // MF_MENUBAR
};

//
// IOleObject verb attribute flags
//
enum ocrVerbAttributes {
  ocrNeverDirties      = 1,   // verb can never cause object to become dirty
  ocrOnContainerMenu   = 2,   // only useful context view is Icon
};

//
// Entries for <usage> registration parameter, class factory registration mode
//
#define ocrSingleUse     _T("1")  // single client per instance
#define ocrMultipleUse   _T("2")  // multiple clients per instance
#define ocrMultipleLocal _T("3")  // multiple clients, separate inproc server

//----------------------------------------------------------------------------
// High-level table based registration support
//

//
// class TRegTemplateList
// ~~~~~ ~~~~~~~~~~~~~~~~
// List of parameterized template strings that represent the actual entries to
// be registered. List is indexed from 1 for used with param-list template
// activation strings. See TRegParamList below.
//
class _OWLCLASS TRegTemplateList {
  public:
    TRegTemplateList(TRegKey& basekey, const _TCHAR* _list[]);
   ~TRegTemplateList();

    int      GetCount() const;
    TRegKey& GetBaseKey();

    LPCTSTR operator [](int i);

    // Enable/Disable and activate templates
    //
    void  DisableAll();
    void  EnableAll();
    void  Enable(int i);
    void  Enable(const _TCHAR* set);

    void  Activate(int i);
    void  Activate(const _TCHAR* set);

    bool  IsActive(int i) const;

  private:
    TRegKey&        BaseKey;       // Registry key that these templates are based
    const _TCHAR**  List;          // List of templates
    int             Count;         // # of templates in list
    
    //TODO Jogy - Change with enumeration
    int8*         EnabledFlags;  // Which templates are currently enabled/active
};

//
// class TRegParamList
// ~~~~~ ~~~~~~~~~~~~~
// A list of param entries for use as an intermediate between a TRegList and
// the actual template list used to generate the registration. Provides default
// values in 'Default', and tags required templates using octal char entries in
// 'TemplatesNeeded'
//
class _OWLCLASS TRegParamList {
  public:
    struct TEntry {
      _TCHAR* Param;            // Substituted parameter name
      _TCHAR* Default;          // Default value, 0 if no default & param is required
      _TCHAR* TemplatesNeeded;  // Octal string list of template indices to activate
    };

    TRegParamList(TEntry* _list);
   ~TRegParamList();

    int Find(const _TCHAR* param);   // Associative lookup of value by param
    int GetCount() const;

    TEntry& operator [](int i);

    const _TCHAR*& Value(int i);
    void   ResetDefaultValues();

  private:
    TEntry*        List;
    int            Count;
    const _TCHAR** Values;
};

//
// class TRegSymbolTable
// ~~~~~ ~~~~~~~~~~~~~~~
// High level symbol-based registry entry manipulation
//
class _OWLCLASS TRegSymbolTable {
  public:
    TRegSymbolTable(TRegKey& basekey, const _TCHAR* tplList[], TRegParamList::TEntry* paramList);

    void Init(_TCHAR* filter);
    void UpdateParams(TLangId lang, TRegItem* item);
    void StreamOut(TRegItem* item, _tostream& out);
//    void StreamIn(TRegItem* item, _tistream& in);

    TRegTemplateList Templates;
    TRegParamList    Params;
    int              UserKeyCount;
};

//
// class TRegistry
// ~~~~~ ~~~~~~~~~
// High level stream & list access to registry
//
class _OWLCLASS TRegistry {
  public:
    static int  Validate(TRegKey& baseKey, _tistream& in);  // Returns number of mismatched entries
    static void Update(TRegKey& baseKey, _tistream& in);    // Writes lines to registry
#pragma pack(push,4)
    struct TUnregParams {
      _TCHAR    Prepend;    // Optional _TCHAR to prepend to key before removing
      _TCHAR*   Name;       // Name of param
      TRegKey*  BaseKey;    // Key that that the param is based upon
    };
#pragma pack(pop)
    static int  Unregister(TRegList& regInfo, TUnregParams* params, TRegItem* overrides = 0);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// TRegKey inlines
//

//
// Return the HANDLE identifying this registry key.
//
inline
TRegKey::operator TRegKey::THandle() const
{
  return Key;
}

//
// Enumerate the subkeys of this registry key
//
inline long
TRegKey::EnumKey(int index, _TCHAR * subKeyName, int subKeyNameSize) const
{
  return ::RegEnumKey(Key, index, subKeyName, subKeyNameSize);
  //::RegEnumKeyEx(); ??
}

//
// Delete the specified subkey of this registry key
//
inline long
TRegKey::DeleteKey(LPCTSTR subKeyName)
{
  return ::RegDeleteKey(Key, subKeyName);
}

//
// Write the attribute of this key in the registry
// Note: 32 bit only
//
inline long
TRegKey::Flush() const
{
  return ::RegFlushKey(Key);
}

//
// Retrieve a copy of the security descriptor protecting this registry key. 
// Note: 32 bit only
//
inline long
TRegKey::GetSecurity(SECURITY_INFORMATION secInf, PSECURITY_DESCRIPTOR secDesc,
                     DWORD* secDescSize)
{
  return ::RegGetKeySecurity(Key, secInf, secDesc, secDescSize);
}

//
// Set the security descriptor of this key
// Note: 32 bit only
//
inline long
TRegKey::SetSecurity(SECURITY_INFORMATION secInf, PSECURITY_DESCRIPTOR secDesc)
{
  return ::RegSetKeySecurity(Key, secInf, secDesc);
}

//
// Save this key and all of its subkeys and values to the specified file.
// Note: 32 bit only
//
inline long
TRegKey::Save(LPCTSTR fileName)
{
  return ::RegSaveKey(Key, fileName, 0/*Security*/);
}

//
// Create a subkey under HKEY_USER or HKEY_LOCAL_MACHINE and stores 
// registration information from a specified file into that subkey. This 
// registration information is in the form of a hive. A hive is a discrete 
// body of keys, subkeys, and values that is rooted at the top of the 
// registry hierarchy. A hive is backed by a single file and .LOG file. 
// Note: 32 bit only
//
inline long
TRegKey::LoadKey(LPCTSTR subKeyName, LPCTSTR fileName)
{
  return ::RegLoadKey(Key, subKeyName, fileName);
}

//
// Replace the file backing this key and all its subkeys with another file, 
// so that when the system is next started, the key and subkeys will have 
// the values stored in the new file. 
// Note: 32 bit only
//
inline long
TRegKey::ReplaceKey(LPCTSTR subKeyName, LPCTSTR newFileName,
                    LPCTSTR oldFileName)
{
  return ::RegReplaceKey(Key, subKeyName, newFileName, oldFileName);
}

//
// Read the registry information in a specified file and copies it over this
// key. This registry information may be in the form of a key and multiple 
// levels of subkeys. 
// Note: 32 bit only
//
inline long
TRegKey::Restore(LPCTSTR fileName, uint32 options)
{
  return ::RegRestoreKey(Key, fileName, options);
}

//
// Unload this key and its subkeys from the registry. 
//
// Note: 32 bit only
inline long
TRegKey::UnLoadKey(LPCTSTR subKeyName)
{
  return ::RegUnLoadKey(Key, subKeyName);
}

//inline long TRegKey::NotifyChangeKeyValue() {}

//
// Return the number of subkeys attached to this key
//
inline uint32
TRegKey::GetSubkeyCount() const
{
  return SubkeyCount;
}

//
// Return the number of values attached to this key
//
inline uint32
TRegKey::GetValueCount() const
{
  return ValueCount;
}

//
// Return a string identifying this key
//
inline LPCTSTR
TRegKey::GetName() const
{
  return Name;
}

//
// Associate a value with this key
//
inline long
TRegKey::SetValue(LPCTSTR valName, uint32 type, const uint8* data,
                  uint32 dataSize) const
{
  return ::RegSetValueEx(Key, valName, 0, type, data, dataSize);
}

//
// Associate a 4-byte value with this key
// Note: 32 bit only
//
inline long
TRegKey::SetValue(LPCTSTR valName, uint32 data) const
{
  return SetValue(valName, REG_DWORD, (uint8*)&data, sizeof data);
}

//
// Retrieve the value associated with the unnamed value for this key
//
inline long
TRegKey::QueryValue(LPCTSTR valName, uint32* type, uint8* data,
                    uint32* dataSize) const
{
#if defined WINELIB   //it seems that Winlib SDK declares a 'unsigned int*' instead of DWORD* and GCC it's tooo strict
  return ::RegQueryValueEx(Key, valName, 0, (unsigned int*)type, data, (unsigned int*)dataSize);
#else
  return ::RegQueryValueEx(Key, valName, 0, type, data, dataSize);
#endif//WINELIB
}

//
// Remove a named value from this registry key
// Note: 32 bit only
//
inline long
TRegKey::DeleteValue(LPCTSTR valName) const
{
  return ::RegDeleteValue(Key, valName);
}

//
// Set the default [unnamed] value associated with this key
//
inline long
TRegKey::SetDefValue(LPCTSTR subkeyName, uint32 type,
                     LPCTSTR data, uint32 dataSize)
{
  return ::RegSetValue(Key, subkeyName, type, data, dataSize);
}

//
// Retrieve the default [unnamed] value associated with this key
//
inline long
TRegKey::QueryDefValue(LPCTSTR subkeyName, _TCHAR * data,
                       uint32* dataSize) const
{
#if defined WINELIB   //it seems that Winlib SDK declares a 'int*' instead of DWORD* and GCC it's tooo strict
  return ::RegQueryValue(Key, subkeyName, data, (int*)dataSize);
#else
  return ::RegQueryValue(Key, subkeyName, data, (long*)dataSize);
#endif
}

//
// Enumerate the values associated with this key.  Copy the value name and
// data block associated with the passed index.
// Note: 32 bit only
//
inline long
TRegKey::EnumValue(int index, _TCHAR * valueName, uint32& valueNameSize,
                   uint32* type, uint8* data, uint32* dataSize) const
{
#if defined WINELIB   //it seems that Winlib SDK declares a 'int*' instead of DWORD* and GCC it's tooo strict
//  unsigned int auxV=valueNameSize;
  return ::RegEnumValue(Key, index, valueName, (unsigned int*)&valueNameSize, 0, (unsigned int*)type, data, (unsigned int*)dataSize);
#else
  return ::RegEnumValue(Key, index, valueName, &valueNameSize, 0, type, data, dataSize);
#endif
}

//----------------------------------------------------------------------------
// TRegValue inlines
//

//
// Return a string identifying this value
//
inline LPCTSTR
TRegValue::GetName() const
{
  return Name;
}

//
// Return the type code for the data associated with this value
//
inline const uint32
TRegValue::GetDataType() const
{
  return DataType;
}

//
// Return the size in bytes of the data associated with this value
//
inline const uint32
TRegValue::GetDataSize() const
{
  return DataSize;
}

//
// Return the data associated with this value.
//
inline const uint8*
TRegValue::GetData() const {
  RetrieveOnDemand();
  return Data;
}

//
// Return the data associated with this value as a 32bit unsigned integer
//
inline TRegValue::operator uint32() const
{
  return *(uint32*)GetData();           // Assumes dataType==REG_DWORD
}

//
// Return the data associated with this value as a const _TCHAR*
//
inline TRegValue::operator LPCTSTR() const
{
  return (LPCTSTR)GetData();  // Assumes DataType==REG_SZ or sim.
}

//
// Set the data associated with this value
// Note: 32 bit only
//
inline long
TRegValue::Set(uint32 data)
{
  return Set(REG_DWORD, (uint8*)&data, sizeof data);
}

//
// Set the data associated with this value
//
inline long
TRegValue::Set(LPCTSTR data)
{
//  TString strData(data);
//  w_char_t* ustrData = strData;
  return Set(REG_SZ, (uint8*)data, ::_tcslen(data));
}

//
// Remove this value from its associated key.
// Note: the state of this value object becomes undefined.
//
inline long
TRegValue::Delete()
{
  return Key.DeleteValue(Name);
}

//
// Set the data for this value to v
//
inline void
TRegValue::operator =(uint32 v)
{
  Set(DataType, (uint8*)&v, DataSize);
}

//
// Set the data for this value to v
//
inline void
TRegValue::operator =(LPCTSTR v)
{
  Set(DataType, (uint8*)v, ::_tcslen(v) + 1);
}

//----------------------------------------------------------------------------
// TRegKeyIterator inlines
//

//
// Create a subkey iterator for a registration key
//
inline
TRegKeyIterator::TRegKeyIterator(const TRegKey& key)
:
  Key(key),
  Index(0)
{
}

//
// Test the validity of this iterator.  True if the iterator's
// index is greater than or equal to 0 and less than the number
// of subkeys.
//
inline
TRegKeyIterator::operator bool()
{
  return Index >= 0 && Index < int(Key.GetSubkeyCount());
}

//
// Preincrement to the next subkey
//
inline uint32
TRegKeyIterator::operator ++()
{
  return ++Index;
}

//
// Postincrement to the next subkey
//
inline uint32
TRegKeyIterator::operator ++(int)
{
  return Index++;
}

//
// Predecrement to the previous subkey
//
inline uint32
TRegKeyIterator::operator --()
{
  return --Index;
}

//
// Postdecrement to the previous subkey
//
inline uint32
TRegKeyIterator::operator --(int)
{
  return Index--;
}

//
// Set the index of the iterator to the passed value.  Return the new index.
//
inline uint32
TRegKeyIterator::operator [](int index)
{
  PRECONDITION((index >= 0) && (index < int(Key.GetSubkeyCount())));
  return Index = index;
}

//
// Return the index to the current subkey
//
inline int
TRegKeyIterator::Current() const
{
  return Index;
}

//
// Return the registration key this iterator is bound to
//
inline const TRegKey&
TRegKeyIterator::BaseKey() const
{
  return Key;
}

//
// Reset the subkey index to zero
//
inline void
TRegKeyIterator::Reset()
{
  Index = 0;
}

//----------------------------------------------------------------------------
// TRegValueIterator inlines
//

//
// Create a value iterator for a registration key
//
inline
TRegValueIterator::TRegValueIterator(const TRegKey& regKey)
:
  Key(regKey),
  Index(0)
{
}

//
// Test the validity of this iterator.  True if the iterator's
// index is greater than or equal to 0 and less than the number
// of values.
//
inline
TRegValueIterator::operator bool()
{
  return Index >= 0 && Index < int(Key.GetValueCount());
}

//
// Preincrement to the next value
//
inline uint32
TRegValueIterator::operator ++()
{
  return ++Index;
}

//
// Postincrement to the next value
//
inline uint32
TRegValueIterator::operator ++(int)
{
  return Index++;
}

//
// Predecrement to the previous value
//
inline uint32
TRegValueIterator::operator --()
{
  return --Index;
}

//
// Postdecrement to the previous value
//
inline uint32
TRegValueIterator::operator --(int)
{
  return Index--;
}

//
// Set the index of the iterator to the passed value.  Return the new index.
//
inline uint32
TRegValueIterator::operator [](int index)
{
  PRECONDITION((index >= 0) && (index < int(Key.GetValueCount())));
  return Index = index;
}

//
// Return the index to the current value
//
inline int
TRegValueIterator::Current() const
{
  return Index;
}

//
// Return the registration key this iterator is bound to
//
inline const TRegKey&
TRegValueIterator::BaseKey() const
{
  return Key;
}

//
// Reset the value index to zero
//
inline void
TRegValueIterator::Reset()
{
  Index = 0;
}


//----------------------------------------------------------------------------
// TXRegistry inlines
//

//
// Creates a registry exception object.  msg points to an error message and
// key points to the name of the registry key that ObjectComponents was
// processing when the exception occurred.
//
inline
TXRegistry::TXRegistry(const owl_string& msg, const _TCHAR* key)
:
  TXBase(msg),
  Key(key)
{
}

//
// The copy constructor constructs a new registry exception object by copying
// the one passed as copy. 
//
inline
TXRegistry::TXRegistry(const TXRegistry& src)
:
  TXBase(src),
  Key(src.Key)
{
}



//----------------------------------------------------------------------------
// TRegList inlines
//

//
// Create a registration parameter table, composed of a list of TRegItems
//
inline
TRegList::TRegList(TRegItem* _list)
:
  Items(_list)
{
  PRECONDITION(_list);
}

//
// Return the value of the passed key as a const _TCHAR*
//
inline LPCTSTR
TRegList::operator[](LPCSTR key)
{
  PRECONDITION(key);
  return Lookup(key);
}


//----------------------------------------------------------------------------
// TRegLink inlines
//

//
// Registration link node destructor
//
inline
TRegLink::~TRegLink()
{
}

//
// Return a pointer to the next link
//
inline TRegLink*
TRegLink::GetNext() const {
  return Next;
}
inline void 
TRegLink::SetNext(TRegLink* link){ 
  Next = link;
}

//
// Return a pointer to registration parameter table (reglist)
//
inline TRegList&
TRegLink::GetRegList() const
{
  return *RegList;
}

//
// Protected constructor where the derived class must initialize all pointers
//
inline
TRegLink::TRegLink()
:
  Next(0),
  RegList(0)
{

}


//----------------------------------------------------------------------------
// TRegTemplateList
//

//
// Return the number of templates in this list
//
inline int
TRegTemplateList::GetCount() const
{
  return Count;
}

//
// Return the registry key upon which these templates are based
//
inline TRegKey&
TRegTemplateList::GetBaseKey()
{
  return BaseKey;
}

//
// Return the template string at the passed index.
// Note: The list is indexed beginning with 1 not 0.
//
inline LPCTSTR
TRegTemplateList::operator [](int i)
{
  PRECONDITION(i > 0 && i <= Count); //!CM added range check
  if (i <= 0 || i > Count)
    return 0;
  else
    return List[i-1];
}

//
// Disable all templates in this list
//
inline void
TRegTemplateList::DisableAll()
{
  memset(EnabledFlags, 0x80, Count);
}

//
// Enable all templates in this list
//
inline void
TRegTemplateList::EnableAll()
{
  memset(EnabledFlags, 0x00, Count);
}

//
// Enable the template at the passed index
// Note: The list is indexed beginning with 1 not 0.
//
inline void
TRegTemplateList::Enable(int i)
{
  PRECONDITION(i > 0 && i <= Count); //!CM added range check
  if (i > 0 && i <= Count)
    EnabledFlags[i-1] = 0;
}

//
// Activate the template at the passed index
// Note: The list is indexed beginning with 1 not 0.
//
inline void
TRegTemplateList::Activate(int i)
{
  PRECONDITION(i > 0 && i <= Count); //!CM added range check
  if (i > 0 && i <= Count)
    EnabledFlags[i-1]++;
}

//
// Returns true if the template at the passed index is active, false otherwise.
// Note: The list is indexed beginning with 1 not 0.
//
inline bool
TRegTemplateList::IsActive(int i) const
{
  PRECONDITION(i > 0 && i <= Count); //!CM added range check
  if (i > 0 && i <= Count)
    return EnabledFlags[i-1] > 0;
  else
    return false;
}

//----------------------------------------------------------------------------
// TRegParamList
//

//
// Return the number of param entries in this list
//
inline int
TRegParamList::GetCount() const
{
  return Count;
}

//
// Return the template string at the passed index.
//
inline TRegParamList::TEntry&
TRegParamList::operator [](int i)
{
  PRECONDITION(i >= 0 && i < Count); //!CM added range check
  if (i < 0 || i >= Count)
    TXRegistry::Check(1,0); //!CM Throw exception on bad index
  return List[i];
}


//
// Return the value of the param entry at the passed index
//
inline const _TCHAR*&
TRegParamList::Value(int i)
{
  PRECONDITION(i >= 0 && i < Count); //!CM added range check
  return Values[i];
}


__OWL_END_NAMESPACE


#endif  // OWL_REGISTRY_H
