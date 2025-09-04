//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of TDialog class and TDialogAttr struct
//----------------------------------------------------------------------------

#if !defined(OWL_DIALOG_H)
#define OWL_DIALOG_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_RESOURCE_H)
# include <owl/resource.h>
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

//
// struct TDialogAttr
// ~~~~~~ ~~~~~~~~~~~
// TDialog creation attributes
//
struct TDialogAttr {
  LPTSTR Name;
  uint32 Param;
};

//
// class TControlEnabler
// ~~~~~ ~~~~~~~~~~~~~~~
// A TCommandEnabler for child controls in a dialog
//
class _OWLCLASS TControlEnabler : public TCommandEnabler {
  public:
    TControlEnabler(uint id, HWND hWndReceiver = 0);

    // Override virtuals
    //
    void Enable(bool enable = true);
    void SetText(LPCTSTR text);

    // Use this method with only buttons!
    //
    void SetCheck(int check);
};


//
// class TDialog
// ~~~~~ ~~~~~~~
class _OWLCLASS TDialog : virtual public TWindow {
  public:
    // standard constructor
    TDialog(TWindow* parent, TResId resId, TModule* module = 0);
    // construct from pointer to template 
    explicit TDialog(TWindow* parent, const DLGTEMPLATE& dlgTemplate, TAutoDelete = AutoDelete, TModule* module = 0);
    // Not usual,to avoid ambiquaity, construct from handle to template
    explicit TDialog(HGLOBAL hTemplate, TWindow* parent, TAutoDelete = AutoDelete, TModule* module=0);

   ~TDialog();

    // Override this to process messages within the dialog function
    // Return true if message handled, false if not.
    //
    virtual bool    DialogFunction(uint message, TParam1 wParam, TParam2 lParam);

    // Virtual handler for WM_INITDIALOG message, called from DialogFunction()
    //
    virtual bool    EvInitDialog(HWND hWndFocus);

    // Initialize dialog controls with contents of RT_DLGINIT
    //
    bool            PerformDlgInit();

    // Handler for WM_SETFONT, is dispatched from DialogFunction() once
    // during dialog creation, subsequently as normal.
    //
    void            EvSetFont(HFONT hFont, bool redraw);

    // Create a modeless dialog box, and perform actual create call
    //
    virtual bool    Create();
    virtual void    PerformCreate(int menuOrId);

#if defined(OWL5_COMPAT)
    // backward compatibility. not use
    virtual HWND    DoCreate();
#endif
    // Create a modal dialog box, and perform actual modal execute call
    //
    virtual int     Execute();
    virtual int     DoExecute();

    // Override virtual functions defined by class TWindow
    //
    bool            PreProcessMsg(MSG& msg);
    void            CloseWindow(int retValue = IDCANCEL);
    void            Destroy(int retValue = IDCANCEL);

    void            SetCaption(LPCTSTR title);

    TDialogAttr&    GetDialogAttr();
    void            SetDialogAttr(TDialogAttr attr);

    bool            GetIsModal() const;
    void            SetIsModal(bool ismodal);
    
    // enable emulation of Modal Dialog by BeginModal/EndModal
    void            EnableModal(bool enable=true); 

    // Handy dialog functions
    //
    uint            GetDefaultId() const;
    void            SetDefaultId(uint id);
    void            MapDialogRect(TRect& rect);
    void            Reposition();

    // Enumeration to be used when invoking the 'SetControlFocus' method.
    // It specifies whether focus should be set to the next or previous
    // sibling of the current control with focus.
    //
    enum THow {
      Next,         // Set focus to the next sibling
      Previous      // Set focus to the previous sibling
    };
    void            SetControlFocus(HWND hwnd);
    void            SetControlFocus(THow how);

    void            SetMsgResult(uint32 result);

    // Message response functions
    //
    void            EvClose();
    void            EvPaint();
    HBRUSH          EvCtlColor(HDC, HWND hWndChild, uint ctlType);
    uint32          EvGetDefId();
    bool            EvSetDefId(int id);
    void            EvReposition();

    // Child notifications
    //
    void            CmOk();      // IDOK
    void            CmCancel();  // IDCANCEL

#if defined(OWL2_COMPAT)
    // Returns the handle of the dialog's control with the passed Id
    // Obsolete- use TWindow::GetDlgItem(Id)
    //
    HWND            GetItemHandle(int childId);
#endif

    // Sends the passed message to the dialog's control which has id DlgItemId
    // Obsolete- use TWindow::SendDlgItemMessage()
    //
    uint32 SendDlgItemMsg(int childId, uint16 msg, uint16 wParam, uint32 lParam);

    // Callback procs for hooking TDialog to native window
    //
    static int CALLBACK StdDlgProc(THandle, uint, TParam1, TParam2);

