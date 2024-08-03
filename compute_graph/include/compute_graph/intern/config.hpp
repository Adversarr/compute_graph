#pragma once

#include <cstddef>
#define CG_NAMESPACE compute_graph
#define CG_NAMESPACE_BEGIN namespace CG_NAMESPACE {
#define CG_NAMESPACE_END }

#ifndef CG_NODE_PLUGINS
#define CG_NODE_PLUGINS
#endif

CG_NAMESPACE_BEGIN

using size_t = std::size_t;

class NodeDescriptor;   // Describe how to create a node.
class SocketDescriptor; // Describe a socket on a node.

class NodeBase;     // Base type for each node.
class InputSocket;  // a socket on a node.
class OutputSocket; // a socket on a node.
class Link;         // one connection between two sockets.
class PayloadBase;  // the data that flows through the pipe.
class Graph; // the context of the graph.

CG_NAMESPACE_END
