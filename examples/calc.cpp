#include <compute_graph/compute_graph.hpp>
#include <cassert>
#include <iomanip>
#include <iostream>

using namespace compute_graph;

class ConstantInteger : public NodeDerive<ConstantInteger> {
public:
  CG_NODE_COMMON(ConstantInteger, "ConstantInteger", "Generate a constant integer");
  CG_NODE_INPUTS();

  CG_NODE_OUTPUTS((int, value, "A constant integer."));

  void on_construct() {
    set(out::value, value_);
  }

  void operator() (Context& ) final { on_construct(); }

  int value_ = 4;
};

#define DECLARE_BINARY_OP(name, op) \
  class name : public NodeDerive<name> { \
    public:                                          \
      CG_NODE_COMMON(name, #name, "Binary operation " #op);\
      CG_NODE_INPUTS((int, a, "The first operand", 0), (int, b, "The second operand", 0)); \
      CG_NODE_OUTPUTS((int, result, "The result of " #op)); \
      void operator() (Context& ) final { set(out::result, get_or(in::a) op get_or(in::b)); } \
  }

DECLARE_BINARY_OP(Add, +);
DECLARE_BINARY_OP(Sub, -);
DECLARE_BINARY_OP(Mul, *);
DECLARE_BINARY_OP(Div, /);

template <typename T> auto create_node() { return NodeRegistry::instance().create<T>(); }

int main() {
  Graph graph;
  auto add_ab = graph.add(create_node<Add>());
  auto mul_abc = graph.add(create_node<Mul>());

  auto a = graph.add(create_node<ConstantInteger>()),
       b = graph.add(create_node<ConstantInteger>()),
       c = graph.add(create_node<ConstantInteger>());
  a->value_ = 1;
  b->value_ = 2;

  graph.connect(a.output(0), add_ab.input(0));
  graph.connect(b.output(ConstantInteger::out::value), add_ab.input(Add::in::b));
  graph.connect(add_ab.output(Add::out::result), mul_abc.input(Mul::in::a));
  graph.connect(c.output(ConstantInteger::out::value), mul_abc.input(Mul::in::b));

  assert(graph.has_connect(*c.output("value"), mul_abc.input(1)));

  graph.topology_sort();

  for (auto const& [nt_ident, node_desc]: NodeRegistry::instance()) {
    std::cout << "Node: " << node_desc.name() << " (type=" << to_string(nt_ident)
              << ", hash=" << std::hash<utype_ident>{}(nt_ident) << ")" << std::endl;
    std::cout << "  Description: " << node_desc.desc() << std::endl;
    std::cout << "  Inputs:" << std::endl;
    for (auto const& sock : node_desc.inputs()) {
      std::cout << "    " << sock.name() << "(" << sock.pretty_typename() << "): " << sock.desc()
                << std::endl;
    }
    std::cout << "  Outputs:" << std::endl;
    for (auto const& sock : node_desc.outputs()) {
      std::cout << "    " << sock.name() << "(" << sock.pretty_typename() << "): " << sock.desc()
                << std::endl;
    }
  }

  Context rt;
  for (auto & node : graph.nodes()) {
    (*node)(rt);
  }

  void const* result = mul_abc.output(Mul::out::result)->payload().get();
  std::cout << "Result: " << *static_cast<int const*>(result) << std::endl; // should print 12
  return EXIT_SUCCESS;
}
