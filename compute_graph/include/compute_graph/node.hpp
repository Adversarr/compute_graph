// The MIT License (MIT)
// Copyright © 2024 Adversarr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <vector>
#include <optional>

#include "intern/node_descriptor.hpp"
#include "intern/pp.hpp"
#include "intern/traits.hpp"
#include "socket.hpp"

namespace compute_graph {

class NodeBase CG_NODE_INHERITANCE {
public:
  CG_STRONG_INLINE explicit NodeBase(NodeDescriptor const &descriptor) noexcept
      : inputs_{}, outputs_{}, descriptor_(descriptor) {
    inputs_.reserve(descriptor.inputs().size());
    outputs_.reserve(descriptor.outputs().size());

    for (size_t i = 0; i < descriptor.inputs().size(); ++i) {
      inputs_.push_back({descriptor.inputs()[i].type(), *this, i});
    }

    for (size_t i = 0; i < descriptor.outputs().size(); ++i) {
      outputs_.push_back({descriptor.outputs()[i].type(), *this, i});
    }
  }

  NodeBase(NodeBase const &) = delete;
  NodeBase(NodeBase &&) noexcept = default;
  NodeBase &operator=(NodeBase const &) = delete;
  NodeBase &operator=(NodeBase &&) = delete;

  virtual ~NodeBase() noexcept = default;

  // execute, may throw exception.
  virtual void operator()(Context &) = 0;

  CG_STRONG_INLINE NodeDescriptor const &descriptor() const noexcept { return descriptor_; }

  CG_STRONG_INLINE auto const &inputs() const noexcept { return inputs_; }
  CG_STRONG_INLINE auto const &outputs() const noexcept { return outputs_; }

  std::optional<size_t> find_input(std::string_view name) const noexcept {
    return descriptor_.find_input(name);
  }

  std::optional<size_t> find_output(std::string_view name) const noexcept {
    return descriptor_.find_output(name);
  }

protected:
  virtual void on_connect(size_t /*index*/) noexcept {}

  CG_STRONG_INLINE TypeErasedPtr const &get(size_t index) const {
#ifndef CG_NO_CHECK
    return inputs_.at(index).fetch_payload();
#else
    return inputs_[index].fetch_payload();
#endif
  }

  template <typename T>
  CG_STRONG_INLINE T const *get(size_t index) const CG_NOEXCEPT {
#ifndef CG_NO_CHECK
    if (inputs_[index].type() != typeid(T)) {
      CG_THROW(std::invalid_argument, "Type mismatch");
    }
#endif
    return static_cast<T const *>(get(index).get());
  }

  CG_STRONG_INLINE bool has(size_t index) const noexcept {
#ifndef CG_NO_CHECK
    return !inputs_.at(index).is_empty();
#else
    return !inputs_[index].is_empty();
#endif
  }

  template <typename T, typename... Args>
  CG_STRONG_INLINE auto &set(size_t index, Args &&...args) {
    return outputs_[index].emplace<T>(std::forward<Args>(args)...);
  }

