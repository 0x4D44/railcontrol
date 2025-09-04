//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of class TIPAddress.
//----------------------------------------------------------------------------

#if !defined(OWL_IPADDRES_H)
#define OWL_IPADDRES_H

#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TIPAddressBits
// ~~~~~ ~~~~~~~~~~~~~~
//
// Also used as transfer buffer
//
class _OWLCLASS TIPAddressBits {
  public:
    TIPAddressBits():Address(0){}
    TIPAddressBits(int fld0, int fld1, int fld2, int fld3)
             : Address(MakeIPAddress(fld0,fld1,fld2,fld3)){}

    static uint32 MakeIPAddress(int f0, int f1, int f2, int f3)
     {  return ((uint32)(((uint32)(f0)<<24)+((uint32)(f1)<<16)+((uint32)(f2)<<8)+((uint32)(f3)))); }

    void SetAddress(int f0, int f1, int f2, int f3)    { Address = MakeIPAddress(f0,f1,f2,f3);  }

    int GetFirst()    { return ((Address>>24)& 0xff);  }
    int GetSecond()    { return ((Address>>16) & 0xff); }
    int GetThird()    { return ((Address>>8) & 0xff);  }
    int GetFourth()   { return (Address & 0xff);       }

    operator uint32() { return Address;}

    uint32  Address;
};


//
// class TIPAddress
// ~~~~~ ~~~~~~~~~~
//
class _OWLCLASS TIPAddress : public TControl {
  public:
    enum TIPFields{
      ifFirst,
      ifSecond,
      ifThird,
      ifForth,
    };

    TIPAddress (TWindow*   parent,
                int       id,
                int x, int y, int w, int h,
                TModule*   module = 0);

    TIPAddress (TWindow*   parent,
                 int       resourceId,
                TModule*   module = 0);
    ~TIPAddress();


    void   Clear();

    void   SetAddress(const TIPAddressBits& addres);
    int   GetAddress(TIPAddressBits& addres) const;

    bool  IsBlank() const;
    void  SetFocus(const TIPFields& fld);

    bool  SetRange(const TIPFields& fld, int minv, int maxv);

    // Override TWindow virtual member functions
    //
    uint   Transfer(void* buffer, TTransferDirection direction);

  protected:
    // Override TWindow virtual member functions
    //
    LPCTSTR GetClassName();

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TIPAddress(const TIPAddress&);
    TIPAddress& operator =(const TIPAddress&);

//!YB Need to be consistent - Common Controls are not streamable for the
//!YB the most part. This guy only invokes TControl's version anyway.
//!YB
//!YB DECLARE_STREAMABLE_FROM_BASE(_OWLCLASS, __OWL TIPAddress, 1);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// inlines
//

//
inline void TIPAddress::Clear(){
  PRECONDITION(GetHandle());
  SendMessage(IPM_CLEARADDRESS, 0, 0);
}

//
inline void TIPAddress::SetAddress(const TIPAddressBits& address){
  PRECONDITION(GetHandle());
  SendMessage(IPM_SETADDRESS, 0, TParam2((uint32)(TIPAddressBits&)address));
}

//
inline int TIPAddress::GetAddress(TIPAddressBits& address) const{
  PRECONDITION(GetHandle());
  return (int)((TIPAddress*)this)->SendMessage(IPM_GETADDRESS, 0, TParam2(&address.Address));
}

//
inline bool TIPAddress::IsBlank() const{
  PRECONDITION(GetHandle());
  return ToBool(((TIPAddress*)this)->SendMessage(IPM_ISBLANK, 0, 0));
}

//
inline void TIPAddress::SetFocus(const TIPFields& fld){
  PRECONDITION(GetHandle());
  SendMessage(IPM_SETFOCUS, TParam1(fld), 0);
}

//
inline bool TIPAddress::SetRange(const TIPFields& fld, int minv, int maxv){
  PRECONDITION(GetHandle());
  return ToBool(SendMessage(IPM_SETRANGE, TParam1(fld), TParam2(MAKEIPRANGE(minv,maxv))));
}


__OWL_END_NAMESPACE

#endif  // OWL_MONTHCAL_H
