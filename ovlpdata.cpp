// OWLCVT 05/11/95 22:40:23
/* OVLPDATA.CPP
*  ============
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 17-Sep-07
*  Version  : 2.3
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"

// The actual class code


TOverlapData::TOverlapData(int xiSections[2])
{
  int i;

  // Initialize the class
  for (i=0; i<2; i++)
  {
    mSections[i] = xiSections[i];
  }
}


TOverlapData::~TOverlapData()
{
}

int TOverlapData::GetSection(int xiIndex)
{
  if ((xiIndex >= 0) && (xiIndex < 2))
  {
    return(mSections[xiIndex]);
  }
  else
  {
    TRC_ERR((TB, "Invalid section index:%d", xiIndex));
    return(0);
  }
}


