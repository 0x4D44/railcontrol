/* OVLPDATA.H
*  ==========
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

#if !defined(_OVLPDATA_H_)
#define _OVLPDATA_H_


// TOverlapData class
class TOverlapData;
typedef TOverlapData* POverlapData;

class TOverlapData
{
private:
  int    mSections[2];             // Overlapping section references

public:
  TOverlapData(int xiSections[2]);
  ~TOverlapData();

  int   GetSection(int xiIndex);
};


#endif // _OVLPDATA_H_

