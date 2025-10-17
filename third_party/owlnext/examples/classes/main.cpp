//
// OWLNext Application Entry Point
//
#include "pch.h"
#pragma hdrstop

#include "app.h"

using namespace owl;

auto OwlMain(int, LPTSTR []) -> int // argc, argv
{
  try
  {
    return TClassesApp{}.Run();
  }
  catch (const std::exception& x)
  {
    TRACE(_T("Exiting due to unhandled std::exception: ") << x.what()); InUse(x);
    throw;
  }
  catch (const TXEndSession&)
  {
    TRACE(_T("Exiting due to WM_ENDSESSION."));
    throw;
  }
  catch (int x)
  {
    TRACE(_T("Exiting due to unhandled exception of type 'int': ") << x); InUse(x);
    throw;
  }
  catch (...)
  {
    TRACE(_T("Exiting due to unknown unhandled exception."));
    throw;
  }
}
