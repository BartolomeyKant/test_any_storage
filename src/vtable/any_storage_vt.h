#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace vt {
/**
 * This any type storage is using Vtable with separate methods for each
 * operation
 */

struct Vtable {
  void (*destroy)(void *p1);
  void (*copy)(void *p1, void *p2);
  void (*move)(void *p1, void *p2);
};

template <typename T>
static constexpr Vtable MakeVtale{
    .destroy =
        [](void *src) { std::launder(reinterpret_cast<T *>(src))->~T(); },
    .copy =
        [](void *src, void *dst) {
          new (dst) T{std::as_const(*std::launder(reinterpret_cast<T *>(src)))};
        },
    .move =
        [](void *src, void *dst) noexcept {
          new (dst) T{std::move(*std::launder(reinterpret_cast<T *>(src)))};
        },
};

template <std::size_t Size, std::size_t Align> struct AnyStorage {
  // construct directly from T
  template <typename T>
    requires(!std::same_as<std::decay_t<T>, AnyStorage>)
  AnyStorage(T &&t) : vtable{&MakeVtale<T>} {
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

  Vtable const *vtable;
  alignas(Align) std::uint8_t data[Size];
};
} // namespace vt
