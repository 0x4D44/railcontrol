//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:15 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_VALIDATE_H)
#define OWL_VALIDATE_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_BITSET_H)
# include <owl/bitset.h>
#endif
#include <owl/validate.rh>
#if !defined(OWL_CONTAIN_H)
# include <owl/contain.h>
#endif
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class _OWLCLASS TXValidator;

//
// Validator option flags
//
enum TValidatorOptions {
  voFill     =  0x0001,   // option to fill in chars on IsValidInput checks
  voTransfer =  0x0002,   // option to perform conversion & transfer
  voOnAppend =  0x0004,   // option to only validate input on appending
  voReserved =  0x00F8    // reserved for future use
};

//
// class TValidator
// ~~~~~ ~~~~~~~~~~
class _OWLCLASS TValidator : public TStreamableBase {
  public:
    TValidator();
    virtual ~TValidator();

    virtual void Error(TWindow* owner); //!CQ incompatible change, needs doc!
    virtual bool IsValidInput(LPTSTR input, bool suppressFill);
    virtual bool IsValid(LPCTSTR input);
    virtual uint Transfer(LPTSTR text, void* buffer, TTransferDirection direction);
    virtual int  Adjust(owl_string& text, uint& begPos, uint& endPos, int amount); //!CQ new!

    // Checks input against validator for completeness. Never modifies input.
    // Calls error if not valid.
    //
    bool         Valid(LPCTSTR str, TWindow* owner = 0);
    bool         HasOption(int option);
    void         SetOption(int option);
    void         UnsetOption(int option);

#if defined(OWL2_COMPAT)
    typedef __OWL_GLOBAL::TXValidator TXValidator;  // Exception classes moved to global scope
#endif

  protected:
    uint16   Options;

//  DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TValidator );

//
// class TXValidator
// ~~~~~ ~~~~~~~~~~~
class _OWLCLASS TXValidator : public TXOwl {
  public:
    TXValidator(uint resId = IDS_VALIDATORSYNTAX);

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXValidator* Clone();
#endif
    void Throw();

    static void Raise();
};

//
// enum TPXPictureValidator
// ~~~~ ~~~~~~~~~~~~~~~~~~~
// Validation result type
//
enum TPicResult {
  prComplete,
  prIncomplete,
  prEmpty,
  prError,
  prSyntax,
  prAmbiguous,
  prIncompNoFill
};

//
// class TPXPictureValidator
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
class _OWLCLASS TPXPictureValidator : public TValidator {
  public:
    TPXPictureValidator(LPCTSTR pic, bool autoFill=false);

    // Override TValidator's virtuals
    //
    void         Error(TWindow* owner);
    bool         IsValidInput(LPTSTR str, bool suppressFill);
    bool         IsValid(LPCTSTR str);
    int          Adjust(owl_string& text, uint& begPos, uint& endPos, int amount);  //!CQ new!

    virtual TPicResult Picture(LPTSTR input, bool autoFill=false);

  protected:
    const owl_string& GetPic() const;
    void              SetPic(const owl_string& pic);

  protected_data:
    owl_string Pic;

  private:
    bool         IsComplete(TPicResult rslt);
    bool         IsIncomplete(TPicResult rslt);
    void         ToGroupEnd(uint termCh, uint& i);
    bool         SkipToComma(uint termCh, uint& i);
    uint         CalcTerm(uint termCh, uint i);
    TPicResult   Iteration(LPTSTR input, uint termCh, uint& i, uint& j);
    TPicResult   Group(LPTSTR input, uint termCh, uint& i, uint& j);
    TPicResult   CheckComplete(uint termCh, uint& i, TPicResult rslt);

    TPicResult   Scan(LPTSTR input, uint termCh, uint& i, uint& j);
    TPicResult   Process(LPTSTR input, uint termCh, uint& i, uint& j);
    bool         SyntaxCheck();

  //DECLARE_STREAMABLE(_OWLCLASS, TPXPictureValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TPXPictureValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TPXPictureValidator );

//
// class TFilterValidator
// ~~~~~ ~~~~~~~~~~~~~~~~
class _OWLCLASS TFilterValidator : public TValidator {
  public:
    TFilterValidator(const TCharSet& validChars);

    // Override TValidator's virtuals
    //
    void         Error(TWindow* owner);
    bool         IsValid(LPCTSTR str);
    bool         IsValidInput(LPTSTR str, bool suppressFill);

  protected:
    const TCharSet&  GetValidChars();
    void             SetValidChars(const TCharSet& vc);

