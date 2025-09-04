//----------------------------------------------------------------------------
// Borland Services Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Implementation of memory manipulation functions
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_PRIVATE_MEMORY_H)
#include <owl/private/memory.h>
#endif
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif

__OWL_USING_STD_NAMESPACE

//__OWL_BEGIN_NAMESPACE

//
// Make a duplicate of a 0-terminated string using new _TCHAR[]
//
_OWLFUNC(char*)
strnewdup(const char* s, size_t allocSize)
{
  if (!s)
    s = "";
  int alloc = max(strlen(s)+1, allocSize);
  return strcpy(new char[alloc], s);
}

//
// Make a duplicate of a 0-terminated string using new _TCHAR[] 
//

//
#  if defined(__CYGWIN__) //|| defined (WINELIB), GCC 4.x already provides these functions
//
// Wide string copy function.
//
_OWLFUNC(wchar_t*)
wcscpy(wchar_t* dst, const wchar_t* src)
{
  wchar_t* p = dst;
  while ((*p++ = *src++) != 0)
    ;
  return dst;
}

//
// Wide string length function.
//
_OWLFUNC(size_t)
wcslen(const wchar_t* str)
{
  const wchar_t* p = str;
  for (; *p; p++)
    ;
  return p - str;
}
#  elif defined(BI_COMP_BORLANDC) && (__BORLANDC__ <= 0x530)

//
// Wide string copy function.
//
_OWLFUNC(wchar_t*)
_tcscpy(wchar_t* dst, const wchar_t* src)
{
  wchar_t* p = dst;
  while ((*p++ = *src++) != 0)
    ;
  return dst;
}

//
// Wide string length function.
//
_OWLFUNC(size_t)
_tcslen(const wchar_t* str)
{
  const wchar_t* p = str;
  for (; *p; p++)
    ;
  return p - str;
}

#  endif //defined(BI_COMP_BORLANDC) && (__BORLANDC__ >= 0x530)

//
// Make a duplicate of a wide 0-terminated string using new wchar_t[]
//
_OWLFUNC(wchar_t*)
strnewdup(const wchar_t* s, size_t allocSize)
{
  if (!s)
    s = L"";
  int alloc = max((size_t)::wcslen(s)+1, allocSize);
  return ::wcscpy(new wchar_t[alloc], s);
}


/////////////////////////////////////////////////////////////////////////
#if defined(BI_MULTI_THREAD_RTL)
#  include <owl/thread.h>
#endif

__OWL_BEGIN_NAMESPACE

//
// internal structure __TFixedAlloc
//
struct __TFixedAlloc
#if defined(BI_MULTI_THREAD_RTL)
: public TLocalObject
#endif
{
  __TFixedAlloc();
  ~__TFixedAlloc()
  {
  }

  TStandardBlocks* GetAlloc(int index)
      {
        PRECONDITION(this);
        PRECONDITION(index < 12);
        PRECONDITION(index >= 0);
        return Alloc[index];
      }

  TStandardBlocks  Alloc16;
  TStandardBlocks  Alloc32;
  TStandardBlocks  Alloc64;
  TStandardBlocks  Alloc128;
  TStandardBlocks  Alloc256;
  TStandardBlocks  Alloc512;
  TStandardBlocks  Alloc1024;
  TStandardBlocks  Alloc2048;
  TStandardBlocks  Alloc4096;
  TStandardBlocks  Alloc8192;
  TStandardBlocks  Alloc16384;
  TStandardBlocks  Alloc32768;

  TStandardBlocks*  Alloc[12];
};
//
__TFixedAlloc::__TFixedAlloc()
:
  Alloc16(16u,6),
  Alloc32(32u,6),
  Alloc64(64u,6),
  Alloc128(128u,4),
  Alloc256(256u,4),
  Alloc512(512u,4),
  Alloc1024(1024u,3),
  Alloc2048(2048u,3),
  Alloc4096(4096u,3),
  Alloc8192(8192u,2),
  Alloc16384(16384u,2),
  Alloc32768(32768u,2)
{
  Alloc[0] = &Alloc16;
  Alloc[1] = &Alloc32;
  Alloc[2] = &Alloc64;
  Alloc[3] = &Alloc128;
  Alloc[4] = &Alloc256;
  Alloc[5] = &Alloc512;
  Alloc[6] = &Alloc1024;
  Alloc[7] = &Alloc2048;
  Alloc[8] = &Alloc4096;
  Alloc[9] = &Alloc8192;
  Alloc[10] = &Alloc16384;
  Alloc[11] = &Alloc32768;
}
//
static __TFixedAlloc& GetFixedAlloc()
{
#if defined(BI_MULTI_THREAD_RTL)
  static TTlsContainer<__TFixedAlloc> __FixedAlloc;
#else
  static __TFixedAlloc __FixedAlloc;
#endif
  return __FixedAlloc;
};

///////////////////////
//
// class TTmpBufferBase
// ~~~~~ ~~~~~~~~~~~~~~
//
TTmpBufferBase::TTmpBufferBase(int size)
{
  PRECONDITION(size);

  static int szArray[] = {
    16u,32u,64u,128u,256u,512u,1024u,2048u,4096u,8192u,16384u,32768u
  };

  int index;
  for(index = 0; index < (int)(COUNTOF(szArray)); index++){
    if(szArray[index] > size)
      break;
  }
  if(index < (int)(COUNTOF(szArray))){
    Buffer  = GetFixedAlloc().GetAlloc(index)->Allocate(szArray[index]);
    Index    = index;
  }
  else{
    Buffer  = (void*)new uint8[size];
    Index    = -1;
  }
}
//
TTmpBufferBase::~TTmpBufferBase()
{
  if(Index >= 0)
    GetFixedAlloc().GetAlloc(Index)->Free(Buffer);
  else
    delete[] (uint8*)Buffer;
}

#if defined (__GNUC__) //JJH
void* TTmpBufferBase::operator new(size_t /*sz*/) throw()
#else //vc7
void* TTmpBufferBase::operator new(size_t /*sz*/)
#endif
{
 CHECK(false);
 return 0;
}


__OWL_END_NAMESPACE
//////////////////////////////
