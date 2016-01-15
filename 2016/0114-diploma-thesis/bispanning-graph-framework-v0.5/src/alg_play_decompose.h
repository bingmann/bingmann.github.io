/*******************************************************************************
 * src/alg_play_decompose.h
 *
 * Play bispanning exchange game using decomposition algorithm.
 *
 * Copyright (C) 2014-2016 Timo Bingmann <tb@panthema.net>
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

#ifndef BISPANNING_ALG_PLAY_DECOMPOSE_HEADER
#define BISPANNING_ALG_PLAY_DECOMPOSE_HEADER

#include "alg_game.h"

// *****************************************************************************
// *** Play Game Algorithm

struct AlgPlayDecompose
{
    static const bool debug = true;

    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::vertex_iterator;
    using edge = BaseGraph::edge_iterator;
    using vertex_edge = BaseGraph::vertex_edge_iterator;
    using const_vertex = BaseGraph::const_vertex_iterator;
    using const_edge = BaseGraph::const_edge_iterator;
    using const_vertex_edge = BaseGraph::const_vertex_edge_iterator;

    using tvertex = TauGraph::vertex_iterator;
    using tedge = TauGraph::edge_iterator;
    using tvertex_edge = TauGraph::vertex_edge_iterator;

    using Exchange = TauEdge;

    using ExchangeList = std::vector<Exchange>;

    using ExchangeListList = std::vector<ExchangeList>;

    void checked_unique_exchange(BaseGraph& g, edge& eM, edge& eB)
    {
        assert(g.contains(eM) && g.contains(eB));

        DBGG("swap: " << eM << " - " << eB);

        if (eM->dashed) {
            DBGG("ALREADY SWAPPED! " << eM);
        }
        if (eB->dashed) {
            DBGG("ALREADY SWAPPED! " << eB);
        }

        std::vector<edge> xchg = g.calc_exchange_edges(eM);
        DBGG("exchanges: " << xchg);

        if (xchg.size() != 1) {
            DBGG("NON-UNIQUE EXCHANGE!");
            abort();
        }
        else if (xchg[0] != eB) {
            DBGG("NON-MATCHING UNIQUE EXCHANGE!");
            abort();
        }

        std::swap(eM->color, eB->color);
        eM->dashed = eB->dashed = true;
    }

    template <bool CheckLeafUE = false>
    void do_checked_unique_exchange(BaseGraph& g, const Exchange& xchg)
    {
        static const bool debug = false;

        edge eM = g.edge(xchg.UEin), eB = g.edge(xchg.UEout);

        DBGG("swap: " << eM << " - " << eB);

        if (eM->dashed) {
            DBGG1("ALREADY SWAPPED! " << eM);
            abort();
        }
        if (eB->dashed) {
            DBGG1("ALREADY SWAPPED! " << eB);
            abort();
        }
        if (eM->color + 1 != xchg.colors) {
            DBGG1("WRONG SWAP COLOR! " << xchg.colors + 1 << " != " << eM->color);
            abort();
        }

        if (CheckLeafUE)
        {
            unsigned int in_head = 0, in_tail = 0;

            for (vertex_edge ve : eM.head().edge_list()) {
                if (ve->color == eM->color)
                    ++in_head;
            }
            for (vertex_edge ve : eM.tail().edge_list()) {
                if (ve->color == eM->color)
                    ++in_tail;
            }

            std::cerr << "head/tail: " << in_head << " - " << in_tail << "\n";

            if (in_head != 1 && in_tail != 1) {
                DBGG1("NON-LEAF UNIQUE EXCHANGE! " << xchg);
                abort();
            }
        }

        std::vector<edge> swaps = g.calc_exchange_edges(eM);
        DBGG("swappable edge exchanges: " << swaps);

        if (swaps.size() != 1) {
            DBGG1("NON-UNIQUE EXCHANGE! " << xchg);
            abort();
        }
        else if (swaps[0] != eB) {
            DBGG1("NON-MATCHING UNIQUE EXCHANGE! " << xchg);
            abort();
        }

        std::swap(eM->color, eB->color);
        eM->dashed = eB->dashed = true;
    }

    template <bool CheckLeafUE = false>
    void do_checked_leaf_sequence(const BaseGraph& _g,
                                  const ExchangeList& xchglist)
    {
        static const bool debug = true;

        BaseGraph g = _g;

        Tree src_tree = extract_tree(g, RED);
        Tree tgt_tree = tree_complement(src_tree, g);

        DBG(g.graphviz());

        for (const Exchange& xchg : xchglist)
        {
            do_checked_unique_exchange<CheckLeafUE>(g, xchg);

            DBG(g.graphviz());
        }

        if (extract_tree(g, RED) != tgt_tree) {
            DBGG1("NON-INVERTING EXCHANGE SEQUENCE!");
            abort();
        }
    }

    bool is_atomic_or_get_partition(const BaseGraph& g)
    {
        static const bool debug = false;

        std::vector<id_type> vmap(g.total_vertex());

        id_type i = 0;
        for (const_vertex v : g.vertex_list())
            vmap[v.vertex_id()] = i++;

        std::vector<std::vector<size_t> > comp_partition;

        DBG(g.graphviz());

        enumerate_set_partitions(
            g.num_vertex(),
            [&](const std::vector<size_t>& setp)
            {
                size_t np = *std::max_element(setp.begin(), setp.end()) + 1;

                // skip trivial partitions
                if (np == 1 || np == g.num_vertex()) return true;

                unsigned int cross = 0;

                for (const_edge e : g.edge_list())
                {
                    id_type v = e.tail_id(), w = e.head_id();

                    if (setp[vmap[v]] != setp[vmap[w]])
                        ++cross;
                }

                DBGG(setp << " - " << np << " - cross " << cross);

                if (cross == 2 * (np - 1)) { // composite
                    DBGG(setp << " - " << np << " - cross " << cross);
                    comp_partition.push_back(setp);

                                             // calculate super-graph
                    {
                        BaseGraph g0 = g;
                        g0.m_title = "super-graph " + to_str(setp);

                        // find first vertex of each component
                        std::vector<id_type> v0(np, BaseGraph::ID_INVALID);

                        // contract all vertices marked with same component
                        for (vertex v : g0.vertex_list())
                        {
                            id_type vi = v.vertex_id();
                            id_type vc = setp[vmap[vi]];

                            if (v0[vc] == BaseGraph::ID_INVALID) {
                                v0[vc] = vi;
                            }
                            else {
                                g0.contract(v0[vc], vi);
                            }
                        }

                        DBG(g0.graphviz());

                        if (!g0.is_connected())
                            abort();
                    }

                    // calculate subgraphs
                    for (size_t c = 0; c < np; ++c)
                    {
                        BaseGraph g0 = g;
                        g0.m_title = "sub-graph " + to_str(setp) + " c=" + to_str(c);

                        for (vertex v : g0.vertex_list())
                        {
                            id_type vi = v.vertex_id();
                            id_type vc = setp[vmap[vi]];

                            if (vc != c)
                                g0.delete_vertex(v);
                        }

                        DBG(g0.graphviz());

                        if (!g0.is_connected())
                            abort();
                    }

                    // return false;
                }

                return true;
            });

        return comp_partition.size() == 0;
    }

    ExchangeListList get_exchangelist_list_direct(const BaseGraph& g)
    {
        static const bool debug = false;

        TauGraph tau = AlgGame<BaseGraph>(g).tau_;
        DBG(graphviz(tau, TauGraphDecorator()));

        Tree src_tree = extract_tree(g, RED);
        Tree tgt_tree = tree_complement(src_tree, g);

        tvertex src = tau.find_vertex(src_tree);
        tvertex tgt = tau.find_vertex(tgt_tree);

        std::vector<std::vector<tedge> > allpaths
            = tau.find_all_shortest_paths(src, tgt);

        std::sort(allpaths.begin(), allpaths.end());

        ExchangeListList listlist;

        for (const std::vector<tedge>& uepath : allpaths)
        {
            // std::cerr << "uepath: " << uepath << "\n";

            std::vector<std::string> uedesc;
            for (const tedge& ue : uepath)
            {
                std::ostringstream oss;
                oss << ue->UEin << "->" << ue->UEout;
                uedesc.push_back(oss.str());
            }

            std::cerr << vector_join(uedesc, " ") << "\n";

            ExchangeList xchglist;

            for (const tedge& ue : uepath)
                xchglist.push_back(*ue);

            if (debug)
                do_checked_leaf_sequence<true>(g, xchglist);

            listlist.push_back(xchglist);
        }

        return listlist;
    }

    void decompose_deg3(const BaseGraph& g, const_vertex deg3v)
    {
        DBGGE("decomposing atomic at deg(v)=3 : " << deg3v);
        DBGGE(g.graphstring());
        assert(deg3v.degree() == 3);

        const_edge e0 = deg3v.edge(0);
        const_edge e1 = deg3v.edge(1);
        const_edge e2 = deg3v.edge(2);

        // color of single edge
        int single_color = (e0->color + e1->color + e2->color) % 2;
        // color of double edge
        int double_color = (single_color == BLUE ? RED : BLUE);

        DBGGE("single_color = " << single_color << " - double_color = " << double_color);

        // swap edges so that e0 is the single_color

        if (e1->color == single_color) std::swap(e1, e0);
        if (e2->color == single_color) std::swap(e2, e0);

        assert(e1->color == e2->color);

        // identify the three vertices

        // id_type v0i = e0.other_id(deg3v);
        id_type v1i = e1.other_id(deg3v);
        id_type v2i = e2.other_id(deg3v);

        // find path from attached node to deg3v (via e1 or e2)

        std::vector<const_edge> cycle_path
            = g.find_color_path(deg3v, e0.other(deg3v), double_color);
        ASSERT(cycle_path[0] == e1 || cycle_path[0] == e2);

        // select cycle and non-cycle edges
        const_edge& e_cycle = cycle_path[0];
        const_edge& e_noncycle = (cycle_path[0] == e1 ? e2 : e1);

        // construct reduced graph

        BaseGraph gr = g;

        gr.delete_vertex(deg3v.vertex_id());
        edge e_split = gr.add_edge(v1i, v2i, EdgeT(double_color));

        gr.m_title = "reduced at deg3v = " + to_str(deg3v.vertex_id());
        DBG(gr.graphviz());

        // find

        std::cerr << "original graph" << std::endl;
        get_exchangelist_list_direct(g);

        std::cerr << "reduced graph" << std::endl;
        get_exchangelist_list_direct(gr);

        exit(0);
        (void)e_cycle;
        (void)e_noncycle;
        (void)e_split;
    }

    void algorithm_decompose(const BaseGraph& g)
    {
        // test if g is a composite bispanner

        bool is_atomic = is_atomic_or_get_partition(g);

        if (is_atomic)
        {
            DBG0(g.graphviz());

            assert(g.get_min_degree() == 3);

            for (const_vertex deg3v : g.vertex_list())
            {
                if (deg3v.degree() != 3) continue;

                DBG(g.graphviz());
                decompose_deg3(g, deg3v);
            }
        }
        else
        { }
    }

    explicit AlgPlayDecompose(const BaseGraph& _g)
    {
        DBG0(_g.graphviz());

        TauGraph tau = AlgGame<BaseGraph>(_g).tau_;
        DBG0(graphviz(tau, TauGraphDecorator()));

        // iterate over all possible starting tree constellations

        for (TauGraph::const_vertex_iterator vi : tau.vertex_list())
        {
            BaseGraph g = _g;
            vi->tree.colorize_graph(g);

            DBG1(g.graphviz());

            algorithm_decompose(g);
        }
    }
};

#endif // !BISPANNING_ALG_PLAY_DECOMPOSE_HEADER

/******************************************************************************/
