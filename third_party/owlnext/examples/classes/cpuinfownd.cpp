#include "pch.h"
#include "cpuinfownd.h"

#include <owl/system.h>
#include <owl/gdiobjec.h>

using namespace owl;
using namespace std;

auto GetCpuInfo() -> tstring
{
  auto& i = TSystem::GetProcessorInfo();
  const auto a = TSystem::GetProcessorArchitectureName(TSystem::GetProcessorArchitecture());
  const auto v = i.GetVendorName(i.GetVendorId());
  const auto nl = _T("\r\n");
  auto s = tostringstream{};
  s << boolalpha << nl
    << _T("  Architecture: ") << (a.empty() ? _T("(unknown)") : a.c_str()) << nl
    << _T("  Number of processors: ") << TSystem::GetNumberOfProcessors() << nl
    << nl
    << _T("  Name: ") << i.GetName() << nl
    << _T("  Identifier: ") << i.GetIdentifier() << nl
    << _T("  Vendor ID: ") << i.GetVendorId() << nl
    << _T("  Vendor name: ") << (v.empty() ? _T("(unknown)") : v.c_str()) << nl
    << nl
    << _T("  Nominal frequency: ") << i.GetNominalFrequency() << _T(" MHz \r\n")
    << nl
    << _T("  Has MMX: ") << i.HasMmx() << nl
    << _T("  Has 3DNow: ") << i.Has3dNow() << nl
    << _T("  Has SSE: ") << i.HasSse() << nl
    << _T("  Has SSE2: ") << i.HasSse2() << nl
    << _T("  Has SSE3: ") << i.HasSse3() << nl;
  return s.str();
}

TCpuInfoWindow::TCpuInfoWindow()
  : TEdit{ 0, 1, 0, 0, 0, 0, 0 },
  Font{ _T("Microsoft Sans Serif"), -12 }
{
  ModifyStyle(0, ES_READONLY | ES_MULTILINE);
  SetWindowPos(nullptr, 0, 0, 350, 300, SWP_NOMOVE);
  SetBkgndColor(TColor::SysWindow);
}

void TCpuInfoWindow::SetupWindow()
{
  TEdit::SetupWindow();
  SetWindowFont(Font, false);
  SetText(GetCpuInfo());
}

