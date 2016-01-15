/*******************************************************************************
 * src/decomposition.h
 *
 * Test decomposition Cartesian graph product for composite bispanners, also
 * test for an explicit 1-sum of two bispanners.
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

#ifndef BISPANNING_DECOMPOSITION_HEADER
#define BISPANNING_DECOMPOSITION_HEADER

void test_decomposition_composite(
    const EditBaseGraph& g, const std::vector<size_t>& cutset)
{
    static const bool debug = false;

    EditBaseGraph gcontr = g;
    EditBaseGraph gdelete = g;

    for (size_t i = 0; i < cutset.size(); ++i)
    {
        if (cutset[i]) {
            gdelete.delete_edge(i);
        }
        else {
            if (!gcontr.edge_deleted(i)) {
                gcontr.contract(gcontr.edge(i));
            }
        }
    }

    DBG(gcontr.graphviz());
    DBG(gdelete.graphviz());

    gcontr = gcontr.clone_reorder_degree<EditBaseGraph>();
    TauGraph tauc = AlgGame<EditBaseGraph>(gcontr).tau_;

    DBG(graphviz(tauc, TauGraphDecorator()));

    for (size_t r = 0; r < gdelete.total_vertex(); ++r)
    {
        if (gdelete.vertex_deleted(r)) continue;

        std::vector<unsigned int> comp = gdelete.get_connected_undirected(r);

        DBGE("comp = " << comp);

        std::vector<int> keep(gdelete.total_vertex(), 0);

        for (size_t ci = 0; ci < comp.size(); ++ci)
            keep[comp[ci]] = 1;

        EditBaseGraph gdel2 = gdelete;

        for (size_t ki = 0; ki < keep.size(); ++ki)
        {
            if (keep[ki]) {
                gdelete.delete_vertex(ki);
            }
            else if (!gdel2.vertex_deleted(ki)) {
                gdel2.delete_vertex(ki);
            }
        }

        DBG(gdelete.graphviz());
        DBG(gdel2.graphviz());

        if (gdel2.num_vertex() <= 1) continue;

        gdel2 = gdel2.clone_reorder_degree<EditBaseGraph>();
        DBG(gdel2.graphviz());

        TauGraph tauAdd = AlgGame<EditBaseGraph>(gdel2).tau_;

        DBG(graphviz(tauAdd, TauGraphDecorator()));

        TauProductDataFactory taufactory(0);
        tauc = TauGraph::product<TauGraph>(tauc, tauAdd, taufactory);

        DBG(graphviz(tauc, TauGraphDecorator()));
    }

    TauGraph tau = AlgGame<EditBaseGraph>(g).tau_;
    DBG(graphviz(tau, TauGraphDecorator()));

    iGraph<TauGraph> itau(tau);
    iGraph<TauGraph> itauc(tauc);

    bool iso = itau.isomorphic(itauc);
    std::cerr << "isomorphic composite: " << iso << "\n";
    if (!iso) abort();
}

void test_decomposition_2sum(const EditBaseGraph& g, const std::vector<size_t>& cutset)
{
    using id_type = EditBaseGraph::id_type;
    using edge = EditBaseGraph::edge_iterator;

    using tvertex = TauGraph::vertex_iterator;
    using tedge = TauGraph::edge_iterator;
    using tvertex_edge = TauGraph::vertex_edge_iterator;

    static const bool debug = false;

    DBG(g.graphviz());

    EditBaseGraph gcut = g;
    int vother = -1;
    id_type vcut1 = EditBaseGraph::ID_INVALID, vcut2 = EditBaseGraph::ID_INVALID;

    for (size_t i = 0; i < cutset.size(); ++i)
    {
        if (cutset[i]) {
            gcut.delete_vertex(i);

            if (vcut1 == EditBaseGraph::ID_INVALID) vcut1 = i;
            else if (vcut2 == EditBaseGraph::ID_INVALID) vcut2 = i;
            else abort();
        }
        else if (vother == -1) {
            vother = i; // find any other verex
        }
    }

    assert(g.count_edges(vcut1, vcut2) == 0);

    DBG(gcut.graphviz());

    std::vector<id_type> comp0 = gcut.get_connected_undirected(vother);

    DBGG("comp0 " << comp0);

    // mark vertices in comp0 = 0, comp1 = 1 and both = 2
    std::vector<unsigned int> vcompselect(g.num_vertex(), 0);

    for (size_t i = 0; i < comp0.size(); ++i)
        vcompselect[comp0[i]] = 1;

    vcompselect[vcut1] = 2;
    vcompselect[vcut2] = 2;

    EditBaseGraph g1 = g, g2 = g;

    for (size_t i = 0; i < vcompselect.size(); ++i)
    {
        if (vcompselect[i] == 0) {
            g1.delete_vertex(i);
        }
        else if (vcompselect[i] == 1) {
            g2.delete_vertex(i);
        }
    }

    // add 2-sum edges
    edge es1 = g1.add_edge(vcut1, vcut2);
    g2.add_deleted_edge();
    edge es2 = g2.add_edge(vcut1, vcut2);

    es1->color = BLUE, es2->color = RED;

    if (g1.is_cyclic(EditBaseGraph::ColorFilter(BLUE)) ||
        g1.is_cyclic(EditBaseGraph::ColorFilter(RED)))
    {
        std::swap(es1->color, es2->color);
    }

    if (g1.is_cyclic(EditBaseGraph::ColorFilter(BLUE)) ||
        g1.is_cyclic(EditBaseGraph::ColorFilter(RED)) ||
        g2.is_cyclic(EditBaseGraph::ColorFilter(BLUE)) ||
        g2.is_cyclic(EditBaseGraph::ColorFilter(RED)))
    {
        DBGG1("This should be impossible.");
        abort();
    }

    DBG(g1.graphviz());
    DBG(g2.graphviz());

    // generate tau graphs
    TauGraph tau1 = AlgGame<EditBaseGraph>(g1).tau_;
    TauGraph tau2 = AlgGame<EditBaseGraph>(g2).tau_;

    DBG(graphviz(tau1, TauGraphDecorator()));
    DBG(graphviz(tau2, TauGraphDecorator()));

    // calculate product
    TauProductDataFactory taufactory(0);
    TauGraph taup = TauGraph::product<TauGraph>(tau1, tau2, taufactory);

    DBG(graphviz(taup, TauGraphDecorator()));

    DBGE("tau1: " << tau1.num_vertex() << " - " << tau1.num_edge());
    DBGE("tau2: " << tau2.num_vertex() << " - " << tau2.num_edge());
    DBGE("taup: " << taup.num_vertex() << " - " << taup.num_edge());

    id_type ec1 = es1.edge_id(), ec2 = es2.edge_id();
    DBGE("ec1 " << ec1 << " - ec2 " << ec2);

    std::multimap<tvertex, tedge> inbound;

    for (tedge e : taup.edge_list())
    {
        tvertex hv = e.head();

        if ((hv->tree.contains(ec1) && hv->tree.contains(ec2)) ||
            (!hv->tree.contains(ec1) && !hv->tree.contains(ec2)))
        {
            inbound.insert(std::make_pair(hv, e));
        }
    }

    for (auto v : taup.vertex_list())
    {
        if (v->tree.contains(ec1) && v->tree.contains(ec2))
        {
            std::vector<tedge> v_in1, v_out1;
            std::vector<tedge> v_in2, v_out2;

            // find all inbound edges without ec1 & ec2 (thus the edge ec1/ec2 is swapped in)
            for (auto tn = inbound.lower_bound(v); tn != inbound.upper_bound(v); ++tn)
            {
                tedge tne = tn->second;

                tvertex ve = tne.other(v);
                assert(ve == tne.tail());

                DBGG0("1x" << ve << " " << ve->tree << " -> " << tne << " -> " << v->tree << " - " << *tne);

                if (tne->UEout != ec1 && tne->UEout != ec2) continue;

                assert(!(ve->tree.contains(ec1) && ve->tree.contains(ec2)) &&
                       !(!ve->tree.contains(ec1) && !ve->tree.contains(ec2)));

                DBGG("1 " << ve << " " << ve->tree << " -> " << tne << " -> " << v->tree << " - " << *tne);

                if (tne->UEout == ec1)
                    v_in1.push_back(tne);
                if (tne->UEout == ec2)
                    v_in2.push_back(tne);
            }

            // find all neighbors without ec2 (thus the edge ec2 is swapped in)
            for (tvertex_edge tne : v.edge_list())
            {
                tvertex ve = tne.other(v);
                assert(ve == tne.head());

                DBGG0("2x" << v << " " << v->tree << " -> " << tne << " -> " << ve->tree << " - " << *tne);

                if (tne->UEin != ec1 && tne->UEin != ec2) continue;

                assert(!(ve->tree.contains(ec1) && ve->tree.contains(ec2)) &&
                       !(!ve->tree.contains(ec1) && !ve->tree.contains(ec2)));

                DBGG("2 " << v << " " << v->tree << " -> " << tne << " -> " << ve->tree << " - " << *tne);

                if (tne->UEin == ec1)
                    v_out1.push_back(tne);
                if (tne->UEin == ec2)
                    v_out2.push_back(tne);
            }

            if (v_in1.size() == 1 && v_out2.size() == 1)
            {
                tedge e1 = v_in1[0], e2 = v_out2[0];

                tvertex v1 = e1.other(v);
                tvertex v2 = e2.other(v);

                DBGG("adding: " << e1 << " - " << e2 <<
                     " : " << v1 << v1->tree << " -> " << e1->UEin << "/" << e2->UEout << " -> " << v2 << v2->tree);

                taup.add_edge(v1, v2, TauEdge(3, e1->UEin, e2->UEout))->dashed = true;
            }
            if (v_in2.size() == 1 && v_out1.size() == 1)
            {
                tedge e1 = v_in2[0], e2 = v_out1[0];

                tvertex v1 = e1.other(v);
                tvertex v2 = e2.other(v);

                DBGG("adding: " << e1 << " - " << e2 <<
                     " : " << v1 << v1->tree << " -> " << e1->UEin << "/" << e2->UEout << " -> " << v2 << v2->tree);

                taup.add_edge(v1, v2, TauEdge(3, e1->UEin, e2->UEout))->dashed = true;
            }
        }
#if 1
        else if (!v->tree.contains(ec1) && !v->tree.contains(ec2))
        {
            std::vector<tedge> v_in1, v_out1;
            std::vector<tedge> v_in2, v_out2;

            // find all neighbors without ec1 (thus the edge ec1 is swapped in)
            for (auto tn = inbound.lower_bound(v); tn != inbound.upper_bound(v); ++tn)
            {
                tedge tne = tn->second;

                tvertex ve = tne.other(v);
                assert(ve == tne.tail());

                DBGG0("1x" << ve << " " << ve->tree << " -> " << tne << " -> " << v->tree << " - " << *tne);

                if (tne->UEout != ec1 && tne->UEout != ec2) continue;

                assert(!(ve->tree.contains(ec1) && ve->tree.contains(ec2)) &&
                       !(!ve->tree.contains(ec1) && !ve->tree.contains(ec2)));

                DBGG("1 " << ve << " " << ve->tree << " -> " << v << " " << v->tree << " - " << *tne);

                if (tne->UEout == ec1)
                    v_in1.push_back(tne);
                if (tne->UEout == ec2)
                    v_in2.push_back(tne);
            }

            // find all neighbors without ec2 (thus the edge ec2 is swapped in)
            for (tvertex_edge tne : v.edge_list())
            {
                tvertex ve = tne.other(v);
                assert(ve == tne.head());

                DBGG0("2x" << v << " " << v->tree << " -> " << tne << " -> " << ve->tree << " - " << *tne);

                if (tne->UEin != ec1 && tne->UEin != ec2) continue;

                assert(!(ve->tree.contains(ec1) && ve->tree.contains(ec2)) &&
                       !(!ve->tree.contains(ec1) && !ve->tree.contains(ec2)));

                DBGG("2 " << v << " " << v->tree << " -> " << tne << " -> " << ve->tree << " - " << *tne);

                if (tne->UEin == ec1)
                    v_out1.push_back(tne);
                if (tne->UEin == ec2)
                    v_out2.push_back(tne);
            }

            if (v_in1.size() == 1 && v_out2.size() == 1)
            {
                tedge e1 = v_in1[0], e2 = v_out2[0];

                tvertex v1 = e1.other(v);
                tvertex v2 = e2.other(v);

                DBGG("adding: " << e1 << " - " << e2 <<
                     " : " << v1 << v1->tree << " -> " << e1->UEin << "/" << e2->UEout << " -> " << v2 << v2->tree);

                taup.add_edge(v1, v2, TauEdge(3, e1->UEin, e2->UEout))->dashed = true;
            }
            if (v_in2.size() == 1 && v_out1.size() == 1)
            {
                tedge e1 = v_in2[0], e2 = v_out1[0];

                tvertex v1 = e1.other(v);
                tvertex v2 = e2.other(v);

                DBGG("adding: " << e1 << " - " << e2 <<
                     " : " << v1 << v1->tree << " -> " << e1->UEin << "/" << e2->UEout << " -> " << v2 << v2->tree);

                taup.add_edge(v1, v2, TauEdge(3, e1->UEin, e2->UEout))->dashed = true;
            }
        }
#endif
    }

    for (auto v : taup.vertex_list())
    {
        if ((v->tree.contains(ec1) && v->tree.contains(ec2)) ||
            (!v->tree.contains(ec1) && !v->tree.contains(ec2)))
        {
            DBGG("deleted: " << v->tree);
            taup.delete_vertex(v);
        }
    }

    TauGraph tau = AlgGame<EditBaseGraph>(g).tau_;

    DBG(graphviz(tau, TauGraphDecorator()));
    DBG(graphviz(taup, TauGraphDecorator()));

    DBGE("tau: " << tau.num_vertex() << " - " << tau.num_edge());
    DBGE("taup: " << taup.num_vertex() << " - " << taup.num_edge());

    TauGraph taud = taup;
    tau.rebuild_treemap();

    for (tedge ei : taud.edge_list())
    {
        Tree t1 = ei.head()->tree;
        if (t1.contains(ec1)) t1.erase(ec1);
        if (t1.contains(ec2)) t1.erase(ec2);

        Tree t2 = ei.tail()->tree;
        if (t2.contains(ec1)) t2.erase(ec1);
        if (t2.contains(ec2)) t2.erase(ec2);

        tvertex v1 = tau.find_vertex(t1);
        tvertex v2 = tau.find_vertex(t2);

        assert(v1 != tau.vertex_end());
        assert(v2 != tau.vertex_end());

        if (tau.count_edges(v1, v2))
        {
            taud.delete_edge(ei);
        }
    }

    DBGE("taud: " << taud.num_vertex() << " - " << taud.num_edge());

    DBG(graphviz(taud, TauGraphDecorator()));

    taup = taup.clone_reorder_degree<TauGraph>();

    iGraph<TauGraph> itaup(taup);
    iGraph<TauGraph> itau(tau);

    bool iso = itaup.isomorphic(itau);
    std::cerr << "isomorphic 2sum: " << iso << "\n";
    if (!iso) abort();
}

bool is_leafUE(const BaseGraph& gv, BaseGraph::id_type e0, BaseGraph::id_type e1)
{
    using vertex = BaseGraph::const_vertex_iterator;
    using edge = BaseGraph::const_edge_iterator;
    using vertex_edge = BaseGraph::const_vertex_edge_iterator;

    edge swap_e0 = gv.edge(e0), swap_e1 = gv.edge(e1);

    if (!(swap_e0.head() == swap_e1.head() ||
          swap_e0.head() == swap_e1.tail() ||
          swap_e0.tail() == swap_e1.head() ||
          swap_e0.tail() == swap_e1.tail()))
        return false;

    size_t in_head = 0, in_tail = 0;

    for (vertex_edge ve : swap_e0.head().edge_list()) {
        if (ve->color == swap_e0->color)
            ++in_head;
    }
    for (vertex_edge ve : swap_e0.tail().edge_list()) {
        if (ve->color == swap_e0->color)
            ++in_tail;
    }

    // std::cerr << "head/tail: " << in_head << " - " << in_tail << "\n";

    return (in_head == 1 || in_tail == 1);
}

TauGraph test_3deg_stripUEs(
    const EditBaseGraph& base, const TauGraph& _tau,
    EditBaseGraph::id_type esplit, EditBaseGraph::id_type vattach,
    EditBaseGraph::id_type e0, EditBaseGraph::id_type e1,
    EditBaseGraph::id_type e2,
    const EditBaseGraph& _g0, const TauGraph& _tau0)
{
    using id_type = EditBaseGraph::id_type;
    using vertex = EditBaseGraph::vertex_iterator;
    using edge = EditBaseGraph::edge_iterator;

    using tvertex = TauGraph::vertex_iterator;
    using tedge = TauGraph::edge_iterator;
    using tvertex_edge = TauGraph::vertex_edge_iterator;

    static const bool debug = false;

    TauGraph tau = _tau;

    DBGG1("process " << write_graph6(base));

    // delete edges between subgraphs containing e0+e1 and not(e0+e1)

    for (tedge te : tau.edge_list())
    {
        tvertex v0 = te.head(), v1 = te.tail();

        if (v0->tree.contains(esplit) != v1->tree.contains(esplit))
            tau.delete_edge(te);
    }

    // delete edges that are no longer UEs

    unsigned int total_broken = 0;
    std::vector<std::tuple<id_type, id_type, int> > delete_pair;

    TauGraph tauc = tau;

    for (tvertex tv : tau.vertex_list())
    {
        EditBaseGraph gx = base;

        tv->tree.colorize_graph(gx);

        edge gxe1 = gx.edge(esplit);

        int ecolor = gxe1->color;
        int other_color = ecolor == RED ? BLUE : RED;
        gxe1->color = other_color;

        std::vector<edge> cutedge = gx.calc_cut_edges(gxe1);

        std::vector<edge> attpath =
            gx.find_color_path(vattach, gxe1.tail_id(), other_color);

        if (attpath.back().edge_id() == gxe1.edge_id())
            attpath.pop_back();

        std::sort(attpath.begin(), attpath.end());

        std::vector<edge> inter;
        std::set_intersection(cutedge.begin(), cutedge.end(),
                              attpath.begin(), attpath.end(),
                              std::back_inserter(inter));

        std::ostringstream oss;
        oss << tv->label() << "\\n"
            << "cuts: " << cutedge << "\\n"
            << "path: " << attpath << "\\n"
            << "inter: " << inter << "\\n";
        gx.m_title = oss.str();

        gxe1->color = ecolor;

        DBG(gx.graphviz());

        DBGG(oss.str());

        // cannot delete right away, because deletion changes order in vertex's edgelists.
        std::vector<id_type> to_delete;
        for (tvertex_edge tve : tv.edge_list())
        {
            if (vector_contains(inter, gx.edge(tve->UEin)))
            {
                to_delete.push_back(tve.edge_id());
                DBGG1("BrokenUE: " << tve << " - " << *tve);
            }
        }

        total_broken += to_delete.size();

        for (id_type& ei : to_delete)
        {
            delete_pair.push_back(std::make_tuple(tau.edge(ei).tail_id(), tau.edge(ei).head_id(), tau.edge(ei)->colors));

            tau.delete_edge(ei);
            tauc.edge(ei)->dashed = true;
        }
    }

    std::cerr << "broken count = " << total_broken << " - " << write_graph6(_g0) << " - " << base.graphstring() << "\n";

    tau.m_title = "Removed edges";
    DBG(graphviz(tau, TauGraphDecorator()));

    TauGraph tau0 = _tau0;

    for (tvertex tv : tau0.vertex_list())
    {
        tv->seen = false;
    }

    for (tvertex tv : tau.vertex_list())
    {
        // translate tree from g1 in tau to g0 in tau0
        Tree t0 = tv->tree;

        if (t0.contains(esplit)) {
            t0.erase(esplit);
            t0.insert_one(e0);
            t0.insert_one(e1);
        }
        else {
            t0.insert_one(e2);
        }

        tvertex t0v = tau0.find_vertex(t0);

        if (t0v != tau0.vertex_end())
            t0v->seen = true;
    }

    for (tvertex tv : tau0.vertex_list())
    {
        if (!tv->seen) {
            // std::cerr << "delete " << tv << std::endl;
            tau0.delete_vertex(tv);
        }
    }

    for (const auto& dp : delete_pair)
    {
        (void)dp;
#if 0
        unsigned int dist = tau.find_path(dp.first, dp.second).size();
        std::cerr << "distance: " << dist << "\n";
        if (dist >= 5 && 0) {
            EditBaseGraph b1 = base, b2 = base;
            tau.vertex(dp.first)->tree.colorize_graph(b1);
            tau.vertex(dp.second)->tree.colorize_graph(b2);
            std::cerr << "broken from " << graphstring(b1, GraphDecoratorS()) << " to " << graphstring(b2, GraphDecoratorS()) << "\n";

            DBG1(b1.graphviz());
            DBG1(b2.graphviz());
            DBG1(graphviz(tauc, TauGraphDecorator()));
            abort();
        }
#endif

#if 0
        // calculate distance of broken UE -> turns out this is not always <= 3!
        tvertex vv0 = tau.vertex(std::get<0>(dp));
        tvertex vw0 = tau.vertex(std::get<1>(dp));

        if (tau.get_distance(vv0, vw0) >= 3)
        {
            DBGE("distance = " << tau.get_distance(vv0, vw0));

            EditBaseGraph b1 = base, b2 = base;
            vv0->tree.colorize_graph(b1);
            vw0->tree.colorize_graph(b2);

            DBGE("broken from " << graphstring(b1, GraphDecoratorS()) << " to " << graphstring(b2, GraphDecoratorS()));

            DBG1(b1.graphviz());

            TauGraph bfs = tau.get_all_shortest_path_graph(vv0, vw0);
            std::cout << graphviz(bfs, TauGraphDecorator()) << "\n";

            TauGraph bfs2 = tau.get_all_shortest_path_graph(vv0, vw0, TauColorMaskFilter(std::get<2>(dp)));
            std::cout << graphviz(bfs2, TauGraphDecorator()) << "\n";
        }
#endif
    }

    tau0.m_title = "tau0 restricted to g0";

    DBG(graphviz(tau0, TauGraphDecorator()));

    return tau;
}

//! Test hypothesis about contraction and deletion of arbitrary edges in atomic
//! bispanners.
void bispanner_test_contraction(const BaseGraph& _g)
{
    // pick any pair of edges:
    for (size_t ei = 0; ei < _g.num_edge(); ++ei)
    {
        for (size_t ej = ei + 1; ej < _g.num_edge(); ++ej)
        {
            BaseGraph g1 = _g, g2 = _g;

            g1.contract(g1.edge(ei));
            g1.delete_edge(ej);

            g2.delete_edge(ei);
            g2.contract(g2.edge(ej));

            BaseGraph g1x = g1.clone_reorder_degree<BaseGraph>();
            BaseGraph g2x = g2.clone_reorder_degree<BaseGraph>();

            AlgBispanning<BaseGraph> g1b(g1x);
            if (!g1b.good()) {
                std::cerr << "Not a bispanner" << std::endl;
                abort();
            }

            AlgBispanning<BaseGraph> g2b(g2x);
            if (!g2b.good()) {
                std::cerr << "Not a bispanner" << std::endl;
                abort();
            }
        }
    }
}

void test_decomposition_3deg(const BaseGraph& _g0, int vdeg3id)
{
    EditBaseGraph g0 = EditBaseGraph(_g0);

    using id_type = EditBaseGraph::id_type;
    using vertex = EditBaseGraph::vertex_iterator;
    using edge = EditBaseGraph::edge_iterator;

    using tvertex = TauGraph::vertex_iterator;
    using tedge = TauGraph::edge_iterator;
    using tvertex_edge = TauGraph::vertex_edge_iterator;

    static const bool debug = false;

    g0.m_title = "base graph";
    DBG(g0.graphviz());

    // find the three edges of vdeg3

    vertex vdeg3 = g0.vertex(vdeg3id);
    assert(vdeg3.degree() == 3);
    DBGE("vdeg3 = " << vdeg3.vertex_id());

    edge e0 = g0.vertex_edge(vdeg3, 0);
    edge e1 = g0.vertex_edge(vdeg3, 1);
    edge e2 = g0.vertex_edge(vdeg3, 2);

    id_type v0 = e0.other(vdeg3).vertex_id();
    id_type v1 = e1.other(vdeg3).vertex_id();
    id_type v2 = e2.other(vdeg3).vertex_id();

    // construct component graphs

    EditBaseGraph g1 = EditBaseGraph(g0);
    EditBaseGraph g2 = EditBaseGraph(g0);
    EditBaseGraph g3 = EditBaseGraph(g0);

    g1.delete_vertex(vdeg3.vertex_id());
    edge g1e = g1.add_edge(v0, v1);

    g2.delete_vertex(vdeg3.vertex_id());
    edge g2e = g2.add_edge(v0, v2);

    g3.delete_vertex(vdeg3.vertex_id());
    edge g3e = g3.add_edge(v1, v2);

    // added split edges all have the same id (|V| + 1).

    assert(g1e.edge_id() == g2e.edge_id());
    assert(g1e.edge_id() == g3e.edge_id());

    id_type ge_split = g1e.edge_id();

    // reconstruct bispanning property

    g1.m_title = "part 1";
    g2.m_title = "part 2";
    g3.m_title = "part 3";

    DBG0(g1.graphviz());
    DBG0(g2.graphviz());
    DBG0(g3.graphviz());

    {
        AlgBispanning<EditBaseGraph> g1bi(g1);
        if (!g1bi.good()) {
            DBG3(true, g0.graphviz());
            DBG3(true, g1.graphviz());
            abort();
        }
        g1bi.write(g1);
    }
    {
        AlgBispanning<EditBaseGraph> g2bi(g2);
        if (!g2bi.good()) {
            DBG3(true, g0.graphviz());
            DBG3(true, g2.graphviz());
            abort();
        }
        g2bi.write(g2);
    }
    {
        AlgBispanning<EditBaseGraph> g3bi(g3);
        if (!g3bi.good()) {
            DBG3(true, g0.graphviz());
            DBG3(true, g3.graphviz());
            abort();
        }
        g3bi.write(g3);
    }

    DBG(g1.graphviz());
    DBG(g2.graphviz());
    DBG(g3.graphviz());

    // construct tau graphs

    TauGraph tau0 = AlgGame<EditBaseGraph>(g0).tau_;

    TauGraph tau1 = AlgGame<EditBaseGraph>(g1).tau_;
    TauGraph tau2 = AlgGame<EditBaseGraph>(g2).tau_;
    TauGraph tau3 = AlgGame<EditBaseGraph>(g3).tau_;

    if (tau0.num_vertex() !=
        tau1.num_vertex() + tau2.num_vertex() + tau3.num_vertex())
    {
        DBGE("tau vertex sum not matching!");
        abort();
    }
    else
    {
        DBGE("tau vertex sum matching!");
    }

    tau0.m_title = "tau0";
    DBG(graphviz(tau0, TauGraphDecorator()));

    DBG(graphviz(tau1, TauGraphDecorator()));
    DBG(graphviz(tau2, TauGraphDecorator()));
    DBG(graphviz(tau3, TauGraphDecorator()));

    if (0)
    {
        DBGE("diam0 = " << tau0.get_diameter());

        DBGE("diam1 = " << tau1.get_diameter());
        DBGE("diam2 = " << tau2.get_diameter());
        DBGE("diam3 = " << tau3.get_diameter());
    }

#if 0
    // Take tau1,tau2,tau3 and remove split_edge UEs
    {
        TauGraph tau1s =
            test_3deg_stripUEs(g1, tau1, g1e.edge_id(), v2,
                               e0.edge_id(), e1.edge_id(), e2.edge_id(),
                               g0, tau0);

        TauGraph tau2s =
            test_3deg_stripUEs(g2, tau2, g2e.edge_id(), v1,
                               e0.edge_id(), e2.edge_id(), e1.edge_id(),
                               g0, tau0);

        TauGraph tau3s =
            test_3deg_stripUEs(g3, tau3, g3e.edge_id(), v0,
                               e1.edge_id(), e2.edge_id(), e0.edge_id(),
                               g0, tau0);

        DBG0(graphviz(tau1s, TauGraphDecorator()));
        DBG0(graphviz(tau2s, TauGraphDecorator()));
        DBG0(graphviz(tau3s, TauGraphDecorator()));

        // Calculate diameter of tau1,tau2,tau3

        if (tau1s.get_num_components() != 2 ||
            tau2s.get_num_components() != 2 ||
            tau3s.get_num_components() != 2)
        {
            DBGE("Component number not two.");
            exit(1);
        }

        std::vector<id_type> tau1c = tau1s.get_component_id_undirected();
        std::vector<id_type> tau2c = tau2s.get_component_id_undirected();
        std::vector<id_type> tau3c = tau3s.get_component_id_undirected();

        // DBGE(tau1c);
        // DBGE(tau2c);
        // DBGE(tau3c);

        TauGraph tau1a = tau1s, tau1b = tau1s;
        TauGraph tau2a = tau2s, tau2b = tau2s;
        TauGraph tau3a = tau3s, tau3b = tau3s;

        for (size_t i = 0; i < tau1c.size(); ++i)
        {
            if (tau1c[i] == 0)
                tau1a.delete_vertex(i);
            else
                tau1b.delete_vertex(i);
        }

        for (size_t i = 0; i < tau2c.size(); ++i)
        {
            if (tau2c[i] == 0)
                tau2a.delete_vertex(i);
            else
                tau2b.delete_vertex(i);
        }

        for (size_t i = 0; i < tau3c.size(); ++i)
        {
            if (tau3c[i] == 0)
                tau3a.delete_vertex(i);
            else
                tau3b.delete_vertex(i);
        }

        tau1a.m_title = "tau1a";
        tau1b.m_title = "tau1b";
        tau2a.m_title = "tau2a";
        tau2b.m_title = "tau2b";
        tau3a.m_title = "tau3a";
        tau3b.m_title = "tau3b";

        DBG(graphviz(tau1a, TauGraphDecorator()));
        DBG(graphviz(tau1b, TauGraphDecorator()));
        DBG(graphviz(tau2a, TauGraphDecorator()));
        DBG(graphviz(tau2b, TauGraphDecorator()));
        DBG(graphviz(tau3a, TauGraphDecorator()));
        DBG(graphviz(tau3b, TauGraphDecorator()));

        // DBGE("diam1a = " << tau1a.get_diameter());
        // DBGE("diam1b = " << tau1b.get_diameter());
        // DBGE("diam2a = " << tau2a.get_diameter());
        // DBGE("diam2b = " << tau2b.get_diameter());
        // DBGE("diam3a = " << tau3a.get_diameter());
        // DBGE("diam3b = " << tau3b.get_diameter());

        DBGE("conn1a = " << tau1a.is_connected());
        DBGE("conn1b = " << tau1b.is_connected());
        DBGE("conn2a = " << tau2a.is_connected());
        DBGE("conn2b = " << tau2b.is_connected());
        DBGE("conn3a = " << tau3a.is_connected());
        DBGE("conn3b = " << tau3b.is_connected());

        DBGE("conn1a = " << tau1a.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau1a.is_connected(TauColorMaskFilter(RED + 1)));
        DBGE("conn1b = " << tau1b.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau1b.is_connected(TauColorMaskFilter(RED + 1)));
        DBGE("conn2a = " << tau2a.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau2a.is_connected(TauColorMaskFilter(RED + 1)));
        DBGE("conn2b = " << tau2b.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau2b.is_connected(TauColorMaskFilter(RED + 1)));
        DBGE("conn3a = " << tau3a.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau3a.is_connected(TauColorMaskFilter(RED + 1)));
        DBGE("conn3b = " << tau3b.is_connected(TauColorMaskFilter(BLUE + 1)) <<
             " / " << tau3b.is_connected(TauColorMaskFilter(RED + 1)));
    }
#endif

    // lift tree vectors in tau from small graphs back to large graph

    for (tvertex tv : tau1.vertex_list())
    {
        if (tv->tree.contains(g1e.edge_id())) {
            tv->tree.erase(g1e.edge_id());
            tv->tree.insert_one(e0.edge_id());
            tv->tree.insert_one(e1.edge_id());
        }
        else {
            tv->tree.insert_one(e2.edge_id());
        }
    }

    for (tvertex tv : tau2.vertex_list())
    {
        if (tv->tree.contains(g2e.edge_id())) {
            tv->tree.erase(g2e.edge_id());
            tv->tree.insert_one(e0.edge_id());
            tv->tree.insert_one(e2.edge_id());
        }
        else {
            tv->tree.insert_one(e1.edge_id());
        }
    }

    for (tvertex tv : tau3.vertex_list())
    {
        if (tv->tree.contains(g3e.edge_id())) {
            tv->tree.erase(g3e.edge_id());
            tv->tree.insert_one(e1.edge_id());
            tv->tree.insert_one(e2.edge_id());
        }
        else {
            tv->tree.insert_one(e0.edge_id());
        }
    }

    DBG0(graphviz(tau1, TauGraphDecorator()));
    DBG0(graphviz(tau2, TauGraphDecorator()));
    DBG0(graphviz(tau3, TauGraphDecorator()));

    // Construct tau from tau1,tau2,tau3

    TauGraph tau = tau1;
    tau.add_copy(tau2);
    tau.add_copy(tau3);
    tau.rebuild_treemap();

    DBG(graphviz(tau, TauGraphDecorator()));

    // delete edges that are no longer UEs

    DBGE("e0 " << e0.edge_id() << " e1 " << e1.edge_id() << " e2 " << e2.edge_id());

    std::set<id_type> no_delete;

    for (tvertex vv : tau.vertex_list())
    {
        Tree& tv = vv->tree;

        EditBaseGraph gv = tv.get_colorized_graph(g0);

        id_type e_single =
            tv.contains(e1.edge_id()) == tv.contains(e2.edge_id()) ? e0.edge_id() :
            tv.contains(e0.edge_id()) == tv.contains(e2.edge_id()) ? e1.edge_id() :
            tv.contains(e0.edge_id()) == tv.contains(e1.edge_id()) ? e2.edge_id() :
            BaseGraph::ID_INVALID;

        assert(e_single != BaseGraph::ID_INVALID);

        id_type v_single = gv.edge(e_single).other_id(vdeg3id);

        int single_color = gv.edge(e_single)->color;

        DBGE("\ntree " << tv << " - " << e_single);

        DBGE(gv.graphstring());

        id_type e_double1 =
            e_single == e0.edge_id() ? e1.edge_id() :
            e_single == e1.edge_id() ? e0.edge_id() :
            e_single == e2.edge_id() ? e0.edge_id() :
            BaseGraph::ID_INVALID;

        id_type e_double2 =
            e_single == e0.edge_id() ? e2.edge_id() :
            e_single == e1.edge_id() ? e2.edge_id() :
            e_single == e2.edge_id() ? e1.edge_id() :
            BaseGraph::ID_INVALID;

        int double_color = single_color == RED ? BLUE : RED;

        assert(gv.edge(e_double1)->color == double_color);
        assert(gv.edge(e_double2)->color == double_color);

        id_type v_double1 = gv.edge(e_double1).other_id(vdeg3id);
        id_type v_double2 = gv.edge(e_double2).other_id(vdeg3id);

        // calculate edge cut of split edge

        gv.edge(e_double1)->color = single_color;
        gv.edge(e_double2)->color = single_color;

        std::vector<int> edgecut_mark;
        std::vector<edge> edgecut = gv.calc_cut_edges(
            v_double1, v_double2, single_color, &edgecut_mark);

        DBGE("edgecut = " << edgecut);

        assert(edgecut_mark[v_double1] == 1);
        assert(edgecut_mark[v_double2] == 2);

        gv.edge(e_double1)->color = double_color;
        gv.edge(e_double2)->color = double_color;

        // find cycle of attached edge in other color -> cycle and non-cycle edge

        std::vector<edge> cycle =
            gv.find_color_path(vdeg3id, v_single, double_color);

        DBGE("cycle " << cycle);

        assert(cycle.size());
        assert(cycle[0].edge_id() == e_double1 || cycle[0].edge_id() == e_double2);

        id_type e_cycle = cycle[0].edge_id();
        id_type e_noncycle = e_cycle == e_double1 ? e_double2 : e_double1;

        id_type v_cycle = gv.edge(e_cycle).other_id(vdeg3id);
        id_type v_noncycle = gv.edge(e_noncycle).other_id(vdeg3id);

        // find edge cut of split edge

        std::vector<edge> attpath1 =
            gv.find_color_path(vdeg3id, v_noncycle, single_color);

        assert(attpath1[0] == gv.edge(e_single));
        attpath1.erase(attpath1.begin());

        DBGE("attpath1 = " << attpath1);

        std::sort(attpath1.begin(), attpath1.end());

        std::vector<edge> attpath2 =
            gv.find_color_path(vdeg3id, v_cycle, single_color);

        assert(attpath2[0] == gv.edge(e_single));
        attpath2.erase(attpath2.begin());

        DBGE("attpath2 = " << attpath2);

        std::sort(attpath2.begin(), attpath2.end());

        std::vector<edge> attpath;
        std::set_intersection(attpath1.begin(), attpath1.end(),
                              attpath2.begin(), attpath2.end(),
                              std::back_inserter(attpath));

        DBGE("attpath = " << attpath);

        std::vector<edge> inter;
        std::set_intersection(edgecut.begin(), edgecut.end(),
                              attpath.begin(), attpath.end(),
                              std::back_inserter(inter));

        DBGE("inter = " << inter);

        // cannot delete right away, because deletion changes order in vertex's edgelists.

        std::vector<id_type> to_delete;

        for (tvertex_edge vve : vv.edge_list())
        {
            tvertex vw = vve.other(vv);
            Tree& tw = vw->tree;

            DBGE(vve << " - " << *vve << " with trees "
                     << tv << " -> " << tw);

            EditBaseGraph gw = g0;
            tw.colorize_graph(gw);

            // DBGE(tv << " - " << tw);

            if (vve->UEout == ge_split)
            {
                EditBaseGraph gtest = g0;
                tv.colorize_graph(gtest);

                DBG(gtest.graphviz());

                gtest.edge(vve->UEin)->color = double_color;

                std::vector<edge> uecut =
                    gtest.calc_cut_edges(gtest.edge(vve->UEin));

                id_type uepair;

                if (vector_contains(uecut, gtest.edge(e_cycle))) {
                    assert(!vector_contains(uecut, gtest.edge(e_noncycle)));
                    uepair = e_cycle;
                }
                else {
                    assert(vector_contains(uecut, gtest.edge(e_noncycle)));
                    uepair = e_noncycle;
                }

                Tree ts = tv;

                ts.toggle(vve->UEin);
                ts.toggle(uepair);

                DBGE(" tw " << tw << " -> ts " << ts << " using " <<
                     (uepair == e_cycle ? "cycle" : "noncycle") << " edge");

                tvertex vs = tau.find_vertex(ts);
                assert(vs != tau.vertex_end());

                if (!tau.edge_exists(vv, vs, vve->colors)) {
                    tedge ea = tau.add_edge(vv, vs, TauEdge(vve->colors, vve->UEin, uepair));
                    no_delete.insert(ea.edge_id());
                }

                if (!tau.edge_exists(vs, vv, vve->colors)) {
                    tedge eb = tau.add_edge(vs, vv, TauEdge(vve->colors, uepair, vve->UEin));
                    no_delete.insert(eb.edge_id());
                }

                DBGE("add split edge UE: for (" << vve->UEin << "," << uepair << ") " << ts << " <-> " << tv);
                DBGE("add split edge UE: for (" << vve->UEin << "," << uepair << ") " << tv << " <-> " << ts);

                to_delete.push_back(vve.edge_id());
            }
            else if (vve->UEin == g1e.edge_id())
            {
                DBGE("??? DELETE");
                vve->dashed = true;

                to_delete.push_back(vve.edge_id());
            }
            else if (tv.contains(e_double1) != tw.contains(e_double1) ||
                     tv.contains(e_double2) != tw.contains(e_double2))
            {
                if (!no_delete.count(vve.edge_id()))
                {
                    to_delete.push_back(vve.edge_id());
                    DBGE("UNHANDLED CROSS EDGE???");
                    abort();
                }
            }
            else if (vector_contains(inter, gv.edge(vve->UEin)))
            {
                DBGE("BROKEN UE");
                to_delete.push_back(vve.edge_id());

#if 0
                // calculate distance of broken UE -> turns out this is not always <= 3!
                tvertex vv0 = tau0.find_vertex(tv);
                tvertex vw0 = tau0.find_vertex(tw);

                if (tau0.get_distance(vv0, vw0) >= 3)
                {
                    DBGE("distance = " << tau0.get_distance(vv0, vw0));

                    BaseGraph b1 = g0, b2 = g0;
                    vv0->tree.colorize_graph(b1);
                    vw0->tree.colorize_graph(b2);

                    DBGE("broken from " << graphstring(b1, GraphDecoratorS()) << " to " << graphstring(b2, GraphDecoratorS()));

                    DBG1(b1.graphviz());

                    TauGraph bfs = tau0.get_all_shortest_path_graph(vv0, vw0);
                    std::cout << graphviz(bfs, TauGraphDecorator()) << "\n";

                    TauGraph bfs2 = tau0.get_all_shortest_path_graph(vv0, vw0, TauColorMaskFilter(vve->colors == BLUE + 1 ? RED + 1 : BLUE + 1));
                    std::cout << graphviz(bfs2, TauGraphDecorator()) << "\n";
                }
#if 0
                std::vector<std::vector<tedge> > pathlist =
                    tau0.find_all_shortest_paths(vv0, vw0);

                bool any_good = false;

                for (std::vector<tedge>& path : pathlist)
                {
                    int brcolor = gv.edge(vve->UEout)->color;

                    for (const tedge& xchg : path)
                    {
                        edge eM = h0.edge(xchg->UEin), eB = h0.edge(xchg->UEout);

                        if (eM->color != brcolor && xchg != path.back()) {
                            good = false;
                            break;
                        }

                        std::swap(eM->color, eB->color);
                    }

                    if (!good) continue;
                    any_good = true;

                    if (path.size() <= 3) continue;

                    DBGE("distance: " << path.size());
                    DBGE("path: " << path);

                    // if (dist >= 4)
                    // {
                    //     BaseGraph b1 = g0, b2 = g0;
                    //     vv0->tree.colorize_graph(b1);
                    //     vw0->tree.colorize_graph(b2);
                    //     DBGE("broken from " << graphstring(b1, GraphDecoratorS()) << " to " << graphstring(b2, GraphDecoratorS()));

                    //     DBG1( graphviz(tau0, TauGraphDecorator()) );
                    //     abort();
                    // }
                }

                if (!any_good) {
                    DBGE("NOT GOOD! " + to_str(vve->UEin) + " -> " + to_str(vve->UEout) + " with deg3 " + to_str(vdeg3id));

                    BaseGraph h0 = g0;
                    vv0->tree.colorize_graph(h0);

                    DBGE(graphstring(h0, GraphDecoratorS()));

                    for (std::vector<tedge>& path : pathlist)
                    {
                        h0.m_title = "NotGood " + to_str(vve->UEin) + " -> " + to_str(vve->UEout)
                                     + " " + to_str(path);

                        DBG1(h0.graphviz());
                    }
                }
#endif

#endif
            }
        }

        for (id_type& ei : to_delete)
        {
            tau.delete_edge(ei);
        }

        // *** add swap UE caused by single edge

        if (1)
        {
            Tree ts = tv;

            ts.toggle(e_single);
            ts.toggle(e_cycle);

            tvertex vs = tau.find_vertex(ts);
            assert(vs != tau.vertex_end());

            DBGE("singleUE with trees " << tv << " -> " << ts);
            DBGE("singleUE back with trees " << ts << " -> " << tv);

            if (!tau.edge_exists(vv, vs, single_color + 1)) {
                tedge ea = tau.add_edge(vv, vs, TauEdge(single_color + 1, e_single, e_cycle));
                no_delete.insert(ea.edge_id());
            }

            if (!tau.edge_exists(vs, vv, single_color + 1)) {
                tedge eb = tau.add_edge(vs, vv, TauEdge(single_color + 1, e_cycle, e_single));
                no_delete.insert(eb.edge_id());
            }
        }

        if (1)
        {
            // find cycle in maker tree
            std::vector<edge> cycle =
                gv.find_color_path(vdeg3id, v_cycle, single_color);
            DBGE("cycycle = " << cycle);

            std::sort(cycle.begin(), cycle.end());

            gv.edge(e_cycle)->color = single_color;

            std::vector<edge> cut = gv.calc_cut_edges(gv.edge(e_cycle));
            DBGE("cycut = " << cut);

            std::vector<edge> inter;
            std::set_intersection(cycle.begin(), cycle.end(),
                                  cut.begin(), cut.end(),
                                  std::back_inserter(inter));

            DBGE("cyinter = " << inter);

            gv.edge(e_cycle)->color = double_color;

            if (inter.size() == 1)
            {
                assert(inter[0].edge_id() == e_single);

                Tree ts = tv;

                ts.toggle(e_single);
                ts.toggle(e_cycle);

                tvertex vs = tau.find_vertex(ts);
                assert(vs != tau.vertex_end());

                tedge te = tau.add_edge(vv, vs, TauEdge(double_color + 1, e_cycle, e_single));

                DBGE("Add extra cycle UE: " << *te << " trees " << tv << " -> " << ts);
                DBGE(tv.get_colorized_graph(g0).graphstring());
            }
        }
    }

    tau.m_title = "Removed edges";
    DBG0(graphviz(tau, TauGraphDecorator()));

#if 0
    // separate tau0 into

    // DBG( graphviz(tau0, TauGraphDecorator()) );

    TauGraph tau0x = tau0;

    for (tedge te : tau0.edge_list())
    {
        Tree v0 = te.tail()->tree;
        Tree v1 = te.head()->tree;

        if (v0.contains(e0.edge_id()) != v1.contains(e0.edge_id()) ||
            v0.contains(e1.edge_id()) != v1.contains(e1.edge_id()) ||
            v0.contains(e2.edge_id()) != v1.contains(e2.edge_id()))
        {
            tau0.delete_edge(te);
        }
        else
        {
            tau0x.delete_edge(te.edge_id());
        }
    }

    tau0.m_title = "tau0 restricted to non-ext";
    DBG(graphviz(tau0, TauGraphDecorator()));

    tau0x.m_title = "tau0 restricted to cross";
    DBG(graphviz(tau0x, TauGraphDecorator()));

    DBGE("Components:" <<
         " tau1s=" << tau1s.get_num_components() <<
         " tau2s=" << tau2s.get_num_components() <<
         " tau3s=" << tau3s.get_num_components() <<
         " tau0s=" << tau0.get_num_components() <<
         " tau0x=" << tau0x.get_num_components());

    if (tau1s.get_num_components() != 2) abort();
    if (tau2s.get_num_components() != 2) abort();
    if (tau3s.get_num_components() != 2) abort();

#endif

    TauGraph taudelta = tau0;
    taudelta.rebuild_treemap();

    assert(taudelta.num_vertex() == tau.num_vertex());

    // print out all vertices and edges of tau0 and taudelta.
    if (debug)
    {
        for (tvertex tv : tau.vertex_list()) {

            tvertex tdv = taudelta.find_vertex(tv->tree);
            assert(tdv != taudelta.vertex_end());

            DBGE("tv = " << tv << " " << tv->tree << " tdv = " << tdv << " " << tdv->tree);

            for (tvertex_edge tve : tv.edge_list()) {
                DBGE("tau1 " << tve << " " << tve.tail()->tree << " -> " << tve.head()->tree);
            }

            for (tvertex_edge tve : tdv.edge_list()) {
                DBGE("tau2 " << tve << " " << tve.tail()->tree << " -> " << tve.head()->tree);
            }
        }
    }

    for (tedge te : tau.edge_list())
    {
        tvertex tv = te.head(), tw = te.tail();

        tvertex tdv = taudelta.find_vertex(tv->tree);
        assert(tdv != taudelta.vertex_end());

        tvertex tdw = taudelta.find_vertex(tw->tree);
        assert(tdw != taudelta.vertex_end());

        tedge tde = taudelta.edge_end();

        for (size_t i = 0; (tde = taudelta.find_edge(tdv, tdw, i)) != taudelta.edge_end(); ++i)
        {
            if (tde->colors == te->colors) {
                taudelta.delete_edge(tde);
                break;
            }
        }

        if (tde == taudelta.edge_end()) {
            DBGE("Could not match edge " << te << ": " << *te << " - " <<
                 te.tail()->tree << " -> " << te.head()->tree);
            abort();
        }
    }

    taudelta.m_title = "delta";
    DBG(graphviz(taudelta, TauGraphDecorator()));

    DBGE("taudelta edges = " << taudelta.num_edge() << " " << write_graph6(_g0));
    //return;

    iGraph<TauGraph> itau0(tau0.clone_reorder_degree<TauGraph>());
    iGraph<TauGraph> itau(tau.clone_reorder_degree<TauGraph>());

    bool iso = itau0.isomorphic(itau);
    DBGE1("isomorphic vdeg3: " << iso);
    if (!iso) abort();

    // exit(0);
}

void test_decomposition(const BaseGraph& gxx)
{
    static const bool debug = false;

    BaseGraph g = gxx.clone_reorder_degree<BaseGraph>();
    g.m_title = write_graph6(g);

    // iterate over all edge cuts (just as in is_atomic_bispanner) -> finds
    // composite bispanning boundaries and calls test_decomposition_composite
    // for each of them.
    if (0)
    {
        std::vector<size_t> cutset(g.num_edge(), 0);
        bool is_composite = false;

        // intentionally skip the first [0,0,0,...,0] vector
        while (increment_boolean_vector(cutset))
        {
            EditBaseGraph g_edit = EditBaseGraph(g);

            size_t cutsize = 0;

            for (size_t i = 0; i < cutset.size(); ++i)
            {
                if (cutset[i]) {
                    g_edit.delete_edge(i);
                    ++cutsize;
                }
            }

            // all edges removed?
            if (cutsize == g.num_edge()) continue;

            size_t comp = g_edit.get_num_components_undirected();
            DBGE(cutset << " - " << comp);

            if (2 * (comp - 1) == cutsize)
            {
                DBGE("composite: " << cutset << " - " << comp);
                test_decomposition_composite(EditBaseGraph(g), cutset);
                is_composite = true;
            }
        }

        if (is_composite) return;
    }

    // iterate over all vertex cuts to find 2 vertex cuts and test 2-clique sum
    // composition/decomposition
    if (0)
    {
        std::vector<size_t> cutset(g.num_vertex(), 0);

        // intentionally skip the first [0,0,0,...,0] vector
        while (increment_boolean_vector(cutset))
        {
            EditBaseGraph g_edit = EditBaseGraph(g);

            size_t cutsize = 0;

            for (size_t i = 0; i < cutset.size(); ++i)
            {
                if (cutset[i]) {
                    g_edit.delete_vertex(i);
                    ++cutsize;
                }
            }

            // all edges removed?
            if (cutsize == g.num_vertex()) continue;

            size_t comp = g_edit.get_num_components_undirected();
            DBGE(cutset << " - " << comp);

            if (cutsize == 2 && comp > 1)
            {
                DBGE("2-sum: " << cutset << " - " << comp);

                test_decomposition_2sum(EditBaseGraph(g), cutset);
            }
        }
    }

    // iterate over degree three vertices and test deg3 decomposition

    for (auto v : gxx.vertex_list())
    {
        if (v.degree() != 3) continue;
        test_decomposition_3deg(gxx, v.vertex_id());
    }
}

void post_process_product_1sum()
{
    const BaseGraph& g1 = g_graphs[1];
    const BaseGraph& g2 = g_graphs[1];

    // BaseGraph gp = BaseGraph::product<BaseGraph>(g1, g1);
    // std::cout << gp.graphviz() << std::endl;

    std::cout << g1.graphviz() << std::endl;
    std::cout << g2.graphviz() << std::endl;

    BaseGraph g3 = g1;
    g3.add_copy(g2);

    std::cout << g3.graphviz() << std::endl;

    g3.contract(0, 8);

    std::cout << g3.graphviz() << std::endl;
    g3 = g3.clone_reorder_degree<BaseGraph>();

    TauGraph tau1 = AlgGame<BaseGraph>(g1).tau_;
    TauGraph tau2 = AlgGame<BaseGraph>(g2).tau_;

    std::cout << graphviz(tau1, TauGraphDecorator()) << "\n";
    std::cout << graphviz(tau2, TauGraphDecorator()) << "\n";

    TauProductDataFactory taufactory(g1.num_edge());
    TauGraph taup = TauGraph::product<TauGraph>(tau1, tau2, taufactory);

    std::cout << "// PRODUCT RESULT"
              << " num_vertex=" << taup.num_vertex()
              << " num_edge=" << taup.num_edge()
              << std::endl;

    // std::cout << graphviz(taup, TauGraphDecorator()) << "\n";

    TauGraph tau3 = AlgGame<BaseGraph>(g3).tau_;

    iGraph<TauGraph> itaup(taup);
    iGraph<TauGraph> itau3(tau3);

    std::cout << "isomorphic: " << itaup.isomorphic(itau3) << "\n";
}

#endif // !BISPANNING_DECOMPOSITION_HEADER

/******************************************************************************/
