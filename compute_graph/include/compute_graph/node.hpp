#pragma once
#include <vector>

#include "intern/node.hpp"
#include "intern/node_descriptor.hpp"
#include "intern/pp.hpp"
#include "socket.hpp"

CG_NAMESPACE_BEGIN

class NodeBase {
public:
  explicit NodeBase(NodeDescriptor const *descriptor) noexcept
      : descriptor_(descriptor) {
    std::transform(descriptor->inputs().begin(), descriptor->inputs().end(),
                   std::back_inserter(inputs_),
                   [](auto const &desc) { return InputSocket(desc.type()); });
    std::transform(descriptor->outputs().begin(), descriptor->outputs().end(),
                   std::back_inserter(outputs_),
                   [](auto const &desc) { return OutputSocket(desc.type()); });
  }

  virtual ~NodeBase() noexcept = default;

  // execute, may throw exception.
  virtual void operator()(Graph &) = 0;

  // After connect to some pipe, may throw exception.
  virtual void on_connect(Link const &pipe) {}

  // Before disconnect from some pipe, must be noexcept.
  virtual void on_disconnect(Link const &pipe) noexcept {}

  // Plugins.
  CG_NODE_PLUGINS

  NodeDescriptor const *descriptor() const noexcept { return descriptor_; }

  auto const &inputs() const noexcept { return inputs_; }
  auto const &outputs() const noexcept { return outputs_; }

protected:
  template <typename T> auto *get(T) const {
    constexpr size_t index = T::index;
    return get_input(index).template as<std::add_const_t<typename T::type>>();
  }

  template <typename MT, typename... Args> auto &set(MT, Args &&...args) {
    constexpr size_t index = MT::index;
    using T = typename MT::type;
    return set_output<T>(index, std::forward<Args>(args)...);
  }

private:
  inline TypeErasedPtr const &get_input(size_t index) const {
    return (inputs_[index].fetch()).value();
  }

  template <typename T, typename... Args>
  auto &set_output(size_t index, Args &&...args) {
    return outputs_[index].emplace<T>(std::forward<Args>(args)...);
  }

  std::vector<InputSocket> inputs_;
  std::vector<OutputSocket> outputs_;
  NodeDescriptor const *descriptor_;
};

// crtp.
template <typename Derived> class NodeDerive : public NodeBase {
public:
  NodeDerive(NodeDescriptor const *descriptor) noexcept : NodeBase(descriptor) {
    static_assert(std::is_base_of_v<NodeDerive, Derived>,
                  "Derived must be derived from NodeDerive");
  }

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
    return builder.build();
  }
};

CG_NAMESPACE_END
