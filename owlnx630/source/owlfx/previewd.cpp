//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop


#include <windows.h>
#include <dlgs.h>  		// cmb1 for file open dialog box
#include <commctrl.h> // for InitCommonControls()

#include <owl/listbox.h>
#include <owl/filename.h>
#include <owl/uihelper.h>
#include <owl/static.h>

#include <owlfx/previewd.h>
#include <owlfx/imgdescr.h>

#include <owlfx/owlfx.rh>

// (Readable versions of) IDs of controls used by CommonDialogs
#define IDC_FILENAMETXT       stc3
#define IDC_FILENAME          edt1
#define IDC_FILELIST          lst1
#define IDC_CURDIR            stc1
#define IDC_DIRLIST           lst2
#define IDC_FILETYPEEXT       stc2
#define IDC_FILETYPES         cmb1
#define IDC_DRIVESTEXT        stc4
#define IDC_DRIVES            cmb2
#define IDC_HELPF             pshHelp
#define IDC_READONLY          chx1

#if 0
#undef DENABLE
#undef DOUT
#undef DOUTNAME
#define DENABLE(_state_) bEnable = _state_;
#define DOUT(_lv_,_str_) if(bEnable && iLevel >= (_lv_)) _dbgstr_ << _str_ << endl
#define DOUTNAME(_Level_, _name_) static bool bEnable = true; static int iLevel = _Level_; static fstream _dbgstr_(_name_, ios::out)
DOUTNAME(4, "previewd.log");
#endif

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE


static const _TCHAR vfwStr[]						= _T("msvfw32.dll");
static const char DrawDibOpenStr[]				= "DrawDibOpen";
static const char DrawDibCloseStr[]				= "DrawDibClose";
static const char DrawDibRealizeStr[]			= "DrawDibRealize";
static const char DrawDibDrawStr[]				= "DrawDibDraw";

//-----------------------------------------------------------------------------
// class TVFW32
// ~~~~~ ~~~~~~~~
//
// delay loading VFW32.DLL
class TVfw32 {
public:
	static  TModule&	GetModule();

	//  Environmental
	static  HANDLE	DrawDibOpen();
	static  BOOL		DrawDibClose(HANDLE hdd);
	static  UINT		DrawDibRealize(HANDLE hdd, HDC hdc, BOOL fBackground);
	static  BOOL		DrawDibDraw(HANDLE hdd,HDC hdc,int xDst,int yDst,int dxDst,
		int dyDst, LPBITMAPINFOHEADER lpbi,
		LPVOID lpBits, int xSrc,int ySrc,int dxSrc, 
		int dySrc, UINT wFlags);
};
//-----------------------------------------------------------------------------
TModule&
TVfw32::GetModule()
{
	static TModule vfwModule(vfwStr, true, true, false);
	return vfwModule;
}
//-----------------------------------------------------------------------------
HANDLE TVfw32::DrawDibOpen()
{
	static TModuleProc0<HANDLE>
		drawDibOpen(GetModule(), DrawDibOpenStr);
	return drawDibOpen();
}
//-----------------------------------------------------------------------------
BOOL TVfw32::DrawDibClose(HANDLE hdd)
{
	static TModuleProc1<BOOL, HANDLE>
		drawDibClose(GetModule(), DrawDibCloseStr);
	return drawDibClose(hdd);
}
//-----------------------------------------------------------------------------
UINT TVfw32::DrawDibRealize(HANDLE hdd, HDC hdc, BOOL fBackground)
{
	static TModuleProc3<UINT,HANDLE,HDC,BOOL>
		drawDibRealize(GetModule(), DrawDibRealizeStr);
	return drawDibRealize(hdd, hdc, fBackground);
}
//-----------------------------------------------------------------------------
BOOL TVfw32::DrawDibDraw(HANDLE p1,HDC p2,int p3,int p4,int p5,int p6, 
						 LPBITMAPINFOHEADER p7,LPVOID p8,int p9,int p10,
						 int p11, int p12, UINT p13)
{
	static TModuleProc13<BOOL,HANDLE,HDC,int,int,int,int,LPBITMAPINFOHEADER,
		LPVOID,int,int,int, int,UINT>
		drawDibDraw(GetModule(), DrawDibDrawStr);
	return drawDibDraw(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13);
};


