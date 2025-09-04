//----------------------------------------------------------------------------
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:44 $
//
// TString (& TUString) implementation (others functions are inline)
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_WSYSCLS_H)
#include <owl/wsyscls.h>
#endif
#if !defined(OWL_STRING_H)
#include <owl/string.h>
#endif
#if !defined(OWL_PRIVATE_MEMORY_H)
#include <owl/private/memory.h>
#endif
#include <string.h>

__OWL_BEGIN_NAMESPACE

TUString TUString::Null;   // null TAutoStrings reference this object

#if defined(BI_HAS_WCHAR)
//
// Take a wide char string & return an ANSI string in a new'd char[] buffer
//
char* TUString::ConvertWtoA(const wchar_t* src, size_t len)
{
  size_t size;
  char* dst;
#if defined WINELIB
  //convert from UTF32 (wchar in linux are UTF32) to UTF16 (WCHAR)
  size_t s = wcslen(src);
  WCHAR *auxSrc = new WCHAR[s*2]; //maximum possible
  int i, j;
  for (i=0, j=0; i<s && src[i]; i++) {
    if (src[i]<=0xFFFF) 
	auxSrc[j++] = src[i];
    else { //convert into a surrogate pair
        wchar_t w=src[i]-0x10000;
	auxSrc[j++] = (w >> 10) + 0xD800;
	auxSrc[j++] = (w & 0x3FF)+0xDC00;
    }
  }
  auxSrc[j++] = 0 ;
  size = WideCharToMultiByte(CP_ACP, 0, auxSrc, len, 0, 0, 0, 0);
  dst = new char[size + (len != (size_t)-1)];  // room for null if fixed size
  size = WideCharToMultiByte(CP_ACP, 0, auxSrc, len, dst, size, 0, 0);
  delete auxSrc;
#else
  size = WideCharToMultiByte(CP_ACP, 0, src, len, 0, 0, 0, 0);
  dst = new char[size + (len != (size_t)-1)];  // room for null if fixed size
  size = WideCharToMultiByte(CP_ACP, 0, src, len, dst, size, 0, 0);
#endif
  if (len != (size_t)-1)
    dst[size] = 0;
  return dst;
}

//
// Take an ANSI char string & return a wide string in a new'd wchar_t[] buffer
//
wchar_t* TUString::ConvertAtoW(const char* src, size_t len)
{
  size_t size = MultiByteToWideChar(CP_ACP, 0, src, len, 0, 0);
  wchar_t* dst;
#if defined WINELIB
  WCHAR* auxDst = new WCHAR[size + (len != (size_t)-1)];
  size = MultiByteToWideChar(CP_ACP, 0, src, len, auxDst, size);
  if (len != (size_t)-1)
    auxDst[size] = 0;

  //Now convert from UTF16 to UTF32
  dst = new wchar_t[size + (len != (size_t)-1)];
  int i, j;
  for (i=0, j=0; i<size && auxDst[i]; i++) {
    if (auxDst[i]<0xD800 || auxDst[i]>0xDFFF) 
	dst[j++] = auxDst[i];
    else { //it's a surrogate pair
        dst[j++] = 0x10000 + (auxDst[i] - 0xD800) <<10 + (auxDst[i+1] - 0xDC00);
        i++;
    }
  }
  dst[j] = 0;
  delete auxDst;

#else
  dst = new wchar_t[size + (len != (size_t)-1)];
  size = MultiByteToWideChar(CP_ACP, 0, src, len, dst, size);
  if (len != (size_t)-1)
    dst[size] = 0;
#endif
  return dst;
}
#endif

#if !defined(BI_SUPPRESS_OLE)
//static 
BSTR TUString::ConvertAtoBSTR(const char* src)
{
  size_t size = MultiByteToWideChar(CP_ACP, 0, src, -1, 0, 0);
  wchar_t* pWide = (wchar_t*)alloca(sizeof(wchar_t)*(size+1));
  size = MultiByteToWideChar(CP_ACP, 0, src, -1, pWide, size);
  pWide[size] = 0;
  return ::SysAllocString(pWide);
}
#endif //#if !defined(BI_SUPPRESS_OLE)

