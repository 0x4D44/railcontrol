//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// Classes for window system structure and type encapsulation
//----------------------------------------------------------------------------

#if !defined(OWL_WSYSCLS_H)
#define OWL_WSYSCLS_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif

#if !defined(OWL_GEOMETRY_H)
# include <owl/geometry.h>     // TPoint, et. al.
#endif

#if !defined(BI_NOTEMPLATE_H) && !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif
#include <shellapi.h>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_DECLCLASS(ipstream);
__OWL_DECLCLASS(opstream);


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TResId
// ~~~~~ ~~~~~~
// Resource Id class that can be constructed from a integer or string resource
// identifier. Does not own or delete the string.
//

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif



class TResId {
  public:
    // Constructors
    //
    TResId() : Num(0) {}
    TResId(const _TCHAR * resStr) : Str(resStr) {}
    TResId(int resNum) : Num(unsigned(resNum)) {}

    // Type Conversion operators & information
    //
    operator _TCHAR *() {return (_TCHAR *)Str;}
    bool IsString() const {return HiUint16(Num) != 0;}

  private:
    union {
      uint32            Num;  // Assumes that these 32bit fields will overlay
      const _TCHAR * Str;
    };

  friend _OWLCFUNC(ipstream&) operator >>(ipstream& is, TResId& id);
  friend _OWLCFUNC(opstream&) operator <<(opstream& os, const TResId& id);
#ifdef BI_STD_IO
  friend _OWLCFUNC(std::ostream&)  operator <<(std::ostream& os, const TResId& id);
#else
  friend _OWLCFUNC(ostream&)  operator <<(ostream& os, const TResId& id);
#endif
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE
//
// MSW only classes
//
#if defined(BI_PLAT_MSW)

# if !defined(OWL_WSYSINC_H)
#   include <owl/wsysinc.h>
# endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TDropInfo
// ~~~~~ ~~~~~~~~~
// Encapsulation of a system drag-drop event object in an HDROP handle.
//
class _OWLCLASS TDropInfo {
  public:
    // Constructors
    //
    TDropInfo(HDROP handle) : Handle(handle) {}

    // Type Conversion operators
    //
    operator HDROP() {return Handle;}

    // Information access
    //
    uint DragQueryFile(uint index, LPTSTR name, uint nameLen);
    uint DragQueryFileCount();
    uint DragQueryFileNameLen(uint index);
    bool DragQueryPoint(TPoint& point);
    void DragFinish();

  private:
    HDROP  Handle;
};

//
// class TFileDrop
// ~~~~~ ~~~~~~~~~
// Encapsulation of information about a single dropped file, its name, where
// it was dropped, and whether or not it was in the client area
//
class _OWLCLASS TFileDroplet {
  public:
    TFileDroplet(LPCTSTR fileName, const TPoint& p, bool inClient);
    TFileDroplet(TDropInfo& drop, int i);
   ~TFileDroplet();

    bool operator ==(const TFileDroplet& other) const {return &other == this;}

    LPCTSTR    GetName() const {return FileName;}
    TPoint    GetPoint() const {return Point;}
    bool      GetInClientArea() const {return InClientArea;}

  private:
    LPTSTR  FileName;
    TPoint  Point;
    bool    InClientArea;

    // Hidden to prevent accidental copying or assignment
    //
    TFileDroplet(const TFileDroplet&);
    TFileDroplet& operator =(const TFileDroplet&);
};

template <class T>            class TIPtrArray;
template <class T, class T1>  class TPtrArrayIterator;

typedef TIPtrArray<TFileDroplet*>   TFileDropletList;
typedef TPtrArrayIterator<TFileDroplet*, TFileDropletList > TFileDropletListIter;

//
// class TProcInstance
// ~~~~~ ~~~~~~~~~~~~~
// A ProcInstance object. This encapsulates the MakeProcInstance call, which is
// really only needed in old Win3.X real mode. This exists now for Owl 2.x
// compatibility only
//
class TProcInstance {
  public:
    TProcInstance(FARPROC p) {Instance = FARPROC(p);}

    operator FARPROC() {return Instance;}

  private:
    FARPROC Instance;
};



//
// FILETIME and SYSTEMTIME  wrippers
//
class _OWLCLASS TSystemTime;
class _OWLCLASS TTime;

class _OWLCLASS TFileTime: public FILETIME {
  public:
    TFileTime() { dwLowDateTime=0; dwHighDateTime=0;}
    TFileTime(const TSystemTime& tm);
    TFileTime(const TFileTime& tm){memcpy(this, &tm, sizeof(tm));}
    TFileTime(const FILETIME& tm){memcpy(this, &tm, sizeof(tm));}
    TFileTime(const TTime& tm);
    TFileTime& operator=(const TTime& tm);

    bool ToLocalTime();
    bool ToLocalTime(TFileTime& tm) const 
          { return ::FileTimeToLocalFileTime((FILETIME*)this, &tm); }

