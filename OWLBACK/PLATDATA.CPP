/* PLATDATA.CPP
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
#include "platdata.h"

// The actual class code

// Initialize static members
float TPlatData::XScaleFactor = 1.0;
float TPlatData::YScaleFactor = 1.0;


TPlatData::TPlatData(int X[4], int Y[4])
{
  int i;

  // Initialize the class
  Timetable = 0;
  Loco = 0;
  for (i=0; i<4; i++)
  {
    SetPoint[i].x = X[i];
    SetPoint[i].y = Y[i];
  }

  SelectorRef = 0;
  Redraw = FALSE;
}


TPlatData::~TPlatData()
{
}


float TPlatData::SetXScaleFactor(float NewFactor)
{
  // Set the scale factor to the new value and return the old value
  float Temp;

  Temp = XScaleFactor;
  XScaleFactor = NewFactor;
  return Temp;
}


float TPlatData::SetYScaleFactor(float NewFactor)
{
  // Set the scale factor to the new value and return the old value
  float Temp;

  Temp = YScaleFactor;
  YScaleFactor = NewFactor;
  return Temp;
}


void TPlatData::GetSection(POINT ThePoints[4])
{
  int i;

  // Scale points
  for (i=0; i<4; i++)
  {
    ThePoints[i].x = SetPoint[i].x * XScaleFactor;
    ThePoints[i].y = SetPoint[i].y * YScaleFactor;
  }
}


int TPlatData::GetTimetable()
{
  // Get timetable number
  return Timetable;
}


int TPlatData::SetTimetable(int NewNumber)
{
  int Temp;

  // Update timetable number
  Temp = Timetable;
  Timetable = NewNumber;
  return Temp; 
}


int TPlatData::GetLoco()
{
  // Get loco number
  return Loco;
}


int TPlatData::SetLoco(int NewNumber)
{
  int Temp;

  // Update loco number
  Temp = Loco;
  Loco = NewNumber;
  return Temp; 
}


int TPlatData::SetSelectorRef(int NewNumber)
{
  int Temp;

  // Set selector reference to the given value and return the old value
  Temp = SelectorRef;
  SelectorRef = NewNumber;
  return Temp;
}


BOOL TPlatData::SetRedraw(BOOL Flag)
{
  BOOL Temp;

  // Set the platform redraw flag and return the old one
  Temp = Redraw;
  Redraw = Flag;
  return Temp;
}


int TPlatData::GetSelectorRef()
{
  // Return the selector reference
  return SelectorRef;
}


BOOL TPlatData::GetRedraw()
{
  // Return the platform redraw flag
  return Redraw;
}
