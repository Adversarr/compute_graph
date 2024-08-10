#pragma once
#include "compute_graph/node.hpp"
#include "compute_graph/context.hpp"

namespace compute_graph {

namespace intern {
NodeDescriptor build_subgraph_node_descriptor();
}

template<typename T>
class ReadContext final : public NodeDerive<ReadContext<T>>{
public:
  CG_NODE_COMMON(ReadContext, "ReadContext", "Extract a value in context");
  CG_NODE_INPUTS((std::string, key, "Variable name"),
                 (bool, top_only, "Only find the variable on the top frame.", true));
  CG_NODE_OUTPUTS((T, value, "Value of the extracted variable"));

  void operator()(Context& ctx) override {
    const bool top_only = get_or(in::top_only);
    const std::string &key = ensure(in::key);

    if (top_only) {
      auto const& value = ctx.get_top(key);
      set(out::value, std::any_cast<T const&>(value));
    } else {
      auto const& value = ctx.get(key);
      set(out::value, std::any_cast<T const&>(value));
    }
  }
};

template <typename T>
class WriteContext final : public NodeDerive<WriteContext<T>> {
public:
  CG_NODE_COMMON(WriteContext, "WriteContext", "Write a value to current context frame.");
  CG_NODE_INPUTS((std::string, key, "Variable name"),
                 (T, value, "Value of variable to write"));
  CG_NODE_OUTPUTS();

  void operator()(Context& ctx) override {
    auto const& key = ensure(in::key);
    auto const& value = ensure(in::value);
    ctx.insert(&key, &value);
  }
};

class SubgraphNode final : public NodeDerive<SubgraphNode> {
public:
  CG_NODE_COMMON(SubgraphNode, "SubgraphNode", "Run a subgraph in a graph.");
  CG_NODE_INPUTS();
  CG_NODE_OUTPUTS();

  // Push the new frame, and run the subgraph.
  void operator()(Context& ctx) override;

  std::shared_ptr<Graph> graph_;
};

CG_STRONG_INLINE void SubgraphNode::operator()(Context& ctx) {
  if (! graph_) {
    throw std::runtime_error("graph is not set.");
  }

  ctx.push_stack();
  graph_->topology_sort();
  for (auto const& node : graph_->nodes()) {
    (*node)(ctx);
  }
  ctx.pop_stack();
}

}  // namespace compute_graph