    bool ToUniversalTime();
    bool ToUniversalTime(TFileTime& tm) const 
          { return ::LocalFileTimeToFileTime(this, &tm); }

    // Boolean operators.
    bool operator <  ( const TFileTime & t ) const;
    bool operator <= ( const TFileTime & t ) const;
    bool operator >  ( const TFileTime & t ) const;
    bool operator >= ( const TFileTime & t ) const;
    bool operator == ( const TFileTime & t ) const;
    bool operator != ( const TFileTime & t ) const;

    // Read or write times on persistent streams
    friend _OWLCFUNC(opstream &) operator << ( opstream  & s, const TFileTime  & d );
    friend _OWLCFUNC(ipstream &) operator >> ( ipstream  & s, TFileTime  & d );
};


//
// class TSystemTime
// ~~~~~ ~~~~~~~~~~~
//
class _OWLCLASS TSystemTime: public SYSTEMTIME {
  public:
    TSystemTime() { memset(this,0,sizeof(SYSTEMTIME)); }
    TSystemTime(uint y, uint m, uint d, uint h=0, uint mn=0, uint s=0, uint dw=0,uint ms=0);
    TSystemTime(const TSystemTime& tm) { memcpy(this, &tm, sizeof(tm)); }
    TSystemTime(const TFileTime& tm)   { ::FileTimeToSystemTime(&tm, this); }
    TSystemTime(const SYSTEMTIME& tm)  { memcpy(this, &tm, sizeof(tm)); }

    uint16   GetYear()         { return wYear;         }
    uint16   GetYear() const    { return wYear;         }
    void     SetYear(uint y)   { wYear = (uint16)y;    }

    uint16   GetMonth()         { return wMonth;        }
    uint16   GetMonth() const  { return wMonth;        }
    void     SetMonth(uint m)  { wMonth = (uint16)m;    }

    uint16   GetDayOfWeek()     { return wDayOfWeek;    }
    uint16   GetDayOfWeek() const { return wDayOfWeek;    }
    void     SetDayOfWeek(uint dw)  { wDayOfWeek = (int16)dw;  }

    uint16   GetDay()           { return wDay;          }
    uint16   GetDay() const    { return wDay;          }
    void     SetDay(uint d)     { wDay = (uint16)d;      }

    uint16  GetHour()         { return wHour;          }
    uint16  GetHour() const    { return wHour;          }
    void    SetHour(uint h)   { wHour = (uint16)h;    }

    uint16   GetMinute()       { return wMinute;        }
    uint16   GetMinute() const  { return wMinute;        }
    void    SetMinute(uint m)  { wMinute = (uint16)m;  }

    uint16   GetSecond()       { return wSecond;        }
    uint16   GetSecond() const  { return wSecond;        }
    void    SetSecond(uint s) { wSecond = (uint16)s;  }

    uint16   GetMilliseconds()  { return wMilliseconds;  }
    uint16   GetMilliseconds()  const { return wMilliseconds;  }
    void    SetMilliseconds(uint ms){ wMilliseconds = (uint16)ms;  }

    TFileTime GetFileTime() { return TFileTime(*this);}
    static TSystemTime LocalTime()  { TSystemTime tm; ::GetLocalTime(&tm); return tm;  }
    static TSystemTime SystemTime() { TSystemTime tm; ::GetSystemTime(&tm); return tm; }

    TSystemTime& operator=(const TFileTime& ft)   { *this = TSystemTime(ft); return *this; }

