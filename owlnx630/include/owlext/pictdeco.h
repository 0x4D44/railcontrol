//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// TPictDecorator
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_PICTDECO_H)
#define __OWLEXT_PICTDECO_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                TPictDecorator
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TPictDecorator : public __OWL TWindow
{
  // Object lifetime methods
  //
public:
  TPictDecorator(__OWL TDib* dib, __OWL TWindow& parent);
  virtual ~TPictDecorator();

  // OWL overrides
  //
protected:
  DECLARE_RESPONSE_TABLE(TPictDecorator);
  virtual bool EvEraseBkgnd(HDC hdc);
  virtual void EvSize(uint sizeType, __OWL TSize& size);

  /*
  // This was how the code was originally written; however, in order to give you
  // a demonstration of how to use the TProperty<> mechanism in PROPERTY.H, I've
  // chosen to use the TProperty<> mechanism to control the m_pdib member.

  // Accessors
  //
public:
  __OWL TDib* GetDib()
    { return m_pdib; }

  // Mutators
  //
public:
  __OWL TDib* SetDib(__OWL TDib* newDib);

  // Internal data
  //
private:
  __OWL TDib* m_pdib;
  */

  // Properties
  //
public:
  __OWL TProperty<__OWL TDib*> pDib;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif