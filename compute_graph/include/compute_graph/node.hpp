#pragma once
#include <vector>
#include <algorithm>

#include "intern/node.hpp"
#include "intern/node_descriptor.hpp"
#include "intern/pp.hpp"
#include "socket.hpp"

namespace compute_graph {

class NodeBase {
public:
  explicit NodeBase(NodeDescriptor const *descriptor) noexcept
      : descriptor_(descriptor) {
    inputs_.reserve(descriptor->inputs().size());
    outputs_.reserve(descriptor->outputs().size());

    for (size_t i = 0; i < descriptor->inputs().size(); ++i) {
      inputs_.push_back({descriptor->inputs()[i].type(), *this, i});
    }

    for (size_t i = 0; i < descriptor->outputs().size(); ++i) {
      outputs_.push_back({descriptor->outputs()[i].type(), *this, i});
    }
  }

  virtual ~NodeBase() noexcept = default;

  // execute, may throw exception.
  virtual void operator()(Graph &) = 0;

  // After connect to some pipe, must be no except.
  virtual void on_connect(size_t index) noexcept {}
  // Before disconnect from some pipe, must be noexcept.
  virtual void on_disconnect(size_t index) noexcept {}

  NodeDescriptor const *descriptor() const noexcept { return descriptor_; }

  auto const &inputs() const noexcept { return inputs_; }
  auto const &outputs() const noexcept { return outputs_; }

protected:

  TypeErasedPtr const &get_input(size_t index) const {
    return (inputs_[index].fetch()).value();
  }

  bool has_input(size_t index) const noexcept {
    return inputs_[index].fetch().has_value();
  }

  template <typename T, typename... Args>
  auto &set_output(size_t index, Args &&...args) {
    return outputs_[index].emplace<T>(std::forward<Args>(args)...);
  }

private:

  friend class Graph;
  std::vector<InputSocket> inputs_;
  std::vector<OutputSocket> outputs_;
  NodeDescriptor const *descriptor_;
};

// crtp.
template <typename Derived> class NodeDerive : public NodeBase {
public:
  explicit NodeDerive(NodeDescriptor const *descriptor) noexcept:
    NodeBase(descriptor) {}

  struct intern_node_register {
    static constexpr const char *name = Derived::name;
    static constexpr const char *description = Derived::description;
    using input_metas = typename Derived::intern_input_meta;
    using output_metas = typename Derived::intern_output_meta;
    template <size_t i> struct input_reg_fn {
      static constexpr void eval(NodeDescriptorBuilder<Derived> &builder) {
        using meta = typename input_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.input(make_socket_descriptor<T>(meta::name, meta::description));
      }
    };

    template <size_t i> struct output_reg_fn {
      static constexpr void eval(NodeDescriptorBuilder<Derived> &builder) {
        using meta = typename input_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.output(
            make_socket_descriptor<T>(meta::name, meta::description));
      }
    };
  };

  static constexpr const auto &register_node() {
    NodeDescriptorBuilder<Derived> builder(intern_node_register::name,
                                           intern_node_register::description);
    intern::static_for_eval<
        0, intern::count_socket_v<typename intern_node_register::input_metas>,
        intern_node_register::template input_reg_fn>(builder);
    intern::static_for_eval<
        0, intern::count_socket_v<typename intern_node_register::output_metas>,
        intern_node_register::template output_reg_fn>(builder);

    intern::call_on_register_if_presented<Derived>::exec();
    return builder.build();
  }

protected:

  template <typename MT> auto const *get(MT) const {
    constexpr size_t index = MT::index;
    return get_input(index).template as<std::add_const_t<typename MT::type>>();
  }

  template <typename MT> auto const& get_or(MT) const {
    static_assert(intern::has_default_value_v<MT>, "No default value.");
    constexpr size_t index = MT::index;
    return has_input(index) ? *get<MT>({}) : default_value<MT>({});
  }

  template <typename MT, typename... Args> auto &set(MT, Args &&...args) {
    constexpr size_t index = MT::index;
    using T = typename MT::type;
    return set_output<T>(index, std::forward<Args>(args)...);
  }

  template <typename MT> static auto const& default_value(MT) noexcept {
    return MT{}.default_value();
  }

};

}
