#pragma once
#include "compute_graph/socket.hpp"
#include "intern/config.hpp"
#include "node.hpp"
#include <any>
#include <functional>
#include <memory>
#include <typeindex>
#include <vector>

CG_NAMESPACE_BEGIN

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

bool can_connect(OutputSocket const &from, InputSocket const &to);

class Link {
public:
  OutputSocketHandle from() const noexcept {
    return OutputSocketHandle(ctx_, from_, from_index_);
  }
  InputSocketHandle to() const noexcept {
    return InputSocketHandle(ctx_, to_, to_index_);
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
  using typed_map = std::unordered_map<std::type_index, T>;

  Graph() = default;
  void clear();
  size_t num_nodes() const noexcept { return nodes_.size(); }
  size_t num_links() const noexcept { return link_size_; }

  inline node_container const &nodes() const noexcept { return nodes_; }
  inline node_container &nodes() noexcept { return nodes_; }

  // node op.
  NodeHandle add(std::unique_ptr<NodeBase> node);
  bool erase(NodeHandle const &handle);
  inline void clear_nodes() noexcept;

  // socket op
  Link connect(OutputSocketHandle const &from, InputSocketHandle const &to);
  Link get(OutputSocketHandle const &from, InputSocketHandle const &to);
  void erase(Link const &link);
  void clear_links() noexcept;

  // context op
  ctx_type &ctx() noexcept { return ctx_; }

private:
  node_container nodes_;
  ctx_type ctx_;
  id_container free_ids_;
  size_t uid_next_ = 0;
  size_t link_size_ = 0;
  // TODO: connection callback.
};

CG_NAMESPACE_END