  std::vector<InputSocket> inputs_;
  std::vector<OutputSocket> outputs_;

private:
  friend class Graph;
  NodeDescriptor const & descriptor_;
};

// crtp.
template <typename Derived> class NodeDerive : public NodeBase {
public:
  CG_STRONG_INLINE explicit NodeDerive(NodeDescriptor const &descriptor) noexcept
      : NodeBase(descriptor) {}

  ~NodeDerive() noexcept override = default;

  struct intern_node_traits {
    using input_metas = typename Derived::intern_input_meta;
    using output_metas = typename Derived::intern_output_meta;
    template <size_t i> struct input_reg_fn {
      static CG_STRONG_INLINE void eval(NodeDescriptorBuilder<Derived> &builder) noexcept {
        using meta = typename input_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.input(make_socket_descriptor<T>(meta::name, meta::description, meta::pretty_typename));
      }
    };

    template <size_t i> struct output_reg_fn {
      static CG_STRONG_INLINE void eval(NodeDescriptorBuilder<Derived> &builder) noexcept {
        using meta = typename output_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.output(make_socket_descriptor<T>(meta::name, meta::description, meta::pretty_typename));
      }
    };

    template <size_t i> struct input_on_connect_fn {
      static CG_STRONG_INLINE void eval(size_t index, Derived &node) noexcept {
        using meta = typename input_metas::template socket_meta<i>;
        if (index == i) {
          intern::call_on_connect_mt_if_presented<Derived, meta>::exec(node, meta{});
        }
      }
    };

    template <size_t i> struct input_default_value_fn {
      template <bool enable, typename> struct avoid_if_constexpr {
        CG_STRONG_INLINE static void eval(size_t, void const*&) {}
      };

      template <typename Dummy> struct avoid_if_constexpr<true, Dummy> {
        CG_STRONG_INLINE static void eval(size_t index, void const*& data) {
          using meta = typename input_metas::template socket_meta<i>;
          if (index == i) {
            data = &meta::default_value();
          }
        }
      };

      static CG_STRONG_INLINE void eval(size_t index, void const*& data) noexcept {
        using meta = typename input_metas::template socket_meta<i>;
        avoid_if_constexpr<has_default_value_v<meta>, void>::eval(index, data);
      }
    };
    static constexpr size_t num_inputs = intern::count_socket_v<input_metas>;
    static constexpr size_t num_outputs = intern::count_socket_v<output_metas>;
  };


  static CG_STRONG_INLINE NodeDescriptor build_descriptor() {
    NodeDescriptorBuilder<Derived> builder(Derived::name, Derived::description);
    intern::static_for_eval<0, intern_node_traits::num_inputs,
                            intern_node_traits::template input_reg_fn>(builder);
    intern::static_for_eval<0, intern_node_traits::num_outputs,
                            intern_node_traits::template output_reg_fn>(builder);
    intern::call_on_register_if_presented<Derived>::exec();
    return builder.build();
  }


protected:
  CG_STRONG_INLINE void on_connect(size_t index) noexcept override {
    constexpr size_t total = intern_node_traits::num_inputs;
    intern::static_for_eval<0, total, intern_node_traits::template input_on_connect_fn>(
        index, *static_cast<Derived *>(this));
  }

  using NodeBase::set, NodeBase::has;

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  CG_STRONG_INLINE auto const *get(MT) const noexcept {
    constexpr size_t index = MT::index;
    return static_cast<typename MT::type const *>(NodeBase::get(index).get());
  }

  template <typename T, typename = std::enable_if_t<!intern::is_socket_meta_v<T>>>
  CG_STRONG_INLINE T const* get(size_t index) const {
    return NodeBase::get<T>(index);
  }

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  CG_STRONG_INLINE auto const &get_or(MT) const {
    constexpr size_t index = MT::index;
    return has(index) ? *get<MT>({}) : default_value<MT>({});
  }

  template <typename T, typename = std::enable_if_t<!intern::is_socket_meta_v<T>>>
  CG_STRONG_INLINE T const* get_or(size_t index) const {
    return static_cast<T const*>(has(index) ? get<T>(index) : default_value(index));
  }

  template <typename MT, typename... Args,
            typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  CG_STRONG_INLINE auto &set(MT, Args &&...args) {
    constexpr size_t index = MT::index;
    using T = typename MT::type;
    return set<T>(index, std::forward<Args>(args)...);
  }

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  static CG_STRONG_INLINE auto const &default_value(MT) noexcept {
    static_assert(intern::has_default_value_v<MT>, "No default value.");
    return MT::default_value();
  }

  template <typename T, typename = std::enable_if_t<std::is_convertible_v<T, size_t>>>
  static CG_STRONG_INLINE void const* default_value(T index) CG_NOEXCEPT {
#ifndef CG_NO_CHECK
    if (index >= intern_node_traits::num_inputs) {
      CG_THROW(std::out_of_range, "Input index out of range.");
    }
#endif
    void const* data = nullptr;
    auto const ind = static_cast<size_t>(index);
    intern::static_for_eval<0, intern_node_traits::num_inputs,
              intern_node_traits::template input_default_value_fn>(ind, data);
    return data;
  }

  template <typename MT, typename = std::enable_if_t<intern::is_socket_meta_v<MT>>>
  CG_STRONG_INLINE bool has(MT) const noexcept {
    constexpr size_t index = MT::index;
    return has(index);
  }

  template <size_t i> static CG_STRONG_INLINE auto const &default_value() noexcept {
    static_assert(i < intern_node_traits::num_inputs, "Index out of range.");
    using MT = typename intern_node_traits::input_metas::template socket_meta<i>;
    return default_value<MT>({});
  }

private:
  template <size_t ... Idx> auto get_all(std::index_sequence<Idx...>) const {
    using inputs = typename intern_node_traits::input_metas;
    return std::make_tuple(get<typename inputs::template socket_meta<Idx, int>>({})...);
  }

  template <typename ... Args, size_t ... Idx>
  auto set_all_impl(std::index_sequence<Idx ...>, std::tuple<Args...> arg) {
    using outputs = typename intern_node_traits::output_metas;
    return std::make_tuple(set<typename outputs::template socket_meta<Idx, int>::type>(
                           Idx, std::get<Idx>(arg)) ...);
  }

protected:
  CG_STRONG_INLINE auto get_all() const {
    return get_all(std::make_index_sequence<intern_node_traits::num_inputs>());
  }

  template <typename ... Args>
  CG_STRONG_INLINE auto set_all(Args && ... args) {
    return set_all_impl(std::make_index_sequence<intern_node_traits::num_outputs>(),
                        std::tuple<Args&&...>(std::forward<Args>(args)...));
  }

private:
  using NodeBase::inputs_, NodeBase::outputs_;
};


}  // namespace compute_graph

