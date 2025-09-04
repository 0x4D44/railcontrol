//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TCursor, a GDI Cursor object class
//----------------------------------------------------------------------------
#include <owl/pch.h>

//DLN try to get rid of this hack on HPUX later
//FMM Ugly...I know... =)
#ifndef __HPUX__
#include <owl/module.h>
#else
#undef OWL_MODULE_H
#include <owl/module.h>
#define OWL_MODULE_H
#endif

#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGDI);        // General GDI diagnostic group

//
//
//
TCursor::TCursor(HCURSOR handle, TAutoDelete autoDelete)
:
  TGdiBase(handle, autoDelete)
{
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor constructed @" << (void*)this <<
    " from handle " << uint(handle));
}

//
//
//
TCursor::TCursor(HINSTANCE, const TCursor& cursor)
{
  Handle = (HCURSOR)::CopyIcon((HICON)(HCURSOR)cursor);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor @" << (void*)this <<
    " copied from TCursor " << (void*)&cursor);
}

//
//
//
TCursor::TCursor(HINSTANCE instance, TResId resId)
{
  PRECONDITION(resId);
  Handle = ::LoadCursor(instance, resId);
  if(!Handle && instance != gModule->GetHandle()){ // default load from OWL DLL
// if(!Handle && instance != (*gModule)){ // default load from OWL DLL 
    Handle = gModule->LoadCursor(resId);
    instance = gModule->GetHandle();
  }
  ShouldDelete = (instance != 0);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor @" << (void*)this <<
    " loaded from resource " << resId);
}

//
//
//
TCursor::TCursor(HINSTANCE instance, const TPoint& hotSpot, const TSize& size,
                 void * andBits, void * xorBits)
{
  PRECONDITION(andBits && xorBits);
  Handle = ::CreateCursor(instance, hotSpot.x, hotSpot.y, size.cx, size.cy,
                          andBits, xorBits);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor @" << (void*)this << " created from bits ");
}

//
//
//
TCursor::TCursor(const void* resBits, uint32 resSize)
{
  PRECONDITION(resBits && resSize);
  Handle = ::CreateIconFromResource((PBYTE)resBits, resSize, false, 0x00030000);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor @" << (void*)this <<
    " created from bits (32)");
}

//
//
//
TCursor::TCursor(const ICONINFO* iconInfo)
{
  PRECONDITION(iconInfo);
  //iconInfo->fIcon = false;
  Handle = ::CreateIconIndirect((PICONINFO)iconInfo);
  CheckValid();
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor constructed indirectly @" << (void*)this);
}

//
//
//
TCursor::~TCursor()
{
  if (ShouldDelete && Handle)
    ::DestroyCursor((HCURSOR)Handle);
  TRACEX(OwlGDI, OWL_CDLEVEL, "TCursor destructed @" << (void*)this);
}

__OWL_END_NAMESPACE
/* ========================================================================== */
