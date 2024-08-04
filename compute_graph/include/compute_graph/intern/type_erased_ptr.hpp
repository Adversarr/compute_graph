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
#include <functional>
#include <typeindex>

#include "config.hpp"

namespace compute_graph {

class bad_type_erased_cast : public std::bad_cast {};

class TypeErasedPtr {
public:
  CG_STRONG_INLINE void *get() noexcept { return data_; }

  template <typename T> CG_STRONG_INLINE auto *as() CG_NOEXCEPT {
    using U = std::decay_t<T>;
#ifdef CG_NO_CHECK
    return static_cast<U *>(data_);
#else
    if (type_ == typeid(U)) {
      return static_cast<U *>(data_);
    } else {
      CG_THROW(bad_type_erased_cast);
    }
#endif
  }

  CG_STRONG_INLINE const void *get() const noexcept { return data_; }

  template <typename T> CG_STRONG_INLINE const auto *as() const CG_NOEXCEPT {
    using U = std::decay_t<T>;
#ifdef CG_NO_CHECK
    return static_cast<const U *>(data_);
#else
    if (type_ == typeid(U)) {
      return static_cast<const U *>(data_);
    } else {
      CG_THROW(bad_type_erased_cast);
    }
#endif
  }

  CG_STRONG_INLINE TypeIndex type() const noexcept { return type_; }

  CG_STRONG_INLINE ~TypeErasedPtr() {
    if (deleter_ && data_) {
      deleter_(data_);
    }
  }

  CG_STRONG_INLINE TypeErasedPtr(TypeErasedPtr const &) = delete;
  CG_STRONG_INLINE TypeErasedPtr &operator=(TypeErasedPtr const &) = delete;
  CG_STRONG_INLINE TypeErasedPtr &operator=(TypeErasedPtr &&) = delete;
  CG_STRONG_INLINE TypeErasedPtr(TypeErasedPtr &&another) noexcept
      : data_(another.data_), type_(another.type_), deleter_(std::move(another.deleter_)) {
    another.data_ = nullptr;
  }

  CG_STRONG_INLINE explicit TypeErasedPtr(TypeIndex type_index, void *data,
                                          std::function<void(void *)> deleter)
      : data_(data), type_(type_index), deleter_(std::move(deleter)) {}

protected:
  void *data_;
  const TypeIndex type_;
  std::function<void(void *)> deleter_;
};

template <typename T, typename... Args,
          typename = std::enable_if_t<std::is_default_constructible_v<T>>>
CG_STRONG_INLINE TypeErasedPtr make_type_erased_ptr(Args &&...args) {
  static_assert(std::is_same_v<T, std::decay_t<T>>, "T must be a decayed type");
  return TypeErasedPtr(typeid(T), new T(std::forward<Args>(args)...),
                       [](void *data) { delete static_cast<T *>(data); });
}

}  // namespace compute_graph
