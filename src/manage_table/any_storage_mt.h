#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace mt {

/**
 * This any type storage is using Vtable with one manage method
 * Where operations are selected by Operation enum
 */

enum class Operation : unsigned char { Copy, Move, Destroy };

struct Vtable {
  void (*manage)(void *p1, void *p2, Operation o);
};

template <typename T>
static constexpr Vtable MakeVtale{
    .manage =
        [](void *src, void *dst, Operation o) {
          switch (o) {
          case Operation::Copy: {
            new (dst)
                T{std::as_const(*std::launder(reinterpret_cast<T *>(src)))};
          } break;
          case Operation::Move: {
            new (dst) T{std::move(*std::launder(reinterpret_cast<T *>(src)))};
          } break;
          case Operation::Destroy: {
            std::launder(reinterpret_cast<T *>(src))->~T();
          } break;
          }
        }

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
      vtable->manage(data, nullptr, Operation::Destroy);
    }
  }

  AnyStorage(AnyStorage const &other) : vtable{other.vtable} {
    vtable->manage(const_cast<std::uint8_t *>(other.data), data,
                   Operation::Copy);
  }
  AnyStorage(AnyStorage &&other) noexcept : vtable{other.vtable} {
    other.vtable = nullptr;
    vtable->manage(other.data, data, Operation::Move);
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
} // namespace mt
