#pragma once
#include "config.hpp"
#include "socket_descriptor.hpp"
#include <functional>
#include <typeindex>
#include <vector>

CG_NAMESPACE_BEGIN

template <typename T> class NodeDescriptorBuilder;

using node_factory =
    std::function<std::unique_ptr<NodeBase>(NodeDescriptor const *)>;

std::unordered_map<std::type_index, NodeDescriptor> &node_descriptors();

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

  std::type_index type() const noexcept { return type_; }

  NodeDescriptor(NodeDescriptor const &) = default;

  NodeDescriptor(NodeDescriptor &&) = default;

  std::unique_ptr<NodeBase> build() const { return factory_(this); }

  template <typename NodeType> friend class NodeDescriptorBuilder;
private:
  NodeDescriptor(std::string name, std::string desc, std::type_index type,
                 node_factory factory) noexcept
      : name_(std::move(name)), desc_(std::move(desc)), type_(type),
        factory_(std::move(factory)) {}

  const std::string name_;
  const std::string desc_;
  const std::type_index type_;
  node_factory factory_;
  std::vector<SocketDescriptor> inputs_;
  std::vector<SocketDescriptor> outputs_;
};

template <typename NodeType> class NodeDescriptorBuilder {
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

  NodeDescriptor const &build() noexcept { return register_node(descriptor_); }

private:
  NodeDescriptor descriptor_;
};

inline std::unordered_map<std::type_index, NodeDescriptor> &node_descriptors() {
  static std::unordered_map<std::type_index, NodeDescriptor> descriptors;
  return descriptors;
}

inline NodeDescriptor const &register_node(NodeDescriptor const &descriptor) {
  return node_descriptors()
      .emplace(descriptor.type(), descriptor)
      .first->second;
}

inline std::unique_ptr<NodeBase> create_node(std::type_index node_type) {
  const auto& descriptors = node_descriptors();
  auto it = descriptors.find(node_type);
  if (it == descriptors.end()) {
    return nullptr;
  }
  return it->second.build();
}

CG_NAMESPACE_END
