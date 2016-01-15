
/*******************************************************************************
 * src/bispanning.h
 *
 * Base graph class to represent bispanning graphs.
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

#ifndef BISPANNING_BISPANNING_HEADER
#define BISPANNING_BISPANNING_HEADER

#include <queue>
#include <set>
#include <stack>

#include "combinatorics.h"
#include "debug.h"
#include "graph.h"
#include "graph6.h"

// *** Construct BaseGraph for bispanning graphs

enum { BLUE = 0, RED = 1 };

struct VertexT
{ };

struct EdgeT
{
    int  color;

    bool dashed;
    int  original;

    explicit EdgeT(int c = 0)
        : color(c), dashed(false), original(-1) { }
};

template <bool Deleteable>
class BaseGraphGeneric : public GraphBase<
                             VertexT, EdgeT,
                             /* directed */ false,
                             /* allow_parallel */ true,
                             /* allow_loops */ false,
                             /* allow_deletes */ Deleteable>
{
public:
    using Super = GraphBase<VertexT, EdgeT, false, true, false, Deleteable>;

    using deletable_graph_type = GraphBase<VertexT, EdgeT, false, true, false, true>;

    explicit BaseGraphGeneric(unsigned int num_vertex = 0)
        : Super(num_vertex)
    { }

    // copy-construction from an other BaseGraph (editable or not)
    template <bool OtherDeletable>
    explicit BaseGraphGeneric(const BaseGraphGeneric<OtherDeletable>& other)
        : Super(Super::copy_from(other)),
          m_title(other.m_title)
    { }

    using typename Super::id_type;
    using typename Super::vertex_iterator;
    using typename Super::const_vertex_iterator;
    using typename Super::edge_iterator;
    using typename Super::const_edge_iterator;
    using typename Super::vertex_edge_iterator;
    using typename Super::const_vertex_edge_iterator;

    using Super::num_vertex;
    using Super::num_edge;
    using Super::total_vertex;
    using Super::total_edge;

    using Super::vertex_begin;
    using Super::vertex_end;
    using Super::vertex_list;

    using Super::edge_begin;
    using Super::edge_end;
    using Super::edge_list;

    using Super::vertex_edge_begin;
    using Super::vertex_edge_end;
    using Super::vertex_edge_list;

    using Super::has_deleted;
    using Super::contains;

    using Super::find_path;
    using Super::is_cyclic;
    using Super::is_connected;

    /**************************************************************************/

    //! Graphviz decorator for bispanning graphs
    class GraphDecoratorA : public GraphDecorator<Super>
    {
    public:
        void graph(std::ostream& os, const BaseGraphGeneric&) const
        {
            os << "splines=false,K=2.6";
        }

        void vertex(std::ostream& os, const const_vertex_iterator& v) const
        {
            os << "label=\"v" << v.vertex_id() << "\"";
        }

        void vertex_label(std::ostream& os, const const_vertex_iterator& v) const
        {
            os << v.vertex_id();
        }

        static constexpr const char* cBlue = "cyan1";
        static constexpr const char* cRed = "red3";
        static const int penwidth = 2;

        void edge(std::ostream& os, const const_edge_iterator& e) const
        {
            if (e->color == BLUE)
                os << "color=\"" << cBlue << "\",";
            else if (e->color == RED)
                os << "color=\"" << cRed << "\",";

            os << "penwidth=" << penwidth << ",";

            if (e->dashed)
                os << "style=\"dashed\",";

            os << "label=\"e" << e.edge_id();
            if (e->original >= 0) os << " (" << e->original << ")";
            os << "\"";
        }

        void edge_label(std::ostream& os, const const_edge_iterator& e) const
        {
            os << e.edge_id()
               << (e->color == RED ? " red" : e->color == BLUE ? " blue" : "");
        }

        void extra(std::ostream& os, const BaseGraphGeneric& g) const
        {
            if (g.m_title.size())
            {
                os << "label=\"" << g.m_title << "\";\n"
                   << "labelloc=\"t\";\n";
            }
            else
            {
                os << "label=\""
                   << write_graph6(g.template clone_reorder_degree<BaseGraphGeneric>())
                   << "\";\n"
                   << "labelloc=\"t\";\n";
            }
        }
    };

    GraphvizOutput<BaseGraphGeneric, GraphDecoratorA> graphviz() const
    {
        return ::graphviz(*this, GraphDecoratorA());
    }

    /**************************************************************************/

    //! GraphString decorator for base bispanning graphs
    class GraphDecoratorS : public GraphDecorator<Super>
    {
    public:
        bool vertex_short(const BaseGraphGeneric& g) const
        {
            return (g.num_vertex() == g.total_vertex());
        }

        void edge(std::ostream& os, const const_edge_iterator& e) const
        {
            os << 'c' << e->color;
        }
    };

    GraphStringOutput<BaseGraphGeneric, GraphDecoratorS> graphstring() const
    {
        return ::graphstring(*this, GraphDecoratorS());
    }

    /**************************************************************************/

    //! Output graph in a JSON format for JavaScript
    void output_json(std::ostream& os)
    {
        using vertex = const_vertex_iterator;
        using edge = const_edge_iterator;

        os << "var data = {\n";

        os << "  \"nodes\" : [\n";
        for (vertex v : vertex_list())
        {
            os << (v == vertex_begin() ? "" : ",\n");
            os << "    { \"name\": \"" << v.vertex_id() << "\" }";
        }
        os << "\n"
           << "  ],\n";

        os << "  \"links\" : [\n";
        for (edge e : edge_list())
        {
            os << (e == edge_begin() ? "" : ",\n");

            os << "    { \"source\": " << e.tail_id() << ", \"target\": " << e.head_id()
               << ", \"color\": \"" << (e->color == RED ? "red" : e->color == BLUE ? "blue" : "black") << "\""
               << " }";
        }
        os << "\n"
           << "  ]\n";

        os << "}\n";
    }

    /**************************************************************************/

    //! Filter for generic graph algorithms to select only one of the colored
    //! trees
    class ColorFilter : public Super::DefaultFilter
    {
    public:
        //! selected tree
        int color;

        //! set up color filter for a tree
        explicit ColorFilter(int c)
            : color(c)
        { }

        //! ignore all edges not in the colored tree
        bool filter_edge(const const_edge_iterator& e) const
        {
            return (e->color != color);
        }
    };

    /**************************************************************************/

    //! test if graph is a correctly colored bispanning graph
    bool is_bispanning_colored()
    {
        if (num_edge() != 2 * num_vertex() - 2) return false;

        if (!is_connected(ColorFilter(RED))) return false;
        if (!is_connected(ColorFilter(BLUE))) return false;

        return true;
    }

    template <typename VertexS, typename VertexT>
    std::vector<edge_iterator>
    find_color_path(const VertexS& s, const VertexT& t, int color)
    {
        return Super::find_path(s, t, ColorFilter(color));
    }

    template <typename VertexS, typename VertexT>
    std::vector<const_edge_iterator>
    find_color_path(const VertexS& s, const VertexT& t, int color) const
    {
        return Super::find_path(s, t, ColorFilter(color));
    }

    /**************************************************************************/

    //! Calculate cut by performing two BFS to find the cut edges
    std::vector<edge_iterator>
    calc_cut_edges(const id_type& vseed1,
                   const id_type& vseed2,
                   int color,
                   std::vector<int>* edgecut_mark = nullptr)
    {
        using edge = edge_iterator;
        using vertex_edge = const_vertex_edge_iterator;

        // mark both sides of cut
        std::vector<int> mark(total_vertex(), -1);

        // BFS queue
        std::queue<id_type> queue;

        // Initialize queue with first node
        queue.push(vseed1);
        mark[vseed1] = 1;

        while (!queue.empty())
        {
            id_type v = queue.front();
            queue.pop();

            for (vertex_edge ei : vertex_edge_list(v))
            {
                if (ei->color == color) continue;

                id_type w = ei.other_id(v);

                if (mark[w] != -1) // vertex already seen
                    continue;

                queue.push(w);
                mark[w] = 1;
            }
        }

        // Test that other end was not marked
        ASSERT(mark[vseed2] == -1);

        // Initialize queue with tail node
        queue.push(vseed2);
        mark[vseed2] = 2;

        while (!queue.empty())
        {
            id_type v = queue.front();
            queue.pop();

            for (vertex_edge ei : vertex_edge_list(v))
            {
                if (ei->color == color) continue;

                id_type w = ei.other_id(v);

                if (mark[w] != -1) // vertex already seen
                    continue;

                queue.push(w);
                mark[w] = 2;
            }
        }

        if (edgecut_mark)
            *edgecut_mark = mark;

        // Find cut edges
        std::vector<edge> cut;

        for (edge e : edge_list())
        {
            if (mark[e.tail_id()] != mark[e.head_id()])
            {
                assert(e->color == color);
                cut.push_back(e);
            }
        }

        return cut;
    }

    //! Calculate cut by performing two BFS to find the cut edges
    std::vector<edge_iterator>
    calc_cut_edges(const edge_iterator& bridge)
    {
        assert(contains(bridge));
        return calc_cut_edges(bridge.tail_id(), bridge.head_id(), bridge->color);
    }

    //! Calculate (unique) exchange edges / breaker edges for given edge
    std::vector<edge_iterator>
    calc_exchange_edges(edge_iterator& eflip)
    {
        static const bool debug = false;
        using id_type = id_type;
        using edge = edge_iterator;

        if (eflip->color == BLUE)
        {
            DBGG("maker colors edge " << eflip << " red (color 1)");

            // find cycle in maker tree (color RED)
            std::vector<edge> path =
                find_color_path(eflip.head(), eflip.tail(), RED);

            eflip->color = RED;

            std::vector<edge> validbreaker;

            // check each edge in path for breaking edge
            for (size_t i = 0; i < path.size(); ++i)
            {
                edge ebreak = path[i];
                assert(ebreak != edge_end());
                assert(ebreak->color == RED);

                DBGG("break cycle with blue at " << ebreak << ".");

                ebreak->color = BLUE;

                if (is_cyclic(ColorFilter(BLUE)))
                {
                    DBGG("cycle.");
                }
                else
                {
                    DBGG("NO cycle in breaker blue.");
                    validbreaker.push_back(ebreak);
                }

                ebreak->color = RED;
            }

            DBGG("maker forced: " << eflip << " -> " << validbreaker);

            eflip->color = BLUE;

            return validbreaker;
        }
        else if (eflip->color == RED)
        {
            DBGG("maker colors edge " << eflip << " blue");

            // find cycle in maker tree (color BLUE)
            std::vector<edge> path =
                find_color_path(eflip.head(), eflip.tail(), BLUE);

            eflip->color = BLUE;

            std::vector<edge> validbreaker;

            // check each edge in path for breaking edge
            for (size_t i = 0; i < path.size(); ++i)
            {
                edge ebreak = path[i];
                assert(ebreak != edge_end());
                assert(ebreak->color == BLUE);

                DBGG("break cycle with red at " << ebreak << ".");

                ebreak->color = RED;

                if (is_cyclic(ColorFilter(RED)))
                {
                    DBGG("cycle.");
                }
                else
                {
                    DBGG("NO cycle in red.");
                    validbreaker.push_back(ebreak);
                }

                ebreak->color = BLUE;
            }

            DBGG("maker forced: " << eflip << " -> " << validbreaker);

            eflip->color = RED;

            return validbreaker;
        }
        else {
            assert(0);
            abort();
        }
    }

    /**************************************************************************/

    //! Tests if the graph is bispanning using Nash-Williams' theorem
    bool is_atomic_bispanner() const
    {
        using vertex = const_vertex_iterator;
        using edge = const_edge_iterator;

        if (has_deleted())
        {
            // make a mapping V -> {0,...,n-1}
            std::vector<id_type> vmap(total_vertex());

            id_type i = 0;
            for (vertex v : vertex_list())
                vmap[v.vertex_id()] = i++;

            return enumerate_set_partitions(
                num_vertex(),
                [&](const std::vector<size_t>& setp)
                {
                    size_t np = *std::max_element(setp.begin(), setp.end()) + 1;

                    // skip trivial partitions
                    if (np == 1 || np == num_vertex()) return true;

                    unsigned int cross = 0;

                    for (edge e : edge_list())
                    {
                        id_type v = e.tail_id(), w = e.head_id();

                        if (setp[vmap[v]] != setp[vmap[w]])
                            ++cross;
                    }

                    if (cross == 2 * (np - 1)) // composite
                        return false;

                    return true;
                });
        }
        else
        {
            return enumerate_set_partitions(
                num_vertex(),
                [&](const std::vector<size_t>& setp)
                {
                    size_t np = *std::max_element(setp.begin(), setp.end()) + 1;

                    // skip trivial partitions
                    if (np == 1 || np == num_vertex()) return true;

                    unsigned int cross = 0;

                    for (edge e : edge_list())
                    {
                        id_type v = e.tail_id(), w = e.head_id();

                        if (setp[v] != setp[w])
                            ++cross;
                    }

                    if (cross == 2 * (np - 1)) // composite
                        return false;

                    return true;
                });
        }
    }

    //! Tests if the graph is bispanning using Tutte's theorem
    bool is_atomic_bispanner_tutte(std::vector<unsigned int>* _cutset = nullptr) const
    {
        using graph_type = deletable_graph_type;

        using id_type = graph_type::id_type;
        using vertex = graph_type::vertex_iterator;
        using edge = graph_type::edge_iterator;

        if (has_deleted()) abort();

        std::vector<unsigned int> cutset(num_edge(), 0);

        // intentionally skip the first [0,0,0,...,0] vector
        while (increment_boolean_vector(cutset))
        {
            graph_type g = graph_type::copy_from(*this);

            unsigned int cutsize = 0;

            for (unsigned int i = 0; i < cutset.size(); ++i)
            {
                if (cutset[i]) {
                    g.delete_edge(i);
                    ++cutsize;
                }
            }

            // all edges removed?
            if (cutsize == num_edge()) continue;

            // std::cerr << cutset << " - " << g.get_num_components_undirected() << "\n";

            unsigned int comp = g.get_num_components_undirected();

            if (2 * (comp - 1) == cutsize)
            {
                // std::cerr << cutset << " - " << g.get_num_components_undirected() << "\n";

                if (_cutset) *_cutset = cutset;
                return false;
            }
        }

        return true;
    }

    //! returns true if there is a non-trivial 3 edge cut (which is not just a
    //! vertex of degree 3)
    bool test_nontrivial_3edge_cuts() const
    {
        using graph_type = deletable_graph_type;

        using id_type = graph_type::id_type;
        using vertex = graph_type::vertex_iterator;
        using edge = graph_type::edge_iterator;

        static const bool debug = true;

        std::vector<unsigned int> cutset(num_edge(), 0);

        // set three edge to delete
        cutset[0] = cutset[1] = cutset[2] = 1;

        bool found = false;

        do
        {
            graph_type g = graph_type::copy_from(*this);

            id_type ecut[3];

            for (unsigned int i = 0, j = 0; i < cutset.size(); ++i)
            {
                if (cutset[i]) {
                    g.delete_edge(i);
                    ecut[j++] = i;
                }
            }

            std::vector<id_type> comp = g.get_component_sizes_undirected();
            if (comp.size() == 1) continue;

            if (*std::min_element(comp.begin(), comp.end()) == 1) continue;

            found = true;

            DBGG(cutset << " - " << comp);

            DBGG(" e0 = " << this->edge(ecut[0]) <<
                 " e1 = " << this->edge(ecut[1]) <<
                 " e2 = " << this->edge(ecut[2]));
        }
        while (std::prev_permutation(cutset.begin(), cutset.end()));

        return found;
    }

public:
    std::string m_title;
};

