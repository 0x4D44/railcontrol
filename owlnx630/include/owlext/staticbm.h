//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// An OWL static bitmap class for dialogs
// Allows highlighting (by drawing a border), aspect resizing,
// and masking (i.e. transparency)
//
// Original code:
// Written by Richard Brown
// Richard@TowerSoftware.com
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
//-------------------------------------------------------------------
#if !defined(__OWLEXT_STATICBM_H)
#define __OWLEXT_STATICBM_H

// Grab the core OWLEXT file we need
//
#if !defined(__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

// Grab some OWL headers that may or may not be included in the OWL/CORE.H file
//
#if !defined(OWL_STATIC_H)
# include <owl/static.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


// -------------------------------------------------------------

class OWLEXTCLASS TStaticBitmap : public __OWL TStatic {
  // Object lifetime methods
public:
  TStaticBitmap(__OWL TWindow* parent, __OWL TBitmap* user_bitmap, __OWL TPalette* user_palette,
      int id, int x, int y, int width, int height, bool flag=false,
      bool use_aspect=false,bool use_mask=true);
  virtual ~TStaticBitmap();

  // Mutators
  //
public:
	void SetText(LPTSTR text);
	void UpdateBitmap(__OWL TBitmap* user_bitmap,int x,int y,int width,int height,
      bool use_mask=true);
	bool IsOver(__OWL TPoint& point);
	void Select(bool flag);

  // OWL overrides
  //
protected:
  void SetupWindow();
  void Paint(__OWL TDC& dc,bool,__OWL TRect&);

  // Internal data
  //
private:
  enum { BORDER_OFFSET=2 };
  __OWL TBitmap*	Bitmap;
  __OWL TBitmap*	MaskBitmap;
  __OWL TPalette* Palette;
  int				X;
  int				Y;
  int				Width;
  int				Height;
  bool			Selected;
  bool			prevSelected;
  bool			UseAspect;
  bool			MaskUsed;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE


// Richard's code originally called this "StaticBitmap"; for consistency's sake
// within the framework, I renamed it to "TStaticBitmap", but provide this typedef
// to make existing code source-compatible. #define NO_STATICBITMAP_TYPEDEF if
// it clashes with another name. Note that this needs to be done outside the
// OwlExt namespace, or the typedef will really be OwlExt::StaticBitmap, which
// doesn't quite mimic the original code.
//
#ifndef NO_STATICBITMAP_TYPEDEF
typedef __OWLEXT TStaticBitmap StaticBitmap;
#endif


#endif


