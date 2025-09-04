//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:17 $
//
// Implementation of class TBarDescr
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_RESOURCE_H)
# include <owl/resource.h>
#endif
#if !defined(OWL_CELARRAY_H)
# include <owl/celarray.h>
#endif
#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
#endif
#if !defined(OWL_BUTTONGA_H)
# include <owl/buttonga.h>
#endif
#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_FUNCTOR_H)
# include <owl/functor.h>
#endif
#if !defined(OWL_BARDESCR_H)
#include <owl/bardescr.h>
#endif
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif
#if  !defined(OWL_FUNCTOR_H)
#include <owl/functor.h>
#endif
#if !defined(OWL_GLYPHBTN_H)
# include <owl/glyphbtn.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

struct __TBarDescrGdNode {
  __TBarDescrGdNode(TGadget* gd, bool ug): Gadget(gd),UseGlyph(ug){}
  ~__TBarDescrGdNode();
  TGadget*  Gadget;
  bool      UseGlyph;
};
//
__TBarDescrGdNode::~__TBarDescrGdNode()
{
  delete Gadget;
}
//
class TBarDescrGdArray:  public TIPtrArray<__TBarDescrGdNode*>{ // internal usage
  public:
    TBarDescrGdArray(int count):TIPtrArray<__TBarDescrGdNode*>(count){}
};

//
//
//
static TGadget* CreateGadget(int index, int id, bool& usecell)
{
  usecell = false;
  if(id == 0)
    return new TSeparatorGadget;
  usecell = true;
  return new TButtonGadget(index, 
                           id, 
                           TButtonGadget::Command,
                           false,
                           TButtonGadget::Up,true);
}

// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//  class TBarDescr
//  ~~~~~ ~~~~~~~~~
TBarDescr::TBarDescr()
:
  BarRes(0),
  CelArray(0),
  Id(0),
  Module(0)
{
  GadgetFunctor = new TGadgetFunctor;
  SetBuilder(TGadget_FUNCTOR(CreateGadget));
  Gadgets = new TBarDescrGdArray(5);
  for (int i = 0; i < NumGroups; i++)
    GroupCount[i] = 0;

}
// -----------------------------------------------------------------------------
TBarDescr::TBarDescr(const TBarDescr& bar)
:
  Id(bar.Id),
  Module(bar.Module)
{
  GadgetFunctor = new TGadgetFunctor;
  SetBuilder(TGadget_FUNCTOR(CreateGadget));

  Gadgets = new TBarDescrGdArray(5);
  BarRes = new TToolbarRes(*bar.BarRes);

  CHECK(BarRes->IsOK());
  { // map colors
    TBtnBitmap bitmap(BarRes->GetBitmap(), 
                      TBtnBitmap::DefaultFaceColor, 
                      NoAutoDelete);
  }
  CelArray = new TCelArray(&BarRes->GetBitmap(),
                           BarRes->GetCount(),
                           TSize(BarRes->GetWidth(), BarRes->GetHeight()), 
                           0, 
                           BarRes->GetBitmap().Height()/BarRes->GetHeight());
  ExtractGroups();
}
// -----------------------------------------------------------------------------
TBarDescr::TBarDescr(TResId barResId, TModule* module)
:
  Id(barResId),
  Module(module),
  BarRes(0),
  CelArray(0)
{
  GadgetFunctor = new TGadgetFunctor;
  SetBuilder(TGadget_FUNCTOR(CreateGadget));

  Gadgets = new TBarDescrGdArray(5);

  SetBitmap(barResId, module);

  ExtractGroups();
}
// -----------------------------------------------------------------------------
TBarDescr::TBarDescr(TResId id, int fg, int eg, int cg, int og, int wg, 
                     int hg, TModule* module)