using BaseGraph = BaseGraphGeneric</* deletable */ false>;
using EditBaseGraph = BaseGraphGeneric</* deletable */ true>;

template <typename BaseGraph>
std::vector<typename BaseGraph::id_type>
edge2idlist(const std::vector<typename BaseGraph::edge_iterator>& el)
{
    std::vector<typename BaseGraph::id_type> idlist;
    for (const auto& i : el)
    {
        idlist.push_back(i.edge_id());
    }
    return idlist;
}

////////////////////////////////////////////////////////////////////////////////

struct UEVertexT
{
    int color;
};

struct UEEdgeT
{
    int color;

    explicit UEEdgeT(int c = 0)
        : color(c)
    { }
};

using UEGraphType = GraphBase<UEVertexT, UEEdgeT, true, true, true, false>;

//! Graphviz decorator for bispanning graphs
struct GraphDecoratorUE : public GraphDecorator<UEGraphType>
{
    void graph(std::ostream& os, const UEGraphType&) const
    {
        os << "splines=false,K=1.6";
    }

    void vertex(std::ostream& os, const vertex_iterator& v) const
    {
        os << "label=\"e" << v.vertex_id() << "\"";

        if (v->color == BLUE)
            os << ",color=\"blue\"";
        else if (v->color == RED)
            os << ",color=\"red\"";
    }

