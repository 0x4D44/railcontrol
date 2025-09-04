//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Implementation of TGdiObject, abstract class for GDI objects that can be
// selected into TDCs
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif

#include <owl/template.h>

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlGDI, 1, 0);
// General GDI diagnostic group
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlGDIOrphan, 1, 0);
// Orphan control tracing group


//
// TGdiObject's internal orphan control object, container and member functions
//

struct TObjInfo {
	HANDLE            Handle;
	TGdiObject::TType Type       : 8;
	int               RefCount  : 8;

	TObjInfo() : Handle(0), Type(TGdiObject::None), RefCount(0) 
	{}
	TObjInfo(HANDLE handle) : Handle(handle) 
	{}
	TObjInfo(HANDLE handle, TGdiObject::TType type, int ref)
		: Handle(handle), Type(type), RefCount(ref) 
	{}
	~TObjInfo()
	{}
	bool operator ==(const TObjInfo& other) const {
		return other.Handle == Handle;
	}
	bool operator <(const TObjInfo& other) const {
		return other.Handle < Handle;
	}
};

typedef TSortedObjectArray<TObjInfo> TObjInfoBag;

static TObjInfoBag*    __ObjInfoBag      = 0;
#if defined(BI_MULTI_THREAD_RTL)
static TMRSWSection*  __ObjInfoBagLock  = 0;
#endif

#if defined(BI_MULTI_THREAD_RTL) //ObjInfoBag().Lock
#define LOCKOBJECTBAG(s) TMRSWSection::TLock __lock(*__ObjInfoBagLock,s)
#else
#define LOCKOBJECTBAG(s)
#endif

//static TObjInfoBag* ObjInfoBag;

#if defined(__TRACE)
static const LPTSTR ObjTypeStr[] = {
	_T("?"), _T("Pen"), _T("Brush"), _T("Font"), _T("Palette"),
	_T("Bitmap"), _T("TextBrush"), 0
};
#endif

//
static void createObjInfoBag()
{
	static LONG createObjInfo = -1;
	if(__ObjInfoBag)
		return;

	if(::InterlockedIncrement(&createObjInfo)==0){
		::InterlockedIncrement(&createObjInfo);
		__ObjInfoBag      = new TObjInfoBag;
#if defined(BI_MULTI_THREAD_RTL)
		__ObjInfoBagLock  = new TMRSWSection;
#endif
		createObjInfo = -1;
	}
}
//
static void destroyObjInfoBag()
{
	static LONG destroyObjInfo = -1;
	if(__ObjInfoBag==0 || __ObjInfoBag->Size()>0)
		return;

	if(::InterlockedIncrement(&destroyObjInfo)==0){
		::InterlockedIncrement(&destroyObjInfo);
		delete __ObjInfoBag;
#if defined(BI_MULTI_THREAD_RTL)
		delete __ObjInfoBagLock;
#endif
		__ObjInfoBag  =  0;
		destroyObjInfo = -1;
	}
}

//
// Find a reference to a given handle in the ObjInfoBag
//
TObjInfo*
TGdiObject::RefFind(HANDLE handle)
{
	if(handle && __ObjInfoBag){
		LOCKOBJECTBAG(true);
		int pos = __ObjInfoBag->Find(TObjInfo(handle));
		if(pos != (int)NPOS)
			return &((*__ObjInfoBag)[pos]);
	}
	return 0;
}

//
// Add an object reference entry into table, starting ref count at one
//
void
TGdiObject::RefAdd(HANDLE handle, TGdiObject::TType type)
{
	createObjInfoBag();
#if !defined(NO_GDI_SHARE_HANDLES)
	if (handle) {
		TObjInfo* member = RefFind(handle);
		if (member)
			member->RefCount++;
		else {
			LOCKOBJECTBAG(false);
			__ObjInfoBag->Add(TObjInfo(handle, type, 1));
		}
	}
#else
	if (handle && !RefFind(handle)){
		LOCKOBJECTBAG(false);
		__ObjInfoBag->Add(TObjInfo(handle, type, 1));
	}
#endif
}

//
// Remove an object reference entry from table
//
void
TGdiObject::RefRemove(HANDLE handle)
{
	if(!__ObjInfoBag)
		return;
	{
		LOCKOBJECTBAG(false);
		__ObjInfoBag->DestroyItem(TObjInfo(handle));
	}
	destroyObjInfoBag();
}

//
// Increment an object reference entry's ref count
//
void
TGdiObject::RefInc(HANDLE handle)
{
	createObjInfoBag();
	TObjInfo* member = RefFind(handle);
	if (member)
	{
		member->RefCount++;
	}
}


//
// Decrement an object reference entry's ref count.  Delete object if
// refcount goes to zero.  Warn if deletion was/wasn't supposed to
// happen and it didn't/did. Detach info if object was deleted.
//
void
#if defined(__TRACE)
TGdiObject::RefDec(HANDLE handle, bool wantDelete)
#else
TGdiObject::RefDec(HANDLE handle, bool)
#endif
{
	if(!__ObjInfoBag)
		return;
	TObjInfo* member = RefFind(handle);
	if (member) {
		bool needDelete = --(member->RefCount) == 0;
#if defined(__TRACE)
		if (needDelete != wantDelete) {
			if (needDelete)
				TRACEX(OwlGDIOrphan, 1, "Orphan" << ObjTypeStr[member->Type] <<
				(uint)member->Handle << "Deleted");

			else
				TRACEX(OwlGDIOrphan, 1, ObjTypeStr[member->Type] <<
				(uint)member->Handle << "Orphan");

		}
#endif
		if (needDelete) {

			if (!::DeleteObject(member->Handle))
			{
				TXGdi::Raise(IDS_GDIDELETEFAIL, member->Handle);
			}


			{
				LOCKOBJECTBAG(false);
				__ObjInfoBag->DestroyItem(*member);
			}

			destroyObjInfoBag();
		}
	}
}

//
// Return the reference count of a handle, -1 if not found
//
int
TGdiObject::RefCount(HANDLE handle)
{
	TObjInfo* member = RefFind(handle);
	if (member)
		return member->RefCount;
	return -1;
}



//----------------------------------------------------------------------------

//
//
//
#if defined(__TRACE) || defined(__WARN)
ostream& operator <<(ostream& os, const TGdiObject& gdiObject)
{
	os << "@" << (void*)&gdiObject << " ";
	os << '(';
# if !defined(BI_NO_RTTI)
	os << typeid(gdiObject).name() << ',';
# endif
	os << ')';
	return os;
}
#endif

//
// TGdiObject constructors
//
TGdiObject::TGdiObject()
{
	// Handle must be set by derived class
	TRACEX(OwlGDI, 2, "TGdiObject constructed " << *this);
}

//
// Create a wrapper for a given GDI object.
//
TGdiObject::TGdiObject(HANDLE handle, TAutoDelete autoDelete)
:
TGdiBase(handle, autoDelete)
{
	// CheckValid();  // cant do this here, as derived class may set handle later
	// Derived class must call RefAdd(Handle, type) if ShouldDelete
	TRACEX(OwlGDI, 2, "TGdiObject constructed " << *this);
}

//
// Decrement the reference count on this object.
//
TGdiObject::~TGdiObject()
{
	if (ShouldDelete)
		RefDec(Handle, true);
	TRACEX(OwlGDI, 2, "TGdiObject destructed " << *this);
}

__OWL_END_NAMESPACE

/* ========================================================================== */

