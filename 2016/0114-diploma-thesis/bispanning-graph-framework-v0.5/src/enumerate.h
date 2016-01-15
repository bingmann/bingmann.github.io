/*******************************************************************************
 * src/enumerate.h
 *
 * Enumerate and play game on all bispanning graphs.
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

#ifndef BISPANNING_ENUMERATE_HEADER
#define BISPANNING_ENUMERATE_HEADER

#include "alg_bispanning.h"
#include "bispanning.h"
#include "thread_pool.hpp"

#include <boost/regex.hpp>
#include <string>

//! load or create a specific graph, or parse graph as graph6/sparse6 string.
BaseGraph load_graph(const std::string& graph)
{
    if (graph == "k4")
    {
        BaseGraph g = BaseGraph(4);

        g.add_edge(0, 1);
        g.add_edge(0, 2);
        g.add_edge(0, 3);
        g.add_edge(1, 2);
        g.add_edge(1, 3);
        g.add_edge(2, 3);

        return g;
    }
    else if (graph == "g4a")
    {
        BaseGraph g = BaseGraph(4);

        g.add_edge(0, 1);
        g.add_edge(0, 1);
        g.add_edge(0, 2);
        g.add_edge(0, 3);
        g.add_edge(1, 2);
        g.add_edge(2, 3);

        return g;
    }
    else if (graph == "w5")
    {
        BaseGraph g = BaseGraph(5);

        g.add_edge(0, 1);
        g.add_edge(1, 2);
        g.add_edge(0, 2);
        g.add_edge(2, 3);
        g.add_edge(0, 3);
        g.add_edge(3, 4);
        g.add_edge(0, 4);
        g.add_edge(4, 1);

        return g;
    }
    else if (graph == "w6")
    {
        BaseGraph g = BaseGraph(6);

        g.add_edge(0, 1);
        g.add_edge(1, 2);
        g.add_edge(0, 2);
        g.add_edge(2, 3);
        g.add_edge(0, 3);
        g.add_edge(3, 4);
        g.add_edge(0, 4);
        g.add_edge(4, 5);
        g.add_edge(0, 5);
        g.add_edge(5, 1);

        return g;
    }
    else if (graph == "k4,k4")
    {
        BaseGraph g = BaseGraph(6);

        g.add_edge(0, 1);
        g.add_edge(0, 3);
        g.add_edge(0, 2);
        g.add_edge(1, 3);
        g.add_edge(1, 2);
        g.add_edge(2, 3);

        g.add_edge(1, 4);
        g.add_edge(1, 5);
        g.add_edge(1, 3);
        g.add_edge(4, 5);
        g.add_edge(4, 3);
        g.add_edge(3, 5);

        g.delete_edge(g.edge(3));
        g.delete_edge(g.edge(8));

        return g;
    }
    else if (graph == "k3,4")
    {
        BaseGraph g = BaseGraph(7);

        g.add_edge(0, 3);
        g.add_edge(0, 4);
        g.add_edge(0, 5);
        g.add_edge(0, 6);
        g.add_edge(1, 3);
        g.add_edge(1, 4);
        g.add_edge(1, 5);
        g.add_edge(1, 6);
        g.add_edge(2, 3);
        g.add_edge(2, 4);
        g.add_edge(2, 5);
        g.add_edge(2, 6);

        return g;
    }
    else {
        return read_graph6<BaseGraph>(graph);
    }
}

class BispanningFilter
{
public:
    bool add_keyword(const std::string& word)
    {
        if (word == "parallel") run_parallel_ = true;
        else if (word == "general") enumerate_general_ = true;
        else if (word == "atomic") only_atomic_ = true;
        else if (word == "composite") only_composite_ = true;
        else if (word == "triangle-free") only_triangle_free_ = true;
        else if (word == "square-free") only_square_free_ = true;
        else if (word == "vconn=1") only_vconn_ = 1;
        else if (word == "vconn=2") only_vconn_ = 2;
        else if (word == "vconn=3") only_vconn_ = 3;
        else if (word == "econn=2") only_econn_ = 2;
        else if (word == "econn=3") only_econn_ = 3;
        else if (word == "maxdeg=4") max_degree_ = 4;
        else return false;

        return true;
    }

    template <typename Functor>
    void pre_process(BaseGraph& g, Functor&& process, size_t* process_count)
    {
        if (enumerate_general_) {
            // very cheap test: atomic bispanners have no degree two vertices.
            if (only_atomic_ && g.get_min_degree() != 3) return;

            if (max_degree_ && g.get_max_degree() > max_degree_) return;
        }

        // check if graph is bispanning: this is cheaper than atomic tests
        AlgBispanning<BaseGraph> ab(g);
        if (!ab.good()) return;
        ab.write(g);

        // filter by edge or vertex connectivity (igraph does this well)
        if (only_vconn_ || only_econn_) {
            iGraph<BaseGraph> ig(g);

            DBGG0("vconn = " << ig.get_vertex_connectivity() << " - " <<
                  "econn = " << ig.get_edge_connectivity());

            if (only_vconn_ && ig.get_vertex_connectivity() != only_vconn_) return;
            if (only_econn_ && ig.get_edge_connectivity() != only_econn_) return;
        }

        // determine atomic or composite property
        if (only_atomic_ || only_composite_) {
            bool atomic = g.is_atomic_bispanner();
            if (only_atomic_ && !atomic) return;
            if (only_composite_ && atomic) return;
        }

        if (process_count)
            ++(*process_count);

        if (!run_parallel_) {
            std::forward<Functor>(process)(g);
        }
        else {
            std::function<void(BaseGraph&)> p = process;
            thread_pool_.Enqueue([=]() mutable { p(g); });
        }
    }

    template <typename Functor>
    void enumerate(size_t num_vertex, Functor&& process)
    {
        if (enumerate_general_ && !only_atomic_)
            return enumerate_general(num_vertex, std::forward<Functor>(process));

        std::vector<std::string> options;

        // very cheap test: atomic bispanners have no degree two vertices.
        if (only_atomic_) options.push_back("-d3");

        if (max_degree_) options.push_back("-D" + std::to_string(max_degree_));

        if (only_triangle_free_) options.push_back("-t");
        if (only_square_free_) options.push_back("-f");

        GengEnumerate geng(num_vertex, 2 * num_vertex - 2, join(" ", options));

        BaseGraph g;
        size_t count = 0;
        size_t offset = offset_;
        size_t process_count = 0;

        while (geng(g) && count < limit_)
        {
            if (offset > 0) {
                --offset;
                continue;
            }

            ++count;

            pre_process(g, std::forward<Functor>(process), &process_count);
        }

        std::cerr << "// processed " << process_count << " bispanners."
                  << std::endl;
    }

    //! enumerate general bispanners with parallel edges by enumerating
    //! bipartite graphs with genbg and taking these as the edge adjacency.
    template <typename Functor>
    void enumerate_general(size_t num_vertex, Functor&& process)
    {
        size_t num_edge = 2 * num_vertex - 2;

        GenbgEnumerate genbg(num_vertex, num_edge);

        using id_type = BaseGraph::id_type;
        using vertex = BaseGraph::vertex_iterator;
        using vertex_edge = BaseGraph::vertex_edge_iterator;

        BaseGraph bipartite;
        unsigned int count = 0;
        size_t offset = offset_;
        size_t process_count = 0;

        while (genbg(bipartite) && count < limit_)
        {
            if (offset > 0) {
                --offset;
                continue;
            }

            ++count;

            // std::cout << bipartite.graphviz() << std::endl;

            // transform bipartite graph into bispanning graph.
            BaseGraph g(num_vertex);

            for (size_t i = 0; i < num_edge; ++i)
            {
                vertex v = bipartite.vertex(i);
                assert(v.degree() == 2);

                vertex_edge ve0 = bipartite.vertex_edge(i, 0);
                vertex_edge ve1 = bipartite.vertex_edge(i, 1);

                id_type v0 = ve0.other(v).vertex_id();
                id_type v1 = ve1.other(v).vertex_id();

                g.add_edge(v0 - num_edge, v1 - num_edge);
            }

            pre_process(g, std::forward<Functor>(process), &process_count);
        }

        std::cerr << "// processed " << process_count << " bispanners."
                  << std::endl;
    }

private:
    //! first in enumerate
    size_t offset_ = 0;

    //! limit of graphs in enumeration
    size_t limit_ = std::numeric_limits<size_t>::max();

    //! run using parallel thread pool
    bool run_parallel_ = false;

    //! enumerate bispanners with parallel edges as well
    bool enumerate_general_ = false;

    //! only allow atomic bispanners
    bool only_atomic_ = false;

    //! only allow composite bispanners
    bool only_composite_ = false;

    //! only triangle-free bispanners
    bool only_triangle_free_ = false;

    //! only square-free bispanners
    bool only_square_free_ = false;

    //! filter by vertex-connectivity
    unsigned only_vconn_ = 0;

    //! filter by edge-connectivity
    unsigned only_econn_ = 0;

    //! maximum degree
    unsigned max_degree_ = 0;

    //! thread pool
    ThreadPool thread_pool_;
};

/*!
 * Class which takes a specification "da(0,0),es(1,0)" which describes a
 * double-attach and edge-split-attach sequence.
 */
