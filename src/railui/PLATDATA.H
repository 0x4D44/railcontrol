/* PLATDATA.H
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
*  Version  : 1.0
*  Language : C++ (BORLAND v3.1)
*
*/

#if !defined(_PLATDATA_H_)
#define _PLATDATA_H_


// TPlatData class
class TPlatData;
typedef TPlatData* PPlatData;

class TPlatData
{
private:
  POINT  SetPoint[4];              // Shape in general co-ordinates
  int    Timetable[2];             // Timetable information
  int    Loco;                     // Loco information
  int    SelectorRef;              // Reference to selector
  BOOL   Redraw;                   // Platform window needs redrawn
  static float  XScaleFactor;      // X scaling factor
  static float  YScaleFactor;      // Y scaling factor


public:
  TPlatData(int X[4], int Y[4]);
  ~TPlatData();
  static float SetXScaleFactor(float NewFactor);
  static float SetYScaleFactor(float NewFactor);
  void  GetSection(POINT ThePoints[4]);
  int   GetTimetable(int xiIndex);
  int   GetTimetable();
  int   SetTimetable(int xiIndex, int NewNumber);
  int   SetTimetable(int NewNumber);
  int   GetLoco();
  int   SetLoco(int NewNumber);
  int   SetSelectorRef(int NewNumber);
  BOOL  SetRedraw(BOOL Flag);
  int   GetSelectorRef();
  BOOL  GetRedraw();
};


#endif // _PLATDATA_H_

