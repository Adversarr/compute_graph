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

#include "intern/node.hpp"
#include "intern/node_descriptor.hpp"
#include "intern/pp.hpp"
#include "socket.hpp"

namespace compute_graph {

class NodeBase {
public:
  CG_STRONG_INLINE explicit NodeBase(NodeDescriptor const *descriptor) noexcept
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

  CG_STRONG_INLINE NodeDescriptor const *descriptor() const noexcept { return descriptor_; }

  CG_STRONG_INLINE auto const &inputs() const noexcept { return inputs_; }
  CG_STRONG_INLINE auto const &outputs() const noexcept { return outputs_; }

  CG_NODE_EXTENSION

protected:

  CG_STRONG_INLINE TypeErasedPtr const &get_input(size_t index) const {
    return (inputs_[index].fetch()).value();
  }

  CG_STRONG_INLINE bool has_input(size_t index) const noexcept {
    return !inputs_[index].is_empty();
  }

  template <typename T, typename... Args>
  CG_STRONG_INLINE auto &set_output(size_t index, Args &&...args) {
    return outputs_[index].emplace<T>(std::forward<Args>(args)...);
  }


private:
  friend class Graph;
  std::vector<InputSocket> inputs_;
  std::vector<OutputSocket> outputs_;
  NodeDescriptor const * const descriptor_;
};

// crtp.
template <typename Derived> class NodeDerive : public NodeBase {
public:
  CG_STRONG_INLINE explicit NodeDerive(NodeDescriptor const *descriptor) noexcept:
    NodeBase(descriptor) {
    intern::call_on_construct_if_presented<Derived>::exec(static_cast<Derived&>(*this));
  }

  struct intern_node_register {
    static constexpr const char *name = Derived::name;
    static constexpr const char *description = Derived::description;
    using input_metas = typename Derived::intern_input_meta;
    using output_metas = typename Derived::intern_output_meta;
    template <size_t i> struct input_reg_fn {
      static CG_STRONG_INLINE void eval(NodeDescriptorBuilder<Derived> &builder) {
        using meta = typename input_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.input(make_socket_descriptor<T>(meta::name, meta::description));
      }
    };

    template <size_t i> struct output_reg_fn {
      static CG_STRONG_INLINE void eval(NodeDescriptorBuilder<Derived> &builder) {
        using meta = typename output_metas::template socket_meta<i>;
        using T = typename meta::type;
        builder.output(
            make_socket_descriptor<T>(meta::name, meta::description));
      }
    };
  };

  static CG_STRONG_INLINE const auto &register_node() {
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

  template <typename MT> CG_STRONG_INLINE auto const *get(MT) const {
    constexpr size_t index = MT::index;
    return get_input(index).template as<std::add_const_t<typename MT::type>>();
  }

  template <typename MT> CG_STRONG_INLINE auto const& get_or(MT) const {
    static_assert(intern::has_default_value_v<MT>, "No default value.");
    constexpr size_t index = MT::index;
    return has_input(index) ? *get<MT>({}) : default_value<MT>({});
  }

  template <typename MT, typename... Args> CG_STRONG_INLINE auto &set(MT, Args &&...args) {
    constexpr size_t index = MT::index;
    using T = typename MT::type;
    return set_output<T>(index, std::forward<Args>(args)...);
  }

  template <typename MT> static CG_STRONG_INLINE auto const& default_value(MT) noexcept {
    return MT::default_value();
  }

};

}

// Helper macros to define a node.
#define CG_NODE_SOCKET_IMPL(ith, Type, Name, desc, ...)         \
  template <typename _WHATEVER> struct socket_meta<ith, _WHATEVER> {  \
    using type = Type;                                                \
    static constexpr size_t index = ith;                              \
    static constexpr const char *name = #Name;                        \
    static constexpr const char* description = desc;                  \
    __VA_OPT__(                                                       \
      static CG_STRONG_INLINE Type const& default_value()  {                    \
      static Type _v {__VA_ARGS__}; return _v;                        \
    } )                                                               \
  };                                                                  \
  using Name##_t = socket_meta<ith, int>;                             \
  static constexpr Name##_t Name{};

#define CG_NODE_PP_ADAPTOR(x, i) \
    CG_PP_EVAL(CG_NODE_SOCKET_IMPL CG_PP_EMPTY() (i, CG_PP_TUPLE_UNPACK x))


// Usage:
// CG_NODE_INPUTS(
//    (<type>, <identifier>, <description>, <optional-default-value>),
//    (<type>, <identifier>, <description>, <optional-default-value>),
//    ...);
// Example:
// CG_NODE_INPUTS(
//    (int,         x, "integer input", 0 /* default = 0      */),
//    (std::string, y, "string input"     /* no default value */),
#define CG_NODE_INPUTS(...)                                               \
    typedef struct intern_input_meta {                                    \
      template<size_t I, typename=int> struct socket_meta {               \
        using type = void;                                                \
      };                                                                  \
      __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__)) \
    } in

// Usage:
// CG_NODE_OUTPUTS(
//    (<type>, <identifier>, <description>),
//    (<type>, <identifier>, <description>),
//    ...);
#define CG_NODE_OUTPUTS(...) \
    typedef struct intern_output_meta { \
      template<size_t I, typename = int> struct socket_meta {             \
        using type = void;                                                \
      };                                                                  \
      __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__)) \
    } out


#ifdef CG_NO_AUTO_REGISTER
#define CG_NODE_REGISTER_BODY(NodeType) /* empty */
#else
#define CG_NODE_REGISTER_BODY(NodeType)                                        \
  struct intern_auto_register {                                                \
    CG_STRONG_INLINE intern_auto_register() { NodeType::register_node(); }     \
  }; inline static const intern_auto_register intern_register
#endif

// Use to define a node.
#define CG_NODE_COMMON(NodeType, Name, Desc)                                    \
  CG_STRONG_INLINE explicit NodeType(NodeDescriptor const *descriptor) noexcept \
      : NodeDerive<NodeType>(descriptor) {}                                     \
  friend class NodeDescriptorBuilder<NodeType>;                                 \
  static constexpr const char *name = Name;                                     \
  static constexpr const char *description = Desc;                              \
  CG_NODE_REGISTER_BODY(NodeType)