template <typename Functor>
class GenerateSpecific
{
public:
    void pre_process(const BaseGraph& g)
    {
        if (!isocheck(g)) {
            std::cout << "// dup" << std::endl;
            return;
        }

        ++count;

        BaseGraph g_copy = g;
        filter_.pre_process(g_copy, std::forward<Functor>(process_), &process_count);
    }

    void recurse_specific(const BaseGraph& g, const std::string& desc)
    {
        if (desc.size() == 0)
            return;

        boost::regex re1("(da|es)\\(([0-9]+|\\?),([0-9]+|\\?)\\),?(.*)");
        boost::smatch m;

        if (!boost::regex_match(desc, m, re1)) {
            DBGG1("Invalid specific da/es sequence: " << desc);
            abort();
        }

        if (m[1] == "da")
        {
            int v1b, v1e, v2b, v2e;

            if (m[2] == "?") {
                v1b = 0, v1e = g.total_vertex() - 1;
            }
            else {
                v1b = v1e = atoi(m[2].str().c_str());
            }

            if (m[3] == "?") {
                v2b = 0, v2e = g.total_vertex() - 1;
            }
            else {
                v2b = v2e = atoi(m[3].str().c_str());
            }

            for (int v1 = v1b; v1 <= v1e; ++v1)
            {
                if (g.vertex_deleted(v1)) continue;

                for (int v2 = v2b; v2 <= v2e; ++v2)
                {
                    if (g.vertex_deleted(v2)) continue;

                    BaseGraph g2 = g;

                    g2.add_vertex();
                    genbig_double_attach(g2, g2.total_vertex() - 1, v1, v2);

                    pre_process(g2);

                    recurse_specific(g2, m[4]);
                }
            }
        }
        else if (m[1] == "es")
        {
            int e1b, e1e, v2b, v2e;

            if (m[2] == "?") {
                e1b = 0, e1e = g.total_edge() - 1;
            }
            else {
                e1b = e1e = atoi(m[2].str().c_str());
            }

            if (m[3] == "?") {
                v2b = 0, v2e = g.total_vertex() - 1;
            }
            else {
                v2b = v2e = atoi(m[3].str().c_str());
            }

            for (int e1 = e1b; e1 <= e1e; ++e1)
            {
                if (g.edge_deleted(e1)) continue;

                for (int v2 = v2b; v2 <= v2e; ++v2)
                {
                    if (g.vertex_deleted(v2)) continue;

                    BaseGraph g2 = g;

                    g2.add_vertex();
                    genbig_split_edge(g2, g2.total_vertex() - 1,
                                      g2.edge(e1), g2.vertex(v2));

                    pre_process(g2);

                    recurse_specific(g2, m[4]);
                }
            }
        }
        else {
            abort();
        }
    }

