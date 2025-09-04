//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//
// OWL FX Entry point.
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/bmp.h>
#include <owlfx/pcx.h>
#include <owlfx/tga.h>

//
OWL_DIAGINFO;

//DIAG_DECLARE_GROUP(OwlMain);

_OWLFX_USING_NAMESPACE

__OWL_USING_NAMESPACE

//
static void RegisterBmpDescriptor(bool reg)
{
	static TBmpDescriptor BmpDescriptor;

	if(reg)
		TImageDescriptor::Register(&BmpDescriptor);
	else
		TImageDescriptor::Unregister(&BmpDescriptor);
}
//
static void RegisterPcxDescriptor(bool reg)
{
	static TPcxDescriptor PcxDescriptor;

	if(reg)
		TImageDescriptor::Register(&PcxDescriptor);
	else
		TImageDescriptor::Unregister(&PcxDescriptor);
}
//
static void RegisterTgaDescriptor(bool reg)
{
	static TTgaDescriptor TgaDescriptor;

	if(reg)
		TImageDescriptor::Register(&TgaDescriptor);
	else
		TImageDescriptor::Unregister(&TgaDescriptor);
}
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Image Descriptors existent in this file
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////
#if defined(BI_COMP_BORLANDC)
extern "C" _OWLFXFUNC(void)
#else
STDAPI_(void)
#endif
RegisterImageDescriptors()
{
	RegisterBmpDescriptor(true);
	RegisterPcxDescriptor(true);
	RegisterTgaDescriptor(true);
}
//
#if defined(BI_COMP_BORLANDC)
extern "C" _OWLFXFUNC(void)
#else
STDAPI_(void)
#endif
UnregisterImageDescriptors()
{
	RegisterBmpDescriptor(false);
	RegisterPcxDescriptor(false);
	RegisterTgaDescriptor(false);
}
// Singleton accessor method; does the same as TOWLEXTDll::Instance(), but
// some may find the global function syntax to be cleaner
//
#if defined(BI_COMP_BORLANDC)
extern "C" _OWLFXFUNC(TModule* WINAPI)
#else
STDAPI_(TModule*)
#endif
GetOwlFxModule()
{
	return gModule;
}
////////////////////////////////////////////////////////////////
//
//
#if defined (_BUILD_OWLFXDLL)
#if defined(BI_COMP_MSC)
int WINAPI DllMain(HINSTANCE hInstance, uint32 reason, LPVOID)
#else
int WINAPI DllEntryPoint(HINSTANCE hInstance, uint32 reason, LPVOID)
#endif
{
	TRACE("DllEntryPoint(" << hInstance << ", " << reason << ", " << ") called");

	switch (reason) {
	case DLL_PROCESS_ATTACH:
		// If you provide your own version of DLL EntryPoint, make sure
		// you call OwlInitUserDLL to allow OWL to initialize '::Module'
		//
		return OWLInitUserDLL(hInstance, _T("")) ? 1 : 0;
	}
	return 1;
}
#endif
/*============================================================================*/
