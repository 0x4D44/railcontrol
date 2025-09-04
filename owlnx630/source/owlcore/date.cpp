//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1997 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
// TDate class implementation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DATE_H)
#include <owl/date.h>
#endif
#if !defined(OWL_TIME_H)
#include <owl/time.h>
#endif

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <tchar.h>

__OWL_BEGIN_NAMESPACE

/****************************************************************
 *                                                              *
 *                      static constants                        *
 *                                                              *
 ****************************************************************/

const uint32 SECONDS_IN_DAY  = 86400L;


static const _TUCHAR sDaysInMonth[12] =
    { 31,28,31,30,31,30,31,31,30,31,30,31 };
static const DayTy sFirstDayOfEachMonth[12] =
    { 1,32,60,91,121,152,182,213,244,274,305,335 };

// default names
static const _TCHAR* DefMonthNames[12] =
    { _T("January"),_T("February"),_T("March"),_T("April"),_T("May"),_T("June"),
      _T("July"),_T("August"),_T("September"),_T("October"),_T("November"),_T("December") };
static const TCHAR* DefUCMonthNames[12] =
    { _T("JANUARY"),_T("FEBRUARY"),_T("MARCH"),_T("APRIL"),_T("MAY"),_T("JUNE"),
      _T("JULY"),_T("AUGUST"),_T("SEPTEMBER"),_T("OCTOBER"),_T("NOVEMBER"),_T("DECEMBER") };

static const _TCHAR* DefWeekDayNames[7] =
    { _T("Monday"),_T("Tuesday"),_T("Wednesday"),
      _T("Thursday"),_T("Friday"),_T("Saturday"),_T("Sunday") };
static const TCHAR* DefUCWeekDayNames[7] =
    { _T("MONDAY"),_T("TUESDAY"),_T("WEDNESDAY"),
      _T("THURSDAY"),_T("FRIDAY"),_T("SATURDAY"),_T("SUNDAY") };


static const _TCHAR** MonthNames          = DefMonthNames;
static const TCHAR**  UCMonthNames        = DefUCMonthNames;
static const _TCHAR** WeekDayNames        = DefWeekDayNames;
static const TCHAR**  UCWeekDayNames      = DefUCWeekDayNames;

_TCHAR TDate::ShortDateFmt[11] = _T("");

_OWLSTATICFUNC(static int) FindMatch( const _TCHAR *str, const _TCHAR**candidates, int icand );

/***************************************************************************/

//      constructors

/***************************************************************************/

//
// Construct a TDate for today's date.
//
TDate::TDate()
{
  //long clk = time(0);
  //struct tm _FAR *now = localtime(&clk);
  // We no longer need to default to adding 1900 - (GSF) ???????????
  //long _Julnum = Jday(now->tm_mon+1, now->tm_mday, now->tm_year+1900);
  TSystemTime _clk(TSystemTime::LocalTime());
  Julnum = Jday( _clk.GetMonth(), _clk.GetDay(), _clk.GetYear());
}

//
// Construct a TDate with a given day of the year and a given year.  The
// base date for this computation is Dec. 31 of the previous year.  If
// year == 0, Construct a TDate with Jan. 1, 1901 as the "day zero".
// i.e., TDate(-1) = Dec. 31, 1900 and TDate(1) = Jan. 2, 1901.
//
TDate::TDate(DayTy day, YearTy year)
{
  if( year )
    Julnum = Jday( 12, 31, year-1 ) + (JulTy)day;
  else
    Julnum = jul1901                + (JulTy)day;
}

//
//   Construct a TDate for the given day, monthName, and year.
//
TDate::TDate( DayTy day, const _TCHAR * monthName, YearTy year )
{
  Julnum = Jday( IndexOfMonth(monthName), day, year );
}

//
//   Construct a TDate for the given day, month, and year.
//
TDate::TDate( DayTy day, MonthTy month, YearTy year )
{
  Julnum = Jday( month, day, year );
}

//
// Type conversion to date.
//
TDate::TDate( const TTime  & t )
{
  Julnum = t.IsValid() ? jul1901 + (JulTy)(t.LocalSecs()/SECONDS_IN_DAY) : 0;
}

//
// Construct a TDate from a TSystemTime
//
TDate::TDate( const TSystemTime & t)
{
  Julnum = Jday( t.GetMonth(), t.GetDay(), t.GetYear());
}

//
//
//
TSystemTime
TDate::GetSystemTime() const
{
  MonthTy m; DayTy d; YearTy y;
  Mdy(m, d, y);
  return TSystemTime(y, m, d, 0, 0, 0, WeekDay());
}

/***************************************************************************/

//                      static member functions

/***************************************************************************/

//
// Returns a string name for the weekday number.
// Monday == 1, ... , Sunday == 7
// Return 0 for weekday number out of range
//
const _TCHAR * TDate::DayName( DayTy weekDayNumber )
{
    return AssertWeekDayNumber(weekDayNumber) ? WeekDayNames[weekDayNumber-1] : 0;
}

