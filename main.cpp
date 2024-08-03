#include <iostream>
#include "compute_graph/node.hpp"
#include "compute_graph/graph.hpp"
#include "compute_graph/algo.hpp"

using namespace compute_graph;

class TestNode : public NodeDerive<TestNode> {
public:
  CG_NODE_COMMON(TestNode, "TestNode", "TestNodeDescription");
  CG_NODE_INPUTS((int, x, "Describe x", 1),
                 (float, y, "Describe y"));
  CG_NODE_OUTPUTS((std::string, z, "Describe z"));

  static void on_register() {
    std::cout << "Registering Test Node." << std::endl;
  }

  void operator()(Graph& ctx) final {
    get_or(in::x);
    get(in::y);
    set(out::z, "Hello, World!");
    default_value(in::x);
    // get_or(in::y);
  }
};

int main() {
  std::cout << "Hello, World!" << std::endl;

  Graph g;
  auto nh = g.add(create_node<TestNode>());
  std::cout << g.num_nodes() << std::endl;
  return 0;
}
