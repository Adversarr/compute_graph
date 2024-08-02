//
// Created by adversarr on 2024/8/2.
//

#pragma once
#include <vector>

#include "intern/node_descriptor.hpp"
#include "intern/pp.hpp"
#include "socket.hpp"

CG_NAMESPACE_BEGIN

class NodeBase {
public:
  explicit NodeBase(NodeDescriptor const &descriptor) noexcept
      : descriptor_(descriptor){};

  virtual ~NodeBase() noexcept = default;

  // execute, may throw exception.
  virtual void operator()() = 0;

  // After connect to some pipe, may throw exception.
  virtual void on_connect(Pipe const &pipe);

  // Before disconnect from some pipe, must be noexcept.
  virtual void on_disconnect(Pipe const &pipe) noexcept;

  // Plugins.
  CG_NODE_PLUGINS

protected:
  inline TypeErasedPtr const &get_input(size_t index) const {
    return inputs_[index].pipe()->payload();
  }

  template <typename T, typename... Args>
  void set(size_t index, Args &&...args) {
    outputs_[index].put<T>(std::forward<Args>(args)...);
  }

  template<typename T> auto* get(T) const {
    constexpr size_t index = T::index;
    return NodeBase::get_input(index).template as<std::add_const_t<typename T::type>>();
  }

  template <typename MT, typename ... Args>
  void set(MT, Args &&...args) {
    constexpr size_t index = MT::index;
    using T = typename MT::type;
    NodeBase::set<T>(index, std::forward<Args>(args)...);
  }

private:
  std::vector<InputSocket> inputs_;
  std::vector<OutputSocket> outputs_;
  NodeDescriptor const &descriptor_;
};

CG_NAMESPACE_END
