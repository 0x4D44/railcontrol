// Based on https://www.wincodebits.in/2015/05/visual-c-digital-clock.html?587203&587203
// Converted to OWLNext class by Jogy

#include <owl/pch.h>
#pragma hdrstop

#include "ledclock.h"

const int DIGIT_WIDTH = 42;
const int DIGIT_HEIGHT = 72;
const int COLON_WIDTH = 12;

using namespace owl;

TPoint TLedClock::segmentPoints[7][6] = {
  TPoint(7, 6), TPoint(11, 2), TPoint(31, 2), TPoint(35, 6), TPoint(31, 10), TPoint(11, 10),
  TPoint(6, 7), TPoint(10, 11), TPoint(10, 31), TPoint(6, 35), TPoint(2, 31), TPoint(2, 11),
  TPoint(36, 7), TPoint(40, 11), TPoint(40, 31), TPoint(36, 35), TPoint(32, 31), TPoint(32, 11),
  TPoint(7, 36), TPoint(11, 32), TPoint(31, 32), TPoint(35, 36), TPoint(31, 40), TPoint(11, 40),
  TPoint(6, 37), TPoint(10, 41), TPoint(10, 61), TPoint(6, 65), TPoint(2, 61), TPoint(2, 41),
  TPoint(36, 37), TPoint(40, 41), TPoint(40, 61), TPoint(36, 65), TPoint(32, 61), TPoint(32, 41),
  TPoint(7, 66), TPoint(11, 62), TPoint(31, 62), TPoint(35, 66), TPoint(31, 70), TPoint(11, 70)
};

bool TLedClock::digitSegments[10][7] = {
    1, 1, 1, 0, 1, 1, 1, // 0
    0, 0, 1, 0, 0, 1, 0, // 1
    1, 0, 1, 1, 1, 0, 1, // 2
    1, 0, 1, 1, 0, 1, 1, // 3
    0, 1, 1, 1, 0, 1, 0, // 4
    1, 1, 0, 1, 0, 1, 1, // 5
    1, 1, 0, 1, 1, 1, 1, // 6
    1, 0, 1, 0, 0, 1, 0, // 7
    1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 0, 1, 1 }; // 9


TLedClock::TLedClock(TWindow* parent, int id, int x, int y, int w, int h, TModule* module)
  : TStatic(parent, id, NULL, x, y, w, h, 0, module)
{
  Attr.Style |= SS_OWNERDRAW;

  Init();
}


TLedClock::TLedClock(TWindow* parent, int resourceId, TModule* module)
  : TStatic(parent, resourceId, 0, module)
{
  Init();
}


TLedClock::~TLedClock()
{
}

void TLedClock::Init()
{
  xMargin = 0;
  yMargin = 0;
  textColor = TColor::LtGreen;

  TCHAR szBuffer[2];
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
  clockDisplay = (szBuffer[0] == '1') ? Display24Hour : Display12Hour;
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
  displayHourLeadingZero = (szBuffer[0] == '1');

  displaySeconds = true;
}

void TLedClock::ODADrawEntire(DRAWITEMSTRUCT& drawInfo)
{
  int cxClient = drawInfo.rcItem.right - drawInfo.rcItem.left - xMargin * 2;
  int cyClient = drawInfo.rcItem.bottom - drawInfo.rcItem.top - yMargin * 2;

  if (cxClient <= 0 || cyClient <= 0)
    return;

  int canvasWidth = displaySeconds ? (6 * DIGIT_WIDTH + 2 * COLON_WIDTH) : (4 * DIGIT_WIDTH + COLON_WIDTH);
  int canvasHeight = DIGIT_HEIGHT;

  double ratio = ((double)cxClient / (double)cyClient) / ((double)canvasWidth / (double)canvasHeight);

  if (ratio > 1.0)
  {
    cxClient = static_cast<int>(cxClient / ratio);
  }
  else if (ratio < 1.0)
  {
    cyClient = static_cast<int>(cyClient * ratio);
  }

  TDC dc(drawInfo.hDC);

  //dc.FillSolidRect(TRect(0, 0, cxClient, cyClient), BkgndColor);
  dc.FillSolidRect(drawInfo.rcItem, GetBkgndColor());
  //dc.Rectangle(drawInfo.rcItem);

  dc.SetMapMode(MM_ISOTROPIC);
  dc.SetWindowExt(TPoint(canvasWidth, canvasHeight));
  dc.SetViewportExt(TPoint(cxClient, cyClient));
  dc.SetWindowOrg(TPoint(canvasWidth / 2, canvasHeight / 2));
  dc.SetViewportOrg(TPoint(cxClient / 2 + xMargin, cyClient / 2 + yMargin));
  //dc.Rectangle(TRect(0, 0, canvasWidth, canvasHeight));
  dc.SelectStockObject(NULL_PEN);
  dc.SelectObject(TBrush(textColor));

  DisplayTime(dc);
}

void TLedClock::DisplayDigit(TDC& dc, int number)
{
  int seg;
  for (seg = 0; seg < 7; seg++)
    if (digitSegments[number][seg])
      dc.Polygon(segmentPoints[seg], 6);
}
void TLedClock::DisplayTwoDigits(TDC& dc, int number, bool displayLeadingZero)
{
  if (displayLeadingZero || (number / 10 != 0))
    DisplayDigit(dc, number / 10);
  dc.OffsetWindowOrg(TPoint(-DIGIT_WIDTH, 0), NULL);
  DisplayDigit(dc, number % 10);
  dc.OffsetWindowOrg(TPoint(-DIGIT_WIDTH, 0), NULL);
}
void TLedClock::DisplayColon(TDC& dc)
{
  TPoint ptColon[2][4] = { 
    TPoint(2, 21), TPoint(6, 17), TPoint(10, 21), TPoint(6, 25),
    TPoint(2, 51), TPoint(6, 47), TPoint(10, 51), TPoint(6, 55)
  };
  dc.Polygon(ptColon[0], 4);
  dc.Polygon(ptColon[1], 4);
  dc.OffsetWindowOrg(TPoint(-COLON_WIDTH, 0), NULL);
}
void TLedClock::DisplayTime(TDC& dc)
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  if (clockDisplay == Display24Hour)
    DisplayTwoDigits(dc, st.wHour, displayHourLeadingZero);
  else
    DisplayTwoDigits(dc, (st.wHour %= 12) ? st.wHour : 12, displayHourLeadingZero);

  DisplayColon(dc);
  DisplayTwoDigits(dc, st.wMinute, true);

  if (displaySeconds)
  {
    DisplayColon(dc);
    DisplayTwoDigits(dc, st.wSecond, true);
  }
}
