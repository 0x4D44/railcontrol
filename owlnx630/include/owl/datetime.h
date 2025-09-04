//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of class TDateTimePicker.
//----------------------------------------------------------------------------

#if !defined(OWL_DATETIME_H)
#define OWL_DATETIME_H

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

class _OWLCLASS TDateTimePickerData{
  public:
    TDateTimePickerData(const TSystemTime& dt   = TSystemTime::LocalTime(),
                        const TSystemTime& mind = TSystemTime(1980,0,0),
                        const TSystemTime& maxd = TSystemTime(2100,0,0))
                        : DateTime(dt),MaxDate(maxd),MinDate(mind)
      { }
    TSystemTime  DateTime;
    TSystemTime MaxDate; //default TSystemTime(0) not set
    TSystemTime MinDate; //default TSystemTime(0) not set
};

//
// class TDateTimePicker
// ~~~~~ ~~~~~~~~~~~~~~~
//
class _OWLCLASS TDateTimePicker : public TControl{
  // Constructors
  public:
    enum TDateTimeColors{
        dtBackColor=MCSC_BACKGROUND,
        dtTextColor=MCSC_TEXT,
        dtTitleBackColor=MCSC_TITLEBK,
        dtTitleTextColor=MCSC_TITLETEXT,
        dtMonthBackColor=MCSC_MONTHBK,
        dtTrailingTextColor=MCSC_TRAILINGTEXT,
    };

    TDateTimePicker(TWindow*   parent,
                    int       id,
                    int x, int y, int w, int h,
                    TModule*   module = 0);

    TDateTimePicker(TWindow*   parent,
                    int       resourceId,
                    TModule*   module = 0);
    ~TDateTimePicker();

    void      SetColor(TDateTimeColors index, const TColor& clr);
    TColor  GetColor(TDateTimeColors index) const;

    bool     SetFormat(LPCTSTR format);
    HFONT   GetFont() const;
    void     SetFont(HFONT font, bool redraw = true);

    bool     SetTime(const TSystemTime& dt);
    bool     SetNoTime();
    int     GetTime(TSystemTime& dt) const;

    bool     SetRange(const TSystemTime& mint, const TSystemTime& maxt);
    void     GetRange(TSystemTime& mint, TSystemTime& maxt) const;

    HWND    GetMonthCalCtrl();

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
    TDateTimePicker(const TDateTimePicker&);
    TDateTimePicker& operator =(const TDateTimePicker&);

//!YB Need to be consistent - Common Controls are not streamable for the
//!YB the most part. This guy only invokes TControl's version anyway.
//!YB
//!YB DECLARE_STREAMABLE_FROM_BASE(_OWLCLASS, __OWL TDateTimePicker, 1);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// inlines
//

//
inline void TDateTimePicker::SetColor(TDateTimeColors index, const TColor& clr)
{
  PRECONDITION(GetHandle());
  SendMessage(DTM_SETMCCOLOR, TParam1(index), TParam2((COLORREF)clr));
}

//
inline TColor TDateTimePicker::GetColor(TDateTimeColors index) const
{
  PRECONDITION(GetHandle());
   return TColor(::SendMessage(GetHandle(), DTM_GETMCCOLOR, TParam1(index),0));
}

//
inline bool TDateTimePicker::SetFormat(LPCTSTR format)
{
  PRECONDITION(GetHandle());
  return (bool)SendMessage(DTM_SETFORMAT, 0, (TParam2)format);
}

//
inline HFONT TDateTimePicker::GetFont() const 
{
  PRECONDITION(GetHandle());
  return (HFONT)::SendMessage(GetHandle(), DTM_GETMCFONT,0,0);
}

//
inline void TDateTimePicker::SetFont(HFONT font, bool redraw)
{
  PRECONDITION(GetHandle());
  SendMessage(DTM_SETMCFONT, (TParam1)font, MkUint32((uint16)redraw, 0));
}

//
inline HWND TDateTimePicker::GetMonthCalCtrl()
{
  PRECONDITION(GetHandle());
  return (HWND)SendMessage(DTM_GETMONTHCAL, 0, 0);
}

//
inline bool TDateTimePicker::SetTime(const TSystemTime& dt)
{
  PRECONDITION(GetHandle());
  return (bool)SendMessage(DTM_SETSYSTEMTIME, (TParam1)GDT_VALID, (TParam2)&dt);
}

//
inline bool TDateTimePicker::SetNoTime()
{
  PRECONDITION(GetHandle());
  return (bool)SendMessage(DTM_SETSYSTEMTIME, (TParam1)GDT_NONE, 0);
}

// return GDT_VALID,GDT_NONE,GDT_ERROR
inline int TDateTimePicker::GetTime(TSystemTime& dt) const
{
  PRECONDITION(GetHandle());
  return (int)::SendMessage(GetHandle(), DTM_GETSYSTEMTIME, 0, (TParam2)&dt);
}

__OWL_END_NAMESPACE

#endif  // OWL_DATETIME_H
