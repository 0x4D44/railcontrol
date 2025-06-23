// OWLCVT 05/11/95 22:40:47
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
#include <stdio.h>
#include "timetabl.h"
#include "general.h"



// The actual class code
TTimetable::TTimetable(char* xiArrDesc, char* xiDepDesc, int xiValue[9], char* xiComment)
{
  int   laa;               // Used to calculate arrival delay
  char* lTrimArr;
  char* lTrimDep;
  char* lTrimCmt;

  // Initialize the class
  lTrimArr = Trim(xiArrDesc);
  lTrimDep = Trim(xiDepDesc);
  strncpy(mArrDesc, lTrimArr, 20);
  strncpy(mDepDesc, lTrimDep, 20);
  mArrDesc[20] = 0;
  mDepDesc[20] = 0;

  /**************************************************************************/
  /* Tidy up the comment.                                                   */
  /**************************************************************************/
  if (xiComment != NULL)
  {
    lTrimCmt = Trim(xiComment);
    strncpy(mComment, lTrimCmt, TT_CMT_LEN);
    mComment[TT_CMT_LEN - 1] = 0;
  }
  else
  {
    mComment[0] = 0;
  }

  mArrPnt          = xiValue[0];
  mArrTime         = xiValue[1];
  mRelTime         = xiValue[2];
  mDepTime         = xiValue[3];
  mArrCode         = xiValue[4];
  mDepCode         = xiValue[5];
  mStatus          = xiValue[6];
  mNextTimeTabPos  = xiValue[8];
  mTwinAssoc       = 0;

  mLoco[0]         = 0;
  mLoco[1]         = 0;
  mRoute           = 0;
  mMinLate         = 0;
  mInitialLocoPlat = 0;
  mDelayFlag       = 0;

  if ((mStatus == ST_INPLAT) || (mStatus == ST_STOCKOK))
  {
    /************************************************************************/
    /* Train is in the station at the start of the simulation.  The Extra1  */
    /* field (GValue[7]) gives us the platform this train is in.            */
    /************************************************************************/
    mInitialLocoPlat = xiValue[7];
  }
  else if (mStatus == ST_TWINASSOC)
  {
    /************************************************************************/
    /* Train is the first part of a twin set.  We'll get the delay/platform */
    /* information from the second part of the set.                         */
    /************************************************************************/
    mTwinAssoc = xiValue[7];
  }
  else if (mStatus == 0)
  {
    /************************************************************************/
    /* Train is not in the station at the start of the simulation.  The     */
    /* Extra1 field (GValue[7]) gives us the delay flag to be applied to    */
    /* this train.                                                          */
    /************************************************************************/
    if (xiValue[7] == DLF_NODELAYS)
    {
      mMinLate = 0;
    }
    else
    {
      // Calculate +/- arrival time (minutes late/early)
      laa = random(100);
      mMinLate = 0;
      if (laa == 0)                    mMinLate = -12;
      if (laa == 1)                    mMinLate = -11;
      if (laa == 2)                    mMinLate = -10;
      if ((laa >= 3) && (laa <= 4))    mMinLate = -9;
      if ((laa >= 5) && (laa <= 6))    mMinLate = -8;
      if ((laa >= 7) && (laa <= 8))    mMinLate = -7;
      if ((laa >= 9) && (laa <= 10))   mMinLate = -6;
      if ((laa >= 11) && (laa <= 12))  mMinLate = -5;
      if ((laa >= 13) && (laa <= 16))  mMinLate = -4;
      if ((laa >= 17) && (laa <= 20))  mMinLate = -3;
      if ((laa >= 21) && (laa <= 29))  mMinLate = -2;
      if ((laa >= 30) && (laa <= 39))  mMinLate = -1;
      if ((laa >= 40) && (laa <= 59))  mMinLate = 0;
      if ((laa >= 60) && (laa <= 69))  mMinLate = 1;
      if ((laa >= 70) && (laa <= 78))  mMinLate = 2;
      if ((laa >= 79) && (laa <= 82))  mMinLate = 3;
      if ((laa >= 83) && (laa <= 86))  mMinLate = 4;
      if ((laa >= 87) && (laa <= 88))  mMinLate = 5;
      if ((laa >= 89) && (laa <= 90))  mMinLate = 6;
      if ((laa >= 91) && (laa <= 92))  mMinLate = 7;
      if ((laa >= 93) && (laa <= 94))  mMinLate = 8;
      if ((laa >= 96) && (laa <= 96))  mMinLate = 9;
      if (laa == 97)                   mMinLate = 10;
      if (laa == 98)                   mMinLate = 11;
      if (laa == 99)                   mMinLate = 12;

      /**********************************************************************/
      /* If trains aren't allowed to be early then force any early trains   */
      /* to be ontime.                                                      */
      /**********************************************************************/
      if (xiValue[7] == DLF_ONLYLATE)
      {
        if (mMinLate < 0)
        {
          mMinLate   = 0;
        }

        xiValue[7] = DLF_DELAYS;
      }

      if (xiValue[7] == DLF_ONLYLATE_THRU)
      {
        if (mMinLate < 0)
        {
          mMinLate   = 0;
        }

        xiValue[7] = DLF_DELAYS_THRU;
      }
    }

    mDelayFlag = xiValue[7];
  }
  else
  {
    TRC_ERR((TB, 
             "Invalid status value (%d) for train (ArrTime:%d ArrDesc:'%s')", 
             mStatus,
             mArrTime,
             mArrDesc));
  }

  return;
}


