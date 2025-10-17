#include "pch.h"
#pragma hdrstop

#include "Replacer.h"
#include <owl/filename.h>
#include <algorithm>

using namespace owl;
using namespace std;

namespace
{

  const uint FileAndExt = TFileName::File | TFileName::Ext;

  //-----------------------------------------------------------------------------

  class TFileProcessorBase 
  {
  public:

    virtual bool ProcessFile(const owl::TFileName&) = 0;
    virtual void ReportResults(owl::TWindow&) = 0;
  };

  //-----------------------------------------------------------------------------

  class TFileRecurser
  {
  public:

    TFileRecurser(const owl::tstring& folder, const owl::tstring& filter, bool recurseFlag, TFileProcessorBase&);

    bool RecurseFolders();

  private:

    owl::tstring Folder;
    typedef std::vector<owl::tstring> TFilters;
    TFilters Filters;
    bool RecurseFlag;
    TFileProcessorBase& FileProcessor;

    bool RecurseFiles(const tstring& folder);
    bool IterateFiles(const tstring& folder);
  };

  //-----------------------------------------------------------------------------

  //
  // Replaces text in the given file.
  //
  class TReplacer 
    : public TFileProcessorBase
  {
  public:

    TReplacer(const owl::tstring& searchTerm, const owl::tstring& replacement)
      : SearchTerm(searchTerm), Replacement(replacement), FileCount(0), ReplacementCount(0)
    {}

    virtual bool ProcessFile(const owl::TFileName&); // override
    virtual void ReportResults(owl::TWindow&); // override

  private:

    owl::tstring SearchTerm;
    owl::tstring Replacement;
    int FileCount;
    int ReplacementCount;
  };

  //-----------------------------------------------------------------------------

  //
  // Changes file date and time.
  //
  class TToucher : public TFileProcessorBase
  {
  public:

    TToucher(const owl::TTime& filetime)
      : FileTime(filetime), FileCount(0) 
    {}

    virtual bool ProcessFile(const owl::TFileName&); // override
    virtual void ReportResults(owl::TWindow&); // override

  private:

    owl::TTime FileTime;
    int FileCount;
  };

  //-----------------------------------------------------------------------------

  TFileRecurser::TFileRecurser(const tstring& folder, const tstring& filter, bool recurseFlag, TFileProcessorBase& p)
    : Folder(folder), RecurseFlag(recurseFlag), FileProcessor(p)
  {
    // Split the filter string into individual filters.
    //
    tstring::const_iterator b = filter.begin();
    for (;;)
    {
      tstring::const_iterator e = std::find(b, filter.end(), _T(';'));
      Filters.push_back(tstring(b, e));
      if (e == filter.end()) break;
      b = ++e;
    }
  }

  //-----------------------------------------------------------------------------

  bool TFileRecurser::RecurseFolders()
  {
    return RecurseFlag ? RecurseFiles(Folder) : IterateFiles(Folder);
  }

  //-----------------------------------------------------------------------------

  bool TFileRecurser::RecurseFiles(const tstring& folder)
  {
    // Recurse subfolders.
    //
    TFileName p = TFileName(folder, true);
    p.MergeParts(FileAndExt, _T("*.*"));
    for (TFileNameIterator i(p.Canonical()); i; ++i) 
    {
      if ((i.Current().attribute & FILE_ATTRIBUTE_DIRECTORY) == 0) continue;
      
      tstring f = i.Current().fullName;
      if (f == _T(".") || f == _T("..")) continue;

      p.MergeParts(FileAndExt, f);
      bool r = RecurseFiles(p.Canonical());
      if (!r) return false;
    }
    return IterateFiles(folder);
  }

  //-----------------------------------------------------------------------------

  bool TFileRecurser::IterateFiles(const tstring& folder)
  {
    // Iterate over files by filter.
    //
    TFileName p = TFileName(folder, true);
    for (TFilters::const_iterator fit = Filters.begin(); fit != Filters.end(); ++fit)
    {
      p.MergeParts(FileAndExt, *fit);
      for (TFileNameIterator i(p.Canonical()); i; ++i) 
      {
        const uint skipFlags = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY;
        if (i.Current().attribute & skipFlags) continue;

        p.MergeParts(FileAndExt, i.Current().fullName);
        bool r = FileProcessor.ProcessFile(p);
        if (!r) return false;
      }
    }
    return true;
  }

  //-----------------------------------------------------------------------------

  bool TReplacer::ProcessFile(const TFileName& filename)
  {
    tifstream ifs(filename.Canonical().c_str(), ios::in | ios::binary);
    if (!ifs) return false;

    TFileName temp(TFileName::TempFile);
    tofstream ofs(temp.Canonical().c_str(), ios::out | ios::binary);
    if (!ofs) return false;

    // Build partial match table (Knuth-Morris-Pratt algorithm).
    //
    // On a mismatch at position i within the search term, the next[i] value gives us the next 
    // position where there may be a match. For example, consider a mismatch at 'x' for the search 
    // term "ab_abx". In this case, next[5] will be 2, since the prefix "ab" is also a suffix of 
    // the partial match "ab_ab".
    //
    //                     Text: ...ab_ab_abx...
    // Search term (mismatched):    ab_abx
    //    Search term (matched):       ab_abx
    //
    vector<int> next(SearchTerm.size());
    {
      int i = 0; 
      int j = next[0] = -1;
      while (i < static_cast<int>(SearchTerm.size()) - 1)
      {
        while (j >= 0 && SearchTerm[i] != SearchTerm[j])
          j = next[j];
        next[++i] = ++j;
      }
    }

    int pos = 0;
    int localReplacements = 0;

    while (!ifs.eof()) 
    {
      tchar c = static_cast<tchar>(ifs.get());
      if (ifs.eof())
      {
        // Output any partial match, then break.
        //
        ofs.write(SearchTerm.c_str(), pos);
        break;
      }

      // On mismatch, backtrack to the next partial match, if any.
      // Then output the mismatching part.
      //
      int oldpos = pos;
      while (pos > 0 && c != SearchTerm[pos])
        pos = next[pos];
      ofs.write(SearchTerm.c_str(), oldpos - pos);

      // Accumulate match, or output mismatched character.
      //
      if (c == SearchTerm[pos]) // match
      {
        pos++;
        if (pos == static_cast<int>(SearchTerm.size())) // Full match; output replacement.
        {
          ofs << Replacement;
          ReplacementCount++;
          localReplacements++;
          pos = 0;
        }
      }
      else // no match
        ofs.put(c);
    }

    ifs.close();
    ofs.close();

    // If replacements were made, then overwrite the original file with the changes.
    // Then remove the temporary file.
    //
    if (localReplacements > 0) 
    {
      bool r = temp.Copy(filename, false);
      if (!r) return false;
    }
    bool r = temp.Remove();
    if (!r) return false;

    FileCount++;
    return true;
  }

  //-----------------------------------------------------------------------------

  void TReplacer::ReportResults(TWindow& window)
  {
    tostringstream s;
    if (FileCount == 0)
      s << _T("No files processed.");
    else if (FileCount == 1)
      s << _T("1 file processed.");
    else
      s << FileCount << _T(" files processed.");
    s << endl;

    if (ReplacementCount == 0)
      s << _T("No replacements made.");
    else if (ReplacementCount == 1)
      s << _T("1 replacement made.");
    else
      s << ReplacementCount << _T(" replacements made.");

    window.MessageBox(s.str(), _T("Replace Results"), MB_ICONINFORMATION);
  }

  //-----------------------------------------------------------------------------

  bool TToucher::ProcessFile(const TFileName& filename)
  {
    TFileStatus status;
    if (!filename.GetStatus(status))
      return false;

    status.modifyTime = FileTime;
    if (!TFileName(filename).SetStatus(status))
      return false;

    FileCount++;
    return true;
  }

  //-----------------------------------------------------------------------------

  void TToucher::ReportResults(TWindow& window)
  {
    tostringstream s;
    if (FileCount == 0)
      s << _T("No files processed.");
    else if (FileCount == 1)
      s << _T("1 file processed.");
    else
      s << FileCount << _T(" files processed.");
    window.MessageBox(s.str(), _T("Replace"), MB_OK);
  }

} // namespace

//-----------------------------------------------------------------------------

bool PerformCommand(const TReplaceData& d, TWindow* resultParent)
{
  if (d.Action == 0)
  {
    // Perform find-and-replace.
    //
    TReplacer replacer(d.SearchTerm, d.Replacement);
    bool r = TFileRecurser(d.Folder, d.Filter, d.RecurseFlag, replacer).RecurseFolders();
    if (r && resultParent)
      replacer.ReportResults(*resultParent);
    return r;
  }
  else 
  {
    // Perform touch.
    //
    TFileTime ft(TTime(d.Date, d.Time.GetHour(), d.Time.GetMinute(), d.Time.GetSecond()));
    ft.ToUniversalTime();

    TToucher toucher(ft);
    bool r = TFileRecurser(d.Folder, d.Filter, d.RecurseFlag, toucher).RecurseFolders();
    if (r && resultParent)
      toucher.ReportResults(*resultParent);
    return r;
  }
}


