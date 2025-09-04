//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:48 $
//
// Implementation of class TOleMDIFrame.
//----------------------------------------------------------------------------
#define INC_OLE2
#include <ocf/pch.h>

#if !defined(OWL_DECMDIFR_H)
# include <owl/decmdifr.h>
#endif
#if !defined(OWL_STATUSBA_H)
# include <owl/statusba.h>
#endif
#if !defined(OCF_OCFEVENT_H)
# include <ocf/ocfevent.h>
#endif
#if !defined(OCF_OLEMDIFR_H)
# include <ocf/olemdifr.h>
#endif

__OCF_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

__OWL_USING_STD_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OcfOleMenu);

DEFINE_RESPONSE_TABLE2(TOleMDIFrame, TOleFrame, TMDIFrame)
  EV_WM_ACTIVATEAPP,
  EV_OC_APPINSMENUS,
END_RESPONSE_TABLE;

//
//
//
TOleMDIFrame::TOleMDIFrame(LPCTSTR 			title,
                           TResId       menuResId,
                           TMDIClient&  clientWnd,
                           bool         trackMenuSelection,
                           TModule*     module)
:
  TOleFrame(title, &clientWnd, trackMenuSelection, module),
  TMDIFrame(title, menuResId, clientWnd, module)
//!CQ when did this change?
//  ,TFrameWindow(0, title, &clientWnd, false, module),
//  TWindow(0, title, module)
{
}

//
//
//
TOleMDIFrame::~TOleMDIFrame()
{
}

//
// Forward Activate messages to OcApp to allow it to notify any embedded servers
// about being activated
//
void
TOleMDIFrame::EvActivateApp(bool active, HTASK hTask)
{
  OcApp->EvActivate(active);
  TMDIFrame::EvActivateApp(active, hTask);
}

//
// Insert our menus into a provided menu bar, possibly merging them with a
// child and servers.
//
bool
TOleMDIFrame::EvOcAppInsMenus(TOcMenuDescr & sharedMenu)
{
  if (HOldMenu)
    return true;

  // Recreate a temporary composite menu for frame and MDI child
  //
  TMenuDescr compMenuDesc; // empty menudescr
  if (GetMenuDescr())
    compMenuDesc.Merge(*GetMenuDescr()); 
	// explisit conversion for VC++, for Borland it is the same, 05/26/98 Yura Bidus 
#if defined(BI_NO_COVAR_RET)
	TMDIClient* mdiClient = TYPESAFE_DOWNCAST(GetClientWindow(),TMDIClient);
  const TMenuDescr* childMenu = mdiClient? mdiClient->GetActiveMDIChild()->GetMenuDescr() : 0;
#else
  const TMenuDescr* childMenu = GetClientWindow()->GetActiveMDIChild()->GetMenuDescr();
#endif
  if (childMenu)
    compMenuDesc.Merge(*childMenu);

  // Mask off the server menus
  //
  compMenuDesc.Merge(TMenuDescr(0,  0, -1, 0, -1, 0, -1));

  // Merge into the OLE shared menubar
  //
  TMenuDescr shMenuDescr(sharedMenu.HMenu,
                         sharedMenu.Width[0],
                         sharedMenu.Width[1],
                         sharedMenu.Width[2],
                         sharedMenu.Width[3],
                         sharedMenu.Width[4],
                         sharedMenu.Width[5]);
  shMenuDescr.Merge(compMenuDesc);

  // Copy the shared menu widths back to the OC struct
  //
  for (int i = 0; i < 6; i++)
    sharedMenu.Width[i] = shMenuDescr.GetGroupCount(i);

  // Save the container popups so they can be destroyed later
  //
  StashContainerPopups(shMenuDescr);

  TRACEX(OcfOleMenu, 0, _T("MDI merged menu ") << hex << (uint)sharedMenu.HMenu);
  return true;
}

//
//
//
TResult
TOleMDIFrame::DefWindowProc(uint message, TParam1 param1, TParam2 param2)
{
  //
  // ::DefFrameProc() will response to WM_SIZE by making the MDI client the
  // same size as the client rectangle; this conflicts with what TLayoutWindow
  // has done
  //
  return message == WM_SIZE ?
                      0 :
                      TMDIFrame::DefWindowProc(message, param1, param2);
}

__OCF_END_NAMESPACE

//==============================================================================

