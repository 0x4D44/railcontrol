#include <owl/pch.h>

#include "Editor.h"
#include "App.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TPlainEditor, TEditView)
  EV_WM_GETMINMAXINFO,
END_RESPONSE_TABLE;

TPlainEditor::TPlainEditor(TDocument& doc, TWindow* parent)
  : TEditView(doc, parent)
{}

//
// Paint routine for Window, Printer, and PrintPreview for a TEditView client.
// Only paint if we're printing and we have something to paint, otherwise do nothing.
//
void TPlainEditor::Paint(TDC& dc, bool, TRect& rect)
{
  TRichEditorApp* a = dynamic_cast<TRichEditorApp*>(GetApplication()); CHECK(a);
  if (!a->Printing || !a->Printer || rect.IsEmpty()) return;

  TSize pageSize(rect.right - rect.left, rect.bottom - rect.top);
  HFONT hFont = (HFONT)GetWindowFont();
  TFont font(_T("Arial"), -12);
  if (!hFont)
    dc.SelectObject(font);
  else
    dc.SelectObject(TFont(hFont));
  TEXTMETRIC tm = dc.GetTextMetrics();
  int fHeight = tm.tmHeight + tm.tmExternalLeading;

  // Calculate how many lines of this font can we fit on a page.
  //
  int linesPerPage = MulDiv(pageSize.cy, 1, fHeight);
  if (linesPerPage == 0) return;

  // Compute the number of pages to print.
  //
  TPrintDialog::TData& printerData = a->Printer->GetSetup();
  int maxPg = ((GetNumLines() / linesPerPage) + 1.0);
  printerData.MinPage = 1;
  printerData.MaxPage = maxPg;

  // Do the text stuff.
  //
  int fromPage = printerData.FromPage == -1 ? 1 : printerData.FromPage;
  int toPage = printerData.ToPage == -1 ? 1 : printerData.ToPage;
  int currentPage = fromPage;
  const int bufSize = 256;
  TAPointer<_TCHAR> buffer(new _TCHAR[bufSize]);
  while (currentPage <= toPage)
  {
    int startLine = (currentPage - 1) * linesPerPage;
    int lineIdx = 0;
    while (lineIdx < linesPerPage)
    {
      // If the string is no longer valid then there's nothing more to display.
      //
      if (!GetLine(buffer, bufSize - 1, startLine + lineIdx))
        break;
      dc.TabbedTextOut(TPoint(0, lineIdx * fHeight), &buffer[0], -1, 0, 0, 0);
      lineIdx++;
    }
    currentPage++;
  }
}

void TPlainEditor::EvGetMinMaxInfo(MINMAXINFO& minmaxinfo)
{
  TRichEditorApp* a = dynamic_cast<TRichEditorApp*>(GetApplication()); CHECK(a);
  if (a->Printing)
  {
    minmaxinfo.ptMaxSize = TPoint(32000, 32000);
    minmaxinfo.ptMaxTrackSize = TPoint(32000, 32000);
    return;
  }
  TEditView::EvGetMinMaxInfo(minmaxinfo);
}

//------------------------------------------------------------------------------
// TRichEditor 

DEFINE_RESPONSE_TABLE1(TRichEditor, TRichEditView)
  EV_WM_GETMINMAXINFO,
END_RESPONSE_TABLE;

TRichEditor::TRichEditor(TDocument& doc, TWindow* parent)
  : TRichEditView(doc, parent)
{}

void TRichEditor::EvGetMinMaxInfo(MINMAXINFO& minmaxinfo)
{
  TRichEditorApp* a = dynamic_cast<TRichEditorApp*>(GetApplication()); CHECK(a);
  if (a->Printing)
  {
    minmaxinfo.ptMaxSize = TPoint(32000, 32000);
    minmaxinfo.ptMaxTrackSize = TPoint(32000, 32000);
    return;
  }
  TRichEditView::EvGetMinMaxInfo(minmaxinfo);
}
