#pragma once

#include "config.hpp"
#include <string>
#include <typeindex>

CG_NAMESPACE_BEGIN

class SocketDescriptor {
public:
  SocketDescriptor(std::type_index type, std::string name,
                   std::string desc) noexcept
      : type_(type), name_(std::move(name)), desc_(std::move(desc)) {}
  SocketDescriptor(SocketDescriptor const &) noexcept = default;
  SocketDescriptor(SocketDescriptor &&) noexcept = default;

  std::type_index type() const noexcept { return type_; }
  std::string const &name() const noexcept { return name_; }
  std::string const &desc() const noexcept { return desc_; }

private:
  const std::type_index type_;
  const std::string name_;
  const std::string desc_;
};

template <typename T>
SocketDescriptor make_socket_descriptor(std::string name, std::string desc) {
  return {typeid(T), std::move(name), std::move(desc)};
}

CG_NAMESPACE_END
