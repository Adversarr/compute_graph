#include <iostream>

#include "compute_graph/include/compute_graph/node.hpp"

using namespace compute_graph;

class TestNode : public NodeBase {
public:
  CG_NODE_COMMON(TestNode, "TestNode", "TestNodeDescription");
  CG_NODE_INPUTS((int, x, "Describe x"), (float, y, "Describe y"));
  CG_NODE_OUTPUTS((int, z, "Describe z"));

  void run() {
    get(in::x);
    get(in::y);
    set(out::z, 1.0f);
  }
};

int main() {
  std::cout << "Hello, World!" << std::endl;

  NodeDescriptorBuilder<TestNode>("TestNode", "TestNodeDescription").build();

  return 0;
}
