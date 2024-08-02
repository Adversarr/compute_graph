#pragma once
#include <vector>
#include <functional>

#include "config.hpp"
#include "socket_descriptor.hpp"
#include "type_erased_factory.hpp"

CG_NAMESPACE_BEGIN

template <typename T> class NodeDescriptorBuilder;

using node_factory =
    std::function<std::unique_ptr<NodeBase>(NodeDescriptor const &)>;

class NodeDescriptor {
public:
  std::vector<SocketDescriptor> const &inputs() const noexcept;

  std::vector<SocketDescriptor> const &outputs() const noexcept;

  std::string const &name() const noexcept;

  std::string const &desc() const noexcept;

  std::type_index type() const noexcept;

  NodeDescriptor(NodeDescriptor const &) = default;

  NodeDescriptor(NodeDescriptor &&) = default;

  std::unique_ptr<NodeBase> build() { return factory_(*this); }

private:
  NodeDescriptor(std::string name, std::string desc, std::type_index type,
                 node_factory factory) noexcept
      : name_(std::move(name)), desc_(std::move(desc)), type_(type),
        factory_(std::move(factory)) {}

  friend class NodeDescriptorBuilder<NodeBase>;

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
                    factory_([](NodeDescriptor const &descriptor) {
                      return std::make_unique<NodeType>(descriptor);
                    })) {}

  NodeDescriptorBuilder &input(SocketDescriptor const &desc) noexcept {
    descriptor_.inputs_.push_back(desc);
  }

  NodeDescriptorBuilder &output(SocketDescriptor const &desc) noexcept {
    descriptor_.outputs_.push_back(desc);
  }

  NodeDescriptor const &build() noexcept { return register_node(descriptor_); }

private:
  NodeDescriptor descriptor_;
};

CG_NAMESPACE_END
