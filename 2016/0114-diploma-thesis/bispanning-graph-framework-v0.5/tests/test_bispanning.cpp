/*******************************************************************************
 * tests/test_bispanning.cpp
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

#include "alg_bispanning.h"
#include "bispanning.h"

void run_test(size_t num_vertex)
{
    GengEnumerate geng(num_vertex, 2 * num_vertex - 2);

    size_t count = 0;
    size_t okay_bispanning = 0, okay_nashwilliams = 0, okay_tutte = 0;

    BaseGraph g;

    while (geng(g))
    {
        ++count;

        if (AlgBispanning<BaseGraph>(g).good())
            ++okay_bispanning;
        else
            continue;

        if (g.is_atomic_bispanner())
            ++okay_nashwilliams;

        if (g.is_atomic_bispanner_tutte())
            ++okay_tutte;
    }

    if (num_vertex == 4) {
        ASSERT(okay_bispanning == 1);
        ASSERT(okay_nashwilliams == 1);
        ASSERT(okay_tutte == okay_nashwilliams);
    }
    else if (num_vertex == 5) {
        ASSERT(okay_bispanning == 2);
        ASSERT(okay_nashwilliams == 1);
        ASSERT(okay_tutte == okay_nashwilliams);
    }
    else if (num_vertex == 6) {
        ASSERT(okay_bispanning == 12);
        ASSERT(okay_nashwilliams == 4);
        ASSERT(okay_tutte == okay_nashwilliams);
    }
    else if (num_vertex == 7) {
        ASSERT(okay_bispanning == 92);
        ASSERT(okay_nashwilliams == 15);
        ASSERT(okay_tutte == okay_nashwilliams);
    }
    else {
        DBG1("okay_bispanning = " << okay_bispanning <<
             " okay_nashwilliams = " << okay_nashwilliams <<
             " okay_tutte = " << okay_tutte);
    }
}

int main()
{
    run_test(4);
    run_test(5);
    run_test(6);
    run_test(7);

    return 0;
}

// forced instantiations
template class BaseGraphGeneric<false>;
template class BaseGraphGeneric<true>;

/******************************************************************************/
