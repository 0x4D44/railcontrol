//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// Provides TGaugeGadget, a gadget multiply-derived from TGauge and TGadget,
// for use in Gadget Windows.
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
//
// Original code:
// Copyright (c) 1997 Rich Goldstein, MD
// goldstei@interport.net
// May be used without permission, if appropriate credit is given
//
//-------------------------------------------------------------------
#if !defined(__OWLEXT_GAUGEGAD_H)
#define __OWLEXT_GAUGEGAD_H

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H)
# include <owlext/core.h>
#endif

// Grab other required OWL headers
//
#if !defined(OWL_GAUGE_H)
# include <owl/gauge.h>
#endif
#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


class OWLEXTCLASS TGaugeGadget : public __OWL TGadget, public __OWL TGauge
{
  // Object lifetime methods
  //
public:
  TGaugeGadget(int id, LPCTSTR title, int width, TBorderStyle = None);
  ~TGaugeGadget();

protected:
  virtual void Paint(__OWL TDC& dc, bool erase, __OWL TRect& rect);
      // Overriden to change the font to the GagdetWindow font
  virtual void Paint(__OWL TDC& dc)
    { __OWL TGadget::Paint(dc); }

  virtual void Created();
  virtual void Inserted();
  virtual void Removed();

  virtual void InvalidateRect(const __OWL TRect& rect, bool erase = true);
  virtual void Update();
      // Paint now if possible
  virtual void SetBounds(const __OWL TRect& rect);

  virtual void GetDesiredSize(__OWL TSize& size);
      // Computes the area excluding the borders and margins

  // Internal data
  //
private:
  int _width;
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


#endif // GSC_GAUGEGAD_H


