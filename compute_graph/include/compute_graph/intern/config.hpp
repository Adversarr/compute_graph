#pragma once

#include <cstddef>
#include <string>
#include <typeindex>
#include <variant>

namespace compute_graph {

using size_t = std::size_t;

class NodeDescriptor;   // Describe how to create a node.
class SocketDescriptor; // Describe a socket on a node.

class NodeBase;     // Base type for each node.
class InputSocket;  // a socket on a node.
class OutputSocket; // a socket on a node.
class Link;         // one connection between two sockets.
class PayloadBase;  // the data that flows through the pipe.
class Graph;        // the context of the graph.

using TypeIndex = std::type_index;
using utype_ident = std::variant<std::string, TypeIndex>;

std::string to_string(TypeIndex type_index) {
  return type_index.name();
}

std::string to_string(utype_ident ident) {
  return std::visit([](auto&& arg) -> std::string {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, std::string>) {
      return arg;
    } else {
      return to_string(arg);
    }
  }, ident);
}

} // namespace compute_graph

template<>
struct std::hash<compute_graph::utype_ident> {
  constexpr std::size_t operator()(const compute_graph::utype_ident &id) const noexcept {
    return std::visit(
      [](auto &&arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;
        return std::hash<T>{}(arg);
      }, id);
  }
};
