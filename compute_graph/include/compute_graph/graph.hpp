#pragma once
#include "compute_graph/socket.hpp"
#include "intern/config.hpp"
#include "node.hpp"
#include <any>
#include <functional>
#include <memory>
#include <vector>

#include "graph.hpp"

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
  InputSocketHandle(Graph &ctx, NodeBase &node, size_t index);

  friend class Graph;
  friend class NodeHandle;
  friend class Link;

  Graph &ctx_;
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
  OutputSocketHandle(Graph &ctx, NodeBase &node, size_t index);

  friend class Graph;
  friend class NodeHandle;
  friend class Link;
  Graph &ctx_;
  NodeBase &node_;
  size_t const index_;
};

class Link {
public:
  OutputSocketHandle from() const noexcept {
    return {ctx_, from_, from_index_};
  }
  InputSocketHandle to() const noexcept {
    return {ctx_, to_, to_index_};
  }

private:
  Link(Graph &ctx, NodeBase &from, size_t from_index, NodeBase &to,
       size_t to_index);

  friend class Graph;
  Graph &ctx_;
  NodeBase &from_, &to_;
  size_t const from_index_, to_index_;
};

class Graph {
public:
  using node_ptr = std::unique_ptr<NodeBase>;
  using node_container = std::vector<node_ptr>;
  using ctx_type = std::unordered_map<std::string, std::any>;
  using id_container = std::vector<size_t>;

  template <typename T>
  using typed_map = std::unordered_map<utype_ident, T>;

  Graph() = default;
  void clear();
  size_t num_nodes() const noexcept { return nodes_.size(); }
  size_t num_links() const noexcept { return link_size_; }

  node_container const &nodes() const noexcept { return nodes_; }
  node_container &nodes() noexcept { return nodes_; }

  // node op.
  NodeHandle add(std::unique_ptr<NodeBase> node);
  void erase(NodeHandle handle);

  // socket op
  Link connect(OutputSocketHandle from, InputSocketHandle to);
  Link get(OutputSocketHandle from, InputSocketHandle to);
  void erase(Link link);

  // context op
  ctx_type &ctx() noexcept { return ctx_; }

  void topology_sort();
  bool has_cycle() const;

private:
  size_t fetch_free_id() noexcept;
  void release_id(size_t id) noexcept;
  node_container nodes_;
  ctx_type ctx_;
  id_container free_ids_;
  size_t uid_next_ = 0;
  size_t link_size_ = 0;
};


inline bool can_connect(OutputSocket const &from, InputSocket const &to) noexcept {
  return from.type() == to.type();
}

inline NodeHandle Graph::add(std::unique_ptr<NodeBase> node) {
  if (free_ids_.empty()) {
    nodes_.push_back(std::move(node));
    return {uid_next_++, *nodes_.back()};
  } else {
    size_t const index = free_ids_.back();
    free_ids_.pop_back();
    nodes_[index] = std::move(node);
    return {index, *nodes_[index]};
  }
}

inline void Graph::erase(NodeHandle handle) {
  size_t const index = handle.index();
  for (size_t i = 0; i < handle->inputs().size(); ++i) {
    auto const &input_sock = handle->inputs()[i];
    if (input_sock.is_connected()) {
      auto const& output_sock = *input_sock.from();
      erase(Link{*this, output_sock.node(), output_sock.index(), handle.node(), i});
    }
  }

  for (size_t i = 0; i < handle->outputs().size(); ++i) {
    auto const &output_sock = handle->outputs()[i];
    for (auto const &input_sock : output_sock.connected_sockets()) {
      erase(Link{*this, handle.node(), i, input_sock->node(), input_sock->index()});
    }
  }

  // TODO: callback.
  nodes_[index].reset();
  free_ids_.push_back(index);
}

inline Link Graph::connect(OutputSocketHandle from, InputSocketHandle to) {
  if (!can_connect(*from, *to)) {
    throw std::invalid_argument("Cannot connect sockets of different types.");
  }

  // If already connected, erase the old link.
  if (to->is_connected()) {
    OutputSocket const* previous_from = to->from();
    erase(Link{*this, previous_from->node(), previous_from->index(), to->node(), to->index()});
  }

  // add link in between.
  auto &from_node = from->node(), &to_node = to->node();
  from_node.outputs_[from.index()].connect(*to);
  to_node.inputs_[to.index()].connect(from.operator->());
  ++link_size_;

  // TODO: callback.
  return Link{*this, from_node, from.index(), to_node, to.index()};
}

inline Link Graph::get(OutputSocketHandle from, InputSocketHandle to) {
  if (to->from() != from.operator->()) {
    throw std::invalid_argument("Cannot get link between unconnected sockets.");
  }
  return Link{*this, from.node(), from.index(), to.node(), to.index()};
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

}
