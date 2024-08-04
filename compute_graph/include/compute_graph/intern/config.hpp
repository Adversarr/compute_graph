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

// NOTE: Possible control macros are
//   CG_NO_AUTO_REGISTER:-----------------------------------------------------+
//   |  Controls whether to automatically register nodes before main().       |
//   |  Define this macro to disable auto registration, used in `node.hpp`.   |
//   +------------------------------------------------------------------------+
//   CG_NO_STRONG_INLINE:-----------------------------------------------------+
//   |  controls whether to use __forceinline or not, see below.              |
//   |  Define this macro to disable __forceinline, otherwise just `inline`.  |
//   +------------------------------------------------------------------------+
//   CG_NODE_EXTENSION:-------------------------------------------------------+
//   |  A control macro that can be defined to add extra code to the node     |
//   |  class. This is useful for adding extra methods or members to the      |
//   |  node class.                                                           |
//   +------------------------------------------------------------------------+
//   CG_NO_EXCEPTION:---------------------------------------------------------+
//   |  Controls whether to use exceptions or not.                            |
//   |  Define this macro to disable exceptions, and check failures will      |
//   |  fallback to abort().                                                  |
//   +------------------------------------------------------------------------+
//   CG_NO_CHECK:-------------------------------------------------------------+
//   |  Controls whether to use runtime checks or not.                        |
//   |  Define this macro to disable runtime checks, and check failures will  |
//   |  fallback to undefined behavior.                                       |
//   +------------------------------------------------------------------------+

#pragma once

#include <cstddef>
#include <string>
#include <typeindex>
#include <variant>

#ifdef CG_NO_STRONG_INLINE
#  define CG_STRONG_INLINE inline
#else
#  ifdef _MSC_VER
#    define CG_STRONG_INLINE __forceinline
#  elif defined(__GNUC__)
#    define CG_STRONG_INLINE inline __attribute__((always_inline))
#  elif defined(__clang__)
#    define CG_STRONG_INLINE inline __attribute__((always_inline))
#  else
#    define CG_STRONG_INLINE inline
#  endif
#endif

#ifndef CG_NODE_EXTENSION
#  define CG_NODE_EXTENSION /* empty */
#endif

#ifdef CG_NO_CHECK
#  define CG_THROW(type, ...) /* empty */
#  define CG_NOEXCEPT noexcept
#else
#  ifdef CG_NO_EXCEPTION
#    define CG_THROW(type, ...) abort()
#    define CG_NOEXCEPT noexcept
#  else
#include <stdexcept>
#    define CG_THROW(type, ...) throw type(__VA_ARGS__)
#    define CG_NOEXCEPT
#  endif
#endif

#if __cplusplus >= 201703L
#  define CG_CONSTEXPR constexpr
#else
#  define CG_CONSTEXPR inline
#endif

namespace compute_graph {

using size_t = std::size_t;

class NodeDescriptor;    // Describe how to create a node.
class SocketDescriptor;  // Describe a socket on a node.

class NodeBase;      // Base type for each node.
class InputSocket;   // a socket on a node.
class OutputSocket;  // a socket on a node.
class Link;          // one connection between two sockets.
class Graph;         // the context of the graph.

using TypeIndex = std::type_index;
using utype_ident = std::variant<std::string, TypeIndex>;

CG_STRONG_INLINE std::string to_string(TypeIndex type_index) { return type_index.name(); }

CG_STRONG_INLINE std::string to_string(utype_ident ident) {
  return std::visit(
      [](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          return arg;
        } else {
          return to_string(arg);
        }
      },
      ident);
}

}  // namespace compute_graph

template <> struct std::hash<compute_graph::utype_ident> {
  CG_STRONG_INLINE std::size_t operator()(const compute_graph::utype_ident &id) const noexcept {
    return std::visit(
        [](auto &&arg) -> std::size_t {
          using T = std::decay_t<decltype(arg)>;
          return std::hash<T>{}(arg);
        },
        id);
  }
};