//
DEFINE_RESPONSE_TABLE1(TPictureControl, TControl)
EV_WM_ERASEBKGND,
EV_WM_PAINT,
EV_WM_QUERYNEWPALETTE,
END_RESPONSE_TABLE;
/* -------------------------------------------------------------------------- */
//@mfunc Constructor - Creates a picture object
// TPictureControl(TWindow* parent, int Id, TDib* dib, TModule*  module) -
// create from scratch based on Dib
//
// @parm TWindow* | parent | parent window
// @parm TModule* | module | module from which load resource
// @parm TDib* | dib | previously created dib to use
/* -------------------------------------------------------------------------- */
//@mfunc Cosntructor
TPictureControl::TPictureControl(TWindow* parent, int Id, 
								 TDib* dib, TModule*  module)
								 :
TControl(parent,Id,_T(""),0,0,0,0,module),
Dib(dib),
hDDC(0)
{
	Attr.Style |= BS_OWNERDRAW;
	hDDC = TVfw32::DrawDibOpen();
}
/* -------------------------------------------------------------------------- */
TPictureControl::TPictureControl(TWindow* parent, int resId, TModule* module)
:
TControl(parent,resId,module),
Dib(0),
hDDC(0)
{
	hDDC = TVfw32::DrawDibOpen();
}
/* -------------------------------------------------------------------------- */
//@mfunc Destructor
TPictureControl::~TPictureControl()
{
	delete Dib;
	if(hDDC)
		TVfw32::DrawDibClose(hDDC);
}
/* -------------------------------------------------------------------------- */
//@mfunc bool  | TPictureControl | EvEraseBkgnd | nothing
bool TPictureControl::EvEraseBkgnd(HDC /*hDC*/)
{
	return true;
}
/* -------------------------------------------------------------------------- */
//@mfunc bool | TPictureControl | EvQueryNewPalette | query new palette
bool TPictureControl::EvQueryNewPalette ()
{
	if(!hDDC || !Dib)
		return true;

	BOOL f;
	TClientDC dc(*this);
	// Realize the palette
	if((f = TVfw32::DrawDibRealize(hDDC, dc, FALSE)) != 0)
		Invalidate(true);
	return f;
}
/* -------------------------------------------------------------------------- */
//@mfunc void  | TImagePreviewCntrl | Paint | paint method
void TPictureControl::Paint(TDC& dc, bool /*erase*/, TRect& /*rect*/)
{
	//DOUT(4,"TImagePreviewCntrl::Paint() draw START");
	DRAWITEMSTRUCT draw;
	draw.hwndItem = GetHandle();
	draw.hDC = dc.GetHDC();
	draw.rcItem = GetClientRect();
	draw.itemData = 0;
	ODADrawEntire(draw);
	//DOUT(4,"TImagePreviewCntrl::Paint() draw END");
}
/* -------------------------------------------------------------------------- */
//@mfunc ODADrawEntire(
void TPictureControl::ODADrawEntire(DRAWITEMSTRUCT  &drawInfo)
{
	// Get device contexts for the button.
	TDC dc(drawInfo.hDC);

	// Draw the border
	TUIBorder b(drawInfo.rcItem, TUIBorder::Recessed);
	b.Paint(dc);

	if(!Dib){
		//DOUT(4,"TImagePreviewCntrl::ODADrawEntire() return => Dib or Bitmap = 0");
		return;
	}

	int w,h,rw,rh,bw,bh;
	rw = drawInfo.rcItem.right - drawInfo.rcItem.left;
	rh = drawInfo.rcItem.bottom - drawInfo.rcItem.top;

	// Select the bitmap into the memory DC.
	bw = Dib->Width();
	bh = Dib->Height();
	bool WorW = rw > bw ? ((rw/bw) > (rh/bh)) : (( bw/rw) > (bh/rh));
	if(WorW){ // weight
		w = rw;
		h = MulDiv(rw, bh, bw);
	}
	else{
		w = MulDiv(rh, bw, bh);
		h = rh;
	}

	// Draw the button.
	if(hDDC)
		TVfw32::DrawDibDraw(hDDC, 
		drawInfo.hDC, 
		drawInfo.rcItem.left + 1, 
		drawInfo.rcItem.top + 1,
		w - 2, h - 2, 
		Dib->GetInfoHeader(), 
		Dib->GetBits(), 
		0, 0, -1, -1, 0);
	else
		dc.StretchDIBits(TRect(TPoint(drawInfo.rcItem.left + 1, drawInfo.rcItem.top + 1),
		TSize(w-2, h-2)), TRect(0, 0, bw, bh), *Dib);
}

///////////////////////////////////////////////
// class TImagePreviewWnd
// ~~~~~ ~~~~~~~~~~~~~~~~
//{{TLayoutWindow = TImagePreviewWnd}}
//@class TImagePreviewWnd | Image Preview window
//@base public | TLayoutWindow
class /*_OWLFXCLASS*/ TImagePreviewWnd : public TLayoutWindow {
public:
	TImagePreviewWnd(TWindow* parent, TModule* module = 0);

