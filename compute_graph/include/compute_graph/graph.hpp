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
#include "compute_graph/socket.hpp"
#include "intern/config.hpp"
#include "node.hpp"
#include <any>
#include <memory>
#include <vector>

namespace compute_graph {

class InputSocketHandle;
class OutputSocketHandle;

class NodeHandle {
public:
  NodeHandle(NodeHandle const &) = default;
  NodeHandle(NodeHandle &&) = default;
  NodeHandle &operator=(NodeHandle const &) = delete;
  NodeHandle &operator=(NodeHandle &&) = delete;

  NodeBase const &operator*() const noexcept { return node_; }
  NodeBase &operator*() noexcept { return node_; }
  NodeBase const *operator->() const noexcept { return &node_; }
  NodeBase *operator->() noexcept { return &node_; }
  NodeBase &node() noexcept { return node_; }
  NodeBase const &node() const noexcept { return node_; }

  size_t index() const noexcept { return index_; }

  InputSocketHandle input(size_t index);
  OutputSocketHandle output(size_t index);

  template <typename MT> InputSocketHandle input(MT);
  template <typename MT> OutputSocketHandle output(MT);

private:
  NodeHandle(size_t index, NodeBase &node) : index_(index), node_(node) {}

  const size_t index_;
  NodeBase &node_;
  friend class Graph;
};

class InputSocketHandle {
public:
  InputSocket const &operator*() const noexcept {
    return node_.inputs()[index_];
  }
  InputSocket const *operator->() const noexcept {
    return &node_.inputs()[index_];
  }

  NodeBase &node() noexcept { return node_; }
  NodeBase const &node() const noexcept { return node_; }
  size_t index() const noexcept { return index_; }

private:
  InputSocketHandle(NodeBase &node, size_t index):
      node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle;
  friend class Link;

  NodeBase &node_;
  size_t const index_;
};

class OutputSocketHandle {
public:
  OutputSocket const &operator*() const noexcept {
    return node_.outputs()[index_];
  }
  OutputSocket const *operator->() const noexcept {
    return &node_.outputs()[index_];
  }

  NodeBase &node() noexcept { return node_; }
  NodeBase const &node() const noexcept { return node_; }
  size_t index() const noexcept { return index_; }

private:
  OutputSocketHandle(NodeBase &node, size_t index):
      node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle;
  friend class Link;
  NodeBase &node_;
  size_t const index_;
};

class Link {
public:
  OutputSocketHandle from() const noexcept {
    return {from_, from_index_};
  }
  InputSocketHandle to() const noexcept {
    return {to_, to_index_};
  }

private:
  Link(NodeBase &from, size_t from_index, NodeBase &to, size_t to_index):
      from_(from), to_(to), from_index_(from_index), to_index_(to_index) {}

  friend class Graph;
  NodeBase &from_, &to_;
  size_t const from_index_, to_index_;
};

class Graph {
public:
  using node_ptr = std::unique_ptr<NodeBase>;
  using node_container = std::vector<node_ptr>;
  using ctx_type = std::unordered_map<std::string, std::any>;
  using id_container = std::vector<size_t>;
  using addr_to_index_map = std::unordered_map<NodeBase const *, size_t>;

  template <typename T>
  using typed_map = std::unordered_map<utype_ident, T>;

  Graph() = default;
  ~Graph() { clear(); }
  void clear();
  size_t num_nodes() const noexcept { return nodes_.size() - free_ids_.size(); }
  size_t num_links() const noexcept { return link_size_; }

  node_container const &nodes() const noexcept { return nodes_; }
  node_container &nodes() noexcept { return nodes_; }

  // node op.
  NodeHandle add(std::unique_ptr<NodeBase> node);
  void erase(NodeHandle handle);

  // socket op
  Link connect(OutputSocketHandle from, InputSocketHandle to);
  bool has_connect(OutputSocketHandle from, InputSocketHandle to) const noexcept;
  void erase(Link link);

  // context op
  ctx_type &ctx() noexcept { return ctx_; }

  void topology_sort();
  std::vector<size_t> topology_order() const noexcept;
  bool has_cycle() const noexcept;

private:
  void rebuild_addr_to_index() noexcept;

