/******************************************************************************
 * src/sequential/eberle-mergesort-lcp-losertree.h
 *
 * LCP aware multiway mergesort using a losertree implementation for merging.
 *
 ******************************************************************************
 * Copyright (C) 2013-2014 Andreas Eberle <email@andreas-eberle.com>
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

#ifndef EBERLE_MERGESORT_LCP_LOSERTREE_H_
#define EBERLE_MERGESORT_LCP_LOSERTREE_H_

#include <utility>

#include "../tools/eberle-utilities.h"
#include "../tools/eberle-lcp-losertree.h"
#include "../sequential/eberle-inssort-lcp.h"

//#define EBERLE_LCP_LOSERTREE_MERGESORT_CHECK_LCPS

namespace eberle_mergesort
{

using namespace eberle_lcp_utils;

typedef unsigned char* string;

template<unsigned K>
static inline void
eberle_mergesort_losertree_lcp_kway(string* strings, const LcpCacheStringPtr& tmp,
                                    const LcpCacheStringPtr& output, size_t length)
{
    if (length <= 2 * K)
    {
        return eberle_inssort_lcp::inssort_lcp(strings, output, length);
    }

    //create ranges of the parts
    std::pair<size_t, size_t> ranges[K];
    eberle_utils::calculateRanges(ranges, K, length);

    // execute mergesorts for parts
    for (unsigned i = 0; i < K; i++)
    {
        const size_t offset = ranges[i].first;
        const size_t size = ranges[i].second;
        eberle_mergesort_losertree_lcp_kway<K>(strings + offset,
                output.sub(offset, size), tmp.sub(offset, size), size);
    }

    //merge
    LcpStringLoserTree<K> loserTree(tmp, ranges);
    loserTree.writeElementsToStream(output);
}

template<unsigned K>
static inline void
eberle_mergesort_losertree_lcp_kway(string* strings, const LcpCacheStringPtr& outputPtr)
{
    size_t length = outputPtr.size;
    LcpCacheStringPtr tmpPtr(new string[length], new lcp_t[length], new char_type[length], length);

    eberle_mergesort_losertree_lcp_kway<K>(strings, tmpPtr, outputPtr, length);

    delete[] tmpPtr.strings;
    delete[] tmpPtr.lcps;
    delete[] tmpPtr.cachedChars;
}

// K must be a power of two
template<unsigned K>
static inline void
eberle_mergesort_losertree_kway(string *strings, size_t n)
{
    lcp_t* outputLcps = new lcp_t[n];
    string* tmpStrings = new string[n];
    lcp_t* tmpLcps = new lcp_t[n];

    char_type* outputCache = new char_type[n];
    char_type* tmpCache = new char_type[n];

    LcpCacheStringPtr output(strings, outputLcps, outputCache, n);
    LcpCacheStringPtr tmp(tmpStrings, tmpLcps, tmpCache, n);

    eberle_mergesort_losertree_lcp_kway<K>(strings, tmp, output, n);

#ifdef EBERLE_LCP_LOSERTREE_MERGESORT_CHECK_LCPS
    //check lcps
    stringtools::verify_lcp_cache(output.strings, output.lcps, output.cachedChars, output.size, 0);
#endif //EBERLE_LCP_LOSERTREE_MERGESORT_CHECK_LCPS

    delete[] outputLcps;
    delete[] tmpStrings;
    delete[] tmpLcps;

    delete[] outputCache;
    delete[] tmpCache;
}

void
eberle_mergesort_losertree_4way(string *strings, size_t n)
{
    eberle_mergesort_losertree_kway<4>(strings, n);
}

void
eberle_mergesort_losertree_16way(string *strings, size_t n)
{
    eberle_mergesort_losertree_kway<16>(strings, n);
}

void
eberle_mergesort_losertree_32way(string *strings, size_t n)
{
    eberle_mergesort_losertree_kway<32>(strings, n);
}

void
eberle_mergesort_losertree_64way(string *strings, size_t n)
{
    eberle_mergesort_losertree_kway<64>(strings, n);
}

CONTESTANT_REGISTER(eberle_mergesort_losertree_4way,
    "eberle/mergesort_lcp_losertree_4way",
    "Mergesort with lcp aware Losertree by Andreas Eberle")

CONTESTANT_REGISTER(eberle_mergesort_losertree_16way,
    "eberle/mergesort_lcp_losertree_16way",
    "Mergesort with lcp aware Losertree by Andreas Eberle")

CONTESTANT_REGISTER(eberle_mergesort_losertree_32way,
    "eberle/mergesort_lcp_losertree_32way",
    "Mergesort with lcp aware Losertree by Andreas Eberle")

CONTESTANT_REGISTER(eberle_mergesort_losertree_64way,
    "eberle/mergesort_lcp_losertree_64way",
    "Mergesort with lcp aware Losertree by Andreas Eberle")

}
 // namespace eberle_mergesort

#endif // EBERLE_MERGESORT_LCP_LOSERTREE_H_
