#pragma once

#include <owl/static.h>

class TLedClock : public owl::TStatic
{
public:
  enum ClockDisplayEnum { Display24Hour, Display12Hour };

  TLedClock(TWindow* parent, int id, int x, int y, int w, int h, owl::TModule* = 0);
  TLedClock(TWindow* parent, int resourceId, owl::TModule* = 0);

  ~TLedClock();

  int GetXMargin() const { return xMargin; }
  void SetXMargin(int x) { xMargin = x; }

  int GetYMargin() const { return yMargin; }
  void SetYMargin(int y) { yMargin = y; }

  owl::TColor GetTextColor() const { return textColor; }
  void SetTextColor(const owl::TColor& color) { textColor = color; }

  ClockDisplayEnum GetClockDisplay() const { return clockDisplay; }
  void SetClockDisplay(ClockDisplayEnum val) { clockDisplay = val; }

  bool GetDisplayHourLeadingZero() const { return displayHourLeadingZero;  }
  void SetDisplayHourLeadingZero(bool val) { displayHourLeadingZero = val; }

  bool GetDisplaySeconds() const { return displaySeconds; }
  void SetDisplaySeconds(bool val) { displaySeconds = val; }

protected:
  void ODADrawEntire(DRAWITEMSTRUCT& drawInfo);

  void DisplayDigit(owl::TDC& dc, int number);
  void DisplayTwoDigits(owl::TDC& dc, int number, bool displayLeadingZero);
  void DisplayColon(owl::TDC& dc);
  void DisplayTime(owl::TDC& dc);

private:
  static owl::TPoint segmentPoints[7][6];
  static bool digitSegments[10][7];

  void Init();

  int xMargin, yMargin;
  owl::TColor textColor;

  ClockDisplayEnum clockDisplay;
  bool displayHourLeadingZero;
  bool displaySeconds;
  
};