TTimetable::~TTimetable()
{
}


void TTimetable::GetArrDesc(char* Text)
{
  // Copy arrival description to <Text>
  strcpy(Text, mArrDesc);
}


void TTimetable::GetDepDesc(char* Text)
{
  // Copy departure description to <Text>
  strcpy(Text, mDepDesc);
}


void TTimetable::GetComment(char* Text)
{
  // Copy comment description to <Text>
  strcpy(Text, mComment);
}


int TTimetable::GetArrPnt()
{
  // Return the arrival point
  return mArrPnt;
}


int TTimetable::GetArrTime()
{
  // Return the arrival time
  return mArrTime;
}


int TTimetable::GetRelTime()
{
  // Return the stock release time
  return mRelTime;
}


int TTimetable::GetDepTime()
{
  // Return the departure time
  return mDepTime;
}


int TTimetable::GetArrCode()
{
  // Return the arrival code
  return mArrCode;
}


int TTimetable::GetDepCode()
{
  // Return the departure code
  return mDepCode;
}


int TTimetable::GetMinLate()
{
  // Return the minutes late value
  return mMinLate;
}


int TTimetable::GetLoco(int xiIndex)
{
  // Return the loco value
  return mLoco[xiIndex];
}


int TTimetable::GetLoco()
{
  // Return the loco value
  return mLoco[0];
}


int TTimetable::GetStatus()
{
  // Return loco status flag
  return mStatus;
}


int TTimetable::GetRoute()
{
  // Return the route number
  return mRoute;
}


int TTimetable::GetDelayFlag()
{
  // Return delay flag
  return mDelayFlag;
}


int TTimetable::GetInitialLocoPlat()
{
  // Return the initial loco platform
  return mInitialLocoPlat;
}


int TTimetable::GetNextTimeTabPos()
{
  // Return the timetable position of the train which inherits this loco
  return mNextTimeTabPos;
}

int TTimetable::GetTwinAssoc()
{
  // Return the associated twin timetable entry for this train which describes how this train arrived.
  return mTwinAssoc;
}

int TTimetable::SetRelTime(int NewTime)
{
  int Temp;

  // Set the new time and return the old time value
  Temp = mRelTime;
  mRelTime = NewTime;
  return Temp;
}


int TTimetable::SetArrPnt(int NewPnt)
{
  int Temp;

  // Set the new arrival point and return the old one
  Temp = mArrPnt;
  mArrPnt = NewPnt;
  return Temp;
}


int TTimetable::SetArrCode(int NewCode)
{
  int Temp;

  // Set the new code and return the old code value
  Temp = mArrCode;
  mArrCode = NewCode;
  return Temp;
}


int TTimetable::SetMinLate(int NewMinLate)
{
  int Temp;

  // Set the new value of <MinLate> and return the old value
  Temp = mMinLate;
  mMinLate = NewMinLate;
  return Temp;
}


int TTimetable::SetLoco(int xiIndex, int NewNumber)
{
   int Temp;

   // Set the new code and return the old value
   Temp = mLoco[xiIndex];
   mLoco[xiIndex] = NewNumber;
   return Temp;
}


int TTimetable::SetStatus(int NewStatus)
{
  int Temp;

  // Set the new status flag and return the old one
  Temp = mStatus;
  mStatus = NewStatus;
  return Temp;
}


int TTimetable::SetRoute(int NewRoute)
{
  int Temp;

  // Set the new route code and return the old one
  Temp = mRoute;
  mRoute = NewRoute;
  return Temp;
}


int TTimetable::SetInitialLocoPlat(int xiPlat)
{
  int Temp;

  // Set the new flag and return the old one
  Temp = mInitialLocoPlat;
  mInitialLocoPlat = xiPlat;
  return Temp;
}


int  TTimetable::AddMinLate(int NewMinLate)
{
  int Temp;

  // Add the extra delay to MinLate
  Temp = mMinLate;
  mMinLate += NewMinLate;
  return Temp;
}




