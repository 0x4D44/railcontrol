//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Implementation for TBrush GDI Brush object. Includes cache for solid brushes
//----------------------------------------------------------------------------
#if defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);        // General GDI diagnostic group

#define CACHE_SIZE 16 

//////////////////////////////////////////////////////////////////////////////////////////////////
// multithread support
//
// Internal MRU brush cache to keep most recent solid brushes around cached by
// color
//
class TBrushCache 
#if defined(BI_MULTI_THREAD_RTL)
	: public TLocalObject
#endif
{
public:
	struct TEntry {
		HANDLE    Handle;
		COLORREF  Color;
		int       RefCnt;
	};

	TBrushCache();
	~TBrushCache();

	HANDLE Lookup(COLORREF color);
	bool   CheckHandle(HANDLE handle);
	void   Float(int i);
	bool   AddRef(HANDLE handle, COLORREF color);
	void   DecRef(HANDLE handle);

#if defined(BI_MULTI_THREAD_RTL)
	TMRSWSection  Lock;
#endif

private:
	TEntry Entries[CACHE_SIZE];
	int Size;
};

#if !defined(SECTION)
static 
#endif
TBrushCache& GetBrushCache();

#if !defined(SECTION) || SECTION == 1

//
// Static instance of the brush cache used by TBrush
//
#if !defined(SECTION)
static 
#endif
TBrushCache& GetBrushCache() 
{
	//#if defined(BI_MULTI_THREAD_RTL)
	//  static TProcessContainer<TBrushCache> __BrushCache;
	//#else
	static TBrushCache __BrushCache;
	//#endif
	return __BrushCache;
};

#if defined(BI_MULTI_THREAD_RTL)
#define LOCKCACHE(l,s) TMRSWSection::TLock __lock(l,s)
#else
#define LOCKCACHE(l,s)
#endif

//
//
//
TBrushCache::TBrushCache()
:
Size(CACHE_SIZE)
{
	memset(Entries,0,sizeof(Entries));
}


//
//
//
TBrushCache::~TBrushCache()
{
	for (int i = 0; i < Size; i++){
		if (Entries[i].Handle) {
			TGdiObject::RefDec(Entries[i].Handle, false);

			LOCKCACHE(Lock,false);
			Entries[i].Handle = 0;
			Entries[i].Color  = 0;
			Entries[i].RefCnt = 0;
		}
	}
}

//
// Move an entry to the front, shifting in between ones down
//
void
TBrushCache::Float(int i)
{
	if(i > 0){
		HANDLE    handle  = Entries[i].Handle;
		COLORREF  color   = Entries[i].Color;
		int       refCnt  = Entries[i].RefCnt;

		TRACEX(OwlGDI, 2, _T("TBrushCache::Float(") << i << _T(") (") <<
			hex << uint(handle) << _T(", ") << hex << color << _T(")"));
		{
			LOCKCACHE(Lock,false);
			memmove(Entries+1, Entries, sizeof(TEntry)*i);
			Entries[0].Handle = handle;
			Entries[0].Color  = color;
			Entries[0].RefCnt = refCnt;
		}
	}
}

//
// Add a new entry to the front, pushing the last old one off the end.
//
bool
TBrushCache::AddRef(HANDLE handle, COLORREF color)
{

	TRACEX(OwlGDI, 2, _T("TBrushCache::Add(") << hex << uint(handle) <<
		_T(", ") << hex << color << _T(")"));
	CHECK(Size > 1);

	bool retval = true;
	if (Entries[Size-1].Handle){
		int i = Size-1;
		for(; i >= 0; i--){
			// if RefCnt == 0 no one use this
			if(!Entries[i].RefCnt){
				TGdiObject::RefDec(Entries[i].Handle, true);
				LOCKCACHE(Lock,false);
				Entries[i].Handle = handle;
				Entries[i].Color  = color;
				Entries[i].RefCnt = 1; 
				Float(i);// move up
				break;
			}
		}
		if(i < 0) // free node not found
			retval = false;
	}
	else{
		LOCKCACHE(Lock,false);
		memmove(Entries+1, Entries, sizeof(TEntry)*(Size-1));
		Entries[0].Handle = handle;
		Entries[0].Color = color;
		Entries[0].RefCnt = 1; 
	}

	TGdiObject::RefAdd(handle, TGdiObject::Brush);

	return retval;
}