    void edge(std::ostream& os, const edge_iterator& e) const
    {
        if (e->color == 1)
            os << "color=\"blue\",";
        else if (e->color == 2)
            os << "color=\"red\",";
        else
            os << "color=\"black\",";
    }
};

//! Construct UEGraph from a colored bispanning graph
UEGraphType make_uegraph(BaseGraph g)
{
    // static const bool debug = true;

    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::edge_iterator;

    UEGraphType uegraph(g.total_edge());

    for (edge e : g.edge_list())
    {
        uegraph.vertex(e.edge_id())->color = e->color;
    }

    for (edge eflip : g.edge_list())
    {
        std::vector<edge> validbreaker = g.calc_exchange_edges(eflip);

        if (validbreaker.size() == 1) // unique breaker edge -> unique exchange
        {
            uegraph.add_edge(eflip.edge_id(), validbreaker.front().edge_id(),
                             UEEdgeT(eflip->color + 1));
#if 0
            // test whether UEs are always inzident -> NO
            id_type v1 = eflip.tail_id(), v2 = eflip.head_id();
            id_type w1 = g.edge(validbreaker.front()).tail_id();
            id_type w2 = g.edge(validbreaker.front()).head_id();

            assert(v1 == w1 || v1 == w2 ||
                   v2 == w1 || v2 == w2);
#endif
        }
    }

    return uegraph;
}

