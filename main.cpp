#include <iomanip>
#include <iostream>
#include "compute_graph/graph.hpp"

using namespace compute_graph;

class ConstIntegerNode : public NodeDerive<ConstIntegerNode> {
public:
  CG_NODE_COMMON(ConstIntegerNode, "ConstInteger", "Produce a constant integer value");
  CG_NODE_INPUTS();
  CG_NODE_OUTPUTS((int, value, "The constant integer value"));

  void operator()(Graph &) final {
    set(out::value, 5);
  }
};

class WhateverNode : public NodeDerive<WhateverNode> {
public:
  CG_NODE_COMMON(WhateverNode, "Whatever", "WhateverDescription");
  CG_NODE_INPUTS((int, x, "Describe x", 0  /* default value = 0 */),
                 (int, y, "Describe y", 42 /* default value = 42 */));
  CG_NODE_OUTPUTS((std::string, z, "z equals x + y"));

  static void on_register() /* optional */ {
    printf("Do whatever you want!\n");
  }

  void operator()(Graph &) final {
    auto x = get_or(in::x);
    auto y = get_or(in::y);
    set(out::z, "x=" + std::to_string(x) + ", y=" + std::to_string(y));
  }
};

class EchoString : public NodeDerive<EchoString> {
public:
  CG_NODE_COMMON(EchoString, "EchoString", "EchoStringDescription");
  CG_NODE_INPUTS((std::string, str, "Input string"));
  CG_NODE_OUTPUTS();

  void operator()(Graph &) final {
    auto str = *get(in::str);
    std::cout << "str: " << std::quoted(str) << std::endl;
  }
};

int main() {
  Graph g;
  auto nh1 = g.add(create_node<WhateverNode>()),
       nh2 = g.add(create_node<EchoString>()),
       nh3 = g.add(create_node<ConstIntegerNode>());

  g.connect(nh1.output(WhateverNode::out::z), nh2.input(EchoString::in::str));
  g.connect(nh3.output(ConstIntegerNode::out::value), nh1.input(WhateverNode::in::x));
  g.topology_sort();

  for (auto const& node: g.nodes()) {
    (*node)(g);
  }

  return 0;
}
