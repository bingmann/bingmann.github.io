/*******************************************************************************
 * src/graph_boost.h
 *
 * Interface of our custom graph library to Boost.Graph. Used only to the if a
 * graph is planar.
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

#ifndef BISPANNING_GRAPH_BOOST_HEADER
#define BISPANNING_GRAPH_BOOST_HEADER

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "graph.h"

template <typename GraphType>
class BoostGraph
{
protected:
    const GraphType* m_graph;

    typedef boost::adjacency_list<
            boost::vecS, boost::vecS,
            boost::undirectedS,
            boost::property<boost::vertex_index_t, int>
            > graph_type;

    graph_type graph;

public:
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    explicit BoostGraph(const GraphType& g)
        : m_graph(&g), graph(g.num_vertex())
    {
        for (auto e : g.edge_list())
        {
            boost::add_edge(e.tail_id(), e.head_id(), graph);
        }
    }

    bool is_planar()
    {
        return boost::boyer_myrvold_planarity_test(graph);
    }
};

#endif // !BISPANNING_GRAPH_BOOST_HEADER

/******************************************************************************/
