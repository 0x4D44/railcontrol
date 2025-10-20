/* HANDLEGUARD.H
 *  RAII wrappers for Windows handles.
 */
#ifndef HANDLEGUARD_H_INCLUDED
#define HANDLEGUARD_H_INCLUDED

#include <utility>
#include <memory>
#include <type_traits>

#include <windows.h>

template <typename Traits>
class THandleGuard
{
public:
  typedef Traits traits_type;
  typedef typename traits_type::handle_type handle_type;

  THandleGuard()
    : mHandle(traits_type::Invalid())
  {
  }

  explicit THandleGuard(handle_type handle)
    : mHandle(handle)
  {
  }

  ~THandleGuard()
  {
    Reset();
  }

  THandleGuard(const THandleGuard&) = delete;
  THandleGuard& operator=(const THandleGuard&) = delete;

  THandleGuard(THandleGuard&& other) noexcept
    : mHandle(other.Release())
  {
  }

  THandleGuard& operator=(THandleGuard&& other) noexcept
  {
    if (this != &other)
    {
      Reset();
      mHandle = other.Release();
    }
    return *this;
  }

  handle_type Get() const
  {
    return mHandle;
  }

  operator handle_type() const
  {
    return Get();
  }

  bool IsValid() const
  {
    return mHandle != traits_type::Invalid();
  }

  handle_type Release()
  {
    handle_type handle = mHandle;
    mHandle = traits_type::Invalid();
    return handle;
  }

  void Reset(handle_type handle = traits_type::Invalid())
  {
    if ((mHandle != traits_type::Invalid()) && (mHandle != handle))
    {
      traits_type::Close(mHandle);
    }
    mHandle = handle;
  }

  handle_type* Put()
  {
    Reset();
    return &mHandle;
  }

  THandleGuard& operator=(handle_type handle)
  {
    Reset(handle);
    return *this;
  }

  void Swap(THandleGuard& other) noexcept
  {
    std::swap(mHandle, other.mHandle);
  }

private:
  handle_type mHandle;
};

template <typename Traits>
class TSharedHandle
{
public:
  typedef Traits traits_type;
  typedef typename traits_type::handle_type handle_type;
  static_assert(std::is_pointer<handle_type>::value, "TSharedHandle requires pointer handle types");

  TSharedHandle() = default;

  explicit TSharedHandle(handle_type handle)
  {
    Reset(handle);
  }

  handle_type Get() const
  {
    if (!mHandle)
    {
      return traits_type::Invalid();
    }
    return reinterpret_cast<handle_type>(mHandle.get());
  }

  operator handle_type() const
  {
    return Get();
  }

  bool IsValid() const
  {
    return Get() != traits_type::Invalid();
  }

  void Reset(handle_type handle = traits_type::Invalid())
  {
    if (handle == traits_type::Invalid())
    {
      mHandle.reset();
      return;
    }

    mHandle.reset(reinterpret_cast<void*>(handle), HandleCloser());
  }

private:
  struct HandleCloser
  {
    typedef Traits traits_type;
    typedef typename traits_type::handle_type handle_type;

    void operator()(void* rawHandle) const
    {
      handle_type handle = reinterpret_cast<handle_type>(rawHandle);
      if (handle != traits_type::Invalid())
      {
        traits_type::Close(handle);
      }
    }
  };

  std::shared_ptr<void> mHandle;
};

template <typename HandleType>
struct TGdiHandleTraits
{
  typedef HandleType handle_type;

  static handle_type Invalid()
  {
    return handle_type();
  }

  static void Close(handle_type handle)
  {
    if (handle != Invalid())
    {
      ::DeleteObject(handle);
    }
  }
};

struct TDcHandleTraits
{
  typedef HDC handle_type;

  static handle_type Invalid()
  {
    return nullptr;
  }

  static void Close(handle_type handle)
  {
    if (handle)
    {
      ::DeleteDC(handle);
    }
  }
};

struct TMenuHandleTraits
{
  typedef HMENU handle_type;

  static handle_type Invalid()
  {
    return nullptr;
  }

  static void Close(handle_type handle)
  {
    if (handle)
    {
      ::DestroyMenu(handle);
    }
  }
};

struct TWindowHandleTraits
{
  typedef HWND handle_type;

  static handle_type Invalid()
  {
    return nullptr;
  }

  static void Close(handle_type handle)
  {
    if (handle)
    {
      ::DestroyWindow(handle);
    }
  }
};

typedef THandleGuard<TGdiHandleTraits<HBITMAP> > TBitmapGuard;
typedef THandleGuard<TGdiHandleTraits<HBRUSH> > TBrushGuard;
typedef THandleGuard<TGdiHandleTraits<HPEN> > TPenGuard;
typedef THandleGuard<TGdiHandleTraits<HFONT> > TFontGuard;
typedef THandleGuard<TDcHandleTraits> TDcGuard;

class TReleaseDcGuard
{
public:
  TReleaseDcGuard() = default;

  TReleaseDcGuard(HWND window, HDC dc)
    : mWindow(window)
    , mHandle(dc)
  {
  }

  ~TReleaseDcGuard()
  {
    Reset();
  }

  TReleaseDcGuard(const TReleaseDcGuard&) = delete;
  TReleaseDcGuard& operator=(const TReleaseDcGuard&) = delete;

  TReleaseDcGuard(TReleaseDcGuard&& other) noexcept
    : mWindow(other.mWindow)
    , mHandle(other.mHandle)
  {
    other.mWindow = nullptr;
    other.mHandle = nullptr;
  }

  TReleaseDcGuard& operator=(TReleaseDcGuard&& other) noexcept
  {
    if (this != &other)
    {
      Reset();
      mWindow = other.mWindow;
      mHandle = other.mHandle;
      other.mWindow = nullptr;
      other.mHandle = nullptr;
    }
    return *this;
  }

  void Attach(HWND window, HDC dc)
  {
    if ((window == mWindow) && (dc == mHandle))
    {
      return;
    }
    Reset();
    mWindow = window;
    mHandle = dc;
  }

  HDC Get() const
  {
    return mHandle;
  }

  operator HDC() const
  {
    return Get();
  }

  HDC Release()
  {
    HDC dc = mHandle;
    mHandle = nullptr;
    mWindow = nullptr;
    return dc;
  }

  void Reset()
  {
    if (mWindow && mHandle)
    {
      ::ReleaseDC(mWindow, mHandle);
    }
    mWindow = nullptr;
    mHandle = nullptr;
  }

private:
  HWND mWindow = nullptr;
  HDC mHandle = nullptr;
};

#endif // HANDLEGUARD_H_INCLUDED
