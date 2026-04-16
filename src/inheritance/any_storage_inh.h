#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace it {
/**
 * This any type storage is using Vtable with separate methods for each
 * operation
 */

struct VtableBase {
  virtual void destroy(void *p1) const = 0;
  virtual void copy(void *p1, void *p2) const = 0;
  virtual void move(void *p1, void *p2) const = 0;

protected:
  ~VtableBase() = default;
};

template <typename T> struct MakeVtale final : public VtableBase {
  void destroy(void *src) const override {
    std::launder(reinterpret_cast<T *>(src))->~T();
  }
  void copy(void *src, void *dst) const override {
    new (dst) T{std::as_const(*std::launder(reinterpret_cast<T *>(src)))};
  }
  void move(void *src, void *dst) const override {
    new (dst) T{std::move(*std::launder(reinterpret_cast<T *>(src)))};
  }
};

template <typename T> static constexpr auto kMakeTable = MakeVtale<T>();

template <std::size_t Size, std::size_t Align> struct AnyStorage {
  // construct directly from T
  template <typename T>
    requires(!std::same_as<std::decay_t<T>, AnyStorage>)
  AnyStorage(T &&t) : vtable{&kMakeTable<T>} {
    new (data) T{std::move(t)};
  }

  ~AnyStorage() {
    if (vtable != nullptr) {
      vtable->destroy(data);
    }
  }

  AnyStorage(AnyStorage const &other) : vtable{other.vtable} {
    vtable->copy(const_cast<std::uint8_t *>(other.data), data);
  }
  AnyStorage(AnyStorage &&other) noexcept : vtable{other.vtable} {
    other.vtable = nullptr;
    vtable->move(other.data, data);
  }

  template <typename T> T *get() noexcept {
    return std::launder(reinterpret_cast<T *>(data));
  }
  template <typename T> T const *get() const noexcept {
    return std::launder(reinterpret_cast<T const *>(data));
  }

  VtableBase const *vtable;
  alignas(Align) std::uint8_t data[Size];
};
} // namespace it
