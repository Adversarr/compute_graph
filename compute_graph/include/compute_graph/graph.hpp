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

#include "intern/config.hpp"
#include "node.hpp"
#include "socket.hpp"

#define CG_PP_HANDLE_COMMON(HandleType)               \
  HandleType(HandleType const &) noexcept = default;  \
  HandleType(HandleType &&) noexcept = default;       \
  HandleType &operator=(HandleType const &) = delete; \
  HandleType &operator=(HandleType &&) = delete

namespace compute_graph {
template <typename NodeType = NodeBase> class InputSocketHandle;
template <typename NodeType = NodeBase> class OutputSocketHandle;
template <typename NodeType = NodeBase> class NodeHandle;
template <typename From = NodeBase, typename To = NodeBase> class Link;

template <typename NodeType> class NodeHandle {
public:
  CG_PP_HANDLE_COMMON(NodeHandle);

  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<AnotherNodeType, NodeType>>>
  CG_STRONG_INLINE NodeHandle<AnotherNodeType> cast() const noexcept {
    return {static_cast<AnotherNodeType &>(node_), index_};
  }

  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<NodeType, AnotherNodeType>>>
  CG_STRONG_INLINE NodeHandle(NodeHandle<AnotherNodeType> const& another) noexcept:
        NodeHandle(another.template cast<NodeType>()) {}

  CG_STRONG_INLINE NodeType const &operator*() const noexcept { return node_; }
  CG_STRONG_INLINE NodeType &operator*() noexcept { return node_; }
  CG_STRONG_INLINE NodeType const *operator->() const noexcept { return &node_; }
  CG_STRONG_INLINE NodeType *operator->() noexcept { return &node_; }
  CG_STRONG_INLINE NodeType &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeType const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

  std::optional<InputSocketHandle<NodeType>> input(std::string_view name) noexcept;
  std::optional<OutputSocketHandle<NodeType>> output(std::string_view name) noexcept;

  InputSocketHandle<NodeType> input(size_t index);
  OutputSocketHandle<NodeType> output(size_t index);

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  InputSocketHandle<NodeType> input(MT);
  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  OutputSocketHandle<NodeType> output(MT);

private:
  NodeHandle(NodeType &node, size_t index) : node_(node), index_(index) {}

  NodeType &node_;
  const size_t index_;
  template <typename T> friend class NodeHandle;
  friend class Graph;
};

template <typename NodeType> class InputSocketHandle {
public:
  CG_PP_HANDLE_COMMON(InputSocketHandle);
  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<AnotherNodeType, NodeType>>>
  CG_STRONG_INLINE InputSocketHandle<AnotherNodeType> cast() const noexcept {
    return {static_cast<AnotherNodeType &>(node_), index_};
  }

  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<NodeType, AnotherNodeType>>>
  CG_STRONG_INLINE InputSocketHandle(NodeHandle<AnotherNodeType> const& another) noexcept:
        InputSocketHandle(another.template cast<NodeType>()) {}

  CG_STRONG_INLINE InputSocket const &operator*() const noexcept { return node_.inputs()[index_]; }
  CG_STRONG_INLINE InputSocket const *operator->() const noexcept {
    return &node_.inputs()[index_];
  }

  CG_STRONG_INLINE NodeType &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeType const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

private:
  CG_STRONG_INLINE InputSocketHandle(NodeType &node, size_t index) : node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle<NodeType>;
  template <typename T> friend class InputSocketHandle;
  template <typename From, typename To> friend class Link;

  NodeType &node_;
  size_t const index_;
};

template <typename NodeType> class OutputSocketHandle {
public:
  CG_PP_HANDLE_COMMON(OutputSocketHandle);
  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<AnotherNodeType, NodeType>>>
  CG_STRONG_INLINE OutputSocketHandle<AnotherNodeType> cast() const noexcept {
    return {static_cast<AnotherNodeType &>(node_), index_};
  }

  template <typename AnotherNodeType,
            typename = std::enable_if_t<std::is_base_of_v<NodeType, AnotherNodeType>>>
  CG_STRONG_INLINE OutputSocketHandle(NodeHandle<AnotherNodeType> const& another) noexcept:
        OutputSocketHandle(another.template cast<NodeType>()) {}

  CG_STRONG_INLINE OutputSocket const &operator*() const noexcept {
    return node_.outputs()[index_];
  }
  CG_STRONG_INLINE OutputSocket const *operator->() const noexcept {
    return &node_.outputs()[index_];
  }

  CG_STRONG_INLINE NodeType &node() noexcept { return node_; }
  CG_STRONG_INLINE NodeType const &node() const noexcept { return node_; }
  CG_STRONG_INLINE size_t index() const noexcept { return index_; }

private:
  CG_STRONG_INLINE OutputSocketHandle(NodeType &node, size_t index) : node_(node), index_(index) {}

  friend class Graph;
  friend class NodeHandle<NodeType>;
  template <typename T> friend class OutputSocketHandle;
  template <typename From, typename To> friend class Link;
  NodeType &node_;
  size_t const index_;
};

template <typename From, typename To> class Link {
public:
  using FromType = OutputSocketHandle<From>;
  using ToType = InputSocketHandle<To>;


  template <typename AnotherFrom, typename AnotherTo>
  CG_STRONG_INLINE Link<AnotherFrom, AnotherTo> cast() const noexcept {
    return {from_.template cast<AnotherFrom>(), to_.template cast<AnotherTo>()};
  }

  template <typename AnotherFrom, typename AnotherTo>
  CG_STRONG_INLINE Link(Link<AnotherFrom, AnotherTo> const& another) noexcept:
        Link(another.template cast<AnotherFrom, AnotherTo>()) {}

  CG_PP_HANDLE_COMMON(Link);
  CG_STRONG_INLINE FromType const &from() const noexcept { return from_; }
  CG_STRONG_INLINE ToType const &to() const noexcept { return to_; }

private:
  CG_STRONG_INLINE Link(FromType from, ToType to) noexcept : from_(from), to_(to) {}

  friend class Graph;
  FromType from_;
  ToType to_;
};

template <typename From, typename To>
class StrongLink {
public:
  using FromType = NodeHandle<From>;
  using ToType = NodeHandle<To>;

