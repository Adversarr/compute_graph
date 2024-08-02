#pragma once

#include "config.hpp"

#include <typeindex>
#include <compute_graph/node_descriptor.hpp>

#include "./type_erased_ptr.hpp"


CG_NAMESPACE_BEGIN

using node_factory = std::function<std::unique_ptr<NodeBase>(NodeDescriptor const &)>;

inline std::unordered_map<std::type_index, NodeDescriptor> & node_descriptors() {
    static std::unordered_map<std::type_index, NodeDescriptor> descriptors;
    return descriptors;
}

inline NodeDescriptor const& register_node(NodeDescriptor const& descriptor) {
    return node_descriptors().emplace(descriptor.type(), descriptor).first->second;
}

CG_NAMESPACE_END