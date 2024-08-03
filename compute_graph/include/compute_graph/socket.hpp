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
#include <optional>
#include <algorithm>

#include "intern/config.hpp"
#include "intern/type_erased_ptr.hpp"

namespace compute_graph {

using Payload = std::optional<TypeErasedPtr>;

class OutputSocket final {
public:
  utype_ident type() const noexcept { return type_; }

  auto const &connected_sockets() const noexcept { return connected_sockets_; }

  template <typename T, typename... Args> T &emplace(Args &&...args) {
    auto ptr = make_type_erased_ptr<T>(std::forward<Args>(args)...);
    return *(payload_.emplace(std::move(ptr)).template as<T>());
  }

  Payload const &payload() const noexcept { return payload_; }
  Payload &payload() noexcept { return payload_; }

  size_t index() const noexcept { return index_; }
  NodeBase& node() const noexcept { return node_; }
  OutputSocket(OutputSocket&& ) = default;

private:
  OutputSocket(utype_ident type, NodeBase& node, size_t index) noexcept:
    type_(type), node_(node), index_(index) {}

  void erase(InputSocket const& to) noexcept {
    connected_sockets_.erase(
        std::remove_if(connected_sockets_.begin(), connected_sockets_.end(),
                       [&to](auto const &socket) { return socket == &to; }),
        connected_sockets_.end());
  }


  void connect(InputSocket const &to) noexcept {
    connected_sockets_.push_back(&to);
  }

  friend class NodeBase;
  friend class Graph;
  utype_ident const type_;
  std::vector<InputSocket const *> connected_sockets_;
  Payload payload_;
  NodeBase &node_;
  size_t const index_;
};

class InputSocket final {
public:
  utype_ident type() const noexcept { return type_; }

  Payload const &fetch() const noexcept { return from_->payload(); }

  bool is_connected() const noexcept { return from_ != nullptr; }

  bool is_empty() const noexcept {
    return !is_connected() || !fetch().has_value();
  }

  OutputSocket const *from() const noexcept { return from_; }

  NodeBase &node() const noexcept { return node_; }
  size_t index() const noexcept { return index_; }

  InputSocket(InputSocket&& ) = default;

private:
  InputSocket(utype_ident type, NodeBase& node, size_t const index) noexcept:
    type_(type), from_{nullptr}, node_(node), index_(index) {}
  void clear() noexcept { from_ = nullptr; }
  void connect(OutputSocket const *from) noexcept { from_ = from; }

  friend class NodeBase;
  friend class Graph;
  const utype_ident type_;
  OutputSocket const *from_;
  NodeBase &node_;
  size_t const index_;
};

}