    // Boolean operators.
    bool operator <  ( const TSystemTime  & t ) const;
    bool operator <= ( const TSystemTime  & t ) const;
    bool operator >  ( const TSystemTime  & t ) const;
    bool operator >= ( const TSystemTime  & t ) const;
    bool operator == ( const TSystemTime  & t ) const;
    bool operator != ( const TSystemTime  & t ) const;

};

//
// TResource simplifies access to a resource by encapsulating
// the find, load, lock and free steps for accessing a resource.
//   - 'T' represents a structure which defines the binary layout of the
//     resource.
//   - 'resType' is a constant string that defines the resource type.
//
// For example,
//
//     typedef TResource<DLGTEMPLATE, (int)RT_DIALOG> TDlgResource;
//     TDlgResource dlgInfo(hInstance, IDD_ABOUTDLG);
//     DLGTEMPLATE* pDlgTmpl = dlgInfo;
//
//template <class T, const _TCHAR _FAR* resType>
template <class T, int resType> class /*_OWLCLASS*/ TResource {
  public:
    TResource(HINSTANCE hModule, TResId resId);
    TResource(HINSTANCE hModule, TResId resid, LANGID langid);
   ~TResource();
    bool      IsOK() const;     // Confirms whether resource was found
    operator  T*();             // Conversion operator to point to
                                // structure representing binary layout
                                // of the resource...
  protected:
    HGLOBAL   MemHandle;        // Handle of resource
    T*        MemPtr;           // Pointer to locked resource
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// inlines
//
inline bool TFileTime::operator <  ( const TFileTime  & t ) const{
//JJH
#ifdef __GNUC__
  return ::CompareFileTime((FILETIME*) this, (FILETIME*) &t) < 0;
#else
  return ::CompareFileTime(this, &t) < 0;
#endif
}

//
inline bool TFileTime::operator <= ( const TFileTime  & t ) const{
  return !(*this > t);
}

//
inline bool TFileTime::operator >  ( const TFileTime  & t ) const{
//JJH
#ifdef __GNUC__
  return ::CompareFileTime((FILETIME*) this, (FILETIME*) &t) > 0;
#else
  return ::CompareFileTime((FILETIME*) this, (FILETIME*) &t) > 0;
#endif
}

//
inline bool TFileTime::operator >= ( const TFileTime  & t ) const{
  return !(*this < t);
}

//
inline bool TFileTime::operator != ( const TFileTime  & t ) const{
  return !(*this == t);
}

//
inline bool TFileTime::operator == ( const TFileTime  & t ) const{
//JJH
#ifdef __GNUC__
  return ::CompareFileTime((FILETIME*)this, (FILETIME*)&t) == 0;
#else
  return ::CompareFileTime(this, &t) == 0;
#endif
}

//
inline TFileTime::TFileTime(const TSystemTime& tm) {
  ::SystemTimeToFileTime(&tm, this);
}

//
inline TSystemTime::TSystemTime(uint y, uint m, uint d, uint h, uint mn,
                                uint s, uint dw, uint ms)
{
  wYear=(uint16)y;
  wMonth=(uint16)m;
  wDay=(uint16)d;
  wHour=(uint16)h;
  wMinute=(uint16)mn;
  wSecond= (uint16)s;
  wDayOfWeek=(uint16)dw;
  wMilliseconds=(uint16)ms;
}

//
inline bool TSystemTime::operator < ( const TSystemTime  & t ) const{
  return TFileTime(*this) < TFileTime(t);
}

//
inline bool TSystemTime::operator <= ( const TSystemTime  & t ) const{
  return !(*this > t);
}

//
inline bool TSystemTime::operator >  ( const TSystemTime  & t ) const{
  return TFileTime(*this) > TFileTime(t);
}

//
inline bool TSystemTime::operator >= ( const TSystemTime  & t ) const{
  return !(*this < t);
}

//
inline bool TSystemTime::operator == ( const TSystemTime  & t ) const{
  return TFileTime(*this) == TFileTime(t);
}

//
inline bool TSystemTime::operator != ( const TSystemTime  & t ) const{
  return !(*this == t);
}

//
// Loads & locks the specified resource..
//
//template <class T, const _TCHAR _FAR* resType>
template <class T, int resType>
TResource<T, resType>::TResource(HINSTANCE hModule, TResId resId)
:
  MemHandle(0),
  MemPtr(0)
{
  HRSRC resHandle = FindResource(hModule, resId, (_TCHAR _FAR*)resType);
  if (resHandle) {
    MemHandle = LoadResource(hModule, resHandle);
    if (MemHandle)
      MemPtr = (T*)LockResource(MemHandle);
  }
}

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif




//
// Loads & locks a resource of the type 'resType' from the module
// (hModule). Accepts a LanguageID for localized resources.
//
//template <class T, const _TCHAR _FAR* resType>
template <class T, int resType>
TResource<T, resType>::TResource(HINSTANCE hModule, TResId resId, LANGID lcid)
:
  MemHandle(0),
  MemPtr(0)
{
  HRSRC resHandle = FindResourceEx(hModule, (_TCHAR _FAR*)resType, resId, lcid);
  if (resHandle) {
    MemHandle = LoadResource(hModule, resHandle);
    if (MemHandle)
      MemPtr = (T*)LockResource(MemHandle);
  }
}

//
// Unlocks and frees the resource loaded earlier.
// NOTE: Unlocking and freeing of resources is not necessary in WIN32.
//
//template <class T, const _TCHAR _FAR* resType>
template <class T, int resType>
TResource<T, resType>::~TResource()
{
  // NOTE: UnlockResource and FreeResource are not necessary (albeit harmless)
  // in 32-bit
  //
  MemHandle = 0;
  MemPtr = 0;
}

//
// Returns true if resource was successfully loaded or false otherwise.
//
//template <class T, const _TCHAR _FAR* resType> bool
template <class T, int resType> bool
TResource<T, resType>::IsOK() const
{
  return ((MemHandle != 0) && (MemPtr != 0));
}

//
// Returns a pointer to the locked resource.
// NOTE: This operator eliminates the need to explicitly cast
//       LPVOID to a structure representing the actual layout
//       of individual resource types.
//
//template <class T, const _TCHAR _FAR* resType>
template <class T, int resType>
TResource<T, resType>::operator T* ()
{
  PRECONDITION(IsOK());
  return MemPtr;
}

#endif  // BI_PLAT_MSW


__OWL_END_NAMESPACE



#endif  // OWL_WINCLASS_H
