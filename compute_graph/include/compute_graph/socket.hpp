#pragma once

#include "intern/config.hpp"
#include "intern/type_erased_ptr.hpp"
#include <optional>

CG_NAMESPACE_BEGIN

using Payload = std::optional<TypeErasedPtr>;

class OutputSocket final {
public:
  std::type_index type() const noexcept { return type_; }

  auto const &connected_sockets() const noexcept { return connected_sockets_; }

  template <typename T, typename... Args> T &emplace(Args &&...args) {
    auto &ptr =
        payload_.emplace(make_type_erased_ptr<T>(std::forward<Args>(args)...));
    return *ptr.template as<T>();
  }

  Payload const &payload() const noexcept { return payload_; }
  Payload &payload() noexcept { return payload_; }

private:
  explicit OutputSocket(std::type_index type) noexcept : type_(type) {}
  friend class NodeBase;
  friend class Graph;
  void connect(InputSocket const &to) noexcept {
    connected_sockets_.push_back(&to);
  }

  const std::type_index type_;
  std::vector<InputSocket const *> connected_sockets_;
  Payload payload_;
};

class InputSocket final {
public:
  std::type_index type() const noexcept { return type_; }

  Payload const &fetch() const noexcept { return from_->payload(); }

  bool is_connected() const noexcept { return from_ != nullptr; }

  bool is_empty() const noexcept {
    return !is_connected() || !fetch().has_value();
  }

private:
  friend class NodeBase;
  friend class Graph;
  explicit InputSocket(std::type_index type) noexcept : type_(type) {}
  void connect(OutputSocket const *from) noexcept { from_ = from; }

  const std::type_index type_;
  OutputSocket const *from_;
};

CG_NAMESPACE_END
