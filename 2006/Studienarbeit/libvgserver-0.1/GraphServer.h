// $Id: GraphServer.h 50 2006-02-01 09:52:10Z bingmann $

#ifndef VGS_GraphServer_H
#define VGS_GraphServer_H

#include "GraphContainer.h"

namespace VGServer {

/** GraphServer contains the main loop of a graph server which waits for a new
 * connection. It contains the factory method to create a new application
 * specific connection class. This abstract class must be derived by the
 * application, which will initialize the global graph and filled with the
 * factory code. Currently the run function is empty. This class will be
 * derived into GraphCORBAServer and possibly GraphXMLRPCServer. */

class GraphServer : public GraphContainer
{
public:
    GraphServer(const class GraphProperties &properties);

    virtual ~GraphServer();

    /// when a new incoming connection is accepted, then the server uses this
    /// to construct a new handler object for it.
    // virtual class GraphConnection* newGraphConnection();

    /// start the accept loop.
    int		run();
};

} // namespace VGServer

#endif // VGS_GraphServer_H
