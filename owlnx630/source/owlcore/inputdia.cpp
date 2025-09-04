//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2007-07-20 10:47:43 $                                                    //
//
// Implementation of TInputDialog.  User string input dialog box
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_INPUTDIA_H)
# include <owl/inputdia.h>
#endif
#if !defined(OWL_EDIT_H)
# include <owl/edit.h>
#endif
#if !defined(OWL_VALIDATE_H)
# include <owl/validate.h>
#endif
#include <string.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

/** Let the compiler know that the following template instances will be defined elsewhere. **/
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

//
//
//
TInputDialog::TInputDialog(TWindow*        parent,
                           LPCTSTR         title,
                           LPCTSTR         prompt,
                           LPTSTR          buffer,
                           int             bufferSize,
                           TModule*        module,
                           TValidator*     validator)
:
  TWindow(parent, title, module),
  TDialog(parent, IDD_INPUTDIALOG, module)
{
  PRECONDITION(buffer);
  SetCaption(title);
  Prompt = strnewdup(prompt);
  Buffer = buffer;
  BufferSize = bufferSize;
  if (validator)
    (new TEdit(this,ID_INPUT))->SetValidator(validator);
}

//
//
//
TInputDialog::~TInputDialog()
{
  delete[] Prompt;
}

//
// sets and gets the values of the items (controls) of the input dialog
//
void
TInputDialog::TransferData(TTransferDirection direction)
{
  if (direction == tdSetData) {
    SetDlgItemText(ID_PROMPT, Prompt);
    SetDlgItemText(ID_INPUT, Buffer);
  }
  else if (direction == tdGetData) {
    GetDlgItemText(ID_INPUT, Buffer, BufferSize);
  }
}

//
// sets the values of the items(controls) of the input dialog
//
void
TInputDialog::SetupWindow()
{
  TDialog::SetupWindow();
  SendDlgItemMessage(ID_INPUT, EM_LIMITTEXT, BufferSize - 1, 0);
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE2(TInputDialog, TDialog, TWindow);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TInputDialog from the passed ipstream
//
void*
TInputDialog::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TDialog*)GetObject(), is);
#if defined(UNICODE)
  _USES_CONVERSION;
  char * prompt = is.freadString();

  GetObject()->Prompt = strnewdup(_A2W(prompt));
  
  delete[] prompt;
#else
  GetObject()->Prompt = is.freadString();
#endif
  return GetObject();
}

//
// writes the TInputDialog to the passed opstream
//
void
TInputDialog::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TDialog*)GetObject(), os);
  _USES_CONVERSION;
  os.fwriteString(_W2A(GetObject()->Prompt));
}


#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

