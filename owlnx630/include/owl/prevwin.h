//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
// Definition of TPreviewWin, the Preview Window class
//----------------------------------------------------------------------------

#if !defined(OWL_PREVWIN_H)
#define OWL_PREVWIN_H

#if !defined(OWL_DECFRAME_H)
# include <owl/decframe.h>
#endif
#if !defined(OWL_CONTROLB_H)
# include <owl/controlb.h>
#endif
#if !defined(OWL_PRINTER_H)
# include <owl/printer.h>
#endif
#if !defined(OWL_PREVIEW_H)
#include <owl/preview.h>
#endif
#if !defined(OWL_BUTTONGA_H)
# include <owl/buttonga.h>
#endif
#if !defined(OWL_TEXTGADG_H)
# include <owl/textgadg.h>
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
// class TPreviewWin
// ~~~~~ ~~~~~~~~~~~
// TPreviewWin encapsulates a simple preview window frame. It contains
// one or two preview pages as well as a simple toolbar.
//
class _OWLCLASS TPreviewWin : public TDecoratedFrame {
  public:
    TPreviewWin(TWindow*   parentWindow, 
                TPrinter&  printer, 
                TPrintout& printout,
                TWindow&   dataWindow, 
                LPCTSTR title, 
                TLayoutWindow* client);
   ~TPreviewWin();

  protected:
    void               SetupWindow();
    virtual void       LayoutPages();
    virtual TPreviewPage* GetNewPreviewPage(TWindow* parent, 
                                            TPrintout& printout,
                                            TPrintDC&  prndc,
                                            TSize&     printExtent,
                                            int        pagenum = 1);
    virtual void       SetupSpeedBar();
    virtual void       UpdateSpeedBar();

    int                FirstPage;         // First Page displayed
    int                LastPage;          // Last Page displayed
    TWindow&           DataWindow;        // Window /w data we're previewing
    TPrinter&          Printer;           // Printer device object
    TPrintout&         Printout;          // Printer document object
    TPrintDC*          PrnDC;             // Printer DC
    TSize              PrinterPageSize;   // Printer's page size

    TControlBar*       PreviewSpeedBar;   // Pointer to toolbar object      
    TPreviewPage*      Page1;             // Pointer to first preview page
    TPreviewPage*      Page2;             // Pointer to second preview page

  private:
    TLayoutWindow*     Client;

    void               CmPrevious();
    void               CePrevious(TCommandEnabler& ce);
    void               CmNext();
    void               CeNext(TCommandEnabler& ce);
    void               CmOneUp();
    void               CmTwoUp();
    void               CeTwoUp(TCommandEnabler& ce);
    void               CmDone();

  DECLARE_RESPONSE_TABLE(TPreviewWin);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_PREVWIN_H
