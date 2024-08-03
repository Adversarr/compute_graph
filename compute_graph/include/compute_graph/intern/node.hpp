// The MIT License (MIT)
// Copyright © 2024 Adversarr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once
#include "config.hpp"

namespace compute_graph {

namespace intern {
template <size_t N, size_t Low, size_t High> struct static_for {
  template <template <size_t> typename Fn, typename... Args>
  static constexpr void eval(Args &...args) {
    Fn<N>::eval(args...);
    static_for<N + 1, Low, High>().template eval<Fn>(args...);
  }
};

template <size_t N, size_t Low> struct static_for<N, Low, N> {
  template <template <size_t> typename Fn, typename... Args>
  static constexpr void eval(Args &...) {}
};

template <size_t Low, size_t High, template <size_t> typename Fn,
          typename... Args>
constexpr void static_for_eval(Args &...args) {
  static_for<Low, Low, High>().template eval<Fn, Args...>(args...);
}

template <typename T> struct is_meta_valid {
  static constexpr bool value = !std::is_same_v<typename T::type, void>;
};

template <template <size_t> typename T, size_t current,
          bool valid = is_meta_valid<T<current>>::value>
struct count_meta;

template <template <size_t> typename T, size_t current>
struct count_meta<T, current, true> {
  static constexpr size_t count = 1 + count_meta<T, current + 1>::count;
};
template <template <size_t> typename T, size_t current>
struct count_meta<T, current, false> {
  static constexpr size_t count = 0;
};

template <template <size_t> typename T>
constexpr size_t count_meta_v = count_meta<T, 0>::count;

template <typename T>
constexpr size_t count_socket_v = count_meta_v<T::template socket_meta>;


template<typename, typename T>
struct has_default_value {
  static_assert(
    std::integral_constant<T, false>::value,
    "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret>
struct has_default_value<C, Ret()> {
private:
  template<typename T>
  static constexpr auto check(T *)
    -> typename
    std::is_same<decltype(T::default_value()), Ret>::type;

  template<typename>
  static constexpr std::false_type check(...);

  typedef decltype(check<C>(nullptr)) type;

public:
  static constexpr bool value = type::value;
};

template<typename C>
struct has_on_register {
private:
  template<typename T>
  static constexpr auto check(T *)
    -> typename
    std::is_same<decltype(T::on_register()), void>::type;

  template<typename>
  static constexpr std::false_type check(...);

  typedef decltype(check<C>(nullptr)) type;

public:
  static constexpr bool value = type::value;
};

template<typename C, bool is_valid_call = has_on_register<C>::value>
struct call_on_register_if_presented;

template<typename C>
struct call_on_register_if_presented<C, true> {
  static void exec() { C::on_register(); }
};

template<typename C>
struct call_on_register_if_presented<C, false> {
  static void exec() {
  }
};

template<typename M>
static constexpr bool has_default_value_v = has_default_value<M, typename M::type const&()>::value;
} // namespace intern

}