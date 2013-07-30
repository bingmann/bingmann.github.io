/******************************************************************************
 * src/sequential/bingmann-sample_sortBTC.h
 *
 * Experiments with sequential Super Scalar String Sample-Sort (S^5).
 *
 * Binary tree search with bucket cache.
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

namespace bingmann_sample_sortBTC {

using namespace bingmann_sample_sort;

#if 0
static const size_t numsplitters = 31;
#else
//static const size_t l2cache = 256*1024;

// bounding equations:
// splitters            + bktsize
// n * sizeof(key_type) + (2*n+1) * sizeof(size_t) <= l2cache

//static const size_t numsplitters2 = ( l2cache - sizeof(size_t) ) / (sizeof(key_type) + 2 * sizeof(size_t));
//static const size_t numsplitters2 = l2cache / sizeof(key_type);
static const size_t numsplitters2 = ( l2cache - sizeof(size_t) ) / (2 * sizeof(size_t));

//static const size_t numsplitters2 = ( l2cache - sizeof(size_t) ) / ( sizeof(key_type) );

static const size_t treebits = logfloor_<numsplitters2>::value;
static const size_t numsplitters = (1 << treebits) - 1;
#endif

// ****************************************************************************
// *** Classification Subroutines: rolled, assembler and unrolled variants

struct ClassifySimple
{
    /// search in splitter tree for bucket number
    static inline unsigned int
    find_bkt_tree(const key_type& key, const key_type* splitter, const key_type* splitter_tree0)
    {
#if 1
        // binary tree traversal without left branch

        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i = 1;

        while ( i < numsplitters+1 )
        {
            if (key <= splitter_tree[i])
                i = 2*i + 0;
            else // (key > splitter_tree[i])
                i = 2*i + 1;
        }

        i -= numsplitters+1;

        size_t b = i * 2;                                   // < bucket
        if (i < numsplitters && splitter[i] == key) b += 1; // equal bucket

#else
        // binary search variant with keeping the last left branch
        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i = 1;
        unsigned int ll = 1;        // last left branch taken

        while ( i <= numsplitters )
        {
            if (key <= splitter_tree[i]) {
                ll = i;
                i = 2*i + 0;
            }
            else // (key > splitter_tree[i])
                i = 2*i + 1;
        }

        i -= numsplitters+1;

#if 0
        // Verify result of binary search:
        int pos = numsplitters-1;
        while ( pos >= 0 && key <= splitter[pos] ) --pos;
        pos++;

        std::cout << "i " << i << " pos " << pos << "\n";
        assert(i == pos);
#endif

        assert(i >= numsplitters || splitter_tree[ll] == splitter[i]);

        size_t b = i * 2;                                   // < bucket
        if (i < numsplitters && splitter_tree[ll] == key) b += 1; // equal bucket

#endif

        return b;
    }

    /// classify all strings in area by walking tree and saving bucket id
    static inline void
    classify(string* strB, string* strE, uint16_t* bktout,
             const key_type* splitter, const key_type* splitter_tree,
             size_t depth)
    {
        for (string* str = strB; str != strE; )
        {
            key_type key = get_char<key_type>(*str++, depth);

            unsigned int b = find_bkt_tree(key, splitter, splitter_tree);
            *bktout++ = b;
        }
    }
};

struct ClassifyAssembler
{
    /// binary search on splitter array for bucket number
    static inline unsigned int
    find_bkt_tree(const key_type& key, const key_type* splitter, const key_type* splitter_tree0)
    {
        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i;

        // hand-coded assembler binary tree traversal with equality
        asm("mov    $1, %%rax \n"             // rax = i
            // body of while loop
            "1: \n"
            "cmpq	(%[splitter_tree],%%rax,8), %[key] \n"
            "lea    (%%rax,%%rax), %%rax \n"
            "lea    1(%%rax), %%rcx \n"
            "cmova  %%rcx, %%rax \n"           // CMOV rax = 2 * i + 1 
            "cmp 	%[numsplitters], %%rax \n" // i < numsplitters+1
            "jb     1b \n"
            "sub    %[numsplitters], %%rax \n" // i -= numsplitters+1;
            : "=&a" (i)
            : [key] "r" (key), [splitter_tree] "r" (splitter_tree), [numsplitters] "g" (numsplitters+1)
            : "ecx");

        size_t b = i * 2;                                   // < bucket
        if (i < numsplitters && splitter[i] == key) b += 1; // equal bucket

        return b;
    }

    /// classify all strings in area by walking tree and saving bucket id
    static inline void
    classify(string* strB, string* strE, uint16_t* bktout,
             const key_type* splitter, const key_type* splitter_tree,
             size_t depth)
    {
        for (string* str = strB; str != strE; )
        {
            key_type key = get_char<key_type>(*str++, depth);

            unsigned int b = find_bkt_tree(key, splitter, splitter_tree);
            *bktout++ = b;
        }
    }
};

struct ClassifyUnroll
{
    /// search in splitter tree for bucket number, unrolled for U keys at once.
    template <int U>
    __attribute__((optimize("unroll-all-loops"))) static inline void
    find_bkt_tree_unroll(const key_type key[U], const key_type* splitter, const key_type* splitter_tree0, uint16_t obkt[U])
    {
        // binary tree traversal without left branch

        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i[U];
        std::fill(i+0, i+U, 1);

        for (size_t l = 0; l < treebits; ++l) // asdfasdf
        {
            for (int u = 0; u < U; ++u)
            {
#if 0
                // in gcc-4.6.3 this produces a SETA, LEA sequence
                i[u] = 2 * i[u] + (key[u] <= splitter_tree[i[u]] ? 0 : 1);
#else
                // in gcc-4.6.3 this produces two LEA and a CMOV sequence, which is slightly faster
                if (key[u] <= splitter_tree[i[u]])
                    i[u] = 2*i[u] + 0;
                else // (key > splitter_tree[i[u]])
                    i[u] = 2*i[u] + 1;
#endif
            }
        }

        for (int u = 0; u < U; ++u)
            i[u] -= numsplitters+1;

        for (int u = 0; u < U; ++u)
            obkt[u] = i[u] * 2; // < bucket

        for (int u = 0; u < U; ++u)
        {
            if (i[u] < numsplitters && splitter[i[u]] == key[u]) obkt[u] += 1; // equal bucket
        }
    }

    /// classify all strings in area by walking tree and saving bucket id
    static inline void
    classify(string* strB, string* strE, uint16_t* bktout,
             const key_type* splitter, const key_type* splitter_tree,
             size_t depth)
    {
        for (string* str = strB; str != strE; )
        {
            static const int rollout = 6;

            if (str + rollout < strE)
            {
                key_type key[rollout];
                key[0] = get_char<key_type>(str[0], depth);
                key[1] = get_char<key_type>(str[1], depth);
                key[2] = get_char<key_type>(str[2], depth);
                key[3] = get_char<key_type>(str[3], depth);
                key[4] = get_char<key_type>(str[4], depth);
                key[5] = get_char<key_type>(str[5], depth);

                find_bkt_tree_unroll<rollout>(key, splitter, splitter_tree, bktout);

                str += rollout;
                bktout += rollout;
            }
            else
            {
                key_type key = get_char<key_type>(*str++, depth);

                unsigned int b = ClassifySimple::find_bkt_tree(key, splitter, splitter_tree);
                *bktout++ = b;
            }
        }
    }
};

// *******************************************************************************************************
// *** Variant 4 of string sample-sort: use super-scalar binary search on splitters, with index caching.

template <class Classify>
void sample_sortBTC(string* strings, size_t n, size_t depth)
{
    if (n < g_samplesort_smallsort)
    {
        g_rs_steps++;
        //return inssort::inssort(strings, n, depth);
        g_timer.change(TM_SMALLSORT);
        bingmann_radix_sort::msd_CI5(strings, n, depth);
        g_timer.change(TM_GENERAL);
        return;
    }
    g_ss_steps++;

    // step 1: select splitters with oversampling
    g_timer.change(TM_MAKE_SAMPLE);

    const size_t samplesize = oversample_factor * numsplitters;

    static key_type samples[ samplesize ];

    LCGRandom rng(&strings);

    for (unsigned int i = 0; i < samplesize; ++i)
    {
        samples[i] = get_char<key_type>(strings[ rng() % n ], depth);
    }

    std::sort(samples, samples + samplesize);

    g_timer.change(TM_MAKE_SPLITTER);

    key_type* splitter = new key_type[numsplitters];
    unsigned char* splitter_lcp = new unsigned char[numsplitters];

    DBG(debug_splitter, "splitter:");
    splitter_lcp[0] = 0; // sentinel for first < everything bucket
    for (size_t i = 0, j = oversample_factor/2; i < numsplitters; ++i)
    {
        splitter[i] = samples[j];
        DBG(debug_splitter, "key " << toHex(splitter[i]));

        if (i != 0) {
            key_type xorSplit = splitter[i-1] ^ splitter[i];

            DBG1(debug_splitter, "    XOR -> " << toHex(xorSplit) << " - ");

            DBG3(debug_splitter, count_high_zero_bits(xorSplit) << " bits = "
                << count_high_zero_bits(xorSplit) / 8 << " chars lcp");

            splitter_lcp[i] = count_high_zero_bits(xorSplit) / 8;
        }

        j += oversample_factor;
    }

    // step 2.1: construct splitter tree to perform binary search

    key_type* splitter_tree = new key_type[numsplitters];

    {
        size_t t = 0;
        size_t highbit = (numsplitters+1) / 2;

        while ( highbit > 0 )
        {
            DBG(debug_splitter_tree, "highbit = " << highbit);

            size_t p = highbit-1;
            size_t inc = highbit << 1;

            while ( p <= numsplitters )
            {
                DBG(debug_splitter_tree, "p = " << p);

                splitter_tree[t++] = splitter[p];

                p += inc;
            }

            highbit >>= 1;
        }
    }

    if (debug_splitter_tree)
    {
        DBG1(1, "splitter_tree: ");
        for (size_t i = 0; i < numsplitters; ++i)
        {
            DBG2(1, splitter_tree[i] << " ");
        }
        DBG3(1, "");
    }

    // step 2.2: classify all strings and count bucket sizes
    g_timer.change(TM_CLASSIFY);

#if 0
    uint16_t* bktcache = new uint16_t[n];

    static const size_t bktnum = 2*numsplitters+1;

    size_t* bktsize = new size_t[bktnum];
    memset(bktsize, 0, bktnum * sizeof(size_t));

    for (size_t si = 0; si < n; ++si)
    {
        // binary search in splitter with equal check
        key_type key = get_char<key_type>(strings[si], depth);

        unsigned int b = find_bkt(key, splitter, splitter_tree);

        assert(b < bktnum);

        bktcache[si] = b;
        ++bktsize[ b ];
    }

#else
    uint16_t* bktcache = new uint16_t[n];

    static const size_t bktnum = 2*numsplitters+1;

    Classify::classify(strings, strings+n, bktcache,
                       splitter, splitter_tree, depth);

    delete [] splitter_tree;

    size_t* bktsize = new size_t[bktnum];
    memset(bktsize, 0, bktnum * sizeof(size_t));

    for (size_t si = 0; si < n; ++si)
        ++bktsize[ bktcache[si] ];
#endif

    if (debug_bucketsize)
    {
        DBG1(1, "bktsize: ");
        for (size_t i = 0; i < bktnum; ++i)
        {
            DBG2(1, bktsize[i] << " ");
        }
        DBG3(1, "");
    }

    // step 3: prefix sum
    g_timer.change(TM_PREFIXSUM);

    size_t bktindex[bktnum];
    bktindex[0] = bktsize[0];
    size_t last_bkt_size = bktsize[0];
    for (unsigned int i=1; i < bktnum; ++i) {
        bktindex[i] = bktindex[i-1] + bktsize[i];
        if (bktsize[i]) last_bkt_size = bktsize[i];
    }
    assert(bktindex[bktnum-1] == n);

    // step 4: premute in-place
    g_timer.change(TM_PERMUTE);

    for (size_t i=0, j; i < n - last_bkt_size; )
    {
        string perm = strings[i];
        uint16_t permbkt = bktcache[i];

        while ( (j = --bktindex[ permbkt ]) > i )
        {
            std::swap(perm, strings[j]);
            std::swap(permbkt, bktcache[j]);
        }

        strings[i] = perm;
        i += bktsize[ permbkt ];
    }

    delete [] bktcache;

    // step 5: recursion
    g_timer.change(TM_GENERAL);

    size_t i = 0, bsum = 0;
    while (i < bktnum-1)
    {
        // i is even -> bkt[i] is less-than bucket
        if (bktsize[i] > 1)
        {
            DBG(debug_recursion, "Recurse[" << depth << "]: < bkt " << bsum << " size " << bktsize[i] << " lcp " << int(splitter_lcp[i/2]));
            sample_sortBTC<Classify>(strings+bsum, bktsize[i], depth + splitter_lcp[i/2]);
        }
        bsum += bktsize[i++];

        // i is odd -> bkt[i] is equal bucket
        if (bktsize[i] > 1)
        {
            if ( (splitter[i/2] & 0xFF) == 0 ) { // equal-bucket has NULL-terminated key, done.
                DBG(debug_recursion, "Recurse[" << depth << "]: = bkt " << bsum << " size " << bktsize[i] << " is done!");
            }
            else {
                DBG(debug_recursion, "Recurse[" << depth << "]: = bkt " << bsum << " size " << bktsize[i] << " lcp keydepth!");
                sample_sortBTC<Classify>(strings+bsum, bktsize[i], depth + sizeof(key_type));
            }
        }
        bsum += bktsize[i++];
    }
    if (bktsize[i] > 0)
    {
        DBG(debug_recursion, "Recurse[" << depth << "]: > bkt " << bsum << " size " << bktsize[i] << " no lcp");
        sample_sortBTC<Classify>(strings+bsum, bktsize[i], depth);
    }
    bsum += bktsize[i++];
    assert(i == bktnum && bsum == n);

    delete [] splitter_lcp;
    delete [] splitter;
    delete [] bktsize;
}

void bingmann_sample_sortBTC(string* strings, size_t n)
{
    sample_sort_pre();
    g_statscache >> "numsplitters" << numsplitters
                 >> "splitter_treebits" << treebits;
    sample_sortBTC<ClassifySimple>(strings,n,0);
    sample_sort_post();
}

CONTESTANT_REGISTER(bingmann_sample_sortBTC, "bingmann/sample_sortBTC",
                    "bingmann/sample_sortBTC (binary tree, bkt cache)")

void bingmann_sample_sortBTCA(string* strings, size_t n)
{
    sample_sort_pre();
    g_statscache >> "numsplitters" << numsplitters
                 >> "splitter_treebits" << treebits;
    sample_sortBTC<ClassifyAssembler>(strings,n,0);
    sample_sort_post();
}

CONTESTANT_REGISTER(bingmann_sample_sortBTCA, "bingmann/sample_sortBTCA",
                    "bingmann/sample_sortBTCA (binary tree, asm CMOV, bkt cache)")

void bingmann_sample_sortBTCU(string* strings, size_t n)
{
    sample_sort_pre();
    g_statscache >> "numsplitters" << numsplitters
                 >> "splitter_treebits" << treebits;
    sample_sortBTC<ClassifyUnroll>(strings,n,0);
    sample_sort_post();
}

CONTESTANT_REGISTER(bingmann_sample_sortBTCU, "bingmann/sample_sortBTCU",
                    "bingmann/sample_sortBTCU (binary tree, unrolled, bkt cache)")

// ----------------------------------------------------------------------------

#if 0 // improved version in bingmann-sample_sortBTCE.h

struct ClassifyEqualSimple
{
    // Variant 4.5 of string sample-sort: use super-scalar binary search on splitters with equality check and index caching.

    static inline unsigned int
    treeid_to_bkt(unsigned int id)
    {
        assert(id > 0);
        //std::cout << "index: " << id << " = " << binary(id) << "\n";

        //int treebits = 4;
        //int bitmask = ((1 << treebits)-1);
        static const int bitmask = numsplitters;

        int hi = treebits-32 + count_high_zero_bits<uint32_t>(id); // sdfsdf
        //std::cout << "high zero: " << hi << "\n";

        unsigned int bkt = ((id << (hi+1)) & bitmask) | (1 << hi);

        //std::cout << "bkt: " << bkt << " = " << binary(bkt) << "\n";
    
        return bkt;
    }

    /// search in splitter tree for bucket number
    static inline unsigned int
    find_bkt_tree_equal(const key_type& key, const key_type* /* splitter */, const key_type* splitter_tree0)
    {
        // binary tree traversal without left branch

        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i = 1;

        while ( i <= numsplitters )
        {
            if (key == splitter_tree[i])
                return 2 * treeid_to_bkt(i) - 1;
            else if (key < splitter_tree[i])
                i = 2*i + 0;
            else // (key > splitter_tree[i])
                i = 2*i + 1;
        }

        i -= numsplitters+1;

        return 2 * i; // < or > bucket
    }

    /// classify all strings in area by walking tree and saving bucket id
    static inline void
    classify(string* strB, string* strE, uint16_t* bktout,
             const key_type* splitter, const key_type* splitter_tree,
             size_t depth)
    {
        for (string* str = strB; str != strE; )
        {
            key_type key = get_char<key_type>(*str++, depth);

            unsigned int b = find_bkt_tree_equal(key, splitter, splitter_tree);
            *bktout++ = b;
        }
    }
};

