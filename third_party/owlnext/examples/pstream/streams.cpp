//
// OWLNext Persistent Streams Example
//
// This example demonstrates the Persistent Streams feature from the Borland C++ Class Libraries.
// The code is based on "examples/classlib/pstream" included with Borland C++ 5.02.
//
// Note that Persistent Streams are deprecated and only available in OWL5_COMPAT mode.
// For new code, use Boost Serialization or other alternatives instead.
//
// See "Replacing the Borland C++ Class Libraries" in the OWLNext Wiki for more information.
// http://sourceforge.net/p/owlnext/wiki/Replacing_the_Borland_C%2B%2B_Class_Libraries
//
// Copyright (c) 1993 Borland International
// Copyright (c) 2015 Vidar Hasfjord
//
// For usage rights, see the OWLNext License (http://sourceforge.net/projects/owlnext).

#include <owl/objstrm.h>
#include <owl/window.h>
#include <owl/dc.h>
#include <owl/dialog.h>
#include <owl/framewin.h>
#include <owl/applicat.h>
#include <owl/opensave.h>
#include <vector>
#include <memory>

#include "streams.rh"

using namespace std;
using namespace owl;

//
// Helper for streaming in a smart pointer.
//
template <class P>
auto operator >>(ipstream& in, unique_ptr<P>& p) -> ipstream&
{
  auto raw = static_cast<P*>(nullptr);
  in >> raw;
  if (in.good())
    p.reset(raw);
  return in;
}

//
// Base class for the graphical objects that can be displayed by this graphical system.
//
class TGraphicalObject
  : public TStreamableBase
{
public:

  TGraphicalObject(const TRect& boundingBox)
    : BoundingBox(boundingBox)

#if VERSION >= 2

    , IsFilled(false)

#endif

  {}

  auto GetBoundingBox() const -> const TRect& {return BoundingBox;}
  void SetBoundingBox(const TRect& r) { BoundingBox = r; }

#if VERSION >= 2

  auto GetFill() const -> bool { return IsFilled; }
  void SetFill(bool isFilled) { IsFilled = isFilled; }

#endif

  //
  // Does some preliminary setup of the device context, then calls the virtual function DoDraw to
  // actually draw the object.
  //
  void Draw(TDC& dc)
  {

#if VERSION >= 2

    const auto b = IsFilled ? BLACK_BRUSH : NULL_BRUSH;

#else

    const auto b = NULL_BRUSH;

#endif

    dc.SelectStockObject(b);
    DoDraw(dc);
  }

protected:

  //
  // Concrete derived class must override this to draw the object.
  //
  virtual void DoDraw(TDC&) = 0;

private:

  TRect BoundingBox;

#if VERSION >= 2

  bool IsFilled;

#endif

  DECLARE_ABSTRACT_STREAMABLE(, TGraphicalObject, VERSION);
};

IMPLEMENT_ABSTRACT_STREAMABLE(TGraphicalObject);

auto TGraphicalObject::Streamer::Read(ipstream& in, uint32 version) const -> void*
{
  auto& i = *GetObject();
  in >> i.BoundingBox;

#if VERSION >= 2

  if (version >= 2)
    in >> i.IsFilled;
  else
    i.IsFilled = false; // Old objects didn't write this field.

#endif

  return &i;
}

void TGraphicalObject::Streamer::Write(opstream& out) const
{
  auto& i = *GetObject();
  out << i.BoundingBox;

#if VERSION >= 2

  out << i.IsFilled;

#endif

}

//
// Draws a line between two specified points.
//
class TLine
  : public TGraphicalObject
{
public:

  TLine(TPoint p1, TPoint p2)
    : TGraphicalObject(TRect{p1, p2})
  {}

protected:

  void DoDraw(TDC& dc) override
  {
    const auto& b = GetBoundingBox();
    dc.MoveTo(b.TopLeft());
    dc.LineTo(b.BottomRight());
  }

  DECLARE_STREAMABLE(, TLine, 1);
};

IMPLEMENT_STREAMABLE1(TLine, TGraphicalObject);

auto TLine::Streamer::Read(ipstream& is, uint32) const -> void*
{
  const auto p = GetObject();
  ReadBaseObject<TGraphicalObject>(p, is);
  return p;
}

void TLine::Streamer::Write(opstream& os) const
{
  WriteBaseObject<TGraphicalObject>(GetObject(), os);
}

//
// Draws a rectangle.
//
class TRectangle
  : public TGraphicalObject
{
public:

  TRectangle(const TRect& r)
    : TGraphicalObject(r)
  {}

protected:

  void DoDraw(TDC& dc) override
  {
    dc.Rectangle(GetBoundingBox());
  }

  DECLARE_STREAMABLE(, TRectangle, 1);
};

IMPLEMENT_STREAMABLE1(TRectangle, TGraphicalObject);

auto TRectangle::Streamer::Read(ipstream& is, uint32) const -> void*
{
  const auto p = GetObject();
  ReadBaseObject<TGraphicalObject>(p, is);
  return p;
}

void TRectangle::Streamer::Write(opstream& os) const
{
  WriteBaseObject<TGraphicalObject>(GetObject(), os);
}

//
// Draws an ellipse.
//
class TEllipse
  : public TGraphicalObject
{
public:

  TEllipse(const TRect& boundingBox)
    : TGraphicalObject(boundingBox)
  {}

protected:

  void DoDraw(TDC& dc) override
  {
    dc.Ellipse(GetBoundingBox());
  }

  DECLARE_STREAMABLE(, TEllipse, 1);
};

IMPLEMENT_STREAMABLE1(TEllipse, TGraphicalObject);

auto TEllipse::Streamer::Read(ipstream& is, uint32) const -> void*
{
  const auto p = GetObject();
  ReadBaseObject<TGraphicalObject>(p, is);
  return p;
}

void TEllipse::Streamer::Write(opstream& os) const
{
  WriteBaseObject<TGraphicalObject>(GetObject(), os);
}

//
// Provides the core functionality for building objects derived from TGraphicalObject.
// It handles changing the cursor to one that indicates the type of object being built, clipping
// the cursor so that it stays within the application's client area, handles anchoring the object's
// bounding box and dragging the unanchored corner of the bounding box.
//
class TObjectBuilder
{
public:

  //
  // Clips the cursor so that it stays within the owner's client area and replaces the current
  // window cursor by the given cursor.
  //
  TObjectBuilder(TWindow& owner, TResId cursor)
    : Owner(owner),
    Anchor(),
    Current(),
    IsAnchored(false)
  {
    auto r = Owner.GetWindowRect();
    ClipCursor(&r);
    Owner.SetCursor(Owner.GetApplication(), cursor);
  }

  //
  // Restores the default cursor and ends the cursor clipping.
  //
  virtual ~TObjectBuilder()
  {
    Owner.SetCursor(nullptr, 0);
    ClipCursor(nullptr);
  }

  auto GetOwner() const -> TWindow&
  { return Owner; }

  //
  // Sets the anchor point of the bounding box to the given point.
  //
  void SetAnchor(const TPoint& pos)
  {
    PRECONDITION(!IsAnchored);

    Anchor = Current = pos;
    TWindowDC dc{Owner};
    Init(dc);
    DrawSkeleton(dc, Anchor, Current);
    IsAnchored = true;
  }

  //
  // Moves the unanchored corner of the bounding box to the given point, drawing the object's
  // skeleton as appropriate by calling virtual DrawSkeleton.
  //
  void Drag(const TPoint& pos)
  {
    if (!IsAnchored) return;

    TWindowDC dc{Owner};
    Init(dc);
    DrawSkeleton(dc, Anchor, Current);
    Current = pos;
    DrawSkeleton(dc, Anchor, Current);
  }

  using TObjectPtr = unique_ptr<TGraphicalObject>;

  //
  // Returns a pointer to the newly created object contained within the bounding box delimited by
  // the anchor point and the unanchored corner as set up by SetAnchor and Drag. Calls virtual
  // DoCreateObject to create the concrete object.
  //
  auto CreateObject() const -> TObjectPtr
  {
    return DoCreateObject(Anchor, Current);
  }

  //
  // Cleans up whatever needs to be cleaned up when the object is not wanted.
  //
  void Cancel() const
  {
    if (!IsAnchored) return;

    TWindowDC dc{Owner};
    Init(dc);
    DrawSkeleton(dc, Anchor, Current);
  }

  TObjectBuilder(const TObjectBuilder&) = delete;
  auto operator =(const TObjectBuilder&) -> const TObjectBuilder& = delete;

protected:

  //
  // This must be overridden by the derived class. It is called from Drag to erase the previously
  // drawn skeleton which was drawn at `p1` and `p2`, and it is called again to draw the new
  // skeleton at positions `p1` and `p2`. The skeleton should be some sort of simple line
  // representation of the object being built.
  //
  virtual void DrawSkeleton(TDC& dc, const TPoint& p1, const TPoint& p2) const = 0;

  //
  // Called from CreateObject to actually build the desired object, with its bounding box defined
  // by `p1` and `p2`. Typically this just calls a constructor for the desired object.
  //
  virtual auto DoCreateObject(const TPoint& p1, const TPoint& p2) const -> TObjectPtr = 0;

private:

  TWindow& Owner;
  TPoint Anchor;
  TPoint Current;
  bool IsAnchored;

  static void Init(TDC& dc)
  {
    dc.SelectStockObject(WHITE_PEN);
    dc.SetROP2(R2_XORPEN);
    dc.SelectStockObject(NULL_BRUSH);
  }

};

//
// Builds a TLine object using the mechanisms inherited from TObjectBuilder.
//
class TLineBuilder
  : public TObjectBuilder
{

public:

  TLineBuilder(TWindow& owner)
    : TObjectBuilder(owner, LINE_CURSOR)
  {}

protected:

  void DrawSkeleton(TDC& dc, const TPoint& p1, const TPoint& p2) const override
  {
    dc.MoveTo(p1.x, p1.y);
    dc.LineTo(p2.x, p2.y);
  }

  auto DoCreateObject(const TPoint& p1, const TPoint& p2) const -> TObjectPtr override
  {
    return make_unique<TLine>(p1, p2);
  }

};

//
// Builds a TRectangle object using the mechanisms inherited from TObjectBuilder.
//
class TRectangleBuilder
  : public TObjectBuilder
{

public:

  TRectangleBuilder(TWindow& owner)
    : TObjectBuilder(owner, RECT_CURSOR)
  {}

protected:

  void DrawSkeleton(TDC& dc, const TPoint& p1, const TPoint& p2) const override
  {
    dc.Rectangle(p1, p2);
  }

  auto DoCreateObject(const TPoint& p1, const TPoint& p2) const -> TObjectPtr override
  {
    return make_unique<TRectangle>(TRect{p1, p2});
  }

};

//
// Builds a TEllipse object using the mechanisms inherited from TObjectBuilder.
//
class TEllipseBuilder
  : public TObjectBuilder
{

public:

  TEllipseBuilder(TWindow& owner)
    : TObjectBuilder(owner, ELLIPSE_CURSOR)
  {}

private:

  void DrawSkeleton(TDC& dc, const TPoint& p1, const TPoint& p2) const override
  {
    dc.Ellipse(p1, p2);
  }

  auto DoCreateObject(const TPoint& p1, const TPoint& p2) const -> TObjectPtr override
  {
    return make_unique<TEllipse>(TRect{p1, p2});
  }

};

//
// Pulls all of the foregoing together into a rather limited shape editor.
//
class TGraphWindow
  : public TWindow
{
public:

  TGraphWindow()
    : TWindow(nullptr, nullptr, nullptr),
    FileData(FileFlags, FileFilter, nullptr, nullptr, ".stm"),
    Objects(),
    Builder(),
    IsWindowDirty(false)

#if VERSION >= 2

    , ShouldFill(false)

#endif

  {}

  void Paint(TDC& dc, bool, TRect&)
  {
    const auto pen = TPen{TColor::SysWindowText};
    dc.SelectObject(pen);
    dc.SetBkColor(TColor::SysWindow);
    const auto oldAlign = dc.SetTextAlign(TA_CENTER);

    for (auto&& obj : Objects)
    {
      CHECK(obj);
      obj->Draw(dc);
    }

    dc.SetTextAlign(oldAlign);
    dc.RestorePen();
  }

  auto CanClose() -> bool override
  {
    return CheckAndClear();
  }

  TOpenSaveDialog::TData FileData;

private:

  static const uint32 FileFlags;
  static const LPCTSTR FileFilter;

  using TObjectPtr = TGraphicalObject*;
  using TObjects = vector<TObjectPtr>;

  TObjects Objects;
  unique_ptr<TObjectBuilder> Builder;
  bool IsWindowDirty;

#if VERSION >= 2

  bool ShouldFill;

#endif

  void CmFileNew()
  {
    if (CheckAndClear())
      Invalidate();
  }

  void CmFileOpen()
  {
    if (CheckAndClear())
    {
      FileData.FileName[0] = '\0';
      auto d = TFileOpenDialog{this, FileData};
      if (d.Execute() == IDOK)
      {
        ReadObjects();
        Invalidate();
      }
    }
  }

  void CmFileSave()
  {
    Save();
  }

  void CmFileSaveAs()
  {
    SaveAs();
  }

  void CmLine()
  {
    Builder = make_unique<TLineBuilder>(*this);
  }

  void CmRectangle()
  {
    Builder = make_unique<TRectangleBuilder>(*this);
  }

  void CmEllipse()
  {
    Builder = make_unique<TEllipseBuilder>(*this);
  }

#if VERSION >= 2

  void CmToggleFilled()
  {
    ShouldFill = !ShouldFill;
  }

  void CeFilled(TCommandEnabler& c)
  {
    c.SetCheck(ShouldFill);
  }

#endif

  void CmAbout()
  {
    TDialog{this, IDD_ABOUT}.Execute();
  }

  //
  // During building of an object, this anchors the object's bounding box at the current position
  // of the mouse pointer.
  //
  void EvLButtonDown(uint modKeys, const TPoint& point)
  {
    if (Builder)
      Builder->SetAnchor(point);
    else
      TWindow::EvLButtonDown(modKeys, point);
  }

  //
  // During building of an object, this creates the actual object with its bounding box defined by
  // the previous anchor position and the current position of the mouse.
  //
  void EvLButtonUp(uint modKeys, const TPoint& point)
  {
    if (Builder)
    {
      AddObject(Builder->CreateObject());
      Builder.reset();
      Invalidate();
    }
    else
      TWindow::EvLButtonUp(modKeys, point);
  }

  //
  // During building of an object, this terminates building.
  //
  void EvRButtonDown(uint modKeys, const TPoint& point)
  {
    if (Builder)
    {
      Builder->Cancel();
      Builder.reset();
    }
    else
      TWindow::EvRButtonDown(modKeys, point);
  }

  //
  // During building of an object, after having anchored the bounding box (EvLButtonDown), this
  // drags the free corner of the bounding box.
  //
  void EvMouseMove(uint modKeys, const TPoint& point)
  {
    if (Builder)
      Builder->Drag(point);
    else
      TWindow::EvMouseMove(modKeys, point);
  }

  auto Save() -> bool
  {
    return (FileData.FileName[0] == '\0') ? SaveAs() : WriteObjects();
  }

  auto SaveAs() -> bool
  {
    auto r = TFileSaveDialog{this, FileData}.Execute();
    return (r == IDOK) ? WriteObjects() : false;
  }

  auto ReadObjects() -> bool
  {
    auto in = ifpstream{FileData.FileName};
    auto count = int{};
    in >> count;
    while (count--)
    {
      auto p = unique_ptr<TGraphicalObject>{};
      in >> p;
      if (in.fail())
        break;
      Objects.push_back(p.get());
      p.release(); // We have taken ownership.
    }
    IsWindowDirty = false;
    return in.good();
  }

  auto WriteObjects() -> bool
  {
    PRECONDITION(FileData.FileName[0] != '\0');
    auto out = ofpstream{FileData.FileName};
    out << static_cast<int>(Objects.size());
    for (auto&& p : Objects)
      out << p;
    auto ok = out.good();
    if (ok)
      IsWindowDirty = false;
    return ok;
  }

  auto CheckAndClear() -> bool
  {
    auto saveDiscardOrCancel = [&]
    {
      const auto msg = "The document has not been saved.\n\nSave before closing?";
      const auto r = MessageBox(msg, GetApplication()->GetName(), MB_ICONEXCLAMATION | MB_YESNOCANCEL);
      return (r == IDYES) ? Save() : (r == IDNO);
    };

    const auto ok = IsWindowDirty ? saveDiscardOrCancel() : true;
    if (ok)
      FlushObjects();
    return ok;
  }

  void FlushObjects()
  {
    for (auto&& p : Objects)
      delete p;
    Objects.clear();
    IsWindowDirty = false;
  }

  void AddObject(TObjectBuilder::TObjectPtr p)
  {

#if VERSION >= 2

    p->SetFill(ShouldFill);

#endif

    Objects.push_back(p.get());
    p.release(); // We have taken ownership.
    IsWindowDirty = true;
  }

  DECLARE_RESPONSE_TABLE(TGraphWindow);
};

const uint32 TGraphWindow::FileFlags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
const LPCTSTR TGraphWindow::FileFilter = "Persistent Streams (*.stm)|*.stm|All Files (*.*)|*.*|";

DEFINE_RESPONSE_TABLE1(TGraphWindow, TWindow)
  EV_COMMAND(CM_FILENEW, CmFileNew),
  EV_COMMAND(CM_FILEOPEN, CmFileOpen),
  EV_COMMAND(CM_FILESAVE, CmFileSave),
  EV_COMMAND(CM_FILESAVEAS, CmFileSaveAs),
  EV_COMMAND(CM_EDITLINE, CmLine),
  EV_COMMAND(CM_EDITRECTANGLE, CmRectangle),
  EV_COMMAND(CM_EDITELLIPSE, CmEllipse),

#if VERSION >= 2

  EV_COMMAND(CM_EDITFILLED, CmToggleFilled),
  EV_COMMAND_ENABLE(CM_EDITFILLED, CeFilled),

#endif

  EV_COMMAND(CM_HELPABOUT, CmAbout),
  EV_WM_LBUTTONDOWN,
  EV_WM_LBUTTONUP,
  EV_WM_RBUTTONDOWN,
  EV_WM_MOUSEMOVE,
END_RESPONSE_TABLE;

class TStreamingExample
  : public TApplication
{
public:

  TStreamingExample()
    : TApplication("Persistent Streams Example")
  {}

  void InitMainWindow()
  {
    const auto f = new TFrameWindow(nullptr, GetName(), new TGraphWindow());
    f->AssignMenu(IDM_MAIN);
    SetMainWindow(f);
  }

};

auto OwlMain(int, tchar* []) -> int
{
  return TStreamingExample{}.Run();
}
