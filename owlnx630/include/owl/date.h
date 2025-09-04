//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_DATE_H)
#define OWL_DATE_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TTime;
class _OWLCLASS ipstream;
class _OWLCLASS opstream;
class _OWLCLASS TSystemTime;

typedef unsigned DayTy;
typedef unsigned MonthTy;
typedef unsigned YearTy;
typedef unsigned long JulTy;

static const JulTy jul1901 = 2415386L;  // Julian day for 1/1/1901

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
//  class TDate
//  ~~~~~ ~~~~~
//
class _OWLCLASS TDate {
  public:

    enum HowToPrint {
        Normal,
        Terse,
        Numbers,
        EuropeanNumbers,
        European,
        WinIntSection,
      };

    // Construct a TDate with the current date

    TDate();

    // Construct a TDate with a given day of the year and a given year.
    // The base date for this computation is Dec. 31 of the previous year.
    // If year == 0, Construct a TDate with Jan. 1, 1901 as the "day zero".
    // i.e., TDate(-1,0) = Dec. 31, 1900 and TDate(1,0) = Jan. 2, 1901.

    TDate( DayTy day, YearTy year );

    //  Construct a TDate for the given day, month, and year.
    TDate( DayTy, LPCTSTR month, YearTy );
    TDate( DayTy, MonthTy, YearTy );
    TDate( const _tistream& s );      // Read date from stream.
    TDate( const TTime& );            // Construct a TDate from a TTime
    TDate( const TSystemTime& );      // Construct a TDate from a TSystemTime
    TDate(LPCTSTR str, LPCTSTR format = 0);


    TSystemTime GetSystemTime() const;

    owl_string AsString() const;
    int Between( const TDate& d1, const TDate& d2 ) const;
    int CompareTo( const TDate& ) const;
    DayTy Day() const;            // 1-365
    int DaysInMonth(MonthTy month=0) const; // 28-31
    DayTy DayOfMonth() const;     // 1-31
    DayTy FirstDayOfMonth() const;
    DayTy FirstDayOfMonth( MonthTy ) const;
    unsigned Hash() const;
    int IsValid() const;
    int Leap()  const;                             // leap year?
    TDate Max( const TDate& dt ) const;
    TDate Min( const TDate& dt ) const;
    MonthTy Month() const;
    const _TCHAR * NameOfDay() const;
    const _TCHAR * NameOfMonth() const;
    TDate Previous( const _TCHAR * dayName) const;  // Return date of previous dayName
    TDate Previous( DayTy ) const;                         // Same as above, but use day of week
    DayTy WeekDay(DayTy day=0) const;
    YearTy Year() const;

    // Date comparisons:
    int operator <  ( const TDate& date ) const;
    int operator <= ( const TDate& date ) const;
    int operator >  ( const TDate& date ) const;
    int operator >= ( const TDate& date ) const;
    int operator == ( const TDate& date ) const;
    int operator != ( const TDate& date ) const;

    // Arithmetic operators:
    JulTy operator - ( const TDate& dt ) const;
    friend _OWLFUNC(TDate)  operator + ( const TDate& dt, int dd );
    friend _OWLFUNC(TDate)   operator + ( int dd, const TDate& dt );
    friend _OWLFUNC(TDate)   operator - ( const TDate& dt, int dd );
    void operator ++ ();
    void operator -- ();
    void operator += ( int dd );
    void operator -= ( int dd );

    // Read or write dates:
    friend _OWLCFUNC( _tostream&)  operator << ( _tostream& s, const TDate& d );
    friend _OWLCFUNC( _tistream&) operator >> (  _tistream& s, TDate& d );

    // Read or write dates on persistent streams
    friend _OWLCFUNC(opstream&) operator << ( opstream& s, const TDate& d );
    friend _OWLCFUNC(ipstream&) operator >> ( ipstream& s, TDate& d );

