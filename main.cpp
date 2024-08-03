#include <iostream>
#include "compute_graph/include/compute_graph/node.hpp"

using namespace compute_graph;

class TestNode : public NodeDerive<TestNode> {
public:
  CG_NODE_COMMON(TestNode, "TestNode", "TestNodeDescription");
  CG_NODE_INPUTS((int, x, "Describe x"), (float, y, "Describe y"));
  CG_NODE_OUTPUTS((std::string, z, "Describe z"));

  void operator()(Graph& ctx) final {
    get(in::x);
    get(in::y);
    set(out::z, "Hello, World!");
  }
};

int main() {
  std::cout << "Hello, World!" << std::endl;
  TestNode::register_node();

  auto const& reg = node_descriptors();
  for (auto const& [key, value] : reg) {
    std::cout << key.name() << std::endl;
    std::cout << value.name() << std::endl;
    std::cout << value.desc() << std::endl;

    for (auto const& input : value.inputs()) {
      std::cout << input.name() << std::endl;
      std::cout << input.desc() << std::endl;
    }

    for (auto const& output : value.outputs()) {
      std::cout << output.name() << std::endl;
      std::cout << output.desc() << std::endl;
    }
  }

  NodeDescriptorBuilder<TestNode>("TestNode", "TestNodeDescription").build();
  return 0;
}
