//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// General Registry access & registration implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_REGISTRY_H)
#include <owl/registry.h>
#endif
#if !defined(OWL_SYSTEM_H)
#include <owl/system.h>
#endif
#if !defined(OWL_POINTER_H)
#include <owl/pointer.h>
#endif
#include <stdlib.h>  // atol()

__OWL_BEGIN_NAMESPACE

/////////////////////////////////////////////////////////////////////////////////
//
// Predefined constant registry keys
//
#if 0
TRegKey TRegKey::ClassesRoot(HKEY_CLASSES_ROOT, true, _T("HKEY_CLASSES_ROOT"));
#endif
#if 0
TRegKey TRegKey::ClassesRootClsid(ClassesRoot, _T("CLSID"), KEY_ALL_ACCESS, NoCreate);
#endif

//
//
//
TRegKey&
TRegKey::GetClassesRoot()
{
  static TRegKey ClassesRoot(HKEY_CLASSES_ROOT, true, _T("HKEY_CLASSES_ROOT"));
  return ClassesRoot;
}

//
//
//
TRegKey&
TRegKey::GetClassesRootClsid()
{
  static TRegKey ClassesRootClsid(GetClassesRoot(), _T("CLSID"), KEY_ALL_ACCESS, NoCreate);
  return ClassesRootClsid;
}

//
//
//
TRegKey&
TRegKey::GetCurrentUser()
{
  static TRegKey CurrentUser(HKEY_CURRENT_USER, true, _T("HKEY_CURRENT_USER"));
  return CurrentUser;
}

//
//
//
TRegKey&
TRegKey::GetLocalMachine()
{
  static TRegKey LocalMachine(HKEY_LOCAL_MACHINE, true, _T("HKEY_LOCAL_MACHINE"));
  return LocalMachine;
}

//
//
//
TRegKey&
TRegKey::GetUsers()
{
  static TRegKey Users(HKEY_USERS, true, _T("HKEY_USERS"));
  return Users;
}

//
//
//
TRegKey&
TRegKey::GetCurrentConfig()
{
  static TRegKey CurrentConfig(HKEY_CURRENT_CONFIG, true, _T("HKEY_CURRENT_CONFIG"));
  return CurrentConfig;
}

//
//
//
TRegKey&
TRegKey::GetDynData()
{
  static TRegKey DynData(HKEY_DYN_DATA, true, _T("HKEY_DYN_DATA"));
  return DynData;
}

//
//
//
TRegKey&
TRegKey::GetPerformanceData()
{
  static TRegKey PerformanceData(HKEY_PERFORMANCE_DATA, true, _T("HKEY_PERFORMANCE_DATA"));
  return PerformanceData;
}

//
// Different parameter usage & initial code for Win16 vs Win32
//
//
// Initialize the object with the passed-in data.
//
TRegKey::TRegKey(THandle baseKey, LPCTSTR keyName, REGSAM samDesired, TCreateOK createOK)
:
  Key(0),
  Name(strnewdup(keyName))
{
  if (createOK == CreateOK) {
    DWORD disposition;
    ::RegCreateKeyEx(baseKey, Name, 0, _T("")/*class*/, 
      REG_OPTION_NON_VOLATILE, samDesired, 
      0/*SecurityAttributes*/, &Key, &disposition);
  }
  else
    ::RegOpenKeyEx(baseKey, Name, 0, samDesired, &Key);

  if (Key)
    QueryInfo(0, 0, &SubkeyCount, 0, 0, &ValueCount, 0, 0, 0, 0);

  ShouldClose = true;

//  TXRegistry::Check(long(Key), Name);
}

