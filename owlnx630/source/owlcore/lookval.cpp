//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2007-05-11 15:29:19 $                                                    //
//
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_VALIDATE_H)
# include <owl/validate.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

// Let the compiler know that the following template instances will be defined elsewhere. 
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

//
// TLookupValidator
//
TLookupValidator::TLookupValidator()
{
}

//
//
//
bool
TLookupValidator::IsValid(LPCTSTR str)
{
  return Lookup(str);
}

//
//
//
bool
TLookupValidator::Lookup(LPCTSTR /*str*/)
{
  return true;
}

//----------------------------------------------------------------------------
//
// TStringLookupValidator
//

TStringLookupValidator::TStringLookupValidator(TSortedStringArray* strings)
:
  TLookupValidator()
{
  Strings = strings ? strings : new TSortedStringArray;
}

//
//
//
TStringLookupValidator::~TStringLookupValidator()
{
  delete Strings;
}

//
//
//
void
TStringLookupValidator::Error(TWindow* owner)
{
  PRECONDITION(owner);
  TApplication* app = owner->GetApplication();
  LPCTSTR msg = app->LoadString(IDS_VALNOTINLIST).c_str();
  owner->MessageBox(msg, 0, MB_ICONEXCLAMATION|MB_OK);
}

//
//
//
bool
TStringLookupValidator::Lookup(LPCTSTR str)
{
  if (Strings)
    return Strings->Find(owl_string(str)) != (int)NPOS;
  return false;
}

//
//
//
void
TStringLookupValidator::NewStringList(TSortedStringArray* strings)
{
  delete Strings;
  Strings = strings;
}

// Adjust the 'value' of the text, given a cursor position & an amount
// Return the actual amount adjusted.
//
int
TStringLookupValidator::Adjust(owl_string& text, uint& /*begPos*/, uint& /*endPos*/, int amount)
{
  if (!Strings)
    return 0;

  int count = Strings->Size();
  int index = Strings->Find(text);
  if(index == (int)NPOS)
    return 0;
  int newIndex = index + amount;
  if (newIndex < 0)
    newIndex = 0;
  else if (newIndex >= count)
    newIndex = count-1;

  text = (*Strings)[newIndex];
  return newIndex - index;
}

#endif

#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TLookupValidator, TValidator);
IMPLEMENT_STREAMABLE1(TStringLookupValidator, TLookupValidator);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TLookupValidator::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TValidator*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TLookupValidator::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TValidator*)GetObject(), os);
}

//
//
//
void*
TStringLookupValidator::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TLookupValidator*)GetObject(), is);
  unsigned count;
  is >> count;
  GetObject()->Strings = new TSortedStringArray;
  for (unsigned i = 0; i < count; i++ ) {
    owl_string temp;
    is >> temp;
    GetObject()->Strings->Add(temp);
  }
  return GetObject();
}

//
//
//
void
TStringLookupValidator::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TLookupValidator*)GetObject(), os);
  unsigned count = GetObject()->Strings->Size();
  os << count;
  for (unsigned i = 0; i < count; i++)
    os << (*GetObject()->Strings)[i];
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

