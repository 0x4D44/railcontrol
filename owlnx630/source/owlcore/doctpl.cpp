//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:39 $
//
// Implementation of class TDocTemplate
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_DOCTPL_H)
# include <owl/doctpl.h>
#endif
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
#if !defined(SECTION) || SECTION == 1

//
// Construct a Doc/View template from the specified parameters.
//
TDocTemplate::TDocTemplate(TRegList& regList, TModule*& module,
                           TDocTemplate*& rptpl)
:
  TRegLink(regList, (TRegLink*&)rptpl),
  Directory(0),
  ModulePtr(&module)
{
  RefCnt = module ? 1 : 0x8001;  // static if constructed before Module
  Flags = _ttol((*RegList)["docflags"]);
}

//
//
//
void TDocTemplate::SetFlag(long flag)
{
  Flags = GetFlags() | flag;
}

//
//
//
void TDocTemplate::ClearFlag(long flag)
{
  Flags = GetFlags() & ~flag;
}

//
//
//
LPCTSTR TDocTemplate::GetDirectory() const
{
  if (Directory)
    return Directory;
  return (*RegList)["directory"];
}

//
//
//
void TDocTemplate::SetDirectory(LPCTSTR txt)
{
  delete[] Directory;
  Directory = 0;
  if (txt)
    Directory = strnewdup(txt);
}

//
//
//
void TDocTemplate::SetDirectory(LPCTSTR txt, int len)
{
  delete[] Directory;
  Directory = 0;
  if (txt && len > 0) {
    Directory = strnewdup(txt, len);
    Directory[len] = 0;
  }
}

//
// Called only when RefCnt goes to 0
//
TDocTemplate::~TDocTemplate()
{
  if (GetFlags() & dtDynRegInfo) {
    delete RegList;
    RegList = 0;
  }
  delete[] Directory;
}

//
LPCTSTR TDocTemplate::GetFileFilter() const
{
  return (*RegList)["docfilter"];
}

//
LPCTSTR TDocTemplate::GetDescription() const
{
  return (*RegList)["description"];
}

//
LPCTSTR TDocTemplate::GetDefaultExt() const
{
  return (*RegList)["extension"];
}

//----------------------------------------------------------------------------
// Backward compatibility with old style doc templates
//
#if defined(OWL2_COMPAT)

//
// The following three function vectors get reset by TDocManager constructor
//
static bool       _CALLCNVN SelectSaveX(TDocTemplate*, TDocument&) {return false;}
static TView*     _CALLCNVN InitViewX(TView*) {return 0;}
static TDocument* _CALLCNVN InitDocX(TDocTemplate&, TDocument*, LPCTSTR, long)
                                    {return 0;}

bool       _CALLCNVN (*TDocTemplate::SelectSave_)(TDocTemplate*,TDocument&) = SelectSaveX;
TView*     _CALLCNVN (*TDocTemplate::InitView_)(TView*) = InitViewX;
TDocument* _CALLCNVN (*TDocTemplate::InitDoc_)(TDocTemplate&, TDocument*,
                                     LPCTSTR, long) = InitDocX;

//Jogy
#endif  //  defined(OWL2_COMPAT)

//
// private class for backward compatibility
//
class TRegListOldDocTemplate : public TRegList {
  public:
    TRegListOldDocTemplate(LPCTSTR desc, LPCTSTR filt,
                           LPCTSTR dir,  LPCTSTR ext, long flags);
    TRegItem List[6];  // 4 strings, flags, terminator
    _TCHAR   FlagBuf[12];  // for string representation of doc template flags
};

//
//
//
TRegListOldDocTemplate::TRegListOldDocTemplate(LPCTSTR desc,
                                               LPCTSTR filt,
                                               LPCTSTR dir,
                                               LPCTSTR ext,
                                               long    flags)
:
  TRegList(List)
{
  _stprintf(FlagBuf,_T("0x%lX"),flags);
  List[0].Key = "description";
  List[0].Value = desc;
  List[1].Key = "docfilter";
  List[1].Value = filt;
  List[2].Key = "directory";
  List[2].Value = dir;
  List[3].Key = "extension";
  List[3].Value = ext;
  List[4].Key = "docflags";
  List[4].Value = FlagBuf;
  List[5].Key = 0;
}

//
// Construct a Doc/View template from the description, filter, directory,
// file extension, 'dt' flags, module and template head parameters.
// This constructor is primarily for backward compatibility with earlier
// implementation of ObjectWindows' Doc/View subsystem.
//
TDocTemplate::TDocTemplate(LPCTSTR desc, LPCTSTR filt,
                           LPCTSTR dir, LPCTSTR ext,
                           long flags, TModule*& module,
                           TDocTemplate*& rphead)
:
  TRegLink(),
  Directory(0),
  Flags(flags | dtDynRegInfo),
  ModulePtr(&module)
{
//JJH this is nasty...but works.
#if (__GNUC__ >= 3)
  AddLink((TRegLink**)&rphead, reinterpret_cast<TRegLink*>(this));
#else
  AddLink((TRegLink**)&rphead, (TRegLink*)this);
#endif
  RefCnt = module ? 1 : 0x8001;  // static if contructed before Module
//  NextTemplate = 0;
  RegList = new TRegListOldDocTemplate(desc, filt, dir, ext, flags);
}

//Jogy#endif  //  defined(OWL2_COMPAT)

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_ABSTRACT_STREAMABLE(TDocTemplate);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TDocTemplate::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TDocTemplate* o = GetObject();
  bool wasStatic = o->IsStatic();  // test in case dummy template passed
  is >> o->RefCnt;  // need to set back to 1 if doc attach increments!!?
  is >> o->Flags;
  if (o->IsStatic()) {
    delete[] o->Directory;
  }
#if defined(UNICODE)
  _USES_CONVERSION;
  char * dir = is.freadString();
  o->Directory  = strnewdup(_A2W(dir));
  delete[] dir;
#else
  o->Directory   = is.freadString();
#endif
  if (o->IsStatic() && !wasStatic) {  // dummy template passed as sink
    delete[] o->Directory;
  }
  return o;
  //!JD need to link up reg info table!!
}

//
//
//
void
TDocTemplate::Streamer::Write(opstream& os) const
{
  TDocTemplate* o = GetObject();
  os << o->RefCnt;
  os << o->GetFlags();
  _USES_CONVERSION;
  os.fwriteString(_W2A(o->Directory));
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE

