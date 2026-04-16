#pragma once

#include <cassert>
#include <cstddef>
#include <format>
#include <iostream>
#include <string>
#include <vector>

static inline std::size_t foo_destructed = 0;
static inline std::size_t foo_copied = 0;
static inline std::size_t foo_moved = 0;

struct FooBase {
  virtual void print() const = 0;

protected:
  ~FooBase() = default;
};

template <std::size_t I, typename _ = void> struct Foo;

template <std::size_t I>
struct Foo<I, std::enable_if_t<(I % 3 == 0) && (I % 5 == 0)>> final
    : public FooBase {
  Foo()
      : a{static_cast<int>(I)}, b{static_cast<float>(I) / 2.F},
        c{std::format("Hello {}", I)} {}
  Foo(const Foo &f) : a{f.a}, b{f.b}, c{f.c} { foo_copied++; }
  Foo(Foo &&f) noexcept : a{f.a}, b{f.b}, c{std::move(f.c)} { foo_moved++; }
  ~Foo() {
    if (!c.empty()) { // if not moved
      foo_destructed++;
    }
  }

  void print() const override {
    std::cout << std::format("FizzBuzz: a:{}, b:{}, c:{}\n", a, b, c);
  }

  int a;
  float b;
  std::string c;
};

template <std::size_t I>
struct Foo<I, std::enable_if_t<(I % 3 == 0) && (I % 5 != 0)>> final
    : public FooBase {
  Foo() : a{static_cast<int>(I)} {}
  Foo(const Foo &f) : a{f.a} { foo_copied++; }
  Foo(Foo &&f) noexcept : a{f.a} {
    foo_moved++;
    f.a = -1;
  }
  ~Foo() {
    if (a != -1) { // if not moved
      foo_destructed++;
    }
  }

  void print() const override { std::cout << std::format("Fizz: a:{}\n", a); }

  int a;
};

template <std::size_t I>
struct Foo<I, std::enable_if_t<(I % 3 != 0) && (I % 5 == 0)>> final
    : public FooBase {
  Foo() : b{static_cast<float>(I) / 2.F} {}
  Foo(const Foo &f) : b{f.b} { foo_copied++; }
  Foo(Foo &&f) noexcept : b{f.b} {
    foo_moved++;
    f.b = -1.F;
  }
  ~Foo() {
    if (b != -1.F) { // if not moved
      foo_destructed++;
    }
  }

  void print() const override { std::cout << std::format("Buzz: b:{}\n", b); }

  float b;
};

template <std::size_t I>
struct Foo<I, std::enable_if_t<(I % 3 != 0) && (I % 5 != 0)>> final
    : public FooBase {
  Foo() {}
  Foo(const Foo &f) { foo_copied++; }
  Foo(Foo &&f) noexcept {
    foo_moved++;
    f.moved = true;
  }
  ~Foo() {
    if (!moved) { // if not moved
      foo_destructed++;
    }
  }

  void print() const override { std::cout << std::format("I: i:{}\n", I); }
  bool moved = false;
};

template <typename Storage, std::size_t... Is>
void fill_foo_impl(std::vector<Storage> &anys, std::index_sequence<Is...>) {
  (anys.emplace_back(Foo<Is>{}), ...);
}

template <typename Storage, std::size_t N>
void fill_foo(std::vector<Storage> &anys) {
  fill_foo_impl<Storage>(anys, std::make_index_sequence<N>{});
}

template <typename Storage, std::size_t N> int test_storage() {
  std::vector<Storage> anys;
  anys.reserve(N);
  fill_foo<Storage, N>(anys);
  auto anys2 = anys;
  auto anys3 = std::move(anys2);

  for (auto const &a : anys3) {
    auto const *foo = a.template get<FooBase>();
    foo->print();
  }
  anys.clear();
  anys2.clear();
  anys3.clear();

  std::cout << std::format("foo_destructed: {} foo_copied: {} foo_moved: {}\n",
                           foo_destructed, foo_copied, foo_moved);
  // expected 2xN because anys2 was moved
  if (foo_destructed != 2 * N || foo_copied != N || foo_moved != N) {
    return 1;
  }
  return 0;
}
