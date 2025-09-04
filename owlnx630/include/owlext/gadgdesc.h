// ****************************************************************************
// Copyright (C) 1998 by Dieter Windau
// All rights reserved
//
// gadgdesc.h:   header file
// Version:      1.6
// Date:         08.11.1998
// Author:       Dieter Windau
//
// Freeware OWL classes that extents the dockable and gadget system
//
// You are free to use/modify this code but leave this header intact.
// May not be sold for profit.
//
// Tested with Borland C++ 5.02, OWL 5.02, OWL6 patch #3 and with Windows
// NT 4.0 SP3 but I think the class should work with Windows 95 too.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk.
//
// This package contains many classes and source that are based on other OWL
// developers work. Very special thanks to Alan Chambers, Christopher Kohlhoff,
// Jo Parrello, Mark Hatsell, Michael Mogensen and Yura Bidus
//
// Please send me bug reports, bug fixes, enhancements, requests, etc., and
// I'll try to keep this in next versions:
//   EMail: dieter.windau@usa.net
//   Web:   http://members.aol.com/softengage/index.htm
// ****************************************************************************

#if !defined(__OWLEXT_GADGDESC_H)
#define __OWLEXT_GADGDESC_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
  #include <owlext/core.h>
#endif

#if  !defined(OWL_BUTTONGA_H)
#include <owl/buttonga.h>
#endif
#if  !defined(OWL_TEXTGADG_H)
#include <owl/textgadg.h>
#endif
#ifndef   OWL_TIMEGADG_H
#include <owl/timegadg.h>
#endif
#ifndef   OWL_CONTAIN_H
#include <owl/contain.h>
#endif
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif

#if !defined(__OWLEXT_COLPICK_H)
#include <owlext/colpick.h>
#endif

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

// ****************** Forward declaration *************************************

class OWLEXTCLASS TGadgetDesc;
class OWLEXTCLASS TInvisibleGadgetDesc;
class OWLEXTCLASS TButtonGadgetDesc;
class OWLEXTCLASS TButtonTextGadgetDesc;
class OWLEXTCLASS TMenuButtonGadgetDesc;
class OWLEXTCLASS TColorButtonGadgetDesc;
class OWLEXTCLASS TSeparatorGadgetDesc;
class OWLEXTCLASS TTextGadgetDesc;
class OWLEXTCLASS TMenuGadgetDesc;
class OWLEXTCLASS TBitmapGadgetDesc;

class TCommandCategory;
class TButtonGadgetGroup;
class TGadgetDescriptors;

// ************************* Types ********************************************
typedef enum tPopupType {
  Normal,           // Normal button style
                    // - press button -> PopupActionStart()
  DownArrow,        // Normal button style and additional a down arrow
                    // - press button -> PopupActionStart()
  DownArrowAction,  // Separator between left area and down arrow
                    // - press left button area -> Activate()
                    // - press down arrow area -> PopupActionStart()
} TPopupType;

// ******************** TGadgetDesc *******************************************

#define GADG_ALLOPERATION  0
#define GADG_NODELETE      1
#define GADG_NOCOPY        2
#define GADG_NOCUT         4
#define GADG_NOOPERATION   7
#define GADG_ISCOMMAND     8
#define GADG_HASHELP       16
#define GADG_BUTAPPEARANCE 32

class OWLEXTCLASS TGadgetDesc
{
public:
  enum TType {
    InvisibleGadgetDesc,
    ButtonGadgetDesc,
    ButtonTextGadgetDesc,
    MenuButtonGadgetDesc,
    MenuReplaceGadgetDesc, // not yet implemented
    RecentFilesGadgetDesc,
    ColorButtonGadgetDesc,
    SeparatorGadgetDesc,
    ControlGadgetDesc,
    TextGadgetDesc,
    MenuGadgetDesc,
    TimeGadgetDesc,
    BitmapGadgetDesc
  };

public:
  TType Type;
  int   Id;