struct ClassifyEqualAssembler
{
    /// binary search on splitter array for bucket number
    static inline unsigned int
    find_bkt_tree_asmequal(const key_type& key, const key_type* /* splitter */, const key_type* splitter_tree0)
    {
        const key_type* splitter_tree = splitter_tree0 - 1;
        unsigned int i;

        // hand-coded assembler binary tree traversal with equality
        asm("mov    $1, %%rax \n"             // rax = i
            // body of while loop
            "1: \n"
            "cmpq	(%[splitter_tree],%%rax,8), %[key] \n"
            "je     2f \n"
            "lea    (%%rax,%%rax), %%rax \n"
            "lea    1(%%rax), %%rcx \n"
            "cmova  %%rcx, %%rax \n"             // CMOV rax = 2 * i + 1 
            "cmp 	%[numsplitters1], %%rax \n"  // i < numsplitters+1
            "jb     1b \n"
            "sub    %[numsplitters1], %%rax \n"  // i -= numsplitters+1;
            "lea    (%%rax,%%rax), %%rax \n"     // i = i*2
            "jmp    3f \n"
            "2: \n"
            "bsr    %%rax, %%rdx \n"             // dx = bit number of highest one
            "mov    %[treebits], %%rcx \n"
            "sub    %%rdx, %%rcx \n"             // cx = treebits - highest
            "shl    %%cl, %%rax \n"              // shift ax to left
            "and    %[numsplitters], %%rax \n"   // mask off other bits
            "lea    -1(%%rcx), %%rcx \n"
            "mov    $1, %%rdx \n"                // dx = (1 << (hi-1))
            "shl    %%cl, %%rdx \n"              //
            "or     %%rdx, %%rax \n"             // ax = OR of both
            "lea    -1(%%rax,%%rax), %%rax \n"    // i = i * 2 - 1
            "3: \n"
            : "=&a" (i)
            : [key] "r" (key), [splitter_tree] "r" (splitter_tree),
              [numsplitters1] "g" (numsplitters+1),
              [treebits] "g" (treebits),
              [numsplitters] "g" (numsplitters)
            : "rcx", "rdx");

        return i;
    }