//
// Return the number, 1-7, of the day of the week named nameOfDay.
// Return 0 if name doesn't match.
//
DayTy TDate::DayOfWeek( const _TCHAR  *nameOfDay )
{
  return (DayTy)(FindMatch( nameOfDay, UCWeekDayNames, 7 )+1);
}

// Return the number of days of a given month
// Return 0 if "month" is outside of the range 1 through 12, inclusive.
int TDate::DaysInMonth(MonthTy month) const
{
  uint days = 0;

  if (month == 0)
    month = Month();

  if (AssertIndexOfMonth(month)) {
    days = sDaysInMonth[month-1];
    if(Leap() && month == 2)
      days++;
  }

  return days;
}

//
// Is a day (1-31) within a given month?
//
int TDate::DayWithinMonth( MonthTy month, DayTy day, YearTy year )
{
  if( day <= 0 || !AssertIndexOfMonth(month) )
      return 0;
  unsigned d = sDaysInMonth[month-1];
  if( LeapYear(year) && month == 2 )
      d++;
  return day <= d;
}

//
// How many days are in the given YearTy year?
//
DayTy TDate::DaysInYear( YearTy year )
{
  return LeapYear(year) ? 366 : 365;
}

//
// Returns the number, 1-12, of the month named nameOfMonth.
// Return 0 for no match.
//
MonthTy TDate::IndexOfMonth( const _TCHAR  *nameOfMonth )
{
  return (MonthTy)(FindMatch( nameOfMonth, UCMonthNames, 12 )+1);
}

//
// Convert Gregorian calendar date to the corresponding Julian day
// number j.  Algorithm 199 from Communications of the ACM, Volume 6, No.
// 8, (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
// This function not valid before that.
// Returns 0 if the date is invalid.
//
JulTy TDate::Jday( MonthTy m, DayTy d, YearTy y )
{
  uint32 c, ya;
  // We now always default to adding 1900 like the TDate CTOR - (GSF)
  // if( y <= 99 ).
  if( y <= 137 ) // year 2037 problem (long overflow)
    y += 1900;
  if( !DayWithinMonth(m, d, y) )
    return (JulTy)0;

  if( m > 2 )
    m -= 3;
  else{
    m += 9;
    y--;
  }

  c = y / 100;
  ya = y - 100*c;
  return ((146097L*c)>>2) + ((1461*ya)>>2) + (153*m + 2)/5 + d + 1721119L;
}

//
// Algorithm from K & R, "The C Programming Language", 1st ed.
//
int TDate::LeapYear( YearTy year )
{
  return (year&3) == 0 && year%100 != 0 || year % 400 == 0;
}

//
// Returns a string name for the month number.
// Return 0 if invalid month number.
//
const _TCHAR  *TDate::MonthName( MonthTy monthNumber )
{
    return AssertIndexOfMonth(monthNumber) ? MonthNames[monthNumber-1] : 0;
}

//
// Return index of case-insensitive match; -1 if no match.
//
_OWLSTATICFUNC(static int)
FindMatch( const _TCHAR* str, const _TCHAR** candidates, int icand )
{
  unsigned len = ::_tcslen(str);

  while(icand--){
    if( _tcsnicmp(str, candidates[icand], len) == 0)
      break;
  }
  return icand;
}

/****************************************************************
 *                                                              *
 *                      Member functions                        *
 *                                                              *
 ****************************************************************/

//
// Compare function:
//
int TDate::CompareTo( const TDate  &d ) const
{
  if( Julnum < d.Julnum )
    return -1;
  else if( Julnum > d.Julnum )
    return 1;
  else
    return 0;
}

//
//
//
DayTy TDate::Day() const
{
  return DayTy(Julnum - Jday( 12, 31, Year()-1 ));
}

//
// Returns the day of the month of this TDate.
//
DayTy TDate::DayOfMonth() const
{
  MonthTy m; DayTy d; YearTy y;
  Mdy( m, d, y );
  return d;
}

//
// Return the number of the first day of a given month
// Return 0 if "month" is outside of the range 1 through 12, inclusive.
//
DayTy TDate::FirstDayOfMonth( MonthTy month ) const
{
  if ( !AssertIndexOfMonth(month) )
    return 0;
  unsigned firstDay = sFirstDayOfEachMonth[month-1];
  if (month > 2 && Leap())
    firstDay++;
  return firstDay;
}

unsigned TDate::Hash() const
{
  return (unsigned)Julnum;
}

//
// Convert a Julian day number to its corresponding Gregorian calendar
// date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
// (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
// This function not valid before that.
//
_OWLSTATICFUNC(void) 
TDate::Mdy( MonthTy & m, DayTy & d, YearTy & y ) const
{
  uint32 _d;
  JulTy j = Julnum - 1721119L;
  y = (YearTy) (((j<<2) - 1) / 146097L);
  j = (j<<2) - 1 - 146097L*y;
  _d = (j>>2);
  j = ((_d<<2) + 3) / 1461;
  _d = (_d<<2) + 3 - 1461*j;
  _d = (_d + 4)>>2;
  m = (MonthTy)(5*_d - 3)/153;
  _d = 5*_d - 3 - 153*m;
  d = (DayTy)((_d + 5)/5);
  y = (YearTy)(100*y + j);

  if( m < 10 )
    m += 3;
  else{
    m -= 9;
    y++;
  }
}

