#pragma once

#include <cassert>
#include <cstddef>
#include <format>
#include <iostream>
#include <string>
#include <vector>

static inline std::size_t foo_destructed = 0;

template <std::size_t I> struct Foo {
  Foo()
      : a{static_cast<int>(I)}, b{static_cast<float>(I) / 2.F},
        c{std::format("Hello {}", I)} {}
  Foo(const Foo &f) : a{f.a}, b{f.b}, c{f.c} {}
  Foo(Foo &&f) noexcept : a{f.a}, b{f.b}, c{std::move(f.c)} {}
  ~Foo() {
    if (!c.empty()) { // if not moved
      foo_destructed++;
    }
  }

  int a;
  float b;
  std::string c;
};

template <typename Storage, std::size_t... Is>
void fill_foo_impl(std::vector<Storage> &anys, std::index_sequence<Is...>) {
  (anys.push_back(Foo<Is>{}), ...);
}

template <typename Storage, std::size_t N>
void fill_foo(std::vector<Storage> &anys) {
  fill_foo_impl<Storage>(anys, std::make_index_sequence<N>{});
}

template <typename Storage, std::size_t N> int test_storage() {
  std::vector<Storage> anys;
  fill_foo<Storage, N>(anys);
  auto anys2 = anys;
  auto anys3 = std::move(anys2);

  for (auto &a : anys3) {
    // get any Foo as Foo<0> since every Foo has same layout
    auto *foo = a.template get<Foo<0>>();
    std::cout << std::format("a:{}, b:{}, c:{}\n", foo->a, foo->b, foo->c);
  }
  anys.clear();
  anys2.clear();
  anys3.clear();

  std::cout << std::format("foo_destructed: {}\n", foo_destructed);
  // expected 2xN because anys2 was moved
  if (foo_destructed != 2 * N) {
    return 1;
  }
  return 0;
}