  uint Attr;
  // Combination of the following values:
  // GADG_ALLOPERATION:  Full customization support
  // GADG_NODELETE:      Gadget can't be deleted from toolbar by user
  // GADG_NOCOPY:        Gadget can't be copied into toolbars. But it can be
  //                     moved from one toolbar to another. The gadget exist
  //                     not more then one time in the system
  // GADG_NOCUT:         Gadget can't be cut/paste or moved from one toolbar to
  //                     another toolbar
  // GADG_NOOPERATION:   Combination of CUST_NODELETE, CUST_NOCOPY, CUST_NOCUT
  // GADG_ISCOMMAND:     Gadget will be shown in "Customize" dialog on
  //                     "Command" Page with defined category
  // GADG_HASHELP:       There exist a help page for the gadget. This is only
  //                     relevant, if you use my TCtxHelpFileMananger.
  // GADG_BUTAPPEARANCE: You can change the bitmap and/or text. This is only
  //                     relevant for all gadgets derived from TButtonGadget.

public:
  TGadgetDesc(TType type, int id, uint attr);
  virtual ~TGadgetDesc() {}

  TType GetType() { return Type; }
  int   GetId()   { return Id; }
  uint  GetAttr() { return Attr; }

  bool  IsPopupButtonGadget()
    { return (Type == MenuButtonGadgetDesc ||
              Type == MenuReplaceGadgetDesc ||
              Type == RecentFilesGadgetDesc ||
              Type == ColorButtonGadgetDesc) ? true : false;
    }

  bool operator ==(const TGadgetDesc& gd) const
    { return (Id == gd.Id) ? true : false; }

public:
  virtual __OWL TGadget* ConstructGadget() = 0;
};

// ******************** TInvisibleGadgetDec ***********************************

class OWLEXTCLASS TInvisibleGadgetDesc: public TGadgetDesc
{
public:
  TInvisibleGadgetDesc();
  virtual ~TInvisibleGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TButtonGadgetDesc *************************************

class OWLEXTCLASS TButtonGadgetDesc: public TGadgetDesc
{
public:
  uint32                BmpResId;
  __OWL TButtonGadget::TType  ButtonGadgetType;
  bool                  Enabled;
  __OWL TButtonGadget::TState ButtonGadgetState;
  bool                  SharedGlyph;

public:
  TButtonGadgetDesc(
    uint32 bmpResId,
    int    id,
    __OWL TButtonGadget::TType buttonGadgetType=__OWL TButtonGadget::Command,
    bool   enabled = false,
    __OWL TButtonGadget::TState buttonGadgetState=__OWL TButtonGadget::Up,
    bool   sharedGlyph = false,
    uint   attr = GADG_ALLOPERATION | GADG_HASHELP | GADG_BUTAPPEARANCE
  );
  virtual ~TButtonGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TButtonTextGadgetDesc *********************************

typedef enum tDisplayType {
  Bitmap,
  Text,
  BitmapText
} TDisplayType;

class OWLEXTCLASS TButtonTextGadgetDesc: public TButtonGadgetDesc
{
public:
  owl_string       Text;
  TDisplayType DispType;

public:
  TButtonTextGadgetDesc(
    LPCTSTR      text,
    TDisplayType disptype,
    uint32       bmpResId,
    int          id,
    __OWL TButtonGadget::TType buttonGadgetType=__OWL TButtonGadget::Command,
    bool         enabled = false,
    __OWL TButtonGadget::TState buttonGadgetState=__OWL TButtonGadget::Up,
    bool         sharedGlyph = false,
    uint         attr = GADG_ALLOPERATION | GADG_HASHELP | GADG_ISCOMMAND |
                        GADG_BUTAPPEARANCE
  );

  virtual ~TButtonTextGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();

};

// ******************** TMenuButtonGadgetDesc ********************************

class OWLEXTCLASS TMenuButtonGadgetDesc: public TButtonTextGadgetDesc
{
public:
  TPopupType  PopupType;
  HMENU       hMenu;
  __OWL TWindow*    CmdTarget;

public:
  TMenuButtonGadgetDesc(
    TPopupType   popupType,
    HMENU        hmenu,
    __OWL TWindow*     cmdTarget,
    LPCTSTR      text,
    TDisplayType disptype,
    uint32       bmpResId,
    int          id,
    __OWL TButtonGadget::TType buttonGadgetType=__OWL TButtonGadget::Command,
    bool         enabled = false,
    __OWL TButtonGadget::TState buttonGadgetState=__OWL TButtonGadget::Up,
    bool         sharedGlyph = false,
    uint         attr = GADG_ALLOPERATION | GADG_HASHELP | GADG_ISCOMMAND |
                        GADG_BUTAPPEARANCE
  );