    // Handle enabling and disabling of child controls
    //
    bool            IdleAction(long idleCount);

  protected:
    // Override virtual functions defined by class TWindow
    //
    void            SetupWindow();
    LPCTSTR          GetClassName();
    void            GetWindowClass(WNDCLASS& wndClass);

    // Registers WINDOW class(es) of Children
    //
    void            RegisterChildObjects();

    // Common initialization in constructoe
    void            Init(TWindow* parent, LPCTSTR title, TModule* module);

  public_data:
    union {
      TDialogAttr  Attr;        // Old name
      TDialogAttr  DialogAttr;  // New name
    };

#if defined(OWL5_COMPAT)
    bool          IsModal;     // Don't use
#endif

  protected_data:
    enum{ 
      // using TWindows::Flags
      //wfInsertAtEdge  = 0x008000 // (Decoration) Window to be inserted against frame's edge
      dfResId           = 0x000000L, // Dialog created from ResId default
      dfNoDeleteHandle  = 0x010000L, // Dialog created from HGLOBAL no delete
      dfDeleteHandle    = 0x020000L, // Dialog created from HGLOBAL delete
      dfNoDeleteTemplate= 0x040000L, // Dialog created from LPDIALOGTEMPLATE no delete
      dfDeleteTemplate  = 0x080000L, // Dialog created from LPDIALOGTEMPLATE delete
      dfIsModal         = 0x100000L, // Dialog Is Modal
      dfModalWindow     = 0x200000L, // Use BeginModal() EndModal()  functionality
    };

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TDialog(const TDialog&);
    TDialog& operator =(const TDialog&);

  DECLARE_RESPONSE_TABLE(TDialog);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TDialog, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TDialog, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TDialog );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

#if defined(OWL5_COMPAT)
//
inline TWindow::THandle 
TDialog::DoCreate()
{
  PerformCreate((int)(void*)DialogAttr.Name);
  return GetHandle();
}
#endif

//
inline uint32
TDialog::SendDlgItemMsg(int ChildId, uint16 Msg, uint16 WParam, uint32 LParam) 
{
  return SendDlgItemMessage(ChildId, Msg, WParam, LParam);
}

//
// Set the result of the message for the dialog's WndProc.
//
inline void
TDialog::SetMsgResult(uint32 result)
{
  PRECONDITION(GetHandle());
  SetWindowLong(DWL_MSGRESULT, result);
}

//
// Return the ID of the default pushbutton.
//
inline uint
TDialog::GetDefaultId() const
{
  PRECONDITION(GetHandle());
  return LoUint16(CONST_CAST(TDialog*,this)->SendMessage(DM_GETDEFID));
}

//
// Return the attributes data member of TDialog.
//
inline TDialogAttr&
TDialog::GetDialogAttr()
{
  return DialogAttr;
}

//
// Set the attributes of the dialog.
//
inline void
TDialog::SetDialogAttr(TDialogAttr attr)
{
  DialogAttr = attr;
}

//
// Return true if the dialog is modal.
//
inline bool
TDialog::GetIsModal() const
{
  return CONST_CAST(TDialog*,this)->IsFlagSet(dfIsModal);//IsModal -> old flag don't use;
}

//
// Set the modal data member of TDialog.
//
inline void
TDialog::SetIsModal(bool ismodal)
{
#if defined(OWL5_COMPAT)
  IsModal = ismodal;// old flag
#endif
  ismodal ? SetFlag(dfIsModal) : ClearFlag(dfIsModal);
}

//
// Enable/disable emulation of ModalDialog by Begin/End modal
//
inline void            
TDialog::EnableModal(bool enable)
{
  enable ? SetFlag(dfModalWindow) : ClearFlag(dfModalWindow);   
}

//
// Set the id of the default push button.
//
inline void
TDialog::SetDefaultId(uint id)
{
  PRECONDITION(GetHandle());
  SendMessage(DM_SETDEFID, id, 0);
}

//
// Convert the dialog units into screen coordinates.
//
inline void
TDialog::MapDialogRect(TRect& rect)
{
  PRECONDITION(GetHandle());
  ::MapDialogRect(GetHandle(), &rect);
}

#if defined(OWL2_COMPAT)
//
inline HWND
TDialog::GetItemHandle(int childId)
{
  PRECONDITION(GetHandle());
  return GetDlgItem(childId);
}
#endif

//
// Reposition the dialog to ensure it is fully visible.
//
inline void
TDialog::Reposition()
{
  PRECONDITION(GetHandle());
  SendMessage(DM_REPOSITION);
}


__OWL_END_NAMESPACE

#endif  // OWL_DIALOG_H
