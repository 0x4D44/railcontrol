//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of class TMetaFileDC, a Metafile DC encapsulation
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Create a regular metafile DC.
//
TMetaFileDC::TMetaFileDC(LPCTSTR filename)
:
  TDC(),
  Enhanced(false)
{
  Handle = ::CreateMetaFile(filename);
  CheckValid();
}

//
// Destroy the metafile.
//
TMetaFileDC::~TMetaFileDC()
{
  if (Handle) {
    if (ShouldDelete)
      // Do not restore objects, Metafiles cleanup after themselves
      //
      if (IsEnhanced())
        ::DeleteEnhMetaFile(CloseEnh());
      else
        ::DeleteMetaFile(Close());
    Handle = 0;
  }
}

//
// Create an enhanced metafile dc.
//
TMetaFileDC::TMetaFileDC(const TDC& dc, LPCTSTR filename, TRect* rect,
                         LPCTSTR description)
:
  TDC(),
  Enhanced(true)
{
  Handle = ::CreateEnhMetaFile(dc, filename, rect, description);
  CheckValid();
}

__OWL_END_NAMESPACE
/* ========================================================================== */

