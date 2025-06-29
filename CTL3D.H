/* CTL3D.H
*  =======
*
*  This is the header file for the 3D library (CTL3D.DLL) supplied by MicroSoft.
*  You will also require CTL3D.DLL and CTL3D.LIB (the latter can be generated
*  using the Borland import librarian).
*
*  To load the custom controls use CTL3DREGISTER and CTL3DAUTOSUBCLASS to subclass
*  the dialogs. Then use CTL3DUNREGISTER to unload the DLL. Version 2.0 was held on
*  28/06/1994. 
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date	    : 28/06/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

/* Header file for CTL3D DLL */

#if !defined( _CTL3D_H_)
#define _CTL3D_H_

#ifdef __cplusplus
   extern "C" {                     // avoid name-mangling if used from C++
#endif // __cplusplus


/* Ctl3dSubclassDlg3d flags */
#define    ctl3d_Buttons      0x0001;
#define    ctl3d_Listboxes    0x0002;
#define    ctl3d_Edits        0x0004;
#define    ctl3d_Combos       0x0008;
#define    ctl3d_StaticTexts  0x0010;
#define    ctl3d_StaticFrames 0x0020;
#define    ctl3d_NoDlgWindow  0x00010000;

#define    ctl3d_All          0xffff;


#define    wm_DlgBorder       wm_User+3567;

/* WM_DLGBORDER PInteger(lParam)^ return codes */
#define    ctl3d_NoBorder    0;
#define    ctl3d_Border      1;

#define    wm_DlgSubclass    wm_User+3568;

/* WM_DLGSUBCLASS PInteger(lParam)^ return codes */
#define    ctl3d_NoSubclass   0;
#define    ctl3d_Subclass     1;


BOOL   _far _pascal  Ctl3dSubclassDlg (HWND HWindow, WORD Grbits);
BOOL   _far _pascal  Ctl3dSubclassDlgEx (HWND HWindow, long Grbits);
WORD   _far _pascal  Ctl3dGetVer (void);
BOOL   _far _pascal  Ctl3dEnabled (void);
HBRUSH _far _pascal  Ctl3dCtlColor (HDC DC, COLORREF Color);   // ARCHAIC, use Ctl3dCtlColorEx
HBRUSH _far _pascal  Ctl3dCtlColorEx (WORD Message, WORD wParam, long lParam);
BOOL   _far _pascal  Ctl3dColorChange (void);
BOOL   _far _pascal  Ctl3dSubclassCtl (HWND HWindow);
LONG   _far _pascal  Ctl3dDlgFramePaint (HWND HWindow, WORD Message, WORD wParam, long lParam);
BOOL   _far _pascal  Ctl3dAutoSubclass (HANDLE Instance);
BOOL   _far _pascal  Ctl3dRegister (HANDLE Instance);
BOOL   _far _pascal  Ctl3dUnregister (HANDLE Instance);


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif
