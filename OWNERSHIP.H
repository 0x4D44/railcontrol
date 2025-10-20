/* OWNERSHIP.H
 *  Managed pointer array helpers for MSVC builds with Borland fallback.
 */
#ifndef OWNERSHIP_H_INCLUDED
#define OWNERSHIP_H_INCLUDED

#include <cstddef>
#include <utility>

#if defined(_MSC_VER)
#include <memory>
#endif

template <typename T>
class TManagedSlot
{
public:
  TManagedSlot() = default;
  ~TManagedSlot()
  {
    Reset();
  }

  TManagedSlot(const TManagedSlot&) = delete;
  TManagedSlot& operator=(const TManagedSlot&) = delete;

  TManagedSlot(TManagedSlot&& other) noexcept
  {
    MoveAssign(std::move(other));
  }

  TManagedSlot& operator=(TManagedSlot&& other) noexcept
  {
    if (this != &other)
    {
      Reset();
      MoveAssign(std::move(other));
    }
    return *this;
  }

  TManagedSlot& operator=(T* value)
  {
    Reset(value);
    return *this;
  }

  TManagedSlot& operator=(std::nullptr_t)
  {
    Reset(nullptr);
    return *this;
  }

  T* Get() const
  {
    return InternalGet();
  }

  operator T*() const
  {
    return Get();
  }

  T* operator->() const
  {
    return Get();
  }

  T& operator*() const
  {
    return *Get();
  }

  void Reset(T* value = nullptr)
  {
#if defined(_MSC_VER)
    if (mPointer.get() != value)
    {
      mPointer.reset(value);
    }
#else
    if (mPointer != value)
    {
      delete mPointer;
    }
    mPointer = value;
#endif
  }

  T* Release()
  {
#if defined(_MSC_VER)
    return mPointer.release();
#else
    T* released = mPointer;
    mPointer = nullptr;
    return released;
#endif
  }

  void Swap(TManagedSlot& other) noexcept
  {
#if defined(_MSC_VER)
    mPointer.swap(other.mPointer);
#else
    std::swap(mPointer, other.mPointer);
#endif
  }

  template <typename... Args>
  T* Emplace(Args&&... args)
  {
    Reset(new T(std::forward<Args>(args)...));
    return Get();
  }

private:
#if defined(_MSC_VER)
  using storage_type = std::unique_ptr<T>;
  storage_type mPointer;
#else
  using storage_type = T*;
  storage_type mPointer = nullptr;
#endif

  T* InternalGet() const
  {
#if defined(_MSC_VER)
    return mPointer.get();
#else
    return mPointer;
#endif
  }

  void MoveAssign(TManagedSlot&& other) noexcept
  {
#if defined(_MSC_VER)
    mPointer = std::move(other.mPointer);
#else
    mPointer = other.mPointer;
    other.mPointer = nullptr;
#endif
  }
};

template <typename T, std::size_t Count>
class TManagedArray
{
public:
  using value_type = T;
  using slot_type = TManagedSlot<T>;

  TManagedArray() = default;

  slot_type& operator[](std::size_t index)
  {
    return mSlots[index];
  }

  const slot_type& operator[](std::size_t index) const
  {
    return mSlots[index];
  }

  T* Get(std::size_t index) const
  {
    return mSlots[index].Get();
  }

  void Reset(std::size_t index, T* value = nullptr)
  {
    mSlots[index].Reset(value);
  }

  template <typename... Args>
  T* Emplace(std::size_t index, Args&&... args)
  {
    return mSlots[index].Emplace(std::forward<Args>(args)...);
  }

  T* Release(std::size_t index)
  {
    return mSlots[index].Release();
  }

  void Clear()
  {
    for (std::size_t i = 0; i < Count; ++i)
    {
      mSlots[i].Reset();
    }
  }

  constexpr std::size_t Size() const
  {
    return Count;
  }

private:
  slot_type mSlots[Count] = {};
};

#endif // OWNERSHIP_H_INCLUDED
