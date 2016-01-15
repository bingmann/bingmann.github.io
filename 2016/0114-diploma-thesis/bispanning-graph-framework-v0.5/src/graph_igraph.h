/*******************************************************************************
 * src/graph_igraph.h
 *
 * Bridge from our custom graph library to libigraph for calling various more complex
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

#ifndef BISPANNING_GRAPH_IGRAPH_HEADER
#define BISPANNING_GRAPH_IGRAPH_HEADER

#include <igraph/igraph.h>

#include "graph.h"

template <typename GraphType>
class iGraph
{
protected:
    igraph_t graph;
    const GraphType* m_graph;

protected:
    explicit iGraph(igraph_t g)
        : graph(g), m_graph(nullptr)
    { }

public:
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    explicit iGraph(const GraphType& g)
        : m_graph(&g)
    {
        std::vector<igraph_real_t> edges;
        edges.reserve(2 * g.num_edge());

        for (auto e : g.edge_list())
        {
            edges.push_back(e.tail_id());
            edges.push_back(e.head_id());
        }

        igraph_vector_t edges_vec;
        igraph_vector_view(&edges_vec, edges.data(), edges.size());

        // construct empty graph
        igraph_create(&graph, &edges_vec,
                      g.num_vertex(),
                      g.is_directed ? IGRAPH_DIRECTED : IGRAPH_UNDIRECTED);
    }

    static iGraph complete(unsigned int v)
    {
        igraph_t kg;
        igraph_full(&kg, v, false, false);
        return iGraph(kg);
    }

    static iGraph star(unsigned int v)
    {
        igraph_t sg;
        igraph_star(&sg, v, IGRAPH_STAR_UNDIRECTED, 0);
        return iGraph(sg);
    }

    static iGraph wheel(unsigned int v)
    {
        igraph_t wg;
        igraph_star(&wg, v, IGRAPH_STAR_UNDIRECTED, 0);
        for (unsigned int i = 0; i < v - 1; ++i)
            igraph_add_edge(&wg, 1 + i, 1 + (i + 1) % (v - 1));
        return iGraph(wg);
    }

    ~iGraph()
    {
        igraph_destroy(&graph);
    }

    igraph_integer_t get_diameter()
    {
        igraph_integer_t dia;

        igraph_diameter(&graph, &dia, nullptr, nullptr, nullptr, true, true);

        return dia;
    }

    igraph_integer_t get_diameter(vertex_iterator& from, vertex_iterator& to)
    {
        igraph_integer_t dia, efrom, eto;

        igraph_diameter(&graph, &dia, &efrom, &eto, nullptr, true, true);

        assert(m_graph);
        from = m_graph->vertex(efrom);
        to = m_graph->vertex(eto);

        return dia;
    }

    igraph_real_t get_average_path_length()
    {
        igraph_real_t apl;
        igraph_average_path_length(&graph, &apl, false, true);
        return apl;
    }

    igraph_integer_t get_vertex_connectivity()
    {
        igraph_integer_t c;
        igraph_vertex_connectivity(&graph, &c, true);
        return c;
    }

    igraph_integer_t get_edge_connectivity()
    {
        igraph_integer_t c;
        igraph_edge_connectivity(&graph, &c, true);
        return c;
    }

    bool isomorphic(const iGraph& g2)
    {
        igraph_bool_t iso;
        igraph_isomorphic(&graph, &g2.graph, &iso);
        return iso;
    }

    bool contains_subgraph(const iGraph& sub)
    {
        igraph_bool_t iso;
        igraph_subisomorphic(&graph, &sub.graph, &iso);
        return iso;
    }

    igraph_real_t get_radius()
    {
        igraph_real_t dia;
        igraph_radius(&graph, &dia, IGRAPH_ALL);
        return dia;
    }

    igraph_integer_t get_girth()
    {
        igraph_integer_t girth;
        igraph_girth(&graph, &girth, nullptr);
        return girth;
    }

    int write_edgelist(FILE* out)
    {
        return igraph_write_graph_edgelist(&graph, out);
    }

    void layout(GraphType& g)
    {
        assert(igraph_vcount(&graph) == (igraph_integer_t)g.num_vertex());

        igraph_matrix_t coords;
        igraph_matrix_init(&coords, 0, 0);

        igraph_layout_fruchterman_reingold(&graph, &coords,
                                           1000,                                             // iterations
                                           g.num_vertex(),                                   // max delta
                                           g.num_vertex() * sqrt(g.num_vertex()) * 2,        // area
                                           1.25,                                             // cooling
                                           g.num_vertex() * g.num_vertex() * g.num_vertex(), // vertex-vertex replulsion
                                           false, nullptr, nullptr, nullptr, nullptr, nullptr);

/*
        igraph_layout_kamada_kawai(&graph, &coords,
                                   1000, // iterations
                                   g.num_vertex() / 2, // variation
                                   10, // initial temperature
                                   0.99, // cooling exponent
                                   g.num_vertex() , // attraction
                                   false, nullptr, nullptr, nullptr, nullptr);
*/
/*
        igraph_layout_drl_options_t drl;
        igraph_layout_drl_options_init(&drl, IGRAPH_LAYOUT_DRL_DEFAULT);
        igraph_layout_drl(&graph, &coords,
                          false, &drl, nullptr, nullptr);
*/
        for (size_t i = 0; i < g.num_vertex(); i++) {
            g.vertex(i)->x = MATRIX(coords, i, 0);
            g.vertex(i)->y = MATRIX(coords, i, 1);
        }

        igraph_matrix_destroy(&coords);
    }
};

