//----------------------------------------------------------------------------
// ObjectWindows, OWL NExt
// Created by Kenneth Haley ( khaley@bigfoot.com ) 
//
//$Revision: 1.25 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:20 $
//
// GNU suport functions
//
//----------------------------------------------------------------------------
#include <stdio.h>

//FMM
#if defined(__HPUX_SOURCE)
#include <owl/private/defs.h>
#endif

#include <windows.h>

static const char c_list[]="0123456789abcdefghijklmnopqrstuvwxyz";

char* my_itoa(int val, char *str, int radix)
{
  if(radix==10)
  {
    sprintf(str,"%d",val);
  }
  char tmp[33];
  int cnt=0,x=0;
  while(cnt<33)
  {
    if(val<radix)
    {
      str[cnt]=tmp[cnt];
      break;
    }
    str[cnt]=tmp[val%radix];
    val/=radix;
    ++cnt;
  }
  tmp[++cnt]=0;
  for(;cnt!=-1;cnt--,x++)
    str[x]=tmp[cnt];
  return str;
}

char* myfullpath(char* buf,const char* path,int buflen)
{
  char* x;
  if(!GetFullPathName(buf,buflen,const_cast<char*>(path),&x))
    return 0;
  return buf;
}

/*===========================================================*/

