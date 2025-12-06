// OWLCVT 05/11/95 22:40:38
/* LOCOS.CPP
*  =========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"
#include <algorithm>
#include <string>

// The actual class code
TLocos::TLocos(int GClass, int GNumber, int GType)
{
  // Initialize the class
  Class = GClass;
  Number = GNumber;
  Type = GType;
  Flag = LF_UNASSIGN;
  RefuelTime = 0;
}


TLocos::~TLocos()
{
}


int TLocos::GetFlag()
{
  // Return the locomotive flag
  return Flag;
}


int TLocos::SetFlag(int NewFlag)
{
  int Temp;

  // Set the new flag and return the old one
  Temp = Flag;
  Flag = NewFlag;
  return Temp;
}


int TLocos::GetType()
{
  // Return loco type
  return Type;
}


int TLocos::GetRefuelTime()
{
  // Return <RefuelTime>
  return RefuelTime;
}


int TLocos::SetRefuelTime(int NewTime)
{
  int Temp;

  // Set the new time and return the oldtime
  Temp = RefuelTime;
  RefuelTime = NewTime;
  return Temp;
}

void TLocos::LocoNumStr(char* Text, size_t BufferSize)
{
  if ((Text == nullptr) || (BufferSize == 0))
  {
    return;
  }

  long Temp;

  // Calculate loco number, convert it to a string and return it in <Text>
  Temp = 1000*long(Class) + Number;
  const auto locoText = std::to_string(Temp);
  if (BufferSize == 1)
  {
    Text[0] = '\0';
    return;
  }
  const size_t toCopy = std::min(BufferSize - 1, locoText.size());
  std::copy_n(locoText.data(), toCopy, Text);
  Text[toCopy] = '\0';
}


