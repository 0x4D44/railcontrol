/* SECTION.H
*  =========
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

#if !defined(_SECTION_H_)
#define _SECTION_H_


// TSection class
class TSection;
typedef TSection* PSection;

class TSection
{
private:
  POINT  SetPoint[4];              // Shape in general co-ordinates
  int    SectNumber;               // What is the section number?
  BOOL   Occupied;                 // Is the section occupied?
  static float  XScaleFactor;      // X scaling factor
  static float  YScaleFactor;      // Y scaling factor

public:
  TSection(int Number, int X[4], int Y[4]);
  ~TSection();
  static float SetXScaleFactor(float NewFactor);
  static float SetYScaleFactor(float NewFactor);
  void  GetSection(POINT ThePoints[4]);
  BOOL  IsOccupied();
  BOOL  SetOccupied(BOOL NewState);
};


#endif // _SECTION_H_