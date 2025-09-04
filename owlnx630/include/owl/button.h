//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TButton. This defines the basic behavior of all buttons.
//----------------------------------------------------------------------------

#if !defined(OWL_BUTTON_H)
#define OWL_BUTTON_H

#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
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
// class TButton
// ~~~~~ ~~~~~~~
class _OWLCLASS TButton : public TControl {
  public:
    //For use with SetImage method.
    enum TImageType { Bitmap=IMAGE_BITMAP, Icon=IMAGE_ICON };

    TButton(TWindow*        parent,
            int             id,
            LPCTSTR         text,
            int X, int Y, int W, int H,
            bool            isDefault = false,
            TModule*        module = 0);

    TButton(TWindow* parent, int resourceId, TModule* module = 0);
   ~TButton();

    bool  GetIsDefPB() const;
    void  SetIsDefPB(bool isdefpb);

    bool  GetIsCurrentDefPB() const;
    void  SetIsCurrentDefPB(bool is);

    uint  GetState() const;
    void  SetState(uint state);

    void  SetStyle(uint style, bool redraw);

    void   Click();

    // image can be HICON or HBITMAP
    //
    HANDLE GetImage() const;
    HANDLE SetImage(HANDLE image, TImageType imageType = Bitmap);

  protected:
    // Message response functions
    //
    uint      EvGetDlgCode(MSG *);
    TResult   BMSetStyle(TParam1, TParam2);

    // Override TWindow member functions
    //
    LPCTSTR   GetClassName();
    void      SetupWindow();

  public_data:
    bool  IsDefPB;

  protected_data:
    bool  IsCurrentDefPB;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TButton(const TButton&);
    TButton& operator=(const TButton&);

  DECLARE_RESPONSE_TABLE(TButton);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TButton, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TButton, 1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TButton );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// button notification response table entry macros, methods are: void method()
//
// EV_BN_CLICKED(id, method)
// EV_BN_DISABLE(id, method)
// EV_BN_DOUBLECLICKED(id, method)
// EV_BN_HILITE(id, method)
// EV_BN_PAINT(id, method)
// EV_BN_UNHILITE(id, method)

//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return true if this button is the default pushbutton.
//
inline bool TButton::GetIsDefPB() const
{
  return IsDefPB;
}

//
// Set this button the default pushbutton.
//
inline void TButton::SetIsDefPB(bool isdefpb)
{
  IsDefPB = isdefpb;
}

//
// Return true if this button is the current default pushbutton.
//
inline bool TButton::GetIsCurrentDefPB() const
{
  return IsCurrentDefPB;
}

//
// Set this button the current default push button.
//
inline void TButton::SetIsCurrentDefPB(bool is)
{
  IsCurrentDefPB = is;
}

//
// Simulate clicking of this button
//
inline void TButton::Click()
{
  PRECONDITION(GetHandle());
  SendMessage(BM_CLICK);
}

//
// Return the image associated with the button.
//
inline HANDLE TButton::GetImage() const
{
  PRECONDITION(GetHandle());
  return (HANDLE)CONST_CAST(TButton*, this)->SendMessage(BM_GETIMAGE);
}

//
// Set the associated image for this button.
//
inline HANDLE TButton::SetImage(HANDLE newImage, TImageType imageType)
{
  PRECONDITION(GetHandle());
  return (HANDLE)SendMessage(BM_SETIMAGE, (TParam1)imageType, (TParam2)newImage);
}

//
// Set the display state of the button.
//
inline void TButton::SetState(uint state)
{
  PRECONDITION(GetHandle());
  SendMessage(BM_SETSTATE, state);
}

//
//
inline uint TButton::GetState() const
{
  PRECONDITION(GetHandle());
  return (uint)CONST_CAST(TButton*, this)->SendMessage(BM_GETSTATE);
}

//
inline void TButton::SetStyle(uint style, bool redraw)
{
  PRECONDITION(GetHandle());
  SendMessage(BM_SETSTYLE, style, MkParam2(redraw,0));
}


__OWL_END_NAMESPACE


#endif  // OWL_BUTTON_H

