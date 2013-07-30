/*
 * Copyright 2007-2008 by Tommi Rantala <tt.rantala@gmail.com>
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

/*
 * msd_A() is an implementation of the MSD radix sort that maintains a manual
 * cache of a few bytes from each string to be sorted. This cache is updated
 * along the execution of the algorithm.
 *
 * The idea can be found in the CRadix algorithm by Ng and Kakehi.
 *    http://dx.doi.org/10.1093/ietfec/e90-a.2.457
 */

namespace rantala_msd_a {

#define CACHED_BYTES 4

typedef struct {
	unsigned char bytes[CACHED_BYTES];
	unsigned char* ptr;
} cacheblock_t;

static inline void
inssort_cache(cacheblock_t* cache, int n, size_t depth)
{
	cacheblock_t *pi, *pj;
	unsigned char *s, *t;
	for (pi = cache + 1; --n > 0; ++pi) {
		unsigned char* tmp = pi->ptr;
		for (pj = pi; pj > cache; --pj) {
			t = tmp + depth;
			for (s=(pj-1)->ptr+depth; *s==*t && *s!=0; ++s, ++t)
				;
			if (*s <= *t)
				break;
			pj->ptr = (pj-1)->ptr;
		}
		pj->ptr = tmp;
	}
}

static void
fill_cache(cacheblock_t* cache, size_t N, size_t depth)
{
	for (size_t i=0; i < N; ++i) {
		unsigned int j=0;
		while (j < CACHED_BYTES && cache[i].ptr[depth+j]) {
			cache[i].bytes[j] = cache[i].ptr[depth+j];
			++j;
		}
		while (j < CACHED_BYTES) {
			cache[i].bytes[j] = 0;
			++j;
		}
	}
}

static void
msd_A(cacheblock_t* cache, size_t N, size_t cache_depth, size_t true_depth)
{
	if (N < 32) {
		inssort_cache(cache, N, true_depth);
		return;
	}
	if (cache_depth >= CACHED_BYTES) {
		fill_cache(cache, N, true_depth);
		cache_depth = 0;
	}
	size_t bucketsize[256] = {0};
	for (size_t i=0; i < N; ++i)
		++bucketsize[cache[i].bytes[cache_depth]];
	cacheblock_t* sorted = (cacheblock_t*)
		malloc(N*sizeof(cacheblock_t));
	static size_t bucketindex[256];
	bucketindex[0] = 0;
	for (unsigned i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1] + bucketsize[i-1];
	for (size_t i=0; i < N; ++i)
		memcpy(&sorted[bucketindex[cache[i].bytes[cache_depth]]++],
				cache+i, sizeof(cacheblock_t));
	memcpy(cache, sorted, N*sizeof(cacheblock_t));
	free(sorted);
	size_t bsum = bucketsize[0];
	for (unsigned i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_A(cache+bsum, bucketsize[i],
			  cache_depth+1, true_depth+1);
		bsum += bucketsize[i];
	}
}

static void
msd_A_adaptive(cacheblock_t* cache,
               size_t N,
               size_t cache_depth,
               size_t true_depth)
{
	if (N < 0x10000) {
		msd_A(cache, N, cache_depth, true_depth);
		return;
	}
	if (cache_depth >= CACHED_BYTES) {
		fill_cache(cache, N, true_depth);
		cache_depth = 0;
	}
	size_t* bucketsize = (size_t*) calloc(0x10000, sizeof(size_t));
	for (size_t i=0; i < N; ++i) {
		uint16_t bucket =
			(cache[i].bytes[cache_depth] << 8) |
			cache[i].bytes[cache_depth+1];
		++bucketsize[bucket];
	}
	cacheblock_t* sorted = (cacheblock_t*)
		malloc(N*sizeof(cacheblock_t));
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (unsigned i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1] + bucketsize[i-1];
	for (size_t i=0; i < N; ++i) {
		uint16_t bucket = (cache[i].bytes[cache_depth] << 8)
			| cache[i].bytes[cache_depth+1];
		memcpy(&sorted[bucketindex[bucket]++],
				cache+i, sizeof(cacheblock_t));
	}
	memcpy(cache, sorted, N*sizeof(cacheblock_t));
	free(sorted);
	size_t bsum = bucketsize[0];
	for (unsigned i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_A_adaptive(cache+bsum, bucketsize[i],
				cache_depth+2, true_depth+2);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}

void
msd_A(unsigned char** strings, size_t N)
{
	cacheblock_t* cache = (cacheblock_t*) malloc(N*sizeof(cacheblock_t));
	for (size_t i=0; i < N; ++i) cache[i].ptr = strings[i];
	fill_cache(cache, N, 0);
	msd_A(cache, N, 0, 0);
	for (size_t i=0; i < N; ++i) strings[i] = cache[i].ptr;
	free(cache);
}
CONTESTANT_REGISTER(msd_A,
                    "rantala/msd_A",
                    "msd_A")

void
msd_A_adaptive(unsigned char** strings, size_t N)
{
	cacheblock_t* cache = (cacheblock_t*) malloc(N*sizeof(cacheblock_t));
	for (size_t i=0; i < N; ++i) cache[i].ptr = strings[i];
	fill_cache(cache, N, 0);
	msd_A_adaptive(cache, N, 0, 0);
	for (size_t i=0; i < N; ++i) strings[i] = cache[i].ptr;
	free(cache);
}
CONTESTANT_REGISTER(msd_A_adaptive,
                    "rantala/msd_A_adaptive",
                    "msd_A_adaptive")

} // namespace rantala_msd_a