//
//
//
TDate 
TDate::Max( const TDate  & dt ) const
{
  return dt.Julnum > Julnum ? dt : *this;
}

//
//
//
TDate 
TDate::Min( const TDate  & dt ) const
{
  return dt.Julnum < Julnum ? dt : *this;
}

//
// Returns the month of this TDate.
//
MonthTy 
TDate::Month() const
{
  MonthTy m; DayTy d; YearTy y;
  Mdy(m, d, y);
  return m;
}

//
//
//
TDate 
TDate::Previous( const _TCHAR  *dayName) const
{
  return Previous( DayOfWeek(dayName) );
}

//
//
//
TDate 
TDate::Previous( DayTy desiredDayOfWeek ) const
{
  //    Renumber the desired and current day of week to start at 0 (Monday)
  //    and end at 6 (Sunday).

  desiredDayOfWeek--;
  DayTy thisDayOfWeek = WeekDay() - 1;
  JulTy j = Julnum;

  //    Have to determine how many days difference from current day back to
  //    desired, if any.  Special calculation under the 'if' statement to
  //    effect the wraparound counting from Monday (0) back to Sunday (6).
  if( desiredDayOfWeek > thisDayOfWeek )
    thisDayOfWeek += 7 - desiredDayOfWeek;
  else
    thisDayOfWeek -= desiredDayOfWeek;
  j -= thisDayOfWeek; // Adjust j to set it at the desired day of week.
  return  TDate(j);
}

//
//
//
DayTy TDate::WeekDay(DayTy day) const
{
  JulTy julnum = (day == 0 ? Julnum : Jday(day, Month(), Year()));
  return DayTy(((((julnum+1)%7)+6)%7)+1);
}

//
// Returns the year of this TDate.
//
YearTy TDate::Year() const
{
  MonthTy m; DayTy d; YearTy y;
  Mdy(m, d, y);
  return y;
}

//
// Convert a string to a date according to the [Intl] section in win.ini or the
// passed format specifier
void TDate::ParseFrom(LPCTSTR str, LPCTSTR format)
{
  struct Local
  {
    static int accept_digits(LPCTSTR str)
    {
      int n = 0;
      while (_istdigit(str[n]))
        n++;
      return n;
    }

    static int skip(LPCTSTR picture, TCHAR c)
    {
      int n = 0;
      while (_totlower(picture[n]) == c)
        n++;
      return n;      
    }

    static int parse_number(LPCTSTR& str, LPCTSTR& picture, TCHAR format_char)
    {
      const int n = Local::accept_digits(str);
      int result = _ttoi(owl_string(str, 0, n).c_str());
      str += n;
      picture += Local::skip(picture, format_char);
      return result;      
    }
  };

  int day, month, year;
  Julnum = day = month = year = 0;

  GetProfileString(_T("intl"), _T("sShortDate"),  _T("MM/dd/yyyy"), ShortDateFmt, COUNTOF(ShortDateFmt));
  LPCTSTR picture = format ? format : ShortDateFmt;

  while (*picture && *str) {
    TCHAR c = _totlower(*picture);
    switch (c) {
      case _T('d'): {
        day = Local::parse_number(str, picture, c);
        break;
      }
      case _T('m'): {
        month = Local::parse_number(str, picture, c);
        break;
      }
      case _T('y'): {
        year = Local::parse_number(str, picture, c);
        break;
      }
      default: {
        picture++;
        str++;
      }
    }
  }

  Julnum = Jday(month, day, year);
  if (Julnum < 1)
    Julnum = 0;
}

//
//
//
void
TDate::SetIntlNames(TDateType type, const _TCHAR** names)
{
  const _TCHAR*** Names[] = { &MonthNames, &UCMonthNames,
                             &WeekDayNames, &UCWeekDayNames,
                           };
  const _TCHAR*** DefNames[] = {
    (const _TCHAR***)&DefMonthNames, (const _TCHAR***)&DefUCMonthNames,
    (const _TCHAR***)&DefWeekDayNames, (const _TCHAR***)&DefUCWeekDayNames,
  };
  if(names)
    *(Names[type])  = names;
  else
    *(Names[type])  = *(DefNames[type]);
}

//
//
//
const _TCHAR*
TDate::GetIntlName(TDateType type, int index)
{
  const _TCHAR** Names[] = { MonthNames, UCMonthNames,
                             WeekDayNames, UCWeekDayNames,
                           };
  return Names[type][index];
}

//
//
//
_OWLFUNC(TDate) operator + ( const TDate& dt, int dd )
{
  return TDate(dt.Julnum + dd);
}

//
//
//
_OWLFUNC(TDate) operator + ( int dd, const TDate& dt )
{
  return TDate(dt.Julnum + dd);
}

//
//
//
_OWLFUNC(TDate) operator - ( const TDate& dt, int dd )
{
  return TDate(dt.Julnum - dd);
}

//
//
//
__OWL_END_NAMESPACE
/* ========================================================================== */
