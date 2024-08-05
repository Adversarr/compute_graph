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
#include <any>
#include <memory>
#include <vector>

#include "socket.hpp"
#include "intern/config.hpp"
#include "node.hpp"

#define CG_PP_HANDLE_COMMON(HandleType)               \
  HandleType(HandleType const &) noexcept = default;  \
  HandleType(HandleType &&) noexcept = default;       \
  HandleType &operator=(HandleType const &) = delete; \
  HandleType &operator=(HandleType &&) = delete

namespace compute_graph {

class InputSocketHandle;
class OutputSocketHandle;

class NodeHandle {
public:
  CG_PP_HANDLE_COMMON(NodeHandle);
  CG_STRONG_INLINE NodeBase const &operator*() const noexcept { return node_; }
  CG_STRONG_INLINE NodeBase &operator*() noexcept { return node_; }
  CG_STRONG_INLINE NodeBase const *operator->() const noexcept { return &node_; }
  CG_STRONG_INLINE NodeBase *operator->() noexcept { return &node_; }
  CG_STRONG_INLINE NodeBase &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeBase const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

  std::optional<InputSocketHandle> input(std::string_view name) noexcept;
  std::optional<OutputSocketHandle> output(std::string_view name) noexcept;

  InputSocketHandle input(size_t index);
  OutputSocketHandle output(size_t index);

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  InputSocketHandle input(MT);
  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  OutputSocketHandle output(MT);

private:
  NodeHandle(size_t index, NodeBase &node) : index_(index), node_(node) {}

  const size_t index_;
  NodeBase &node_;
  friend class Graph;
};

class InputSocketHandle {
public:
  CG_PP_HANDLE_COMMON(InputSocketHandle);

  CG_STRONG_INLINE InputSocket const &operator*() const noexcept { return node_.inputs()[index_]; }
  CG_STRONG_INLINE InputSocket const *operator->() const noexcept {
    return &node_.inputs()[index_];
  }

  CG_STRONG_INLINE NodeBase &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeBase const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

private:
  CG_STRONG_INLINE InputSocketHandle(NodeBase &node, size_t index) : node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle;
  friend class Link;

  NodeBase &node_;
  size_t const index_;
};

class OutputSocketHandle {
public:
  CG_PP_HANDLE_COMMON(OutputSocketHandle);

  CG_STRONG_INLINE OutputSocket const &operator*() const noexcept {
    return node_.outputs()[index_];
  }
  CG_STRONG_INLINE OutputSocket const *operator->() const noexcept {
    return &node_.outputs()[index_];
  }

  CG_STRONG_INLINE NodeBase &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeBase const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

private:
  CG_STRONG_INLINE OutputSocketHandle(NodeBase &node, size_t index) : node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle;
  friend class Link;
  NodeBase &node_;
  size_t const index_;
};

class Link {
public:
  CG_PP_HANDLE_COMMON(Link);
  CG_STRONG_INLINE OutputSocketHandle const& from() const noexcept { return from_; }
  CG_STRONG_INLINE InputSocketHandle const& to() const noexcept { return to_; }

private:
  CG_STRONG_INLINE Link(OutputSocketHandle from, InputSocketHandle to) noexcept
      : from_(from), to_(to) {}

  friend class Graph;
  OutputSocketHandle from_;
  InputSocketHandle to_;
};

class Graph {
public:
  using node_ptr = std::unique_ptr<NodeBase>;
  using node_container = std::vector<node_ptr>;
  using ctx_type = std::unordered_map<std::string, std::any>;
  using id_container = std::vector<size_t>;
  using node_addr_to_index_map = std::unordered_map<NodeBase const *, size_t>;

  Graph() = default;
  ~Graph() noexcept { clear(); }
  void clear() noexcept;
  CG_STRONG_INLINE size_t num_nodes() const noexcept { return nodes_.size() - free_ids_.size(); }
  CG_STRONG_INLINE size_t num_links() const noexcept { return link_size_; }

