// OWLCVT 05/11/95 22:39:27
/* TOOLBAR.CPP
*  ===========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 25/03/1994
*  Version  : 2.02
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"   		// Most important

#if defined(DIAG_SETUP_STACK)
extern volatile const char* gSetupStackLabel;
extern volatile unsigned long gSetupStackBefore;
extern volatile unsigned long gSetupStackAfter;

#define TBAR_STACK_SNAPSHOT(name) unsigned long name = 0; __asm { mov name, esp }
#define TBAR_STACK_VERIFY(label, before)                                           \
  {                                                                                \
    unsigned long _stack_after = 0;                                                \
    __asm { mov _stack_after, esp }                                                \
    if (_stack_after != (before))                                                  \
    {                                                                              \
      gSetupStackLabel = (label);                                                  \
      gSetupStackBefore = (before);                                                \
      gSetupStackAfter = _stack_after;                                             \
      __debugbreak();                                                              \
    }                                                                              \
  }
#else
#define TBAR_STACK_SNAPSHOT(name) unsigned long name = 0
#define TBAR_STACK_VERIFY(label, before) do { (void)(before); } while (0)
#endif


/***************************************************************/
/* General program methods follow...  */


/***************************************************************/
/* Methods of TToolbar follow...  */


TToolbar::TToolbar(TWindow * AParent, int NumButtons, const Toolbuttondata& ButtonData, HBITMAP ButtonBitmap)
	  :TFrameWindow(AParent, "")
{
  // Store given information
  NumButt = NumButtons;
  ButtData = ButtonData;
  ButtBitmap = ButtonBitmap;

  // Turn off the MDI flag that parent set
  Attr.Style = WS_BORDER | WS_CHILD | WS_VISIBLE;

  // Load the up and down button bitmaps
  HINSTANCE hInst = GetModule()->GetHandle();
  HUpButt = ::LoadBitmap(hInst, TEXT("UPBUTTON"));
  HDownButt = ::LoadBitmap(hInst, TEXT("DOWNBUTTON"));
}


TToolbar::~TToolbar()
{
  // Delete objects
  HUpButt.Reset();
  HDownButt.Reset();
}


void TToolbar::SetupWindow()
{
  int i;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Create the toolbar buttons - we are given a bitmap in the form
  //
  //  <enabled-pic><disabled-pic>   - for button 1
  //  <enabled-pic><disabled-pic>   - for button 2
  //  ...........................
  //  ...........................
  //  <enabled-pic><disabled-pic>   - for button n
  //
  //  and we are also told how many buttons to display. We must also
  //  be told the correct button spacing. Thus we are also passed a
  //  pointer to an arrray containing the X spacing information. A
  //  second pointer is also passed which contains information
  //  as to what message value to send when a particular button is
  //  pressed. All toolbar handles are internally stored within this
  //  class.

  for (i=1; i<=NumButt; i++)
  {
    // Create the Toolbar buttons
    TBAR_STACK_SNAPSHOT(stackBeforeButtonEmplace);
    TToolbutton* button = Buttons.Emplace(i,
                                          this,                      // this application
				                                  ButtData.XPos[i],          // X position
				                                  3,                         // Total num buttons
				                                  i,                         // Button num
				                                  ButtData.WParam[i],        // WParam
				                                  ButtData.LParam[i],        // LParam
				                                  ButtData.SentWindow[i],    // Window handle to send message to
				                                  ButtData.StatWindow[i],    // Window handle to send status bar info to
				                                  ButtData.UpText[i].Data(),        // Up text
				                                  ButtData.DownText[i].Data());     // Down text
    TBAR_STACK_VERIFY("TToolbar::Buttons.Emplace", stackBeforeButtonEmplace);

    if (button)
    {
      TBAR_STACK_SNAPSHOT(stackBeforeButtonCreate);
      button->Create();
      TBAR_STACK_VERIFY("TToolbar::button->Create", stackBeforeButtonCreate);
    }
  }
}


void TToolbar::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Update the screen display
  DrawScreen(dc);
}


void TToolbar::DrawScreen(HDC TheDC)
{
  TReleaseDcGuard dcGuard;
  if (TheDC == 0)
  {
    dcGuard.Attach(HWindow, GetDC(HWindow));
    TheDC = dcGuard.Get();
  }
  (void)TheDC;
}


void TToolbar::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


auto TToolbar::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Toolbar Class"));
}


void TToolbar::EvSize(owl::uint sizeType, const TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
}


DEFINE_RESPONSE_TABLE1(TToolbar, TFrameWindow)
    EV_WM_SIZE,
END_RESPONSE_TABLE;


TToolbutton* TToolbar::GetButton(int index)
{
  if (index < 0 || index >= static_cast<int>(Buttons.Size()))
  {
    return nullptr;
  }
  return Buttons.Get(static_cast<std::size_t>(index));
}


const TToolbutton* TToolbar::GetButton(int index) const
{
  if (index < 0 || index >= static_cast<int>(Buttons.Size()))
  {
    return nullptr;
  }
  return Buttons.Get(static_cast<std::size_t>(index));
}