  node_container nodes_;
  ctx_type ctx_;
  id_container free_ids_;
  addr_to_index_map addr_to_index_;
  size_t uid_next_ = 0;
  size_t link_size_ = 0;
};


inline bool can_connect(OutputSocket const &from, InputSocket const &to) noexcept {
  return from.type() == to.type();
}

inline InputSocketHandle NodeHandle::input(size_t index) {
  return {node_, index};
}

template<typename MT> InputSocketHandle NodeHandle::input(MT) {
  return input(MT::index);
}

inline OutputSocketHandle NodeHandle::output(size_t index) {
  return {node_, index};
}

template<typename MT> OutputSocketHandle NodeHandle::output(MT) {
  return output(MT::index);
}

inline void Graph::clear() {
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

inline NodeHandle Graph::add(std::unique_ptr<NodeBase> node) {
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

inline void Graph::erase(NodeHandle handle) {
  size_t const index = handle.index();
  for (size_t i = 0; i < handle->inputs().size(); ++i) {
    auto const &input_sock = handle->inputs()[i];
    if (input_sock.is_connected()) {
      auto const& output_sock = *input_sock.from();
      erase(Link{output_sock.node(), output_sock.index(), handle.node(), i});
    }
  }

  for (size_t i = 0; i < handle->outputs().size(); ++i) {
    auto const &output_sock = handle->outputs()[i];
    for (auto const &input_sock : output_sock.connected_sockets()) {
      erase(Link{handle.node(), i, input_sock->node(), input_sock->index()});
    }
  }

  // TODO: callback.
  addr_to_index_.erase(nodes_[index].get());
  nodes_[index].reset();
  free_ids_.push_back(index);
}

inline Link Graph::connect(OutputSocketHandle from, InputSocketHandle to) {
  if (!can_connect(*from, *to)) {
    throw std::invalid_argument("Cannot connect sockets of different types." + to_string(from->type()) + " and " + to_string(to->type()));
  }

  // If already connected, erase the old link.
  if (to->is_connected()) {
    OutputSocket const* previous_from = to->from();
    erase(Link{previous_from->node(), previous_from->index(), to->node(), to->index()});
  }

  // add link in between.
  auto &from_node = from->node(), &to_node = to->node();
  from_node.outputs_[from.index()].connect(*to);
  to_node.inputs_[to.index()].connect(from.operator->());
  ++link_size_;

  // TODO: callback.
  return Link{from_node, from.index(), to_node, to.index()};
}

inline bool Graph::has_connect(OutputSocketHandle from, InputSocketHandle to) const noexcept {
  return to->from() == from.operator->();
}

inline void Graph::erase(Link link) {
  auto &from = link.from().node();
  auto &to = link.to().node();
  auto &from_sock = from.outputs_[link.from().index()];
  auto &to_sock = to.inputs_[link.to().index()];

  // TODO: callback.
  from_sock.erase(to_sock);
  to_sock.clear();
  --link_size_;
}


inline bool Graph::has_cycle() const noexcept {
  return topology_order().empty();
}

inline void Graph::rebuild_addr_to_index() noexcept {
  addr_to_index_.clear();
  addr_to_index_.reserve(nodes_.size());
  for (size_t i = 0; i < nodes_.size(); ++i) {
    addr_to_index_[nodes_[i].get()] = i;
  }
}

inline void Graph::topology_sort() {
  auto const order = topology_order();
  node_container new_nodes;
  new_nodes.reserve(nodes_.size());
  for (size_t i: order) {
    new_nodes.push_back(std::move(nodes_[i]));
  }
  nodes_ = std::move(new_nodes);
}

inline std::vector<size_t> Graph::topology_order() const noexcept {
  std::vector<size_t> result;
  size_t const n = nodes_.size();
  result.reserve(n);
  std::vector<size_t> connected_count(n, 0);
  for (size_t i = 0; i < n; ++i) {
    auto const& node = nodes_[i];
    size_t count = 0;
    for (auto const& input: node->inputs()) {
      count += input.is_connected() ? 1 : 0;
    }
    connected_count[i] = count;
    if (count == 0) {
      result.push_back(i);
    }
  }

  for (size_t i = 0; i < result.size(); ++i) {
    auto const& node = nodes_[result[i]];
    for (auto const& output: node->outputs()) {
      for (auto const* to_socket: output.connected_sockets()) {
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

}
