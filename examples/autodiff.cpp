#include <cmath>
#include <iostream>
#include <vector>

namespace autodiff {

class Differentiable {
public:
  Differentiable() = default;
  virtual ~Differentiable() = default;
  virtual void backward() {}

  void setup_gradients(size_t total_count) { grads.resize(total_count, 0); }
  void accumulate_gradient(size_t i, float grad) { grads[i] += grad; }
  void clear_grad() {
    for (auto &grad : grads) {
      grad = 0;
    }
  }
  std::vector<float> grads;
};

}  // namespace autodiff

#define CG_NODE_SUPERBASE \
public                    \
  autodiff::Differentiable
#define CG_NODE_EXTENSION                                                   \
  void backprop(std::vector<float> const &grad) {                           \
    for (size_t i = 0; i < grad.size(); ++i) {                              \
      auto from_sock = inputs_[i].from();                                   \
      if (from_sock) {                                                      \
        from_sock->node().accumulate_gradient(from_sock->index(), grad[i]); \
      }                                                                     \
    }                                                                       \
  }

#include "compute_graph/compute_graph.hpp"

using namespace compute_graph;

#define DECLARE_UNARY_OP(name, op, diff_op)              \
  class name : public NodeDerive<name> {                 \
  public:                                                \
    CG_NODE_COMMON(name, #name, "Unary operation " #op); \
    CG_NODE_INPUTS((float, x, "The first operand"));     \
    CG_NODE_OUTPUTS((float, y, "The result of " #op));   \
    void on_construct() { setup_gradients(1); }          \
    void operator()(Context &) final {                   \
      clear_grad();                                      \
      set(out::y, op(*get(in::x)));                      \
    }                                                    \
    void backward() final {                              \
      float grad_x = diff_op(*get(out::y)) * grads[0];   \
      backprop({grad_x});                                \
    }                                                    \
  }

DECLARE_UNARY_OP(Sin, std::sin, std::cos);

#define DECLARE_BINARY_OP(name, op)                                                          \
  class name : public NodeDerive<name> {                                                     \
  public:                                                                                    \
    CG_NODE_COMMON(name, #name, "Binary operation " #op);                                    \
    CG_NODE_INPUTS((float, a, "The first operand", 0), (float, b, "The second operand", 0)); \
    CG_NODE_OUTPUTS((float, result, "The result of " #op));                                  \
    void operator()(Context &) final { set(out::result, get_or(in::a) op get_or(in::b)); }   \
  }

class Add : public NodeDerive<Add> {
public:
  CG_NODE_COMMON(Add, "Add", "Binary operation +");
  CG_NODE_INPUTS((float, a, "The first operand"), (float, b, "The second operand"));
  CG_NODE_OUTPUTS((float, result, "The result of +"));
  void on_construct() { setup_gradients(2); }
  void operator()(Context &) final {
    clear_grad();
    set(out::result, *get(in::a) + *get(in::b));
  }
  void backward() final { backprop({grads[0], grads[0]}); }
};

class Mul : public NodeDerive<Mul> {
public:
  CG_NODE_COMMON(Mul, "Mul", "Binary operation *");
  CG_NODE_INPUTS((float, a, "The first operand"), (float, b, "The second operand"));
  CG_NODE_OUTPUTS((float, result, "The result of *"));
  void on_construct() { setup_gradients(2); }
  void operator()(Context &) final {
    clear_grad();
    set(out::result, *get(in::a) * *get(in::b));
  }
  void backward() final {
    float grad_a = *get(in::b) * grads[0];
    float grad_b = *get(in::a) * grads[0];
    backprop({grad_a, grad_b});
  }
};

class ConstantFloat : public NodeDerive<ConstantFloat> {
public:
  CG_NODE_COMMON(ConstantFloat, "ConstantFloat", "Generate a constant float");
  CG_NODE_INPUTS();

  CG_NODE_OUTPUTS((float, value, "A constant float."));

  void on_construct() { setup_gradients(1); }

  void operator()(Context &) final { set(out::value, value_); }

  float value_ = 4;
};

template <typename T> auto create() { return NodeRegistry::instance().create<T>(); }
int main() {
  Graph g;
  auto sin = g.add(create<Sin>());
  auto add_ab = g.add(create<Add>());
  auto mul_ab_c = g.add(create<Mul>());
  auto a = g.add(create<ConstantFloat>()),
       b = g.add(create<ConstantFloat>()),
       c = g.add(create<ConstantFloat>());

  // Sin [(a + b) * c]
  a->value_ = 1;
  b->value_ = 2;
  c->value_ = 3.14159265358979323846 / 4;

  g.connect(a.output(0), add_ab.input(0));
  g.connect(b.output(ConstantFloat::out::value), add_ab.input(Add::in::b));
  g.connect(add_ab.output(Add::out::result), mul_ab_c.input("a").value());
  g.connect(c.output("value").value(), mul_ab_c.input(1));
  g.connect(mul_ab_c.output("result").value(), sin.input("x").value());
  g.topology_sort();

  Context ctx;
  for (auto const &node : g.nodes()) {
    (*node)(ctx);
  }
  sin->grads[0] = 1;
  for (auto it = g.nodes().rbegin(); it != g.nodes().rend(); ++it) {
    (*it)->backward();
  }

  // partial A = Cosine[(a + b) * c] * c = -0.5553
  // partial B = Cosine[(a + b) * c] * c = -0.5553
  // partial C = Cosine[(a + b) * c] * (a + b) = -2.1213

  std::cout << "p/pa: " << a->grads[0] << std::endl;
  std::cout << "p/pb: " << b->grads[0] << std::endl;
  std::cout << "p/pc: " << c->grads[0] << std::endl;

  return EXIT_SUCCESS;
}