// Helper macros to define a node.
#define CG_NODE_SOCKET_IMPL(ith, Type, Name, desc, ...)              \
  template <typename _WHATEVER> struct socket_meta<ith, _WHATEVER>   \
      : ::compute_graph::intern::socket_meta_base {                  \
    using type = Type;                                               \
    static constexpr size_t index = ith;                             \
    static constexpr const char *name = #Name;                       \
    static constexpr const char *pretty_typename = #Type;            \
    static constexpr const char *description = desc;                 \
    __VA_OPT__(static CG_STRONG_INLINE Type const &default_value() { \
      static Type _v{__VA_ARGS__};                                   \
      return _v;                                                     \
    })                                                               \
  };                                                                 \
  using Name##_ = socket_meta<ith, int>;                             \
  static constexpr Name##_ Name{};

#define CG_NODE_PP_ADAPTOR(x, i) \
  CG_PP_EVAL(CG_NODE_SOCKET_IMPL CG_PP_EMPTY()(i, CG_PP_TUPLE_UNPACK x))

// Usage:
// CG_NODE_INPUTS(
//    (<type>, <identifier>, <description>, <optional-default-value>),
//    (<type>, <identifier>, <description>, <optional-default-value>),
//    ...);
// Example:
// CG_NODE_INPUTS(
//    (int,         x, "integer input", 0 /* default = 0      */),
//    (std::string, y, "string input"     /* no default value */),
#define CG_NODE_INPUTS(...)                                             \
  typedef struct intern_input_meta {                                    \
    template <size_t I, typename = int> struct socket_meta {            \
      using type = void;                                                \
    };                                                                  \
    __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__)) \
  } in

// Usage:
// CG_NODE_OUTPUTS(
//    (<type>, <identifier>, <description>),
//    (<type>, <identifier>, <description>),
//    ...);
#define CG_NODE_OUTPUTS(...)                                            \
  typedef struct intern_output_meta {                                   \
    template <size_t I, typename = int> struct socket_meta {            \
      using type = void;                                                \
    };                                                                  \
    __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__)) \
  } out

#ifdef CG_NO_AUTO_REGISTER
#  define CG_NODE_REGISTER_BODY(NodeType) /* empty */
#else
#  define CG_NODE_REGISTER_BODY(NodeType)                    \
    struct intern_auto_register {                            \
      CG_STRONG_INLINE intern_auto_register() {              \
        auto &r = ::compute_graph::NodeRegistry::instance(); \
        r.emplace(NodeType::build_descriptor());             \
      }                                                      \
    };                                                       \
    inline static const intern_auto_register intern_register
#endif

// Use to define a node.
#define CG_NODE_COMMON(NodeType, Name, Desc)                                        \
  CG_STRONG_INLINE explicit NodeType(NodeDescriptor const &descriptor) noexcept     \
      : NodeDerive<NodeType>(descriptor) {                                          \
    ::compute_graph::intern::call_on_construct_if_presented<NodeType>::exec(*this); \
  }                                                                                 \
public:                                                                             \
  friend class NodeDescriptorBuilder<NodeType>;                                     \
  static constexpr const char *name = Name;                                         \
  static constexpr const char *description = Desc;                                  \
  CG_NODE_REGISTER_BODY(NodeType)