	LPCTSTR GetFileName(){ return FileName.c_str(); }
	bool SetImageFile(LPCTSTR path, TDib* dib);

private:
	TStatic* 					Text;
	TPictureControl*	Picture;
	owl_string				FileName;

public:
	virtual void SetupWindow();
}; //{{TImagePreviewWnd}}

///////////////////////////////////////////////
//
const int IDC_OPNFILENAME = 200;
const int IDC_OPNPIC			= 201;

//
TImagePreviewWnd::TImagePreviewWnd(TWindow* parent, TModule* module)
:
TLayoutWindow(parent, _T(""), module)
{
	//DOUT(4,"TImagePreviewWnd::TImagePreviewWnd() START");
	SetBkgndColor(NoErase); // no need to erase client area
	TLayoutMetrics lm;
	lm.X.Units = lm.Y.Units = lm.Width.Units = lm.Height.Units = lmPixels;

	// create caption
	Text = new TStatic(this,IDC_OPNFILENAME,_T(""),0,0,0,0,0,module);

	// create main preview window
	Picture = new TPictureControl(this,IDC_OPNPIC,0,module);

	int CaptionHeight = 14;

	//--- metrics for Text1 ---
	lm.Y.Below(Text);
	lm.Y.Set(lmBottom, lmAbove, lmParent, lmBottom, 0 );
	lm.Height.Absolute(CaptionHeight);
	lm.X.SameAs(lmParent, lmLeft);
	lm.Width.SameAs(lmParent, lmRight);
	SetChildLayoutMetrics(*Text, lm);

	// set metrix for picture
	lm.X.SameAs(lmParent, lmLeft);
	lm.Y.Set(lmTop, lmBelow, lmParent, lmTop, 0);
	lm.Width.SameAs(lmParent, lmRight);
	lm.Height.Above(Text);
	SetChildLayoutMetrics(*Picture, lm);

	//DOUT(4,"TImagePreviewWnd::TImagePreviewWnd() END");
}
/* -------------------------------------------------------------------------- */
void TImagePreviewWnd::SetupWindow()
{
	TLayoutWindow::SetupWindow();

	Text->SetWindowFont(GetParentO()->GetWindowFont(), true);

	TTmpBuffer<_TCHAR> name(MAX_PATH);
	LPCTSTR ptr = _tcsrchr((_TCHAR*)FileName.c_str(), _T('\\'));
	if(ptr)
		_tcscpy(name, ptr + 1);
	else
		_tcscat(name, FileName.c_str());
	Text->SetText(name);
}
/* -------------------------------------------------------------------------- */
bool TImagePreviewWnd::SetImageFile(LPCTSTR path, TDib* dib)
{
	// hide window
	if(GetHandle()){
		ShowWindow(SW_HIDE);
	}

	/////  create new dib
	FileName = path;

	// init filename
	delete Picture->SetDib(dib);

	// set position
	if(GetHandle()){
		TTmpBuffer<_TCHAR> name(MAX_PATH);
		LPCTSTR ptr = _tcsrchr(path, _T('\\'));
		if(!ptr)
			ptr = _tcsrchr(path, _T('/'));

		if(ptr)
			_tcscpy(name, ptr + 1);
		else
			_tcscat(name, path);

		Text->SetText(name);
		ShowWindow(SW_SHOWNA);
		Picture->Invalidate();
	}
	return true;
}

