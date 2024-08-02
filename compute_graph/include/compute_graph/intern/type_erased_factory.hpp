#pragma once

#include "config.hpp"

#include "node_descriptor.hpp"
#include <typeindex>
#include <unordered_map>

CG_NAMESPACE_BEGIN

inline std::unordered_map<std::type_index, NodeDescriptor> &node_descriptors() {
  static std::unordered_map<std::type_index, NodeDescriptor> descriptors;
  return descriptors;
}

inline NodeDescriptor const &register_node(NodeDescriptor const &descriptor) {
  return node_descriptors()
      .emplace(descriptor.type(), descriptor)
      .first->second;
}

CG_NAMESPACE_END
