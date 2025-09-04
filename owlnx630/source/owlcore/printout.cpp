//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:43 $
//
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h> 
#if !defined(OWL_PRINTER_H)
# include <owl/printer.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

//
//
//
TPrintout::TPrintout(LPCTSTR title)
:
  Title(strnewdup(title)),
  Banding(false),
  ForceAllBands(true),
  DC(0),
  PageSize(0)
{
}

//
//
//
TPrintout::~TPrintout()
{
  delete[] Title;
}

//
//
//
void
TPrintout::SetPrintParams(TPrintDC* dc, TSize pageSize)
{
  SetPrintDC(dc);
  SetPageSize(pageSize);
}

//
//
//
void
TPrintout::GetDialogInfo(int& minPage, int& maxPage,
                         int& selFromPage, int& selToPage)
{
  minPage = 1;
  maxPage = INT_MAX;
  selFromPage = selToPage = 0;
}

//
//
//
void
TPrintout::BeginPrinting()
{
}

//
//
//
void
TPrintout::BeginDocument(int, int, unsigned)
{
}

//
//
//
bool
TPrintout::HasPage(int page)
{
  return page == 1;
}

//
//
//
void
TPrintout::PrintPage(int, TRect&, unsigned)
{
}

//
//
//
void
TPrintout::EndDocument()
{
}

//
//
//
void
TPrintout::EndPrinting()
{
}

//
// Set a new title for this printout. The passed title string is copied.
//
void
TPrintout::SetTitle(LPCTSTR title)
{
  delete[] Title;
  Title = strnewdup(title);
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE(TPrintout);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TPrintout::Streamer::Read(ipstream& is, uint32) const
{
#if defined(UNICODE)
  _USES_CONVERSION;
  char * title = is.freadString();
  
  GetObject()->Title = _A2W(title);

  delete[] title;
#else
  GetObject()->Title = is.freadString();
#endif
  is >> GetObject()->Banding;
  is >> GetObject()->ForceAllBands;
  return GetObject();
}

//
//
//
void
TPrintout::Streamer::Write(opstream& os) const
{
  _USES_CONVERSION;
  os.fwriteString(_W2A(GetObject()->Title));
  os << GetObject()->Banding;
  os << GetObject()->ForceAllBands;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

