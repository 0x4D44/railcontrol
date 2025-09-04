//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
//
//Original code by David Foringer
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:54 $
//
//-------------------------------------------------------------------
#include <owlext\pch.h>
#pragma hdrstop

#include <owlext/validate.h>
#include <stdio.h>

__OWL_USING_NAMESPACE

OWLEXT_BEGIN_NAMESPACE

//ctor:
TDoubleValidator::TDoubleValidator(double min, double max)
:
TFilterValidator(_T("0-9+-."))
{
	if (min >= 0.)
		ValidChars -= _T('-');
	Min = min;
	Max = max;
}


//called on error:
void TDoubleValidator::Error(TWindow* owner)
{
	_TCHAR msg[255];
	TApplication* app = owner->GetApplication();
	_stprintf(msg, _T("Value not in range %.3f , %.3f"), Min, Max);
	owner->MessageBox(msg, app->GetName(), MB_ICONEXCLAMATION|MB_OK);
}


//called on focus lost, str is * to edit line string:
bool TDoubleValidator::IsValid(LPCTSTR str)
{
	//see IsValidInput for description !
	if (!TFilterValidator::IsValid(str))
		return false;
	LPCTSTR t = _tcschr(str,_T('-'));
	if(t!=NULL){
		if(t!=str)
			return false;
		if(_tcschr(t+1,_T('-')))
			return false;
	}
	if((t=_tcschr(str,_T('+')))!=NULL){
		if(t!=str)
			return false;
		if(_tcschr(t+1,_T('+')))
			return false;
	}
	if((t=_tcschr(str,_T('.')))!=NULL){
		if(_tcschr(t+1,_T('.')))
			return false;
	}
	double value = _ttof(str);
	if (value >= Min && value <= Max)
		return true;
	return false;
}

//called on each entry, str is * to edit line string:
bool
TDoubleValidator::IsValidInput(LPTSTR str, bool /*suppressFill*/)
{
	for (LPCTSTR p = str; *p; ) {
		uint n = CharSize(p) / sizeof(TCHAR);
		if (n > 1 || !ValidChars.Has((uchar)*p))
			return false;
		p += n;
	}
	LPCTSTR t;
	//find '-'
	if((t=_tcschr(str,_T('-')))!=NULL){
		//if found, must be first char:
		if(t!=str)
			return false;
		//if found, disallow additional occurences:
		if(_tcschr(t+1,_T('-')))
			return false;
	}
	//find '+'
	if((t=_tcschr(str,_T('+')))!=NULL){
		//if found, must be first char:
		if(t!=str)
			return false;
		//if found, disallow additional occurences:
		if(_tcschr(t+1,_T('+')))
			return false;
	}
	//even if set so in locale, don't allow comma for decimal point
	if((t=_tcschr(str,_T(',')))!=NULL)
		return false;

	//find decimal point
	if((t=_tcschr(str,_T('.')))!=NULL){
		//if found, disallow additional occurences:
		if(_tcschr(t+1,_T('.')))
			return false;
	}
	//if here, succeded
	return true;
}

//
//
//
uint
TDoubleValidator::Transfer(LPTSTR  s, void* buffer, TTransferDirection direction)
{
	if (Options & voTransfer) {
		if (direction == tdGetData)
			*(double *)buffer = _ttof(s);
		else if (direction == tdSetData)
			_stprintf(s, _T("%.3f"), *(double *)buffer);  // need wsprintf for char *
		return sizeof(double);
	}
	else
		return 0;
}

//
// Adjust the 'value' of the text, given a cursor position & an amount
// Return the actual amount adjusted.
//
int
TDoubleValidator::Adjust(owl_string& text, uint& /*begPos*/, uint& /*endPos*/,
						 int amount)
{
	double value = _ttof(text.c_str());
	double newValue = value + (double)amount;
	if (newValue < Min)
		newValue = Min;
	else if (newValue > Max)
		newValue = Max;

	_TCHAR buffer[15];
	_stprintf(buffer, _T("%.3f"), newValue);
	text = buffer;

	return int(newValue - value);
}


OWLEXT_END_NAMESPACE
//==============================================================================