  protected_data:
    TCharSet     ValidChars;

  //DECLARE_STREAMABLE(_OWLCLASS, TFilterValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TFilterValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TFilterValidator );

//
// class TRangeValidator
// ~~~~~ ~~~~~~~~~~~~~~~
class _OWLCLASS TRangeValidator : public TFilterValidator {
  public:
    TRangeValidator(long min, long max);

    // Override TValidator's virtuals
    //
    void         Error(TWindow* owner);
    bool         IsValid(LPCTSTR str);
    uint         Transfer(LPTSTR str, void* buffer, TTransferDirection direction);
    int          Adjust(owl_string& text, uint& begPos, uint& endPos, int amount);  //!CQ new!

  protected:
    long   GetMin();
    void   SetMin(long min);
    long   GetMax();
    void   SetMax(long max);

  protected_data:
    long   Min;
    long   Max;

  //DECLARE_STREAMABLE(_OWLCLASS, TRangeValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TRangeValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TRangeValidator );

//
// class TLookupValidator
// ~~~~~ ~~~~~~~~~~~~~~~~
class _OWLCLASS TLookupValidator : public TValidator {
  public:
    TLookupValidator();

    // Override TValidator's virtuals
    //
    bool         IsValid(LPCTSTR str);

    // Virtual lookup of a string
    //
    virtual bool Lookup(LPCTSTR str);

  //DECLARE_STREAMABLE(_OWLCLASS, TLookupValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TLookupValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TLookupValidator );


//
// class TStringLookupValidator
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~
class _OWLCLASS TStringLookupValidator : public TLookupValidator {
  public:
    TStringLookupValidator(TSortedStringArray* strings);
   ~TStringLookupValidator();

    // Override TValidator's virtuals
    //
    void         Error(TWindow* owner);
    int          Adjust(owl_string& text, uint& begPos, uint& endPos, int amount); //!CQ new!

    // Override TLookupValidator's virtuals
    //
    bool         Lookup(LPCTSTR str);

    void         NewStringList(TSortedStringArray* strings);

  protected:
    const TSortedStringArray* GetStrings() const;
    void                      SetStrings(TSortedStringArray* strings);

  protected_data:
    TSortedStringArray* Strings;

  //DECLARE_STREAMABLE(_OWLCLASS, TStringLookupValidator, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TStringLookupValidator, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TStringLookupValidator );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline Implementation
//

//
// Return true if the entry for the validator is valid.
//
inline bool TValidator::Valid(LPCTSTR str, TWindow* owner)
{
  if (!IsValid(str)) {
    Error(owner);
    return false;
  }
  return true;
}

//
// Return true if the validator has a particular option.
//
inline bool TValidator::HasOption(int option)
{
  return Options & option;
}

//
// Turn on a validator option.
//
inline void TValidator::SetOption(int option)
{
  Options |= uint16(option);
}

//
// Turn off a validator option.
//
inline void TValidator::UnsetOption(int option)
{
  Options &= uint16(~option);
}

//
// Return the picture mask used by the validator.
//
inline const owl_string& TPXPictureValidator::GetPic() const
{
  return Pic;
}

//
// Set the picture mask for the validator.
//
inline void TPXPictureValidator::SetPic(const owl_string& pic)
{
  Pic = pic;
}

//
// Return the valid character set for the validator.
//
inline const TCharSet& TFilterValidator::GetValidChars()
{
  return ValidChars;
}

//
// Set the valid character set for the validator.
//
inline void TFilterValidator::SetValidChars(const TCharSet& vc)
{
  ValidChars = vc;
}

//
// Return the minimum number the validator can accept.
//
inline long TRangeValidator::GetMin()
{
  return Min;
}

//
// Set the minimum number the validator can accept.
//
inline void TRangeValidator::SetMin(long min)
{
  Min = min;
}

//
// Return the maximum number the validator can accept.
//
inline long TRangeValidator::GetMax()
{
  return Max;
}

//
// Set the maximum number the validator can accept.
//
inline void TRangeValidator::SetMax(long max)
{
  Max = max;
}

//
// Return the set of valid strings used by the validator.
//
inline const TSortedStringArray* TStringLookupValidator::GetStrings() const
{
  return Strings;
}

//
// Set the valid strings used by the validator.
//
inline void TStringLookupValidator::SetStrings(TSortedStringArray* strings)
{
  delete Strings;
  Strings = strings;
}

__OWL_END_NAMESPACE


#endif  // OWL_VALIDATE_H
