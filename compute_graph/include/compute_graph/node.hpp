//
// Created by adversarr on 2024/8/2.
//

#pragma once

#include <memory>
#include <vector>

#include "intern/pp.hpp"
#include "socket_descriptor.hpp"
#include "socket.hpp"

CG_NAMESPACE_BEGIN

class NodeBase {
public:
    explicit NodeBase(NodeDescriptor const& descriptor) noexcept
        : descriptor_(descriptor) {};

    virtual ~NodeBase() noexcept = default;

    // execute, may throw exception.
    virtual void operator()() = 0;

    // After connect to some pipe, may throw exception.
    virtual void on_connect(Pipe const& pipe);

    // Before disconnect from some pipe, must be noexcept.
    virtual void on_disconnect(Pipe const& pipe) noexcept;

    // Plugins.
    CG_NODE_PLUGINS

private:
    std::vector<InputSocket> inputs_;
    std::vector<OutputSocket> outputs_;
    NodeDescriptor const& descriptor_;
};

CG_NAMESPACE_END
