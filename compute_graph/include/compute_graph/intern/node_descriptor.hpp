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
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "config.hpp"
#include "socket_descriptor.hpp"

namespace compute_graph {

using NodeFactory = std::function<std::unique_ptr<NodeBase>(NodeDescriptor const&)>;

class NodeDescriptor {
public:
  CG_STRONG_INLINE std::vector<SocketDescriptor> const &inputs() const noexcept { return inputs_; }

  CG_STRONG_INLINE std::vector<SocketDescriptor> const &outputs() const noexcept {
    return outputs_;
  }

  CG_STRONG_INLINE std::string const &name() const noexcept { return name_; }

  CG_STRONG_INLINE std::string const &desc() const noexcept { return desc_; }

  CG_STRONG_INLINE utype_ident const &type() const noexcept { return type_; }

  CG_STRONG_INLINE std::optional<size_t> find_input(std::string const &name) const noexcept {
    for (size_t i = 0; i < inputs_.size(); ++i) {
      if (inputs_[i].name() == name) {
        return i;
      }
    }
    return std::nullopt;
  }

  CG_STRONG_INLINE std::optional<size_t> find_output(std::string const &name) const noexcept {
    for (size_t i = 0; i < outputs_.size(); ++i) {
      if (outputs_[i].name() == name) {
        return i;
      }
    }
    return std::nullopt;
  }

  CG_STRONG_INLINE NodeDescriptor(NodeDescriptor const &) = default;
  CG_STRONG_INLINE NodeDescriptor(NodeDescriptor &&) = default;

  CG_STRONG_INLINE std::unique_ptr<NodeBase> build() const { return factory_(*this); }
  template <typename NodeType> friend class NodeDescriptorBuilder;

private:
  CG_STRONG_INLINE NodeDescriptor(std::string name, std::string desc, utype_ident type,
                                  NodeFactory factory) noexcept
      : name_(std::move(name)),
        desc_(std::move(desc)),
        type_(std::move(type)),
        factory_(std::move(factory)),
        inputs_{},
        outputs_{} {}

  const std::string name_;
  const std::string desc_;
  const utype_ident type_;
  NodeFactory factory_;
  std::vector<SocketDescriptor> inputs_;
  std::vector<SocketDescriptor> outputs_;
};

template <typename NodeType> class NodeDescriptorBuilder {
  static_assert(std::is_base_of_v<NodeBase, NodeType>, "NodeType must be derived from NodeBase");

public:
  CG_STRONG_INLINE NodeDescriptorBuilder(std::string name, std::string desc) noexcept
      : descriptor_(std::move(name), std::move(desc), typeid(NodeType),
                    [](NodeDescriptor const &descriptor) {
                      return std::make_unique<NodeType>(descriptor);
                    }) {}

  CG_STRONG_INLINE NodeDescriptorBuilder &input(SocketDescriptor const &desc) noexcept {
    descriptor_.inputs_.push_back(desc);
    return *this;
  }

  CG_STRONG_INLINE NodeDescriptorBuilder &output(SocketDescriptor const &desc) noexcept {
    descriptor_.outputs_.push_back(desc);
    return *this;
  }

  CG_STRONG_INLINE NodeDescriptor build() const noexcept { return descriptor_; }

private:
  NodeDescriptor descriptor_;
};

class NodeRegistry {
public:
  using container_type = std::unordered_map<utype_ident, NodeDescriptor>;

  CG_STRONG_INLINE auto find(utype_ident type) const noexcept { return descriptors_.find(type); }
  CG_STRONG_INLINE auto begin() const noexcept { return descriptors_.begin(); }
  CG_STRONG_INLINE auto end() const noexcept { return descriptors_.end(); }

  CG_STRONG_INLINE NodeDescriptor const &emplace(NodeDescriptor const& descriptor) {
    return descriptors_.emplace(descriptor.type(), descriptor).first->second;
  }

  std::unique_ptr<NodeBase> create(utype_ident type) const CG_NOEXCEPT {
    auto const it = descriptors_.find(type);
#ifndef CG_NO_CHECK
    if (it == descriptors_.end()) {
      CG_THROW(std::invalid_argument, "Invalid node type: " + to_string(type));
    }
#endif
    return it->second.build();
  }

  template <typename T> std::unique_ptr<NodeBase> create() const CG_NOEXCEPT {
    return create(typeid(T));
  }

  CG_STRONG_INLINE bool has(utype_ident type) const noexcept {
    return descriptors_.find(type) != descriptors_.end();
  }

  CG_STRONG_INLINE void clear() noexcept { descriptors_.clear(); }

  static NodeRegistry &instance() {
    static NodeRegistry registry;
    return registry;
  }

  CG_STRONG_INLINE NodeRegistry(): descriptors_{} {}
  NodeRegistry(NodeRegistry const &) = delete;
  NodeRegistry(NodeRegistry &&) = default;
  NodeRegistry &operator=(NodeRegistry const &) = delete;
  NodeRegistry &operator=(NodeRegistry &&) = default;

private:
  container_type descriptors_;
};

}  // namespace compute_graph
