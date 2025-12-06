/* LOCOS.H
*  =======
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

#if !defined(_LOCOS_H_)
#define _LOCOS_H_

#include <cstddef>


// TLocos class
class TLocos;
typedef TLocos* PLocos;

class TLocos
{
private:
  int  Class;                    // What class is the locomotive?
  int  Number;                   // What is its number (ie 312, 004 etc)
  int  Flag;                     // What flag has it currently have? (see GENERAL.H)
  int  Type;                     // What code (SC_XXXX) is the loco?
  int  RefuelTime;               // When will it be finished refueling?

public:
  TLocos(int GClass, int GNumber, int GType);
  ~TLocos();
  int  GetFlag();
  int  SetFlag(int NewFlag);
  int  GetType();
  int  GetRefuelTime();
  int  SetRefuelTime(int NewTime);
  void LocoNumStr(char* Text, size_t BufferSize);
};


#endif // _LOCOS_H_

