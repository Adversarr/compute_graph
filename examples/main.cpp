#include <compute_graph/compute_graph.hpp>
#include <compute_graph/extra/ctx_nodes.hpp>
#include <iomanip>
#include <iostream>

using namespace compute_graph;

class ConstIntegerNode : public NodeDerive<ConstIntegerNode> {
public:
  CG_NODE_COMMON(ConstIntegerNode, "ConstInteger", "Produce a constant integer value");
  CG_NODE_INPUTS();
  CG_NODE_OUTPUTS((int, value, "The constant integer value"));

  void on_construct() /* optional */ { set(out::value, 5); }

  void operator()(Context &) final { set_all(5); }
};

class WhateverNode : public NodeDerive<WhateverNode> {
public:
  CG_NODE_COMMON(WhateverNode, "Whatever", "WhateverDescription");
  CG_NODE_INPUTS((int, x, "Describe x", 0 /* default value = 0  */),
                 (int, y, "Describe y", 42 /* default value = 42 */));
  CG_NODE_OUTPUTS((std::string, z, "z equals x + y"));

  static void on_register() /* optional */ { printf("Do whatever you want!\n"); }

  void on_construct() /* optional */ { std::cout << "Constructing Whatever..." << std::endl; }

  void operator()(Context &) final {
    auto x = get_or(in::x);
    auto y = *get_or<int>(1);
    set(out::z, "x=" + std::to_string(x) + ", y=" + std::to_string(y));
  }
};

class EchoString : public NodeDerive<EchoString> {
public:
  CG_NODE_COMMON(EchoString, "EchoString", "EchoStringDescription");
  CG_NODE_INPUTS((std::string, str, "Input string"));
  CG_NODE_OUTPUTS();

  void on_connect_dispatch(in::str_) /* automatically called. */ {
    std::cout << "EchoString::on_connect" << std::endl;
    std::cout << " has input set? " << std::boolalpha << has(in::str) << std::endl;
  }

  void operator()(Context &) final {
    auto str = *get<std::string>(0);
    std::cout << "str: " << std::quoted(str) << std::endl;
  }
};

class EchoInteger : public NodeDerive<EchoInteger> {
public:
  CG_NODE_COMMON(EchoInteger, "EchoInteger", "EchoIntegerDescription");
  CG_NODE_INPUTS((int, x, "Input integer"));
  CG_NODE_OUTPUTS();

  void operator()(Context &) final {
    auto [x] = get_all();
    std::cout << "x: " << *x << std::endl;
  }
};

class ConstantString : public NodeDerive<ConstantString> {
public:
  CG_NODE_COMMON(ConstantString, "ConstantString", "ConstantString");
  CG_NODE_INPUTS();
  CG_NODE_OUTPUTS((std::string, value, "Return 'what'"));

  void operator()(Context &) final {
    set(out::value, "what");
  }
};
namespace compute_graph {

template class ReadContext<std::string>;

}

int main() {
  Graph g;
  NodeHandle<NodeBase>
       nh1 = g.add(NodeRegistry::instance().create<WhateverNode>()),
       nh2 = g.add(NodeRegistry::instance().create<EchoString>()),
       nh3 = g.add(NodeRegistry::instance().create<ConstIntegerNode>()),
       nh4 = g.add(NodeRegistry::instance().create<EchoInteger>()),
       nh5 = g.add(NodeRegistry::instance().create<ConstantString>()),
       nh6 = g.add(NodeRegistry::instance().create<ReadContext<std::string>>()),
       nh7 = g.add(NodeRegistry::instance().create<EchoString>());
  g.connect(nh1.output(WhateverNode::out::z), nh2.input(EchoString::in::str));
  g.connect(nh3.output(ConstIntegerNode::out::value), nh1.input("x").value());
  g.connect(nh3.output(ConstIntegerNode::out::value), nh4.input(0));

  g.connect(nh5.output(ConstantString::out::value), nh6.input("key").value());
  g.connect(nh6.output(0), nh7.input(0));

  g.topology_sort();

  Context rt;
  rt.insert("what", std::string("is?"));

  for (auto const &node : g.nodes()) {
    (*node)(rt);
  }

  return 0;
}
