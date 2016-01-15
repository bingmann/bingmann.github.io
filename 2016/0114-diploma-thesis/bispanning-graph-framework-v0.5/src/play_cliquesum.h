/*******************************************************************************
 * src/play_cliquesum.h
 *
 * Post process enumeration: join pairs of graphs using 2- or 3-sum
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

#ifndef BISPANNING_PLAY_CLIQUESUM_HEADER
#define BISPANNING_PLAY_CLIQUESUM_HEADER

void post_process_2sums()
{
    using id_type = BaseGraph::id_type;
    using edge = BaseGraph::const_edge_iterator;

    static const bool debug = true;

    for (const BaseGraph& g1 : g_graphs)
    {
        for (const BaseGraph& g2 : g_graphs)
        {
            DBGG1("g1: " << write_graph6(g1) << " g2: " << write_graph6(g2));

            //DBG( g1.graphviz() );
            //DBG( g2.graphviz() );

            for (edge e1 : g1.edge_list())
            {
                for (edge e2 : g2.edge_list())
                {
                    BaseGraph g = g1;

                    id_type vbase = g.total_vertex();
                    id_type ebase = g.total_edge();

                    g.add_copy(g2);

                    //DBG( g.graphviz() );

                    g.contract(e1.head_id(), vbase + e2.head_id());
                    g.contract(e1.tail_id(), vbase + e2.tail_id());

                    g.delete_edge(e1.edge_id());
                    g.delete_edge(ebase + e2.edge_id());

                    //DBG( g.graphviz() );

                    g = g.clone_reorder_degree<BaseGraph>();

                    //DBG( g.graphviz() );

                    AlgBispanning<BaseGraph> ab(g);

                    if (!ab.good()) {
                        DBG(g.graphviz());
                        std::cerr << "// Not a bispanner: " << write_graph6(g) << std::endl;
                        return;
                    }

                    ab.write(g);

                    //DBG( g.graphviz() );
                }
            }
        }
    }
}

void post_process_3sums()
{
    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::const_vertex_iterator;
    using edge = BaseGraph::const_edge_iterator;
    using triangle = BaseGraph::triangle_type;
    using triangle_vertex = BaseGraph::triangle_vertex_type;

    static const bool debug = true;

    BaseGraph::IsomorphismCheck isocheck;

    for (const BaseGraph& g1 : g_graphs)
    {
        for (const BaseGraph& g2 : g_graphs)
        {
            DBGG1("g1: " << write_graph6(g1) << " g2: " << write_graph6(g2));

            DBG(g1.graphviz());
            DBG(g2.graphviz());

            std::vector<triangle> tri1 = g1.find_triangles();
            std::vector<triangle> tri2 = g2.find_triangles();

            for (const triangle& t1 : tri1)
            {
                for (const triangle& t2 : tri2)
                {
                    BaseGraph gjoin = g1;

                    id_type vbase = gjoin.total_vertex();
                    id_type ebase = gjoin.total_edge();

                    gjoin.add_copy(g2);

                    DBG(gjoin.graphviz());

                    if (0)
                    {
                        BaseGraph gtest = gjoin;

                        gtest.delete_edge(t1[0].edge_id());
                        gtest.delete_edge(t1[1].edge_id());
                        gtest.delete_edge(t1[2].edge_id());
                        gtest.delete_edge(ebase + t2[0].edge_id());
                        gtest.delete_edge(ebase + t2[1].edge_id());
                        gtest.delete_edge(ebase + t2[2].edge_id());

                        if (gtest.get_num_components() > 2)
                        {
                            DBG(gjoin.graphviz());
                            DBG(gtest.graphviz());

                            continue;
                        }
                    }

                    std::vector<int> mapping = { 0, 1, 2 };

                    do {
                        BaseGraph gcontr = gjoin;

                        triangle_vertex tv1 = gcontr.get_triangle_vertices(t1);
                        triangle_vertex tv2 = gcontr.get_triangle_vertices(t2);

                        gcontr.contract(tv1[0].vertex_id(), vbase + tv2[mapping[0]].vertex_id());
                        gcontr.contract(tv1[1].vertex_id(), vbase + tv2[mapping[1]].vertex_id());
                        gcontr.contract(tv1[2].vertex_id(), vbase + tv2[mapping[2]].vertex_id());

                        int map_good = 0, map_bad = 0;

                        for (id_type ex = 0; ex < 6; ++ex)
                        {
                            BaseGraph g = gcontr;
                            BaseGraph gcmark = gcontr;
                            BaseGraph gjmark = gjoin;

                            //DBG( g.graphviz() );

                            if (ex != 0 && ex != 3 && ex != 4) {
                                g.delete_edge(t1[0].edge_id());
                                gcmark.edge(t1[0].edge_id())->dashed = true;
                                gjmark.edge(t1[0].edge_id())->dashed = true;
                            }
                            if (ex != 1 && ex != 3 && ex != 5) {
                                g.delete_edge(t1[1].edge_id());
                                gcmark.edge(t1[1].edge_id())->dashed = true;
                                gjmark.edge(t1[1].edge_id())->dashed = true;
                            }
                            if (ex != 2 && ex != 4 && ex != 5) {
                                g.delete_edge(t1[2].edge_id());
                                gcmark.edge(t1[2].edge_id())->dashed = true;
                                gjmark.edge(t1[2].edge_id())->dashed = true;
                            }

                            if (ex != 0) {
                                g.delete_edge(ebase + t2[0].edge_id());
                                gcmark.edge(ebase + t2[0].edge_id())->dashed = true;
                                gjmark.edge(ebase + t2[0].edge_id())->dashed = true;
                            }
                            if (ex != 1) {
                                g.delete_edge(ebase + t2[1].edge_id());
                                gcmark.edge(ebase + t2[1].edge_id())->dashed = true;
                                gjmark.edge(ebase + t2[1].edge_id())->dashed = true;
                            }
                            if (ex != 2) {
                                g.delete_edge(ebase + t2[2].edge_id());
                                gcmark.edge(ebase + t2[2].edge_id())->dashed = true;
                                gjmark.edge(ebase + t2[2].edge_id())->dashed = true;
                            }

                            //DBG( g.graphviz() );
                            //DBG( gcmark.graphviz() );

                            bool is_bad =
                                (g.get_min_degree() <= 1) ||
                                (g.count_edges(tv1[0].vertex_id(), tv1[1].vertex_id()) >= 3) ||
                                (g.count_edges(tv1[0].vertex_id(), tv1[2].vertex_id()) >= 3) ||
                                (g.count_edges(tv1[1].vertex_id(), tv1[2].vertex_id()) >= 3);

                            DBGG0("is_bad = " << is_bad);

                            BaseGraph gr = g.clone_reorder_degree<BaseGraph>();

                            //DBG( g.graphviz() );

                            AlgBispanning<BaseGraph> ab(gr);

                            if (!ab.good()) {

                                map_bad++;

                                //if (isocheck(gr)) {

                                //DBG( gjmark.graphviz() );
                                //DBG( gcmark.graphviz() );
                                //DBG( g.graphviz() );

                                std::cout << "// Not a bispanner: " << write_graph6(gr) << std::endl;
                                //return;
                                //}
                            }
                            else {
                                map_good++;
                            }

                            //ab.write(g);

                            //DBG( g.graphviz() );
                        }

                        std::cerr << "maps: " << map_good << " good, " << map_bad << " bad." << std::endl;
                        if (map_good < 2)
                        {
                            DBG(gjoin.graphviz());
                            DBG(gcontr.graphviz());
                            abort();
                        }
                    }
                    while (std::next_permutation(mapping.begin(), mapping.end()));

                    //return;
                }
            }
        }
    }
}

void post_process_3sums_keep_colors()
{
    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::const_vertex_iterator;
    using edge = BaseGraph::const_edge_iterator;
    using triangle = BaseGraph::triangle_type;
    using triangle_vertex = BaseGraph::triangle_vertex_type;

    static const bool debug = true;

    BaseGraph::IsomorphismCheck isocheck;

    for (const BaseGraph& g1 : g_graphs)
    {
        for (const BaseGraph& g2 : g_graphs)
        {
            DBGG1("g1: " << write_graph6(g1) << " g2: " << write_graph6(g2));

            //DBG( g1.graphviz() );
            //DBG( g2.graphviz() );

            std::vector<triangle> tri1 = g1.find_triangles();
            std::vector<triangle> tri2 = g2.find_triangles();

            for (const triangle& t1 : tri1)
            {
                if (g1.count_edges(t1[0]) != 1) continue;
                if (g1.count_edges(t1[1]) != 1) continue;
                if (g1.count_edges(t1[2]) != 1) continue;

                for (const triangle& t2 : tri2)
                {
                    if (g2.count_edges(t2[0]) != 1) continue;
                    if (g2.count_edges(t2[1]) != 1) continue;
                    if (g2.count_edges(t2[2]) != 1) continue;

                    BaseGraph gjoin = g1;

                    id_type vbase = gjoin.total_vertex();
                    id_type ebase = gjoin.total_edge();

                    gjoin.add_copy(g2);

                    //DBG( gjoin.graphviz() );

                    if (0)
                    {
                        BaseGraph gtest = gjoin;

                        gtest.delete_edge(t1[0].edge_id());
                        gtest.delete_edge(t1[1].edge_id());
                        gtest.delete_edge(t1[2].edge_id());
                        gtest.delete_edge(ebase + t2[0].edge_id());
                        gtest.delete_edge(ebase + t2[1].edge_id());
                        gtest.delete_edge(ebase + t2[2].edge_id());

                        if (gtest.get_num_components() > 2) {

                            //DBG( gjoin.graphviz() );
                            //DBG( gtest.graphviz() );

                            continue;
                        }
                    }

                    std::vector<int> mapping = { 0, 1, 2 };

                    do {
                        BaseGraph gcontr = gjoin;

                        triangle_vertex tv1 = gcontr.get_triangle_vertices(t1);
                        triangle_vertex tv2 = gcontr.get_triangle_vertices(t2);

                        gcontr.contract(tv1[0].vertex_id(), vbase + tv2[mapping[0]].vertex_id());
                        gcontr.contract(tv1[1].vertex_id(), vbase + tv2[mapping[1]].vertex_id());
                        gcontr.contract(tv1[2].vertex_id(), vbase + tv2[mapping[2]].vertex_id());

                        if (0)
                        {
                            edge e0 = gcontr.find_edge(tv1[0].vertex_id(), tv1[1].vertex_id(), 0);
                            edge e1 = gcontr.find_edge(tv1[0].vertex_id(), tv1[1].vertex_id(), 1);

                            edge e2 = gcontr.find_edge(tv1[0].vertex_id(), tv1[2].vertex_id(), 0);
                            edge e3 = gcontr.find_edge(tv1[0].vertex_id(), tv1[2].vertex_id(), 1);

                            edge e4 = gcontr.find_edge(tv1[1].vertex_id(), tv1[2].vertex_id(), 0);
                            edge e5 = gcontr.find_edge(tv1[1].vertex_id(), tv1[2].vertex_id(), 1);

                            if (e0->color == e1->color) break;
                            if (e2->color == e3->color) break;
                            if (e4->color == e5->color) break;
                        }

                        int map_good = 0, map_bad = 0;

                        for (id_type ex1 = 0; ex1 < 6; ++ex1)
                        {
                            for (id_type ex2 = ex1 + 1; ex2 < 6; ++ex2)
                            {
                                BaseGraph g = gcontr;
                                BaseGraph gcmark = gcontr;
                                BaseGraph gjmark = gjoin;

                                //DBG( g.graphviz() );

                                if (ex1 != 0 && ex2 != 0) {
                                    g.delete_edge(t1[0].edge_id());
                                    gcmark.edge(t1[0].edge_id())->dashed = true;
                                    gjmark.edge(t1[0].edge_id())->dashed = true;
                                }
                                if (ex1 != 1 && ex2 != 1) {
                                    g.delete_edge(t1[1].edge_id());
                                    gcmark.edge(t1[1].edge_id())->dashed = true;
                                    gjmark.edge(t1[1].edge_id())->dashed = true;
                                }
                                if (ex1 != 2 && ex2 != 2) {
                                    g.delete_edge(t1[2].edge_id());
                                    gcmark.edge(t1[2].edge_id())->dashed = true;
                                    gjmark.edge(t1[2].edge_id())->dashed = true;
                                }

                                if (ex1 != 3 && ex2 != 3) {
                                    g.delete_edge(ebase + t2[0].edge_id());
                                    gcmark.edge(ebase + t2[0].edge_id())->dashed = true;
                                    gjmark.edge(ebase + t2[0].edge_id())->dashed = true;
                                }
                                if (ex1 != 4 && ex2 != 4) {
                                    g.delete_edge(ebase + t2[1].edge_id());
                                    gcmark.edge(ebase + t2[1].edge_id())->dashed = true;
                                    gjmark.edge(ebase + t2[1].edge_id())->dashed = true;
                                }
                                if (ex1 != 5 && ex2 != 5) {
                                    g.delete_edge(ebase + t2[2].edge_id());
                                    gcmark.edge(ebase + t2[2].edge_id())->dashed = true;
                                    gjmark.edge(ebase + t2[2].edge_id())->dashed = true;
                                }

                                //DBG( g.graphviz() );
                                //DBG( gcmark.graphviz() );

                                bool is_bad =
                                    (g.get_min_degree() <= 1) ||
                                    (g.count_edges(tv1[0].vertex_id(), tv1[1].vertex_id()) >= 3) ||
                                    (g.count_edges(tv1[0].vertex_id(), tv1[2].vertex_id()) >= 3) ||
                                    (g.count_edges(tv1[1].vertex_id(), tv1[2].vertex_id()) >= 3);

                                DBGG0("is_bad = " << is_bad);

                                BaseGraph gr = g.clone_reorder_degree<BaseGraph>();

                                //DBG( g.graphviz() );

                                //AlgBispanning<BaseGraph> ab(gr);

                                //if (!ab.good()) {

                                if (!gr.is_bispanning_colored()) {

                                    map_bad++;

                                    //if (isocheck(gr)) {

                                    //DBG( gjmark.graphviz() );
                                    DBG(gcmark.graphviz());
                                    //DBG( gr.graphviz() );

                                    std::cout << "// Not a bispanner: " << write_graph6(gr) << std::endl;
                                    //return;
                                    //}
                                }
                                else {
                                    map_good++;
                                }

                                //ab.write(g);

                                //DBG( g.graphviz() );
                            }
                        }

                        std::cerr << "maps: " << map_good << " good, " << map_bad << " bad." << std::endl;
                        if (map_good == 0)
                        {
                            DBG(gjoin.graphviz());
                            DBG(gcontr.graphviz());
                            abort();
                        }
                    }
                    while (std::next_permutation(mapping.begin(), mapping.end()));

                    //return;
                }
            }
        }
    }
}

void post_process_3sums_colors()
{
    using id_type = BaseGraph::id_type;
    using vertex = BaseGraph::const_vertex_iterator;
    using edge = BaseGraph::const_edge_iterator;
    using triangle = BaseGraph::triangle_type;
    using triangle_vertex = BaseGraph::triangle_vertex_type;

    using tvertex = TauGraph::const_vertex_iterator;

    static const bool debug = true;

    BaseGraph::IsomorphismCheck isocheck;

    for (const BaseGraph& g1 : g_graphs)
    {
        for (const BaseGraph& g2 : g_graphs)
        {
            DBGG1("g1: " << write_graph6(g1) << " g2: " << write_graph6(g2));

            DBG0(g1.graphviz());
            DBG0(g2.graphviz());

            TauGraph tau1 = AlgGame<BaseGraph>(g1).tau_;
            TauGraph tau2 = AlgGame<BaseGraph>(g2).tau_;

            std::vector<triangle> triangles1 = g1.find_triangles();
            std::vector<triangle> triangles2 = g2.find_triangles();

            for (const triangle& tri1 : triangles1)
            {
                if (g1.count_edges(tri1[0]) != 1) continue;
                if (g1.count_edges(tri1[1]) != 1) continue;
                if (g1.count_edges(tri1[2]) != 1) continue;

                for (const triangle& tri2 : triangles2)
                {
                    if (g2.count_edges(tri2[0]) != 1) continue;
                    if (g2.count_edges(tri2[1]) != 1) continue;
                    if (g2.count_edges(tri2[2]) != 1) continue;

                    for (tvertex tv1 : tau1.vertex_list())
                    {
                        for (tvertex tv2 : tau2.vertex_list())
                        {
                            BaseGraph gc1 = g1, gc2 = g2;
                            tv1->tree.colorize_graph(gc1);
                            tv2->tree.colorize_graph(gc2);

                            for (auto e : gc2.edge_list()) e->dashed = true;

                            BaseGraph gjoin = gc1;

                            id_type vbase = gjoin.total_vertex();
                            id_type ebase = gjoin.total_edge();

                            gjoin.add_copy(gc2);
                            DBG0(gjoin.graphviz());

                            BaseGraph gcontr = gjoin;

                            triangle_vertex trv1 = gcontr.get_triangle_vertices(tri1);
                            triangle_vertex trv2 = gcontr.get_triangle_vertices(tri2);

                            gcontr.contract(trv1[0].vertex_id(), vbase + trv2[0].vertex_id());
                            gcontr.contract(trv1[1].vertex_id(), vbase + trv2[1].vertex_id());
                            gcontr.contract(trv1[2].vertex_id(), vbase + trv2[2].vertex_id());

                            BaseGraph gcontrx = gcontr;

                            gcontr.delete_edge(tri1[0].edge_id());
                            gcontr.delete_edge(tri1[1].edge_id());
                            gcontr.delete_edge(ebase + tri2[1].edge_id());
                            gcontr.delete_edge(ebase + tri2[2].edge_id());

                            if (!gcontr.is_cyclic(BaseGraph::ColorFilter(BLUE)) &&
                                !gcontr.is_cyclic(BaseGraph::ColorFilter(RED)))
                            {
                                DBG(gcontrx.graphviz());
                                DBG(gcontr.graphviz());
                            }
                        }
                    }

                    return;
                }
            }
        }
    }
}

#endif // !BISPANNING_PLAY_CLIQUESUM_HEADER

/******************************************************************************/
