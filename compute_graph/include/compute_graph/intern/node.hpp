#pragma once
#include "config.hpp"
#include <utility>

CG_NAMESPACE_BEGIN

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

} // namespace intern

CG_NAMESPACE_END