//
//
//
void   
TBrushCache::DecRef(HANDLE handle)
{
	for (int i = 0; i < Size && Entries[i].Handle; i++){
		if (handle == Entries[i].Handle) {
			TRACEX(OwlGDI, 2, _T("TBrushCache::DecRef(") << hex << handle <<
				_T("): found ") << _T(" @") << i);
			Entries[i].RefCnt--;
			CHECK(Entries[i].RefCnt >= 0);
			break;
		}
	}
}

//
//
//
bool   
TBrushCache::CheckHandle(HANDLE handle)
{
	for (int i = 0; i < Size && Entries[i].Handle; i++){
		if (handle == Entries[i].Handle) {
			TRACEX(OwlGDI, 2, _T("TBrushCache::CheckHandle(") << 
				hex << uint(Entries[i].Handle) << _T(" @") << i);
			Entries[i].RefCnt++;
			return true;  
		}
	}
	TRACEX(OwlGDI, 2, _T("TBrushCache::CheckHandle(") << hex << uint(handle) << _T("): not found"));
	return 0;
}

//
// Lookup a color in the cache, returning the handle if found
//
HANDLE
TBrushCache::Lookup(COLORREF color)
{
	for (int i = 0; i < Size && Entries[i].Handle; i++){
		if (color == Entries[i].Color) {
			TRACEX(OwlGDI, 2, _T("TBrushCache::Lookup(") << hex << color <<
				_T("): found ") << hex << uint(Entries[i].Handle) << _T(" @") << i);
			Entries[i].RefCnt++;
			Float(i);
			return Entries[0].Handle;  // After Float() our entry is at 0!
		}
	}
	TRACEX(OwlGDI, 2, _T("TBrushCache::Lookup(") << hex << color << _T("): not found"));
	return 0;
}
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2

//
// Constructors
//

//
// Alias an existing brush handle. Assume ownership if autoDelete says so
//
TBrush::TBrush(HBRUSH handle, TAutoDelete autoDelete)
:
TGdiObject(handle, autoDelete)
{
	if (ShouldDelete)
		RefAdd(Handle, Brush);
	else
		GetBrushCache().CheckHandle(Handle);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" with handle ") << uint(handle));
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 3

//
// Construct a brush from a solid color. Gets handle from the cache if found,
// otherwise create the brush and add it to the cache
// If useCache == false not add brush to cache.
TBrush::TBrush(const TColor& color, bool useCache)
{
	ShouldDelete = !useCache;

	COLORREF cr = color;
	if(useCache && (Handle = GetBrushCache().Lookup(cr)) != 0)
		return;

	//Use new win4.0 function if available? Probably very fast.
	if (color.IsSysColor())
	{
		Handle = ::GetSysColorBrush(color.Index());
	}
	else 
	{
		Handle = ::CreateSolidBrush(cr);
	}
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create solid TBrush ") << hex << color);
	CheckValid();

	if (!color.IsSysColor())
	{	
		if (useCache)
		{
			if(!GetBrushCache().AddRef(Handle, color))
			{
				TRACEX(OwlGDI, OWL_CDLEVEL, _T("Cannot add new TBrush to cache ") << hex << color);
				ShouldDelete = true;
			}
		}
		else
		{
			RefAdd(Handle, Brush);
		}
	}
	else
	{
		ShouldDelete = false;
	}


	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" with color ") << (COLORREF)color);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 4

//
// Construct a hatch brush of a given color and hatch style
//
TBrush::TBrush(const TColor& color, int style)
{
	Handle = ::CreateHatchBrush(style, color);
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create hatch TBrush ") << hex << color <<
		_T(" ") << style);
	CheckValid();
	RefAdd(Handle, Brush);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" with color ") << (COLORREF)color <<
		_T(", style 0x") << style);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 5

//
// Construct a pattern brush given a TBitmap
//
TBrush::TBrush(const TBitmap& pattern)
{
	Handle = ::CreatePatternBrush(pattern);
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create pattern TBrush from bitmap ") <<
		hex << (uint)(HBITMAP)pattern);
	CheckValid();
	RefAdd(Handle, Brush);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" from bitmap @") << (void*)&pattern);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 6

