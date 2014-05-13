/******************************************************************************
 * src/sequential/bingmann-lcp_inssort.h
 *
 * LCP-aware insertion sort, used in parallel variants as base case.
 *
 ******************************************************************************
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <string.h>

#include "../tools/stringtools.h"
#include "../tools/contest.h"

#ifndef BINGMANN_LCP_INSSORT_H_
#define BINGMANN_LCP_INSSORT_H_

namespace bingmann_lcp_inssort {

using namespace stringtools;

//! LCP insertion sort
static inline
void lcp_insertion_sort(string* str, uintptr_t* lcp, size_t n, size_t depth)
{
    if (n <= 1) return;

    for (size_t j = 0; j < n - 1; ++j)
    {
        // insert strings[j] into sorted strings[0..j-1]

        string new_str = str[j];
        size_t new_lcp = depth; // start with LCP depth

        size_t i = j;
        while (i > 0)
        {
            size_t prev_lcp = new_lcp;

            string cur_str = str[i - 1];
            size_t cur_lcp = lcp[i];

            if (cur_lcp < new_lcp)
            {
                // CASE 1: lcp goes down -> insert string
                break;
            }
            else if (cur_lcp == new_lcp)
            {
                // CASE 2: compare more characters

                string s1 = new_str + new_lcp;
                string s2 = cur_str + new_lcp;

                while (*s1 != 0 && *s1 == *s2)
                    ++s1, ++s2, ++new_lcp;

                // if (new_str >= curr_str) -> insert string
                if (*s1 >= *s2)
                {
                    // update lcp of prev (smaller string) with inserted string
                    lcp[i] = new_lcp;
                    // lcp of inserted string with next string
                    new_lcp = prev_lcp;
                    break;
                }
            }
            // else (cur_lcp > new_lcp), CASE 3: nothing to do

            str[i] = cur_str;
            lcp[i + 1] = cur_lcp;

            --i;
        }

        str[i] = new_str;
        lcp[i + 1] = new_lcp;
    }

    // last loop specialized with checks for out-of-bound access to lcp.
    {
        size_t j = n - 1;

        // insert strings[j] into sorted strings[0..j-1]

        string new_str = str[j];
        size_t new_lcp = depth; // start with LCP depth

        size_t i = j;
        while (i > 0)
        {
            size_t prev_lcp = new_lcp;

            string cur_str = str[i - 1];
            size_t cur_lcp = lcp[i];

            if (cur_lcp < new_lcp)
            {
                // CASE 1: lcp goes down -> insert string
                break;
            }
            else if (cur_lcp == new_lcp)
            {
                // CASE 2: compare more characters

                string s1 = new_str + new_lcp;
                string s2 = cur_str + new_lcp;

                while (*s1 != 0 && *s1 == *s2)
                    ++s1, ++s2, ++new_lcp;

                // if (new_str >= curr_str) -> insert string
                if (*s1 >= *s2)
                {
                    // update lcp of prev (smaller string) with inserted string
                    lcp[i] = new_lcp;
                    // lcp of inserted string with next string
                    new_lcp = prev_lcp;
                    break;
                }
            }
            // else (cur_lcp > new_lcp), CASE 3: nothing to do

            str[i] = cur_str;

            if (i + 1 < n) // check out-of-bounds copy
                lcp[i + 1] = cur_lcp;

            --i;
        }

        str[i] = new_str;

        if (i + 1 < n) // check out-of-bounds save
            lcp[i + 1] = new_lcp;
    }
}

//! LCP insertion sort, close to journal's pseudo-code
static inline
void lcp_insertion_sort_pseudocode(string* str, uintptr_t* lcp, size_t n, size_t depth)
{
    unsigned int cmp = 0;

    for (size_t j = 0; j < n; ++j)
    {
        // insert strings[j] into sorted strings[0..j-1]

        string snew = str[j];
        size_t h = depth; // start with LCP depth

        size_t i = j;
        while (i > 0)
        {
            if (lcp[i] < h)
            {
                // CASE 1: lcp goes down -> insert string
                break;
            }
            else if (lcp[i] == h)
            {
                // CASE 2: compare more characters

                size_t prev_lcp = h;

                string s2 = str[i - 1];

                while (cmp++, snew[h] != 0 && snew[h] == s2[h])
                    ++h;

                // if (new_str >= curr_str) -> insert string
                if (snew[h] >= s2[h])
                {
                    // update lcp of prev (smaller string) with inserted string
                    lcp[i] = h;
                    // lcp of inserted string with next string
                    h = prev_lcp;
                    break;
                }
            }
            // else (cur_lcp > new_lcp), CASE 3: nothing to do

            str[i] = str[i - 1];
            lcp[i + 1] = lcp[i];

            --i;
        }

        str[i] = snew;
        lcp[i + 1] = h;
    }

    std::cout << "lcp_inssort comparisons = " << cmp << "\n";
}

// *** Externally Called Functions ***

static inline
void lcp_insertion_sort(string* str, size_t n, size_t depth)
{
    uintptr_t tmp_lcp[n];
    return lcp_insertion_sort(str, tmp_lcp, n, depth);
}

static inline
void lcp_insertion_sort(StringShadowPtr& str, size_t depth)
{
    return lcp_insertion_sort(str.active(), str.size(), depth);
}

static inline
void lcp_insertion_sort(StringShadowLcpPtr& str, size_t depth)
{
    return lcp_insertion_sort(str.active(), str.lcparray(), str.size(), depth);
}

// *** Registered Test Functions ***

static inline
void test_lcp_insertion_sort(string* strings, size_t n)
{
    lcp_insertion_sort(strings, n, 0);
}

CONTESTANT_REGISTER(test_lcp_insertion_sort,
    "bingmann/lcp_insertion_sort",
    "LCP-aware insertion sort")

static inline
void test_lcp_insertion_sort_nolcp(string* strings, size_t n)
{
    string* shadow = new string[n]; // allocate shadow pointer array
    StringShadowPtr strptr(strings, shadow, n);

    lcp_insertion_sort(strptr, 0);

    delete [] shadow;
}

CONTESTANT_REGISTER(test_lcp_insertion_sort_nolcp,
    "bingmann/lcp_insertion_sort_nolcp",
    "LCP-aware insertion sort (without LCP output)")

static inline
void test_lcp_insertion_sort_pseudocode(string* strings, size_t n)
{
    string* shadow = new string[n+1]; // allocate shadow pointer array
    StringShadowLcpPtr strptr(strings, shadow, n);

    strptr.lcp(0) = 42; // must keep lcp[0] unchanged

    lcp_insertion_sort_pseudocode(strptr.active(), strptr.lcparray(), n, 0);

    stringtools::verify_lcp(strptr.active(), strptr.lcparray(), n, 42);

    delete [] shadow;
}

CONTESTANT_REGISTER(test_lcp_insertion_sort_pseudocode,
    "bingmann/lcp_insertion_sort_pseudocode",
    "LCP-aware insertion sort close to pseudo-code, with checking")

////////////////////////////////////////////////////////////////////////////////

//! LCP insertion sort
static inline
void lcp_insertion_sort_cache(string* str, uintptr_t* lcp, char_type* cache,
                              size_t n, size_t depth)
{
    if (n <= 1) return;

    for (size_t j = 0; j < n - 1; ++j)
    {
        // insert strings[j] into sorted strings[0..j-1]

        string new_str = str[j];
        char_type new_ch = new_str[depth];
        size_t new_lcp = depth; // start with LCP depth

        size_t i = j;
        while (i > 0)
        {
            size_t prev_lcp = new_lcp;

            string cur_str = str[i - 1];
            size_t cur_lcp = lcp[i];

            if (cur_lcp < new_lcp)
            {
                // CASE 1: lcp goes down -> insert string
                break;
            }
            else if (cur_lcp == new_lcp)
            {
                // CASE 2: compare more characters

                string s2 = cur_str + new_lcp;

                while (new_ch != 0 && new_ch == *s2)
                {
                    ++s2, ++new_lcp;
                    new_ch = new_str[new_lcp];
                }

                // if (new_str >= curr_str) -> insert string
                if (new_ch >= *s2)
                {
                    // update lcp of prev (smaller string) with inserted string
                    lcp[i] = new_lcp;
                    cache[i] = new_ch;

                    // lcp of inserted string with next string
                    new_lcp = prev_lcp;
                    break;
                }
            }
            // else (cur_lcp > new_lcp), CASE 3: nothing to do

            str[i] = cur_str;
            cache[i] = cache[i - 1];

            lcp[i + 1] = cur_lcp;

            --i;
        }

        str[i] = new_str;

        lcp[i + 1] = new_lcp;
        cache[i + 1] = str[i + 1][new_lcp];
    }

    // last loop specialized with checks for out-of-bound access to lcp.
    {
        size_t j = n - 1;

        // insert strings[j] into sorted strings[0..j-1]

        string new_str = str[j];
        char_type new_ch = new_str[depth];
        size_t new_lcp = depth; // start with LCP depth

        size_t i = j;
        while (i > 0)
        {
            size_t prev_lcp = new_lcp;

            string cur_str = str[i - 1];
            size_t cur_lcp = lcp[i];

            if (cur_lcp < new_lcp)
            {
                // CASE 1: lcp goes down -> insert string
                break;
            }
            else if (cur_lcp == new_lcp)
            {
                // CASE 2: compare more characters

                string s2 = cur_str + new_lcp;

                while (new_ch != 0 && new_ch == *s2)
                {
                    ++s2, ++new_lcp;
                    new_ch = new_str[new_lcp];
                }

                // if (new_str >= curr_str) -> insert string
                if (new_ch >= *s2)
                {
                    // update lcp of prev (smaller string) with inserted string
                    lcp[i] = new_lcp;
                    cache[i] = new_ch;

                    // lcp of inserted string with next string
                    new_lcp = prev_lcp;
                    break;
                }
            }
            // else (cur_lcp > new_lcp), CASE 3: nothing to do

            str[i] = cur_str;
            cache[i] = cache[i - 1];

            if (i + 1 < n) // check out-of-bounds copy
                lcp[i + 1] = cur_lcp;

            --i;
        }

        str[i] = new_str;

        if (i + 1 < n) // check out-of-bounds save
        {
            lcp[i + 1] = new_lcp;
            cache[i + 1] = str[i + 1][new_lcp];
        }
    }
}

static inline
void test_lcp_insertion_sort_cache(string* strings, size_t n)
{
    lcp_t* lcps = new lcp_t[n]; // allocate LCP array
    char_type* cache = new char_type[n]; // allocate distinguishing char cache

    lcp_insertion_sort_cache(strings, lcps, cache, n, 0);

    stringtools::verify_lcp_cache(strings, lcps, cache, n, -1);

    delete [] lcps;
    delete [] cache;
}

CONTESTANT_REGISTER(test_lcp_insertion_sort_cache,
    "bingmann/lcp_insertion_sort_cache",
    "LCP-aware insertion sort (with distinguishing character cache)")

} // namespace bingmann_lcp_inssort

#endif // BINGMANN_LCP_INSSORT_H_