:
  Id(id),
  Module(module),
  BarRes(0),
  CelArray(0)
{
  GadgetFunctor = new TGadgetFunctor;
  SetBuilder(TGadget_FUNCTOR(CreateGadget));

  Gadgets = new TBarDescrGdArray(5);
  
  SetBitmap(id, module);
  
  if (!ExtractGroups()) {
    GroupCount[FileGroup]       = fg;
    GroupCount[EditGroup]       = eg;
    GroupCount[ContainerGroup]   = cg;
    GroupCount[ObjectGroup]     = og;
    GroupCount[WindowGroup]     = wg;
    GroupCount[HelpGroup]       = hg;
  }
}
// -----------------------------------------------------------------------------
TBarDescr::~TBarDescr()
{
  delete Gadgets;
  delete BarRes;
  delete CelArray;
  delete GadgetFunctor;
}
//
void TBarDescr::SetBuilder(const TGadgetFunctor& functor)
{
  *GadgetFunctor = functor;
}
// -----------------------------------------------------------------------------
bool
TBarDescr::SetBitmap(const TResId newResId, TModule* module)
{
  delete BarRes;
  BarRes = 0;
  Module = module;

  // Load toolbar resource
  // NOTE: Don't let TToolbarRes own bitmap, we'll give it to TCelArray
  //       instead.
  BarRes = new TToolbarRes(module ? module->GetHandle() : 0, 
                           newResId, 
                           NoAutoDelete);

  if (BarRes->IsOK()){
    delete CelArray;
    CelArray = 0;
    { // map colors
      TBtnBitmap bitmap(BarRes->GetBitmap(), 
                        TBtnBitmap::DefaultFaceColor, 
                        NoAutoDelete);
    }
    CelArray = new TCelArray(&BarRes->GetBitmap(), BarRes->GetCount(), 
                             TSize(BarRes->GetWidth(), BarRes->GetHeight()), 
                             0, 
                             BarRes->GetBitmap().Height()/BarRes->GetHeight());
    return true;
  }
  return false;
}
//
static void CreateGadgets(TBarDescrGdArray* array, const TGadgetFunctor& builder, 
                          TToolbarRes* barRes)
{
  for (int i=0,j=0; i < barRes->GetCount(); i++) {
    if(barRes->GetIds()[i] == uint16(-1))
       continue;

    bool usecell;
    TGadget* gd = (builder)(j, barRes->GetIds()[i], usecell);
    array->Add(new __TBarDescrGdNode(gd,usecell));
    
    if(usecell)
      j++;
  }
}
// -----------------------------------------------------------------------------
bool
TBarDescr::Create(TGadgetWindow& destWindow)
{
  // Build toolbar from resource and from descriptor string
  if (BarRes && BarRes->IsOK()) {
    if(!Gadgets->Size())
      CreateGadgets(Gadgets, *GetBuilder(), BarRes);

    int numRows = BarRes->GetBitmap().Height()/BarRes->GetHeight();
    TCelArray* cellArray = new TCelArray(TSize(BarRes->GetWidth(), BarRes->GetHeight()), 
                                         0, BarRes->GetCount(), 5, numRows);
    cellArray->RemoveAll();
    destWindow.SetCelArray(cellArray);
    for (int i=0,j=0; i < (int)Gadgets->Size(); i++){
      destWindow.Insert(*(*Gadgets)[i]->Gadget);
      if((*Gadgets)[i]->UseGlyph)
        cellArray->Add(*CelArray, j++);
    }
    return true;
  }
  return false;
}
// -----------------------------------------------------------------------------
bool
TBarDescr::Merge(const TBarDescr& srcBarDescr, TGadgetWindow& destWindow)
{
  if (BarRes && BarRes->IsOK()){

    RemoveGadgets(destWindow);

    if(!srcBarDescr.Gadgets->Size())
      CreateGadgets(srcBarDescr.Gadgets, *((TBarDescr&)srcBarDescr).GetBuilder(), 
                    srcBarDescr.BarRes);

    TCelArray& cellArray = destWindow.GetCelArray();

    cellArray.RemoveAll();

    int gdIndex1 = 0;
    int gdIndex2 = 0;
    int cellIndex = 0;
    int cellIndex1 = 0;
    int cellIndex2 = 0;

    for (int g = 0; g < NumGroups; g++){
      if (srcBarDescr.GroupCount[g] > 0){
         for (int i=0; i < srcBarDescr.GroupCount[g]; i++){
          __TBarDescrGdNode* node = (*srcBarDescr.Gadgets)[gdIndex1];
          destWindow.Insert(*node->Gadget);
          if(node->UseGlyph){
            cellArray.Add(*srcBarDescr.CelArray, cellIndex1);
            TButtonGadget* bg = TYPESAFE_DOWNCAST(node->Gadget, TButtonGadget);
            if(bg)
              bg->SetGlyphIndex(cellIndex);
            cellIndex++;
            cellIndex1++;
          }
          gdIndex1++;
        }
      }
      else if (srcBarDescr.GroupCount[g] == 0 && GroupCount[g] > 0) {
         for (int i=0; i < GroupCount[g]; i++){
          __TBarDescrGdNode* node = (*Gadgets)[gdIndex2];
          destWindow.Insert(*node->Gadget);
          if(node->UseGlyph){
            cellArray.Add(*CelArray, cellIndex2);
            TButtonGadget* bg = TYPESAFE_DOWNCAST(node->Gadget, TButtonGadget);
            if(bg)
              bg->SetGlyphIndex(cellIndex);
            cellIndex++;
            cellIndex2++;
          }
          gdIndex2++;
        }
      }
    }
    destWindow.Invalidate();
    destWindow.LayoutSession();
    destWindow.UpdateWindow();
    return true;
  }
  return false;
}
// -----------------------------------------------------------------------------
bool
TBarDescr::Restore(TGadgetWindow& destWindow)
{
  if (BarRes && BarRes->IsOK()){

    RemoveGadgets(destWindow);

    TCelArray& cellArray = destWindow.GetCelArray();
    cellArray.RemoveAll();

    int gdIndex = 0;
    int cellIndex = 0;
    for (int g=0; g < NumGroups; g++){
      if(GroupCount[g]!= 0){
         for (int i=0; i < GroupCount[g]; i++){
          __TBarDescrGdNode* node = (*Gadgets)[gdIndex];
          destWindow.Insert(*node->Gadget);
          if(node->UseGlyph){
            cellArray.Add(*CelArray, cellIndex);
            TButtonGadget* bg = TYPESAFE_DOWNCAST(node->Gadget, TButtonGadget);
            if(bg)
              bg->SetGlyphIndex(cellIndex);
            cellIndex++;
          }
          gdIndex++;
        }
      }
    }
    destWindow.Invalidate();
    destWindow.LayoutSession();
    destWindow.UpdateWindow();
    return true;
  }
  return false;
}
// -----------------------------------------------------------------------------
bool
TBarDescr::RemoveGadgets(TGadgetWindow& destWindow)
{
  TGadget* gadget = destWindow.FirstGadget();
  if(gadget && gadget->GetId() == IDG_FLATHANDLE)
    gadget = gadget->NextGadget();
  while (gadget) {
    TGadget*  tmp = gadget;
    gadget = gadget->NextGadget();
    destWindow.Remove(*tmp);
  }
  return true;
}
//
// Scan bar looking for separators that signify group divisions
// return whether we found any at all
//
bool
TBarDescr::ExtractGroups()
{
  if (!BarRes)
    return false;  // no bar to extract from...

  // walk BarRes & remove separators, setting up count as we go.
  //
  int itemCount = BarRes->GetCount();
  int g = 0;
  int count = 0;
  for (int i = 0; i < itemCount;i++) {
    if (BarRes->GetIds()[i] == uint16(-1)){
      if (g < NumGroups)
        GroupCount[g++] = count;
      count = 0;
    }
    else
      count++;
  }
  // Leave if no separators were found
  //
  bool retval = true;
  if (!g)
    retval = false;

  // Get trailing group
  //
  if (g < NumGroups)
    GroupCount[g++] = count;

  // Finish zeroing groups
  //
  for (; g < NumGroups; g++)
    GroupCount[g] = 0;
  return retval;
}

//
//
//
#if !defined(BI_NO_OBJ_STREAMING)
//
// Extract the menu descriptor from the persistent stream.
//
_OWLCFUNC(ipstream&)
operator >>(ipstream& is, TBarDescr& b)
{
  is >> b.Id;
  is >> b.Module;
  b.BarRes = new TToolbarRes((b.Module ? b.Module->GetHandle() : 0), b.Id, NoAutoDelete);
  { // map colors
    TBtnBitmap bitmap(b.BarRes->GetBitmap(), TBtnBitmap::DefaultFaceColor, NoAutoDelete);
  }
  int numRows = b.BarRes->GetBitmap().Height()/b.BarRes->GetHeight();
  b.CelArray = new TCelArray(&b.BarRes->GetBitmap(),b.BarRes->GetCount(),
                           TSize(b.BarRes->GetWidth(), b.BarRes->GetHeight()),
                           0, numRows);

  for (int i = 0; i < TBarDescr::NumGroups; i++)
    is >> b.GroupCount[i];

  return is;
}

//
// Insert the menu descriptor into the persistent stream.
//
_OWLCFUNC(opstream&)
operator <<(opstream& os, const TBarDescr& b)
{
  os << b.Id;
  os << b.Module;
  for (int i = 0; i < TBarDescr::NumGroups; i++)
    os << b.GroupCount[i];
  return os;
}

#endif  // #if !defined(BI_NO_OBJ_STREAMING)

__OWL_END_NAMESPACE

/* ========================================================================== */
