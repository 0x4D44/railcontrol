/* ROUTES.H
*  ========
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

#if !defined(_ROUTES_H_)
#define _ROUTES_H_


// TRoutes class
class TRoutes;
typedef TRoutes* PRoutes;

class TRoutes
{
private:
  long  Clear[6];      // Clearing order
  int   From, To;      // From and to descriptors

public:
  TRoutes(int GFrom, int GTo, long ClearBy[6]);
  ~TRoutes();
  int  GetFrom();
  int  GetTo();
  BOOL GetClear(int ToClear, int &Num1, int &Num2);
};


#endif // _ROUTES_H_

