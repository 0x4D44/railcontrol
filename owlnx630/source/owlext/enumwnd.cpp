//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// This file implements the TEnumWindows class.  It is encapsulates the
// EnumTaskWindows (16-bit) or EnumThreadWindows (32-bit) APIs.  It can
// also handle EnumChildWindows to visit every window for a task/thread.
//
// Original code by Don Griffin; used with permission.
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:39 $
//-------------------------------------------------------------------
#include <owlext\pch.h>
#pragma hdrstop

#include <owlext/EnumWnd.h>


__OWL_USING_NAMESPACE

OWLEXT_BEGIN_NAMESPACE


TEnumWindows::TEnumWindows ()
{
	mDoingChildWindows = false;
}


void TEnumWindows::EnumWindows (TEnumMethod em)
{
	LPARAM  lParam = reinterpret_cast<LPARAM> (this);

	mDoingChildWindows = (em == emTopLevelWindows);

	EnumThreadWindows (GetCurrentThreadId(), WndEnumProc, lParam);
}


bool TEnumWindows::WndEnumMethod (HWND hWnd)
{
	if (!OnEnumWindow (hWnd))
		return false;

	if (! mDoingChildWindows){
		mDoingChildWindows = true;
		::EnumChildWindows (hWnd, WndEnumProc, reinterpret_cast<LPARAM>(this));
		mDoingChildWindows = false;
	}

	return true;
}

BOOL CALLBACK 
TEnumWindows::WndEnumProc (HWND hWnd, LPARAM lParam)
{
	TEnumWindows  * object = reinterpret_cast <TEnumWindows *> (lParam);
	return object->WndEnumMethod (hWnd);
}


OWLEXT_END_NAMESPACE

