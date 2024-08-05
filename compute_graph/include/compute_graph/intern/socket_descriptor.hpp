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
#include "config.hpp"

namespace compute_graph {

class SocketDescriptor {
public:
  CG_STRONG_INLINE SocketDescriptor(TypeIndex type, std::string_view name,
                                    std::string_view desc) noexcept
      : type_(type), name_(name), desc_(desc), pretty_typename_{} {}

  CG_STRONG_INLINE SocketDescriptor(TypeIndex type, std::string_view name, std::string_view desc,
                                    std::string_view pretty_typename) noexcept
      : type_(type), name_(name), desc_(desc), pretty_typename_(pretty_typename) {}
  CG_STRONG_INLINE SocketDescriptor(SocketDescriptor const &) noexcept = default;
  CG_STRONG_INLINE SocketDescriptor(SocketDescriptor &&) noexcept = default;

  CG_STRONG_INLINE TypeIndex const &type() const noexcept { return type_; }
  CG_STRONG_INLINE std::string const &name() const noexcept { return name_; }
  CG_STRONG_INLINE std::string const &desc() const noexcept { return desc_; }
  CG_STRONG_INLINE std::string const &pretty_typename() const noexcept { return pretty_typename_; }

private:
  const TypeIndex type_;
  const std::string name_;
  const std::string desc_;
  const std::string pretty_typename_;
};

template <typename T>
CG_STRONG_INLINE SocketDescriptor make_socket_descriptor(std::string_view name,
                                                         std::string_view desc) {
  return {typeid(T), name, desc};
}


template <typename T>
CG_STRONG_INLINE SocketDescriptor make_socket_descriptor(std::string_view name, std::string_view desc,
                                                         std::string_view pretty_typename) {
  return {typeid(T), name, desc, pretty_typename};
}


}  // namespace compute_graph