  virtual ~TMenuButtonGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TRecentFilesGadgetDesc ********************************

class OWLEXTCLASS TRecentFilesGadgetDesc: public TButtonTextGadgetDesc
{
public:
  TRecentFilesGadgetDesc(
    LPCTSTR      text,
    TDisplayType disptype,
    uint32       bmpResId,
    int          id,
    __OWL TButtonGadget::TType buttonGadgetType=__OWL TButtonGadget::Command,
    bool         enabled = false,
    __OWL TButtonGadget::TState buttonGadgetState=__OWL TButtonGadget::Up,
    bool         sharedGlyph = false,
    uint         attr = GADG_ALLOPERATION | GADG_HASHELP | GADG_ISCOMMAND |
                        GADG_BUTAPPEARANCE
  );

  virtual ~TRecentFilesGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TColorButtonGadgetDesc ********************************

class OWLEXTCLASS TColorButtonGadgetDesc: public TButtonTextGadgetDesc
{
public:
  TPopupType     PopupType;
  __OWL TColor         StartColorSel;
  TColorPickerData Data;
  __OWL TRect*         FillRect;
  __OWL TWindow*       ParentWindow;

public:
  TColorButtonGadgetDesc(
    TPopupType   popupType,
    TColorPickerData& data,
    __OWL TColor       startColorSel,
    __OWL TRect*       fillRect,
    __OWL TWindow*     parentWindow,
    LPCTSTR      text,
    TDisplayType disptype,
    uint32       bmpResId,
    int          id,
    __OWL TButtonGadget::TType buttonGadgetType=__OWL TButtonGadget::Command,
    bool         enabled = false,
    __OWL TButtonGadget::TState buttonGadgetState=__OWL TButtonGadget::Up,
    bool         sharedGlyph = false,
    uint         attr = GADG_ALLOPERATION | GADG_HASHELP | GADG_ISCOMMAND
  );

  virtual ~TColorButtonGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TSeperatorGadgetDesc **********************************

class OWLEXTCLASS TSeparatorGadgetDesc: public TGadgetDesc
{
public:
  int  Size;

public:
  TSeparatorGadgetDesc(int  size = 0,
                       int  id = 0);
  virtual ~TSeparatorGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TControlGadgetDesc ************************************

class OWLEXTCLASS TControlGadgetDesc: public TGadgetDesc
{
public:
  __OWL TWindow*              Control;
  __OWL TGadget::TBorderStyle GadgetBorderStyle;

public:
  TControlGadgetDesc(__OWL TWindow& control,
                     __OWL TGadget::TBorderStyle gadgetBorderStyle = __OWL TGadget::None,
                     uint attr = GADG_NOCOPY
// coming soon!!!!!  GADG_ALLOPERATION | GADG_HASHELP | GADG_ISCOMMAND
  );
  virtual ~TControlGadgetDesc();

  void    CountAdd() { Count++; }
  void    CountSub() { Count--; }

public:
  virtual __OWL TGadget* ConstructGadget();

protected:
  int     Count;     // Count in how much windows the control is inserted
};

// ******************** TTextGadgetDesc ***************************************

class OWLEXTCLASS TTextGadgetDesc: public TGadgetDesc
{
public:
  __OWL TGadget::TBorderStyle GadgetBorderStyle;
  __OWL TTextGadget::TAlign   TextGadgetAlign;
  uint                  NumChars;
  owl_string                Text;

public:
  // Don't initialize id with 0, because this is reserved for SeperatorGadget
  //
  TTextGadgetDesc(int id,
                  __OWL TGadget::TBorderStyle gadgetBorderStyle = __OWL TGadget::Recessed,
                  __OWL TTextGadget::TAlign textGadgetAlign = __OWL TTextGadget::Left,
                  uint    numChars = 10,
                  LPCTSTR text = 0,
                  uint    attr = GADG_ALLOPERATION);
  virtual ~TTextGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TMenuGadgetDesc ***************************************

class OWLEXTCLASS TMenuGadgetDesc: public TGadgetDesc
{
public:
  __OWL TMenu*                Menu;
  __OWL TWindow*              Window;
  __OWL TGadget::TBorderStyle GadgetBorderStyle;
  owl_string                Text;
  __OWL TFont*                Font;

public:

  // Don't initialize id with 0, because this is reserved for SeperatorGadget
  //
  TMenuGadgetDesc(__OWL TMenu*   menu,
                  __OWL TWindow* window,
                  int      id,
                  __OWL TGadget::TBorderStyle gadgetBorderStyle = __OWL TGadget::ButtonUp,
                  LPCTSTR  text = 0,
                  __OWL TFont*   font = new __OWL TGadgetWindowFont,
                  uint     attr = GADG_ALLOPERATION);
  virtual ~TMenuGadgetDesc();

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TTimeGadgetDesc ***************************************

class OWLEXTCLASS TTimeGadgetDesc: public TTextGadgetDesc
{
public:
  __OWL TTimeGadget::TGetTimeFunc TimeFunc;
  __OWL TFont*                    Font;

public:

  // Don't initialize id with 0, because this is reserved for SeperatorGadget
  //
  TTimeGadgetDesc(__OWL TTimeGadget::TGetTimeFunc timeFunc,
                  int      id,
                  __OWL TGadget::TBorderStyle gadgetBorderStyle = __OWL TGadget::Recessed,
                  __OWL TTextGadget::TAlign textGadgetAlign = __OWL TTextGadget::Center,
                  uint     numChars = 12,
                  LPCTSTR  text = 0,
                  __OWL TFont*   font = new __OWL TGadgetWindowFont,
                  uint     attr = GADG_ALLOPERATION);
  virtual ~TTimeGadgetDesc();

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TBitmapGadgetDesc *************************************

class OWLEXTCLASS TBitmapGadgetDesc: public TGadgetDesc
{
public:
  __OWL TResId                ImageResIdOrIndex;
  __OWL TGadget::TBorderStyle GadgetBorderStyle;
  int                   NumImages;
  int                   StartImage;
  bool                  SharedCels;

public:
  // Don't initialize id with 0, because this is reserved for SeperatorGadget
  //
  TBitmapGadgetDesc(__OWL TResId imageResIdOrIndex,
                    int    id,
                    __OWL TGadget::TBorderStyle gadgetBorderStyle,
                    int    numImages,
                    int    startImage = 0,
                    bool   sharedCels = false,
                    uint   attr = GADG_ALLOPERATION);
  virtual ~TBitmapGadgetDesc() {}

public:
  virtual __OWL TGadget* ConstructGadget();
};

// ******************** TCommandCategory **************************************

class OWLEXTCLASS TCommandCategory
// encapsulate information about the gadgets in a command category
{
  public:
    TCommandCategory(LPCTSTR name);

    void CheckConsistence();
    // Delete all duplicate id's in this group

    bool operator ==(const TCommandCategory& g) const;

  public:
    __OWL TIntArray Array;
    owl_string Name;
};

// ******************** TButtonGadgetGroup ************************************

class OWLEXTCLASS TButtonGadgetGroup
// encapsulate information about a button gadget group
{
  public:
    TButtonGadgetGroup(__OWL TButtonGadget::TType buttonGadgetType);

    void CheckConsistence();
    // Delete all duplicate id's in this group

    bool operator ==(const TButtonGadgetGroup& g) const;

  public:
    __OWL TIntArray Array;
    __OWL TButtonGadget::TType    ButtonGadgetType;
};

// ******************** TGadgetDescriptors ************************************

class OWLEXTCLASS TGadgetDescriptors
{
public:
  TGadgetDescriptors();

  __OWL TGadget*            ConstructGadget(int id);
  TGadgetDesc*        Find(int id);
  TButtonGadgetGroup* FindGroup(int id);

  // return the first category where a gadget with id is found
  //
  TCommandCategory*   FindCategory(int id);

  // return the category with name
  //
  TCommandCategory*   FindCategory(owl_string name);

  void CheckConsistence();
  // Checks the consistence of all TButtonGadgetDesc, TButtonGadgetGroup and
  // TCommandCategory. Should be called after all members are added

public:
  __OWL TIPtrArray<TGadgetDesc*>        Array;
  __OWL TIPtrArray<TButtonGadgetGroup*> Groups;
  __OWL TIPtrArray<TCommandCategory*>   Categories;
};

extern OWLEXTDATA(TGadgetDescriptors*) GadgetDescriptors;

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>
OWLEXT_END_NAMESPACE


#endif