    // Static member functions:
    static LPCTSTR DayName( DayTy weekDayNumber );
    static DayTy DayOfWeek( LPCTSTR dayName );
    static int DayWithinMonth( MonthTy, DayTy, YearTy );
    static DayTy DaysInYear( YearTy );
    static MonthTy IndexOfMonth( LPCTSTR monthName );
    static JulTy Jday( MonthTy, DayTy, YearTy );
    static int LeapYear( YearTy year );
    static LPCTSTR MonthName( MonthTy monthNumber );
    static HowToPrint SetPrintOption( HowToPrint h );

    // A YearTy value in the range [0..MaxShortYear) will have BaseYear
    // added to it when converting a year into a Julian day number (the
    // internal representation of TDate).  This affects the TDate
    // constructors and TDate::Jday.
    //???????????????????????????????????
    //static YearTy &BaseYear();           // default: 1900
    //static YearTy &MaxShortYear();       // default: 999

    // set reset month and days names
    enum TDateType{
      dtMonthNames,
      dtUCMonthNames,
      dtWeekDayNames,
      dtUCWeekDayNames,
    };
    static void SetIntlNames(TDateType type, const _TCHAR** names);
    static const _TCHAR* GetIntlName(TDateType type, int index);

  protected:

    static int AssertWeekDayNumber( DayTy d );
    static int AssertIndexOfMonth( MonthTy m );

  private:

    JulTy Julnum;                   // Julian Day Number (Not same as Julian date.)
    static HowToPrint PrintOption;  // Printing with different formats
    static _TCHAR ShortDateFmt[11];

    void ParseFrom(_tistream& );     // Reading dates
    void ParseFrom(LPCTSTR strDate, LPCTSTR format = 0);

    _OWLSTATICFUNC(void) Mdy( MonthTy&, DayTy&, YearTy& ) const;
    TDate( JulTy j );

};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDate inline functions                                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

inline TDate::TDate( const _tistream& s )
{
  ParseFrom((_tistream&)s);
}

inline TDate::TDate(LPCTSTR str, LPCTSTR format)
{
  ParseFrom(str, format);
}

inline int TDate::Between( const TDate& d1, const TDate& d2 ) const
{
    return Julnum >= d1.Julnum && Julnum <= d2.Julnum;
}

inline DayTy TDate::FirstDayOfMonth() const
{
  return FirstDayOfMonth(Month());
}

inline int TDate::IsValid() const
{
  return Julnum>0;
}

inline int TDate::Leap() const
{
  return LeapYear(Year());
}

inline LPCTSTR TDate::NameOfDay() const
{
  return DayName(WeekDay());
}

inline LPCTSTR TDate::NameOfMonth() const
{
  return MonthName(Month());
}

inline int TDate::operator < ( const TDate& date ) const
{
  return Julnum < date.Julnum;
}

inline int TDate::operator <= ( const TDate& date ) const
{
  return Julnum <= date.Julnum;
}

inline int TDate::operator > ( const TDate& date ) const
{
  return Julnum > date.Julnum;
}

inline int TDate::operator >= ( const TDate& date ) const
{
  return Julnum >= date.Julnum;
}

inline int TDate::operator == ( const TDate& date ) const
{
  return Julnum == date.Julnum;
}

inline int TDate::operator != ( const TDate& date ) const
{
  return Julnum != date.Julnum;
}

inline JulTy TDate::operator - ( const TDate& dt ) const
{
  return Julnum - dt.Julnum;
}

inline void TDate::operator ++ ()
{
  Julnum += 1;
}

inline void TDate::operator -- ()
{
    Julnum -= 1;
}

inline void TDate::operator += ( int dd )
{
    Julnum += dd;
}

inline void TDate::operator -= ( int dd )
{
    Julnum -= dd;
}

inline int TDate::AssertWeekDayNumber( DayTy d )
{
    return d>=1 && d<=7;
}

inline int TDate::AssertIndexOfMonth( MonthTy m )
{
    return m>=1 && m<=12;
}

inline TDate::TDate( JulTy j )
{
    Julnum = j;
}

inline unsigned HashValue( TDate& d )
{
    return d.Hash();
}

__OWL_END_NAMESPACE


#endif  // OWL_DATE_H
