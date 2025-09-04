//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $ 
//
// Source file for implementation of TImageDescriptor.
//-------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/imgdescr.h>
#include <owl/owlcore.h>
#include <owl/filename.h>

//
OWL_DIAGINFO;

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

//
//
TImageDescriptor* TImageDescriptor::ImageDescriptorHead = 0;

//
// "Bitmap File|*.bmp;*.dib|bmp"
static bool getFileFilter(TImageDescriptor* tpl, LPTSTR buffer, int size)
{
	tpl->GetDescription(buffer,size);
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
// Internal routine used to match a portion of a filename (i.e. name or
// extension) to a wilcard pattern.
//
static bool
nameMatch(LPCTSTR name, LPCTSTR wc)
{
	while (true) {
		if (*wc == _T('*') || (!*wc && !*name))
			return true;
#if defined(BI_DBCS_SUPPORT)
		int n = CharSize(name);
		if (*wc == '?' && *name) {
			wc++;
			name += n;
			continue;
		}
		else if (n > 1) { // if double byte character
			if (strncmp(name, wc, n) == 0) {
				wc += n;
				name += n;
				continue;
			}
		}
		else if (toupper(*name) == toupper(*wc)) {
#else
		if ((*wc == _T('?') && *name) || (_toupper(*name) == _toupper(*wc))) {
#endif
			wc++;
			name++;
			continue;
		}
		break;
	}
	return false;
}

//
// Helper method used by 'MatchTemplate' to determine whether a filename
// fits the pattern [wilcard] described by a filter.
//
bool TImageDescriptor::IsAMatch(LPCTSTR path, LPCTSTR fltr)
{
	PRECONDITION(path);
	PRECONDITION(fltr);

	// Ensure path has at least a valid file name
	//
	TFileName fname(path);
	if (!fname.HasParts(TFileName::File)) {
		TRACE(_T("TImageDescriptor::IsAMatch: Invoked with invalid path"));
		return false;
	}

	// Retrieve name/extension from path
	//
	owl_string name = fname.GetParts(TFileName::File);
	owl_string ext  = fname.GetParts(TFileName::Ext);

	// Get pointers to path's name and extension
	//
	LPCTSTR ppn = name.c_str();
	LPCTSTR ppe = fname.HasParts(TFileName::Ext) ? ext.c_str() : _T("");
	if (*ppe == _T('.'))
		ppe++;

	// Get pointers to filter's name and extension
	//
	TAPointer<_TCHAR> filter = nstrnewdup(fltr);
	_TCHAR* pfn = filter;
	_TCHAR* pfe = _tcsrchr(filter, _T('.'));

	// Skip past '.' separator of filter
	//
	if (pfe)
		*pfe++ = 0;
	else
		pfe = _T("");

	// Match the name and extension
	//
	return nameMatch(ppn, pfn) && nameMatch(ppe, pfe);
}

//
//
//
TImageDescriptor* TImageDescriptor::MatchTemplate(LPCTSTR path, bool write)
{

	// Iterate through templates
	//
	TTmpBuffer<_TCHAR> fltrCopy(MAX_PATH);

	for (TImageDescriptor* tpl = ImageDescriptorHead; tpl != 0; tpl = tpl->Next) {
		uint clr,addit;
		if(!tpl->GetCapabilities(clr,addit) || (write && (addit&cabCanWrite)==0))
			continue;

		LPTSTR fltr = fltrCopy;
		LPTSTR nxtFltr;

		// Make locale copy of filter
		// and check that filter exist
		if(!getFileFilter(tpl, fltr,MAX_PATH) || !_tcslen(fltr))
			continue;

		// For each template, try each wilcard specified
		//
		while (fltr) {

			// Is there another wildcard following - Null terminate at ';'
			//
			nxtFltr = _tcschr(fltr, _T(';'));
			if (nxtFltr)
				*nxtFltr++ = 0;

			if (IsAMatch(path, fltr))
				return tpl;

			// Try next filter
			//
			fltr = (nxtFltr && *nxtFltr) ? nxtFltr : 0;
		}
	}
	return 0;
}

//
// find ImageDescritor and Create Instance of TDib
TDib* TImageDescriptor::CreateImage(LPCTSTR path)
{
	TImageDescriptor* item = MatchTemplate(path);
	if(item)
		return item->OpenInstance(path);
	return 0;
}

//		
// registrer ImageDescriptor instance
void TImageDescriptor::Register(TImageDescriptor* item)
{
	TImageDescriptor* last = ImageDescriptorHead;

	if(!last){
		ImageDescriptorHead = item;
		return;
	}

	while(last->Next)
		last = last->Next;

	last->Next = item;
}

// unregistrer ImageDescriptor instance
void TImageDescriptor::Unregister(TImageDescriptor* item)
{

	if(ImageDescriptorHead == item){
		ImageDescriptorHead = item->Next;
		return;
	}

	TImageDescriptor* last = ImageDescriptorHead;

	while(last->Next){
		if(last->Next == item){
			last->Next = item->Next;
			break;
		}
		last = last->Next;
	}
}

//
// Walk on all ImageDescriptors
// if item = 0 return ImageDescriptorHead
TImageDescriptor* TImageDescriptor::GetNext(TImageDescriptor* item)
{
	if(!item)
		return ImageDescriptorHead;
	return item->Next;
}

_OWLFX_END_NAMESPACE

/*============================================================================*/
