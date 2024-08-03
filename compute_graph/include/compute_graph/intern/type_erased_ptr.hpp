#pragma once
#include "config.hpp"
#include <functional>
#include <typeindex>

CG_NAMESPACE_BEGIN

class bad_type_erased_cast : public std::bad_cast {};

class TypeErasedPtr {
public:
  void *get() noexcept { return data_; }

  template <typename T> auto *as() {
    using U = std::decay_t<T>;
    if (type_ == typeid(U)) {
      return static_cast<U *>(data_);
    } else {
      throw bad_type_erased_cast();
    }
  }

  const void *get() const noexcept { return data_; }

  template <typename T> const auto *as() const {
    using U = std::decay_t<T>;
    if (type_ == typeid(U)) {
      return static_cast<const U *>(data_);
    } else {
      throw bad_type_erased_cast();
    }
  }

  std::type_index type() const noexcept { return type_; }

  ~TypeErasedPtr() {
    if (deleter_ && data_) {
      deleter_(data_);
    }
  }

  TypeErasedPtr(TypeErasedPtr const &) = delete;
  TypeErasedPtr &operator=(TypeErasedPtr const &) = delete;
  TypeErasedPtr &operator=(TypeErasedPtr &&) = delete;
  TypeErasedPtr(TypeErasedPtr &&) noexcept =
      default; // only allow move construct

  explicit TypeErasedPtr(std::type_index type_index, void *data,
                         std::function<void(void *)> deleter)
      : data_(data), type_(type_index), deleter_(std::move(deleter)) {}

protected:
  TypeErasedPtr(void *data, const std::type_index &type) noexcept
      : data_(data), type_(type), deleter_(nullptr) {}

  TypeErasedPtr(void *data, const std::type_index &type,
                std::function<void(void *)> deleter) noexcept
      : data_(data), type_(type), deleter_(std::move(deleter)) {}

  void *data_;
  const std::type_index type_;
  std::function<void(void *)> deleter_;
};

template <typename T, typename... Args>
TypeErasedPtr make_type_erased_ptr(Args &&...args) {
  static_assert(std::is_same_v<T, std::decay_t<T>>,
                "T must be a non-reference type");
  return TypeErasedPtr(typeid(T), new T(std::forward<Args>(args)...),
                       [](void *data) { delete static_cast<T *>(data); });
}

template <typename T, typename... Args>
TypeErasedPtr make_type_erased_ptr(Args &&...args,
                                   std::function<void(void *)> deleter) {
  static_assert(std::is_same_v<T, std::decay_t<T>>,
                "T must be a non-reference type");
  return TypeErasedPtr(typeid(T), new T(std::forward<Args>(args)...),
                       std::move(deleter));
}

CG_NAMESPACE_END
