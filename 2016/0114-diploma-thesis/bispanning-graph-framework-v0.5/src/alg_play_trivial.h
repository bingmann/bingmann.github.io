/*******************************************************************************
 * src/alg_play_trivial.h
 *
 * Play bispanning exchange game using trivial algorithm.
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

#ifndef BISPANNING_ALG_PLAY_TRIVIAL_HEADER
#define BISPANNING_ALG_PLAY_TRIVIAL_HEADER

#include "alg_game.h"

// *****************************************************************************
// *** Play Game Algorithm

class AlgPlayTrivial
{
public:
    static const bool debug = true;

    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::vertex_iterator;
    using edge = BaseGraph::edge_iterator;
    using vertex_edge = BaseGraph::vertex_edge_iterator;

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

    void algorithm_decompose(const BaseGraph& _g)
    {
        BaseGraph g = _g;
        BaseGraph reduced = g;

        uestack_type uestack = g_uestack;

        for (auto ei : reduced.edge_list())
        {
            ei->original = ei.edge_id();
        }
        reduced.m_title = "Reduced";

        while (reduced.num_vertex() > 1)
        {
            std::cout << g.graphviz() << std::endl;
            std::cout << reduced.graphviz() << std::endl;

            {
                AlgBispanning<BaseGraph> ab(reduced);
                DBGG("REDUCED bispanning: " << ab.good());
                if (!ab.good()) {
                    abort();
                }

                iGraph<BaseGraph> ig(reduced);

                DBGG("Reduced is_atomic: " << reduced.is_atomic_bispanner()
                                           << " vertexconn: " << ig.get_vertex_connectivity()
                                           << " edgeconn: " << ig.get_edge_connectivity());
            }

            if (reduced.get_min_degree() == 2)
            {
                vertex v = reduced.find_vertex_degree(2);

                DBGG("found deg(v)=2 : " << v);

                vertex_edge e0 = v.edge(0);
                vertex_edge e1 = v.edge(1);

                edge e0o = g.edge(e0->original);
                edge e1o = g.edge(e1->original);

                checked_unique_exchange(g, e0o, e1o);
                // uestack.push( uepair_type(e0o, e1o) );

                reduced.delete_vertex(v);
            }
            else if (reduced.get_min_degree() == 3)
            {
                vertex v = reduced.find_vertex_degree(3);

                DBGG("found deg(v)=3 : " << v);

                vertex_edge e0 = v.edge(0);
                vertex_edge e1 = v.edge(1);
                vertex_edge e2 = v.edge(2);

                // color of single edge
                int color1 = (e0->color + e1->color + e2->color) % 2;
                // color of double edge
                int color2 = (color1 == BLUE ? RED : BLUE);

                DBGG("color1 = " << color1 << " - color2 = " << color2);

                // swap edges so that e0 is the color1

                if (e1->color == color1) std::swap(e1, e0);
                if (e2->color == color1) std::swap(e2, e0);

                // find path from v to other end of e0r.

                std::vector<edge> cycle =
                    reduced.find_color_path(e0.tail(), e0.head(), color2);

                DBGG("path = " << cycle);
                assert(cycle[0] == e1 || cycle[0] == e2);

                edge e_cycle = cycle[0];
                edge e_noncycle = (cycle[0] == e1 ? e2 : e1);

                edge oeM = g.edge(e0->original);
                edge oeB = g.edge(e_cycle->original);

                // add split edge, remove vertex of degree 3

                edge e_new = reduced.add_edge(e0.other(v), e_noncycle.other(v));

                e_new->color = color2;
                e_new->original = e_noncycle->original;

                reduced.delete_vertex(v);

                checked_unique_exchange(g, oeM, oeB);
                // uestack.push( uepair_type(oeM, oeB) );
            }
            else {
                abort(); // impossible
            }
        }

        std::cout << g.graphviz() << std::endl;
    }

    void algorithm_greedy_leafUE(const BaseGraph& _g)
    {
        BaseGraph g = _g;

        unsigned int swaps = 0;

        while (1)
        {
            std::cout << g.graphviz() << std::endl;

            vertex vp = g.vertex_end();
            edge eLeaf = g.edge_end();

            for (vertex v : g.vertex_list())
            {
                unsigned int color0 = 0, color1 = 0;
                edge ecolor0 = g.edge_end(), ecolor1 = g.edge_end();

                for (vertex_edge ve : v.edge_list())
                {
                    if (ve->color == BLUE)
                        color0++, ecolor0 = ve;
                    if (ve->color == RED)
                        color1++, ecolor1 = ve;
                }

                if (color0 == 1 && !ecolor0->dashed) // blue leaf vertex
                {
                    vp = v;
                    eLeaf = ecolor0;
                    break;
                }
                else if (color1 == 1 && !ecolor1->dashed) // red leaf vertex
                {
                    vp = v;
                    eLeaf = ecolor1;
                    break;
                }
            }

            if (vp == g.vertex_end()) break;

            DBGG("Found leaf node " << vp << " leaf edge " << eLeaf);

            std::vector<edge> xchg = g.calc_exchange_edges(eLeaf);
            DBGG("exchanges: " << xchg);

            if (xchg.size() != 1) {
                DBGG("NON-UNIQUE EXCHANGE!");
                abort();
            }

            edge eB = xchg[0];

            std::swap(eLeaf->color, eB->color);
            eLeaf->dashed = eB->dashed = 1;
            ++swaps;
        }

        if (swaps != _g.num_edge() / 2) {
            DBGG("GRAPH NOT INVERTED!");
            abort();
        }
    }

    explicit AlgPlayTrivial(const BaseGraph& _g)
    {
        // algorithm_decompose(_g);
        algorithm_greedy_leafUE(_g);

#if 0
        std::cout << graphviz(reduced, GraphDecoratorA()) << std::endl;

        while (uestack.size())
        {
            std::cout << g.graphviz() << std::endl;

            uepair_type ue = uestack.top();
            uestack.pop();

            DBGG("uestack: " << ue);

            while (g_esplitmap[ue.first] != ue.first) {
                DBGG("map first: " << ue.first << " -> " << g_esplitmap[ue.first]);
                ue.first = g_esplitmap[ue.first];
            }

            while (g_esplitmap[ue.second] != ue.second) {
                DBGG("map second: " << ue.second << " -> " << g_esplitmap[ue.second]);
                ue.second = g_esplitmap[ue.second];
            }

            edge e0 = g.edge(ue.first);
            edge e1 = g.edge(ue.second);

            checked_unique_exchange(g, e0, e1);
        }
#endif
    }
};

#endif // !BISPANNING_ALG_PLAY_TRIVIAL_HEADER

/******************************************************************************/
