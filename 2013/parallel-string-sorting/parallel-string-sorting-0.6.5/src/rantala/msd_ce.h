/*
 * Copyright 2007-2008,2011 by Tommi Rantala <tt.rantala@gmail.com>
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
 * These "CE" variants use the double sweep counting sort to implement MSD
 * radix sort. The variants also use an external array of size O(n) when
 * distributing the strings into correct order.
 *
 * msd_CE0: Bare bones implementation, useful for comparison purposes.
 *
 * msd_CE1: Otherwise the same as CE0, except:
 *   - uses O(n) oracle to reduce TLB and other cache misses
 *
 * msd_CE2: Otherwise the same as CE0, except:
 *   - uses O(n) oracle to reduce TLB and other cache misses
 *   - avoids memory stalls by building oracle first, then calculating the size
 *     of each bucket
 *
 * msd_CE3: Otherwise the same as CE0, except:
 *   - uses O(n) oracle to reduce TLB and other cache misses
 *   - avoids memory stalls by building oracle first, then calculating the size
 *     of each bucket
 *   - uses superalphabet when the size of the subinput is large, normal
 *     alphabet otherwise
 *
 * msd_CE4: Otherwise the same as CE0, except:
 *   - uses O(n) oracle to reduce TLB and other cache misses
 *   - avoids memory stalls by building oracle first, then calculating the size
 *     of each bucket
 *   - uses superalphabet when the size of the subinput is large, normal
 *     alphabet otherwise
 *   - uses 16-bit bucket counter type in msd_CE2 that is used for inputs
 *     smaller than 0x10000
 *
 * msd_CE5:
 *   - Allocates the `oracle' and `sorted' arrays just one time before sorting
 *     to reduce the amount of malloc() and free() calls.
 *
 * msd_CE6:
 *   - Manually unrolls the loop that populates the `oracle' array.
 *
 * msd_CE7:
 *   - Check for sorted (sub)inputs to avoid the distribution step when not
 *     required.
 */

namespace rantala {

void
msd_CE0(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 32) {
		insertion_sort(strings, n, depth);
		return;
	}
	size_t bucketsize[256] = {0};
	for (size_t i=0; i < n; ++i)
		++bucketsize[strings[i][depth]];
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	static size_t bucketindex[256];
	bucketindex[0] = 0;
	for (size_t i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[strings[i][depth]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_CE0(strings+bsum, bucketsize[i], depth+1);
		bsum += bucketsize[i];
	}
}

void msd_CE0(unsigned char** strings, size_t n)
{ msd_CE0(strings, n, 0); }

CONTESTANT_REGISTER(msd_CE0,
                    "rantala/msd_CE0",
                    "msd_CE0: baseline")

void
msd_CE1(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 32) {
		insertion_sort(strings, n, depth);
		return;
	}
	size_t bucketsize[256] = {0};
	unsigned char* restrict oracle =
		(unsigned char*) malloc(n);
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i] = strings[i][depth]];
	unsigned char** restrict sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	size_t bucketindex[256];
	bucketindex[0] = 0;
	for (size_t i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	free(oracle);
	size_t bsum = bucketsize[0];
	for (unsigned short i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_CE1(strings+bsum, bucketsize[i], depth+1);
		bsum += bucketsize[i];
	}
}

void msd_CE1(unsigned char** strings, size_t n)
{ msd_CE1(strings, n, 0); }

CONTESTANT_REGISTER(msd_CE1,
                    "rantala/msd_CE1",
                    "msd_CE1: oracle")

void
msd_CE2(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 32) {
		insertion_sort(strings, n, depth);
		return;
	}
	size_t bucketsize[256] = {0};
	unsigned char* restrict oracle =
		(unsigned char*) malloc(n);
	for (size_t i=0; i < n; ++i)
		oracle[i] = strings[i][depth];
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	unsigned char** restrict sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	size_t bucketindex[256];
	bucketindex[0] = 0;
	for (size_t i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	free(oracle);
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_CE2(strings+bsum, bucketsize[i], depth+1);
		bsum += bucketsize[i];
	}
}

void msd_CE2(unsigned char** strings, size_t n)
{ msd_CE2(strings, n, 0); }

CONTESTANT_REGISTER(msd_CE2,
                    "rantala/msd_CE2",
                    "msd_CE2: oracle+loop fission")

void
msd_CE2_16bit(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 32) {
		insertion_sort(strings, n, depth);
		return;
	}
	uint16_t bucketsize[256] = {0};
	unsigned char* restrict oracle =
		(unsigned char*) malloc(n);
	for (size_t i=0; i < n; ++i)
		oracle[i] = strings[i][depth];
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	unsigned char** restrict sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	uint16_t bucketindex[256];
	bucketindex[0] = 0;
	for (size_t i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	free(oracle);
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_CE2_16bit(strings+bsum, bucketsize[i], depth+1);
		bsum += bucketsize[i];
	}
}

void
msd_CE3(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 0x10000) {
		msd_CE2(strings, n, depth);
		return;
	}
	uint16_t* restrict oracle =
		(uint16_t*) malloc(n*sizeof(uint16_t));
	for (size_t i=0; i < n; ++i)
		oracle[i] = get_char<uint16_t>(strings[i], depth);
	size_t* restrict bucketsize = (size_t*)
		calloc(0x10000, sizeof(size_t));
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (size_t i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	free(oracle);
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_CE3(strings+bsum,
				bucketsize[i], depth+2);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}

void msd_CE3(unsigned char** strings, size_t n)
{ msd_CE3(strings, n, 0); }

CONTESTANT_REGISTER(msd_CE3,
                    "rantala/msd_CE3",
                    "msd_CE3: oracle+loop fission+adaptive")

static void
msd_CE4(unsigned char** strings, size_t n, size_t depth)
{
	if (n < 0x10000) {
		msd_CE2_16bit(strings, n, depth);
		return;
	}
	uint16_t* restrict oracle =
		(uint16_t*) malloc(n*sizeof(uint16_t));
	for (size_t i=0; i < n; ++i)
		oracle[i] = get_char<uint16_t>(strings[i], depth);
	size_t* restrict bucketsize = (size_t*)
		calloc(0x10000, sizeof(size_t));
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (size_t i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	free(sorted);
	free(oracle);
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_CE4(strings+bsum,
				bucketsize[i], depth+2);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}

void msd_CE4(unsigned char** strings, size_t n)
{ msd_CE4(strings, n, 0); }

CONTESTANT_REGISTER(msd_CE4,
                    "rantala/msd_CE4",
                    "msd_CE4: oracle+loop fission+adaptive+16bit counter")

static void
msd_CE2_16bit_5(unsigned char** strings, size_t n, size_t depth,
		unsigned char* oracle, unsigned char** sorted)
{
	if (n < 32) {
		insertion_sort(strings, n, depth);
		return;
	}
	uint16_t bucketsize[256] = {0};
	for (size_t i=0; i < n; ++i)
		oracle[i] = strings[i][depth];
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	uint16_t bucketindex[256];
	bucketindex[0] = 0;
	for (size_t i=1; i < 256; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 256; ++i) {
		if (bucketsize[i] == 0) continue;
		msd_CE2_16bit_5(strings+bsum, bucketsize[i], depth+1,
				oracle, sorted);
		bsum += bucketsize[i];
	}
}

static void
msd_CE5(unsigned char** strings, size_t n, size_t depth,
		uint16_t* oracle, unsigned char** sorted)
{
	if (n < 0x10000) {
		msd_CE2_16bit_5(strings, n, depth,
			(unsigned char*)oracle, sorted);
		return;
	}
	for (size_t i=0; i < n; ++i)
		oracle[i] = get_char<uint16_t>(strings[i], depth);
	size_t* restrict bucketsize = (size_t*)
		calloc(0x10000, sizeof(size_t));
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (size_t i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_CE5(strings+bsum, bucketsize[i],
				depth+2, oracle, sorted);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}

void msd_CE5(unsigned char** strings, size_t n)
{
	uint16_t* restrict oracle = (uint16_t*)
		malloc(n*sizeof(uint16_t));
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	msd_CE5(strings, n, 0, oracle, sorted);
	free(oracle);
	free(sorted);
}

CONTESTANT_REGISTER(msd_CE5,
                    "rantala/msd_CE5",
                    "msd_CE5: oracle+loop fission+adaptive+16bit counter+prealloc")

static void
msd_CE6(unsigned char** strings, size_t n, size_t depth,
		uint16_t* oracle, unsigned char** sorted)
{
	if (n < 0x10000) {
		msd_CE2_16bit_5(strings, n, depth, (unsigned char*)oracle, sorted);
		return;
	}
	{
		size_t i;
		for (i=0; i < n-n%2; i+=2) {
			unsigned char* str1 = strings[i];
			unsigned char* str2 = strings[i+1];
			uint16_t ch1 = get_char<uint16_t>(str1, depth);
			uint16_t ch2 = get_char<uint16_t>(str2, depth);
			oracle[i] = ch1;
			oracle[i+1] = ch2;
		}
		for (; i < n; ++i)
			oracle[i] = get_char<uint16_t>(strings[i], depth);
	}
	size_t* restrict bucketsize = (size_t*)
		calloc(0x10000, sizeof(size_t));
	for (size_t i=0; i < n; ++i)
		++bucketsize[oracle[i]];
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (size_t i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_CE6(strings+bsum, bucketsize[i],
				depth+2, oracle, sorted);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}
void msd_CE6(unsigned char** strings, size_t n)
{
	uint16_t* restrict oracle = (uint16_t*)
		malloc(n*sizeof(uint16_t));
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	msd_CE6(strings, n, 0, oracle, sorted);
	free(oracle);
	free(sorted);
}

CONTESTANT_REGISTER(msd_CE6,
                    "rantala/msd_CE6",
                    "msd_CE6: oracle+loop fission+adaptive+16bit counter+prealloc+unroll")

static void
msd_CE7_(unsigned char** strings, size_t n, size_t depth,
		uint16_t* oracle, unsigned char** sorted)
{
	if (n < 0x10000) {
		msd_CE2_16bit_5(strings, n, depth, (unsigned char*)oracle, sorted);
		return;
	}
	{
		size_t i;
		for (i=0; i < n-n%2; i+=2) {
			unsigned char* str1 = strings[i];
			unsigned char* str2 = strings[i+1];
			uint16_t ch1 = get_char<uint16_t>(str1, depth);
			uint16_t ch2 = get_char<uint16_t>(str2, depth);
			oracle[i  ] = ch1;
			oracle[i+1] = ch2;
		}
		for (; i < n; ++i)
			oracle[i] = get_char<uint16_t>(strings[i], depth);
	}
	size_t* restrict bucketsize = (size_t*)
		calloc(0x10000, sizeof(size_t));
	int is_sorted = 1;
	{
		size_t i;
		uint16_t prev_ch = oracle[0];
		++bucketsize[prev_ch];
		for (i=1; i < n; ++i) {
			uint16_t ch = oracle[i];
			++bucketsize[ch];
			if (ch > prev_ch) {
				is_sorted = 0;
				++i;
				break;
			}
			prev_ch = ch;
		}
		for (; i < n; ++i)
			++bucketsize[oracle[i]];
	}
	if (is_sorted)
		goto in_order;
	static size_t bucketindex[0x10000];
	bucketindex[0] = 0;
	for (size_t i=1; i < 0x10000; ++i)
		bucketindex[i] = bucketindex[i-1]+bucketsize[i-1];
	for (size_t i=0; i < n; ++i)
		sorted[bucketindex[oracle[i]]++] = strings[i];
	memcpy(strings, sorted, n*sizeof(unsigned char*));
in_order:
	size_t bsum = bucketsize[0];
	for (size_t i=1; i < 0x10000; ++i) {
		if (bucketsize[i] == 0) continue;
		if (i & 0xFF) msd_CE7_(strings+bsum, bucketsize[i],
				depth+2, oracle, sorted);
		bsum += bucketsize[i];
	}
	free(bucketsize);
}
void msd_CE7(unsigned char** strings, size_t n)
{
	uint16_t* restrict oracle = (uint16_t*)
		malloc(n*sizeof(uint16_t));
	unsigned char** sorted = (unsigned char**)
		malloc(n*sizeof(unsigned char*));
	msd_CE7_(strings, n, 0, oracle, sorted);
	free(oracle);
	free(sorted);
}

CONTESTANT_REGISTER(msd_CE7,
                    "rantala/msd_CE7",
                    "msd_CE7: oracle+loop fission+adaptive+16bit counter+prealloc+unroll+sortedness")

} // namespace rantala
