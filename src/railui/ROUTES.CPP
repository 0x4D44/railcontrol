// OWLCVT 05/11/95 22:40:30
/* ROUTES.CPP
*  ==========
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

// The actual class code
TRoutes::TRoutes(int GFrom, int GTo, long ClearBy[6])
{
  int i;

  // Initialize the class
  From = GFrom;
  To = GTo;
  for (i=0; i<6; i++)
  {
    Clear[i] = ClearBy[i];
  }
}


TRoutes::~TRoutes()
{
}


int TRoutes::GetFrom()
{
  // Return the <From> parameter
  return From;
}


int TRoutes::GetTo()
{
  // Return the <To> parameter
  return To;
}


BOOL TRoutes::GetClear(int ToClear, int &Num1, int &Num2)
{
  // Return the <Clear> members corresponding to the ToClear value
  if ((ToClear < 0) || (ToClear > 5))  return FALSE;

  Num2 = Clear[ToClear]/1000;
  Num1 = Clear[ToClear] - 1000*Num2;
  return TRUE;
}

