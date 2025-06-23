/* TIMETABL.CPP
*  ============
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     :
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "timetabl.h"
#include "general.h"



// The actual class code
TTimetable::TTimetable(LPSTR GArrDesc, LPSTR GDepDesc, int GValue[9])
{
  int  A;     // Used to calculate arrival delay

  // Initialize the class
  strncpy(ArrDesc, GArrDesc, 20);
  strncpy(DepDesc, GDepDesc, 20);
  ArrDesc[20] = 0;
  DepDesc[20] = 0;
  ArrPnt = GValue[0];
  ArrTime = GValue[1];
  RelTime = GValue[2];
  DepTime = GValue[3];
  ArrCode = GValue[4];
  DepCode = GValue[5];
  Status = GValue[6];
  NextTimeTabPos = GValue[8];

  if (GValue[7] == 0)
  {
    MinLate = 0;
  }
  else
  {
    // Calculate +/- arrival time (minutes late/early)
    A = random(100);
    MinLate = 0;
    if (A == 0)                  MinLate = -12;
    if (A == 1)                  MinLate = -11;
    if (A == 2)                  MinLate = -10;
    if ((A >= 3) && (A <= 4))    MinLate = -9;
    if ((A >= 5) && (A <= 6))    MinLate = -8;
    if ((A >= 7) && (A <= 8))    MinLate = -7;
    if ((A >= 9) && (A <= 10))   MinLate = -6;
    if ((A >= 11) && (A <= 12))  MinLate = -5;
    if ((A >= 13) && (A <= 16))  MinLate = -4;
    if ((A >= 17) && (A <= 20))  MinLate = -3;
    if ((A >= 21) && (A <= 29))  MinLate = -2;
    if ((A >= 30) && (A <= 39))  MinLate = -1;
    if ((A >= 40) && (A <= 59))  MinLate = 0;
    if ((A >= 60) && (A <= 69))  MinLate = 1;
    if ((A >= 70) && (A <= 78))  MinLate = 2;
    if ((A >= 79) && (A <= 82))  MinLate = 3;
    if ((A >= 83) && (A <= 86))  MinLate = 4;
    if ((A >= 87) && (A <= 88))  MinLate = 5;
    if ((A >= 89) && (A <= 90))  MinLate = 6;
    if ((A >= 91) && (A <= 92))  MinLate = 7;
    if ((A >= 93) && (A <= 94))  MinLate = 8;
    if ((A >= 96) && (A <= 96))  MinLate = 9;
    if (A == 97)                 MinLate = 10;
    if (A == 98)                 MinLate = 11;
    if (A == 99)                 MinLate = 12;
  }

  Loco = 0;
  Route = 0;
  LocoPlatFlag = GValue[7];
}


TTimetable::~TTimetable()
{
}


void TTimetable::GetArrDesc(LPSTR Text)
{
  // Copy arrival description to <Text>
  strcpy(Text, ArrDesc);
}


void TTimetable::GetDepDesc(LPSTR Text)
{
  // Copy departure description to <Text>
  strcpy(Text, DepDesc);
}


int TTimetable::GetArrPnt()
{
  // Return the arrival point
  return ArrPnt;
}


int TTimetable::GetArrTime()
{
  // Return the arrival time
  return ArrTime;
}


int TTimetable::GetRelTime()
{
  // Return the stock release time
  return RelTime;
}


int TTimetable::GetDepTime()
{
  // Return the departure time
  return DepTime;
}


int TTimetable::GetArrCode()
{
  // Return the arrival code
  return ArrCode;
}


int TTimetable::GetDepCode()
{
  // Return the departure code
  return DepCode;
}


int TTimetable::GetMinLate()
{
  // Return the minutes late value
  return MinLate;
}


int TTimetable::GetLoco()
{
  // Return the loco value
  return Loco;
}


int TTimetable::GetStatus()
{
  // Return loco status flag
  return Status;
}


int TTimetable::GetRoute()
{
  // Return the route number
  return Route;
}


int TTimetable::GetLocoPlatFlag()
{
  // Return loco platform flag
  return LocoPlatFlag;
}


int TTimetable::GetNextTimeTabPos()
{
  // Return the timetable position of the train which inherits this loco
  return NextTimeTabPos;
}


int TTimetable::SetRelTime(int NewTime)
{
  int Temp;

  // Set the new time and return the old time value
  Temp = RelTime;
  RelTime = NewTime;
  return Temp;
}


int TTimetable::SetArrPnt(int NewPnt)
{
  int Temp;

  // Set the new arrival point and return the old one
  Temp = ArrPnt;
  ArrPnt = NewPnt;
  return Temp;
}


int TTimetable::SetArrCode(int NewCode)
{
  int Temp;

  // Set the new code and return the old code value
  Temp = ArrCode;
  ArrCode = NewCode;
  return Temp;
}


int TTimetable::SetMinLate(int NewMinLate)
{
  int Temp;

  // Set the new value of <MinLate> and return the old value
  Temp = MinLate;
  MinLate = NewMinLate;
  return Temp;
}


int TTimetable::SetLoco(int NewNumber)
{
   int Temp;

   // Set the new code and return the old value
   Temp = Loco;
   Loco = NewNumber;
   return Temp;
}


int TTimetable::SetStatus(int NewStatus)
{
  int Temp;

  // Set the new status flag and return the old one
  Temp = Status;
  Status = NewStatus;
  return Temp;
}


int TTimetable::SetRoute(int NewRoute)
{
  int Temp;

  // Set the new route code and return the old one
  Temp = Route;
  Route = NewRoute;
  return Temp;
}


int TTimetable::SetLocoPlatFlag(int NewFlag)
{
  int Temp;

  // Set the new flag and return the old one
  Temp = LocoPlatFlag;
  LocoPlatFlag = NewFlag;
  return Temp;
}


int  TTimetable::AddMinLate(int NewMinLate)
{
  int Temp;

  // Add the extra delay to MinLate
  Temp = MinLate;
  MinLate += NewMinLate;
  return Temp;
}




