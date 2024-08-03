#pragma once
#include "config.hpp"

namespace compute_graph {

class SocketDescriptor {
public:
  SocketDescriptor(utype_ident type, std::string name,
                   std::string desc) noexcept
      : type_(std::move(type)), name_(std::move(name)), desc_(std::move(desc)) {
  }
  SocketDescriptor(SocketDescriptor const &) noexcept = default;
  SocketDescriptor(SocketDescriptor &&) noexcept = default;

  utype_ident const &type() const noexcept { return type_; }
  std::string const &name() const noexcept { return name_; }
  std::string const &desc() const noexcept { return desc_; }

private:
  const utype_ident type_;
  const std::string name_;
  const std::string desc_;
};

template <typename T>
SocketDescriptor make_socket_descriptor(std::string name, std::string desc) {
  return {typeid(T), std::move(name), std::move(desc)};
}

} // namespace compute_graph