  template <typename AnotherFrom, typename AnotherTo>
  CG_STRONG_INLINE StrongLink<AnotherFrom, AnotherTo> cast() const noexcept {
    return {from_.template cast<AnotherFrom>(), to_.template cast<AnotherTo>()};
  }

  template <typename AnotherFrom, typename AnotherTo>
  CG_STRONG_INLINE StrongLink(StrongLink<AnotherFrom, AnotherTo> const& another) noexcept:
        StrongLink(another.template cast<AnotherFrom, AnotherTo>()) {}

  CG_PP_HANDLE_COMMON(StrongLink);
  CG_STRONG_INLINE FromType const &from() const noexcept { return from_; }
  CG_STRONG_INLINE ToType const &to() const noexcept { return to_; }
private:
  FromType from_;
  ToType to_;
};

class Graph {
public:
  using node_ptr = std::unique_ptr<NodeBase>;
  using node_container = std::vector<node_ptr>;
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
  template <typename NodeType> NodeHandle<NodeType> add(std::unique_ptr<NodeType> node);
  template <typename NodeType>
  std::optional<NodeHandle<NodeType>> get(NodeType const *ptr) const noexcept;
  template <typename NodeType> void erase(NodeHandle<NodeType> handle) noexcept;

  // strong link, describe node-wise connection.
  template <typename From, typename To>
  StrongLink<From, To> connect(NodeHandle<From> prec, NodeHandle<To> succ) noexcept;
  template <typename From, typename To>
  bool has_connect(NodeHandle<From> prec, NodeHandle<To> succ) noexcept;
  template <typename From, typename To>
  std::optional<StrongLink<From, To>> get_connect(NodeHandle<From> prec,
                                                  NodeHandle<To> succ) noexcept;
  template <typename From, typename To> void erase(StrongLink<From, To> link) noexcept;