////////////////////////////////////
//
// class TImagePreviewBase
// ~~~~~ ~~~~~~~~~~~~~~~~~
//
TImagePreviewBase::TImagePreviewBase()
:
PreWnd(0),
IsTimer(false)
{
}
//@mfunc HandleNewFileName
//@parm TDialog& | dlg | parent dialog
//@parm char* | file | file name to open
void TImagePreviewBase::HandleNewFileName(TDialog& dlg, LPCTSTR file)
{
	if(!TFileName(file).Exists()){
		//DOUT(0,"MCommonBaseDlg::HandleNewFileName() file not exist.");
		return;
	}

	TWaitCursor wait;
	if(!PreWnd){
		TRect rect;
		HWND hWnd = dlg.GetDlgItem(IDC_OPENDLGPREVIEW);
		::GetWindowRect(hWnd,	&rect);

		TPoint ulPoint(rect.left, rect.top);
		dlg.ScreenToClient(ulPoint);
		TPoint lrPoint(rect.right, rect.bottom);
		dlg.ScreenToClient(lrPoint);

		PreWnd = new TImagePreviewWnd(&dlg);
		TDib* dib = CreateImage(file);
		if(dib){
			PreWnd->SetImageFile(file, dib);
			PreWnd->Create();
			PreWnd->MoveWindow(TRect(ulPoint.x, ulPoint.y, lrPoint.x, lrPoint.y), true);
		}
	}
	else{
		if(_tcsicmp(PreWnd->GetFileName(), file) == 0)
			return;
		TDib* dib = CreateImage(file);
		if(dib)
			PreWnd->SetImageFile(file, dib);
	}
}
//
TDib* TImagePreviewBase::CreateImage(LPCTSTR path)
{
	// have to select template and load from template
	TRY{
		return TImageDescriptor::CreateImage(path);
	}
	CATCH((...){
		return 0;
	})
}
//
//
// "Bitmap File|*.bmp;*.dib|bmp"
static bool getFileFilter(TImageDescriptor* tpl, LPTSTR buffer, int size)
{
	if(!tpl->GetDescription(buffer,size))
		return false;

	// start of description
	LPTSTR p = _tcschr(buffer,_T('|'));
	if(!p)
		return false;
	memmove(buffer,p+1,_tcslen(buffer)-(p-buffer));

	// end of description
	p = _tcschr(buffer,_T('|'));
	if(!p)
		return false;

	*p = 0;
	return true;
}
//
//
// "All Formats|*.bmp;*.dib;*.pcx;*.tga|"
void TImagePreviewBase::SetDialogFilter(TFileOpenDialog::TData& data, bool openDlg)
{
	owl_string strings;
	TTmpBuffer<_TCHAR> fltrCopy(MAX_PATH);

	if(openDlg){
		strings = gModule->LoadString(IDS_ALL_DESCRIPTOR);
		bool first = true;

		// get extensions for all Image Formats
		for(TImageDescriptor* tpl = TImageDescriptor::GetNext(); tpl; 
			tpl = TImageDescriptor::GetNext(tpl)){

				uint clr,addit;
				if(!tpl->GetCapabilities(clr,addit) || (!openDlg && (addit&TImageDescriptor::cabCanWrite)==0))
					continue;
				// Make locale copy of filter and check that filter exist
				if(!getFileFilter(tpl, fltrCopy, MAX_PATH) || !_tcslen(fltrCopy))
					continue;

				if(!first)
					strings += _T(';');
				first = false;

				strings += (_TCHAR*)fltrCopy;
		}
		strings += _T('|');
	}

	// get All Format Descriptions
	for(TImageDescriptor* tpl = TImageDescriptor::GetNext(); 
		tpl; 
		tpl = TImageDescriptor::GetNext(tpl)){

			uint clr,addit;
			if(!tpl->GetCapabilities(clr,addit) || (!openDlg && (addit&TImageDescriptor::cabCanWrite)==0))
				continue;

			if(tpl->GetDescription(fltrCopy, MAX_PATH)){
				LPTSTR p = _tcschr(fltrCopy,_T('|'));
				if(p)
					p = _tcschr(p+1,_T('|'));
				if(p)
					*p = _T('\0');
				strings += (_TCHAR*)fltrCopy;
				strings += _T('|');
			}
	}
	data.SetFilter(strings.c_str());
}
//
bool TImagePreviewBase::BaseDialogFunction(TDialog* dlg, uint message, TParam1 p1, TParam2 p2)
{
	if (message == WM_COMMAND){
		if(LOWORD(p1) == IDC_FILELIST && HIWORD(p1) == CBN_SELCHANGE){
			if(IsTimer)
				dlg->KillTimer(100);
			dlg->SetTimer(100, 500);
			IsTimer = true;
			return false;
		}
	}
	else if(message == WM_NOTIFY){
		LPOFNOTIFY lpNot = (LPOFNOTIFY)p2;
		// The selection has changed.
		if(lpNot->hdr.code == CDN_SELCHANGE){
			if(IsTimer)
				dlg->KillTimer(100);
			dlg->SetTimer(100, 500);
			IsTimer = true;
			return false;
		}
	}
	else if(message  == WM_TIMER){
		if(IsTimer)
			dlg->KillTimer(100);
		IsTimer = false;

		TTmpBuffer<_TCHAR> szFile(MAX_PATH);
		// Get the path of the selected file.
		if(CommDlg_OpenSave_GetFilePath(dlg->GetParentH(), (_TCHAR*)szFile, MAX_PATH) != 0){
			HandleNewFileName(*dlg, (_TCHAR*)szFile);
			return false;
		}
		else{
			// Get the file specification from the common dialog.
			TTmpBuffer<_TCHAR> name(MAX_PATH);
			int index = dlg->SendDlgItemMsg(IDC_FILELIST, LB_GETCURSEL, 0, 0);
			if(index != LB_ERR){
				dlg->SendDlgItemMessage(IDC_FILELIST, LB_GETTEXT, (WPARAM)index,(LPARAM)(_TCHAR*)name);
				::GetCurrentDirectory(MAX_PATH, (_TCHAR*)szFile);
				// Get the path of the selected file.
				_tcscat((_TCHAR*)szFile,_T("\\"));
				_tcscat((_TCHAR*)szFile,(_TCHAR*)name);
				HandleNewFileName(*dlg, (_TCHAR*)szFile);
			}
			return false;
		}
	}
	else if(message  == WM_DESTROY){
		if(IsTimer)
			dlg->KillTimer(100);
	}
	return true;
}