//------------------------------------------------------------------------
//
// Change UString to isCopy regardless of current type
//
char* TUString::ChangeToCopy()
{
  char* dst = 0;
  const char * src = 0;
  int len = 0;
  switch (Kind) {
    case isNull:
      return 0;
    case isConst:
      src = Const;
      len = strlen(Const);
      break;
    case isCopy:
      return Copy;
#if defined(BI_HAS_WCHAR)
    case isWConst:
      dst = ConvertWtoA(WConst);
      break;
    case isWCopy:
      dst = ConvertWtoA(WCopy);
      break;
#endif
#if !defined(BI_SUPPRESS_OLE)
#  if defined(BI_OLECHAR_WIDE)
    case isBstr:
    case isExtBstr:
      dst = ConvertWtoA(Bstr, TOleAuto::SysStringLen(Bstr));
      break;
#  else
    case isBstr:
    case isExtBstr:
      src = Bstr;
      len = TOleAuto::SysStringLen(Bstr);
      break;
#  endif
#endif
    case isString:
#if defined(UNICODE)
      dst = ConvertWtoA(GetOWLString().c_str(), GetOWLString().length());
#else
      src = GetOWLString().c_str();
      len = GetOWLString().length();
#endif
    default: //JJH added empty default construct
      break;
  }
  if (!dst) 
  {
    dst = new char[len+1];
    memcpy(dst, src, len+1);
  }
  Free();
  Kind = isCopy;
  Copy = dst;
  return Copy;
}

#if defined(BI_HAS_WCHAR)
//
// Change UString to isWCopy regardless of current type
//
wchar_t* TUString::ChangeToWCopy()
{
  wchar_t* dst = 0;
  const wchar_t* src = 0;
  int len = 0;
  switch (Kind) {
    case isNull:
      return 0;
    case isConst:
      dst = ConvertAtoW(Const);
      break;
    case isCopy:
      dst = ConvertAtoW(Copy);
      break;
    case isWConst:
      src = WConst;
      len = ::wcslen(WConst);
      break;
    case isWCopy:
      return WCopy;
#if !defined(BI_SUPPRESS_OLE)
#if defined(BI_OLECHAR_WIDE)
    case isBstr:
    case isExtBstr:
      src = Bstr;
      len = TOleAuto::SysStringLen(Bstr);
      break;
#else                // This case probably never happens
    case isBstr:
    case isExtBstr:
      dst = ConvertAtoW(Bstr, TOleAuto::SysStringLen(Bstr));
      break;
#endif
#endif
    case isString:
#if defined(UNICODE)
      src = GetOWLString().c_str();
      len = GetOWLString().length();
#else
      dst = ConvertAtoW(GetOWLString().c_str(), GetOWLString().length());
#endif
  }
  if (!dst) {
    dst = new wchar_t[len+1];
    memcpy(dst, src, (len+1) * sizeof(wchar_t));
  }
  Free();
  Kind = isWCopy;
  WCopy = dst;
  return WCopy;
}
#endif

#if !defined(BI_SUPPRESS_OLE)
BSTR TUString::ConvertToBSTR()
{
  BSTR dst = NULL;
  switch (Kind) {
    case isNull:
      return 0;
    case isConst:
      dst = ConvertAtoBSTR(Const);
      break;
    case isCopy:
      dst = ConvertAtoBSTR(Copy);
      break;
    case isWConst:
      dst = ::SysAllocString(WConst);
      break;
    case isWCopy:
      dst = ::SysAllocString(WConst);
      break;
    case isBstr:
    case isExtBstr:
      return Bstr;
    case isString:
#if defined(UNICODE)
      dst = ::SysAllocString(GetOWLString().c_str());
#else
      dst = ConvertAtoBSTR(GetOWLString().c_str());
#endif
      break;
    default:
      CHECK(!"Unexpected Kind of TUString");
      return 0;
  }
  Free();
  Kind = isBstr;
  Bstr = dst;
  return Bstr;
}
#endif //#if !defined(BI_SUPPRESS_OLE)

//------------------------------------------------------------------------
// inline ctors used by Create functions.
// Note: these are never made public or exported from this unit
//

//
inline TUString::TUString(const char & str)
:
  Lang(0), Kind(isConst), RefCnt(1), Const(&str)
{
}

//
inline TUString::TUString(char& str)
:
  Lang(0),Kind(isCopy), RefCnt(1)
{
  Copy = new char[strlen(&str)+1];
  strcpy(Copy, &str);
}

#if defined(BI_HAS_WCHAR)

//
inline TUString::TUString(const wchar_t& str)
:
  Lang(0), Kind(isWConst), RefCnt(1), WConst(&str)
{
}
  
//
inline TUString::TUString(wchar_t& str)
:
  Lang(0), Kind(isWCopy), RefCnt(1)
{
  WCopy = new wchar_t[::wcslen(&str)+1];
  ::wcscpy(WCopy, &str);
}
#endif