////////////////////////////////////////////////////////////////////////////////

using uepair_type = std::pair<BaseGraph::id_type, BaseGraph::id_type>;
using uestack_type = std::stack<uepair_type>;

uestack_type g_uestack;

std::map<BaseGraph::id_type, BaseGraph::id_type> g_esplitmap;

void genbig_double_attach(BaseGraph& g, BaseGraph::id_type vn, BaseGraph::id_type v1, BaseGraph::id_type v2)
{
    static const bool debug = true;

    DBGG("double_attach to " << v1 << " and " << v2);

    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::edge_iterator;

    // add double edges
    edge e0 = g.add_edge(vn, v1, EdgeT(0));
    edge e1 = g.add_edge(vn, v2, EdgeT(1));

    g_esplitmap[e0.edge_id()] = e0.edge_id();
    g_esplitmap[e1.edge_id()] = e1.edge_id();

    g_uestack.push(uepair_type(e0.edge_id(), e1.edge_id()));

    g.m_title += "da(" + to_str(v1) + "," + to_str(v2) + "),";
}

void genbig_split_edge(BaseGraph& g, BaseGraph::id_type vn, BaseGraph::edge_iterator e_split, BaseGraph::vertex_iterator v_attach)
{
    static const bool debug = true;

    DBGG("split_edge " << e_split << " attach to " << v_attach);

    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::edge_iterator;

    id_type v1 = e_split.tail_id(), v2 = e_split.head_id();

    // copy edge properties
    EdgeT split = *e_split;

    int ei_split = e_split.edge_id();
    int split_other_color = split.color == 0 ? 1 : 0;

    // delete edge!!!

    g.delete_edge(e_split);

    // add two new edges

    edge e1 = g.add_edge(v1, vn, EdgeT(split.color));
    edge e2 = g.add_edge(v2, vn, EdgeT(split.color));

    // find attachment

    id_type vi_attach = v_attach.vertex_id();

    // find path from other node to vn (via e1 or e2)
    std::vector<edge> path = g.find_color_path(vn, vi_attach, split.color);
    ASSERT(path[0] == e1 || path[0] == e2);

    // attach v_attach to split center
    edge e0 = g.add_edge(vn, vi_attach, EdgeT(split_other_color));

    // select cycle and non-cycle edges
    edge& e_cycle = path[0];
    edge& e_noncycle = (path[0] == e1 ? e2 : e1);

    g_esplitmap[ei_split] = e_noncycle.edge_id();
    g_esplitmap[e_noncycle.edge_id()] = e_noncycle.edge_id();

    g_esplitmap[e_cycle.edge_id()] = e_cycle.edge_id();
    g_esplitmap[e0.edge_id()] = e0.edge_id();

    g_uestack.push(uepair_type(e0.edge_id(), e_cycle.edge_id()));

    g.m_title += "es(" + to_str(ei_split) + "," + to_str(vi_attach) + "),";
}

