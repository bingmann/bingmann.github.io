/*******************************************************************************
 * src/alg_game.h
 *
 * Calculate complete bispanning exchange graph.
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

#ifndef BISPANNING_ALG_GAME_HEADER
#define BISPANNING_ALG_GAME_HEADER

#include "bispanning.h"
#include "graph_igraph.h"
//#include "graph_eigen.h"

#include <boost/functional/hash.hpp>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

// *****************************************************************************
// *** Methods to work with tree vectors of vertex ids

//! Tree type is a list of edges (of just ONE tree) with additional functions.
class Tree : public std::vector<BaseGraph::id_type>
{
public:
    // vector<ids>
    using super_type = std::vector<BaseGraph::id_type>;

    Tree() : super_type() { }

    explicit Tree(const std::vector<size_t>& vec)
        : super_type(vec.begin(), vec.end())
    { }

    //! test if the tree contains the id v
    bool contains(const value_type& v) const
    {
        return vector_contains(*this, v);
    }

    //! erase v from tree, returns the number of occurrences
    size_t erase_try(const value_type& v)
    {
        iterator it = std::remove(begin(), end(), v);
        size_t count = end() - it;
        super_type::erase(it, end());
        return count;
    }

    //! erase if v from tree. fails if it does not exist.
    void erase(const value_type& v)
    {
        size_t c = erase_try(v);
        ASSERT(c == 1);
    }

    //! insert an id into the tree.
    void insert_one(const value_type& v)
    {
        insert(std::lower_bound(begin(), end(), v), v);
    }

    //! toggle an id: remove if exists, add if not exists.
    void toggle(const value_type& v)
    {
        iterator it = std::lower_bound(begin(), end(), v);
        if (it != end() && *it == v)
            super_type::erase(it);
        else
            insert(it, v);
    }

    //! Colorize a graph with tree as RED tree
    template <typename BaseGraph>
    void colorize_graph(BaseGraph& g, int c = RED) const
    {
        const_iterator ti = begin();

        for (auto e : g.edge_list())
        {
            if (ti != end() && *ti == e.edge_id()) {
                e->color = c;
                ++ti;
            }
            else {
                e->color = 0;
            }
        }
    }

    //! Colorize a graph with tree, return a copy of the graph.
    template <typename BaseGraph>
    BaseGraph get_colorized_graph(const BaseGraph& g, int c = RED) const
    {
        BaseGraph gcopy = g;
        colorize_graph(gcopy, c);
        return gcopy;
    }
};

class TreeHash
{
public:
    size_t operator () (const Tree& t) const
    {
        return boost::hash_range(t.begin(), t.end());
    }
};

//! Extract the tree colored c1 or c2 from graph
template <typename BaseGraph>
static inline Tree extract_tree(const BaseGraph& g, int c1, int c2)
{
    Tree t;

    for (auto e : g.edge_list())
    {
        if (e->color == c1 || e->color == c2)
            t.push_back(e.edge_id());
    }

    return t;
}

//! Extract the tree colored c1 from graph
template <typename BaseGraph>
static inline Tree extract_tree(const BaseGraph& g, int c1)
{
    return extract_tree(g, c1, c1);
}

template <typename BaseGraph>
static inline Tree
tree_complement(const Tree& tree, const BaseGraph& bg)
{
    Tree comp;
    Tree::const_iterator t = tree.begin();

    for (size_t i = 0; i < bg.total_edge(); ++i)
    {
        if (bg.edge_deleted(i)) {
            assert(t == tree.end() || *t != i);
            continue;
        }

        if (t != tree.end() && *t == i)
            ++t;
        else
            comp.push_back(i);
    }

    // std::cout << "tree: " << tree << " - comp: " << comp << "\n";
    assert(tree.size() == comp.size());

    return comp;
}

//! Calculate tree A \ B
static inline Tree
tree_difference(const Tree& a, const Tree& b)
{
    Tree out;
    std::set_difference(a.begin(), a.end(),
                        b.begin(), b.end(), std::back_inserter(out));
    return out;
}

//! Calculate tree A \triangle B = (A \ B) \cup (B \ A)
static inline Tree
tree_symmetric_difference(const Tree& a, const Tree& b)
{
    Tree out;
    std::set_symmetric_difference(a.begin(), a.end(),
                                  b.begin(), b.end(), std::back_inserter(out));
    return out;
}

// *****************************************************************************
// *** Graph Tau of base pairs and exchanges

struct TauVertex
{
    Tree        tree;
    // float x,y;
    bool        seen;

    std::string label() const
    {
        // convert to signed integer
        std::vector<int> t(tree.begin(), tree.end());
        return vector_join(t, ',');
    }
};

struct TauEdge
{
    using id_type = BaseGraph::id_type;

    int     colors;
    bool    dashed, dotted;
    id_type UEin, UEout;

    TauEdge(int c, id_type ue_in, id_type ue_out)
        : colors(c), dashed(0), dotted(0),
          UEin(ue_in), UEout(ue_out)
    { }

    friend inline std::ostream& operator << (std::ostream& os, const TauEdge& te)
    {
        return (os << '<' << te.colors << '|' << te.UEin << "->" << te.UEout << '>');
    }
};

//! Graph class containing directed Tau graph

static const bool TauGraphDeleteable = true;
static const bool TauGraphDirected = false;

// using TauGraphBase =  Graph<TauVertex,TauEdge,TauGraphDirected> ;
using TauGraphBase = GraphBase<TauVertex, TauEdge, TauGraphDirected, true, false, TauGraphDeleteable>;

class TauGraph : public TauGraphBase
{
public:
    std::string m_title;

    using super_type = TauGraphBase;

    TauGraph()
        : super_type()
    { }

    explicit TauGraph(const super_type& gr)
        : super_type(gr)
    { }

    using treemap_type = std::unordered_map<Tree, id_type, TreeHash>;
    treemap_type m_treemap;

    void rebuild_treemap()
    {
        m_treemap.clear();
        for (const_vertex_iterator vi : vertex_list())
            m_treemap[vi->tree] = vi.vertex_id();
    }

    vertex_iterator find_vertex(const Tree& tree)
    {
        treemap_type::const_iterator mi = m_treemap.find(tree);
        if (mi == m_treemap.end()) return vertex_end();
        else return vertex(mi->second);
    }

    const_vertex_iterator find_vertex(const Tree& tree) const
    {
        treemap_type::const_iterator mi = m_treemap.find(tree);
        if (mi == m_treemap.end()) return vertex_end();
        else return vertex(mi->second);
    }

    vertex_iterator discover_vertex(const Tree& tree)
    {
        vertex_iterator iter = find_vertex(tree);

        if (iter == vertex_end())
        {
            // add new vertex to tau graph
            iter = add_vertex();
            iter->tree = tree;

            m_treemap[tree] = iter.vertex_id();
        }
        return iter;
    }

    bool edge_exists(const const_vertex_iterator& v1,
                     const const_vertex_iterator& v2, int color) const
    {

        if (v1 == vertex_end() || v2 == vertex_end()) {
            //std::cerr << "edgetest: vertex does not exist.\n";
            return false;
        }

        size_t i = 0;
        const_edge_iterator e = edge_end();

        while ((e = find_edge(v1, v2, i)) != edge_end()) {
            //std::cerr << "edgetest: " << e << " : " << v1->tree << " -> " << v2->tree << " - " << e->colors << " - " << color << "\n";
            if (e->colors & color) return true;
            ++i;
        }
        return false;
    }

    bool edge_exists(const Tree& t1, const Tree& t2, int color) const
    {
        return edge_exists(find_vertex(t1), find_vertex(t2), color);
    }

    template <typename BaseGraph>
    bool test_complete(const BaseGraph& bg) const
    {
        for (treemap_type::const_iterator mi = m_treemap.begin();
             mi != m_treemap.end(); ++mi)
        {
            const Tree& t = mi->first;

            Tree c = tree_complement(t, bg);

            if (m_treemap.find(c) == m_treemap.end())
            {
                std::cout << "not complete?: t = " << t << " c = " << c << "\n";
                abort();
            }
        }

        return true;
    }

    //! Find distance of shortest path s -> t using BFS
    template <typename BaseGraph>
    unsigned int get_ue_distance(unsigned int num_edge, const BaseGraph& bg,
                                 const id_type& src, const id_type& tgt,
                                 std::unordered_set<id_type>& main_seen) const;

    template <typename BaseGraph, typename TreeComplementer>
    unsigned int get_ue_diameter_generic(unsigned int num_edge, const BaseGraph& bg,
                                         TreeComplementer complementer) const;

    template <typename BaseGraph>
    class GraphComplementer
    {
    protected:
        BaseGraph bg;

    public:
        explicit GraphComplementer(const BaseGraph& g)
            : bg(g)
        { }

        Tree operator () (const Tree& t)
        {
            return tree_complement(t, bg);
        }
    };

    template <typename BaseGraph>
    unsigned int get_ue_diameter(const BaseGraph& bg) const
    {
        return get_ue_diameter_generic(bg.num_edge(), bg,
                                       GraphComplementer<BaseGraph>(bg));
    }
};

struct TauGraphDecorator : public GraphDecorator<TauGraph>
{
    int m_colormask;

    explicit TauGraphDecorator(int colormask = 3)
        : m_colormask(colormask)
    { }

    void graph(std::ostream& os, const TauGraph&) const
    {
        // os << "splines=false,K=2.0,nodesep=1.4,ranksep=2.5";
        os << "splines=false,K=4.0,nodesep=1.2,ranksep=2.5";
    }

    void extra(std::ostream& os, const TauGraph& g) const
    {
        if (g.m_title.size())
            os << "label=\"" << g.m_title << "\";\n";
    }

    void vertex_label(std::ostream& os, const vertex_iterator& v) const
    {
        os << v->label();
    }

    void vertex(std::ostream& os, const vertex_iterator& v) const
    {
        os << "label=\"" << v->label() << "\",";

        // if (1 || (v->x && v->y)) os << ",pos=\"" << v->x << "," << v->y << "!\"";
    }

    bool filter_edge(const edge_iterator& e) const
    {
        return !(e->colors & m_colormask);
    }

    //! Calculate tree A \triangle B and order and format accordingly
    std::string
    symmetric_difference_ue(const TauGraph::const_edge_iterator& e) const
    {
        const Tree t1 = e.tail()->tree;
        const Tree t2 = e.head()->tree;

        Tree out;

        std::set_symmetric_difference(t1.begin(), t1.end(),
                                      t2.begin(), t2.end(),
                                      std::back_inserter(out));

        std::ostringstream oss;

        // fallback for non-unique exchanges
        if (out.size() != 2) {
            oss << out;
            return oss.str();
        }

        if (vector_contains(t2, out[0])) {
            std::swap(out[0], out[1]);
        }

        int c = e->colors & m_colormask;

        if (c == 1)
            oss << '[' << out[1] << '-' << out[0] << "] r";
        else if (c == 2)
            oss << '[' << out[0] << '-' << out[1] << "] b";
        else if (c == 3)
            oss << '{' << out[0] << ',' << out[1] << "} br";

        return oss.str();
    }

    void edge_label(std::ostream& os, const edge_iterator& e) const
    {
        os << symmetric_difference_ue(e);
    }

    void edge(std::ostream& os, const edge_iterator& e) const
    {
        int c = e->colors & m_colormask;

        using base = BaseGraph::GraphDecoratorA;

        if (c == 1)
            os << "color=\"" << base::cRed << "\",";
        else if (c == 2)
            os << "color=\"" << base::cBlue << "\",";
        else if (c == 3)
            os << "color=\"" << base::cRed << ":" << base::cBlue << "\",";
        else
            os << "color=\"black\",";

        os << "penwidth=" << base::penwidth << ",";

        if (e->dashed)
            os << "style=\"dashed\",";
        if (e->dotted)
            os << "style=\"dotted\",";

        os << "label=\""
            // << symmetric_difference_ue(e)
           << e.edge_id()
           << *e
           << "\",";
    }
};

class TauProductDataFactory : public TauGraph::SelfProductObjectDataFactory
{
public:
    int g2_offset;

    explicit TauProductDataFactory(int _g2_offset = -1)
        : g2_offset(_g2_offset)
    { }

    vertex_data_type vertex(const const_vertex_iterator1& v1,
                            const const_vertex_iterator2& v2)
    {
        vertex_data_type v;
        v.tree = v1->tree;
        if (g2_offset < 0) {
            v.tree.push_back(-1);
            v.tree.insert(v.tree.end(), v2->tree.begin(), v2->tree.end());
        }
        else {
            for (size_t i = 0; i < v2->tree.size(); ++i)
            {
                v.tree.push_back(v2->tree[i] + g2_offset);
            }
            std::sort(v.tree.begin(), v.tree.end());
        }
        return v;
    }

    edge_data_type edge(const const_edge_iterator1& e1,
                        const const_vertex_iterator2&)
    {
        return edge_data_type(*e1);
    }

    edge_data_type edge(const const_vertex_iterator1&,
                        const const_edge_iterator2& e2)
    {
        return edge_data_type(*e2);
    }
};

//! Filter for generic graph algorithms to select only one of the colored trees
struct TauColorMaskFilter : public TauGraph::DefaultFilter
{
    //! selected tree mask
    int color;

    //! set up color filter for a tree
    explicit TauColorMaskFilter(int c)
        : color(c)
    { }

    //! ignore all edges not in the colored tree
    bool filter_edge(const TauGraph::const_edge_iterator& e) const
    {
        return !(e->colors & color);
    }
};

//! Filter for generic graph algorithms to ignore dashed (usually deleted) edges
struct TauDashedFilter : public TauGraph::DefaultFilter
{
    //! ignore all dashed edges
    bool filter_edge(const TauGraph::const_edge_iterator& e) const
    {
        return e->dashed;
    }
};

//! Output a unique exchange path as a sequence of graphs
template <typename BaseGraph>
void print_tau_exchange_path(const BaseGraph& _g,
                             const std::vector<TauGraph::const_edge_iterator>& path)
{
    BaseGraph g = _g;

    assert(path.size() != 0);

    using edge = typename BaseGraph::edge_iterator;
    using tvertex = TauGraph::const_vertex_iterator;
    using tedge = TauGraph::const_edge_iterator;

    tvertex head = path[0].tail();
    assert(path.size() == 0 || (head != path[1].head() && head != path[1].tail()));

    head->tree.colorize_graph(g);

    for (const tedge& xchg : path)
    {
        edge eM = g.edge(xchg->UEin), eB = g.edge(xchg->UEout);

        g.m_title = "UE " + to_str(xchg->UEin) + " -> " + to_str(xchg->UEout)
                    + " " + to_str(path)
                    + "\\l" + get_cycle_cut_listing(g);

        std::vector<edge> swaps = g.calc_exchange_edges(eM);
        assert(swaps.size() == 1);
        assert(swaps[0] == eB);

        eM->dashed = !eM->dashed;
        eB->dashed = !eB->dashed;

        DBG1(g.graphviz());

        std::swap(eM->color, eB->color);
    }

    DBG1(g.graphviz());
}

//! Find distance of shortest path s -> t using BFS
template <typename BaseGraph>
unsigned int TauGraph::get_ue_distance(unsigned int num_edge, const BaseGraph& bg,
                                       const id_type& src, const id_type& tgt,
                                       std::unordered_set<id_type>& main_seen) const
{
    static const bool debug = false;

    Tree srcTree = vertex(src)->tree;

    //! Type of id list
    using idlist_type = std::vector<id_type>;

    // distance of vertices
    std::vector<id_type> dist(total_vertex(), ID_INVALID);

    //! Predecessor vertices in BFS tree/DAG
    std::vector<idlist_type> pred(total_vertex());

    // BFS queues: current level and next level
    std::queue<id_type> currQueue, nextQueue;

    // number of shortest paths
    std::vector<id_type> nssp(total_vertex(), 0);

    currQueue.push(src);
    dist[src] = 0;
    nssp[src] = 1;

    size_t level;

    for (level = 0; level < num_edge / 2; ++level)
    {
        DBGG("exploring level " << level <<
             " with " << currQueue.size() << " nodes");

        while (currQueue.size())
        {
            id_type v = currQueue.front();
            currQueue.pop();

            (void)bg;
#if 0
            BaseGraph tmpg = bg;
            Tree thistree = vertex(v)->tree;
            thistree.colorize_graph(tmpg);
            DBGG("tree " << graphstring(tmpg, GraphDecoratorS())
                         << " - " << tree_difference(srcTree, thistree).size());
#endif

            for (auto ei : vertex_edge_list(v))
            {
                auto wv = ei.other(v);
                id_type w = wv.vertex_id();

                if (dist[w] == ID_INVALID) // vertex not seen yet
                {
                    // this prunes the search space but is too slow!
                    if (0 && tree_difference(srcTree, wv->tree).size() != level + 1)
                        continue;

                    dist[w] = level;
                    nextQueue.push(w);
                }
                if (dist[w] == level)
                {
                    nssp[w] += nssp[v];
                    pred[w].push_back(v);
                }
            }
        }

        std::swap(currQueue, nextQueue);
    }

    DBGG("final level " << level <<
         " has " << currQueue.size() << " nodes");

    while (currQueue.size())
    {
        id_type v = currQueue.front();
        currQueue.pop();

        if (v == tgt)
        {
            DBGG("found complement " << tgt);
#if 1
            // mark all nodes in shortest-path graph as main_seen.

            std::queue<id_type> predQueue;
            predQueue.push(tgt);

            while (predQueue.size())
            {
                id_type x = predQueue.front();
                predQueue.pop();

                main_seen.insert(x);

                for (id_type& w : pred[x]) {
                    predQueue.push(w);
                }
            }
#else
            main_seen.insert(tgt);
#endif
            return nssp[tgt];
        }
    }

    DBGG1("Could not find complement at level " << level);

#if 0
    BaseGraph bgx = bg;
    srcTree.colorize_graph(bgx);
    DBG1(graphviz(bgx, GraphDecoratorA()));
    DBGG1(graphstring(bgx, GraphDecoratorS()));
#endif
    abort();
    return 0;
    // return level;
}

template <typename BaseGraph, typename TreeComplementer>
unsigned int TauGraph::get_ue_diameter_generic(unsigned int num_edge, const BaseGraph& bg,
                                               TreeComplementer complementer) const
{
    static const bool debug = true;

    // treemap_type::const_iterator smallest = std::min_element(m_treemap.begin(), m_treemap.end());

    id_type min_nssp = 2000000000;

    std::unordered_set<id_type> seen;
    seen.reserve(m_treemap.size());

    for (treemap_type::const_iterator mi = m_treemap.begin();
         mi != m_treemap.end(); ++mi)
    {
        if (seen.count(mi->second) && mi->second != 0) continue;
        seen.insert(mi->second);

        const Tree& t = mi->first;

        Tree c = complementer(t);

        treemap_type::const_iterator xi = m_treemap.find(c);

        if (xi == m_treemap.end()) {
            std::cout << "not complete?: t = " << t << " c = " << c << "\n";
            // return -1;
            continue;
        }

        id_type nssp = get_ue_distance(num_edge, bg,
                                       mi->second, xi->second,
                                       seen);

        if (min_nssp > nssp)
        {
            min_nssp = std::min(min_nssp, nssp);
            // DBGG1("diam: "<< diam);
        }

        // seen.insert(xi->second);

        if (mi->second == 0 && 0)
        {
            std::vector<std::vector<const_edge_iterator> > pathlist =
                find_all_shortest_paths(mi->second, xi->second);

            std::vector<std::string> outlist;

            for (auto path : pathlist)
            {
                print_tau_exchange_path(bg, path);
                exit(0);

                // get source vertex of path
                const_vertex_iterator curr = vertex(mi->second);
                // get tree representation of path
                Tree tcurr = curr->tree;

                std::ostringstream oss;

                for (auto edge : path)
                {
                    const_vertex_iterator next = edge.other(curr);
                    Tree tnext = next->tree;

                    oss << " " << tree_symmetric_difference(tcurr, tnext);

                    curr = next;
                    tcurr = tnext;
                }

                outlist.push_back(oss.str());
            }

            if (0)
            {
                DBGG("paths: " << mi->first << " -> " << xi->first);
                std::sort(outlist.begin(), outlist.end());
                for (auto s : outlist) std::cout << "//" << s << std::endl;
                DBGG("end path - " << pathlist.size() << " paths");
            }

            if (0)
            {
                super_type bfs =
                    get_all_shortest_path_graph(mi->second, xi->second);

                // std::cout << graphviz(bfs, TauGraphDecorator()) << "\n";
            }
        }
    }

    return min_nssp;
}

// ****************************************************************************
// *** Run Game Algorithm

template <typename BaseGraph, bool DoOnlyLeafUEsFlag>
class AlgGameBase
{
public:
    using id_type = typename BaseGraph::id_type;
    using vertex = typename BaseGraph::vertex_iterator;
    using edge = typename BaseGraph::edge_iterator;
    using const_edge = typename BaseGraph::const_edge_iterator;
    using vertex_edge = typename BaseGraph::vertex_edge_iterator;

    static const bool debug = false;

    //! the base graph
    const BaseGraph base_;

    //! complete exchange graph
    TauGraph tau_;

    //! BFS-queue of trees in tau graph
    std::queue<Tree> queue_;

    //! full discovery of tau graph or just a component
    static const bool full_discovery_ = true;

    //! ignore all leaf UEs
    static const bool only_leaves_ = false;

    //! find leaf UEs but do not discover them
    static const bool doOnlyLeafUEs = DoOnlyLeafUEsFlag;

    //! discovery method: add to tree and BFS queue if not seen yet.
    TauGraph::vertex_iterator discover(const Tree& tree)
    {
        TauGraph::vertex_iterator iter = tau_.find_vertex(tree);

        if (iter == tau_.vertex_end())
        {
            // add new vertex to tau graph
            iter = tau_.add_vertex();
            iter->tree = tree;

            tau_.m_treemap[tree] = iter.vertex_id();
            queue_.push(tree);
        }
        return iter;
    }

    //! add an edge to the exchange graph.
    TauGraph::edge_iterator tau_add(
        const Tree& stree, const Tree& ttree, const TauEdge& e)
    {
        TauGraph::vertex_iterator siter = discover(stree);
        TauGraph::vertex_iterator titer = discover(ttree);

        if (TauGraph::is_directed)
        {
            // simple add edge
            return tau_.add_edge(siter, titer, e);
        }
        else
        {
            // overlay parallel edges
            if (tau_.find_edge(siter, titer) != tau_.edge_end()) {
                TauGraph::edge_iterator ei = tau_.find_edge(siter, titer);
                ei->colors |= e.colors;
                return ei;
            }
            else
            {
                return tau_.add_edge(siter, titer, e);
            }
        }
    }

    void play(BaseGraph& g, const Tree& makertree)
    {
        for (edge eflip : g.edge_list())
        {
            if (eflip->color < 0) continue;

            // check if edge is a leaf edge in either tree
            bool isLeafM = true, isLeafB = true;

            if (1)
            {
                vertex v1 = eflip.head();

                unsigned int v1color0 = 0, v1color1 = 0;

                for (vertex_edge ve : v1.edge_list())
                {
                    if (ve->color == BLUE) v1color0++;
                    if (ve->color == RED) v1color1++;
                }

                assert(v1color0 + v1color1 == v1.degree());

                vertex v2 = eflip.tail();

                unsigned int v2color0 = 0, v2color1 = 0;

                for (vertex_edge ve : v2.edge_list())
                {
                    if (ve->color == BLUE) v2color0++;
                    if (ve->color == RED) v2color1++;
                }

                assert(v2color0 + v2color1 == v2.degree());

                isLeafM = (v1color0 == 1 || v2color0 == 1);
                isLeafB = (v1color1 == 1 || v2color1 == 1);
            }

            if (eflip->color == BLUE && (!doOnlyLeafUEs || isLeafM))
            {
                DBGG("maker " << makertree << " colors edge " << eflip << " red");

                // find cycle in maker tree
                std::vector<edge> cycle =
                    g.find_color_path(eflip.head(), eflip.tail(), RED);
                std::sort(cycle.begin(), cycle.end());

                eflip->color = RED;

                std::vector<edge> cut = g.calc_cut_edges(eflip);

                std::vector<edge> inter;
                std::set_intersection(cycle.begin(), cycle.end(),
                                      cut.begin(), cut.end(),
                                      std::back_inserter(inter));

                if (inter.size() == 1 && (!only_leaves_ || isLeafM))
                {
                    // unique breaker edge -> unique exchange

                    inter.front()->color = BLUE;

                    assert(!g.is_cyclic(typename BaseGraph::ColorFilter(BLUE)));
                    assert(!g.is_cyclic(typename BaseGraph::ColorFilter(RED)));

                    Tree newmaker = extract_tree(g, RED);

                    // TauGraph::edge_iterator e =
                    tau_add(makertree, newmaker,
                            TauEdge(1, eflip.edge_id(), inter.front().edge_id()));

                    DBGG("maker forced: " << makertree << " -> " << newmaker);

                    DBGG("cycle length: " << cycle.size() << " - " << isLeafM);

                    // if (!isLeafM) e->dashed = 1;

                    discover(newmaker);

                    inter.front()->color = RED;
                }
                else if (full_discovery_)
                {
                    for (auto ebreak : inter)
                    {
                        ebreak->color = BLUE;

                        assert(!g.is_cyclic(typename BaseGraph::ColorFilter(BLUE)));
                        assert(!g.is_cyclic(typename BaseGraph::ColorFilter(RED)));

                        Tree newmaker = extract_tree(g, RED);
                        discover(newmaker);

                        ebreak->color = RED;
                    }
                }

                eflip->color = BLUE;
            }
            else if (eflip->color == RED && (!doOnlyLeafUEs || isLeafB)) // Maker Kante
            {
                DBGG("maker " << makertree << " colors edge " << eflip << " blue");

                // find cycle in breaker tree
                std::vector<edge> cycle =
                    g.find_color_path(eflip.head(), eflip.tail(), BLUE);
                std::sort(cycle.begin(), cycle.end());

                eflip->color = BLUE;

                std::vector<edge> cut = g.calc_cut_edges(eflip);

                std::vector<edge> inter;
                std::set_intersection(cycle.begin(), cycle.end(),
                                      cut.begin(), cut.end(),
                                      std::back_inserter(inter));

                if (inter.size() == 1 && (!only_leaves_ || isLeafB))
                {
                    // unique breaker edge -> unique exchange

                    inter.front()->color = RED;

                    assert(!g.is_cyclic(typename BaseGraph::ColorFilter(BLUE)));
                    assert(!g.is_cyclic(typename BaseGraph::ColorFilter(RED)));

                    Tree newmaker = extract_tree(g, RED);

                    // TauGraph::edge_iterator e =
                    tau_add(makertree, newmaker,
                            TauEdge(2, eflip.edge_id(), inter.front().edge_id()));

                    DBGG("maker forced: " << makertree << " -> " << newmaker);

                    DBGG("cycle length: " << cycle.size() << " - " << isLeafB);

                    // if (!isLeafB) e->dashed = 1;

                    discover(newmaker);

                    inter.front()->color = BLUE;
                }
                else if (full_discovery_)
                {
                    for (auto ebreak : inter)
                    {
                        ebreak->color = RED;

                        assert(!g.is_cyclic(typename BaseGraph::ColorFilter(BLUE)));
                        assert(!g.is_cyclic(typename BaseGraph::ColorFilter(RED)));

                        Tree newmaker = extract_tree(g, RED);
                        discover(newmaker);

                        ebreak->color = BLUE;
                    }
                }

                eflip->color = RED;
            }
        }
    }

    explicit AlgGameBase(const BaseGraph& _g)
        : base_(_g)
    {
        BaseGraph g = _g;

        // collect edges of the first maker tree as found by AlgBispanning
        Tree first_tree = extract_tree(g, RED);
        discover(first_tree);

        while (!queue_.empty())
        {
            Tree t = queue_.front();
            queue_.pop();

            t.colorize_graph(g);

            play(g, t);
        }
    }

    void print(bool verbose = false, bool write_tgf = false)
    {
        id_type comp = tau_.get_num_components();
        id_type comp1 = tau_.get_num_components(TauColorMaskFilter(1));
        id_type comp2 = tau_.get_num_components(TauColorMaskFilter(2));

        // if (comp1 == 1) return;

        iGraph<BaseGraph> ibase(base_);
        iGraph<TauGraph> itau(tau_);
        // TauGraph::const_vertex_iterator dfrom = tau_.vertex_begin(), dto = tau_.vertex_begin();
        // int d = itau.get_diameter(dfrom, dto);

        // *** print RESULT

        std::cout << "// RESULT"
                  << " graph=" << write_graph6(base_)
                  << " num_vertex=" << base_.num_vertex()
                  << " num_edge=" << base_.num_edge()
                  << " degrees=" << base_.get_degree_sequence()
                  << " girth=" << ibase.get_girth()
                  << " vertex_conn=" << ibase.get_vertex_connectivity()
                  << " edge_conn=" << ibase.get_edge_connectivity();

        if (0)
        {
            int vertex_chromatic, edge_chromatic;

            std::string a = "chromatic '" + write_graph6(base_) + "'";
            FILE* f = popen(a.c_str(), "r");

            int r = fscanf(f, "%d\n%d\n", &vertex_chromatic, &edge_chromatic);
            if (r != 2) abort();

            fclose(f);

            std::cout << " vertex_chrom=" << vertex_chromatic
                      << " edge_chrom=" << edge_chromatic;
        }

        std::cout << " components=" << comp
                  << " components1=" << comp1
                  << " components2=" << comp2
                  << " tau_num_vertex=" << tau_.num_vertex()
                  << " tau_num_edge=" << tau_.num_edge()
                  << " tau_min_degree=" << tau_.get_min_degree()
                  << " tau_max_degree=" << tau_.get_max_degree()
                  << " tau_avg_degree=" << tau_.get_avg_degree()
                  << " tau_regularity=" << tau_.get_regularity()
            // << " tau_average_plens=" << itau.get_average_path_length()
            // << " tau_diameter=" << itau.get_diameter()
            // << " tau_radius=" << itau.get_radius()
            // << " tau_econn=" << itau.get_edge_connectivity()
            // << " tau_diameter_path=[" << dfrom->label << "]-[" << dto->label << "]"
                  << " min_UECBOs=" << tau_.get_ue_diameter(base_);

        // calcExpansionParameters(base_);
        std::cout << "\n";

        // std::cout << "// tau_degrees: " << tau_.get_degree_sequence() << "\n";

        if (comp != 1)
        {
            std::cout << "// connected: " << tau_.is_connected() << "\n";

            std::cout << base_.graphviz() << std::endl;

            std::cout << graphviz(tau_, TauGraphDecorator(1)) << "\n";
            std::cout << graphviz(tau_, TauGraphDecorator(2)) << "\n";
            std::cout << graphviz(tau_, TauGraphDecorator(3)) << "\n";

            abort();
        }

        // std::cout << "// connected: " << is_connected(tau_) << "\n";
        // std::cout << "// connected rot: " << is_connected_mask(tau_, 2) << "\n";
        // std::cout << "// connected blau: " << is_connected_mask(tau_, 1) << "\n";
        // std::cout << "// knoten: " << tau_.num_vertex() << "\n";

        if (verbose)
        {
            // std::cout << graphviz(tau_, TauGraphDecorator(1)) << "\n";
            // std::cout << graphviz(tau_, TauGraphDecorator(2)) << "\n";
            std::cout << graphviz(tau_, TauGraphDecorator(3)) << "\n";
        }

        if (0)
        {
            TauGraphDecorator deco(3);

            using id_pair_type = std::pair<id_type, id_type>;
            std::map<id_pair_type, unsigned int> uemap;

            for (TauGraph::edge_iterator ei : tau_.edge_list())
            {
                const Tree t1 = ei.tail()->tree;
                const Tree t2 = ei.head()->tree;

                Tree out;

                std::set_symmetric_difference(t1.begin(), t1.end(),
                                              t2.begin(), t2.end(),
                                              std::back_inserter(out));

                assert(out.size() == 2 && out[0] < out[1]);

                // std::cout << out << "\n";

                uemap[std::make_pair(out[0], out[1])]++;
            }

            std::vector<std::pair<id_pair_type, unsigned int> > uelist;

            for (auto ue : uemap)
            {
                uelist.push_back(std::make_pair(ue.first, ue.second));
            }

            std::sort(uelist.begin(), uelist.end(),
                      [](const std::pair<id_pair_type, unsigned int>& a,
                         const std::pair<id_pair_type, unsigned int>& b)
                      {
                          if (a.second == b.second)
                              return a.first < b.first;
                          return a.second > b.second;
                      });

            for (auto ue : uelist)
            {
                const_edge e0 = base_.edge(ue.first.first);
                const_edge e1 = base_.edge(ue.first.second);

                id_type e0v0 = e0.tail_id(), e0v1 = e0.head_id();
                id_type e1v0 = e1.tail_id(), e1v1 = e1.head_id();

                bool onNode = (e0v0 == e1v0 || e0v0 == e1v1 ||
                               e0v1 == e1v0 || e0v1 == e1v1);

                DBGG1(ue.first << " - " << ue.second << (onNode ? " near" : " far"));
            }
        }

        if (write_tgf)
        {
            std::string fname = "tau-" + write_graph6(base_) + ".tgf";

            std::ofstream of(fname);
            of << graphtgf(tau_, TauGraphDecorator()) << "\n";
        }

        tau_.test_complete(base_);
    }
};

template <typename BaseGraph>
using AlgGame = AlgGameBase<BaseGraph, false>;

template <typename BaseGraph>
using AlgGameLeafUEs = AlgGameBase<BaseGraph, true>;

#endif // !BISPANNING_ALG_GAME_HEADER

/******************************************************************************/
