#include "pch.h"
#include "app.h"
#include "frame.h"
#include <array>

#include "resource.h"

using namespace owl;
using namespace std;

TClassesApp::TClassesApp()
  :TApplication(_T("OWLNext examples"))
{
}


TClassesApp::~TClassesApp()
{
}

void TClassesApp::InitMainWindow()
{
  TClassesMDIFrame* frame = new TClassesMDIFrame(0, GetName()); // Use the application name as title.
  SetMainWindow(frame);
  GetMainWindow()->SetMenuDescr(TMenuDescr(IDM_MAINMENU));
  GetMainWindow()->SetAcceleratorTable(TResId{IDM_MAINMENU});
}

void TClassesApp::InitInstance()
{
  TApplication::InitInstance();
  AssignMenuBitmaps();
}

void TClassesApp::AssignMenuBitmaps()
{
  PRECONDITION(GetMainWindow());
  PRECONDITION(GetMainWindow()->GetMenuDescr());

  static const auto id = array
  {
    CM_EDITCUT,
    CM_EDITCOPY,
    CM_EDITPASTE
  };

  // Note: We here modify the menu descriptor, not the current menu, because the descriptor is the
  // actual template for merging the menu with the active MDI child's menu descriptor.
  //
  auto m = TMenuDescr{*GetMainWindow()->GetMenuDescr()};
  for (auto c : id)
  {
    auto& b = MenuBitmaps.emplace_back(LoadBitmap(TResId{c})); // In dynamic build mode, searches the OWLNext DLL as well.
    const auto r = m.SetMenuItemBitmaps(c, MF_BYCOMMAND, &b);
    if (!r) throw runtime_error{__func__ + ": SetMenuItemBitmaps failed: "s + to_string(::GetLastError())};
  }
  GetMainWindow()->SetMenuDescr(m); // Updates the menu based on the new descriptor.
}
