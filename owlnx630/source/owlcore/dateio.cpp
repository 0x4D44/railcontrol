//----------------------------------------------------------------------------
// Borland Class Library
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// TDate class IO and conversion implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/date.h>
#include <stdio.h>
#include <ctype.h>

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

//
//
//
TDate::HowToPrint TDate::PrintOption = TDate::Normal;
//TDate::HowToPrint TDate::PrintOption = TDate::WinIntSection;

//
//
//
owl_string TDate::AsString() const
{
#if defined(BI_STD_NEWSSTREAM)
  _tostrstream strtemp;
#else
  _TCHAR buf[80];
  _tostrstream strtemp(buf, COUNTOF(buf));
#endif
  strtemp << (*this) << ends;
#if defined(BI_STD_NEWSSTREAM)
  return strtemp.str();
#else
  return owl_string(buf);
#endif
}

//
//
//
TDate::HowToPrint TDate::SetPrintOption( HowToPrint h )
{
  HowToPrint oldoption = PrintOption;
  PrintOption = h;
  return oldoption;
}

//
// Skip any characters except alphanumeric characters
//
_OWLSTATICFUNC(static void) SkipDelim(_tistream& strm )
{
  _TCHAR c;
  if( !strm.good() )
    return;

  do  {
    strm >> c;
  } while (strm.good() && !(_istdigit(c)||_istalpha(c))) ; //_istalnum is not yet implemented in WineLib

  if (strm.good())
    strm.putback(c);
}

//
// Parse the name of a month from input stream.
//
_OWLSTATICFUNC(static _TCHAR*)
ParseMonth(_tistream& s, _TCHAR* month)
{
//  static _TCHAR month[12];
  _TCHAR* p = month;
  _TCHAR c;
  SkipDelim(s);
  s.get(c);
  while (s.good() && _istalpha(c) && (p != &month[10])){
    *p++ = c;
    s.get(c);
  }
  if( s.good() )
    s.putback(c);
  *p = _T('\0');
  return month;
}

//
//  Parse a date from the specified input stream.
//    The date must be in one of the following forms:
//                dd-mmm-yy, mm/dd/yy, or mmm dd,yy
//        e.g.: 10-MAR-86,  3/10/86, or March 10, 1986.
//  Any non-alphanumeric character may be used as a delimiter.
//
void TDate::ParseFrom(_tistream& s )
{
  unsigned d,m,y;
  Julnum = 0;                 // Assume failure
  _TCHAR month[20];

  if (s.good()){
    SkipDelim(s);
    s >> m;                 // try to parse day or month number
    SkipDelim(s);
    if (s.eof())
      return;
    if( s.fail()){          // parse <monthName><day><year>
      s.clear();
      m = IndexOfMonth(ParseMonth(s,month)); // parse month name
      SkipDelim(s);
      s >> d;                 // parse day
    }
    else{                        // try to parse day number
      s >> d;
      if (s.eof())
        return;
      if (s.fail()){          // parse <day><monthName><year>
        d = m;
        s.clear();
        m = IndexOfMonth(ParseMonth(s,month)); // parse month name
      }
    }
    SkipDelim(s);
    s >> y;
  }
  Julnum = s.good() ? Jday(m, d, y) : 0;
  if(Julnum==0)
    s.clear(ios::badbit);
}

//
//
//
_OWLCFUNC( _tostream&) operator << ( _tostream& s, const TDate& d )
{
  // we use an _tostrstream to format into buf so that
  // we don't affect the ostream's width setting.
  //
  _TCHAR buf[80];
#if defined(BI_STD_NEWSSTREAM)
  _tostrstream out;
#else
  _tostrstream out( buf, COUNTOF(buf) );
#endif

  switch ( TDate::PrintOption )
      {
      case TDate::Normal:
          out.width(2);
           out.fill(_T('0'));
          out << d.NameOfMonth() << _T(" ")\
              << d.DayOfMonth()  << _T(", ")\
              << d.Year();
          break;
      case TDate::Terse:
          _stprintf(buf,_T("%02u-%.3s-%.4u"),
                  d.DayOfMonth(),
                  d.NameOfMonth(),
                  d.Year());
#if defined(BI_STD_NEWSSTREAM)
          out << buf;
#endif
          break;
      case TDate::Numbers:
          out.width(2);
           out.fill('0');
          out << d.Month() << _T("/")
              << d.DayOfMonth() << _T("/")
              << d.Year();
          break;
      case TDate::EuropeanNumbers:
          out.width(2);
           out.fill('0');
          out << d.DayOfMonth()   << _T("/")\
              << d.Month() << _T("/")\
              << d.Year();
          break;
      case TDate::European:
          out.width(2);
           out.fill('0');
          out << d.DayOfMonth() << _T(" ")\
              << d.NameOfMonth() << _T(" ")\
              << d.Year();
          break;
      case TDate::WinIntSection: {
           out.fill(_T('0'));
          GetProfileString(_T("intl"), _T("sShortDate"),  _T("mm/dd/yyyy"), TDate::ShortDateFmt, COUNTOF(TDate::ShortDateFmt));
          LPCTSTR  picture = TDate::ShortDateFmt;
          while (*picture) {
            switch (*picture) {
              case _T('d'):  case _T('D'): {
                int length = 0;
                while (*picture == _T('d') || *picture == _T('D')) {
                  picture++;
                  length++;
                }
                out.width(length <  2 ? 2 : length);
                out << d.DayOfMonth();
                break;
              }
              case _T('m'):  case _T('M'): {
                int length = 0;
                while (*picture == _T('m') || *picture == _T('M')) {
                  picture++;
                  length++;
                }
                out.width(length <  2 ? 2 : length);
                out << d.Month();
                break;
              }
              case _T('y'):  case _T('Y'): {
#if 0 // Y2B
                int length = 0;
                while (*picture == _T('y') || *picture == _T('Y')) {
                  picture++;
                  length++;
                }
                if (length == 2)
                  out << (d.Year() % 100);
                else
                  out << d.Year();
#else
                while (*picture == _T('y') || *picture == _T('Y'))
                  picture++;
                out << d.Year();
#endif
                break;
              }
              default: {
                out << *picture;
                picture++;
                break;
              }
            }
          }
          break;
       }
  }

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

_OWLCFUNC(_tistream&) operator >> ( _tistream& s, TDate& d )
{
    d.ParseFrom(s);
    return s;
}

__OWL_END_NAMESPACE
/* ========================================================================== */