    /// classify all strings in area by walking tree and saving bucket id
    static inline void
    classify(string* strB, string* strE, uint16_t* bktout,
             const key_type* splitter, const key_type* splitter_tree,
             size_t depth)
    {
        for (string* str = strB; str != strE; )
        {
            key_type key = get_char<key_type>(*str++, depth);

            unsigned int b = find_bkt_tree_asmequal(key, splitter, splitter_tree);
            *bktout++ = b;
        }
    }
};

void bingmann_sample_sortBTCE1(string* strings, size_t n)
{
    sample_sort_pre();
    g_statscache >> "numsplitters" << numsplitters
                 >> "splitter_treebits" << treebits;
    sample_sortBTC<ClassifyEqualSimple>(strings,n,0);
    sample_sort_post();
}

CONTESTANT_REGISTER(bingmann_sample_sortBTCE1, "bingmann/sample_sortBTCE1",
                    "bingmann/sample_sortBTCE1 (binary tree equal, bkt cache)")

void bingmann_sample_sortBTCE1A(string* strings, size_t n)
{
    sample_sort_pre();
    g_statscache >> "numsplitters" << numsplitters
                 >> "splitter_treebits" << treebits;
    sample_sortBTC<ClassifyEqualAssembler>(strings,n,0);
    sample_sort_post();
}

CONTESTANT_REGISTER(bingmann_sample_sortBTCE1A, "bingmann/sample_sortBTCE1A",
                    "bingmann/sample_sortBTCE1A (binary tree equal, asm CMOV, bkt cache)")

#endif // improved version in bingmann-sample_sortBTCE.h

// ----------------------------------------------------------------------------

} // namespace bingmann_sample_sortBTC