//
// Initialize object with the key pointed to by the iterator.
//
TRegKey::TRegKey(const TRegKeyIterator& iter, REGSAM samDesired)
:
  Key(0),
  Name(new _TCHAR[_MAX_PATH+1])
{
  iter.BaseKey().EnumKey(iter.Current(), Name, _MAX_PATH+1);
  ::RegOpenKeyEx(iter.BaseKey(), Name, 0, samDesired, &Key);

  ShouldClose = true;
  QueryInfo(0, 0, &SubkeyCount, 0, 0, &ValueCount, 0, 0, 0, 0);
}

//Changes by Peter Sliepenbeek
#if (((__GNUC__ != 3) || (__GNUC_MINOR__ != 3)) && \
  ((__GNUC__ != 3) || (__GNUC_MINOR__ != 2)))

//
// Construct a TRegKey object around an existing key. Commonly used for
// wrapping the preexisting pseudo-keys.
//
//JJH commented out BI_COMP_GNUC
//#if !defined(BI_COMP_GNUC)
//#if !defined(__HPUX_SOURCE)
TRegKey::TRegKey(THandle aliasKey, bool shouldClose, LPCTSTR keyName)
:
  Key(aliasKey),
  Name(strnewdup(keyName))
{
  ShouldClose = shouldClose;

// copied from old unix owl (JAM 4-16-01)
// Added by Val Ovechkin 10:50 AM 5/22/98
// CC 5.1 filtered out Sun.  dont know if HP still needs this
//#if defined( __unix ) && !defined(__SUNPRO_CC)
//#define __try try
//#define __except catch
//#endif


  //JJH - WINELIB stuff
#  if !defined(WINELIB)
#    if !defined(BI_COMP_BORLANDC)
  __try 
#    else  //#if !defined(BI_COMP_BORLANDC)
  try
#    endif //#if !defined(BI_COMP_BORLANDC)
#  endif //#  if !defined(WINELIB)
  {
    long err = QueryInfo(0, 0, &SubkeyCount, 0, 0, &ValueCount, 0, 0, 0, 0);
    if (err != S_OK && err != ERROR_INSUFFICIENT_BUFFER)
      Key = 0;
  }//__try

  // Some key handles are unsupported & sometimes the OS crashes, & doesn't
  // just return an error. Catch it here & zero out this key.
  //
//  __except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ?
//              EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
//    Key = 0;
//  }
//  __endexcept
// JRS fix to get it to compile in OCC, this should be the same as above :)
  //JJH - WINELIB stuff
#  if !defined(WINELIB)
  __except(true)
  {
    if (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
      Key = 0;
    else
      RaiseException(GetExceptionCode(), 0, 0, 0);
  }
  __endexcept
#  endif //!defined(WINELIB)
}
//__HPUX_SOURCE #endif
//JJH commented out 
//#endif //!defined(BI_COMP_GNUC)

//Changes by Peter Sliepenbeek
#endif  // (((__GNUC__ != 3) || (__GNUC_MINOR__ != 3)) &&
        // ((__GNUC__ != 3) || (__GNUC_MINOR__ != 2)))

//
// Destructor closes the key if necessary and deletes any previously
// allocated memory by the object.
//
TRegKey::~TRegKey()
{
  if (Key && ShouldClose)
    ::RegCloseKey(Key);
  delete[] Name;
}

//
// Wrapper for RegQueryInfoKey API.
//
long
TRegKey::QueryInfo(LPTSTR class_, DWORD* classSize,
                   DWORD* subkeyCount,  DWORD* maxSubkeySize,
                   DWORD* maxClassSize, DWORD* valueCount,
                   DWORD* maxValueName, DWORD* maxValueData,
                   DWORD* secDescSize, PFILETIME lastWriteTime)
{

#if 0
//Jogy - Querying the class of a predefined root key can return ERROR_INVALID_PARAMETER.
  _TCHAR    db[256];
  DWORD    d1 = 256;
#endif

  DWORD    d2 = 0;
  DWORD    d3 = 0;
  DWORD    d4 = 0;
  DWORD    d5 = 0;
  DWORD    d6 = 0;
  DWORD    d7 = 0;
  DWORD    d8 = 0;
  FILETIME  ft;

  // NT 3.50 has trouble with 0 ptrs for some of these
  //
  return ::RegQueryInfoKey(
#if 0
                           Key, class_ ? class_ : db,
                           classSize ? classSize : &d1, 0,
#else
                           Key, class_, classSize, 0,
#endif
                           subkeyCount ? subkeyCount : &d2,
                           maxSubkeySize ? maxSubkeySize : &d3,
                           maxClassSize ? maxClassSize : &d4,
                           valueCount ? valueCount : &d5,
                           maxValueName ? maxValueName : &d6,
                           maxValueData ? maxValueData : &d7,
                           secDescSize ? secDescSize : &d8,
                           lastWriteTime ? lastWriteTime : &ft);
}

//
// Completely nuke a child key, including any of its subkeys. RegDeleteKey
// fails if a key has subkeys, so must tail-recurse to clean them up first.
//
long
TRegKey::NukeKey(LPCTSTR subKeyName)
{
  {
    TRegKey subKey(*this, subKeyName, KEY_ALL_ACCESS, TRegKey::NoCreate);

    // Don't increment since deleting & all subkeys will slide down to 0
    //
    for (TRegKeyIterator i(subKey); i; ) {
      TRegKey subSubKey(i);
      if (!subSubKey)
        break;     // All done, ran out before iterator knew, since deleting
      long err = subKey.NukeKey(subSubKey.GetName());
      if (err)
        return err;
    }
  }
  return DeleteKey(subKeyName);
}

//
// Create a value object named 'name' associated with the the current key.
//
TRegValue::TRegValue(const TRegKey& key, LPCTSTR name)
:
  Key(key), Name(name), AName(0), Data(0)
{
  DataSize = 0;
  Key.QueryValue(Name, &DataType, 0, &DataSize); // Get type & size only
}

//
// Create a value object pointed to by the current position of the iterator.
//
TRegValue::TRegValue(const TRegValueIterator& iter)
:
  Key(iter.BaseKey()), Data(0)
{
  uint32 nameSize = _MAX_PATH; //0;
  Name = AName = new _TCHAR[(int)nameSize];
  DataSize = 0;
  Key.EnumValue(iter.Current(), AName, nameSize, &DataType, 0, &DataSize);
}

//
// Destructor deletes any previously allocated memory.
//
TRegValue::~TRegValue()
{
  delete[] AName;
  delete[] Data;
}

//
// Retrieve the data for the value.
//
void
TRegValue::RetrieveOnDemand() const
{
  if (!Data) 
  {
    Data = new uint8[(int)DataSize];
    uint32 ds = DataSize;
    Key.QueryValue(Name, 0, Data, &ds);  // DataSize shouldn't change...
  }
}

//
// Set the data for the value.
//
long
TRegValue::Set(uint32 type, uint8* data, uint32 dataSize)
{
  long ret = Key.SetValue(Name, type, data, dataSize);
  if (ret == 0) 
  {
    DataType = type;
    if (dataSize == DataSize && Data != 0) 
	{
      memcpy(Data, data, (int)DataSize);  // resync w/ new data if same size
    }
    else 
	{
      DataSize = dataSize;           // Otherwise retrieve on demand later
      delete[] Data;
      Data = 0;
    }
  }
  return ret;
}

//----------------------------------------------------------------------------

//
// Registry exception checking. Throw a TXRegistry if arg is non-zero
//
void
TXRegistry::Check(long stat, LPCTSTR key)
{
  if (stat != 0 && !InstanceCount) {
    if (stat == 1)      // default bool true to a generic E_FAIL
      stat = E_FAIL;
    _TCHAR buf[100];
    if (key)
      wsprintf(buf, _T("Registry failure on key: %s, ErrorCode = %lX\n"),
               (LPCTSTR)key, stat);
    else
      wsprintf(buf, _T("Registry failure on unknown key: ErrorCode = %lX\n"),
               stat);
    WARN(stat != 0, buf);
    throw TXRegistry(buf, key);
  }
}

//----------------------------------------------------------------------------

//
// Initialize object with the passed-in data.
// Counts the number of items in the list.
//
TRegTemplateList::TRegTemplateList(TRegKey& basekey, LPCTSTR list[])
:
  BaseKey(basekey),
  List(list)
{
  PRECONDITION(List);

  for (Count = 0; List[Count]; Count++)
    ;
  EnabledFlags = new int8[Count];
}

//
// Destructor deletes any previously allocated memory.
//
TRegTemplateList::~TRegTemplateList()
{
  delete[] EnabledFlags;
}

//
// Enable items from the set.
//
void
TRegTemplateList::Enable(LPCTSTR set)
{
  for (LPCTSTR pc = set; *pc != 0; pc++)
    EnabledFlags[*pc - 1] = 0;                 // Selectively allow enable
}

//
// Activate the items in the set.
//
void
TRegTemplateList::Activate(LPCTSTR set)
{
  for (LPCTSTR pc = set; *pc != 0; pc++)
    EnabledFlags[*pc - 1]++;
}

//----------------------------------------------------------------------------

//
// Initialize the object with the passed-in data.
// Counts the number of items in the list.
//
TRegParamList::TRegParamList(TEntry* list)
:
  List(list)
{
  PRECONDITION(List);

  for (Count = 0; List[Count].Param; Count++)
    ;
  typedef LPCTSTR cpchar;
  Values = new cpchar[Count];
}

//
// Destructor deletes any previously allocated memory.
//
TRegParamList::~TRegParamList()
{
  delete[] Values;
}

//
// Reset all values to their default values.
//
void
TRegParamList::ResetDefaultValues()
{
  for (int i = Count; --i >= 0; )
    Values[i] = List[i].Default;
}

//
// Look for a given param in this param list
//
int TRegParamList::Find(LPCTSTR param)
{
  PRECONDITION(param);
  if (!param)
    return -1;

  int i = Count;
  while (--i >= 0) {
    if (_tcscmp(List[i].Param, param) == 0)
      break;
  }
  return i;
}

//----------------------------------------------------------------------------

//
// Initialize the symbol table with the data.
//
TRegSymbolTable::TRegSymbolTable(TRegKey& basekey, LPCTSTR tplList[],
                                 TRegParamList::TEntry* paramList)
:
  Templates(basekey, tplList),
  Params(paramList)
{
  UserKeyCount = 0;
}

//
// Enable all templates, or if filter is given enable only those
// Initialize paramater values with defaults
//
void
TRegSymbolTable::Init(_TCHAR* filter)
{
  if (filter) {
    Templates.DisableAll();
    Templates.Enable(filter);
  }
  else {
    Templates.EnableAll();
  }

  Params.ResetDefaultValues();
  UserKeyCount = 0;
}

//
// Scan through provided items assinging values from the item to the matching
// param. A langId is needed for locale translation on assignment.
//
void
TRegSymbolTable::UpdateParams(TLangId lang, TRegItem* item)
{
  for (; item && item->Key; item++) {
    // Note presence of user-specified key and value, process when streaming
    //
    if (*item->Key == ' ') {
      UserKeyCount++;
    }
    else {
      // Replace default with user-specified parameter value
      //
      _USES_CONVERSION;
      int i = Params.Find(_A2W(item->Key));
      TXRegistry::Check(i < 0, _A2W(item->Key));
      Params.Value(i) = item->Value.Translate(lang);

      // Activate all templates invoked by parameter
      //
      Templates.Activate(Params[i].TemplatesNeeded);
    }
  }
}

//
// Scan thru and process enabled templates, substituting filled in parameter
// values and streaming the resulting strings into the out stream provided.
//
void
TRegSymbolTable::StreamOut(TRegItem* item, _tostream& out)
{
#if defined(UNICODE)
  _USES_CONVERSION;
   TAPointer<_TCHAR> conv_buf = new _TCHAR[MAX_PATH];
#  define _A2WB_(val) _A2WB(val,(_TCHAR*)conv_buf)
#else
#  define _A2WB_(val) val
#endif
  for (int itpl = 1; itpl <= Templates.GetCount() || UserKeyCount--; itpl++) {

    // Setup pt from standard templates, or from userKeys in item prepended
    // by spaces.
    //
    LPCTSTR userval = 0;
    LPCTSTR pt;
    if (itpl <= Templates.GetCount()) {  // Processing standard template array
      if (!Templates.IsActive(itpl))
        continue;
      pt = Templates[itpl];
    }
    else {     // Now processing user-defined templates
      while (*(pt = _A2WB_(item->Key)) != _T(' '))
        item++;
      pt++;
      userval = item->Value;
    }

    // Walk thru template (pt) copying it into buf, replacing <params> on the
    // way.
    //
    const int bufSize  = 512;
    TAPointer<_TCHAR> buf = new _TCHAR[bufSize];

    _TCHAR*    data = 0; // Optional data for keys-value=data
    LPCTSTR   pc;       // Point where param token began
    _TCHAR*   pb = buf; // Working buffer write pointer
    for (;;) {
      TCHAR  c = *pt++;  // Current character being copied
      switch (c) {
        case _T('<'):
          pc = pb;      // Remember where param token starts
          continue;
        case _T('>'): {
          *pb = 0;        // Terminate param token

          // Lookup param in param list
          //
          int i = Params.Find(pc);
          TXRegistry::Check(i < 0, pc); // internal err

          // Now get the value obtained above
          //
          pb = (_TCHAR*)pc; // Move buffer ptr back
          TXRegistry::Check((pc = Params.Value(i)) == 0, Params[i].Param);

          if (*pc == 0 && *pt == _T(' '))
            pt++;

          // Copy value to buf
          //
          while (*pc != 0)
            *pb++ = *pc++;
          continue;
        }
        case _T('='):
          while (*(pb-1) == _T(' '))  // Remove trailing space before =
            pb--;
          *pb++ = 0;              // Terminate keys-value
          data = pb;              // Rest of pt will be data written from here
          while (*pt == _T(' '))
            pt++;
          continue;
        default:
          *pb++ = c;  // Copy character to buf. Param name will be overwriten
          continue;
        case 0:
          *pb++ = 0;
          if (!userval)
            break;
          pt = userval;
          userval = 0;
          data = pb;
          continue;
      }
      if (!c)
        break;
    }
    // Now write out a key with an optional assignment value to output stream
    //
    out
      << Templates.GetBaseKey().GetName()
      << _T('\\') << (LPCTSTR)buf;
    if (data)
      out << _T(" = ") << data;
    out << _T('\n');
  }
}

//----------------------------------------------------------------------------

//
// Walk thru an input stream and use "basekey\key\key=data" lines to set
// registry entries
// Has named value support in the form "basekey\key\key|valuename=data"
//
void TRegistry::Update(TRegKey& baseKey, _tistream& in)
{
  // Loop thru all lines in input stream
  //
  while (!in.eof()) {
    // Get line into entry buffer, skip over base key if it exists, ignore line
    // if it doesn't
    //
    _TCHAR entry[512];
    in.get(entry, ::_tcslen(baseKey.GetName())+1);
    if (_tcscmp(entry, baseKey.GetName()) != 0)
      continue;
    in.get(entry, 1+1);  // Eat the '\' separator
    in.getline(entry, 512);

    // Split entry into keys-value and data strings
    //
    _TCHAR* data = _tcschr(entry, _T('='));
    if (data) {
      _TCHAR* pc = data;
      while (*(pc-1) == _T(' '))
        pc--;
      *pc = 0;
      while (*(++data) == _T(' '))
        ;
    }
    else
      data = _T("");
    _TCHAR* valName = _tcschr(entry, _T('|'));
    if (valName)
      *valName++ = 0;  // Terminate key at value name separator

    // Set default value
    //
    if (valName) {
      TRegKey subKey(baseKey, entry);
      TXRegistry::Check(subKey.SetValue(valName, REG_SZ, (const uint8*)data, 0), entry);
    }
    else {
      TXRegistry::Check(baseKey.SetDefValue(entry, REG_SZ, data, 0), entry);
    }
  }
}

//
// Walk thru an input stream and use "basekey\key\key=data" lines to check
// registry entries.
// Return the number of differences. Zero means a complete match
//
// Has named value support in the form "basekey\key\key|valuename=data"
//
int TRegistry::Validate(TRegKey& baseKey, _tistream& in)
{
  int diffCount = 0;
  while (in) {
    // Get line into entry buffer, skip over hive key if it exists, ignore line
    // if it doesn't
    //
    _TCHAR entry[512];
    //Staso : Call get instead of getline - getline eats too much
    in.get(entry, ::_tcslen(baseKey.GetName())+1);  // 1 for '\', 1 for 0
    if (_tcscmp(entry, baseKey.GetName()) != 0)
      continue;
    in.get(entry, 1+1);  // Eat the '\' separator
    in.getline(entry, 512);

    // Split entry into keys-value and data strings
    //
    _TCHAR* data = _tcschr(entry, _T('='));
    if (data) {
      _TCHAR* pc = data;
      while (*(pc-1) == _T(' '))
        pc--;
      *pc = 0;
      while (*(++data) == _T(' '))
        ;
    }
    _TCHAR* valName = _tcschr(entry, _T('|'));
    if (valName)
      *valName = _T('\\');  // Replace value name separator with \ for query

    // Now lookup keys-value part to get its data and see if it matches the
    // data value from the instream
    //
    _TCHAR buf[300];
    uint32 bufSize = 300;
    if (baseKey.QueryDefValue(entry, buf, &bufSize) != S_OK
        || (data && _tcscmp(data, buf) != 0))
      diffCount++;
  }
  return diffCount;
}

//
// Unregisters entries given a reglist, an optional overrides regItem.
// Return the # of errors deleting keys
//
int
TRegistry::Unregister(TRegList& regInfo, TUnregParams* params, TRegItem* overrides)
{
#if defined(UNICODE)
  _USES_CONVERSION;
   TAPointer<_TCHAR> conv_buf = new _TCHAR[MAX_PATH];
#  undef _A2WB_
#  undef _W2AB_
#  define _A2WB_(val) _A2WB(val,(_TCHAR*)conv_buf)
#  define _W2AB_(val) _W2AB(val,(_TCHAR*)conv_buf)
#else
#  undef _A2WB_
#  undef _W2AB_
#  define _A2WB_(val) val
#  define _W2AB_(val) val
#endif
  int  errorCount = 0;

  // Loop thru unregister params to nuke each root level key, thus cleaning
  // up all nested keys too
  //
  for (int i = 0; params[i].Name && params[i].BaseKey; i++) {
    LPCTSTR regKey = regInfo[_W2AB_(params[i].Name)];

    // If the param not found & it matches the overrides item, then use that
    // value
    //
    if (!regKey && overrides && _tcscmp(_A2WB_(overrides->Key), params[i].Name) == 0)
      regKey = overrides->Value;

    // If key-value was found, unregister it
    //
    if (regKey) {
      _TCHAR buff[16];
      if (params[i].Prepend) { // Special case prepending char to key
        buff[0] = _T('.');
        ::_tcscpy(buff+1, regKey);
        regKey = buff;
      }

      // Nuke the key, using the basekey as a reference point
      //
      if (params[i].BaseKey->NukeKey(regKey))
        errorCount++;         // Should throw exception if certain errors?
    }
  }
  return errorCount;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