template <typename GraphType, typename GraphFilter>
void igraph_do_layout(GraphType& g, GraphFilter gf = GraphType::DefaultFilter())
{
    using id_type = typename GraphType::id_type;

    std::vector<igraph_real_t> edges;
    edges.reserve(2 * g.num_edge());

    for (auto e : g.edges())
    {
        if (gf.filter_edge(e))
            continue;

        edges.push_back(e.tail_id());
        edges.push_back(e.head_id());
    }

    igraph_vector_t edges_vec;
    igraph_vector_view(&edges_vec, edges.data(), edges.size());

    // construct empty graph
    igraph_t graph;
    igraph_create(&graph, &edges_vec,
                  g.num_vertex(),
                  g.is_directed ? IGRAPH_DIRECTED : IGRAPH_UNDIRECTED);

    igraph_matrix_t coords;
    igraph_matrix_init(&coords, 0, 0);

    igraph_layout_fruchterman_reingold(&graph, &coords,
                                       1000,                                      // iterations
                                       g.num_vertex(),                            // max delta
                                       g.num_vertex() * sqrt(g.num_vertex()) * 2, // area
                                       1.25,                                      // cooling
                                       g.num_vertex() * g.num_vertex(),           // vertex-vertex replulsion
                                       false, nullptr, nullptr, nullptr, nullptr, nullptr);

    for (size_t i = 0; i < g.num_vertex(); i++) {
        g.vertex(i)->x = MATRIX(coords, i, 0);
        g.vertex(i)->y = MATRIX(coords, i, 1);
    }

    igraph_matrix_destroy(&coords);

    igraph_destroy(&graph);
}

//! Output a graph in graphviz (dot, fdp, etc) format using given decorator.
template <typename GraphType, typename GraphDecorator>
class GraphvizLayoutOutput
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
    explicit GraphvizLayoutOutput(GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    {
        igraph_do_layout(g, gd);
    }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const GraphvizLayoutOutput& go)
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
        for (vertex_iterator v : g.vertices())
        {
            if (gd.filter_vertex(v)) continue;
            os << "  " << v.vertex_id() << " [";
            gd.vertex(os, v);
            os << "];\n";
        }
        for (edge_iterator e : g.edges())
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

//! Template function for easier output: use std::cout << graphviz_layout(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
GraphvizLayoutOutput<GraphType, GraphDecorator> graphviz_layout(GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return GraphvizLayoutOutput<GraphType, GraphDecorator>(g, gd);
}

#endif // !BISPANNING_GRAPH_IGRAPH_HEADER

/******************************************************************************/
