//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// TTime class IO and conversion implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_TIME_H)
#include <owl/time.h>
#endif

#if !defined(OWL_FILE_H)
#include <owl/file.h>
#endif

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif
#if defined(BI_STD_IO)
#  include <iomanip>
#else
#  include <iomanip.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

//
// Static variable intialization:
//
int TTime::PrintDateFlag = 1;

owl_string TTime::AsString() const
{
#if defined(BI_STD_NEWSSTREAM)
  _tostrstream strtemp;
#else
  _TCHAR buf[80];
  _tostrstream strtemp(buf, 80);
#endif
  strtemp << (*this) << ends;
#if defined(BI_STD_NEWSSTREAM)
  return strtemp.str();
#else
  return owl_string(buf);
#endif
}
/* -------------------------------------------------------------------------- */
_OWLCFUNC(_tostream &) operator << ( _tostream & s, const TTime & t )
{
  // We use an ostrstream to format into buf so that
  // we don't affect the ostream's width setting.
  //
#if defined(BI_STD_NEWSSTREAM)
  _tostrstream out;
#else
  TCHAR buf[80];
  _tostrstream out( buf, 80 );
#endif
  // First print the date if requested:
  //
  if(TTime::PrintDateFlag)
      out << TDate(t) << _T(" ");

  unsigned hh = t.Hour();
  out << (hh <= 12 ? hh : hh-12) << _T(':');
#if defined(BI_STD_IO)   // otherwise caused internal compiler error with MSC6
  out << std::setfill(_T('0'));
  out << std::setw(2);
#else
  out << setfill(_T('0'));
  out << setw(2);
#endif
  out << t.Minute() << _T(':');
#if defined(BI_STD_IO)   // otherwise caused internal compiler error with MSC6
  out << std::setw(2);
#else
  out << setw(2);
#endif
  out << t.Second() << _T(' ');
#if defined(BI_STD_IO)   // otherwise caused internal compiler error with MSC6
  out << std::setfill(_T(' '));
#else
  out << setfill(_T(' '));
#endif
  out << ( hh<12 ? _T("am") : _T("pm"));

  // now we write out the formatted buffer, and the ostream's
  // width setting will control the actual width of the field.
  //
#if defined(BI_STD_NEWSSTREAM)
  s << out.str();
#else
  s << buf;
#endif
  return s;
}

int TTime::PrintDate( int f )
{
  int temp = PrintDateFlag;
  PrintDateFlag = f;
  return temp;
}

_OWLCFUNC(_tostream  &) operator << ( _tostream  & os, const TFileStatus  & status )
{
  os << _T("File Status: ") << status.fullName << endl;
  os << _T("    created: ") << status.createTime << endl;
  os << _T("   modified: ") << status.modifyTime << endl;
  os << _T("   accessed: ") << status.accessTime << endl;
  os << _T("       size: ") << status.size << endl;
  os << _T(" attributes: ") << (int)status.attribute << endl;
  return os;
}

__OWL_END_NAMESPACE
/* ========================================================================== */