    GenerateSpecific(BispanningFilter& filter, Functor&& process)
        : filter_(filter), process_(std::forward<Functor>(process))
    {
        filter_.add_keyword("general");
    }

    void Run(const std::string& desc)
    {
        BaseGraph g(1);
        recurse_specific(g, desc);

        DBGG1("// processed " << process_count << " bispanners "
                              << "from " << count << " non-isomorphic graphs.");
    }

private:
    BaseGraph::IsomorphismCheck isocheck;

    size_t count = 0;
    size_t process_count = 0;

    // filter which is applied to generated filters
    BispanningFilter& filter_;

    // processing function
    Functor && process_;
};

//! enumerate all bispanning selected by filter
template <typename Functor>
void enumerate_bispanning_graphs(
    const std::vector<std::string>& wordlist, Functor&& process)
{
    BispanningFilter filter;

    for (const std::string& word : wordlist)
    {
        if (filter.add_keyword(word)) {
            continue;
        }
        else if (str_is_numeric(word)) {
            unsigned num = from_str<unsigned>(word);
            filter.enumerate(num, std::forward<Functor>(process));
        }
        else if (word.size() > 2 &&
                 (word.substr(0, 2) == "es" || word.substr(0, 2) == "da")) {
            GenerateSpecific<Functor>(
                filter, std::forward<Functor>(process)).Run(word);
        }
        else {
            // try as graph name or graph6/sparse6
            BaseGraph g = load_graph(word);
            filter.pre_process(g, std::forward<Functor>(process), nullptr);
        }
    }
}

//! enumerate all bispanning selected by filter
template <typename Functor>
void enumerate_bispanning_graphs(
    const std::string& filter, Functor&& process)
{
    return enumerate_bispanning_graphs(
        split(filter, ' '), std::forward<Functor>(process));
}

#endif // !BISPANNING_ENUMERATE_HEADER

/******************************************************************************/
