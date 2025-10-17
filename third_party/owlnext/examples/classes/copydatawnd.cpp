//
// OWLNext WM_COPYDATA Demo
// This code demonstrates sending and receiving the WM_COPYDATA message.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#include "pch.h"
#include "copydatawnd.h"

#include "resource.h"

using namespace owl;

//
// Data to be copied between windows/applications
//
struct TCopyData
{
  tchar Message[256];
};

//
// This enum defines the types of data this application knows about.
//
enum TCopyDataId
{
  cdidCopyData = 423141 // TCopyData message (random id to reduce likelihood of clashes)
};


DEFINE_RESPONSE_TABLE1(TCopyDataWindow, TListBox)
EV_COMMAND(CM_TESTCOPYDATA, CmCopyData),
EV_WM_COPYDATA,
END_RESPONSE_TABLE;


TCopyDataWindow::TCopyDataWindow(TWindow* parent)
  : TListBox(parent, 1, 0, 0, 100, 100)
{
  Attr.Style &= ~LBS_SORT;
}

void TCopyDataWindow::SetupWindow()
{
  TListBox::SetupWindow();

  AddString(_T("Use the Test menu to send a WM_COPYDATA message to self."));
}

TCopyDataWindow::~TCopyDataWindow()
{
}

void TCopyDataWindow::CmCopyData()
{
  TCopyData d = { _T("Hello! This WM_COPYDATA message was sent from application to itself.") };
  COPYDATASTRUCT c = { cdidCopyData, sizeof d, &d };
  TResult r = SendMessage(WM_COPYDATA,
    reinterpret_cast<TParam1>(GetHandle()),
    reinterpret_cast<TParam2>(&c));
  tstring m = r ? _T("The message was handled.") : _T("The message was not handled.");
  //MessageBox(m, _T("WM_COPYDATA Sender"));
  AddString(m);
}

//
// Returns true if the message is handled.
//
bool TCopyDataWindow::EvCopyData(HWND sender, const COPYDATASTRUCT& c)
{
  PRECONDITION(sender == GetHandle());
  const tchar title[] = _T("WM_COPYDATA Receiver");
  switch (c.dwData)
  {
    case cdidCopyData:
    {
      CHECK(c.cbData == sizeof TCopyData);
      const TCopyData& d = *reinterpret_cast<const TCopyData*>(c.lpData);
      AddString(d.Message);
      //MessageBox(d.Message, title);
      return true;
    }
    default:
    {
      AddString(_T("Unknown message."));
      //MessageBox(_T("Unknown message."), title);
      return false;
    }
  }
}