//! Generate a random bispanning graph using double-edge additions and edge-splits
BaseGraph generate_random_bispanning(unsigned int num_vertex, unsigned int* seed)
{
    static const bool debug = false;

    g_uestack = uestack_type();
    g_esplitmap.clear();

    BaseGraph g(1);

    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::vertex_iterator;
    using edge = BaseGraph::edge_iterator;

    for (unsigned int vn = 1; vn < num_vertex; ++vn)
    {
        g.add_vertex();
        DBG(g.graphviz());

        int ch = rand_r(seed) % 2;

        if (ch == 0 || g.num_edge() == 0)
        {
            // join double edge

            BaseGraph::id_type v1 = rand_r(seed) % vn;
            BaseGraph::id_type v2;
            do {
                v2 = rand_r(seed) % vn;
            } while (!g.allow_parallel && v1 == v2);

            genbig_double_attach(g, vn, v1, v2);
        }
        else // split edge
        {
            // select any edge

            id_type ei_split;

            do {
                ei_split = rand_r(seed) % g.num_edge();
            } while (g.edge_deleted(ei_split));

            edge e_split = g.edge(ei_split);

            vertex v1 = e_split.tail(), v2 = e_split.head();

            // select another node (possibly different from v1,v2)

            BaseGraph::vertex_iterator v_attach = g.vertex_end();
            do {
                v_attach = g.vertex(rand_r(seed) % vn);
            } while (!g.allow_parallel &&
                     (v_attach == v1 || v_attach == v2));

            genbig_split_edge(g, vn, e_split, v_attach);
        }

        assert(g.num_edge() == 2 * g.num_vertex() - 2);

        DBG(g.graphviz());
        DBG(graphviz(make_uegraph(g), GraphDecoratorUE()));
    }

    assert(g.num_vertex() == num_vertex &&
           g.num_edge() == 2 * num_vertex - 2);

    return g;
}

BaseGraph generate_specific_bispanning(unsigned int num_vertex, unsigned int* seed)
{
    BaseGraph g(1);

    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::edge_iterator;

    for (unsigned int vn = 1; vn < num_vertex; ++vn)
    {
        g.add_vertex();

        switch (vn)
        {
        /*
                case 1:
                    genbig_double_attach(g, vn, 0, 0);
                    break;

                case 2:
                    genbig_double_attach(g, vn, 0, 1);
                    break;

                case 3:
                    genbig_double_attach(g, vn, 1, 0);
                    break;

                case 4:
                    genbig_double_attach(g, vn, 2, 1);
                    break;

                case 5:
                    genbig_double_attach(g, vn, 3, 2);
                    break;

                case 6:
                    genbig_double_attach(g, vn, 1, 4);
                    break;
        */
        case 1: case 2: case 3: case 4: case 5:
        {
            int i = rand_r(seed) % (g.num_vertex() - 1);
            int j = rand_r(seed) % (g.num_vertex() - 1);

            genbig_double_attach(g, vn, i, j);
            // DBG( g.graphviz() );
            break;
        }

        case 6: case 7: {
            int e = rand_r(seed) % g.num_edge();
            int t = rand_r(seed) % (g.num_vertex() - 1);

            // genbig_split_edge(g, vn, g.edge(10), g.vertex(5));
            genbig_split_edge(g, vn, g.edge(e), g.vertex(t));

            // DBG( g.graphviz() );

            break;
        }

        case 8:
            // genbig_split_edge(g, vn, g.edge(2), g.vertex(5));
            break;

        default:
            break;
        }

        // DBG( g.graphviz() );
        // DBG( graphviz(make_uegraph(g), GraphDecoratorUE()) );
    }

    return g;
}

