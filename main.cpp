#include <iostream>

#include "compute_graph/include/compute_graph/node.hpp"
#include "compute_graph/include/compute_graph/socket_descriptor.hpp"
#include "compute_graph/include/compute_graph/node_descriptor.hpp"


using namespace compute_graph;

class TestNode : public NodeBase {};

int main() {
    std::cout << "Hello, World!" << std::endl;

    NodeDescriptorBuilder<TestNode>("TestNode", "TestNodeDescription")
        .input("input", SocketDescriptor::make<int>())
        .output("output", SocketDescriptor::make<int>())
        .build();

    return 0;
}
