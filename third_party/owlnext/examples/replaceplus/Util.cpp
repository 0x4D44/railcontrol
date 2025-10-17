#include "pch.h"
#pragma hdrstop

#include "Util.h"
#include <owl/shellitm.h>

using namespace owl;

//-----------------------------------------------------------------------------

tstring BrowseForFolder(HWND parent, const tstring& path, const tstring& title)
{
  // Callback for the BrowseForFolder; sets the initial folder.
  //
  struct TLocal
  {
    static int CALLBACK BrowseCallbackProc(HWND w, UINT msg, LPARAM, LPARAM lpData)
    {
      if (msg == BFFM_INITIALIZED)
        ::SendMessage(w, BFFM_SETSELECTION, TRUE, lpData);
      return 0;
    }
  };

  // Encapsulates the deallocation of a shell item.
  //
  struct TCleanUp
  {
    LPITEMIDLIST Handle;
    TCleanUp(LPITEMIDLIST h) : Handle(h) {}
    ~TCleanUp()
    {
      LPMALLOC a = 0;
      HRESULT hr = TShell::SHGetMalloc(&a);
      WARN(FAILED(hr), "SHGetMalloc failed");
      a->Free(Handle);
    }
  };

  // Get the PIDL for Computer folder.
  //
  LPITEMIDLIST myComputer;
  HRESULT hr = TShell::SHGetSpecialFolderLocation(parent, CSIDL_DRIVES, &myComputer);
  if (FAILED(hr)) return tstring();
  TCleanUp myComputerCleanUp(myComputer);

  // Set up a buffer.
  //
  tchar buf[_MAX_PATH];
  buf[0] = _T('\0');
  _tcsncat(buf, path.c_str(), COUNTOF(buf) - 1);

  // Browse for a folder.
  //
  BROWSEINFO bi;
  bi.hwndOwner = parent;
  bi.pidlRoot = myComputer;
  bi.pszDisplayName = buf;
  bi.lpszTitle = title.c_str();
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
  bi.lpfn = &TLocal::BrowseCallbackProc;
  bi.lParam = reinterpret_cast<LPARAM>(&buf);
  LPITEMIDLIST folder = TShell::SHBrowseForFolder(&bi);
  if (!folder) return tstring();
  TCleanUp folderCleanUp(folder);

  // Get the full path to the folder.
  //
  bool r = TShell::SHGetPathFromIDList(folder, buf);
  if (!r) return tstring();

  return buf;
}


