#pragma once

#define CG_NAMESPACE compute_graph
#define CG_NAMESPACE_BEGIN namespace CG_NAMESPACE {
#define CG_NAMESPACE_END }

#ifndef CG_NODE_PLUGINS
#define CG_NODE_PLUGINS
#endif

CG_NAMESPACE_BEGIN
class NodeDescriptor;       // Describe how to create a node.
class SocketDescriptor; // Describe a socket on a node.

class NodeBase; // Base type for each node.
class SocketBase; // a socket on a node.
class Pipe; // one connection between two sockets.
class PayloadBase; // the data that flows through the pipe.
class GraphContext; // the context of the graph.

CG_NAMESPACE_END
