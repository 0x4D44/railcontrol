// OWLCVT 05/11/95 22:40:15
/* SECTION.CPP
*  ===========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     :
*  Version  : 1.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"
#include <cmath>

// The actual class code

// Initialize static members
float TSection::XScaleFactor = 1.0;
float TSection::YScaleFactor = 1.0;


TSection::TSection(int Number, int X[4], int Y[4])
{
  int i;

  // Initialize the class
  SectNumber = Number;
  Occupied = FALSE;
  for (i=0; i<4; i++)
  {
    SetPoint[i].x = X[i];
    SetPoint[i].y = Y[i];
  }
}


TSection::~TSection()
{
}


float TSection::SetXScaleFactor(float NewFactor)
{
  // Set the scale factor to the new value and return the old value
  float Temp;

  Temp = XScaleFactor;
  XScaleFactor = NewFactor;
  return Temp;
}


float TSection::SetYScaleFactor(float NewFactor)
{
  // Set the scale factor to the new value and return the old value
  float Temp;

  Temp = YScaleFactor;
  YScaleFactor = NewFactor;
  return Temp;
}


void TSection::GetSection(POINT ThePoints[4])
{
  int i;

  // Scale points
  for (i=0; i<4; i++)
  {
    ThePoints[i].x = static_cast<int>(std::lround(SetPoint[i].x * XScaleFactor));
    ThePoints[i].y = static_cast<int>(std::lround(SetPoint[i].y * YScaleFactor));
  }
}


BOOL TSection::IsOccupied()
{
  // Is the section occupied?
  return Occupied;
}


BOOL TSection::SetOccupied(BOOL NewState)
{
  BOOL OldValue;

  // Update the <Occupied> flag
  OldValue = Occupied;
  Occupied = NewState;
  return OldValue;
}

