#ifndef TEXTPRINTOUT_H
#define TEXTPRINTOUT_H

#include <owl/printer.h>

class TTextPrintout
  : public owl::TPrintout
{
public:

  TTextPrintout(owl::TPrinter* printer, LPCTSTR title, owl::TWindow* window, bool scale = true);

  void GetDialogInfo(int& minPage, int& maxPage, int& selFromPage, int& selToPage);
  void BeginPrinting();
  void BeginPage(owl::TRect& clientR);
  void PrintPage(int page, owl::TRect& rect, unsigned flags);
  void EndPage();
  void SetBanding(bool b);
  bool HasPage(int pageNumber);

private:

  owl::TWindow* Window;
  bool Scale;
  owl::TPrinter* Printer;
  int MapMode;
  int PrevMode;
  owl::TSize OldVExt, OldWExt;
  owl::TRect OrgR;

};

#endif
