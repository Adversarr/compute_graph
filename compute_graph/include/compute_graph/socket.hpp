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
#include <algorithm>
#include <memory>
#include <vector>

#include "intern/config.hpp"

namespace compute_graph {

using TypeErasedPtr = std::shared_ptr<void>;

class OutputSocket {
public:
  template <typename T, typename... Args> T &emplace(Args &&...args) {
#ifndef CG_NO_CHECK
    if (typeid(T) != type_) {
      CG_THROW(std::invalid_argument, "Type mismatch");
    }
#endif
    payload_ = std::make_shared<T>(std::forward<Args>(args)...);
    return *static_cast<T*>(payload_.get());
  }

  CG_STRONG_INLINE TypeIndex const &type() const noexcept { return type_; }
  CG_STRONG_INLINE auto const &connected_sockets() const noexcept { return connected_sockets_; }

  CG_STRONG_INLINE TypeErasedPtr const &payload() const noexcept { return payload_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }
  CG_STRONG_INLINE NodeBase &node() const noexcept { return node_; }
  CG_STRONG_INLINE void clear() noexcept { payload_.reset(); }

  CG_STRONG_INLINE OutputSocket(OutputSocket const &) = delete;
  CG_STRONG_INLINE OutputSocket(OutputSocket &&) noexcept = default;
  CG_STRONG_INLINE OutputSocket &operator=(OutputSocket const &) = delete;
  CG_STRONG_INLINE OutputSocket &operator=(OutputSocket &&) = delete;

private:
  CG_STRONG_INLINE OutputSocket(TypeIndex type, NodeBase &node, size_t index) noexcept
      : type_(type), connected_sockets_{}, payload_{nullptr}, node_(node), index_(index) {}

  CG_STRONG_INLINE void erase(InputSocket const &to) noexcept {
    connected_sockets_.erase(std::remove_if(connected_sockets_.begin(), connected_sockets_.end(),
                                            [&to](auto const &socket) { return socket == &to; }),
                             connected_sockets_.end());
  }

  CG_STRONG_INLINE TypeErasedPtr &payload() noexcept { return payload_; }
  CG_STRONG_INLINE void connect(InputSocket const &to) noexcept {
    connected_sockets_.push_back(&to);
  }

  friend class NodeBase;
  friend class Graph;
  TypeIndex const type_;
  std::vector<InputSocket const *> connected_sockets_;
  TypeErasedPtr payload_;
  NodeBase &node_;
  size_t const index_;
};

class InputSocket {
public:
  CG_STRONG_INLINE TypeIndex const &type() const noexcept { return type_; }
  CG_STRONG_INLINE TypeErasedPtr const &fetch_payload() const noexcept { return from_->payload(); }

  CG_STRONG_INLINE bool is_connected() const noexcept { return from_ != nullptr; }
  CG_STRONG_INLINE bool is_empty() const noexcept {
    return !is_connected() || !static_cast<bool>(fetch_payload());
  }

  CG_STRONG_INLINE OutputSocket const *from() const noexcept { return from_; }
  CG_STRONG_INLINE NodeBase &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

  CG_STRONG_INLINE InputSocket(InputSocket const &) = delete;
  CG_STRONG_INLINE InputSocket(InputSocket &&) noexcept = default;
  CG_STRONG_INLINE InputSocket &operator=(InputSocket const &) = delete;
  CG_STRONG_INLINE InputSocket &operator=(InputSocket &&) = delete;

private:
  CG_STRONG_INLINE InputSocket(TypeIndex type, NodeBase &node, size_t const index) noexcept
      : type_(type), node_(node), index_(index), from_{nullptr} {}
  CG_STRONG_INLINE void clear() noexcept { from_ = nullptr; }
  CG_STRONG_INLINE void connect(OutputSocket const *from) noexcept { from_ = from; }

  friend class NodeBase;
  friend class Graph;
  const TypeIndex type_;
  NodeBase &node_;
  size_t const index_;
  OutputSocket const *from_;
};

}  // namespace compute_graph
