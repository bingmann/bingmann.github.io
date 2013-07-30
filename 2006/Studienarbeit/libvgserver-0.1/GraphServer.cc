// $Id: GraphServer.cc 19 2006-01-06 18:03:44Z bingmann $

#include "GraphServer.h"

namespace VGServer {

GraphServer::GraphServer(const class GraphProperties &properties)
    : GraphContainer(properties)
{
}

GraphServer::~GraphServer()
{
}

int GraphServer::run()
{
    return -1;
}

} // namespace VGServer
