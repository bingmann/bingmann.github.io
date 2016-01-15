/*******************************************************************************
 * src/play_bispanning.cpp
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

#include "graph.h"
#include "graph6.h"
#include "graphml.h"
//#include "graph_boost.h"
#include "alg_bispanning.h"
#include "alg_game.h"
#include "alg_play_decompose.h"
#include "alg_play_trivial.h"
#include "bispanning.h"

#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

bool g_print_graphs = false;
bool g_print_games = false;
bool g_write_tgf = false;
bool g_write_graphml = false;
bool g_write_json = false;

bool g_save_graphs = false;

std::vector<BaseGraph> g_graphs;

#include "decomposition.h"
#include "enumerate.h"
#include "play_cliquesum.h"

void process(BaseGraph& g)
{
    if (0)
    {
        for (size_t cbo = 0; cbo < 80; ++cbo)
        {
            tree_pair_type trees = calc_cyclic_base_ordering(g, cbo);
            if (!trees.valid) break;

            bool testUE = test_cyclic_base_ordering<true>(g, trees, true);

            std::cout << "// cbo[" << 0 << "] tree0: " << trees.t0 << " - tree1: " << trees.t1
                      << " - " << (testUE ? "UEs" : "") << std::endl;

            if (!testUE) abort();
        }
    }

    if (1)
    {
        //if (ig.get_girth() < 4) return;

        //if (!bispanner_test_nontrivial_3edge_cuts(g)) return;

        //if (ig.contains_subgraph(iGraph<BaseGraph>::complete(4))) return;
        //if (ig.contains_subgraph(iGraph<BaseGraph>::wheel(5))) return;
    }

    if (g_write_tgf)
    {
        std::string fname = "g-" + write_graph6(g) + ".tgf";
        std::ofstream(fname) << graphtgf(g, BaseGraph::GraphDecoratorA()) << "\n";
    }

    if (g_write_graphml)
    {
        std::string fname = "g-" + write_graph6(g) + ".graphml";
        std::ofstream(fname) << yedgraphml(g, BaseGraph::GraphDecoratorA()) << "\n";
    }

    if (g_write_json)
    {
        std::string fname = "g-" + write_graph6(g) + ".json";
        std::ofstream of(fname);
        g.output_json(of);
    }

    if (g_print_graphs)
    {
        std::cout << "// " << g.graphstring() << std::endl;
        std::cout << g.graphviz() << std::endl;
        //std::cout << graphviz(calc_kernel(g), GraphDecoratorA()) << std::endl;

        iGraph<BaseGraph> ig(g);

        std::cout << "// GRAPH"
                  << " graph=" << write_graph6(g)
                  << " num_vertex=" << g.num_vertex()
                  << " num_edge=" << g.num_edge()
                  << " degrees=" << g.get_degree_sequence()
                  << " girth=" << ig.get_girth()
                  << " vertex_conn=" << ig.get_vertex_connectivity()
                  << " edge_conn=" << ig.get_edge_connectivity()
                  << std::endl;

        //std::cerr << get_cycle_cut_listing(g, "\n") << std::endl;
    }

    if (g_save_graphs)
        g_graphs.push_back(g);

    AlgGame<BaseGraph> game(g);
    game.print(g_print_games, g_write_tgf);

    // Enable this to run the composition tests
    //test_decomposition(g);
}

void post_process()
{
    //post_process_3sums_colors();
}

int main(int argc, char* argv[])
{
    // boost program options
    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Available options");

    desc.add_options()
        ("help,h", "produce help message")
        ("print-base-graphs,g", po::bool_switch(&g_print_graphs),
        "output base graphs on stdout using the dot graph format")
        ("print-game-graphs,G", po::bool_switch(&g_print_games),
        "output game graphs on stdout using the dot graph format")
        ("write-tgf,T", po::bool_switch(&g_write_tgf),
        "write base graph as TGF trivial graph format")
        ("write-graphml,Y", po::bool_switch(&g_write_graphml),
        "write base and game graphs as yEd's GraphML format")
        ("write-json,J", po::bool_switch(&g_write_json),
        "write base and game graphs as a JSON format")
    ;

    po::parsed_options parsed = po::parse_command_line(argc, argv, desc);

    po::variables_map vm;
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::vector<std::string> args =
        po::collect_unrecognized(parsed.options, po::include_positional);

    enumerate_bispanning_graphs(args, process);

    post_process();

    return 0;
}

/******************************************************************************/