  // socket op
  template <typename From, typename To>
  Link<From, To> connect(OutputSocketHandle<From> from, InputSocketHandle<To> to);
  template <typename From, typename To>
  bool has_connect(OutputSocketHandle<From> const &from,
                   InputSocketHandle<To> const &to) const noexcept;
  template <typename From, typename To>
  std::optional<Link<From, To>> get_connect(OutputSocketHandle<From> const &from,
                                            InputSocketHandle<To> const &to) const noexcept;
  template <typename From, typename To> void erase(Link<From, To> link) noexcept;

  void topology_sort();
  std::vector<size_t> topology_order() const noexcept;
  bool has_cycle() const noexcept;
  void shrink_to_fit() noexcept;

private:
  void rebuild_addr_to_index() noexcept;

  node_container nodes_{};
  id_container free_ids_{};
  node_addr_to_index_map addr_to_index_{};
  size_t uid_next_ = 0;
  size_t link_size_ = 0;
};

CG_STRONG_INLINE bool can_connect(OutputSocket const &from, InputSocket const &to) noexcept {
  return from.type() == to.type();
}

template <typename NodeType>
CG_STRONG_INLINE std::optional<InputSocketHandle<NodeType>> NodeHandle<NodeType>::input(
    std::string_view name) noexcept {
  if (auto const index = node_.find_input(name)) {
    return input(*index);
  } else {
    return std::nullopt;
  }
}

template <typename NodeType>
CG_STRONG_INLINE std::optional<OutputSocketHandle<NodeType>> NodeHandle<NodeType>::output(
    std::string_view name) noexcept {
  if (const auto index = node_.find_output(name)) {
    return output(*index);
  } else {
    return std::nullopt;
  }
}

template <typename NodeType>
CG_STRONG_INLINE InputSocketHandle<NodeType> NodeHandle<NodeType>::input(size_t index) {
  return {node_, index};
}

template <typename NodeType> template <typename MT, typename>
CG_STRONG_INLINE InputSocketHandle<NodeType> NodeHandle<NodeType>::input(MT) {
  return input(MT::index);
}

template <typename NodeType>
CG_STRONG_INLINE OutputSocketHandle<NodeType> NodeHandle<NodeType>::output(size_t index) {
  return {node_, index};
}

template <typename NodeType> template <typename MT, typename>
CG_STRONG_INLINE OutputSocketHandle<NodeType> NodeHandle<NodeType>::output(MT) {
  return output(MT::index);
}

CG_STRONG_INLINE void Graph::clear() noexcept {
  while (!nodes_.empty()) {
    if (nodes_.back()) {
      erase(NodeHandle{*nodes_.back(), nodes_.size() - 1});
    }
    nodes_.pop_back();
  }
  nodes_.clear();
  free_ids_.clear();
  addr_to_index_.clear();
  uid_next_ = 0;
  link_size_ = 0;
}

template <typename NodeType>
CG_STRONG_INLINE NodeHandle<NodeType> Graph::add(std::unique_ptr<NodeType> node) {
  if (free_ids_.empty()) {
    nodes_.push_back(std::move(node));
    addr_to_index_.insert({nodes_.back().get(), nodes_.size() - 1});
    return {*static_cast<NodeType*>(nodes_.back().get()), uid_next_++};
  } else {
    size_t const index = free_ids_.back();
    free_ids_.pop_back();
    nodes_[index] = std::move(node);
    addr_to_index_.insert({nodes_[index].get(), index});
    return {*static_cast<NodeType*>(nodes_[index].get()), index};
  }
}

template <typename NodeType>
CG_STRONG_INLINE std::optional<NodeHandle<NodeType>> Graph::get(
    NodeType const *ptr) const noexcept {
  if (auto it = addr_to_index_.find(ptr); it != addr_to_index_.end()) {
    return NodeHandle{it->second, *nodes_[it->second]};
  } else {
    return std::nullopt;
  }
}

template <typename NodeType>
CG_STRONG_INLINE void Graph::erase(NodeHandle<NodeType> handle) noexcept {
  size_t const index = handle.index();
  for (size_t i = 0; i < handle->inputs().size(); ++i) {
    if (auto const &input_sock = handle->inputs()[i]; input_sock.is_connected()) {
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

template <typename From, typename To>
CG_STRONG_INLINE StrongLink<From, To> Graph::connect(NodeHandle<From> prec,
                                                     NodeHandle<To> succ) noexcept {
  auto &n = prec.node();
  if (! has_connect(prec, succ)) {
    n.partial_succ_.push_back(&(succ.node()));
  }
  return {prec, succ};
}

template <typename From, typename To>
bool Graph::has_connect(NodeHandle<From> prec, NodeHandle<To> succ) noexcept {
  auto &n = prec.node();
  auto &succ_node = succ.node();
  return std::find(n.partial_succ_.begin(), n.partial_succ_.end(), &succ_node)
         != n.partial_succ_.end();
}

template <typename From, typename To>
std::optional<StrongLink<From, To>> Graph::get_connect(NodeHandle<From> prec,
                                                       NodeHandle<To> succ) noexcept {
  if (has_connect(prec, succ)) {
    return StrongLink{prec, succ};
  } else {
    return std::nullopt;
  }
}

template <typename From, typename To> void Graph::erase(StrongLink<From, To> link) noexcept {
  auto &n = link.from().node();
  auto &succ = link.to().node();
  std::erase(n.partial_succ_.begin(), n.partial_succ_.end(), &succ);
}

template <typename From, typename To>
CG_STRONG_INLINE Link<From, To> Graph::connect(OutputSocketHandle<From> from,
                                               InputSocketHandle<To> to) CG_NOEXCEPT {
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
  return {from, to};
}

template <typename From, typename To>
CG_STRONG_INLINE bool Graph::has_connect(OutputSocketHandle<From> const &from,
                                         InputSocketHandle<To> const &to) const noexcept {
  return to->from() == from.operator->();
}

template <typename From, typename To>
std::optional<Link<From, To>> Graph::get_connect(OutputSocketHandle<From> const &from,
                                                 InputSocketHandle<To> const &to) const noexcept {
  if (has_connect(from, to)) {
    return Link{from, to};
  } else {
    return std::nullopt;
  }
}

template <typename From, typename To>
CG_STRONG_INLINE void Graph::erase(Link<From, To> link) noexcept {
  auto &from = link.from_.node_;
  auto &to = link.to_.node_;
  auto &from_sock = from.outputs_[link.from().index()];
  auto &to_sock = to.inputs_[link.to().index()];

  from_sock.erase(to_sock);
  to_sock.clear();
  --link_size_;
}

CG_STRONG_INLINE bool Graph::has_cycle() const noexcept { return topology_order().empty(); }

inline void Graph::shrink_to_fit() noexcept {
  node_container new_nodes;
  new_nodes.reserve(num_nodes());
  for (auto &node : nodes_) {
    if (node) {
      new_nodes.push_back(std::move(node));
    }
  }
  nodes_ = std::move(new_nodes);
  free_ids_.clear();
  rebuild_addr_to_index();
}

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
  size_t empty = 0;
  for (size_t i = 0; i < n; ++i) {
    auto const &node = nodes_[i];
    if (!node) {
      result.push_back(i);
      ++empty;
      continue;
    }

    size_t count = 0;
    for (auto const &input : node->inputs()) {
      count += input.is_connected() ? 1 : 0;
    }
    connected_count[i] = count;
    if (count == 0) {
      result.push_back(i);
    }
  }

  for (size_t i = empty; i < result.size(); ++i) {
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
