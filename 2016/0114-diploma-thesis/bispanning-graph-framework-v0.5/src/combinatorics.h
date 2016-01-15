/*******************************************************************************
 * src/combinatorics.h
 *
 * Some basic combinatorial enumeration utilities.
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

#ifndef BISPANNING_COMBINATORICS_HEADER
#define BISPANNING_COMBINATORICS_HEADER

#include <algorithm>
#include <string>
#include <vector>

//! increment a left-most least-significant vector [0,1,0,1,...,1,0] of boolean
//! values, false if the vector wrapped to zero.
template <typename ValueType>
bool increment_boolean_vector(std::vector<ValueType>& v)
{
    for (unsigned int i = 0; i < v.size(); ++i)
    {
        if (v[i] == 0) {
            v[i] = 1;
            return true;
        }
        else {
            v[i] = 0;
        }
    }
    return false;
}

//! Call a functional object enumerating all k-subsets of {0,...,n-1} providing
//! them as a std::vector<size_t>. The enumeration is ended if the functional
//! returns false.
template <typename Functor>
bool choose(unsigned int n, unsigned int k, Functor functor)
{
    std::string bitmask(k, 1); // k leading 1's
    bitmask.resize(n, 0);      // n-k trailing 0's

    std::vector<size_t> items(k);

    // permute bitmask and output items
    do {
        for (unsigned int i = 0, j = 0; i < n; ++i) { // items [0..N-1]
            if (bitmask[i]) items[j++] = i;
        }

        if (!functor(items)) return false;
    }
    while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    return true;
}

//! Call a functional object for all set partitions of {0,...,n-1}, providing
//! identification via a std::vector<size_t>. The enumeration is ended if the
//! functional returns false.
template <typename Functor>
bool enumerate_set_partitions(size_t n, Functor functor)
{
    // from http://compprog.wordpress.com/2007/10/15/generating-the-partitions-of-a-set/
    // massively fixed and changed to 0..n-1.

    /* s[i] is the number of the set in which the ith element should go */
    std::vector<size_t> s(n, 0);
    /* m[i] is the largest of the first i elements in s */
    std::vector<size_t> m(n, 0);

    /* 0 0 0 0 is the first way to partition a set is to put all the elements
       in the same subset. */

    /* Output the first partitioning. */
    if (!functor(s))
        return false;

    /* Print the other partitioning schemes. */
    while (1)
    {
        /* Update s: 0 0 0 0 -> 1 0 0 0 -> 0 1 0 0 ->
           1 1 0 0 -> 2 1 0 0 -> 0 0 1 0 ... */

        size_t i = 0;
        ++s[i];
        while ((i < n - 1) && (s[i] > m[i + 1] + 1)) {
            s[i] = 0;
            ++i;
            ++s[i];
        }

        /* If i is has reached n-1 th element, then the last unique partitiong
           has been found */
        if (i == n - 1)
            break;

        /* Because all the first i elements are now 1, s[i] (i + 1 th element)
           is the largest. So we update max by copying it to all the first i
           positions in m. */
        if (s[i] > m[i])
            m[i] = s[i];

        size_t j = i;
        while (j > 0)
            m[--j] = m[i];

        if (!functor(s))
            return false;
    }

    return true;
}

#endif // !BISPANNING_COMBINATORICS_HEADER

/******************************************************************************/