////////////////////////////////////////////////////////////////////////////////

//! Recursively reconstruct a bispanning graph

void calc_reconstruct_bispanning(const BaseGraph& gorig)
{
    static const bool debug = true;

    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::vertex_iterator;
    using edge = BaseGraph::edge_iterator;

    BaseGraph g = gorig;

    while (g.num_vertex() > 1)
    {
        std::cout << g.graphviz() << std::endl;

        for (vertex v : g.vertex_list())
        {
            if (v.degree() == 2)
            {
                DBGG("remove " << v << " with deg(v) = " << v.degree());

                // find the two differently colored edges
                edge e0 = v.edge(0);
                edge e1 = v.edge(1);

                ASSERT(e0->color != e1->color);

                g.delete_vertex(v);
                break;
            }
            else if (v.degree() == 3)
            {
                DBGG("remove " << v << " with deg(v) = " << v.degree());

                // find two equally colored edges
                edge e0 = v.edge(0);
                edge e1 = v.edge(1);
                edge e2 = v.edge(2);

                ASSERT(e0->color + e1->color + e2->color == 1 ||
                       e0->color + e1->color + e2->color == 2);

                if (e0->color + e1->color + e2->color == 1)
                {
                    // two are color 0 and one is color 1

                    if (e0->color == 1 && e1->color == 0)
                        std::swap(e0, e1);
                    if (e0->color == 1 && e2->color == 0)
                        std::swap(e0, e2);
                    if (e1->color == 1 && e2->color == 0)
                        std::swap(e1, e2);
                }
                else if (e0->color + e1->color + e2->color == 2)
                {
                    // two are color 1 and one is color 0

                    if (e0->color == 0 && e1->color == 1)
                        std::swap(e0, e1);
                    if (e0->color == 0 && e2->color == 1)
                        std::swap(e0, e2);
                    if (e1->color == 0 && e2->color == 1)
                        std::swap(e1, e2);
                }
                else
                    abort();

                ASSERT(e0->color == e1->color);
                ASSERT(e1->color != e2->color);

                vertex v0 = e0.other(v);
                vertex v1 = e1.other(v);

                g.add_edge(v0, v1, EdgeT(e0->color));

                g.delete_vertex(v);
                break;
            }
        }
    }

    std::cout << g.graphviz() << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

using id_vector_type = std::vector<BaseGraph::id_type>;

struct tree_pair_type
{
    bool           valid;
    id_vector_type t0, t1;

    explicit tree_pair_type(bool _valid)
        : valid(_valid)
    { }

    tree_pair_type(const id_vector_type& _t0 = id_vector_type(),
                   const id_vector_type& _t1 = id_vector_type())
        : valid(true), t0(_t0), t1(_t1)
    { }

    size_t         size() const
    {
        return t0.size() + t1.size();
    }

    const BaseGraph::id_type& operator [] (size_t i) const
    {
        i %= size();
        if (i < t0.size()) return t0[i];
        else return t1[i - t0.size()];
    }
};

template <bool debug = false>
bool test_cyclic_base_ordering(const BaseGraph& gorig, const tree_pair_type& trees, bool testUE = false)
{
    // static const bool debug = false;

    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::edge_iterator;

    assert(trees.t0.size() == trees.t1.size());
    assert(trees.t0.size() + trees.t1.size() == gorig.num_edge());

    DBGG("tree0: " << trees.t0 << " - tree1: " << trees.t1);

    BaseGraph g = gorig;

    for (size_t ti = 0; ti < trees.t0.size(); ++ti)
    {
        // recolor graph
        for (edge ei : g.edge_list())
            ei->color = 42;

        for (id_type i = 0; i < trees.t0.size(); ++i)
            g.edge(trees[ti + i])->color = 0;

        for (id_type i = trees.t0.size(); i < trees.size(); ++i)
            g.edge(trees[ti + i])->color = 1;

        for (edge ei : g.edge_list())
            assert(ei->color == 0 || ei->color == 1);

        // test for cycles
        if (g.is_cyclic(BaseGraph::ColorFilter(0))) {
            DBGG("// ERROR: cycle in tree0");
            // DBG(g.graphviz());
            return false;
        }

        if (g.is_cyclic(BaseGraph::ColorFilter(1))) {
            DBGG("// ERROR: cycle in tree1");
            // DBG(g.graphviz());
            return false;
        }

        // test next edge swaps for unique exchange
        edge swap0 = g.edge(trees[ti]);
        edge swap1 = g.edge(trees[ti + trees.t0.size()]);

        auto swapEx0 = g.calc_exchange_edges(swap0);
        auto swapEx1 = g.calc_exchange_edges(swap1);

        DBGG("swap0 " << swap0 << " is " << (swapEx0.size() == 1 ? "" : "NOT ") << "a UE, "
             "swap1 " << swap1 << " is " << (swapEx1.size() == 1 ? "" : "NOT ") << "a UE");

        if (swapEx0.size() == 1 && swapEx0[0] != swap1) {
            DBGG("swap0 does not match swap1!!!");
        }
        if (swapEx1.size() == 1 && swapEx1[0] != swap0) {
            DBGG("swap1 does not match swap0!!!");
        }

        if (!(swapEx0.size() == 1 || swapEx1.size() == 1)) {
            DBGG("No UE swap found!!!");
            if (testUE) return false;
            // abort();
        }

        // assert(swapEx1[0] == swap0);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

//! Calculate a cyclic base ordering for the given bispanning graph
tree_pair_type calc_cyclic_base_ordering(const BaseGraph& g, size_t cbonum)
{
    static const bool debug = false;

    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::const_vertex_iterator;
    using edge = BaseGraph::const_edge_iterator;

    // std::cout << g.graphviz() << std::endl;

    // test for cycles
    if (g.is_cyclic(BaseGraph::ColorFilter(0))) {
        DBGGE("// ERROR: cycle in tree0");
        std::cout << g.graphviz() << std::endl;
        abort();
    }

    if (g.is_cyclic(BaseGraph::ColorFilter(1))) {
        DBGGE("// ERROR: cycle in tree1");
        std::cout << g.graphviz() << std::endl;
        abort();
    }

    if (g.num_vertex() == 1) {
        if (cbonum == 0) // only one valid CBO
            return tree_pair_type();
        else
            return tree_pair_type(false);
    }

    // for (vertex v = g.vertex_begin(); v != g.vertex_end(); ++v)
    for (id_type vi = 0; vi < g.total_vertex(); ++vi)
    {
        id_type vj = g.total_vertex() - 1 - vi;
        if (g.vertex_deleted(vj)) continue;

        vertex v = g.vertex(vj);

        if (v.degree() == 2)
        {
            DBGG("processing " << v << " with deg(v) = " << v.degree());

            // find the two differently colored edges
            edge e0 = v.edge(0);
            edge e1 = v.edge(1);

            assert(e0->color != e1->color);

            // switch e0 to be color 0
            if (e0->color == 1)
                std::swap(e0, e1);

            // recursively construct cyclic base ordering
            for (size_t subcbo = 0; ; ++subcbo)
            {
                BaseGraph gr = g;
                gr.delete_vertex(v.vertex_id());

                tree_pair_type trees = calc_cyclic_base_ordering(gr, subcbo);
                if (!trees.valid) break;

                if (0)
                {
                    // trees.t0.insert( trees.t0.begin(), e0.edge_id() );
                    // trees.t1.insert( trees.t1.begin(), e1.edge_id() );

                    trees.t0.push_back(e0.edge_id());
                    trees.t1.push_back(e1.edge_id());

                    if (test_cyclic_base_ordering(g, trees))
                        return trees;
                }
                else
                {
                    if (cbonum < trees.t0.size() + 1)
                    {
                        tree_pair_type treex = trees;

                        treex.t0.insert(treex.t0.begin() + cbonum, e0.edge_id());
                        treex.t1.insert(treex.t1.begin() + cbonum, e1.edge_id());

                        if (test_cyclic_base_ordering(g, treex))
                            return treex;
                    }
                    cbonum -= trees.t0.size() + 1;
                }
            }
        }
        else if (v.degree() == 3)
        {
            DBGG("progressing " << v << " with deg(v) = " << v.degree());

            // find two equally colored edges
            edge e0 = v.edge(0);
            edge e1 = v.edge(1);
            edge e2 = v.edge(2);

            assert(e0->color + e1->color + e2->color == 1 ||
                   e0->color + e1->color + e2->color == 2);

            if (e0->color + e1->color + e2->color == 1)
            {
                // two are color 0 and one is color 1

                if (e0->color == 1 && e1->color == 0)
                    std::swap(e0, e1);
                if (e0->color == 1 && e2->color == 0)
                    std::swap(e0, e2);
                if (e1->color == 1 && e2->color == 0)
                    std::swap(e1, e2);
            }
            else if (e0->color + e1->color + e2->color == 2)
            {
                // two are color 1 and one is color 0

                if (e0->color == 0 && e1->color == 1)
                    std::swap(e0, e1);
                if (e0->color == 0 && e2->color == 1)
                    std::swap(e0, e2);
                if (e1->color == 0 && e2->color == 1)
                    std::swap(e1, e2);
            }
            else
                abort();

            assert(e0->color == e1->color);
            assert(e1->color != e2->color);

            vertex v0 = e0.other(v);
            vertex v1 = e1.other(v);

            // recursively construct cyclic base ordering
            for (size_t subcbo = 0; ; ++subcbo)
            {
                BaseGraph gr = g;
                gr.delete_vertex(v.vertex_id());
                edge e_split = gr.add_edge(v0.vertex_id(), v1.vertex_id(), EdgeT(e0->color));

                tree_pair_type trees = calc_cyclic_base_ordering(gr, subcbo);
                if (!trees.valid) break;

                // std::cout << graphviz(gr, GraphDecoratorA()) << std::endl;

                DBGG("progressing " << v << " with deg(v) = " << v.degree());

                DBGG("added split edge: " << e_split);

                // treeA is the one with the split edge, tree B the other
                id_vector_type& treeA = (e_split->color == 0) ? trees.t0 : trees.t1;
                id_vector_type& treeB = (e_split->color == 0) ? trees.t1 : trees.t0;

                // find split edge in treeA:
                id_vector_type::iterator esiter = std::find(treeA.begin(), treeA.end(), e_split.edge_id());
                assert(esiter != treeA.end());

                int espos = esiter - treeA.begin();
                DBGG("Found split edge at position " << espos);

                // std::cout << g.graphviz() << std::endl;

                // find path from vertex to vertex z (= e2.other end)
                std::vector<edge> path = g.find_color_path(v, e2.other(v), e0->color);
                DBGG("path: " << path);
                assert(path[0] == e0 || path[0] == e1);

                edge e_cycle = path[0];
                edge e_noncycle = (path[0] == e0 ? e1 : e0);
                DBGG("cycle edge: " << e_cycle << " - non-cycle edge: " << e_noncycle);

                // insert e3 to treeB before the position corresponding to e_split
                treeB.insert(treeB.begin() + espos + 0, e2.edge_id());

                // replace e_split with [e_noncycle, e_cycle]
                assert(treeA[espos] == e_split.edge_id());
                treeA[espos] = e_noncycle.edge_id();
                treeA.insert(treeA.begin() + espos + 1, e_cycle.edge_id());

                if (test_cyclic_base_ordering(g, trees)) {
                    if (cbonum == 0)
                        return trees;
                    --cbonum;
                }

                if (0)
                {
                    // else try to switch v and z
                    std::swap(treeB[espos], treeB[espos + 1]);

                    if (test_cyclic_base_ordering(g, trees)) {
                        if (cbonum == 0)
                            return trees;
                        --cbonum;
                    }

                    // swap in treeA
                    std::swap(treeA[espos], treeA[espos + 1]);

                    if (test_cyclic_base_ordering(g, trees)) {
                        if (cbonum == 0)
                            return trees;
                        --cbonum;
                    }

                    std::swap(treeB[espos], treeB[espos + 1]);

                    if (test_cyclic_base_ordering(g, trees)) {
                        if (cbonum == 0)
                            return trees;
                        --cbonum;
                    }
                }

                // return tree_pair_type(false);
                // abort();
            }
        }
        // abort();
    }

    return tree_pair_type(false);

    assert(0);
    abort();
}

////////////////////////////////////////////////////////////////////////////////

bool bispanner_test_3cuts(const BaseGraph& _g)
{
    using graph_type = BaseGraph::deletable_graph_type;

    using id_type = graph_type::id_type;
    using vertex = graph_type::vertex_iterator;
    using edge = graph_type::edge_iterator;

    std::vector<unsigned int> cutset(_g.num_vertex(), 0);

    // set three vertices to delete
    cutset[0] = cutset[1] = cutset[2] = 1;

    do
    {
        graph_type g = graph_type::copy_from(_g);

        id_type vcut[3];

        for (unsigned int i = 0, j = 0; i < cutset.size(); ++i)
        {
            if (cutset[i]) {
                g.delete_vertex(i);
                vcut[j++] = i;
            }
        }

        unsigned int comp = g.get_num_components_undirected();
        if (comp == 1) continue;

        std::cout << cutset << " - " << comp << "\n";

        std::cout << " v0v1 = " << _g.count_edges(vcut[0], vcut[1])
                  << " v0v2 = " << _g.count_edges(vcut[0], vcut[2])
                  << " v1v2 = " << _g.count_edges(vcut[1], vcut[2])
                  << "\n";
    }
    while (std::prev_permutation(cutset.begin(), cutset.end()));

    std::cout << "done\n";

    return true;
}

template <typename BaseGraph>
std::string get_cycle_cut_listing(const BaseGraph& _g,
                                  const char* linebreak = "\\l")
{
    using edge = typename BaseGraph::edge_iterator;

    BaseGraph g = _g;

    std::ostringstream oss;

    for (edge& e : g.edge_list())
    {
        int color = e->color;
        int other_color = (color + 1) % 2;

        std::vector<edge> cycle =
            g.find_color_path(e.head(), e.tail(), other_color);
        cycle.push_back(e);
        std::sort(cycle.begin(), cycle.end());

        e->color = other_color;
        std::vector<edge> cut = g.calc_cut_edges(e);
        e->color = color;

        oss << e << " " << (color == BLUE ? "B" : "R")
            << " C " << edge2idlist<BaseGraph>(cycle)
            << " D " << edge2idlist<BaseGraph>(cut) << linebreak;
    }

    return oss.str();
}

#endif // !BISPANNING_BISPANNING_HEADER

/******************************************************************************/
