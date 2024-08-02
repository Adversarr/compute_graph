#pragma once

#include "intern/config.hpp"
#include "intern/type_erased_ptr.hpp"

CG_NAMESPACE_BEGIN
enum class SocketType {
    Input,
    Output
};

class SocketBase {
public:
    explicit SocketBase(std::type_index type,
                        SocketType io_type) noexcept
        : type_(type), io_type_(io_type) {
    }

    std::type_index type() const noexcept {
        return type_;
    }

    virtual SocketType io_type() const noexcept {
        return io_type_;
    }

    virtual ~SocketBase() noexcept = default;

private:
    const std::type_index type_;
    const SocketType io_type_;
};


class InputSocket : public SocketBase {
public:
    explicit InputSocket(std::type_index type) noexcept
        : SocketBase(type, SocketType::Input) {}

    ~InputSocket() noexcept override = default;

    void connect(Pipe* pipe) noexcept {
        pipe_ = pipe;
    }

    Pipe* pipe() const noexcept {
        return pipe_;
    }

private:
    Pipe* pipe_;
};

class OutputSocket : public SocketBase {
public:
    explicit OutputSocket(std::type_index type) noexcept
        : SocketBase(type, SocketType::Output) {}
    ~OutputSocket() noexcept override = default;

    std::vector<Pipe*> const& connected_pipes() const noexcept {
        return connected_pipes_;
    }
private:
    std::vector<Pipe*> connected_pipes_;
};

CG_NAMESPACE_END