/*******************************************************************************
 * tests/test_geng.cpp
 *
 * Test graph enumeration with geng.
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

#include "graph6.h"

#include <iostream>
#include <sstream>

struct Empty { };

using SimpleGraph = Graph<Empty, Empty>;

int main()
{
    GengEnumerate geng(8, 14);

    SimpleGraph g;

    unsigned int count = 0;
    std::ostringstream oss;

    while (geng(g))
    {
        oss << g << std::endl;
        ++count;
    }

    ASSERT(count == 1579);
    std::cout << oss.str().size() << std::endl;
    ASSERT(oss.str().size() == 303168);

    return 0;
}

/******************************************************************************/