///////////////////////////////////////
//
//{{TFileOpenPreviewDlg Implementation}}
TFileOpenPreviewDlg::TFileOpenPreviewDlg(TWindow* parent, TData& data, 
										 TResId templateID, LPCTSTR title,  
										 TModule* module)
										 :
TFileOpenDialog(parent, data, 
				templateID ? templateID : 
				TResId(IDD_FILEOPENPREVIEWDLGNEW),
				title, module ? module : gModule)
{
}
///----------------------------------------------------------------------------
TFileOpenPreviewDlg::~TFileOpenPreviewDlg()
{
	Destroy(IDCANCEL);
}
/* -------------------------------------------------------------------------- */
void TFileOpenPreviewDlg::SetupWindow()
{
	TFileOpenDialog::SetupWindow();
	::ShowWindow(GetDlgItem(IDC_OPENDLGPREVIEW), SW_HIDE);
}
/* -------------------------------------------------------------------------- */
void TFileOpenPreviewDlg::CleanupWindow()
{
	if(IsTimer)
		KillTimer(100);
	IsTimer = false;
	TFileOpenDialog::CleanupWindow();
}
/* -------------------------------------------------------------------------- */
//@mfunc DialogFunction - Overriden virtual function of TFileOpenDialog to
// allow us to open selected file, extract from hem picture and display it.
//@parm uint | message | messsage
//@parm TParam1 | p1 | First parameter
//@parm TParam2 | p2 | Second parameter
//@rdesc true or false
bool TFileOpenPreviewDlg::DialogFunction(uint message, TParam1 p1, TParam2 p2)
{
	if(BaseDialogFunction(this, message, p1, p2))
		return TFileOpenDialog::DialogFunction(message, p1, p2);
	return false;
}
/* -------------------------------------------------------------------------- */
//{{TFileSavePreviewDlg Implementation}}
//@mfunc Constructor
//@parm TWindow* | parent | parent windows
//@parm TData& | data | dialog data
//@parm const char* | title | titles for dialog
TFileSavePreviewDlg::TFileSavePreviewDlg(TWindow* parent, TData& data, 
										 TResId templateID, LPCTSTR title,  
										 TModule* module)
										 :
TFileSaveDialog(parent, data, 
				templateID ? templateID : 
				TResId(IDD_FILEOPENPREVIEWDLGNEW), 
				title, module ? module : gModule)
{
}
//
//
//
TFileSavePreviewDlg::~TFileSavePreviewDlg()
{
	Destroy(IDCANCEL);
}

//
//
//
void TFileSavePreviewDlg::SetupWindow()
{
	TFileSaveDialog::SetupWindow();

	::ShowWindow(GetDlgItem(IDC_OPENDLGPREVIEW), SW_HIDE);
}

//
//
//
void TFileSavePreviewDlg::CleanupWindow()
{
	if(IsTimer)
		KillTimer(100);
	IsTimer = false;
	TFileSaveDialog::CleanupWindow();
}
/* -------------------------------------------------------------------------- */
//@mfunc DialogFunction - Overriden virtual function of TFileOpenDialog to
// allow us to open selected file, extract from hem picture and display it.
//@parm uint | message | messsage
//@parm TParam1 | p1 | First parameter
//@parm TParam2 | p2 | Second parameter
//@rdesc true or false
bool TFileSavePreviewDlg::DialogFunction(uint message, TParam1 p1, TParam2 p2)
{
	if(BaseDialogFunction(this, message, p1, p2))
		return TFileSaveDialog::DialogFunction(message, p1, p2);
	return false;
}

_OWLFX_END_NAMESPACE

/* ========================================================================== */

