#pragma once

#include <windows.h>

// Minimal 32-bit stubs for the legacy CTL3D API used by RailControl.
// The original Borland-era implementation relied on ctl3d.dll, which is
// obsolete on modern Windows versions. Returning success prevents the
// application from failing to load while effectively treating the 3D
// control helpers as no-ops.

#ifndef WM_DLGBORDER
#define WM_DLGBORDER    (WM_USER + 3567)
#endif
#ifndef WM_DLGSUBCLASS
#define WM_DLGSUBCLASS  (WM_USER + 3568)
#endif

inline BOOL Ctl3dSubclassDlg(HWND, WORD) { return TRUE; }
inline BOOL Ctl3dSubclassDlgEx(HWND, LONG_PTR) { return TRUE; }
inline WORD Ctl3dGetVer() { return 0x0200; }
inline BOOL Ctl3dEnabled() { return TRUE; }
inline HBRUSH Ctl3dCtlColor(HDC, COLORREF) { return NULL; }
inline HBRUSH Ctl3dCtlColorEx(UINT, WPARAM, LPARAM) { return NULL; }
inline BOOL Ctl3dColorChange() { return FALSE; }
inline BOOL Ctl3dSubclassCtl(HWND) { return TRUE; }
inline LONG_PTR Ctl3dDlgFramePaint(HWND, UINT, WPARAM, LPARAM) { return 0; }
inline BOOL Ctl3dAutoSubclass(HINSTANCE) { return TRUE; }
inline BOOL Ctl3dRegister(HINSTANCE) { return TRUE; }
inline BOOL Ctl3dUnregister(HINSTANCE) { return TRUE; }
