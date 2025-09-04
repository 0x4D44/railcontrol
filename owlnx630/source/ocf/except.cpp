//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.13 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:48 $
//
// OLE Exception classes
//----------------------------------------------------------------------------
#define INC_OLE2
#include <ocf/pch.h>
#if !defined(OCF_DEFS_H)
# include <ocf/defs.h>
#endif
#if !defined(OCF_OLEUTIL_H)
# include <ocf/oleutil.h>
#endif
#if !defined(OWL_STRING_H)
# include <owl/string.h>
#endif


__OCF_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

//
//
//
#if defined(BI_STD_RTL)
TXOle::~TXOle() throw()
#else
TXOle::~TXOle()
#endif
{
}

//
//
//
#if defined(BI_NO_COVAR_RET)
TXBase*			
#else
TXOle*      
#endif
TXOle::Clone()
{
  return new TXOle(*this);
}

//
//
//
void
TXOle::Throw()
{
  THROW( *this );
}

//
//
//
void
TXOle::Check(HRESULT hr, LPCTSTR msg)
{
  if (FAILED(hr))
    Throw(hr, msg);
}

//
//
//
void
TXOle::Check(HRESULT hr)
{
  Check(hr, 0);
}

//
//
//
void
TXOle::Throw(HRESULT hr, LPCTSTR msg)
{
	if (!InstanceCount) {
		int msg_len = msg ? _tcslen(msg) : 0;
		TAPointer<_TCHAR> buf = new _TCHAR[256 + msg_len];
		wsprintf(buf, _T("%s failed, "), msg ? msg : _T("OLE call"));

		int len = _tcslen(buf);
		OleErrorFromCode(hr, buf + len, 256 + msg_len - len - 2);
		_tcscat(buf, _T("."));

		WARN(hr != HR_NOERROR, buf);
		throw TXOle((_TCHAR*)buf, hr);
  }
}

//
//
//
void
TXOle::OleErrorFromCode(HRESULT stat, LPTSTR buffer, int size)
{
	owl_string error = TSystemMessage(stat).SysMessage();
	int len = _tcslen(error.c_str());
	len = len > size ? len-1 : size-1;
	_tcsncpy(buffer,error.c_str(),len);
	buffer[len] = 0;
}

__OCF_END_NAMESPACE

//==============================================================================


