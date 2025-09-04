//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// OLE Automation - Implementation of TAutoStack
//----------------------------------------------------------------------------
#define INC_OLE2
#include <ocf/pch.h>
#if !defined(OCF_APPDESC_H)
# include <ocf/appdesc.h>
#endif
#if !defined(OCF_OCREG_H)
# include <ocf/ocreg.h>
#endif
#if !defined(OCF_OCCTRL_H)
# include <ocf/occtrl.h>
#endif

__OCF_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// TAutoStack implementation
//

TAutoStack::TAutoStack(DISPID id, VARIANT *stack, TLocaleId locale,
                       int argcount, int named, long * map,
                       TServedObject* owner)
:
  Owner(owner),
  Stack((TAutoVal*)stack),
  LangId(LANGIDFROMLCID(locale)),
  ArgCount(argcount),
  NamedCount(named),
  NamedIds(map),
  CurrentArg(-1),
  DispId(id)
{
}

TAutoStack::~TAutoStack()
{
  TAutoVal* val;
  for (val = Stack; ArgCount; --*const_cast<int*>(&ArgCount), val++)
    val->Restore();
}

TAutoVal& TAutoStack::operator [](int index)
{
  TAutoSymbol* argSymbol = NULL;
  if (index < ArgSymbolCount)
    argSymbol = index >= 0 ? Symbol + index + 1 : Symbol;
  int vIndex = ArgCount - index - 1;        // index if not a named argument
  if (index == TAutoStack::SetValue ||      // property value to set
      index >= ArgCount-NamedCount) {       // named or out of range
    for (vIndex = NamedCount; --vIndex >= 0; )
      if (NamedIds[vIndex] == index) {
        break;
      }
  }
  TAutoVal* val;
  if (vIndex >= 0) {
    CurrentArg = vIndex;   // save index for error return
    val = Stack[vIndex].DereferenceVariant();
    val->SetLocale(LangId);
    if ((val->GetDataType() == atString) &&
        ((argSymbol) && (argSymbol->IsEnum()) &&
         (argSymbol->GetEnum()->Convert(*val, Default))))
      return Default;
  }
  else if (index == TAutoStack::SetValue) {
    TXAuto::Raise(TXAuto::xParameterMissing);
  }
  else {
    val = &Default;
    val->SetLocale(LangId);
    if (!argSymbol)
      TXAuto::Raise(TXAuto::xNoArgSymbol);
    const _TCHAR* dfltStr = argSymbol->Doc.Translate(LangId); // load default
    if (!dfltStr)
      TXAuto::Raise(TXAuto::xNoDefaultValue);
    Default = dfltStr;    // makes a BSTR in order to use OLE conversions
  }
  return *val;
}


__OCF_END_NAMESPACE

//==============================================================================