#if !defined(BI_SUPPRESS_OLE)
//
inline TUString::TUString(BSTR str, bool loan, TLangId lang)
:
  Lang(lang),
  Kind(loan ? isExtBstr : isBstr),
  RefCnt(int16(loan ? 2 : 1)), 
  Bstr(str)
{
}

//
inline TUString::TUString(TSysStr& str, bool loan, TLangId lang)
:
  Lang(lang),
  Kind(loan ? isExtBstr : isBstr),
  RefCnt(int16(loan ? 2 : 1)),
  Bstr(str.operator BSTR())
{
}
#endif
//
//inline void* operator new(size_t, TStringRef** p) {return p;}

//
inline TUString::TUString(const owl_string& str)
:
  Lang(0), Kind(isString), RefCnt(1)
{
  AllocOWLString(str);
}

//------------------------------------------------------------------------
// Static creation, or factory functions return pointers to new UStrings, or
// pointers to the Null UString
// 

//
TUString* TUString::Create(const char * str)
{
  return str /*&& *str*/ ? new TUString(*str) : &++Null;
}

//
TUString* TUString::Create(char* str)
{
  return str /*&& *str*/ ? new TUString(*str) : &++Null;
}

#if defined(BI_HAS_WCHAR)

//
TUString* TUString::Create(const wchar_t* str)
{
  return str /*&& *str*/ ? new TUString(*str) : &++Null;
}

//
TUString* TUString::Create(wchar_t* str)
{
  return str /*&& *str*/ ? new TUString(*str) : &++Null;
}
#endif

#if !defined(BI_SUPPRESS_OLE)
//
TUString* TUString::Create(BSTR str, bool loan, TLangId lang)
{
  if (str && TOleAuto::SysStringLen(str))
    return new TUString(str, loan, lang);
  if (!loan)
    TOleAuto::SysFreeString(str);
  return &++Null;
}

//
TUString* TUString::Create(TSysStr& str, bool loan, TLangId lang)
{
  return Create(str.operator BSTR(), loan, lang);
}
#endif
//
TUString* TUString::Create(const owl_string& str)
{
  return str.length() ? new TUString(str) : &++Null;
}

//------------------------------------------------------------------------
// 
//

//
TUString* TUString::Assign(const TUString& s)
{
  if (RefCnt == 1 && Kind != isNull && Kind != isExtBstr)
    Free();
  else
    --*this;

  CONST_CAST(TUString&,s).RefCnt++;
  return &CONST_CAST(TUString&,s);
}

//
TUString* TUString::Assign(const owl_string& s)
{
  if (s.length() && RefCnt == 1 && Kind != isNull && Kind != isExtBstr) 
  {
    Free();
    Kind = isString;
    AllocOWLString(s);
    return this;
  }
  else 
  {
    --*this;
    return Create(s);
  }
}

//
TUString* TUString::Assign(const char * s)
{
  if (s && *s && RefCnt == 1 && Kind != isNull && Kind != isExtBstr) {
    Free();
    Kind = isConst;
    Const = s;
    return this;
  }
  else {
    --*this;
    return Create(s);
  }
}

//
TUString* TUString::Assign(char* s)
{
  if (s && *s && RefCnt == 1 && Kind != isNull && Kind != isExtBstr) {
    Free();
    Kind = isCopy;
    Copy = new char[strlen(s)+1];
    strcpy(Copy, s);
    return this;
  }
  else {
    --*this;
    return Create(s);
  }
}

#if defined(BI_HAS_WCHAR)

//
TUString* TUString::Assign(const wchar_t* s)
{
  if (s && *s && RefCnt == 1 && Kind != isNull && Kind != isExtBstr) {
    Free();
    Kind = isWConst;
    WConst = s;
    return this;
  }
  else {
    --*this;
    return Create(s);
  }
}

//
TUString* TUString::Assign(wchar_t* s)
{
  if (s && *s && RefCnt == 1 && Kind != isNull && Kind != isExtBstr) {
    Free();
    Kind = isWCopy;
    WCopy = new wchar_t[::wcslen(s)+1];
    ::wcscpy(WCopy, s);
    return this;
  }
  else {
    --*this;
    return Create(s);
  }
}
#endif

#if !defined(BI_SUPPRESS_OLE)
//
TUString* TUString::Assign(BSTR str, TLangId lang)
{
  if (RefCnt==1 && Kind != isNull && Kind != isExtBstr) {
    Free();
    Kind = isBstr;
    Bstr = str;
    Lang = lang;
    if (Bstr && TOleAuto::SysStringLen(Bstr))
      return this;
    delete this;
    return &++Null;
  }
  else {
    --*this;
    return Create(str, false, lang);
  }
}
#endif
//------------------------------------------------------------------------

//
TUString::operator const char *() const
{
  switch (Kind) {
    case isNull:    return 0;
    case isConst:   return Const;
    case isCopy:    return Copy;    
#if defined(UNICODE)
    case isString:  return CONST_CAST(TUString*,this)->ChangeToCopy();
#else
    case isString:  return GetOWLString().c_str();
#endif
#if !defined(BI_SUPPRESS_OLE)
#  if defined(BI_OLECHAR_WIDE)
    case isBstr:
    case isExtBstr: return CONST_CAST(TUString*,this)->ChangeToCopy();
#  else
    case isBstr:
    case isExtBstr: return Bstr;
#  endif
#endif
    case isWConst:
    case isWCopy:   return CONST_CAST(TUString*,this)->ChangeToCopy();
    default: break; //JJH added empty default construct
  }
  return 0; // suppress warning
}

//
TUString::operator char*()
{
  return ChangeToCopy();
}

#if defined(BI_HAS_WCHAR)

//
TUString::operator const wchar_t*() const
{
  switch (Kind) {
    case isNull:    return 0;
    case isWConst:  return WConst;
    case isWCopy:   return WCopy;    
#if !defined(BI_SUPPRESS_OLE)
#  if defined(BI_OLECHAR_WIDE)
    case isBstr:
    case isExtBstr: return Bstr;
#  else
    case isBstr:
    case isExtBstr: return CONST_CAST(TUString*,this)->ChangeToWCopy();
#  endif
#endif
    case isConst:
    case isCopy: 
#if defined(UNICODE)
      return CONST_CAST(TUString*,this)->ChangeToWCopy();
    case isString:  GetOWLString().c_str();
#else
    case isString:  return CONST_CAST(TUString*,this)->ChangeToWCopy();
#endif
  }
  return 0; // suppress warning
}

//
TUString::operator wchar_t*()
{
  return ChangeToWCopy();
}
#endif

//------------------------------------------------------------------------

//
// 
//
int TUString::Length() const
{
  switch (Kind) {
    case isNull:    return 0;
#if defined(BI_HAS_WCHAR)
    case isWConst:  return ::wcslen(WConst);
    case isWCopy:   return ::wcslen(WCopy);
#endif
#if !defined(BI_SUPPRESS_OLE)
    case isBstr:
    case isExtBstr: return TOleAuto::SysStringLen(Bstr);
#endif
    case isConst:   return strlen(Const);
    case isCopy:    return strlen(Copy);
    case isString:  return GetOWLString().length();
      default: break; //JJH added empty default construct
}
  return 0; // suppress warning
}

//------------------------------------------------------------------------

#if !defined(BI_SUPPRESS_OLE)
//
// Revoke BSTR ownership from this UString, i.e. make this UString relinquish
//
void TUString::RevokeBstr(BSTR s)
{
  if (Kind != isExtBstr || Bstr != s)  // Don't have it anymore
    return;
  if (RefCnt == 1) {   // We go away. Assume that our envelope knows about this!
    Kind = isNull;
    delete this;
    return;
  }
#if defined(BI_OLECHAR_WIDE)
  ChangeToWCopy();     // Make an appropriate copy of it
#else
  ChangeToCopy();
#endif
}

//
// Pass BSTR ownership to this UString
//
void TUString::ReleaseBstr(BSTR s)
{
  if (Kind == isExtBstr && Bstr == s) {
    Kind = isBstr;
    --*this;
  }
  else       // Has been overwritten with converted type, don't need anymore
    TOleAuto::SysFreeString(s);
}
#endif
//
// Free any resources held by this UString. Union & Kind left in random state;
// must be reinitialized before use.
//
void TUString::Free()
{
  switch (Kind) {
    case isCopy:    delete[] Copy; break;
#if defined(BI_HAS_WCHAR)
    case isWCopy:   delete[] WCopy; break;
#endif
#if !defined(BI_SUPPRESS_OLE)
    case isBstr:    TOleAuto::SysFreeString(Bstr); break;
#endif
    case isString:  GetOWLString().~owl_string(); break;
    default: break; //JJH added empty default construct
  }
  Lang = 0;
//Kind = isNull;  // for safety, not really needed
}

__OWL_END_NAMESPACE
/* ========================================================================== */

