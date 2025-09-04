//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:33 $
//
// Definition of TInputDialog class
//----------------------------------------------------------------------------

#if !defined(OWL_INPUTDIA_H)
#define OWL_INPUTDIA_H

#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#include <owl/inputdia.rh>
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

class _OWLCLASS TValidator;

//
// class TInputDialog
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TInputDialog : public TDialog {
  public:
    TInputDialog(TWindow*     parent,
                 LPCTSTR      title,
                 LPCTSTR      prompt,
                 LPTSTR        buffer,
                 int          bufferSize,
                 TModule*     module = 0,
                 TValidator*  valid = 0);  // Optional validator

   ~TInputDialog();

    // Override TWindow virtual member functions
    //
    void TransferData(TTransferDirection);

    LPCTSTR GetPrompt() const;
    LPCTSTR GetBuffer() const;
    int     GetBufferSize() const;

  protected:
    // Override TWindow virtual member functions
    //
    void SetupWindow();

  public_data:
    _TCHAR * Prompt;
    _TCHAR * Buffer;
    int          BufferSize;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TInputDialog(const TInputDialog&);
    TInputDialog& operator=(const TInputDialog&);

  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TInputDialog, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TInputDialog, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TInputDialog );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return the prompt for the dialog.
//
inline LPCTSTR TInputDialog::GetPrompt() const
{
  return Prompt;
}

//
// Return the buffer.
//
inline LPCTSTR TInputDialog::GetBuffer() const
{
  return Buffer;
}

//
// Return the size of the buffer.
//
inline int TInputDialog::GetBufferSize() const
{
  return BufferSize;
}



__OWL_END_NAMESPACE

#endif  // OWL_INPUTDIA_H
