/*******************************************************************************
 * src/graph.h
 *
 * A basic super-generic graph framework.
 *
 * Copyright (C) 2013-2016 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef BISPANNING_GRAPH_HEADER
#define BISPANNING_GRAPH_HEADER

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <map>
#include <ostream>
#include <queue>
#include <sstream>
#include <vector>

#include "debug.h"

//! A basic graph structure composed of a mix of adjacency array and list.
template <class VertexData, class EdgeData,
          bool _directed,
          bool _allow_parallel,
          bool _allow_loops,
          bool _allow_deletes>
class GraphBase
{
public:
    //! The type of all integer identifiers used.
    using id_type = unsigned int;

    //! Typedef the VertexData template parameter
    using vertex_data_type = VertexData;

    //! Typedef the EdgeData template parameter
    using edge_data_type = EdgeData;

    //! Invalid sentinel id
    enum { ID_INVALID = id_type(-1) };

    //! Each vertex contains a vector identifying the vertex's edges.
    class Vertex
    {
    public:
        //! edge list
        std::vector<id_type> edgelist;

        //! payload data
        VertexData data;

        explicit Vertex(const VertexData& d)
            : data(d)
        { }

        //! Returns true for vertexes marked as deleted.
        inline bool deleted() const
        {
            if (!allow_deletes) return false;
            return (edgelist.size() == 1 && edgelist[0] == ID_INVALID);
        }

        //! Remove an edge from the edgelist
        inline bool remove_edge(const id_type& e)
        {
            std::vector<id_type>::iterator it
                = std::find(edgelist.begin(), edgelist.end(), e);
            if (it == edgelist.end()) return false;
            edgelist.erase(it);
            return true;
        }
    };

    //! Each edge contains the ids of head and tail vertices of the edge.
    class Edge
    {
    public:
        //! tail (source) and head (target) vertices of the edge
        id_type tail, head;

        //! payload data
        EdgeData data;

        Edge(const id_type& t, const id_type& h, const EdgeData& d)
            : tail(t), head(h), data(d)
        { }

        //! Returns true for edges marked as deleted.
        inline bool deleted() const
        {
            if (!allow_deletes) return false;
            return (tail == ID_INVALID && head == ID_INVALID);
        }

        //! Return the vertex at the other end of the edge.
        inline const id_type & other_id(const id_type& v) const
        {
            assert(!deleted());
            assert(v == head || v == tail);
            return (v == head) ? tail : head;
        }

        //! ostream output operator
        friend std::ostream& operator << (std::ostream& os, const Edge& e)
        {
            return os << "(" << e.tail << "," << e.head << ")";
        }
    };

public:
    //! Indicator whether graph is directed
    enum { is_directed = _directed };

    //! Indicator whether graph is undirected
    enum { is_undirected = !_directed };

    //! Indicator whether to allow adding loops
    enum { allow_loops = _allow_loops };

    //! Indicator whether to allow adding parallel edges
    enum { allow_parallel = _allow_parallel };

    //! Indicator whether to allow deleting vertices and edges
    enum { allow_deletes = _allow_deletes };

protected:
    //! The vertex list by identifier
    std::vector<Vertex> m_vertex;

    //! The edge list by identifier
    std::vector<Edge> m_edge;

    //! Number of deleted vertexes
    size_t m_vertex_deleted;

    //! Number of deleted edges
    size_t m_edge_deleted;

public:
    // *** Prototype Definitions of Iterators

    class vertex_iterator;
    class const_vertex_iterator;

    class edge_iterator;
    class const_edge_iterator;

    class vertex_edge_iterator;
    class const_vertex_edge_iterator;

    class vertex_iterator_range;
    class const_vertex_iterator_range;
    class edge_iterator_range;
    class const_edge_iterator_range;
    class vertex_edge_iterator_range;
    class const_vertex_edge_iterator_range;

public:
    //! Construct a graph or digraph with n vertices
    explicit GraphBase(const id_type& n = 0, const VertexData& vd = VertexData())
        : m_vertex(n, Vertex(vd)),
          m_vertex_deleted(0), m_edge_deleted(0)
    { }

    //! Returns the number of valid vertexes in the graph.
    size_t num_vertex() const
    {
        if (!allow_deletes) return total_vertex();
        return m_vertex.size() - m_vertex_deleted;
    }

    //! Returns the total number of vertexes in the graph.
    size_t total_vertex() const
    {
        return m_vertex.size();
    }

    //! Returns the number of valid edges in the graph.
    size_t num_edge() const
    {
        if (!allow_deletes) return total_edge();
        return m_edge.size() - m_edge_deleted;
    }

    //! Returns the total number of edges in the graph.
    size_t total_edge() const
    {
        return m_edge.size();
    }

    //! Returns true if there are deleted objects in the graph.
    bool has_deleted() const
    {
        return (m_vertex_deleted != 0) || (m_edge_deleted != 0);
    }

    //! Returns the number of edges outgoing from v
    size_t vertex_num_edge(const id_type& v) const
    {
        assert(v < m_vertex.size());
        assert(!m_vertex[v].deleted());
        return m_vertex[v].edgelist.size();
    }

protected:
    //! Return data structure of a vertex
    Vertex & vertex_data(const id_type& v)
    {
        assert(v < m_vertex.size());
        return m_vertex[v];
    }

    //! Return const data structure of a vertex
    const Vertex & vertex_data(const id_type& v) const
    {
        assert(v < m_vertex.size());
        return m_vertex[v];
    }

    //! Return data structure of an edge
    Edge & edge_data(const id_type& e)
    {
        assert(e < m_edge.size());
        return m_edge[e];
    }

    //! Return const data structure of an edge
    const Edge & edge_data(const id_type& e) const
    {
        assert(e < m_edge.size());
        return m_edge[e];
    }

    //! Return edge identifier for a vertex_edge pair
    id_type vertex_edge_id(const id_type& v, const id_type& e) const
    {
        assert(v < m_vertex.size());
        assert(e < m_vertex[v].edgelist.size());
        return m_vertex[v].edgelist[e];
    }

    /// Return data structure of an edge id inside a vertex
    Edge & vertex_edge_data(const id_type& v, const id_type& e)
    {
        assert(v < m_vertex.size());
        assert(e < m_vertex[v].edgelist.size());
        return edge_data(m_vertex[v].edgelist[e]);
    }

    /// Return const data structure of an edge id inside a vertex
    const Edge & vertex_edge_data(const id_type& v, const id_type& e) const
    {
        assert(v < m_vertex.size());
        assert(e < m_vertex[v].edgelist.size());
        return edge_data(m_vertex[v].edgelist[e]);
    }

public:
    //! Mutable iterator for the vertex sequence of the graph.
    class vertex_iterator
    {
    private:
        //! Pointer to graph containing the vertex.
        GraphBase* m_graph;

        //! Identifier of the vertex
        id_type m_id;

    public:
        //! Construct vertex iterator pointing to specific vertex
        vertex_iterator(GraphBase* g, const id_type& v)
            : m_graph(g), m_id(v)
        {
            skip_deleted();
        }

        //! Return graph containing the vertex
        const GraphBase * graph() const
        {
            return m_graph;
        }

        //! Return id of vertex
        const id_type & vertex_id() const
        {
            return m_id;
        }

        //! Returns true if the vertex is deleted
        bool deleted() const
        {
            return m_graph->vertex_data(m_id).deleted();
        }

        //! Return degree of vertex
        id_type degree() const
        {
            assert(!deleted());
            return m_graph->vertex_num_edge(m_id);
        }

        //! Increment vertex number of this iterator
        vertex_iterator& operator ++ ()
        {
            ++m_id;
            return skip_deleted();
        }

        //! Increment iterator to next valid vertex (skipping deleted)
        vertex_iterator & skip_deleted()
        {
            while (m_id < m_graph->total_vertex() && deleted())
                ++m_id;
            return *this;
        }

        //! Return constant data structure associated with vertex
        const VertexData& operator * () const
        {
            assert(!deleted());
            return m_graph->vertex_data(m_id).data;
        }

        //! Return mutable data structure associated with vertex
        VertexData& operator * ()
        {
            assert(!deleted());
            return m_graph->vertex_data(m_id).data;
        }

        //! Return constant data structure associated with vertex
        const VertexData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->vertex_data(m_id).data;
        }

        //! Return mutable data structure associated with vertex
        VertexData* operator -> ()
        {
            assert(!deleted());
            return &m_graph->vertex_data(m_id).data;
        }

        //! Return vertex_edge_iterator of a first edge
        vertex_edge_iterator edge_begin() const
        {
            return m_graph->vertex_edge_begin(m_id);
        }

        //! Return vertex_edge_iterator of a first edge
        vertex_edge_iterator edge_end() const
        {
            return m_graph->vertex_edge_end(m_id);
        }

        //! Return vertex_edge_iterator of a given edge
        vertex_edge_iterator edge(id_type ei) const
        {
            return m_graph->vertex_edge(m_id, ei);
        }

        //! Return vertex_edge_iterator of a first edge
        vertex_edge_iterator_range edge_list() const
        {
            return m_graph->vertex_edge_list(m_id);
        }

        //! Equality comparison to another iterator
        bool operator == (const vertex_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_id == a.m_id);
        }

        //! Inequality comparison to another iterator
        bool operator != (const vertex_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_id != a.m_id);
        }

        //! Less comparison to another iterator
        bool operator < (const vertex_iterator& a) const
        {
            if (m_graph != a.m_graph) return (m_graph < a.m_graph);
            return (m_id < a.m_id);
        }

        //! Formatted output: just contains the vertex id.
        friend std::ostream& operator << (std::ostream& os, const vertex_iterator& vi)
        {
            return os << "(" << vi.vertex_id() << ")";
        }
    };

    //! Return mutable iterator to first vertex of graph
    vertex_iterator vertex_begin()
    {
        return vertex_iterator(this, 0);
    }

    //! Return mutable iterator beyond last vertex of graph
    vertex_iterator vertex_end()
    {
        return vertex_iterator(this, total_vertex());
    }

public:
    //! Constant iterator for the vertex sequence of the graph.
    class const_vertex_iterator
    {
    private:
        //! Pointer to graph containing the vertex.
        const GraphBase* m_graph;

        //! Identifier of the vertex
        id_type m_id;

    public:
        //! Construct vertex iterator pointing to specific vertex
        const_vertex_iterator(const GraphBase* g, const id_type& v)
            : m_graph(g), m_id(v)
        {
            skip_deleted();
        }

        //! Return graph containing the vertex
        const GraphBase * graph() const
        {
            return m_graph;
        }

        //! Construct vertex iterator from mutable iterator
        const_vertex_iterator(const vertex_iterator& vi) // NOLINT
            : m_graph(vi.graph()), m_id(vi.vertex_id())
        { }

        //! Return id of vertex
        const id_type & vertex_id() const
        {
            return m_id;
        }

        //! Returns true if the vertex is deleted
        bool deleted() const
        {
            return m_graph->vertex_data(m_id).deleted();
        }

        //! Return degree of vertex
        id_type degree() const
        {
            assert(!deleted());
            return m_graph->vertex_num_edge(m_id);
        }

        //! Increment vertex number of this iterator
        const_vertex_iterator& operator ++ ()
        {
            ++m_id;
            return skip_deleted();
        }

        //! Increment iterator to next valid vertex (skipping deleted)
        const_vertex_iterator & skip_deleted()
        {
            while (m_id < m_graph->total_vertex() && deleted())
                ++m_id;
            return *this;
        }

        //! Return constant data structure associated with vertex
        const VertexData& operator * () const
        {
            assert(!deleted());
            return m_graph->vertex_data(m_id).data;
        }

        //! Return constant data structure associated with vertex
        const VertexData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->vertex_data(m_id).data;
        }

        //! Return vertex_edge_iterator of a first edge
        const_vertex_edge_iterator edge_begin() const
        {
            return m_graph->vertex_edge_begin(m_id);
        }

        //! Return vertex_edge_iterator of a first edge
        const_vertex_edge_iterator edge_end() const
        {
            return m_graph->vertex_edge_end(m_id);
        }

        //! Return vertex_edge_iterator of a first edge
        const_vertex_edge_iterator_range edge_list() const
        {
            return m_graph->vertex_edge_list(m_id);
        }

        //! Return const_vertex_edge_iterator of a given edge
        const_vertex_edge_iterator edge(id_type ei) const
        {
            return m_graph->vertex_edge(m_id, ei);
        }

        //! Equality comparison to another iterator
        bool operator == (const const_vertex_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_id == a.m_id);
        }

        //! Inequality comparison to another iterator
        bool operator != (const const_vertex_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_id != a.m_id);
        }

        //! Less comparison to another iterator
        bool operator < (const const_vertex_iterator& a) const
        {
            if (m_graph != a.m_graph) return (m_graph < a.m_graph);
            return (m_id < a.m_id);
        }

        //! Formatted output: just contains the vertex id.
        friend std::ostream& operator << (std::ostream& os, const const_vertex_iterator& vi)
        {
            return os << "(" << vi.vertex_id() << ")";
        }
    };

    //! Return constant iterator to first vertex of graph
    const_vertex_iterator vertex_begin() const
    {
        return const_vertex_iterator(this, 0);
    }

    //! Return constant iterator beyond last vertex of graph
    const_vertex_iterator vertex_end() const
    {
        return const_vertex_iterator(this, total_vertex());
    }

    //! Returns true if this graph contains the referenced edge
    bool contains(const const_vertex_iterator& vi) const
    {
        return (vi.graph() == this);
    }

public:
    //! Proxy for vertex_iterator for C++11 range for loop interface
    template <typename Iterator>
    class iterator_proxy
    {
    public:
        //! enclosed iterator
        Iterator m_iter;

        //! construct enclosed iterator
        iterator_proxy(const Iterator& iter) // NOLINT
            : m_iter(iter)
        { }

        //! comparison function during for loop
        bool operator != (const iterator_proxy& i) const
        {
            return m_iter != i.m_iter;
        }

        //! incrementation function during for loop
        iterator_proxy& operator ++ ()
        {
            ++m_iter;
            return *this;
        }

        //! return the iterator itself instead of the value inside.
        Iterator& operator * ()
        {
            return m_iter;
        }
    };

    //! proxy object for C++11 range for loop
    class vertex_iterator_range
    {
    public:
        //! reference to the graph
        GraphBase& m_graph;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<vertex_iterator>;

        //! construct proxy object
        explicit vertex_iterator_range(GraphBase& graph)
            : m_graph(graph)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.vertex_begin();
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.vertex_end();
        }
    };
    //! proxy object for C++11 range for loop
    class const_vertex_iterator_range
    {
    public:
        //! reference to the graph
        const GraphBase& m_graph;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<const_vertex_iterator>;

        //! construct proxy object
        explicit const_vertex_iterator_range(const GraphBase& graph)
            : m_graph(graph)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.vertex_begin();
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.vertex_end();
        }
    };

    //! return proxy object for C++11 range for loop
    vertex_iterator_range vertex_list()
    {
        return vertex_iterator_range(*this);
    }

    //! return proxy object for C++11 range for loop
    const_vertex_iterator_range vertex_list() const
    {
        return const_vertex_iterator_range(*this);
    }

public:
    //! Mutable iterator for the edge sequence of the graph.
    class edge_iterator
    {
    private:
        //! Pointer to graph containing the edge.
        GraphBase* m_graph;

        //! Identifier of the edge
        id_type m_id;

    public:
        //! Construct edge iterator pointing to specific edge
        edge_iterator(GraphBase* g, const id_type& e)
            : m_graph(g), m_id(e)
        {
            skip_deleted();
        }

        //! Construct edge iterator from mutable iterator
        edge_iterator(const vertex_edge_iterator& vei) // NOLINT
            : m_graph(vei.graph()), m_id(vei.edge_id())
        { }

        //! Return graph containing the vertex
        const GraphBase * graph() const
        {
            return m_graph;
        }

        //! Return id of edge
        const id_type & edge_id() const
        {
            return m_id;
        }

        //! Returns true if the edge is deleted
        bool deleted() const
        {
            return m_graph->edge_data(m_id).deleted();
        }

        //! Increment edge number of this iterator
        edge_iterator& operator ++ ()
        {
            ++m_id;
            return skip_deleted();
        }

        //! Increment iterator to next valid edge (skipping deleted)
        edge_iterator & skip_deleted()
        {
            while (m_id < m_graph->total_edge() && deleted())
                ++m_id;
            return *this;
        }

        //! Return constant data structure associated with edge
        const EdgeData& operator * () const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).data;
        }

        //! Return mutable data structure associated with edge
        EdgeData& operator * ()
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).data;
        }

        //! Return constant data structure associated with edge
        const EdgeData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->edge_data(m_id).data;
        }

        //! Return mutable data structure associated with edge
        EdgeData* operator -> ()
        {
            assert(!deleted());
            return &m_graph->edge_data(m_id).data;
        }

        //! Return id of the head side (target) of the edge
        const id_type & head_id() const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).head;
        }

        //! Return id of the tail side (source) of the edge
        const id_type & tail_id() const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).tail;
        }

        //! Return head side (target) of the edge
        vertex_iterator head() const
        {
            return m_graph->vertex(head_id());
        }

        //! Return tail side (source) of the edge
        vertex_iterator tail() const
        {
            return m_graph->vertex(tail_id());
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const id_type& v) const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).other_id(v);
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const const_vertex_iterator& vi) const
        {
            return other_id(vi.vertex_id());
        }

        //! Return the other side of the edge
        vertex_iterator other(const id_type& v) const
        {
            assert(v == head_id() || v == tail_id());
            return m_graph->vertex(other_id(v));
        }

        //! Return the other side of the edge
        vertex_iterator other(const const_vertex_iterator& vi) const
        {
            return other(vi.vertex_id());
        }

        //! Equality comparison to another iterator
        bool operator == (const edge_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_id == a.m_id);
        }

        //! Inequality comparison to another iterator
        bool operator != (const edge_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_id != a.m_id);
        }

        //! Less comparison to another iterator
        bool operator < (const edge_iterator& a) const
        {
            assert(m_graph == a.m_graph);
            return (m_id < a.m_id);
        }

        //! Formatted output: contains the edge id and both edges.
        friend std::ostream& operator << (std::ostream& os, const edge_iterator& ei)
        {
            if (ei.graph()->is_directed)
                return os << "(" << ei.edge_id() << "=(" << ei.head_id() << "," << ei.tail_id() << "))";
            else
                return os << "(" << ei.edge_id() << "={" << ei.head_id() << "," << ei.tail_id() << "})";
        }
    };

    //! Return mutable iterator to first edge of graph
    edge_iterator edge_begin()
    {
        return edge_iterator(this, 0);
    }

    //! Return mutable iterator beyond last edge of graph
    edge_iterator edge_end()
    {
        return edge_iterator(this, total_edge());
    }

public:
    //! Constant iterator for the edge sequence of the graph.
    class const_edge_iterator
    {
    private:
        //! Pointer to graph containing the edge.
        const GraphBase* m_graph;

        //! Identifier of the edge
        id_type m_id;

    public:
        //! Construct edge iterator pointing to specific edge
        const_edge_iterator(const GraphBase* g, const id_type& e)
            : m_graph(g), m_id(e)
        {
            skip_deleted();
        }

        //! Construct edge iterator from mutable iterator
        const_edge_iterator(const edge_iterator& ei) // NOLINT
            : m_graph(ei.graph()), m_id(ei.edge_id())
        { }

        //! Construct edge iterator from mutable iterator
        const_edge_iterator(const const_vertex_edge_iterator& vei) // NOLINT
            : m_graph(vei.graph()), m_id(vei.edge_id())
        { }

        //! Return graph containing the vertex
        const GraphBase * graph() const
        {
            return m_graph;
        }

        //! Return id of edge
        const id_type & edge_id() const
        {
            return m_id;
        }

        //! Returns true if the edge is deleted
        bool deleted() const
        {
            return m_graph->edge_data(m_id).deleted();
        }

        //! Increment edge number of this iterator
        const_edge_iterator& operator ++ ()
        {
            ++m_id;
            return skip_deleted();
        }

        //! Increment iterator to next valid edge (skipping deleted)
        const_edge_iterator & skip_deleted()
        {
            while (m_id < m_graph->total_edge() && deleted())
                ++m_id;
            return *this;
        }

        //! Return constant data structure associated with edge
        const EdgeData& operator * () const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).data;
        }

        //! Return constant data structure associated with edge
        const EdgeData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->edge_data(m_id).data;
        }

        //! Return id of the head side (target) of the edge
        const id_type & head_id() const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).head;
        }

        //! Return id of the tail side (source) of the edge
        const id_type & tail_id() const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).tail;
        }

        //! Return head side (target) of the edge
        const_vertex_iterator head() const
        {
            return m_graph->vertex(head_id());
        }

        //! Return tail side (source) of the edge
        const_vertex_iterator tail() const
        {
            return m_graph->vertex(tail_id());
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const id_type& v) const
        {
            assert(!deleted());
            return m_graph->edge_data(m_id).other_id(v);
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const const_vertex_iterator& vi) const
        {
            return other_id(vi.vertex_id());
        }

        //! Return the other side of the edge
        const_vertex_iterator other(const id_type& v) const
        {
            assert(v == head_id() || v == tail_id());
            return m_graph->vertex(other_id(v));
        }

        //! Return the other side of the edge
        const_vertex_iterator other(const const_vertex_iterator& vi) const
        {
            return other(vi.vertex_id());
        }

        //! Equality comparison to another iterator
        bool operator == (const const_edge_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_id == a.m_id);
        }

        //! Inequality comparison to another iterator
        bool operator != (const const_edge_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_id != a.m_id);
        }

        //! Less comparison to another iterator
        bool operator < (const const_edge_iterator& a) const
        {
            assert(m_graph == a.m_graph);
            return (m_id < a.m_id);
        }

        //! Formatted output: contains the edge id and both edges.
        friend std::ostream& operator << (std::ostream& os, const const_edge_iterator& ei)
        {
            if (ei.graph()->is_directed)
                return os << "(" << ei.edge_id() << "=(" << ei.head_id() << "," << ei.tail_id() << "))";
            else
                return os << "(" << ei.edge_id() << "={" << ei.head_id() << "," << ei.tail_id() << "})";
        }
    };

    //! Return constant iterator to first edge of graph
    const_edge_iterator edge_begin() const
    {
        return const_edge_iterator(this, 0);
    }

    //! Return constant iterator beyond last edge of graph
    const_edge_iterator edge_end() const
    {
        return const_edge_iterator(this, total_edge());
    }

    //! Returns true if this graph contains the referenced edge
    bool contains(const const_edge_iterator& ei) const
    {
        return (ei.graph() == this);
    }

public:
    //! proxy object for C++11 range for loop
    class edge_iterator_range
    {
    public:
        //! reference to the graph
        GraphBase& m_graph;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<edge_iterator>;

        //! construct proxy object
        explicit edge_iterator_range(GraphBase& graph)
            : m_graph(graph)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.edge_begin();
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.edge_end();
        }
    };

    //! proxy object for C++11 range for loop
    class const_edge_iterator_range
    {
    public:
        //! reference to the graph
        const GraphBase& m_graph;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<const_edge_iterator>;

        //! construct proxy object
        explicit const_edge_iterator_range(const GraphBase& graph)
            : m_graph(graph)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.edge_begin();
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.edge_end();
        }
    };

    //! return proxy object for C++11 range for loop
    edge_iterator_range edge_list()
    {
        return edge_iterator_range(*this);
    }

    //! return proxy object for C++11 range for loop
    const_edge_iterator_range edge_list() const
    {
        return const_edge_iterator_range(*this);
    }

public:
    //! Mutable iterator for the edge sequence of a vertex.
    class vertex_edge_iterator
    {
    private:
        //! Pointer to graph containing the edge.
        GraphBase* m_graph;

        //! Identifier of the vertex
        id_type m_vertex;

        //! Identifier of the edge
        id_type m_edge;

    public:
        //! Construct vertex_edge iterator pointing to specific edge of a vertex
        vertex_edge_iterator(GraphBase* g, const id_type& v, const id_type& e)
            : m_graph(g), m_vertex(v), m_edge(e)
        {
            assert(!g->vertex_deleted(v));
            skip_deleted();
        }

        //! Return graph containing the vertex
        GraphBase * graph() const
        {
            return m_graph;
        }

        //! Return id of vertex
        const id_type & vertex_id() const
        {
            return m_vertex;
        }

        //! Return id of edge relative to vertex
        const id_type & vertex_edge_id() const
        {
            return m_edge;
        }

        //! Return id of edge
        id_type edge_id() const
        {
            return m_graph->vertex_edge_id(m_vertex, m_edge);
        }

        //! Returns true if the vertex is deleted
        bool deleted() const
        {
            return m_graph->vertex_edge_data(m_vertex, m_edge).deleted();
        }

        //! Return degree of vertex
        id_type degree() const
        {
            assert(!deleted());
            return m_graph->vertex_num_edge(m_vertex);
        }

        //! Increment edge number of this iterator
        vertex_edge_iterator& operator ++ ()
        {
            ++m_edge;
            return skip_deleted();
        }

        //! Increment iterator to next valid edge (skipping deleted)
        vertex_edge_iterator & skip_deleted()
        {
            while (m_edge < m_graph->vertex_num_edge(m_vertex) && deleted())
                ++m_edge;
            return *this;
        }

        //! Return constant data structure associated with edge
        const EdgeData& operator * () const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return mutable data structure associated with edge
        EdgeData& operator * ()
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return constant data structure associated with edge
        const EdgeData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return mutable data structure associated with edge
        EdgeData* operator -> ()
        {
            assert(!deleted());
            return &m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return id of the head side (target) of the edge
        const id_type & head_id() const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).head;
        }

        //! Return id of the tail side (source) of the edge
        const id_type & tail_id() const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).tail;
        }

        //! Return head side (target) of the edge
        vertex_iterator head() const
        {
            return m_graph->vertex(head_id());
        }

        //! Return tail side (source) of the edge
        vertex_iterator tail() const
        {
            return m_graph->vertex(tail_id());
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const id_type& v) const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).other_id(v);
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const const_vertex_iterator& vi) const
        {
            return other_id(vi.vertex_id());
        }

        //! Return the other side of the edge
        vertex_iterator other(const id_type& v) const
        {
            assert(v == head_id() || v == tail_id());
            return m_graph->vertex(other_id(v));
        }

        //! Return the other side of the edge
        vertex_iterator other(const const_vertex_iterator& vi) const
        {
            return other(vi.vertex_id());
        }

        //! Equality comparison to another iterator
        bool operator == (const vertex_edge_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_vertex == a.m_vertex) && (m_edge == a.m_edge);
        }

        //! Inequality comparison to another iterator
        bool operator != (const vertex_edge_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_vertex != a.m_vertex) || (m_edge != a.m_edge);
        }

        //! Less comparison to another iterator
        bool operator < (const vertex_edge_iterator& a) const
        {
            if (m_graph != a.m_graph) return (m_graph < a.m_graph);
            if (m_vertex != a.m_vertex) return (m_vertex < a.m_vertex);
            return (m_edge < a.m_edge);
        }

        //! Cast automatically down to an edge_iterator
        operator edge_iterator () const
        {
            return edge_iterator(m_graph, edge_id());
        }

        //! Formatted output: contains the edge id and both edges.
        friend std::ostream& operator << (std::ostream& os, const vertex_edge_iterator& ei)
        {
            if (ei.graph()->is_directed)
                return os << "(" << ei.edge_id() << "=(" << ei.head_id() << "," << ei.tail_id() << "))";
            else
                return os << "(" << ei.edge_id() << "={" << ei.head_id() << "," << ei.tail_id() << "})";
        }
    };

    //! Return mutable iterator to first edge of a vertex
    vertex_edge_iterator vertex_edge_begin(const id_type& v)
    {
        return vertex_edge_iterator(this, v, 0);
    }

    //! Return mutable iterator beyond last edge of a vertex
    vertex_edge_iterator vertex_edge_end(const id_type& v)
    {
        return vertex_edge_iterator(this, v, vertex_num_edge(v));
    }

    //! Return mutable iterator to first edge of a vertex
    vertex_edge_iterator vertex_edge_begin(const const_vertex_iterator& vi)
    {
        return vertex_edge_begin(vi.vertex_id());
    }

    //! Return mutable iterator beyond last edge of a vertex
    vertex_edge_iterator vertex_edge_end(const const_vertex_iterator& vi)
    {
        return vertex_edge_end(vi.vertex_id());
    }

public:
    //! Constant iterator for the edge sequence of a vertex.
    class const_vertex_edge_iterator
    {
    private:
        //! Pointer to graph containing the edge.
        const GraphBase* m_graph;

        //! Identifier of the vertex
        id_type m_vertex;

        //! Identifier of the edge
        id_type m_edge;

    public:
        //! Construct vertex_edge iterator pointing to specific edge of a vertex
        const_vertex_edge_iterator(const GraphBase* g, const id_type& v, const id_type& ei)
            : m_graph(g), m_vertex(v), m_edge(ei)
        {
            assert(!g->vertex_deleted(v));
            skip_deleted();
        }

        //! Construct vertex_edge iterator from mutable iterator
        const_vertex_edge_iterator(const vertex_edge_iterator& vi) // NOLINT
            : m_graph(vi.graph()), m_vertex(vi.vertex_id()), m_edge(vi.vertex_edge_id())
        { }

        //! Return graph containing the vertex
        const GraphBase * graph() const
        {
            return m_graph;
        }

        //! Return id of vertex
        const id_type & vertex_id() const
        {
            return m_vertex;
        }

        //! Return id of edge
        id_type edge_id() const
        {
            return m_graph->vertex_edge_id(m_vertex, m_edge);
        }

        //! Returns true if the vertex is deleted
        bool deleted() const
        {
            return m_graph->vertex_edge_data(m_vertex, m_edge).deleted();
        }

        //! Return degree of vertex
        id_type degree() const
        {
            assert(!deleted());
            return m_graph->vertex_num_edge(m_vertex);
        }

        //! Increment edge number of this iterator
        const_vertex_edge_iterator& operator ++ ()
        {
            ++m_edge;
            return skip_deleted();
        }

        //! Increment iterator to next valid edge (skipping deleted)
        const_vertex_edge_iterator & skip_deleted()
        {
            while (m_edge < m_graph->vertex_num_edge(m_vertex) && deleted())
                ++m_edge;
            return *this;
        }

        //! Return constant data structure associated with edge
        const EdgeData& operator * () const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return constant data structure associated with edge
        const EdgeData* operator -> () const
        {
            assert(!deleted());
            return &m_graph->vertex_edge_data(m_vertex, m_edge).data;
        }

        //! Return id of the head side (target) of the edge
        const id_type & head_id() const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).head;
        }

        //! Return id of the tail side (source) of the edge
        const id_type & tail_id() const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).tail;
        }

        //! Return head side (target) of the edge
        const_vertex_iterator head() const
        {
            return m_graph->vertex(head_id());
        }

        //! Return tail side (source) of the edge
        const_vertex_iterator tail() const
        {
            return m_graph->vertex(tail_id());
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const id_type& v) const
        {
            assert(!deleted());
            return m_graph->vertex_edge_data(m_vertex, m_edge).other_id(v);
        }

        //! Return id of the other side of the edge
        const id_type & other_id(const const_vertex_iterator& vi) const
        {
            return other_id(vi.vertex_id());
        }

        //! Return the other side of the edge
        const_vertex_iterator other(const id_type& v) const
        {
            assert(v == head_id() || v == tail_id());
            return m_graph->vertex(other_id(v));
        }

        //! Return the other side of the edge
        const_vertex_iterator other(const const_vertex_iterator& vi) const
        {
            return other(vi.vertex_id());
        }

        //! Equality comparison to another iterator
        bool operator == (const const_vertex_edge_iterator& a) const
        {
            return (m_graph == a.m_graph) && (m_vertex == a.m_vertex) && (m_edge == a.m_edge);
        }

        //! Inequality comparison to another iterator
        bool operator != (const const_vertex_edge_iterator& a) const
        {
            return (m_graph != a.m_graph) || (m_vertex != a.m_vertex) || (m_edge != a.m_edge);
        }

        //! Cast automatically down to a const_edge_iterator
        operator const_edge_iterator () const
        {
            return const_edge_iterator(m_graph, edge_id());
        }

        //! Formatted output: contains the edge id and both edges.
        friend std::ostream& operator << (std::ostream& os, const const_vertex_edge_iterator& e)
        {
            if (e.graph()->is_directed)
                return os << "(" << e.edge_id() << "=(" << e.head_id() << "," << e.tail_id() << "))";
            else
                return os << "(" << e.edge_id() << "={" << e.head_id() << "," << e.tail_id() << "})";
        }
    };

    //! Return mutable iterator to first edge of a vertex
    const_vertex_edge_iterator vertex_edge_begin(const id_type& v) const
    {
        return const_vertex_edge_iterator(this, v, 0);
    }

    //! Return mutable iterator beyond last edge of a vertex
    const_vertex_edge_iterator vertex_edge_end(const id_type& v) const
    {
        return const_vertex_edge_iterator(this, v, vertex_num_edge(v));
    }

    //! Return mutable iterator to first edge of a vertex
    const_vertex_edge_iterator vertex_edge_begin(const const_vertex_iterator& vi) const
    {
        return vertex_edge_begin(vi.vertex_id());
    }

    //! Return mutable iterator beyond last edge of a vertex
    const_vertex_edge_iterator vertex_edge_end(const const_vertex_iterator& vi) const
    {
        return vertex_edge_end(vi.vertex_id());
    }

    //! Returns true if this graph contains the referenced edge
    bool contains(const const_vertex_edge_iterator& vei) const
    {
        return (vei.graph() == this);
    }

public:
    //! proxy object for C++11 range for loop
    class vertex_edge_iterator_range
    {
    public:
        //! reference to the graph
        GraphBase& m_graph;

        //! number of vertex to iterate
        id_type m_v;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<vertex_edge_iterator>;

        //! construct proxy object
        vertex_edge_iterator_range(GraphBase& graph, const id_type& v)
            : m_graph(graph), m_v(v)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.vertex_edge_begin(m_v);
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.vertex_edge_end(m_v);
        }
    };

    //! proxy object for C++11 range for loop
    class const_vertex_edge_iterator_range
    {
    public:
        //! reference to the graph
        const GraphBase& m_graph;

        //! number of vertex to iterate
        id_type m_v;

        //! typedef of iterator proxy
        using iterator_proxy_type = iterator_proxy<const_vertex_edge_iterator>;

        //! construct proxy object
        const_vertex_edge_iterator_range(const GraphBase& graph, const id_type& v)
            : m_graph(graph), m_v(v)
        { }

        //! return the proxy iterator for begin()
        iterator_proxy_type begin()
        {
            return m_graph.vertex_edge_begin(m_v);
        }

        //! return the proxy iterator for end()
        iterator_proxy_type end()
        {
            return m_graph.vertex_edge_end(m_v);
        }
    };

    //! return proxy object for C++11 range for loop
    vertex_edge_iterator_range vertex_edge_list(const id_type& v)
    {
        return vertex_edge_iterator_range(*this, v);
    }

    //! return proxy object for C++11 range for loop
    const_vertex_edge_iterator_range vertex_edge_list(const id_type& v) const
    {
        return const_vertex_edge_iterator_range(*this, v);
    }

public:
    //! Returns true if the vertex id is deleted
    bool vertex_deleted(const id_type& v) const
    {
        assert(v < m_vertex.size());
        return m_vertex[v].deleted();
    }

    //! Returns the vertex object v
    vertex_iterator vertex(const id_type& v)
    {
        assert(v < m_vertex.size());
        assert(!m_vertex[v].deleted());
        return vertex_iterator(this, v);
    }

    //! Returns the vertex object v
    const_vertex_iterator vertex(const id_type& v) const
    {
        assert(v < m_vertex.size());
        assert(!m_vertex[v].deleted());
        return const_vertex_iterator(this, v);
    }

    //! Returns true if the edge id is deleted
    bool edge_deleted(const id_type& e) const
    {
        assert(e < m_edge.size());
        return m_edge[e].deleted();
    }

    //! Returns the edge object e
    edge_iterator edge(const id_type& e)
    {
        assert(e < m_edge.size());
        assert(!m_edge[e].deleted());
        return edge_iterator(this, e);
    }

    //! Returns the edge object e
    const_edge_iterator edge(const id_type& e) const
    {
        assert(e < m_edge.size());
        assert(!m_edge[e].deleted());
        return const_edge_iterator(this, e);
    }

    //! Return the e-th outgoing edge from vertex v
    vertex_edge_iterator vertex_edge(const id_type& v, const id_type& e)
    {
        assert(v < m_vertex.size());
        assert(e < m_vertex[v].edgelist.size());
        assert(m_vertex[v].edgelist[e] < m_edge.size());
        assert(!m_edge[m_vertex[v].edgelist[e]].deleted());
        return vertex_edge_iterator(this, v, e);
    }

    //! Return the e-th outgoing edge from vertex v
    const_vertex_edge_iterator vertex_edge(const id_type& v, const id_type& e) const
    {
        assert(v < m_vertex.size());
        assert(e < m_vertex[v].edgelist.size());
        assert(m_vertex[v].edgelist[e] < m_edge.size());
        assert(!m_edge[m_vertex[v].edgelist[e]].deleted());
        return const_vertex_edge_iterator(this, v, e);
    }

    //! Return the e-th outgoing edge from vertex v
    vertex_edge_iterator vertex_edge(const const_vertex_iterator& vi, const id_type& e)
    {
        return vertex_edge(vi.vertex_id(), e);
    }

    //! Return the e-th outgoing edge from vertex v
    const_vertex_edge_iterator vertex_edge(const const_vertex_iterator& vi, const id_type& e) const
    {
        return vertex_edge(vi.vertex_id(), e);
    }

public:
    //! Add vertex to graph
    vertex_iterator add_vertex(const VertexData& v = VertexData())
    {
        m_vertex.push_back(Vertex(v));
        return vertex_iterator(this, total_vertex() - 1);
    }

    //! Add edge (tail -> head) (and possibly head -> tail) to graph.
    edge_iterator add_edge(const id_type& tail, const id_type& head, const EdgeData& e = EdgeData())
    {
        assert(tail < m_vertex.size());
        assert(head < m_vertex.size());

        // check graph invariants: no loops?
        assert(allow_loops || tail != head);
        // no parallel edges?
        assert(allow_parallel || find_edge(tail, head) == edge_end());
        // no reverse edge in undirected graphs?
        assert(is_directed || allow_parallel || find_edge(head, tail) == edge_end());

        // check if vertexes are deleted
        assert(!vertex_deleted(tail));
        assert(!vertex_deleted(head));

        // add forward edge
        m_vertex[tail].edgelist.push_back(total_edge());

        if (is_undirected) // add backward edge
            m_vertex[head].edgelist.push_back(total_edge());

        // add edge data item
        m_edge.push_back(Edge(tail, head, e));

        return edge_iterator(this, total_edge() - 1);
    }

    //! Add edge (tail -> head) (and possibly head -> tail) to graph.
    edge_iterator add_edge(const const_vertex_iterator& tail,
                           const const_vertex_iterator& head,
                           const EdgeData& e = EdgeData())
    {
        assert(contains(tail) && contains(head));
        return add_edge(tail.vertex_id(), head.vertex_id(), e);
    }

public:
    //! Copy vertices and edges from another instance with the same data.
    template <bool Directed, bool AllowParallel, bool AllowLoops, bool AllowDeletes>
    static GraphBase
    copy_from(const GraphBase<VertexData, EdgeData, Directed, AllowParallel,
                              AllowLoops, AllowDeletes>& g)
    {
        typedef GraphBase<VertexData, EdgeData,
                          Directed, AllowParallel,
                          AllowLoops, AllowDeletes> other_type;

        std::vector<id_type> vmap(g.total_vertex());

        GraphBase out;

        for (typename other_type::const_vertex_iterator vi = g.vertex_begin();
             vi != g.vertex_end(); ++vi)
        {
            vertex_iterator nv = out.add_vertex(*vi);
            vmap[vi.vertex_id()] = nv.vertex_id();
        }

        for (typename other_type::const_edge_iterator ei = g.edge_begin();
             ei != g.edge_end(); ++ei)
        {
            out.add_edge(vmap[ei.tail_id()], vmap[ei.head_id()], *ei);
        }

        return out;
    }

public:
    //! Search for edge (tail -> head) by iterating over neighbors of tail.
    edge_iterator find_edge(const id_type& tail, const id_type& head, id_type num = 0)
    {
        assert(tail < m_vertex.size());
        assert(head < m_vertex.size());

        // check if vertexes are deleted
        assert(!vertex_deleted(tail));
        assert(!vertex_deleted(head));

        Vertex& v = m_vertex[tail];

        for (size_t ei = 0; ei < v.edgelist.size(); ++ei)
        {
            const Edge& e = m_edge[v.edgelist[ei]];

            if (e.deleted()) continue;

            if (is_directed)
            {
                assert(e.tail == tail);

                // if directed: match only edges tail -> head

                if (e.tail == tail && e.head == head)
                {
                    if (num == 0)
                        return edge_iterator(this, v.edgelist[ei]);

                    --num; // find next match
                }
            }
            else
            {
                assert(e.tail == tail || e.head == tail);

                // if undirected: match edges tail -> head or head -> tail.

                if ((e.tail == tail && e.head == head) ||
                    (e.tail == head && e.head == tail))
                {
                    if (num == 0)
                        return edge_iterator(this, v.edgelist[ei]);

                    --num; // find next match
                }
            }
        }
        return edge_end();
    }

    //! Search for edge (tail -> head) by iterating over neighbors of tail.
    edge_iterator find_edge(const const_vertex_iterator& tail,
                            const const_vertex_iterator& head, id_type num = 0)
    {
        assert(contains(tail) && contains(head));
        return find_edge(tail.vertex_id(), head.vertex_id(), num);
    }

    //! Search for edge (tail -> head) by iterating over neighbors of tail.
    const_edge_iterator find_edge(const id_type& tail, const id_type& head, id_type num = 0) const
    {
        assert(tail < m_vertex.size());
        assert(head < m_vertex.size());

        // check if vertexes are deleted
        assert(!vertex_deleted(tail));
        assert(!vertex_deleted(head));

        const Vertex& v = m_vertex[tail];

        for (size_t ei = 0; ei < v.edgelist.size(); ++ei)
        {
            const Edge& e = m_edge[v.edgelist[ei]];

            if (e.deleted()) continue;

            if (is_directed)
            {
                assert(e.tail == tail);

                // if directed: match only edges tail -> head

                if (e.tail == tail && e.head == head)
                {
                    if (num == 0)
                        return const_edge_iterator(this, v.edgelist[ei]);

                    --num; // find next match
                }
            }
            else
            {
                assert(e.tail == tail || e.head == tail);

                // if undirected: match edges tail -> head or head -> tail.

                if ((e.tail == tail && e.head == head) ||
                    (e.tail == head && e.head == tail))
                {
                    if (num == 0)
                        return const_edge_iterator(this, v.edgelist[ei]);

                    --num; // find next match
                }
            }
        }
        return edge_end();
    }

    //! Search for edge (tail -> head) by iterating over neighbors of tail.
    const_edge_iterator find_edge(const const_vertex_iterator& tail,
                                  const const_vertex_iterator& head,
                                  id_type num = 0) const
    {
        assert(contains(tail) && contains(head));
        return find_edge(tail.vertex_id(), head.vertex_id(), num);
    }

    //! Count number of parallel edges (tail -> head).
    id_type count_edges(const id_type& tail, const id_type& head) const
    {
        assert(tail < m_vertex.size());
        assert(head < m_vertex.size());

        // check if vertexes are deleted
        assert(!vertex_deleted(tail));
        assert(!vertex_deleted(head));

        const Vertex& v = m_vertex[tail];
        id_type num = 0;

        for (size_t ei = 0; ei < v.edgelist.size(); ++ei)
        {
            const Edge& e = m_edge[v.edgelist[ei]];

            if (e.deleted()) continue;

            if (is_directed)
            {
                assert(e.tail == tail);

                // if directed: match only edges tail -> head

                if (e.tail == tail && e.head == head)
                {
                    ++num; // matched
                }
            }
            else
            {
                assert(e.tail == tail || e.head == tail);

                // if undirected: match edges tail -> head or head -> tail.

                if ((e.tail == tail && e.head == head) ||
                    (e.tail == head && e.head == tail))
                {
                    ++num; // matched
                }
            }
        }
        return num;
    }

    //! Count number of parallel edges (tail -> head).
    id_type count_edges(const const_vertex_iterator& tail,
                        const const_vertex_iterator& head) const
    {
        assert(contains(tail) && contains(head));
        return count_edges(tail.vertex_id(), head.vertex_id());
    }

    //! Count number of parallel edges to e
    id_type count_edges(const const_edge_iterator& ei) const
    {
        assert(contains(ei));
        return count_edges(ei.tail_id(), ei.head_id());
    }

    //! Count number of parallel edges to e
    id_type count_edges(id_type e) const
    {
        return count_edges(edge(e));
    }

public:
    //! Erase an edge and decrement the ids of all edges > this one. This will
    //! invalidate all edge ids!
    bool delete_edge_reorder(const id_type& e)
    {
        assert(allow_deletes);
        assert(e < m_edge.size());

        // check if edge is deleted
        assert(!edge_deleted(e));

        // shorten edge list
        m_edge.erase(m_edge.begin() + e);

        unsigned int delcount = 0;

        // erase edge from all vertex.edgelists
        for (size_t vi = 0; vi < m_vertex.size(); ++vi)
        {
            Vertex& v = m_vertex[vi];

            if (v.deleted()) continue;

            for (size_t i = 0; i < v.edgelist.size(); ++i)
            {
                if (v.edgelist[i] < e) {       // skip edgelist entry
                }
                else if (v.edgelist[i] == e) { // erase deleted edge
                    v.edgelist.erase(v.edgelist.begin() + i);
                    --i;
                    ++delcount;
                }
                else { // decrement edgelist entry
                    v.edgelist[i]--;
                }
            }
        }

        assert(delcount == (is_directed ? 1 : 2));

        return true;
    }

    //! Erase an edge and decrement the ids of all edges > this one. This will
    //! invalidate all edge ids!
    bool delete_edge_reorder(const const_edge_iterator& ei)
    {
        assert(contains(ei));
        return delete_edge_reorder(ei.edge_id());
    }

public:
    //! Erase a vertex by marking it as deleted. This will keep vertex and edge
    //! ids, but also keeps entries in the lists.
    bool delete_vertex(const id_type& v)
    {
        assert(allow_deletes);
        assert(v < m_vertex.size());

        // check if vertex is deleted
        assert(!vertex_deleted(v));

        // erase all edges connected with vertex v
        for (edge_iterator ei = edge_begin(); ei != edge_end(); ++ei)
        {
            if (ei.tail_id() == v || ei.head_id() == v)
                delete_edge(ei);
        }

        assert(m_vertex[v].edgelist.size() == 0);

        // mark vertex as deleted
        m_vertex[v].edgelist.resize(1);
        m_vertex[v].edgelist[0] = ID_INVALID;
        ++m_vertex_deleted;

        return true;
    }

    //! Erase a vertex by marking it as deleted. This will keep vertex and edge
    //! ids, but also keeps entries in the lists.
    bool delete_vertex(const const_vertex_iterator& vi)
    {
        assert(contains(vi));
        return delete_vertex(vi.vertex_id());
    }

public:
    //! Erase an edge by marking it as deleted. This will keep edge ids, but
    //! also keeps an entry in the edge list.
    bool delete_edge(const id_type& e)
    {
        assert(allow_deletes);
        assert(e < m_edge.size());

        // check if edge is deleted
        assert(!edge_deleted(e));

        // mark edge as deleted
        m_edge[e].tail = m_edge[e].head = ID_INVALID;
        ++m_edge_deleted;

        unsigned int delcount = 0;

        // erase edge from all vertex.edgelists
        for (size_t vi = 0; vi < m_vertex.size(); ++vi)
        {
            Vertex& v = m_vertex[vi];
            if (v.deleted()) continue;

            for (std::vector<id_type>::iterator it = v.edgelist.begin();
                 it != v.edgelist.end(); )
            {
                if (*it == e) { // erase deleted edge
                    it = v.edgelist.erase(it);
                    ++delcount;
                }
                else
                    ++it;
            }
        }

        assert(delcount == (is_directed ? 1 : 2));

        return true;
    }

    //! Erase an edge by marking it as deleted. This will keep edge ids, but
    //! also keeps an entry in the edge list.
    bool delete_edge(const const_edge_iterator& ei)
    {
        assert(contains(ei));
        return delete_edge(ei.edge_id());
    }

    //! Erase an edge by marking it as deleted. This will keep edge ids, but
    //! also keeps an entry in the edge list.
    bool delete_edge(const const_vertex_edge_iterator& ei)
    {
        assert(contains(ei));
        return delete_edge(ei.edge_id());
    }

    //! Add a deleted dummy edge entry
    void add_deleted_edge(const EdgeData& e = EdgeData())
    {
        assert(allow_deletes);

        // mark edge as deleted
        m_edge.push_back(Edge(ID_INVALID, ID_INVALID, e));
        ++m_edge_deleted;
    }

public:
    //! Contract two vertices, if there is an edge it is deleted. This will
    //! keep edge ids, but also keeps an entry in the edge list.
    bool contract(id_type v1, id_type v2)
    {
        assert(allow_deletes);
        assert(allow_parallel);
        assert(!is_directed);

        // move edges from v2 -> v1
        for (id_type e = 0; e < total_edge(); ++e)
        {
            if (edge_deleted(e)) continue;

            if (m_edge[e].head == v2)
            {
                if (m_edge[e].tail == v1 && !allow_loops) {
                    delete_edge(e);
                }
                else {
                    m_edge[e].head = v1;
                    m_vertex[v2].remove_edge(e);
                    m_vertex[v1].edgelist.push_back(e);
                }
            }
            else if (m_edge[e].tail == v2)
            {
                if (m_edge[e].head == v1 && !allow_loops) {
                    delete_edge(e);
                }
                else {
                    m_edge[e].tail = v1;
                    m_vertex[v2].remove_edge(e);
                    m_vertex[v1].edgelist.push_back(e);
                }
            }
        }

        delete_vertex(v2);

        return true;
    }

    //! contract an edge by marking it as deleted. This will keep edge ids, but
    //! also keeps an entry in the edge list.
    bool contract(const edge_iterator& ei)
    {
        assert(contains(ei));
        return contract(ei.tail_id(), ei.head_id());
    }

public:
    //! Output graph in graphviz format for debugging without attributes.
    friend std::ostream& operator << (std::ostream& os, const GraphBase& g)
    {
        os << (g.is_directed ? "digraph" : "graph") << " G {\n";
        for (const_vertex_iterator v = g.vertex_begin(); v != g.vertex_end(); ++v)
        {
            os << "  " << v.vertex_id() << ';' << std::endl;
        }
        for (const_edge_iterator e = g.edge_begin(); e != g.edge_end(); ++e)
        {
            os << "  " << e.head_id()
               << (g.is_directed ? " -> " : " -- ")
               << e.tail_id() << ';' << std::endl;
        }
        os << '}';

        return os;
    }

public:
    //! Clone graph without deleted objects and reordered by vertex degree
    template <typename CloneType = GraphBase>
    CloneType clone_reorder_degree() const
    {
        // get vertex degrees
        using vdegpair = std::pair<id_type, unsigned int>;
        std::vector<vdegpair> vdeg;
        vdeg.reserve(num_vertex());

        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            vdeg.push_back(std::make_pair(vi.vertex_id(), vi.degree()));
        }

        std::sort(vdeg.begin(), vdeg.end(),
                  [](const vdegpair& a, const vdegpair& b) {
                      if (a.second == b.second) return a.first < b.first;
                      return a.second > b.second;
                  });

        // create renumbered graph and vertex id map
        CloneType h;

        std::vector<id_type> vmap(total_vertex(), ID_INVALID);

        for (unsigned int i = 0; i < vdeg.size(); ++i)
        {
            vmap[vdeg[i].first] = i;

            h.add_vertex(vertex_data(vdeg[i].first).data);
        }

        for (const_edge_iterator ei = edge_begin(); ei != edge_end(); ++ei)
        {
            h.add_edge(vmap[ei.tail_id()], vmap[ei.head_id()], *ei);
        }

        return h;
    }

    //! Clone graph without deleted objects and reordered by vertex degree
    template <typename CloneType = GraphBase>
    CloneType clone_reorder_degree(std::map<id_type, id_type>& _vmap,
                                   std::map<id_type, id_type>& _emap) const
    {
        // get vertex degrees
        using vdegpair = std::pair<id_type, unsigned int>;
        std::vector<vdegpair> vdeg;
        vdeg.reserve(num_vertex());

        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            vdeg.push_back(std::make_pair(vi.vertex_id(), vi.degree()));
        }

        std::sort(vdeg.begin(), vdeg.end(),
                  [](const vdegpair& a, const vdegpair& b) {
                      if (a.second == b.second) return a.first < b.first;
                      return b.second < a.second;
                  });

        // create renumbered graph and vertex id map
        CloneType h;

        std::vector<id_type> vmap(total_vertex(), ID_INVALID);

        for (unsigned int i = 0; i < vdeg.size(); ++i)
        {
            vmap[vdeg[i].first] = i;
            _vmap[vdeg[i].first] = i;

            h.add_vertex(vertex_data(vdeg[i].first).data);
        }

        for (const_edge_iterator ei = edge_begin(); ei != edge_end(); ++ei)
        {
            typename CloneType::edge_iterator en =
                h.add_edge(vmap[ei.tail_id()], vmap[ei.head_id()], *ei);

            _emap[ei.edge_id()] = en.edge_id();
        }

        return h;
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    // Node degree functions

    //! Calculate regularity of the graph, or -1 if not regular.
    int get_regularity() const
    {
        if (num_vertex() == 0) return 0;

        const_vertex_iterator vi = vertex_begin();
        int reg = vi.degree();

        while (vi != vertex_end())
        {
            if (static_cast<int>(vi.degree()) != reg)
                return -1;
            ++vi;
        }

        return reg;
    }

    //! Get the minimum degree of a node in the graph
    unsigned int get_min_degree() const
    {
        if (num_vertex() == 0) return 0;

        const_vertex_iterator vi = vertex_begin();
        unsigned int mindeg = vi.degree();

        while (vi != vertex_end())
        {
            if (vi.degree() < mindeg) {
                mindeg = vi.degree();
            }
            ++vi;
        }

        return mindeg;
    }

    //! Get the maximum degree of a node in the graph
    unsigned int get_max_degree() const
    {
        if (num_vertex() == 0) return 0;

        const_vertex_iterator vi = vertex_begin();
        unsigned int maxdeg = vi.degree();

        while (vi != vertex_end())
        {
            if (vi.degree() > maxdeg) {
                maxdeg = vi.degree();
            }
            ++vi;
        }

        return maxdeg;
    }

    //! Get the average degree of a node in the graph
    double get_avg_degree() const
    {
        if (num_vertex() == 0) return 0;

        const_vertex_iterator vi = vertex_begin();
        unsigned int sumdeg = 0;

        while (vi != vertex_end())
        {
            sumdeg += vi.degree();
            ++vi;
        }

        return sumdeg / static_cast<double>(num_vertex());
    }

    //! Count vertices with given degree
    size_t count_degree(size_t deg) const
    {
        size_t count = 0;
        for (const_vertex_iterator vi = vertex_begin();
             vi != vertex_end(); ++vi)
        {
            if (vi.degree() == deg) ++count;
        }
        return count;
    }

    //! Return the degree sequence of the graph
    std::string get_degree_sequence() const
    {
        if (num_vertex() == 0) return "";

        std::vector<id_type> deg;
        deg.reserve(num_vertex());

        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            deg.push_back(vi.degree());
        }

        std::sort(deg.begin(), deg.end(), std::greater<id_type>());

        std::ostringstream oss;
        for (size_t i = 0; i < deg.size(); ++i)
        {
            if (i != 0) oss << ',';
            oss << deg[i];
        }
        return oss.str();
    }

    //! Return any vertex of the given degree or vertex_end() if none exists.
    vertex_iterator find_vertex_degree(unsigned int deg)
    {
        for (vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            if (vi.degree() == deg) return vi;
        }

        return vertex_end();
    }

    //! Return any vertex of the given degree or vertex_end() if none exists.
    const_vertex_iterator find_vertex_degree(unsigned int deg) const
    {
        for (const_vertex_iterator vi = vertex_begin();
             vi != vertex_end(); ++vi)
        {
            if (vi.degree() == deg) return vi;
        }

        return vertex_end();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Breadth-First Search

    //! Graph filter functional class to ignore specific vertices and edges
    class DefaultFilter
    {
    public:
        //! returns true to ignore vertices
        static bool filter_vertex(const const_vertex_iterator&)
        {
            return false;
        }
        //! returns true to ignore edges
        static bool filter_edge(const const_edge_iterator&)
        {
            return false;
        }
    };

    //! Return number of components in undirected graph via BFS
    template <typename GraphFilter = DefaultFilter>
    id_type get_num_components_undirected(GraphFilter gf = DefaultFilter()) const
    {
        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // number of components in graph
        id_type num_components = 0;

        // fill pred with self-predecessors
        for (size_t i = 0; i < pred.size(); ++i)
            pred[i] = i;

        // BFS queue
        std::queue<id_type> queue;

        // iterate over all vertices, try each as root
        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            if (gf.filter_vertex(vi)) continue;

            id_type r = vi.vertex_id();

            if (pred[r] != r) continue;

            ++num_components;

            // initialize queue with node r
            queue.push(r);

            // breath first search
            while (!queue.empty())
            {
                id_type v = queue.front();
                queue.pop();

                if (gf.filter_vertex(vertex(v))) continue;

                for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                     ei != vertex_edge_end(v); ++ei)
                {
                    if (gf.filter_edge(ei)) continue;

                    id_type w = ei.other_id(v);

                    if (pred[w] != w || w == r) // vertex already seen
                        continue;

                    queue.push(w);
                    pred[w] = v;
                }
            }
        }

        return num_components;
    }

    //! Return number of vertices in the components of an undirected graph via
    //! BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type>
    get_component_sizes_undirected(GraphFilter gf = DefaultFilter()) const
    {
        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // number of components in graph
        std::vector<id_type> num_vertices;

        // fill pred with self-predecessors
        for (size_t i = 0; i < pred.size(); ++i)
            pred[i] = i;

        // BFS queue
        std::queue<id_type> queue;

        // iterate over all vertices, try each as root
        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            if (gf.filter_vertex(vi)) continue;

            id_type r = vi.vertex_id();

            if (pred[r] != r) continue;

            num_vertices.push_back(0);

            // initialize queue with node r
            queue.push(r);

            // breath first search
            while (!queue.empty())
            {
                id_type v = queue.front();
                queue.pop();

                if (gf.filter_vertex(vertex(v))) continue;

                num_vertices.back()++;

                for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                     ei != vertex_edge_end(v); ++ei)
                {
                    if (gf.filter_edge(ei)) continue;

                    id_type w = ei.other_id(v);

                    if (pred[w] != w || w == r) // vertex already seen
                        continue;

                    queue.push(w);
                    pred[w] = v;
                }
            }
        }

        return num_vertices;
    }

    //! Return vertices mapped to its component number 0,1,2,...
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type>
    get_component_id_undirected(GraphFilter gf = DefaultFilter()) const
    {
        // component number of vertex
        std::vector<id_type> comp(total_vertex(), ID_INVALID);

        unsigned int c = 0;

        // BFS queue
        std::queue<id_type> queue;

        // iterate over all vertices, try each as root
        for (const_vertex_iterator vi = vertex_begin(); vi != vertex_end(); ++vi)
        {
            if (gf.filter_vertex(vi)) continue;

            id_type r = vi.vertex_id();
            if (comp[r] != ID_INVALID) continue;
            comp[r] = c;

            // initialize queue with node r
            queue.push(r);

            // breath first search
            while (!queue.empty())
            {
                id_type v = queue.front();
                queue.pop();

                if (gf.filter_vertex(vertex(v))) continue;

                for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                     ei != vertex_edge_end(v); ++ei)
                {
                    if (gf.filter_edge(ei)) continue;

                    id_type w = ei.other_id(v);

                    if (comp[w] != ID_INVALID) // vertex already seen
                        continue;

                    queue.push(w);
                    comp[w] = c;
                }
            }

            c++;
        }

        return comp;
    }

    //! Return all vertexs connect to the root r in undirected graph via BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type>
    get_connected_undirected(id_type r, GraphFilter gf = DefaultFilter()) const
    {
        std::vector<id_type> vlist;

        // test if root vertex is in graph
        if (gf.filter_vertex(vertex(r))) return vlist;

        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // fill pred with self-predecessors
        for (size_t i = 0; i < pred.size(); ++i)
            pred[i] = i;

        // BFS queue
        std::queue<id_type> queue;

        // initialize queue with node vr
        queue.push(r);
        vlist.push_back(r);

        // breath first search
        while (!queue.empty())
        {
            id_type v = queue.front();
            queue.pop();

            if (gf.filter_vertex(vertex(v))) continue;

            for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                 ei != vertex_edge_end(v); ++ei)
            {
                if (gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v);

                if (pred[w] != w || w == r) // vertex already seen
                    continue;

                queue.push(w);
                vlist.push_back(w);
                pred[w] = v;
            }
        }

        return vlist;
    }

    //! Return all vertexs connect to the root vr in undirected graph via BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type>
    get_connected_undirected(const_vertex_iterator vi,
                             GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(vi));
        return get_connected_undirected(vi.vertex_id(), gf);
    }

    //! Tarjan's DFS Algorithm for finding SCCs
    template <typename GraphFilter = DefaultFilter>
    class AlgStrongConnectedComponents
    {
    public:
        // number of components in graph
        id_type num_components;

    protected:
        //! Processed graph
        GraphBase m_g;

        //! Graph filter functional
        GraphFilter m_gf;

        //! Running depth first search index
        id_type m_dfs;

        //! Visit index of nodes
        std::vector<id_type> m_index, m_lowlink;

        //! DFS stack path
        std::vector<id_type> m_stack;

        //! Check for vertex in stack
        bool stack_contains(const id_type& v)
        {
            for (size_t i = 0; i < m_stack.size(); ++i) {
                if (m_stack[i] == v) return true;
            }
            return false;
        }

        //! Depth First Discovery
        void strongconnect(const id_type& v)
        {
            // set the depth index for v to the smallest unused index
            m_index[v] = m_lowlink[v] = m_dfs++;
            m_stack.push_back(v);

            // visit all neighbors of v
            for (const_vertex_edge_iterator ei = m_g.vertex_edge_begin(v);
                 ei != m_g.vertex_edge_end(v); ++ei)
            {
                if (m_gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v);
                if (m_gf.filter_vertex(m_g.vertex(w))) continue;

                // visit unseen neighbors via recursive call
                if (m_index[w] == ID_INVALID)
                {
                    strongconnect(w);
                    m_lowlink[v] = std::min(m_lowlink[v], m_lowlink[w]);
                }
                // if neighbor is in dfs path -> in same SCC
                else if (stack_contains(w))
                {
                    m_lowlink[v] = std::min(m_lowlink[v], m_lowlink[w]);
                }
            }

            // if v is a root node, pop the stack and generate an SCC
            if (m_lowlink[v] == m_index[v])
            {
                // all items on stack form a SCC
                // std::cout << "// SCC: " << m_stack << "\n";
                m_stack.clear();
                ++num_components;
            }
        }

    public:
        AlgStrongConnectedComponents(const GraphBase& g, GraphFilter gf)
            : num_components(0),
              m_g(g), m_gf(gf),
              m_dfs(0),
              m_index(g.total_vertex(), ID_INVALID),
              m_lowlink(g.total_vertex(), ID_INVALID)
        {
            // start DFS at each unseen node
            for (const_vertex_iterator vi = g.vertex_begin();
                 vi != g.vertex_end(); ++vi)
            {
                if (m_gf.filter_vertex(vi)) continue;

                if (m_index[vi.vertex_id()] == ID_INVALID)
                    strongconnect(vi.vertex_id());
            }
        }
    };

    //! Return number of strongly connected components in directed graph via DFS
    template <typename GraphFilter = DefaultFilter>
    id_type get_num_components_directed(GraphFilter gf = DefaultFilter()) const
    {
        return AlgStrongConnectedComponents<GraphFilter>(*this, gf).num_components;
    }

    //! Returns number of components or strongly connected components (undirected)
    template <typename GraphFilter = DefaultFilter>
    id_type get_num_components(GraphFilter gf = DefaultFilter()) const
    {
        if (is_undirected)
            return get_num_components_undirected(gf);
        else
            return get_num_components_directed(gf);
    }

    //! Returns true if the graph is connected or strongly connected
    template <typename GraphFilter = DefaultFilter>
    bool is_connected(GraphFilter gf = DefaultFilter()) const
    {
        return (get_num_components(gf) == 1);
    }

    ///////////////////////////////////////////////////////////////////////////
    // BFS distance calculation

    //! Return number of components in undirected graph via BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type> get_bfs_distance(id_type root,
                                          GraphFilter gf = DefaultFilter()) const
    {
        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // level in BFS tree, distance from source
        std::vector<id_type> dist(total_vertex(), ID_INVALID);

        // number of components in graph
        id_type num_components = 0;

        // fill pred with self-predecessors
        for (size_t i = 0; i < pred.size(); ++i)
            pred[i] = i;

        // BFS queue
        std::queue<id_type> queue;

        // iterate over all vertices, try each as root
        if (gf.filter_vertex(vertex(root))) return dist;

        // initialize queue with node root
        queue.push(root);
        dist[root] = 0;

        // breath first search
        while (!queue.empty())
        {
            id_type v = queue.front();
            queue.pop();

            if (gf.filter_vertex(vertex(v))) continue;

            for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                 ei != vertex_edge_end(v); ++ei)
            {
                if (gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v);

                if (pred[w] != w || w == root) // vertex already seen
                    continue;

                queue.push(w);
                pred[w] = v;
                dist[w] = dist[v] + 1;
            }
        }

        return dist;
    }

    //! Return number of components in undirected graph via BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<id_type> get_bfs_distance(const_vertex_iterator root,
                                          GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(root));
        return get_bfs_distance(root.vertex_id(), gf);
    }

    ///////////////////////////////////////////////////////////////////////////
    // cycle finder

    //! Returns true if the graph does not contain a cycle.
    template <typename GraphFilter = DefaultFilter>
    bool is_acyclic_undirected(GraphFilter gf = DefaultFilter()) const
    {
        assert(is_undirected);

        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex());

        // fill pred with self-predecessors
        for (size_t i = 0; i < pred.size(); ++i)
            pred[i] = i;

        // BFS queue
        std::queue<id_type> queue;

        // iterate over all vertices, try each as root
        for (const_vertex_iterator vi = vertex_begin();
             vi != vertex_end(); ++vi)
        {
            if (gf.filter_vertex(vi)) continue;

            id_type r = vi.vertex_id();

            if (pred[r] != r) continue;

            // initialize queue with node r
            queue.push(r);

            // breath first search
            while (!queue.empty())
            {
                id_type v = queue.front();
                queue.pop();

                if (gf.filter_vertex(vertex(v))) continue;

                for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                     ei != vertex_edge_end(v); ++ei)
                {
                    if (gf.filter_edge(ei)) continue;

                    id_type w = ei.other_id(v);
                    if (gf.filter_vertex(vertex(w))) continue;

                    if (w == pred[v]) // dont walk backwards
                        continue;

                    if (pred[w] != w) // cycle in BFS-tree
                        return false;

                    queue.push(w);
                    pred[w] = v;
                }
            }
        }

        return true;
    }

    //! Returns true if the graph does not contain a cycle.
    template <typename GraphFilter = DefaultFilter>
    bool is_acyclic_directed(GraphFilter = DefaultFilter()) const
    {
        assert(is_directed);
        abort(); // not implemented
    }

    //! Returns true if the graph does not contain a cycle.
    template <typename GraphFilter = DefaultFilter>
    bool is_acyclic(GraphFilter gf = DefaultFilter()) const
    {
        if (is_undirected)
            return is_acyclic_undirected(gf);
        else
            return is_acyclic_directed(gf);
    }

    //! Returns true if the graph contains a cycle
    template <typename GraphFilter = DefaultFilter>
    bool is_cyclic(GraphFilter gf = DefaultFilter()) const
    {
        return !is_acyclic(gf);
    }

    ///////////////////////////////////////////////////////////////////////////
    // find_path() and get_distance() in graph via BFS

    //! Find a path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<const_edge_iterator>
    find_path(const id_type& s, const id_type& t,
              GraphFilter gf = DefaultFilter()) const
    {
        std::vector<const_edge_iterator> p;
        if (s == t) return p;

        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // BFS queue
        std::queue<id_type> queue;

        // Initialize queue with node s
        queue.push(s);

        while (!queue.empty() && pred[t] == ID_INVALID)
        {
            id_type v = queue.front();
            queue.pop();

            if (gf.filter_vertex(vertex(v))) continue;

            for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                 ei != vertex_edge_end(v); ++ei)
            {
                if (gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v);
                if (gf.filter_vertex(vertex(w))) continue;

                if (pred[w] != ID_INVALID) // vertex already seen
                    continue;

                queue.push(w);
                pred[w] = ei.edge_id();    // save edge
            }
        }

        assert(pred[t] != ID_INVALID);

        id_type c = t;
        while (c != s)
        {
            p.push_back(edge(pred[c]));
            c = edge(pred[c]).other_id(c);
        }

        std::reverse(p.begin(), p.end());

        return p;
    }

    //! Find a path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<const_edge_iterator>
    find_path(const const_vertex_iterator& s, const const_vertex_iterator& t,
              GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(s) && contains(t));
        return find_path(s.vertex_id(), t.vertex_id(), gf);
    }

    //! Find a path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<edge_iterator>
    find_path(const id_type& s, const id_type& t, GraphFilter gf = DefaultFilter())
    {
        std::vector<edge_iterator> p;
        if (s == t) return p;

        // predecessor vertex in BFS tree
        std::vector<id_type> pred(total_vertex(), ID_INVALID);

        // BFS queue
        std::queue<id_type> queue;

        // Initialize queue with node s
        queue.push(s);

        while (!queue.empty() && pred[t] == ID_INVALID)
        {
            id_type v = queue.front();
            queue.pop();

            if (gf.filter_vertex(vertex(v))) continue;

            for (const_vertex_edge_iterator ei = vertex_edge_begin(v);
                 ei != vertex_edge_end(v); ++ei)
            {
                if (gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v);
                if (gf.filter_vertex(vertex(w))) continue;

                if (pred[w] != ID_INVALID) // vertex already seen
                    continue;

                queue.push(w);
                pred[w] = ei.edge_id();    // save edge
            }
        }

        assert(pred[t] != ID_INVALID);

        id_type c = t;
        while (c != s)
        {
            p.push_back(edge(pred[c]));
            c = edge(pred[c]).other_id(c);
        }

        std::reverse(p.begin(), p.end());

        return p;
    }

    //! Find a path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<edge_iterator>
    find_path(const const_vertex_iterator& s, const const_vertex_iterator& t,
              GraphFilter gf = DefaultFilter())
    {
        assert(contains(s) && contains(t));
        return find_path(s.vertex_id(), t.vertex_id(), gf);
    }

    //! Find distance of shortest path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    id_type get_distance(const id_type& s, const id_type& t, GraphFilter gf = DefaultFilter()) const
    {
        if (s == t) return 0;

        // seen in BFS tree, distance is in the queue
        std::vector<id_type> seen(total_vertex(), 0);

        // BFS queue
        using pair_type = std::pair<id_type, id_type>;
        std::queue<pair_type> queue;

        // initialize queue with source node
        queue.push(pair_type(s, 0));
        seen[s] = 1;

        // breath first search
        while (!queue.empty())
        {
            pair_type v = queue.front();
            queue.pop();

            if (gf.filter_vertex(vertex(v.first))) continue;

            for (const_vertex_edge_iterator ei = vertex_edge_begin(v.first);
                 ei != vertex_edge_end(v.first); ++ei)
            {
                if (gf.filter_edge(ei)) continue;

                id_type w = ei.other_id(v.first);

                if (seen[w]) // vertex already seen
                    continue;

                if (w == t) return v.second + 1;

                seen[w] = 1;
                queue.push(pair_type(w, v.second + 1));
            }
        }

        return ID_INVALID;
    }

    //! Find distance of shortest path s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    id_type get_distance(const const_vertex_iterator& s, const const_vertex_iterator& t,
                         GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(s) && contains(t));
        return get_distance(s.vertex_id(), t.vertex_id(), gf);
    }

    ///////////////////////////////////////////////////////////////////////////
    // find_all_shortest_paths in graph via BFS

    //! Find all path s -> t of shortest path length using BFS
    template <typename GraphFilter,
              typename GraphType, typename EdgeIterator>
    class AlgAllShortestPaths
    {
    protected:
        //! Type of edge list
        using edgelist_type = std::vector<EdgeIterator>;

        //! The graph
        GraphType& gr;

        //! Source and Target nodes
        id_type src, tgt;

        //! Type of id list
        using idlist_type = std::vector<id_type>;

        //! Predecessor vertices in BFS tree/DAG
        std::vector<idlist_type> pred;

        //! perform recursive DFS on pred tree/DAG to generate all paths
        void dfs_predtree(std::vector<edgelist_type>& pathlist,
                          id_type v, const edgelist_type& path) const
        {
            if (v == src) {
                // save path in reverse
                pathlist.push_back(
                    edgelist_type(path.rbegin(), path.rend()));
                return;
            }

            for (idlist_type::const_iterator ui = pred[v].begin();
                 ui != pred[v].end(); ++ui)
            {
                edgelist_type p = path;
                p.push_back(gr.edge(*ui));

                dfs_predtree(pathlist, p.back().other_id(v), p);
            }
        }

    public:
        AlgAllShortestPaths(const id_type& s, const id_type& t,
                            GraphType g, GraphFilter gf)
            : gr(g),
              src(s), tgt(t),
              pred(g.total_vertex())
        {
            if (src == tgt) return;

            id_type d = 0;

            // distance of vertices
            std::vector<id_type> dist(gr.total_vertex(), ID_INVALID);

            // BFS queue
            std::queue<id_type> queue1, queue2;

            // Initialize queue2 with node s
            queue2.push(src);
            dist[src] = d;

            while (!queue2.empty() && dist[t] == ID_INVALID)
            {
                std::swap(queue1, queue2);

                // Fully process queue1 for next BFS layer
                ++d;

                while (!queue1.empty())
                {
                    id_type v = queue1.front();
                    queue1.pop();

                    if (gf.filter_vertex(gr.vertex(v))) continue;

                    for (const_vertex_edge_iterator ei = gr.vertex_edge_begin(v);
                         ei != gr.vertex_edge_end(v); ++ei)
                    {
                        if (gf.filter_edge(ei)) continue;

                        id_type w = ei.other_id(v);
                        if (gf.filter_vertex(gr.vertex(w))) continue;

                        // vertex is new
                        if (dist[w] == ID_INVALID)
                        {
                            dist[w] = d;
                            queue2.push(w);
                        }
                        // save BFS DAG edge to previous level
                        if (dist[w] == d) {
                            pred[w].push_back(ei.edge_id());
                        }
                    }
                }
            }

            assert(dist[t] != ID_INVALID);
        }

        std::vector<edgelist_type> get_pathlist() const
        {
            std::vector<edgelist_type> pathlist;

            // perform recursive DFS on pred tree/DAG to generate all paths
            edgelist_type path;
            dfs_predtree(pathlist, tgt, path);

            return pathlist;
        }

        GraphBase get_bfs_graph() const
        {
            // marker for vertices in backwards tree
            std::vector<id_type> seen(gr.total_vertex(), 0);

            // BFS queue
            std::queue<id_type> queue1, queue2;

            // Initialize queue2 with node s
            queue2.push(tgt);
            seen[tgt] = 1;

            while (!queue2.empty() && seen[src] == 0)
            {
                std::swap(queue1, queue2);

                // Fully process queue1 for next BFS layer
                while (!queue1.empty())
                {
                    id_type v = queue1.front();
                    queue1.pop();

                    for (idlist_type::const_iterator pi = pred[v].begin();
                         pi != pred[v].end(); ++pi)
                    {
                        EdgeIterator w = gr.edge(*pi);
                        id_type wi = w.other_id(v);

                        if (seen[wi]) continue;
                        // vertex is new

                        seen[wi] = 1;
                        queue2.push(wi);
                    }
                }
            }

            GraphBase bfs;

            for (id_type v = 0; v < gr.total_vertex(); ++v)
            {
                if (seen[v]) {
                    vertex_iterator vi = bfs.add_vertex(*gr.vertex(v));
                    seen[v] = vi.vertex_id();
                }
                else {
                    seen[v] = ID_INVALID;
                }
            }

            for (std::vector<idlist_type>::const_iterator pli = pred.begin();
                 pli != pred.end(); ++pli)
            {
                for (idlist_type::const_iterator pi = pli->begin();
                     pi != pli->end(); ++pi)
                {
                    EdgeIterator e = gr.edge(*pi);

                    if (seen[e.tail_id()] == ID_INVALID) continue;
                    if (seen[e.head_id()] == ID_INVALID) continue;

                    bfs.add_edge(seen[e.tail_id()],
                                 seen[e.head_id()], *e);
                }
            }

            return bfs;
        }
    };

    //! Find all path s -> t of shortest path length using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<std::vector<edge_iterator> >
    find_all_shortest_paths(const id_type& s, const id_type& t,
                            GraphFilter gf = DefaultFilter())
    {
        return AlgAllShortestPaths<GraphFilter, GraphBase&, edge_iterator>
                   (s, t, *this, gf).get_pathlist();
    }

    //! Find all path s -> t of shortest path length using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<std::vector<edge_iterator> >
    find_all_shortest_paths(const const_vertex_iterator& s,
                            const const_vertex_iterator& t,
                            GraphFilter gf = DefaultFilter())
    {
        assert(contains(s) && contains(t));
        return find_all_shortest_paths(s.vertex_id(), t.vertex_id(), gf);
    }

    //! Find all path s -> t of shortest path length using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<std::vector<const_edge_iterator> >
    find_all_shortest_paths(const id_type& s, const id_type& t,
                            GraphFilter gf = DefaultFilter()) const
    {
        return AlgAllShortestPaths<GraphFilter, const GraphBase&, const_edge_iterator>
                   (s, t, *this, gf).get_pathlist();
    }

    //! Find all path s -> t of shortest path length using BFS
    template <typename GraphFilter = DefaultFilter>
    std::vector<std::vector<const_edge_iterator> >
    find_all_shortest_paths(const const_vertex_iterator& s,
                            const const_vertex_iterator& t,
                            GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(s) && contains(t));
        return find_all_shortest_paths(s.vertex_id(), t.vertex_id(), gf);
    }

    //! Get graph of a shortest paths s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    GraphBase
    get_all_shortest_path_graph(const id_type& s, const id_type& t,
                                GraphFilter gf = DefaultFilter())
    {
        return AlgAllShortestPaths<GraphFilter, GraphBase&, edge_iterator>
                   (s, t, *this, gf).get_bfs_graph();
    }

    //! Get graph of a shortest paths s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    GraphBase
    get_all_shortest_path_graph(const const_vertex_iterator& s,
                                const const_vertex_iterator& t,
                                GraphFilter gf = DefaultFilter())
    {
        assert(contains(s) && contains(t));
        return get_all_shortest_path_graph(s.vertex_id(), t.vertex_id(), gf);
    }

    //! Get graph of a shortest paths s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    GraphBase
    get_all_shortest_path_graph(const id_type& s, const id_type& t,
                                GraphFilter gf = DefaultFilter()) const
    {
        return AlgAllShortestPaths<GraphFilter, const GraphBase&, const_edge_iterator>
                   (s, t, *this, gf).get_bfs_graph();
    }

    //! Get graph of a shortest paths s -> t using BFS
    template <typename GraphFilter = DefaultFilter>
    GraphBase
    get_all_shortest_path_graph(const const_vertex_iterator& s,
                                const const_vertex_iterator& t,
                                GraphFilter gf = DefaultFilter()) const
    {
        assert(contains(s) && contains(t));
        return get_all_shortest_path_graph(s.vertex_id(), t.vertex_id(), gf);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Structural Properties: diameter

    template <typename GraphFilter = DefaultFilter>
    id_type get_diameter(const_vertex_iterator* vSource = nullptr,
                         const_vertex_iterator* vTarget = nullptr,
                         GraphFilter gf = DefaultFilter()) const
    {
        // currently determined diameter
        unsigned int diam = 0;

        // level in BFS tree, distance from source
        std::vector<id_type> seen(total_vertex(), 0);
        id_type seen_key = 0;

        for (const_vertex_iterator r = vertex_begin(); r != vertex_end(); ++r)
        {
            // iterate over all vertices, try each as root
            if (gf.filter_vertex(r)) continue;

            // BFS queue
            using pair_type = std::pair<id_type, id_type>;
            std::queue<pair_type> queue;

            // initialize queue with node root
            queue.push(pair_type(r.vertex_id(), 0));
            seen[r.vertex_id()] = ++seen_key;

            // breath first search
            while (!queue.empty())
            {
                pair_type v = queue.front();
                queue.pop();

                if (gf.filter_vertex(vertex(v.first))) continue;

                if (v.second > diam)
                {
                    diam = v.second;
                    if (vSource) *vSource = r;
                    if (vTarget) *vTarget = vertex(v.first);
                }

                for (const_vertex_edge_iterator ei = vertex_edge_begin(v.first);
                     ei != vertex_edge_end(v.first); ++ei)
                {
                    if (gf.filter_edge(ei)) continue;

                    id_type w = ei.other_id(v.first);

                    if (seen[w] == seen_key) // vertex already seen
                        continue;

                    seen[w] = seen_key;
                    queue.push(pair_type(w, v.second + 1));
                }
            }
        }

        return diam;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Isomorphism Checker

    //! Algorithmic class to check for isomorphism
    template <typename GraphTypeA, typename GraphTypeB>
    class AlgIsomorphic
    {
    protected:
        bool m_result;

        //! graph A
        const GraphTypeA& m_g1;
        //! graph B
        const GraphTypeB& m_g2;

        using id_type = typename GraphTypeA::id_type;

        using vertexA_iterator = typename GraphTypeA::const_vertex_iterator;
        using edgeA_iterator = typename GraphTypeA::const_edge_iterator;

        using vertexB_iterator = typename GraphTypeA::const_vertex_iterator;
        using edgeB_iterator = typename GraphTypeA::const_edge_iterator;

        //! bijektive mapping from vertices of g1 to g2
        std::vector<id_type> m_biject;

    public:
        //! try to find a bijective mapping from g1 to g2
        AlgIsomorphic(const GraphTypeA& g1, const GraphTypeB& g2)
            : m_result(false),
              m_g1(g1), m_g2(g2)
        {
            if (g1.num_vertex() != g2.num_vertex() ||
                g1.num_edge() != g2.num_edge() ||
                g1.get_degree_sequence() != g2.get_degree_sequence())
            {
                return; // false
            }

            m_biject.resize(g1.total_vertex());

            rec_buildmap(0, 0);
        }

    protected:
        //! faster degree-matching permutation check
        bool rec_buildmap(id_type deg, id_type vcover)
        {
            if (vcover == m_g1.num_vertex())
                return check_bijection();

            // find all vertexes of degree deg in g1 and g2
            std::vector<id_type> vdeg1, vdeg2;

            for (vertexA_iterator v = m_g1.vertex_begin(); v != m_g1.vertex_end(); ++v)
            {
                if (v.degree() == deg) vdeg1.push_back(v.vertex_id());
            }

            for (vertexB_iterator v = m_g2.vertex_begin(); v != m_g2.vertex_end(); ++v)
            {
                if (v.degree() == deg) vdeg2.push_back(v.vertex_id());
            }

            assert(vdeg1.size() == vdeg2.size());

            // iterate over permutation of vertexes of same degree in g1
            do {
                for (size_t i = 0; i < vdeg1.size(); ++i)
                {
                    m_biject[vdeg1[i]] = vdeg2[i];
                }

                if (rec_buildmap(deg + 1, vcover + vdeg1.size())) return true;
            }
            while (std::next_permutation(vdeg1.begin(), vdeg1.end()));

            return false;
        }

        //! check if bijective mapping is valid for all edges
        bool check_bijection()
        {
            for (id_type v = 0; v < m_g1.total_vertex(); ++v)
            {
                if (m_g1.vertex_deleted(v)) continue;

                assert(!m_g2.vertex_deleted(m_biject[v]));

                if (m_g1.vertex(v).degree() != m_g2.vertex(m_biject[v]).degree()) {
                    assert(!"should not happen with degree permutation mapping");
                    return false;
                }
            }

            std::vector<char> emarks(m_g2.total_edge(), false);

            for (edgeA_iterator e1 = m_g1.edge_begin(); e1 != m_g1.edge_end(); ++e1)
            {
                id_type e1t = m_biject[e1.tail_id()], e1h = m_biject[e1.head_id()];

                // iterate over multiple instances of parallel edges
                for (id_type inst = 0; ; ++inst)
                {
                    edgeB_iterator e2 = m_g2.find_edge(e1t, e1h, inst);
                    if (e2 == m_g2.edge_end()) return false;

                    if (!emarks[e2.edge_id()]) {
                        emarks[e2.edge_id()] = true;
                        break;
                    }
                }
            }

            for (id_type e = 0; e < m_g2.total_edge(); ++e)
            {
                if (!emarks[e] != m_g2.edge_deleted(e)) return false;
            }

            m_result = true;

            return true;
        }

    public:
        //! returns true if isomorphism check was successful.
        bool result() const
        {
            return m_result;
        }
    };

    //! check whether the two graphs are isomorphic
    template <typename GraphTypeA, typename GraphTypeB>
    static bool isomorphic(const GraphTypeA& g1, const GraphTypeB& g2)
    {
        return AlgIsomorphic<GraphTypeA, GraphTypeB>(g1, g2).result();
    }

    //! check whether the two graphs are isomorphic
    template <typename GraphTypeB>
    bool isomorphic(const GraphTypeB& g2) const
    {
        return AlgIsomorphic<GraphBase, GraphTypeB>(*this, g2).result();
    }

    //! Class to test graph for isomorphism against all previously checked ones.
    class IsomorphismCheck
    {
    protected:
        using graphmap_type = std::multimap<std::string, GraphBase>;

        //! list of already seen graphs
        graphmap_type m_graphmap;

    public:
        //! Returns true if the graph's isomorphism class was not seen yet.
        inline bool operator () (const GraphBase& g)
        {
            std::string id = g.get_degree_sequence();
            // std::cout << "// degrees = " << id << "\n";

            for (typename graphmap_type::const_iterator gi = m_graphmap.find(id);
                 gi != m_graphmap.end() && gi->first == id; ++gi)
            {
                if (g.isomorphic(gi->second)) return false;
            }

            m_graphmap.insert(std::make_pair(id, g));

            return true;
        }
    };

public:
    using triangle_type = std::array<const_edge_iterator, 3>;

    //! Enumerate all triangles in graph
    std::vector<triangle_type> find_triangles() const
    {
        std::vector<triangle_type> triangles;

        for (const_edge_iterator e1 = edge_begin(); e1 != edge_end(); ++e1)
        {
            const_vertex_iterator v1 = e1.head();
            const_vertex_iterator v2 = e1.tail();

            for (const_vertex_edge_iterator v1e = v1.edge_begin();
                 v1e != v1.edge_end(); ++v1e)
            {
                if (v1e.edge_id() <= e1.edge_id()) continue;

                for (const_vertex_edge_iterator v2e = v2.edge_begin();
                     v2e != v2.edge_end(); ++v2e)
                {
                    if (v2e.edge_id() <= e1.edge_id()) continue;
                    if (v1e.edge_id() <= v2e.edge_id()) continue;

                    const_vertex_iterator v1o = v1e.other(v1);
                    const_vertex_iterator v2o = v2e.other(v2);

                    if (v1o != v2o) continue;

                    // std::cout << "// " << e1 << " - " << v1e << " - " << v2e << "\n";
                    triangles.push_back(triangle_type { e1, v1e, v2e });
                }
            }
        }

        return triangles;
    }

    using triangle_vertex_type = std::array<const_vertex_iterator, 3>;

    //! Return vertices of the triangle
    static triangle_vertex_type get_triangle_vertices(const triangle_type& tri)
    {
        const_vertex_iterator v1 = tri[0].head();
        const_vertex_iterator v2 = tri[0].tail();

        if (tri[1].head() == v1) {
            return triangle_vertex_type { v1, v2, tri[1].tail() };
        }
        else if (tri[1].head() == v2) {
            return triangle_vertex_type { v1, v2, tri[1].tail() };
        }
        else if (tri[1].tail() == v1) {
            return triangle_vertex_type { v1, v2, tri[1].head() };
        }
        else if (tri[1].tail() == v2) {
            return triangle_vertex_type { v1, v2, tri[1].head() };
        }
        else {
            // impossible!
            abort();
        }
    }

public:
    //! Add copy of graph to this one
    void add_copy(const GraphBase& from)
    {
        id_type vbase = total_vertex();

        for (const_vertex_iterator vi = from.vertex_begin();
             vi != from.vertex_end(); ++vi)
        {
            add_vertex(*vi);
        }

        for (const_edge_iterator ei = from.edge_begin();
             ei != from.edge_end(); ++ei)
        {
            add_edge(vbase + ei.tail_id(), vbase + ei.head_id(), *ei);
        }
    }

public:
    //! Vertex and edge data factory for Cartesian product of two graphs
    template <typename ProductType, typename GraphType1, typename GraphType2>
    class ProductObjectDataFactory
    {
    public:
        using id_type = typename GraphBase::id_type;

        using vertex_data_type = typename ProductType::vertex_data_type;
        using edge_data_type = typename ProductType::edge_data_type;

        using const_vertex_iterator1 = typename GraphType1::const_vertex_iterator;
        using const_vertex_iterator2 = typename GraphType2::const_vertex_iterator;

        using const_edge_iterator1 = typename GraphType1::const_edge_iterator;
        using const_edge_iterator2 = typename GraphType2::const_edge_iterator;

        vertex_data_type vertex(const const_vertex_iterator1&,
                                const const_vertex_iterator2&)
        {
            return vertex_data_type();
        }

        edge_data_type edge(const const_edge_iterator1&,
                            const const_vertex_iterator2&)
        {
            return edge_data_type();
        }

        edge_data_type edge(const const_vertex_iterator1&,
                            const const_edge_iterator2&)
        {
            return edge_data_type();
        }
    };

    //! Specialization of DataFactory for self-products with same types.
    class SelfProductObjectDataFactory : public ProductObjectDataFactory<GraphBase, GraphBase, GraphBase>
    { };

    //! Calculate Cartesian product of two graphs of this type
    template <typename ProductType,
              typename GraphType1, typename GraphType2,
              typename DataFactoryType = ProductObjectDataFactory<ProductType, GraphType1, GraphType2> >
    static ProductType product(const GraphType1& g1, const GraphType2& g2,
                               DataFactoryType factory = DataFactoryType())
    {
        assert(!g1.has_deleted() && !g2.has_deleted());

        id_type n1 = g1.num_vertex(), n2 = g2.num_vertex();

        ProductType p;

        for (typename GraphType1::const_vertex_iterator v1 = g1.vertex_begin();
             v1 != g1.vertex_end(); ++v1)
        {
            for (typename GraphType2::const_vertex_iterator v2 = g2.vertex_begin();
                 v2 != g2.vertex_end(); ++v2)
            {
                p.add_vertex(factory.vertex(v1, v2));
            }
        }

        for (typename GraphType1::const_edge_iterator e1 = g1.edge_begin();
             e1 != g1.edge_end(); ++e1)
        {
            for (typename GraphType2::const_vertex_iterator v2 = g2.vertex_begin();
                 v2 != g2.vertex_end(); ++v2)
            {
                p.add_edge(e1.tail_id() * n2 + v2.vertex_id(),
                           e1.head_id() * n2 + v2.vertex_id(),
                           factory.edge(e1, v2));
            }
        }

        for (typename GraphType1::const_vertex_iterator v1 = g1.vertex_begin();
             v1 != g1.vertex_end(); ++v1)
        {
            for (typename GraphType2::const_edge_iterator e2 = g2.edge_begin();
                 e2 != g2.edge_end(); ++e2)
            {
                p.add_edge(v1.vertex_id() * n2 + e2.tail_id(),
                           v1.vertex_id() * n2 + e2.head_id(),
                           factory.edge(v1, e2));
            }
        }

        return p;
    }
};

//! A basic graph structure composed of a mix of adjacency array and list.
template <class VertexData, class EdgeData, bool AllowDeletes = true>
using Graph = GraphBase<VertexData, EdgeData, false, false, false, AllowDeletes>;

//! A basic graph structure composed of a mix of adjacency array and list.
template <class VertexData, class EdgeData, bool AllowDeletes = true>
using Digraph = GraphBase<VertexData, EdgeData, true, false, false, AllowDeletes>;

///////////////////////////////////////////////////////////////////////////////

//! Generate a random graph with given number of vertices and edges, return
//! false if impossible.
template <typename GraphType>
bool generate_random_graph(GraphType& out, unsigned int num_vertex, unsigned int num_edge,
                           unsigned int* seed)
{
    GraphType g(num_vertex);

    if (!g.allow_parallel)
    {
        unsigned int maxedges =
            g.is_directed && g.allow_loops ? (num_vertex * num_vertex) :
            g.is_directed && !g.allow_loops ? (num_vertex * num_vertex - num_vertex) :
            !g.is_directed && g.allow_loops ? (num_vertex * (num_vertex - 1) / 2 + num_vertex) :
            !g.is_directed && !g.allow_loops ? (num_vertex * (num_vertex - 1) / 2) :
            0;

        if (num_edge > maxedges)
            return false;
    }

    for (unsigned int en = 0; en < num_edge; ++en)
    {
        int v = rand_r(seed) % num_vertex,
            w = rand_r(seed) % num_vertex;

        if (!g.allow_loops && v == w) {
            --en;
            continue;
        }
        if (!g.allow_parallel)
        {
            if (g.find_edge(v, w) != g.edge_end()) {
                --en;
                continue;
            }
            if (g.find_edge(w, v) != g.edge_end() && g.is_undirected) {
                --en;
                continue;
            }
        }

        g.add_edge(v, w);
    }

    assert(g.num_edge() == num_edge);
    out = g;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

//! Base class for graph decorators. Decorators are used to add labels and
//! colors to text graph outputs.
template <typename GraphType>
class GraphDecorator
{
protected:
    //! Import id_type from GraphType
    using id_type = typename GraphType::id_type;
    //! Import constant vertex iterator
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import constant edge iterator
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Graph attributes.
    void graph(std::ostream&, const GraphType&) const
    { }
    //! Filter vertices, must return true those to be hidden.
    bool filter_vertex(const vertex_iterator&) const
    {
        return false;
    }
    //! Default vertex attributes
    void vertex_default(std::ostream&) const
    { }
    //! Output no vertex attributes
    bool vertex_short(const GraphType&) const
    {
        return false;
    }
    //! Vertex attributes for iterator
    void vertex(std::ostream&, const vertex_iterator&) const
    { }
    //! Filter edges, must be return true those to be hidden.
    bool filter_edge(const edge_iterator&) const
    {
        return false;
    }
    //! Default edge attributes
    void edge_default(std::ostream&) const
    { }
    //! Edge attributes for iterator
    void edge(std::ostream&, const edge_iterator&) const
    { }
    //! Extra text
    void extra(std::ostream&, const GraphType&) const
    { }
};

//! Output a graph in graphviz (dot, fdp, etc) format using given decorator.
template <typename GraphType, typename GraphDecorator>
class GraphvizOutput
{
protected:
    //! Graph to output
    const GraphType& m_graph;

    //! Graph decorator to use for attributes.
    GraphDecorator m_graphdeco;

    //! Import vertex iterator from GraphType
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import edge iterator from GraphType
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Construct graph output generator for given graph.
    explicit GraphvizOutput(const GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    { }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const GraphvizOutput& go)
    {
        const GraphType& g = go.m_graph;
        const GraphDecorator& gd = go.m_graphdeco;

        os << (g.is_directed ? "digraph" : "graph") << " G {\n";
        os << "  graph [";
        gd.graph(os, g);
        os << "]\n";
        os << "  node [";
        gd.vertex_default(os);
        os << "]\n";
        os << "  edge [";
        gd.edge_default(os);
        os << "]\n";
        for (vertex_iterator v = g.vertex_begin(); v != g.vertex_end(); ++v)
        {
            if (gd.filter_vertex(v)) continue;
            os << "  " << v.vertex_id() << " [";
            gd.vertex(os, v);
            os << "];\n";
        }
        for (edge_iterator e = g.edge_begin(); e != g.edge_end(); ++e)
        {
            if (gd.filter_edge(e)) continue;
            os << "  " << e.tail_id()
               << (g.is_directed ? " -> " : " -- ")
               << e.head_id() << " [";
            gd.edge(os, e);
            os << "];\n";
        }
        gd.extra(os, g);
        os << "}";

        return os;
    }
};

//! Template function for easier output: use std::cout << graphviz(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
GraphvizOutput<GraphType, GraphDecorator> graphviz(const GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return GraphvizOutput<GraphType, GraphDecorator>(g, gd);
}

//! Output a graph in tikz format using given decorator.
template <typename GraphType, typename GraphDecorator>
class GraphTikzOutput
{
protected:
    //! Graph to output
    const GraphType& m_graph;

    //! Graph decorator to use for attributes.
    GraphDecorator m_graphdeco;

    //! Import vertex iterator from GraphType
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import edge iterator from GraphType
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Construct graph output generator for given graph.
    explicit GraphTikzOutput(const GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    { }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const GraphTikzOutput& go)
    {
        const GraphType& g = go.m_graph;
        const GraphDecorator& gd = go.m_graphdeco;

        os << "\\begin{tikzpicture}";
        os << "[graphtheorie,";
        gd.graph(os, g);
        os << "]\n";
        for (vertex_iterator v = g.vertex_begin(); v != g.vertex_end(); ++v)
        {
            if (gd.filter_vertex(v)) continue;
            os << "  \\node[";
            gd.vertex(os, v);
            os << "] (v" << v.vertex_id() << ")"
               << " at (";
            gd.vertex_x(os, v);
            os << ",";
            gd.vertex_y(os, v);
            os << ")"
               << " {";
            gd.vertex_label(os, v);
            os << "};\n";
        }
        for (edge_iterator e = g.edge_begin(); e != g.edge_end(); ++e)
        {
            if (gd.filter_edge(e)) continue;
            os << "  \\draw[";
            gd.edge(os, e);
            os << "] (v" << e.tail_id() << ") -- (v" << e.head_id() << ");\n";
        }
        gd.extra(os, g);
        os << "\\end{tikzpicture}";

        return os;
    }
};

//! Template function for easier output: use std::cout << graphtikz(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
GraphTikzOutput<GraphType, GraphDecorator> graphtikz(const GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return GraphTikzOutput<GraphType, GraphDecorator>(g, gd);
}

//! Output a graph in tgf (trivial graph) format using given decorator.
template <typename GraphType, typename GraphDecorator>
class TrivialGraphOutput
{
protected:
    //! Graph to output
    const GraphType& m_graph;

    //! Graph decorator to use for attributes.
    GraphDecorator m_graphdeco;

    //! Import vertex iterator from GraphType
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import edge iterator from GraphType
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Construct graph output generator for given graph.
    explicit TrivialGraphOutput(const GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    { }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const TrivialGraphOutput& go)
    {
        const GraphType& g = go.m_graph;
        const GraphDecorator& gd = go.m_graphdeco;

        for (vertex_iterator v = g.vertex_begin(); v != g.vertex_end(); ++v)
        {
            os << v.vertex_id() << " ";
            gd.vertex_label(os, v);
            os << "\n";
        }

        os << "#\n";

        for (edge_iterator e = g.edge_begin(); e != g.edge_end(); ++e)
        {
            os << e.tail_id() << " " << e.head_id() << " ";
            gd.edge_label(os, e);
            os << "\n";
        }

        return os;
    }
};

//! Template function for easier output: use std::cout << graphtgf(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
TrivialGraphOutput<GraphType, GraphDecorator> graphtgf(const GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return TrivialGraphOutput<GraphType, GraphDecorator>(g, gd);
}

//! Output a graph in GraphString (our custom) format using given decorator.
template <typename GraphType, typename GraphDecorator>
class GraphStringOutput
{
protected:
    //! Graph to output
    const GraphType& m_graph;

    //! Graph decorator to use for attributes.
    GraphDecorator m_graphdeco;

    //! Import vertex iterator from GraphType
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import edge iterator from GraphType
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Construct graph output generator for given graph.
    explicit GraphStringOutput(const GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    { }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const GraphStringOutput& gso)
    {
        const GraphType& g = gso.m_graph;
        const GraphDecorator& gd = gso.m_graphdeco;

        os << 'V' << g.num_vertex();

        if (!gd.vertex_short(g))
        {
            os << ':';
            for (vertex_iterator v = g.vertex_begin(); v != g.vertex_end(); ++v)
            {
                os << 'i' << v.vertex_id();
                gd.vertex(os, v);
                os << '/';
            }
        }

        os << ';';

        os << 'E' << g.num_edge() << ':';
        for (edge_iterator e = g.edge_begin(); e != g.edge_end(); ++e)
        {
            os << 'i' << e.edge_id()
               << 't' << e.tail_id() << 'h' << e.head_id();
            gd.edge(os, e);
            os << '/';
        }
        os << ';';

        return os;
    }
};

//! Template function for easier output: use std::cout << graphstring(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
GraphStringOutput<GraphType, GraphDecorator>
graphstring(const GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return GraphStringOutput<GraphType, GraphDecorator>(g, gd);
}

#endif // !BISPANNING_GRAPH_HEADER

/******************************************************************************/
