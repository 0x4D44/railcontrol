//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26.4.1 $
//$Author: jogybl $
//$Date: 2010-02-23 10:44:57 $
//
//  Class definition for TBarDescr.
//
//----------------------------------------------------------------------------

#if !defined(OWL_BARDESCR_H)  // Sentry, use file only if it's not already included.
#define OWL_BARDESCR_H

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

class TResId;
class _OWLCLASS TGadget;
class _OWLCLASS TGadgetWindow;
class _OWLCLASS TCelArray;

template <class R, class T1, class T2, class T3>  class TFunctor3;
// TGadgetFunctor is a ptr to a fn taking 4 parameters, and returning TGadget*
typedef TFunctor3<TGadget*,int,int, bool&> TGadgetFunctor;

#if defined(BI_HAS_MEMBER_TEMPLATE)
#  define TGadget_FUNCTOR(func) TFunctionTranslator3<TGadget*,int,int, bool&, TGadget* (*)(int,int, bool&)>(func)
#  define TGadget_MFUNCTOR(obj,memberFunc) TMemberTranslator3<TGadget*,int,int, bool&, TGadget*(obj::*)(int,int, bool&)>(obj, memberFunc);
#else
#  define TGadget_FUNCTOR(func) \
          Functor((TGadgetFunctor*)0, func)
#  define TGadget_MFUNCTOR(obj,memberFunc) \
          Functor((TGadgetFunctor*)0, obj, memberFunc)
#endif
//
// static TGadget* MyHandle(int cellIdx,int cmdId,bool& usecell)
//{
//  usecell == false;
//  if(cmdId==0)
//     return new TSeparatorGadget(6);
//  usecell == true;
//  return new TButtonGadget(index, id, TButtonGadget::Command,false,TButtonGadget::Up,true);
//}
// Funct = TGadget_FUNCTOR(MyHandle)  - function
//
// or:
//
// TGadget* TMyWindow::MyHandle(int cellIdx,int cmdId,bool& usecell)
//{
//  usecell == false;
//  if(cmdId==0)
//     return new TSeparatorGadget(6);
//  usecell == true;
//  return new TButtonGadget(index, id, TButtonGadget::Command,false,TButtonGadget::Up,true);
//}
// Funct = TGadget_MFUNCTOR(myWndPtr,&TMyWindow::MyHandle)  - member function
// Funct = TGadget_MFUNCTOR(this,&TMyWindow::MyHandle)      - member function
//


// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

// Descriptor of Bar Implementation 
// class TBarDescr
//
class TBarDescrGdArray;
class _OWLCLASS TBarDescr {
  public:
    enum TGroup {
      FileGroup,
      EditGroup,
      ContainerGroup,
      ObjectGroup,
      WindowGroup,
      HelpGroup,
      NumGroups
    };
  public:
    TBarDescr(); // for internal use
    TBarDescr(TResId id, int fg, int eg, int cg, int og, int wg, int hg,
               TModule* module = gModule);
    TBarDescr(TResId id, TModule* module = gModule);
    virtual ~TBarDescr();

    TBarDescr& operator =(const TBarDescr& original);

    bool      SetBitmap(const TResId newResId, TModule* module);
    TResId  GetId() const;
    int     GetGroupCount(int group) const;
    bool    RemoveGadgets(TGadgetWindow& destWindow);
    void    SetBuilder(const TGadgetFunctor& functor);
    TGadgetFunctor* GetBuilder();

    virtual bool  Create(TGadgetWindow& destWindow);
    virtual bool  Merge(const TBarDescr& sourceBarDescr, TGadgetWindow& destWindow);
    virtual bool  Restore(TGadgetWindow& destWindow);

    // please use: SetBuilder(TGadgetFunctor& functor);
    // properties
#if 0  // deprecated
    class TGadgetBuilder{
      private:
        TBarDescr* GetParent();
      public:
        TGadgetBuilder& operator=(const TGadgetFunctor& funct)
        {
          GetParent()->SetBuilder(funct);
          return *this;
        }
    }GadgetBuilder;
#endif

  protected:
    TBarDescr(const TBarDescr& node);
    bool    ExtractGroups();

  protected_data:
    int                   GroupCount[NumGroups];
    TToolbarRes*          BarRes;
    TCelArray*            CelArray;
    TResId                Id;           // resource id of bar
    TModule*              Module;       // module
    TGadgetFunctor*        GadgetFunctor;

  private:
    TBarDescrGdArray* Gadgets; // internal usage

  friend _OWLCFUNC(ipstream&) operator >>(ipstream& is, TBarDescr& m);
  friend _OWLCFUNC(opstream&) operator <<(opstream& os, const TBarDescr& m);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// inlines
//
inline TResId TBarDescr::GetId() const {
  return Id;
}

//
inline int TBarDescr::GetGroupCount(int group) const {
  return GroupCount[group];
}
//
//
//
inline TGadgetFunctor* TBarDescr::GetBuilder(){
  return GadgetFunctor;
}
//
//
#if 0  // deprecated
inline TBarDescr* TBarDescr::TGadgetBuilder::GetParent(){
  //return reinterpret_cast<TBarDescr*>((char*)(void*)this - _OFFSETOF(TBarDescr,GadgetBuilder));
  return (TBarDescr*)((char*)this - (int)_OFFSETOF(TBarDescr,GadgetBuilder));
  //return (TBarDescr*)((char*)this-(int)(&((TBarDescr*)0)->GadgetBuilder));
}
#endif
//
//

__OWL_END_NAMESPACE


#endif // __bardescr_h sentry.


