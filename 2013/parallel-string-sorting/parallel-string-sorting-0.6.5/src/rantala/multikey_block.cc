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

/*
 * Implements the Multi-Key-Quicksort using the input array as temporary space.
 * See also msd_DB.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <list>

#include <boost/array.hpp>

#include "tools/debug.h"
#include "tools/get_char.h"
#include "tools/median.h"
#include "../tools/contest.h"
#include "../sequential/bs-mkqs.h"

namespace rantala_multikey_block {

using namespace rantala;

template <typename CharT>
static inline unsigned
get_bucket(CharT c, CharT pivot)
{
        return ((c > pivot) << 1) | (c == pivot);
}

typedef unsigned char** Block;
//typedef std::deque<Block> FreeBlocks;
typedef std::list<Block> FreeBlocks;
typedef std::list<Block> Bucket;
typedef boost::array<Bucket, 3> Buckets;
typedef std::vector<Block*> BackLinks;

static inline Block
take_free_block(FreeBlocks& fb)
{
	assert(not fb.empty());
	Block b(fb.front());
	fb.pop_front();
	return b;
}

template <unsigned B, typename CharT>
static void
multikey_block(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 10000) {
            bs_mkqs::ssort2(strings, n, depth);
            return;
	}
	assert(n > B);
	static Buckets buckets;
	static boost::array<unsigned char*, 32*B> temp_space;
	static FreeBlocks freeblocks;
	const CharT partval = pseudo_median<CharT>(strings, n, depth);
	BackLinks backlinks(n/B+1);
	boost::array<size_t, 3> bucketsize;
	bucketsize.assign(0);
	buckets[0].clear();
	buckets[1].clear();
	buckets[2].clear();
	// Initialize our list of free blocks.
	assert(freeblocks.empty());
	for (size_t i=0; i < 32; ++i)
		freeblocks.push_back(&temp_space[i*B]);
	for (size_t i=0; i < n-n%B; i+=B)
		freeblocks.push_back(strings+i);
	// Distribute strings to buckets. Use a small cache to reduce memory
	// stalls. The exact size of the cache is not very important.
	size_t i=0;
	for (; i < n-n%32; i+=32) {
		boost::array<CharT, 32> cache;
		for (unsigned j=0; j<32; ++j) {
			cache[j] = get_char<CharT>(strings[i+j], depth);
		}
		for (unsigned j=0; j<32; ++j) {
			const CharT c = cache[j];
			const unsigned b = get_bucket(c, partval);
			if (bucketsize[b] % B == 0) {
				Block block = take_free_block(freeblocks);
				buckets[b].push_back(block);
				// Backlinks must be set for those blocks, that
				// use the original string array space.
				if (block >= strings && block < strings+n) {
					backlinks[(block-strings)/B] =
						&(buckets[b].back());
				}
			}
			assert(not buckets[b].empty());
			buckets[b].back()[bucketsize[b] % B] = strings[i+j];
			++bucketsize[b];
		}
	}
	for (; i < n; ++i) {
		const CharT c = get_char<CharT>(strings[i], depth);
		const unsigned b = get_bucket(c, partval);
		if (bucketsize[b] % B == 0) {
			Block block = take_free_block(freeblocks);
			buckets[b].push_back(block);
			// Backlinks must be set for those blocks, that
			// use the original string array space.
			if (block >= strings && block < strings+n) {
				backlinks[(block-strings)/B] =
					&(buckets[b].back());
			}
		}
		assert(not buckets[b].empty());
		buckets[b].back()[bucketsize[b] % B] = strings[i];
		++bucketsize[b];
	}
	assert(bucketsize[0]+bucketsize[1]+bucketsize[2]==n);
	// Process each bucket, and copy all strings in that bucket to proper
	// place in the original string pointer array. This means that those
	// positions that are occupied by other blocks must be moved to free
	// space etc.
	size_t pos = 0;
	for (unsigned i=0; i < 3; ++i) {
		if (bucketsize[i] == 0) continue;
		Bucket::const_iterator it = buckets[i].begin();
		for (size_t bucket_pos=0; bucket_pos < bucketsize[i]; ++it, bucket_pos+=B) {
			const size_t block_items = std::min(size_t(B), bucketsize[i]-bucket_pos);
			const size_t block_overlap = (pos+block_items-1)/B;
			if (*it == (strings+pos)) {
				// Already at correct place.
				assert(pos%B==0);
				backlinks[pos/B] = 0;
				pos += block_items;
				continue;
			}
			// Don't overwrite the block in the position we are
			// about to write to, but copy it into safety.
			if (backlinks[block_overlap]) {
				// Take a free block. The block can be 'stale',
				// i.e. it can point to positions we have
				// already copied new strings into. Take free
				// blocks until we have non-stale block.
				Block tmp = take_free_block(freeblocks);
				while (tmp >= strings && tmp < strings+pos)
					tmp = take_free_block(freeblocks);
				if (tmp >= strings && tmp < strings+n) {
					assert(backlinks[(tmp-strings)/B]==0);
					backlinks[(tmp-strings)/B] = backlinks[block_overlap];
				}
				memcpy(tmp, *(backlinks[block_overlap]), B*sizeof(unsigned char*));
				*(backlinks[block_overlap]) = tmp;
				backlinks[block_overlap] = 0;
			}
			if (*it >= strings && *it < strings+n) {
				assert(*it > strings+pos);
				backlinks[(*it-strings)/B] = 0;
			}
			// Copy string pointers to correct position.
			memcpy(strings+pos, *it, block_items*sizeof(unsigned char*));
			// Return block for later use. Favor those in the
			// temporary space.
			if (*it >= strings && *it < strings+n) {
				freeblocks.push_back(*it);
			} else {
				freeblocks.push_front(*it);
			}
			pos += block_items;
		}
	}
	freeblocks.clear();
	backlinks.clear(); BackLinks().swap(backlinks);
	multikey_block<B, CharT>(strings, bucketsize[0], depth);
	if (not is_end(partval))
		multikey_block<B, CharT>(strings+bucketsize[0], bucketsize[1],
				depth+sizeof(CharT));
	multikey_block<B, CharT>(strings+bucketsize[0]+bucketsize[1],
			bucketsize[2], depth);
}

void multikey_block1(unsigned char** strings, size_t n)
{
	multikey_block<1024, unsigned char>(strings, n, 0);
}
void multikey_block2(unsigned char** strings, size_t n)
{
	multikey_block<1024, uint16_t>(strings, n, 0);
}
void multikey_block4(unsigned char** strings, size_t n)
{
	multikey_block<1024, uint32_t>(strings, n, 0);
}

CONTESTANT_REGISTER(multikey_block1,
                    "rantala/multikey_block1",
                    "multikey_block with 1byte alphabet")
CONTESTANT_REGISTER(multikey_block2,
                    "rantala/multikey_block2",
                    "multikey_block with 2byte alphabet")
CONTESTANT_REGISTER(multikey_block4,
                    "rantala/multikey_block4",
                    "multikey_block with 4byte alphabet")

} // namespace rantala_multikey_block
