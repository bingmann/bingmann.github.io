/*******************************************************************************
 * src/alg_bispanning.h
 *
 * Find two disjoint spanning trees in a graph using the follwing algorithm:
 *
 * James Roskind and Robert E. Tarjan. "A Note on Finding Minimum-Cost
 * Edge-Disjoint Spanning Trees." Mathematics of Operations Research 10, no. 4
 * (November 1, 1985): 701-708. doi:10.2307/3689437.
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

#ifndef BISPANNING_ALG_BISPANNING_HEADER
#define BISPANNING_ALG_BISPANNING_HEADER

#include <algorithm>
#include <queue>
#include <stack>
#include <vector>

#include "debug.h"
#include "graph.h"

//! Simple union find data structure with path compression
template <typename IdType>
class UnionFind
{
protected:
    //! Integer identifier type
    using id_type = IdType;

    //! Struct representing each items
    struct Item
    {
        //! parent item in set
        id_type parent;
        //! rank of subtree
        id_type rank;
    };

    //! Items
    std::vector<Item> m_sets;

public:
    //! Construct union-find with all items as singletons
    explicit UnionFind(const id_type& num)
        : m_sets(num)
    {
        clear();
    }

    //! Reset all items to singletons
    void clear()
    {
        for (size_t v = 0; v < m_sets.size(); ++v)
        {
            m_sets[v].parent = v;
            m_sets[v].rank = 0;
        }
    }

    //! Find set representative of item v.
    const id_type & find(const id_type& v)
    {
        // find root and make root as parent of v (path compression)
        if (m_sets[v].parent != v)
            m_sets[v].parent = find(m_sets[v].parent);

        return m_sets[v].parent;
    }

    //! Find set representative of item v (constant without path compression)
    const id_type & find(const id_type& v) const
    {
        // find root
        if (m_sets[v].parent != v)
            return find(m_sets[v].parent);
        else
            return m_sets[v].parent;
    }

    //! Put items v and w into a union
    void unite(const id_type& v, const id_type& w)
    {
        int vr = find(v), wr = find(w);

        // union by rank
        if (m_sets[vr].rank < m_sets[wr].rank)
            m_sets[vr].parent = wr;
        else if (m_sets[vr].rank > m_sets[wr].rank)
            m_sets[wr].parent = vr;
        // if ranks are same, then make one as root and increment its rank
        else
        {
            m_sets[wr].parent = vr;
            m_sets[vr].rank++;
        }
    }
};

template <typename GraphType, bool debug = false>
struct AlgBispanning : public GraphDecorator<GraphType>
{
protected:
    //! Import id_type from graph type
    using id_type = typename GraphType::id_type;

    //! Import const_vertex_iterator from graph type
    using const_vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import const_edge_iterator from graph type
    using const_edge_iterator = typename GraphType::const_edge_iterator;
    //! Import const_vertex_edge_iterator from graph type
    using const_vertex_edge_iterator = typename GraphType::const_vertex_edge_iterator;

    //! type of color variables
    using color_type = unsigned char;

    //! the processed graph
    const GraphType&        m_graph;

    //! colors of edges 0 = uncolored, 1,2 = tree1 or tree2
    std::vector<color_type> m_color;

    //! three union-find data structures
    UnionFind<id_type>      m_union1, m_union2;

    //! predecessor vertex in BFS tree of both colors
    std::vector<ssize_t>    m_pred1, m_pred2;

    //! label in augmented path search
    std::vector<ssize_t>    m_label;

    //! count number of edges: uncolors, color1 and color2
    unsigned int            m_count[3];

    //! Construct a BFS tree using only colored edges
    void bfs_tree(color_type color, id_type root)
    {
        std::vector<ssize_t>& pred = (color == 1) ? m_pred1 : m_pred2;

        for (size_t v = 0; v < pred.size(); ++v) {
            pred[v] = -1;
        }

        // BFS queue
        std::queue<id_type> queue;

        // Initialize queue with node root
        queue.push(root);
        pred[root] = root;

        // Breadth first search
        while (!queue.empty())
        {
            id_type v = queue.front();
            queue.pop();

            for (const_vertex_edge_iterator ei : m_graph.vertex_edge_list(v))
            {
                if (m_color[ei.edge_id()] != color) continue;

                id_type w = ei.other_id(v);

                if (pred[w] >= 0) // vertex already seen
                    continue;

                queue.push(w);

                // save edge id instead of predecessor vertex due ot parallel
                // edges.
                pred[w] = ei.edge_id();
            }
        }
    }

    bool bfs_augmenting_path(const const_edge_iterator& e0)
    {
        // BFS queue for edges
        std::queue<id_type> queue;

        // initialize queue with node e0
        queue.push(e0.edge_id());

        id_type e0_x = e0.head_id();

        // erase labels
        for (id_type e = 0; e < m_graph.total_edge(); ++e)
            m_label[e] = -1;

        while (!queue.empty())
        {
            id_type e = queue.front();
            queue.pop();

            color_type ti = (m_color[e] % 2) + 1; // other tree

            std::vector<ssize_t>& pred = (ti == 1) ? m_pred1 : m_pred2;
            UnionFind<id_type>& myunion = (ti == 1) ? m_union1 : m_union2;

            id_type e_v = m_graph.edge(e).head_id();
            id_type e_w = m_graph.edge(e).tail_id();

            DBGG("Visiting " << m_graph.edge(e) << " with color " << int(ti) << "!");

            DBGG("Ends of " << e << ": " << myunion.find(e_v) << " - " << myunion.find(e_w));

            if (myunion.find(e_v) != myunion.find(e_w))
            {
                DBGG("Augmenting sequence!");

                myunion.unite(e_v, e_w);

                m_count[ti]++;

                while (m_label[e] >= 0)
                {
                    std::swap(m_color[e], ti);
                    DBGG("colored " << m_graph.edge(e) << " with " << int(m_color[e]));
                    e = m_label[e];
                }

                m_color[e] = ti;
                DBGG("colored final " << m_graph.edge(e) << " with " << int(m_color[e]));

                m_count[0]--;
                return true;
            }

            DBGG("v: pred[" << e_v << "] = " << m_graph.edge(pred[e_v])
                            << " label[" << pred[e_v] << "] = " << m_label[pred[e_v]]);
            DBGG("w: pred[" << e_w << "] = " << m_graph.edge(pred[e_w])
                            << " label[" << pred[e_w] << "] = " << m_label[pred[e_w]]);

            assert(pred[e_v] >= 0 && pred[e_w] >= 0);

            // pick the vertex u which is not the BFS root, and walk upwards to
            // find a part of the cycle
            id_type e_u;
            if (e_v != e0_x && m_label[pred[e_v]] < 0)
                e_u = e_v;
            else if (e_w != e0_x && m_label[pred[e_w]] < 0)
                e_u = e_w;
            else {
                DBGG("Both ends of edge already in label tree.");
                continue;
            }

            DBGG("u0 = " << e_u << ", pred = " << pred[e_u] << " -> " << m_graph.edge(pred[e_u])
                         << ", label = " << m_label[pred[e_u]]);

            std::stack<id_type> predpath;

            while (e_u != e0_x && m_label[pred[e_u]] < 0)
            {
                const_edge_iterator en = m_graph.edge(pred[e_u]);

                DBGG("push (e_u,pred[e_u]) = (" << e_u << "," << pred[e_u] << ") = " << en.edge_id() << " onto stack.");
                predpath.push(en.edge_id());

                e_u = en.other_id(e_u);

                if (e_u != e0_x) {
                    DBGG("u = " << e_u << ", pred = " << pred[e_u] << " -> " << m_graph.edge(pred[e_u])
                                << ", label = " << m_label[pred[e_u]]);
                }
            }

            while (!predpath.empty())
            {
                id_type e_prime = predpath.top();
                predpath.pop();

                DBGG("label " << m_graph.edge(e_prime) << " with " << e << " and put " << e_prime << " on queue.");

                assert(m_label[e_prime] < 0);
                m_label[e_prime] = e;

                queue.push(e_prime);
            }
        }

        return false;
    }

public:
    explicit AlgBispanning(GraphType& g)
        : m_graph(g),
          m_color(g.total_edge(), 0),
          m_union1(g.total_vertex()),
          m_union2(g.total_vertex()),
          m_pred1(g.total_vertex()),
          m_pred2(g.total_vertex()),
          m_label(g.total_edge())
    {
        m_count[0] = m_count[1] = m_count[2] = 0;

        for (const_edge_iterator e0 : m_graph.edge_list())
        {
            id_type e0id = e0.edge_id();
            id_type e0_x = e0.head_id(), e0_y = e0.tail_id();

            m_color[e0id] = e0->color + 1;

            if (m_color[e0id] == 1 && m_union1.find(e0_x) != m_union1.find(e0_y))
            {
                DBGG("Edge " << e0 << " directly readded to tree 1");

                m_union1.unite(e0_x, e0_y);
                m_count[1]++;
            }
            else if (m_color[e0id] == 2 && m_union2.find(e0_x) != m_union2.find(e0_y))
            {
                DBGG("Edge " << e0 << " directly readded to tree 2");

                m_union2.unite(e0_x, e0_y);
                m_count[2]++;
            }
            else
            {
                m_color[e0id] = 0;
                m_count[0]++;
            }
        }

        // iterate over all edges and try to put them into a tree.
        for (const_edge_iterator e0 : m_graph.edge_list())
        {
            id_type e0id = e0.edge_id();
            id_type e0_x = e0.head_id(), e0_y = e0.tail_id();

            DBGG("Processing new edge " << e0);

            if (m_color[e0id] != 0)
            {
                DBGG("Edge already in a tree");
            }
 #if 1      // an optimization -> no bfs necessary.
            // check two simple cases
            else if (m_union1.find(e0_x) != m_union1.find(e0_y))
            {
                DBGG("Edge added directly to tree 1");

                m_color[e0id] = 1;
                m_union1.unite(e0_x, e0_y);
                m_count[1]++;
                m_count[0]--;

                DBG(graphviz(g, *this));
            }
            else if (m_union2.find(e0_x) != m_union2.find(e0_y))
            {
                DBGG("Edge added directly to tree 2");

                m_color[e0id] = 2;
                m_union2.unite(e0_x, e0_y);
                m_count[2]++;
                m_count[0]--;

                DBG(graphviz(g, *this));
            }
#endif
            // apply labeling algorithm
            else
            {
                DBGG("BFS root node x = " << e0_x);

                bfs_tree(1, e0_x);
                bfs_tree(2, e0_x);

                DBG(graphviz(g, *this));

                if (!bfs_augmenting_path(e0)) return;
            }

            id_type count[3] = { 0, 0, 0 };

            for (size_t ei = 0; ei < m_graph.total_edge(); ++ei)
            {
                if (g.edge_deleted(ei)) continue;
                count[m_color[ei]]++;
            }

            DBGG("number of colored edges: " << count[0] << " / " << count[1] << " / " << count[2]);

            assert(m_count[0] == count[0]);
            assert(m_count[1] == count[1]);
            assert(m_count[2] == count[2]);

            if (m_count[1] == g.num_vertex() - 1 &&
                m_count[2] == g.num_vertex() - 1)
                break;
        }

        DBGG("resulting number of colored edges: " << m_count[0] << " / " << m_count[1] << " / " << m_count[2]);

        DBG(graphviz(g, *this));
    }

    bool good()
    {
        return (m_graph.num_edge() == 2 * m_graph.num_vertex() - 2 &&
                m_count[1] == m_graph.num_vertex() - 1 &&
                m_count[2] == m_graph.num_vertex() - 1);
    }

    void write(GraphType& g)
    {
        assert(g.total_edge() == m_color.size());

        for (size_t ei = 0; ei < m_color.size(); ++ei)
        {
            if (g.edge_deleted(ei)) continue;
            g.edge(ei)->color = (m_color[ei] - 1);
        }
    }

    bool test()
    {
        bfs_tree(1, 0);

        for (unsigned int v = 0; v < m_pred1.size(); ++v) {
            if (m_pred1[v] < 0) {
                std::cerr << "Vertex " << v << " is disconnected in tree 1.\n";
                return false;
            }
        }

        bfs_tree(2, 0);

        for (unsigned int v = 0; v < m_pred2.size(); ++v) {
            if (m_pred2[v] < 0) {
                std::cerr << "Vertex " << v << " is disconnected in tree 2.\n";
                return false;
            }
        }

        return true;
    }

    void graph(std::ostream& os, const GraphType&) const
    {
        os << "splines=false,K=1.6";
    }

    void vertex(std::ostream& os, const const_vertex_iterator& v) const
    {
        os << "label=\"" << v.vertex_id()
           << "/" << m_union1.find(v.vertex_id())
           << "." << m_union2.find(v.vertex_id())
           << "/" << m_pred1[v.vertex_id()]
           << "." << m_pred2[v.vertex_id()]
           << "\"";
    }

    void edge(std::ostream& os, const const_edge_iterator& e) const
    {
        id_type eid = e.edge_id();

        if (m_color[eid] == 1)
            os << "color=\"blue\"";
        else if (m_color[eid] == 2)
            os << "color=\"red\"";
        else
            os << "color=\"black\"";

        os << ",label=\"e" << e.edge_id() << "\"";
    }
};

#endif // !BISPANNING_ALG_BISPANNING_HEADER

/******************************************************************************/
