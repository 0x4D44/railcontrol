/* UIHELPERS.H
 *  Shared UI drawing helpers for Win32 surfaces.
 */
#ifndef UIHELPERS_H_INCLUDED
#define UIHELPERS_H_INCLUDED

#include "handleguard.h"

inline void DrawRaisedPanel(HDC dc,
                            const RECT& bounds,
                            int headerY,
                            int edgeInset = 4,
                            int leftInset = 3)
{
  TPenGuard highlightPen(CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT)));
  HPEN oldPen = (HPEN) SelectObject(dc, highlightPen.Get());
  MoveToEx(dc, bounds.right - edgeInset, headerY, nullptr);
  LineTo(dc, bounds.right - edgeInset, bounds.bottom - edgeInset);
  LineTo(dc, leftInset, bounds.bottom - edgeInset);
  SelectObject(dc, oldPen);

  TPenGuard shadowPen(CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW)));
  oldPen = (HPEN) SelectObject(dc, shadowPen.Get());
  LineTo(dc, leftInset, headerY);
  LineTo(dc, bounds.right - edgeInset, headerY);
  SelectObject(dc, oldPen);
}

inline void FillRectOpaque(HDC dc, const RECT& rect, COLORREF colour)
{
  const COLORREF previous = SetBkColor(dc, colour);
  ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rect, nullptr, 0, nullptr);
  SetBkColor(dc, previous);
}

struct HeaderLabel
{
  int x = 0;
  const char* text = nullptr;
};

inline void DrawHeaderLabels(HDC dc,
                             HFONT font,
                             COLORREF textColor,
                             COLORREF backColor,
                             int y,
                             const HeaderLabel* labels,
                             size_t count)
{
  if (!labels || count == 0)
  {
    return;
  }

  HFONT oldFont = (HFONT) SelectObject(dc, font);
  const COLORREF prevText = SetTextColor(dc, textColor);
  const COLORREF prevBack = SetBkColor(dc, backColor);

  for (size_t idx = 0; idx < count; ++idx)
  {
    const HeaderLabel& label = labels[idx];
    if (!label.text)
    {
      continue;
    }
    TextOut(dc, label.x, y, label.text, static_cast<int>(lstrlenA(label.text)));
  }

  SetTextColor(dc, prevText);
  SetBkColor(dc, prevBack);
  SelectObject(dc, oldFont);
}

#endif // UIHELPERS_H_INCLUDED