  CG_STRONG_INLINE node_container const &nodes() const noexcept { return nodes_; }
  CG_STRONG_INLINE node_container &nodes() noexcept { return nodes_; }

  // node op.
  NodeHandle add(std::unique_ptr<NodeBase> node);
  std::optional<NodeHandle> get(NodeBase const *ptr) const noexcept;
  void erase(NodeHandle handle) noexcept;

  // socket op
  Link connect(OutputSocketHandle from, InputSocketHandle to);
  bool has_connect(OutputSocketHandle const& from, InputSocketHandle const& to) const noexcept;
  void erase(Link link) noexcept;

  // context op
  ctx_type &ctx() noexcept { return ctx_; }

  void topology_sort();
  std::vector<size_t> topology_order() const noexcept;
  bool has_cycle() const noexcept;

private:
  void rebuild_addr_to_index() noexcept;

  node_container nodes_{};
  ctx_type ctx_{};
  id_container free_ids_{};
  node_addr_to_index_map addr_to_index_{};
  size_t uid_next_ = 0;
  size_t link_size_ = 0;
};

CG_STRONG_INLINE bool can_connect(OutputSocket const &from, InputSocket const &to) noexcept {
  return from.type() == to.type();
}

CG_STRONG_INLINE std::optional<InputSocketHandle> NodeHandle::input(
    std::string_view name) noexcept {
  if (auto const index = node_.find_input(name)) {
    return input(*index);
  } else {
    return std::nullopt;
  }
}

CG_STRONG_INLINE std::optional<OutputSocketHandle> NodeHandle::output(
    std::string_view name) noexcept {
  if (const auto index = node_.find_output(name)) {
    return output(*index);
  } else {
    return std::nullopt;
  }
}

CG_STRONG_INLINE InputSocketHandle NodeHandle::input(size_t index) { return {node_, index}; }

template <typename MT, typename> CG_STRONG_INLINE InputSocketHandle NodeHandle::input(MT) {
  return input(MT::index);
}

CG_STRONG_INLINE OutputSocketHandle NodeHandle::output(size_t index) { return {node_, index}; }

template <typename MT, typename> CG_STRONG_INLINE OutputSocketHandle NodeHandle::output(MT) {
  return output(MT::index);
}

CG_STRONG_INLINE void Graph::clear() noexcept {
  while (!nodes_.empty()) {
    if (nodes_.back()) {
      erase(NodeHandle{nodes_.size() - 1, *nodes_.back()});
    }
    nodes_.pop_back();
  }
  nodes_.clear();
  free_ids_.clear();
  addr_to_index_.clear();
  uid_next_ = 0;
  link_size_ = 0;
  ctx_.clear();
}

CG_STRONG_INLINE NodeHandle Graph::add(std::unique_ptr<NodeBase> node) {
  if (free_ids_.empty()) {
    nodes_.push_back(std::move(node));
    addr_to_index_.insert({nodes_.back().get(), nodes_.size() - 1});
    return {uid_next_++, *nodes_.back()};
  } else {
    size_t const index = free_ids_.back();
    free_ids_.pop_back();
    nodes_[index] = std::move(node);
    addr_to_index_.insert({nodes_[index].get(), index});
    return {index, *nodes_[index]};
  }
}

CG_STRONG_INLINE std::optional<NodeHandle> Graph::get(NodeBase const *ptr) const noexcept {
  if (auto it = addr_to_index_.find(ptr); it != addr_to_index_.end()) {
    return NodeHandle{it->second, *nodes_[it->second]};
  } else {
    return std::nullopt;
  }
}

CG_STRONG_INLINE void Graph::erase(NodeHandle handle) noexcept {
  size_t const index = handle.index();
  for (size_t i = 0; i < handle->inputs().size(); ++i) {
    auto const &input_sock = handle->inputs()[i];
    if (input_sock.is_connected()) {
      auto const &output_sock = *input_sock.from();
      erase(Link{{output_sock.node(), output_sock.index()}, {handle.node(), i}});
    }
  }

  for (size_t i = 0; i < handle->outputs().size(); ++i) {
    auto const &output_sock = handle->outputs()[i];
    for (auto const &input_sock : output_sock.connected_sockets()) {
      erase(Link{{handle.node(), i}, {input_sock->node(), input_sock->index()}});
    }
  }

  addr_to_index_.erase(nodes_[index].get());
  nodes_[index].reset();
  free_ids_.push_back(index);
}

CG_STRONG_INLINE Link Graph::connect(OutputSocketHandle from, InputSocketHandle to) CG_NOEXCEPT {
  if (!can_connect(*from, *to)) {
    CG_THROW(std::invalid_argument, "Cannot connect sockets of different types."
                                        + to_string(from->type()) + " and "
                                        + to_string(to->type()));
  }

  // If already connected, erase the old link.
  if (to->is_connected()) {
    OutputSocket const *previous_from = to->from();
    erase(Link{{previous_from->node(), previous_from->index()}, {to->node(), to->index()}});
  }

  // add link in between.
  auto &from_node = from->node(), &to_node = to->node();
  from_node.outputs_[from.index()].connect(*to);
  to_node.inputs_[to.index()].connect(from.operator->());
  ++link_size_;

  to->node().on_connect(to.index());
  return Link{{from_node, from.index()}, {to_node, to.index()}};
}

CG_STRONG_INLINE bool Graph::has_connect(OutputSocketHandle const& from,
                                         InputSocketHandle const& to) const noexcept {
  return to->from() == from.operator->();
}

CG_STRONG_INLINE void Graph::erase(Link link) noexcept {
  auto &from = link.from_.node_;
  auto &to = link.to_.node_;
  auto &from_sock = from.outputs_[link.from().index()];
  auto &to_sock = to.inputs_[link.to().index()];

  from_sock.erase(to_sock);
  to_sock.clear();
  --link_size_;
}

CG_STRONG_INLINE bool Graph::has_cycle() const noexcept { return topology_order().empty(); }

CG_STRONG_INLINE void Graph::rebuild_addr_to_index() noexcept {
  addr_to_index_.clear();
  addr_to_index_.reserve(nodes_.size());
  for (size_t i = 0; i < nodes_.size(); ++i) {
    addr_to_index_[nodes_[i].get()] = i;
  }
}

CG_STRONG_INLINE void Graph::topology_sort() {
  auto const order = topology_order();
  node_container new_nodes;
  new_nodes.reserve(nodes_.size());
  for (size_t i : order) {
    new_nodes.push_back(std::move(nodes_[i]));
  }
  nodes_ = std::move(new_nodes);
}

CG_STRONG_INLINE std::vector<size_t> Graph::topology_order() const noexcept {
  std::vector<size_t> result;
  size_t const n = nodes_.size();
  result.reserve(n);
  std::vector<size_t> connected_count(n, 0);
  for (size_t i = 0; i < n; ++i) {
    auto const &node = nodes_[i];
    size_t count = 0;
    for (auto const &input : node->inputs()) {
      count += input.is_connected() ? 1 : 0;
    }
    connected_count[i] = count;
    if (count == 0) {
      result.push_back(i);
    }
  }

  for (size_t i = 0; i < result.size(); ++i) {
    auto const &node = nodes_[result[i]];
    for (auto const &output : node->outputs()) {
      for (auto const *to_socket : output.connected_sockets()) {
        if (size_t const to_index = addr_to_index_.at(&(to_socket->node()));
            --connected_count[to_index] == 0) {
          result.push_back(to_index);
        }
      }
    }
  }

  if (result.size() == n) {
    return result;
  } else {
    return {};
  }
}

}  // namespace compute_graph
