/*
 * Copyright 2008 by Tommi Rantala <tt.rantala@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/* Implements a multi-way mergesort based on the loser tree.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

#include <boost/array.hpp>

#include "tools/debug.h"
#include "tools/get_char.h"
#include "tools/insertion_sort.h"
#include "tools/losertree.h"
#include "../tools/contest.h"

namespace rantala {

void mergesort_4way(unsigned char**, size_t, unsigned char**);

template <unsigned K>
static void
mergesort_losertree(unsigned char** strings, size_t n, unsigned char** tmp)
{
	if (n < 0x10000) {
		mergesort_4way(strings, n, tmp);
		return;
	}
	debug() << __func__ << "(), n="<<n<<"\n";
	const size_t split = size_t(double(n) / double(K));
	boost::array<std::pair<unsigned char**, size_t>, K> ranges;
	for (unsigned i=0; i < K-1; ++i) {
		ranges[i] = std::make_pair(strings+i*split, split);
	}
	ranges[K-1] = std::make_pair(strings+(K-1)*split, n-(K-1)*split);
	for (unsigned i=0; i < K; ++i) {
		mergesort_losertree<K>(ranges[i].first, ranges[i].second,
				tmp+(ranges[i].first-strings));
	}
	unsigned char** result = tmp;
	loser_tree<unsigned char*> tree(ranges.begin(), ranges.end());
	while (tree._nonempty_streams) { *result++ = tree.min(); }
	(void) memcpy(strings, tmp, n*sizeof(unsigned char*));
}

void mergesort_losertree_64way(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree<64>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_128way(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree<128>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_256way(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree<256>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_512way(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree<512>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_1024way(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree<1024>(strings, n, tmp);
	free(tmp);
}

CONTESTANT_REGISTER(mergesort_losertree_64way,
                    "rantala/mergesort_losertree_64way",
                    "mergesort 64way loser tree based")
CONTESTANT_REGISTER(mergesort_losertree_128way,
                    "rantala/mergesort_losertree_128way",
                    "mergesort 128way loser tree based")
CONTESTANT_REGISTER(mergesort_losertree_256way,
                    "rantala/mergesort_losertree_256way",
                    "mergesort 256way loser tree based")
CONTESTANT_REGISTER(mergesort_losertree_512way,
                    "rantala/mergesort_losertree_512way",
                    "mergesort 512way loser tree based")
CONTESTANT_REGISTER(mergesort_losertree_1024way,
                    "rantala/mergesort_losertree_1024way",
                    "mergesort 1024way loser tree based")

void mergesort_4way_parallel(unsigned char**, size_t, unsigned char**);

template <unsigned K>
static void
mergesort_losertree_parallel(unsigned char** strings, size_t n, unsigned char** tmp)
{
	if (n < 0x10000) {
		mergesort_4way_parallel(strings, n, tmp);
		return;
	}
	debug() << __func__ << "(), n="<<n<<"\n";
	const size_t split = size_t(double(n) / double(K));
	boost::array<std::pair<unsigned char**, size_t>, K> ranges;
	for (unsigned i=0; i < K-1; ++i) {
		ranges[i] = std::make_pair(strings+i*split, split);
	}
	ranges[K-1] = std::make_pair(strings+(K-1)*split, n-(K-1)*split);
#pragma omp parallel for
	for (unsigned i=0; i < K; ++i) {
		mergesort_losertree_parallel<K>(ranges[i].first, ranges[i].second,
				tmp+(ranges[i].first-strings));
	}
	unsigned char** result = tmp;
	loser_tree<unsigned char*> tree(ranges.begin(), ranges.end());
	while (tree._nonempty_streams) { *result++ = tree.min(); }
	(void) memcpy(strings, tmp, n*sizeof(unsigned char*));
}

void mergesort_losertree_64way_parallel(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree_parallel<64>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_128way_parallel(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree_parallel<128>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_256way_parallel(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree_parallel<256>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_512way_parallel(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree_parallel<512>(strings, n, tmp);
	free(tmp);
}
void mergesort_losertree_1024way_parallel(unsigned char** strings, size_t n)
{
	unsigned char** tmp = static_cast<unsigned char**>(
			malloc(n*sizeof(unsigned char*)));
	mergesort_losertree_parallel<1024>(strings, n, tmp);
	free(tmp);
}

CONTESTANT_REGISTER_PARALLEL(mergesort_losertree_64way_parallel,
                             "rantala/mergesort_losertree_64way_parallel",
                             "mergesort parallel 64way loser tree based")
CONTESTANT_REGISTER_PARALLEL(mergesort_losertree_128way_parallel,
                             "rantala/mergesort_losertree_128way_parallel",
                             "mergesort parallel 128way loser tree based")
CONTESTANT_REGISTER_PARALLEL(mergesort_losertree_256way_parallel,
                             "rantala/mergesort_losertree_256way_parallel",
                             "mergesort parallel 256way loser tree based")
CONTESTANT_REGISTER_PARALLEL(mergesort_losertree_512way_parallel,
                             "rantala/mergesort_losertree_512way_parallel",
                             "mergesort parallel 512way loser tree based")
CONTESTANT_REGISTER_PARALLEL(mergesort_losertree_1024way_parallel,
                             "rantala/mergesort_losertree_1024way_parallel",
                             "mergesort parallel 1024way loser tree based")

} // namespace rantala
