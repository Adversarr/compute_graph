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
#include "config.hpp"
#include "socket_descriptor.hpp"
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace compute_graph {
using NodeRegistry = std::unordered_map<utype_ident, NodeDescriptor>;
using NodeFactory = std::function<std::unique_ptr<NodeBase>(NodeDescriptor const *)>;

NodeRegistry &node_descriptors();
NodeDescriptor const &register_node(NodeDescriptor const &descriptor);

class NodeDescriptor {
public:
  std::vector<SocketDescriptor> const &inputs() const noexcept {
    return inputs_;
  }

  std::vector<SocketDescriptor> const &outputs() const noexcept {
    return outputs_;
  }

  std::string const &name() const noexcept { return name_; }

  std::string const &desc() const noexcept { return desc_; }

  utype_ident const& type() const noexcept { return type_; }

  NodeDescriptor(NodeDescriptor const &) = default;

  NodeDescriptor(NodeDescriptor &&) = default;

  std::unique_ptr<NodeBase> build() const { return factory_(this); }

  template<typename NodeType> friend class NodeDescriptorBuilder;

private:
  NodeDescriptor(std::string name, std::string desc, utype_ident type,
                 NodeFactory factory) noexcept
    : name_(std::move(name)), desc_(std::move(desc)), type_(std::move(type)),
      factory_(std::move(factory)) {
  }

  const std::string name_;
  const std::string desc_;
  const utype_ident type_;
  NodeFactory factory_;
  std::vector<SocketDescriptor> inputs_;
  std::vector<SocketDescriptor> outputs_;
};

template<typename NodeType>
class NodeDescriptorBuilder {
  static_assert(std::is_base_of_v<NodeBase, NodeType>,
                "NodeType must be derived from NodeBase");

public:
  NodeDescriptorBuilder(std::string name, std::string desc) noexcept
    : descriptor_(std::move(name), std::move(desc), typeid(NodeType),
                  [](NodeDescriptor const *descriptor) {
                    return std::make_unique<NodeType>(descriptor);
                  }) {}

  NodeDescriptorBuilder &input(SocketDescriptor const &desc) noexcept {
    descriptor_.inputs_.push_back(desc);
    return *this;
  }

  NodeDescriptorBuilder &output(SocketDescriptor const &desc) noexcept {
    descriptor_.outputs_.push_back(desc);
    return *this;
  }

  NodeDescriptor const &build() const noexcept { return register_node(descriptor_); }

private:
  NodeDescriptor descriptor_;
};

inline NodeRegistry &node_descriptors() {
  static NodeRegistry descriptors;
  return descriptors;
}

inline NodeDescriptor const &register_node(NodeDescriptor const &descriptor) {
  return node_descriptors()
      .emplace(descriptor.type(), descriptor)
      .first->second;
}

inline std::unique_ptr<NodeBase> create_node(utype_ident node_type) {
  const auto &descriptors = node_descriptors();
  auto const it = descriptors.find(node_type);
  if (it == descriptors.end()) {
    throw std::invalid_argument("Invalid node type: " + to_string(node_type));
  }
  return it->second.build();
}

template <typename T> std::unique_ptr<NodeBase> create_node() {
  return create_node(typeid(T));
}

} // namespace compute_graph
