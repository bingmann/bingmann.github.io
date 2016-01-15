/*******************************************************************************
 * tests/test_random.cpp
 *
 * Test random graph generation and some simple algorithms.
 *
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
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

#include <iostream>
#include <sstream>

struct Empty { };

// undirect graph with parallel edges and loops
using SimpleGraph = Graph<Empty, Empty>;

int main()
{
    unsigned int seed = time(nullptr);

    for (unsigned int gi = 0; gi < 100; ++gi)
    {
        unsigned int
            vertexnum = rand_r(&seed) % 10 + 5,
            edgenum = rand_r(&seed) % 30 + 10;

        SimpleGraph g;
        if (!generate_random_graph(g, vertexnum, edgenum, &seed))
        {
            std::cerr << "// impossible combination: " << vertexnum << " - " << edgenum << std::endl;
            continue;
        }

        ASSERT(g.num_vertex() == vertexnum);
        ASSERT(g.num_edge() == edgenum);

        std::cout << g << std::endl;

        std::cout << "// regularity: " << g.get_regularity() << std::endl;
        std::cout << "// node degree: min " << g.get_min_degree() << ", max " << g.get_max_degree() << std::endl;
        std::cout << "// degree sequence: " << g.get_degree_sequence() << std::endl;
        std::cout << "// components: " << g.get_num_components() << std::endl;
        std::cout << "// acyclic: " << g.is_acyclic() << std::endl;
    }

    return 0;
}

// forced instantiations
template class GraphBase<Empty, Empty, false, true, false, false>;
template class GraphBase<Empty, Empty, false, true, false, true>;

template class GraphBase<Empty, Empty, true, true, false, false>;
template class GraphBase<Empty, Empty, true, true, false, true>;

/******************************************************************************/