//
// Construct a brush from a logical brush definition
//
TBrush::TBrush(const LOGBRUSH * logBrush)
{
	PRECONDITION(logBrush);
	Handle = ::CreateBrushIndirect((LPLOGBRUSH)logBrush);  // API cast
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create TBrush from logBrush @") <<
		hex << uint32(LPVOID(logBrush)));
	CheckValid();
	RefAdd(Handle, Brush);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" from logBrush @") << (void*)logBrush);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 7

//
// Construct a copy of an existing brush. Contructed brush will share the
// handle unless NO_GDI_SHARE_HANDLES is defined, in which case a new handle is
// created
//
TBrush::TBrush(const TBrush& src)
{
#if !defined(NO_GDI_SHARE_HANDLES)
	Handle = src.Handle;
	RefAdd(Handle, Brush);
#else
	LOGBRUSH logBrush;
	src.GetObject(logBrush);
	Handle = ::CreateBrushIndirect(&logBrush);
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create TBrush from TBrush @") <<
		hex << uint32(LPVOID(&src)));
	CheckValid();
	RefAdd(Handle, Brush);
#endif
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("Copy constructed TBrush @") << (void*)this <<
		_T(" from TBrush @") << (void*)&src);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 8

//
// Destroy the C++ object.
//
TBrush::~TBrush()
{
	if(!ShouldDelete)
		GetBrushCache().DecRef(Handle);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush destructed @") << (void*)this);
}
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 9

//
// Construct a pattern brush given a TDib
//
TBrush::TBrush(const TDib& pattern)
{
	Handle = ::CreateDIBPatternBrush(pattern, pattern.Usage());
	WARNX(OwlGDI, !Handle, 0, _T("Cannot create pattern TBrush from DIB ") <<
		hex << (uint)(HANDLE)pattern);
	CheckValid();
	RefAdd(Handle, Brush);
	TRACEX(OwlGDI, OWL_CDLEVEL, _T("TBrush constructed @") << (void*)this <<
		_T(" from DIB @") << (void*)&pattern);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 10

//
// Create a hatch brush.
//
THatch8x8Brush::THatch8x8Brush(const uint8 hatch[], const TColor& fgColor, const TColor& bgColor)
:
TBrush(Create(hatch, fgColor, bgColor), AutoDelete)
{
}

//
// Reconstructs the brush with a new pattern or colors
//
void
THatch8x8Brush::Reconstruct(const uint8 hatch[], const TColor& fgColor, const TColor& bgColor)
{
	if (Handle)
		RefDec(Handle, true);

	Handle = Create(hatch, fgColor, bgColor);
	RefAdd(Handle, Brush);
}

//
// Private static create member to create or re-create the handle
//
HBRUSH
THatch8x8Brush::Create(const uint8 hatch[], const TColor& fgColor, const TColor& bgColor)
{
	TDib dib(8, 8, 2, DIB_RGB_COLORS);
	uint8 * bits = (uint8 *)dib.GetBits();
	memset(bits, 0, 8*sizeof(uint32));
	for (int i = 0; i < 8; i++)
		bits[(7-i) * sizeof(uint32)] = hatch[i];  // Dib is upside down
	dib.SetColor(0, bgColor);
	dib.SetColor(1, fgColor);

#if 0  // Extra step using hbitmap required for some old Win32 versions
	TBitmap bm(dib);
	LOGBRUSH lb = { BS_PATTERN, DIB_RGB_COLORS, (int)HANDLE(bm) };
	HBRUSH handle = ::CreateBrushIndirect(&lb);
#else
	HBRUSH handle = ::CreateDIBPatternBrush(dib.GetHandle(), DIB_RGB_COLORS);
#endif

	_CheckValid(handle);
	return handle;
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 11

//
// Predefined 8x8 hatch patterns
//
const uint8 THatch8x8Brush::Hatch22F1[8] = {
	0x99, 0x33, 0x66, 0xCC, 0x99, 0x33, 0x66, 0xCC
};
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 12
const uint8 THatch8x8Brush::Hatch13F1[8] = {
	0x88, 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44
};
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 13
const uint8 THatch8x8Brush::Hatch11F1[8] = {
	0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 14
const uint8 THatch8x8Brush::Hatch22B1[8] = {
	0xCC, 0x66, 0x33, 0x99, 0xCC, 0x66, 0x33, 0x99
};
#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 15
const uint8 THatch8x8Brush::Hatch13B1[8] = {
	0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11, 0x88
};

#endif

__OWL_END_NAMESPACE

