#include "pch.h"

#include "progressdlg.h"

#include <owlext/thredprg.h>

using namespace owl;
using namespace OwlExt;

class SillyThread : public TProgressThread
{
public:
  SillyThread(TProgressDlg* target, int count)
    : TProgressThread(target, 0), _count(count)
  {}

  int Run()
  {
    NotifyStart(_count);
    for (int i = 0; i < _count && !ShouldTerminate(); ++i)
    {
      Sleep(100);
      NotifyProgress(i);
      auto s = tostringstream{};
      s << _T("Running Silly Loop: ") << i;
      NotifyProgressMessage(s.str());
    }
    NotifyEnd(ShouldTerminate() ? IDCANCEL : IDOK);
    return 0;
  }

protected:
  int   _count;
};

class SillyProgressDlg : public TThreadProgressDlg
{
public:
  SillyProgressDlg(TWindow* parent)
    : TThreadProgressDlg(parent, _T("Running Silly Loop"))
  {}

  TProgressThread* BuildThread() { return new SillyThread(this, 500); }
};

void ShowProgessDialog(TWindow* parent)
{
  int ret = SillyProgressDlg(parent).Execute();
  if (ret == IDCANCEL)
  {
    parent->MessageBox(_T("Cancelled by user"));
  }
}