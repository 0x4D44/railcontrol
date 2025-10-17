/* TIMETABL.H
*  ==========
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

#if !defined(_TIMETABL_H_)
#define _TIMETABL_H_

#define TT_CMT_LEN  200

// TTimetable class
class TTimetable;
typedef TTimetable* PTimetable;

class TTimetable
{
private:
  char  mArrDesc[25];           // Timetable arrival description
  char  mDepDesc[25];           // Timetable departure description
  char  mComment[TT_CMT_LEN];   // Comment
  int   mArrPnt;                // Arrival point on route diagram
  int   mArrTime,               // Arrival time hour/minute
        mRelTime,               // Release time hour/minute
        mDepTime;               // Departure time hour/minute
  int   mArrCode,               // Arrival stock code
        mDepCode;               // Departure stock code
  int   mMinLate;               // Minutes late
  int   mLoco[2];               // Loco number fields
  int   mStatus;                // Is the train due, approaching, held, arriving, waiting, dep'ing etc
  int   mRoute;                 // Which route is the train associated with?
  int   mDelayFlag;             // Loco delay flag (ie DLF_XXX)
  int   mInitialLocoPlat;       // Initial loco platform (for trains in the station at the start of the simulation)
  int   mNextTimeTabPos;        // The timetable number of the train which inherits this loco.
  int   mTwinAssoc;             // Index of the associated twin timetable entry (if applicable).


public:
  TTimetable(const char* xiArrDesc, const char* xiDepDesc, int xiValue[9], const char* xiComment);
  ~TTimetable();
  void  GetArrDesc(char* Text);
  void  GetDepDesc(char* Text);
  void  GetComment(char* Text);
  int   GetArrPnt();
  int   GetArrTime();
  int   GetRelTime();
  int   GetDepTime();
  int   GetArrCode();
  int   GetDepCode();
  int   GetMinLate();
  int   GetLoco();
  int   GetLoco(int xiIndex);
  int   GetStatus();
  int   GetRoute();
  int   GetDelayFlag();
  int   GetInitialLocoPlat();
  int   GetNextTimeTabPos();
  int   GetTwinAssoc();
  int   SetRelTime(int NewTime);
  int   SetArrPnt(int NewPnt);
  int   SetArrCode(int NewCode);
  int   SetMinLate(int NewMinLate);
  int   SetLoco(int xiIndex, int NewNumber);
  int   SetStatus(int NewStatus);
  int   SetRoute(int NewRoute);
  int   SetInitialLocoPlat(int xiPlat);
  int   AddMinLate(int NewMinLate);
};


#endif // _TIMETABL_H_

