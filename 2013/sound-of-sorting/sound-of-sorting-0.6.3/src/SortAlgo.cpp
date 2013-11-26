/******************************************************************************
 * src/SortAlgo.cpp
 *
 * Implementations is many sorting algorithms.
 *
 * Note that these implementations may not be as good/fast as possible. Some
 * are modified so that the visualization is more instructive.
 *
 * Futhermore, some algorithms are annotated using the mark() and watch()
 * functions from WSortView. These functions add colors to the illustratation
 * and thereby makes the algorithm's visualization easier to explain.
 *
 ******************************************************************************
 * The algorithms in this file are copyrighted by the original authors. All
 * code is freely available.
 *
 * The source code added by myself (Timo Bingmann) and all modifications are
 * copyright (C) 2013 Timo Bingmann <tb@panthema.net>
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

#include "SortAlgo.h"
#include "WSortView.h"

#include <algorithm>
#include <numeric>
#include <limits>
#include <inttypes.h>

typedef ArrayItem value_type;

const struct AlgoEntry g_algolist[] =
{
    { _("Selection Sort"), &SelectionSort, NULL },
    { _("Insertion Sort"), &InsertionSort, NULL },
    { _("Merge Sort"), &MergeSort,
      _("Merge sort which merges two sorted sequences into a shadow array, and then copies it back to the shown array.") },
    { _("Quick Sort (LR ptrs)"), &QuickSortLR,
      _("Quick sort variant with left and right pointers.") },
    { _("Quick Sort (LL ptrs)"), &QuickSortLL,
      _("Quick sort variant from 3rd edition of CLRS: two pointers on left.") },
    { _("Quick Sort (ternary, LR ptrs)"), &QuickSortTernaryLR,
      _("Ternary-split quick sort variant, adapted from multikey quicksort by Bentley & Sedgewick: partitions \"=<?>=\" using two pairs of pointers at left and right, then copied to middle.") },
    { _("Quick Sort (ternary, LL ptrs)"), &QuickSortTernaryLL,
      _("Ternary-split quick sort variant: partitions \"<>?=\" using two pointers at left and one at right. Afterwards copies the \"=\" to middle.") },
    { _("Bubble Sort"), &BubbleSort, NULL },
    { _("Cocktail Shaker Sort"), &CocktailShakerSort, NULL },
    { _("Gnome Sort"), &GnomeSort, NULL },
    { _("Comb Sort"), &CombSort, NULL },
    { _("Shell Sort"), &ShellSort, NULL },
    { _("Heap Sort"), &HeapSort, NULL },
    { _("Smooth Sort"), &SmoothSort, NULL },
    { _("Odd-Even Sort"), &OddEvenSort, NULL },
    { _("Bitonic Sort"), &BitonicSort, NULL },
    { _("Radix Sort (LSD)"), &RadixSortLSD,
      _("Least significant digit radix sort, which copies item into a shadow array during counting.") },
    { _("Radix Sort (MSD)"), &RadixSortMSD,
      _("Most significant digit radix sort, which permutes items in-place by walking cycles.") },
    { _("std::sort (gcc)"), &StlSort, NULL },
    { _("std::stable_sort (gcc)"), &StlStableSort, NULL },
    { _("std::sort_heap (gcc)"), &StlHeapSort, NULL },
    { _("Tim Sort"), &TimSort, NULL },
    { _("Bogo Sort"), &BogoSort, NULL },
    { _("Bozo Sort"), &BozoSort, NULL },
    { _("Stooge Sort"), &StoogeSort, NULL },
    { _("Slow Sort"), &SlowSort, NULL },
    { NULL, NULL, NULL },
};

const size_t g_algolist_size = sizeof(g_algolist) / sizeof(g_algolist[0]) - 1;

// ****************************************************************************
// *** Selection Sort

void SelectionSort(WSortView& A)
{
    volatile ssize_t jMin = 0;
    A.watch(&jMin,2);

    for (size_t i = 0; i < A.size()-1; ++i)
    {
        jMin = i;

        for (size_t j = i+1; j < A.size(); ++j)
        {
            if (A[j] < A[jMin]) {
                A.mark_swap(j, jMin);
                jMin = j;
            }
        }

        A.swap(i, jMin);

        // mark the last good element
        if (i > 0) A.unmark(i-1);
        A.mark(i);
    }
    A.unwatch_all();
}

// ****************************************************************************
// *** Insertion Sort

// swaps every time (keeps all values visible)
void InsertionSort(WSortView& A)
{
    for (size_t i = 1; i < A.size(); ++i)
    {
        value_type key = A[i];
        A.mark(i);

        ssize_t j = i - 1;
        while (j >= 0 && A[j] > key)
        {
            A.swap(j, j+1);
            j--;
        }

        A.unmark(i);
    }
}

// with extra item on stack
void InsertionSort2(WSortView& A)
{
    for (size_t i = 1; i < A.size(); ++i)
    {
        value_type tmp, key = A[i];
        A.mark(i);

        ssize_t j = i - 1;
        while (j >= 0 && (tmp = A[j]) > key)
        {
            A.set(j + 1, tmp);
            j--;
        }
        A.set(j + 1, key);

        A.unmark(i);
    }
}

// ****************************************************************************
// *** Merge Sort (out-of-place with sentinels)

// by myself (Timo Bingmann)

void Merge(WSortView& A, size_t lo, size_t mid, size_t hi)
{
    // mark merge boundaries
    A.mark(lo);
    A.mark(mid,2);
    A.mark(hi-1);

    // allocate output
    std::vector<value_type> out(hi-lo);

    // merge
    size_t i = lo, j = mid, o = 0; // first and second halves
    while (i < mid && j < hi)
    {
        // copy out for fewer time steps
        value_type ai = A[i], aj = A[j];

        out[o++] = (ai < aj ? (++i, ai) : (++j, aj));
    }

    // copy rest
    while (i < mid) out[o++] = A[i++];
    while (j < hi) out[o++] = A[j++];

    ASSERT(o == hi-lo);

    A.unmark(mid);

    // copy back
    for (i = 0; i < hi-lo; ++i)
        A.set(lo + i, out[i]);

    A.unmark(lo);
    A.unmark(hi-1);
}

void MergeSort(WSortView& A, size_t lo, size_t hi)
{
    if (lo + 1 < hi)
    {
        size_t mid = (lo + hi) / 2;

        MergeSort(A, lo, mid);
        MergeSort(A, mid, hi);

        Merge(A, lo, mid, hi);
    }
}

void MergeSort(WSortView& A)
{
    return MergeSort(A, 0, A.size());
}

// ****************************************************************************
// *** Quick Sort Pivot Selection

QuickSortPivotType g_quicksort_pivot = PIVOT_FIRST;

// some quicksort variants use hi inclusive and some exclusive, we require it
// to be _exclusive_. hi == array.end()!
ssize_t QuickSortSelectPivot(WSortView& A, ssize_t lo, ssize_t hi)
{
    if (g_quicksort_pivot == PIVOT_FIRST)
        return lo;

    if (g_quicksort_pivot == PIVOT_LAST)
        return hi-1;

    if (g_quicksort_pivot == PIVOT_MID)
        return (lo + hi) / 2;

    if (g_quicksort_pivot == PIVOT_RANDOM)
        return lo + (rand() % (hi - lo));

    if (g_quicksort_pivot == PIVOT_MEDIAN3)
    {
        ssize_t mid = (lo + hi) / 2;

        // cases if two are equal
        if (A[lo] == A[mid]) return lo;
        if (A[lo] == A[hi-1] || A[mid] == A[hi-1]) return hi-1;

        // cases if three are different
        return A[lo] < A[mid]
            ? (A[mid] < A[hi-1] ? mid : (A[lo] < A[hi-1] ? hi-1 : lo))
            : (A[mid] > A[hi-1] ? mid : (A[lo] < A[hi-1] ? lo : hi-1));
    }

    return lo;
}

const wxChar* g_quicksort_pivot_text[] = {
    _("First Item"),
    _("Last Item"),
    _("Middle Item"),
    _("Random Item"),
    _("Median of Three"),
    NULL
};

// ****************************************************************************
// *** Quick Sort LR (in-place, pointers at left and right, pivot is middle element)

// by myself (Timo Bingmann), based on Hoare's original code

void QuickSortLR(WSortView& A, ssize_t lo, ssize_t hi)
{
    // pick pivot and watch
    volatile ssize_t p = QuickSortSelectPivot(A, lo, hi+1);

    value_type pivot = A[p];
    A.watch(&p,1);

    volatile ssize_t i = lo, j = hi;
    A.watch(&i,2);
    A.watch(&j,2);

    while (i <= j)
    {
        while (A[i] < pivot)
            i++;

        while (A[j] > pivot)
            j--;

        if (i <= j)
        {
            A.swap(i,j);

            // follow pivot if it is swapped
            if (p == i) p = j;
            else if (p == j) p = i;

            i++, j--;
        }
    }

    A.unwatch_all();

    if (lo < j)
        QuickSortLR(A, lo, j);

    if (i < hi)
        QuickSortLR(A, i, hi);
}

void QuickSortLR(WSortView& A)
{
    return QuickSortLR(A, 0, A.size()-1);
}

// ****************************************************************************
// *** Quick Sort LL (in-place, two pointers at left, pivot is first element and moved to right)

// by myself (Timo Bingmann), based on CLRS' 3rd edition

size_t PartitionLL(WSortView& A, size_t lo, size_t hi)
{
    // pick pivot and move to back
    size_t p = QuickSortSelectPivot(A, lo, hi);

    value_type pivot = A[p];
    A.swap(p, hi-1);
    A.mark(hi-1);

    volatile ssize_t i = lo;
    A.watch(&i,2);

    for (size_t j = lo; j < hi-1; ++j)
    {
        if (A[j] <= pivot) {
            A.swap(i, j);
            ++i;
        }
    }

    A.swap(i, hi-1);
    A.unmark(hi-1);
    A.unwatch_all();

    return i;
}

void QuickSortLL(WSortView& A, size_t lo, size_t hi)
{
    if (lo + 1 < hi)
    {
        size_t mid = PartitionLL(A, lo, hi);

        QuickSortLL(A, lo, mid);
        QuickSortLL(A, mid+1, hi);
    }
}

void QuickSortLL(WSortView& A)
{
    return QuickSortLL(A, 0, A.size());
}

// ****************************************************************************
// *** Quick Sort Ternary (in-place, two pointers at left, pivot is first element and moved to right)

// by myself (Timo Bingmann), loosely based on multikey quicksort by B&S

void QuickSortTernaryLR(WSortView& A, ssize_t lo, ssize_t hi)
{
    if (hi <= lo) return;

    int cmp;

    // pick pivot and swap to back
    ssize_t piv = QuickSortSelectPivot(A, lo, hi+1);
    A.swap(piv, hi);
    A.mark(hi);

    const value_type& pivot = A[hi];

    // schema: |p ===  |i <<< | ??? |j >>> |q === |piv
    volatile ssize_t i = lo, j = hi-1;
    volatile ssize_t p = lo, q = hi-1;

    A.watch(&i,2);
    A.watch(&j,2);

    for (;;)
    {
        // partition on left
        while (i <= j && (cmp = A[i].cmp(pivot)) <= 0)
        {
            if (cmp == 0) {
                A.mark(p,3);
                A.swap(i, p++);
            }
            ++i;
        }

        // partition on right
        while (i <= j && (cmp = A[j].cmp(pivot)) >= 0)
        {
            if (cmp == 0) {
                A.mark(q,3);
                A.swap(j, q--);
            }
            --j;
        }

        if (i > j) break;

        // swap item between < > regions
        A.swap(i++, j--);
    }

    // swap pivot to right place
    A.swap(i,hi);
    A.mark_swap(i,hi);

    ssize_t num_less = i - p;
    ssize_t num_greater = q - j;

    // swap equal ranges into center, but avoid swapping equal elements
    j = i-1; i = i+1;

    ssize_t pe = lo + std::min(p-lo, num_less);
    for (ssize_t k = lo; k < pe; k++, j--) {
        A.swap(k,j);
        A.mark_swap(k,j);
    }

    ssize_t qe = hi-1 - std::min(hi-1-q, num_greater-1); // one already greater at end
    for (ssize_t k = hi-1; k > qe; k--, i++) {
        A.swap(i,k);
        A.mark_swap(i,k);
    }

    A.unwatch_all();
    A.unmark_all();

    QuickSortTernaryLR(A, lo, lo + num_less - 1);
    QuickSortTernaryLR(A, hi - num_greater + 1, hi);
}

void QuickSortTernaryLR(WSortView& A)
{
    return QuickSortTernaryLR(A, 0, A.size()-1);
}

// ****************************************************************************
// *** Quick Sort LL (in-place, two pointers at left, pivot is first element and moved to right)

// by myself (Timo Bingmann)

std::pair<ssize_t,ssize_t> PartitionTernaryLL(WSortView& A, ssize_t lo, ssize_t hi)
{
    // pick pivot and swap to back
    ssize_t p = QuickSortSelectPivot(A, lo, hi);

    value_type pivot = A[p];
    A.swap(p, hi-1);
    A.mark(hi-1);

    volatile ssize_t i = lo, k = hi-1;
    A.watch(&i,2);

    for (ssize_t j = lo; j < k; ++j)
    {
        int cmp = A[j].cmp(pivot); // ternary comparison
        if (cmp == 0) {
            A.swap(--k, j);
            --j; // reclassify A[j]
            A.mark(k,3);
        }
        else if (cmp < 0) {
            A.swap(i++, j);
        }
    }

    // unwatch i, because the pivot is swapped there
    // in the first step of the following swap loop.
    A.unwatch_all();

    ssize_t j = i + (hi-k);

    for (ssize_t s = 0; s < hi-k; ++s) {
        A.swap(i+s, hi-1-s);
        A.mark_swap(i+s, hi-1-s);
    }
    A.unmark_all();

    return std::make_pair(i,j);
}

void QuickSortTernaryLL(WSortView& A, size_t lo, size_t hi)
{
    if (lo + 1 < hi)
    {
        std::pair<ssize_t,ssize_t> mid = PartitionTernaryLL(A, lo, hi);

        QuickSortTernaryLL(A, lo, mid.first);
        QuickSortTernaryLL(A, mid.second, hi);
    }
}

void QuickSortTernaryLL(WSortView& A)
{
    return QuickSortTernaryLL(A, 0, A.size());
}

// ****************************************************************************
// *** Bubble Sort

void BubbleSort(WSortView& A)
{
    for (size_t i = 0; i < A.size()-1; ++i)
    {
        for (size_t j = 0; j < A.size()-1 - i; ++j)
        {
            if (A[j] > A[j + 1])
            {
                A.swap(j, j+1);
            }
        }
    }
}

// ****************************************************************************
// *** Cocktail Shaker Sort

// from http://de.wikibooks.org/wiki/Algorithmen_und_Datenstrukturen_in_C/_Shakersort

void CocktailShakerSort(WSortView& A)
{
    size_t lo = 0, hi = A.size()-1, mov = lo;

    while (lo < hi)
    {
        for (size_t i = hi; i > lo; --i)
        {
            if (A[i-1] > A[i])
            {
                A.swap(i-1, i);
                mov = i;
            }
        }

        lo = mov;

        for (size_t i = lo; i < hi; ++i)
        {
            if (A[i] > A[i+1])
            {
                A.swap(i, i+1);
                mov = i;
            }
        }

        hi = mov;
    }
}

// ****************************************************************************
// *** Gnome Sort

// from http://en.wikipediA.org/wiki/Gnome_sort

void GnomeSort(WSortView& A)
{
    for (size_t i = 1; i < A.size(); )
    {
        if (A[i] >= A[i-1])
        {
            ++i;
        }
        else
        {
            A.swap(i, i-1);
            if (i > 1) --i;
        }
    }
}

// ****************************************************************************
// *** Comb Sort

// from http://en.wikipediA.org/wiki/Comb_sort

void CombSort(WSortView& A)
{
    const double shrink = 1.3;

    bool swapped = false;
    size_t gap = A.size();

    while ((gap > 1) || swapped)
    {
        if (gap > 1) {
            gap = (size_t)((float)gap / shrink);
        }

        swapped = false;

        for (size_t i = 0; gap + i < A.size(); ++i)
        {
            if (A[i] > A[i + gap])
            {
                A.swap(i, i+gap);
                swapped = true;
            }
        }
    }
}

// ****************************************************************************
// *** Odd-Even Sort

// from http://en.wikipediA.org/wiki/Odd%E2%80%93even_sort

void OddEvenSort(WSortView& A)
{
    bool sorted = false;

    while (!sorted)
    {
        sorted = true;

        for (size_t i = 1; i < A.size()-1; i += 2)
        {
            if(A[i] > A[i+1])
            {
                A.swap(i, i+1);
                sorted = false;
            }
        }

        for (size_t i = 0; i < A.size()-1; i += 2)
        {
            if(A[i] > A[i+1])
            {
                A.swap(i, i+1);
                sorted = false;
            }
        }
    }
}

// ****************************************************************************
// *** Shell Sort

// with gaps by Robert Sedgewick from http://www.cs.princeton.edu/~rs/shell/shell.c

void ShellSort(WSortView& A)
{
    size_t incs[16] = { 1391376, 463792, 198768, 86961, 33936,
                        13776, 4592, 1968, 861, 336,
                        112, 48, 21, 7, 3, 1 };

    for (size_t k = 0; k < 16; k++)
    {
        for (size_t h = incs[k], i = h; i < A.size(); i++)
        {
            value_type v = A[i];
            size_t j = i;

            while (j >= h && A[j-h] > v)
            {
                A.set(j, A[j-h]);
                j -= h;
            }

            A[j] = v;
        }
    }
}

// ****************************************************************************
// *** Heap Sort

// heavily adapted from http://www.codecodex.com/wiki/Heapsort

bool isPowerOfTwo(size_t x)
{
    return ((x != 0) && !(x & (x - 1)));
}

uint32_t prevPowerOfTwo(uint32_t x)
{
    x |= x >> 1; x |= x >> 2; x |= x >> 4;
    x |= x >> 8; x |= x >> 16;
    return x - (x >> 1);
}

void HeapSort(WSortView& A)
{
    size_t n = A.size(), i = n / 2;

    // mark heap levels with different colors
    for (size_t j = i; j < n; ++j)
        A.mark(j, log(prevPowerOfTwo(j+1)) / log(2) + 3);

    while (1)
    {
        if (i > 0) {
            // build heap, sift A[i] down the heap
            i--;
        }
        else {
            // pop largest element from heap: swap front to back, and sift
            // front A[0] down the heap
            n--;
            if (n == 0) return;
            A.swap(0,n);

            A.mark(n);
            if (n+1 < A.size()) A.unmark(n+1);
        }

        size_t parent = i;
        size_t child = i*2 + 1;

        // sift operation - push the value of A[i] down the heap
        while (child < n)
        {
            if (child + 1 < n && A[child + 1] > A[child]) {
                child++;
            }
            if (A[child] > A[parent]) {
                A.swap(parent, child);
                parent = child;
                child = parent*2+1;
            }
            else {
                break;
            }
        }

        // mark heap levels with different colors
        A.mark(i, log(prevPowerOfTwo(i+1)) / log(2) + 3);
    }

}

// ****************************************************************************
// *** Radix Sort (counting sort, most significant digit (MSD) first, in-place redistribute)

// by myself (Timo Bingmann)

void RadixSortMSD(WSortView& A, size_t lo, size_t hi, size_t depth)
{
    A.mark(lo); A.mark(hi-1);

    // radix and base calculations
    const unsigned int RADIX = 4;

    unsigned int pmax = floor( log(A.array_max()) / log(RADIX) );
    ASSERT(depth <= pmax);

    size_t base = pow(RADIX, pmax - depth);

    // count digits
    std::vector<size_t> count(RADIX, 0);

    for (size_t i = lo; i < hi; ++i)
    {
        size_t r = A[i].get() / base % RADIX;
        ASSERT(r < RADIX);
        count[r]++;
    }

    // inclusive prefix sum
    std::vector<size_t> bkt(RADIX, 0);
    std::partial_sum(count.begin(), count.end(), bkt.begin());

    // mark bucket boundaries
    for (size_t i = 0; i < bkt.size(); ++i) {
        if (bkt[i] == 0) continue;
        A.mark(lo + bkt[i]-1, 2);
    }

    // reorder items in-place by walking cycles
    for (size_t i=0, j; i < (hi-lo); )
    {
        while ( (j = --bkt[ (A[lo+i].get() / base % RADIX) ]) > i )
        {
            A.swap(lo + i, lo + j);
        }
        i += count[ (A[lo+i].get() / base % RADIX) ];
    }

    A.unmark_all();

    // no more depth to sort?
    if (depth+1 > pmax) return;

    // recurse on buckets
    size_t sum = lo;
    for (size_t i = 0; i < RADIX; ++i)
    {
        if (count[i] <= 1) continue;
        RadixSortMSD(A, sum, sum+count[i], depth+1);
        sum += count[i];
    }
}

void RadixSortMSD(WSortView& A)
{
    return RadixSortMSD(A, 0, A.size(), 0);
}

// ****************************************************************************
// *** Radix Sort (counting sort, least significant digit (LSD) first, out-of-place redistribute)

// by myself (Timo Bingmann)

void RadixSortLSD(WSortView& A)
{
    // radix and base calculations
    const unsigned int RADIX = 4;

    unsigned int pmax = floor( log(A.array_max()) / log(RADIX) );

    for (unsigned int p = 0; p <= pmax; ++p)
    {
        size_t base = pow(RADIX, p);

        // count digits and copy data
        std::vector<size_t> count(RADIX, 0);
        std::vector<value_type> copy(A.size());

        for (size_t i = 0; i < A.size(); ++i)
        {
            size_t r = (copy[i] = A[i]).get() / base % RADIX;
            ASSERT(r < RADIX);
            count[r]++;
        }

        // exclusive prefix sum
        std::vector<size_t> bkt(RADIX+1, 0);
        std::partial_sum(count.begin(), count.end(), bkt.begin()+1);

        // mark bucket boundaries
        for (size_t i = 0; i < bkt.size()-1; ++i) {
            if (bkt[i] >= A.size()) continue;
            A.mark(bkt[i], 2);
        }

        // redistribute items back into array (stable)
        for (size_t i=0; i < A.size(); ++i)
        {
            size_t r = copy[i].get() / base % RADIX;
            A[ bkt[r]++ ] = copy[i];
        }

        A.unmark_all();
    }
}

// ****************************************************************************
// *** Use STL Sorts via Iterator Adapters

// iterator based on http://zotu.blogspot.de/2010/01/creating-random-access-iterator.html

class MyIterator : public std::iterator<std::random_access_iterator_tag, value_type>
{
protected:
    WSortView*  m_array;
    size_t      m_pos;

public:
    typedef std::iterator<std::random_access_iterator_tag, value_type> base_type;

    typedef std::random_access_iterator_tag iterator_category;

    typedef base_type::value_type value_type;
    typedef base_type::difference_type difference_type;
    typedef base_type::reference reference;
    typedef base_type::pointer pointer;

    MyIterator() : m_array(NULL), m_pos(0) {}

    MyIterator(WSortView* A, size_t p) : m_array(A), m_pos(p) {}

    MyIterator(const MyIterator& r) : m_array(r.m_array), m_pos(r.m_pos) {}

    MyIterator& operator=(const MyIterator& r)
    { m_array = r.m_array, m_pos = r.m_pos; return *this; }

    MyIterator& operator++()
    { ++m_pos; return *this; }

    MyIterator& operator--()
    { --m_pos; return *this; }

    MyIterator operator++(int)
    { return MyIterator(m_array, m_pos++); }

    MyIterator operator--(int)
    { return MyIterator(m_array, m_pos--); }

    MyIterator operator+(const difference_type& n) const
    { return MyIterator(m_array, m_pos + n); }

    MyIterator& operator+=(const difference_type& n)
    { m_pos += n; return *this; }

    MyIterator operator-(const difference_type& n) const
    { return MyIterator(m_array, m_pos - n); }

    MyIterator& operator-=(const difference_type& n)
    { m_pos -= n; return *this; }

    reference operator*() const
    { return (*m_array)[m_pos]; }

    pointer operator->() const
    { return &(*m_array)[m_pos]; }

    reference operator[](const difference_type& n) const
    { return (*m_array)[n]; }

    bool operator==(const MyIterator& r)
    { return (m_array == r.m_array) && (m_pos == r.m_pos); }

    bool operator!=(const MyIterator& r)
    { return (m_array != r.m_array) || (m_pos != r.m_pos); }

    bool operator<(const MyIterator& r)
    { return (m_array == r.m_array ? (m_pos < r.m_pos) : (m_array < r.m_array)); }

    bool operator>(const MyIterator& r)
    { return (m_array == r.m_array ? (m_pos > r.m_pos) : (m_array > r.m_array)); }

    bool operator<=(const MyIterator& r)
    { return (m_array == r.m_array ? (m_pos <= r.m_pos) : (m_array <= r.m_array)); }

    bool operator>=(const MyIterator& r)
    { return (m_array == r.m_array ? (m_pos >= r.m_pos) : (m_array >= r.m_array)); }

    difference_type operator+(const MyIterator& r2) const
    { ASSERT(m_array == r2.m_array); return (m_pos + r2.m_pos); }

    difference_type operator-(const MyIterator& r2) const
    { ASSERT(m_array == r2.m_array); return (m_pos - r2.m_pos); }
};

void StlSort(WSortView& A)
{
    std::sort(MyIterator(&A,0), MyIterator(&A,A.size()));
}

void StlStableSort(WSortView& A)
{
    std::stable_sort(MyIterator(&A,0), MyIterator(&A,A.size()));
}

void StlHeapSort(WSortView& A)
{
    std::make_heap(MyIterator(&A,0), MyIterator(&A,A.size()));
    std::sort_heap(MyIterator(&A,0), MyIterator(&A,A.size()));
}

// ****************************************************************************
// *** BogoSort and more slow sorts

// by myself (Timo Bingmann)

bool BogoCheckSorted(WSortView& A)
{
    size_t i;
    value_type prev = A[0];
    A.mark(0);
    for (i = 1; i < A.size(); ++i)
    {
        value_type val = A[i];
        if (prev > val) break;
        prev = val;
        A.mark(i);
    }

    if (i == A.size()) {
        // this is amazing.
        return true;
    }

    // unmark
    while (i > 0) A.unmark(i--);
    A.unmark(0);

    return false;
}

void BogoSort(WSortView& A)
{
    // keep a permutation of [0,size)
    std::vector<size_t> perm(A.size());

    for (size_t i = 0; i < A.size(); ++i)
        perm[i] = i;

    while (1)
    {
        // check if array is sorted
        if (BogoCheckSorted(A)) break;

        // pick a random permutation of indexes
        std::random_shuffle(perm.begin(), perm.end());

        // permute array in-place
        std::vector<char> pmark(A.size(), 0);

        for (size_t i = 0; i < A.size(); ++i)
        {
            if (pmark[i]) continue;

            // walk a cycle
            size_t j = i;

            //std::cout << "cycle start " << j << " -> " << perm[j] << "\n";

            while ( perm[j] != i )
            {
                ASSERT(!pmark[j]);
                A.swap(j, perm[j]);
                pmark[j] = 1;

                j = perm[j];
                //std::cout << "cycle step " << j << " -> " << perm[j] << "\n";
            }
            //std::cout << "cycle end\n";

            ASSERT(!pmark[j]);
            pmark[j] = 1;
        }

        //std::cout << "permute end\n";

        for (size_t i = 0; i < A.size(); ++i)
            ASSERT(pmark[i]);
    }
}

void BozoSort(WSortView& A)
{
    srand(time(NULL));

    while (1)
    {
        // check if array is sorted
        if (BogoCheckSorted(A)) break;

        // swap two random items
        A.swap(rand() % A.size(), rand() % A.size());
    }
}

// ****************************************************************************
// *** Bitonic Sort

// from http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/oddn.htm

namespace BitonicSortNS {

static const bool ASCENDING = true;    // sorting direction

static void compare(WSortView& A, int i, int j, bool dir)
{
    if (dir == (A[i] > A[j]))
        A.swap(i, j);
}

static int greatestPowerOfTwoLessThan(int n)
{
    int k = 1;
    while (k < n) k = k << 1;
    return k >> 1;
}

static void bitonicMerge(WSortView& A, int lo, int n, bool dir)
{
    if (n > 1)
    {
        int m = greatestPowerOfTwoLessThan(n);

        for (int i = lo; i < lo + n - m; i++)
            compare(A, i, i+m, dir);

        bitonicMerge(A, lo, m, dir);
        bitonicMerge(A, lo + m, n - m, dir);
    }
}

static void bitonicSort(WSortView& A, int lo, int n, bool dir)
{
    if (n > 1)
    {
        int m = n / 2;
        bitonicSort(A, lo, m, !dir);
        bitonicSort(A, lo + m, n - m, dir);
        bitonicMerge(A, lo, n, dir);
    }
}

} // namespace BitonicSortNS

void BitonicSort(WSortView& A)
{
    BitonicSortNS::bitonicSort(A, 0, A.size(), BitonicSortNS::ASCENDING);
}

// ****************************************************************************
// *** Smooth Sort

// from http://en.wikipediA.org/wiki/Smoothsort

namespace SmoothSortNS {

static const int LP[] = {
    1, 1, 3, 5, 9, 15, 25, 41, 67, 109,
    177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529, 21891,
    35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457,
    1664079, 2692537, 4356617, 7049155, 11405773, 18454929, 29860703,
    48315633, 78176337, 126491971, 204668309, 331160281, 535828591,
    866988873 // the next number is > 31 bits.
};

static void sift(WSortView& A, int pshift, int head)
{
    // we do not use Floyd's improvements to the heapsort sift, because we
    // are not doing what heapsort does - always moving nodes from near
    // the bottom of the tree to the root.

    value_type val = A[head];

    while (pshift > 1)
    {
        int rt = head - 1;
        int lf = head - 1 - LP[pshift - 2];

        if (val.compareTo(A[lf]) >= 0 && val.compareTo(A[rt]) >= 0)
            break;

        if (A[lf].compareTo(A[rt]) >= 0) {
            A[head] = A[lf];
            head = lf;
            pshift -= 1;
        }
        else {
            A[head] = A[rt];
            head = rt;
            pshift -= 2;
        }
    }

    A[head] = val;
}

static void trinkle(WSortView& A, int p, int pshift, int head, bool isTrusty)
{
    value_type val = A[head];

    while (p != 1)
    {
        int stepson = head - LP[pshift];

        if (A[stepson].compareTo(val) <= 0)
            break; // current node is greater than head. sift.

        // no need to check this if we know the current node is trusty,
        // because we just checked the head (which is val, in the first
        // iteration)
        if (!isTrusty && pshift > 1) {
            int rt = head - 1;
            int lf = head - 1 - LP[pshift - 2];
            if (A[rt].compareTo(A[stepson]) >= 0
                || A[lf].compareTo(A[stepson]) >= 0)
                break;
        }

        A[head] = A[stepson];

        head = stepson;
        //int trail = Integer.numberOfTrailingZeros(p & ~1);
        int trail = __builtin_ctz(p & ~1);
        p >>= trail;
        pshift += trail;
        isTrusty = false;
    }

    if (!isTrusty) {
        A[head] = val;
        sift(A, pshift, head);
    }
}

void sort(WSortView& A, int lo, int hi)
{
    int head = lo; // the offset of the first element of the prefix into m

    // These variables need a little explaining. If our string of heaps
    // is of length 38, then the heaps will be of size 25+9+3+1, which are
    // Leonardo numbers 6, 4, 2, 1.
    // Turning this into a binary number, we get b01010110 = 0x56. We represent
    // this number as a pair of numbers by right-shifting all the zeros and
    // storing the mantissa and exponent as "p" and "pshift".
    // This is handy, because the exponent is the index into L[] giving the
    // size of the rightmost heap, and because we can instantly find out if
    // the rightmost two heaps are consecutive Leonardo numbers by checking
    // (p&3)==3

    int p = 1; // the bitmap of the current standard concatenation >> pshift
    int pshift = 1;

    while (head < hi)
    {
        if ((p & 3) == 3) {
            // Add 1 by merging the first two blocks into a larger one.
            // The next Leonardo number is one bigger.
            sift(A, pshift, head);
            p >>= 2;
            pshift += 2;
        }
        else {
            // adding a new block of length 1
            if (LP[pshift - 1] >= hi - head) {
                // this block is its final size.
                trinkle(A, p, pshift, head, false);
            } else {
                // this block will get merged. Just make it trusty.
                sift(A, pshift, head);
            }

            if (pshift == 1) {
                // LP[1] is being used, so we add use LP[0]
                p <<= 1;
                pshift--;
            } else {
                // shift out to position 1, add LP[1]
                p <<= (pshift - 1);
                pshift = 1;
            }
        }
        p |= 1;
        head++;
    }

    trinkle(A, p, pshift, head, false);

    while (pshift != 1 || p != 1)
    {
        if (pshift <= 1) {
            // block of length 1. No fiddling needed
            //int trail = Integer.numberOfTrailingZeros(p & ~1);
            int trail = __builtin_ctz(p & ~1);
            p >>= trail;
            pshift += trail;
        }
        else {
            p <<= 2;
            p ^= 7;
            pshift -= 2;

            // This block gets broken into three bits. The rightmost bit is a
            // block of length 1. The left hand part is split into two, a block
            // of length LP[pshift+1] and one of LP[pshift].  Both these two
            // are appropriately heapified, but the root nodes are not
            // necessarily in order. We therefore semitrinkle both of them

            trinkle(A, p >> 1, pshift + 1, head - LP[pshift] - 1, true);
            trinkle(A, p, pshift, head - 1, true);
        }

        head--;
    }
}

} // namespace SmoothSortNS

void SmoothSort(WSortView& A)
{
    return SmoothSortNS::sort(A, 0, A.size()-1);
}

// ****************************************************************************
// *** Stooge Sort

void StoogeSort(WSortView& A, int i, int j)
{
    if (A[i] > A[j])
    {
        A.swap(i, j);
    }

    if (j - i + 1 >= 3)
    {
        int t = (j - i + 1) / 3;

        A.mark(i, 2);
        A.mark(j, 2);

        StoogeSort(A, i, j-t);
        StoogeSort(A, i+t, j);
        StoogeSort(A, i, j-t);

        A.unmark(i);
        A.unmark(j);
    }
}

void StoogeSort(WSortView& A)
{
    StoogeSort(A, 0, A.size()-1);
}

// ****************************************************************************
// *** Slow Sort

void SlowSort(WSortView& A, int i, int j)
{
    if (i >= j) return;

    int m = (i + j) / 2;

    SlowSort(A, i, m);
    SlowSort(A, m+1, j);

    if (A[m] > A[j])
        A.swap(m, j);

    A.mark(j, 1);

    SlowSort(A, i, j-1);

    A.unmark(j);
}

void SlowSort(WSortView& A)
{
    SlowSort(A, 0, A.size()-1);
}

// ****************************************************************************
// *** TimSort

// http://en.wikipedia.org/wiki/Timsort
// code from https://github.com/gfx/cpp-TimSort

/*
 * C++ implementation of timsort
 *
 * ported from Python's and OpenJDK's:
 * - http://svn.python.org/projects/python/trunk/Objects/listobject.c
 * - http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java
 *
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
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

#ifdef ENABLE_TIMSORT_LOG
#define GFX_TIMSORT_LOG(expr) (std::clog << "# " << __func__ << ": " << expr << std::endl)
#else
#define GFX_TIMSORT_LOG(expr) ((void)0)
#endif

#if ENABLE_STD_MOVE && __cplusplus >= 201103L
#define GFX_TIMSORT_MOVE(x) std::move(x)
#else
#define GFX_TIMSORT_MOVE(x) (x)
#endif

namespace TimSortNS {

// ---------------------------------------
// Declaration
// ---------------------------------------

/**
 * Same as std::stable_sort(first, last).
 */
template<typename RandomAccessIterator>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last);

/**
 * Same as std::stable_sort(first, last, c).
 */
template<typename RandomAccessIterator, typename LessFunction>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last, LessFunction compare);


// ---------------------------------------
// Implementation
// ---------------------------------------

template <typename Value, typename LessFunction>
class Compare {
public:
    typedef Value        value_type;
    typedef LessFunction func_type;

    Compare(LessFunction f)
        : less_(f) { }
    Compare(const Compare<value_type, func_type>& other)
        : less_(other.less_) { }

    bool lt(value_type x, value_type y) {
        return less_(x, y);
    }
    bool le(value_type x, value_type y) {
        return less_(x, y) || !less_(y, x);
    }
    bool gt(value_type x, value_type y) {
        return !less_(x, y) && less_(y, x);
    }
    bool ge(value_type x, value_type y) {
        return !less_(x, y);
    }

    func_type& less_function() {
        return less_;
    }
private:
    func_type less_;
};

template <typename RandomAccessIterator, typename LessFunction>
class TimSort
{
    typedef RandomAccessIterator iter_t;
    typedef typename std::iterator_traits<iter_t>::value_type value_t;
    typedef typename std::iterator_traits<iter_t>::reference ref_t;
    typedef typename std::iterator_traits<iter_t>::difference_type diff_t;
    typedef Compare<const value_t&, LessFunction> compare_t;

    static const int MIN_MERGE = 32;

    compare_t comp_;

    static const int MIN_GALLOP = 7;

    int minGallop_; // default to MIN_GALLOP

    std::vector<value_t> tmp_; // temp storage for merges
    typedef typename std::vector<value_t>::iterator tmp_iter_t;

    struct run {
        iter_t base;
        diff_t len;

        run(iter_t const b, diff_t const l) : base(b), len(l) { }
    };
    std::vector<run> pending_;

    static
    void sort(iter_t lo, iter_t hi, compare_t c) {
        assert( lo <= hi );

        diff_t nRemaining = (hi - lo);
        if(nRemaining < 2) {
            return; // nothing to do
        }

        if(nRemaining < MIN_MERGE) {
            diff_t const initRunLen = countRunAndMakeAscending(lo, hi, c);
            GFX_TIMSORT_LOG("initRunLen: " << initRunLen);
            binarySort(lo, hi, lo + initRunLen, c);
            return;
        }

        TimSort ts(c);
        diff_t const minRun = minRunLength(nRemaining);
        iter_t cur          = lo;
        do {
            diff_t runLen = countRunAndMakeAscending(cur, hi, c);

            if(runLen < minRun) {
                diff_t const force  = std::min(nRemaining, minRun);
                binarySort(cur, cur + force, cur + runLen, c);
                runLen = force;
            }

            ts.pushRun(cur, runLen);
            ts.mergeCollapse();

            cur        += runLen;
            nRemaining -= runLen;
        } while(nRemaining != 0);

        assert( cur == hi );
        ts.mergeForceCollapse();
        assert( ts.pending_.size() == 1 );

        GFX_TIMSORT_LOG("size: " << (hi - lo) << " tmp_.size(): " << ts.tmp_.size() << " pending_.size(): " << ts.pending_.size());
    } // sort()

    static
    void binarySort(iter_t lo, iter_t hi, iter_t start, compare_t compare) {
        assert( lo <= start && start <= hi );
        if(start == lo) {
            ++start;
        }
        for( ; start < hi; ++start ) {
            assert(lo <= start);
            /*const*/ value_t pivot = GFX_TIMSORT_MOVE(*start);

            iter_t const pos = std::upper_bound(lo, start, pivot, compare.less_function());
            for(iter_t p = start; p > pos; --p) {
                *p = GFX_TIMSORT_MOVE(*(p - 1));
            }
            *pos = GFX_TIMSORT_MOVE(pivot);
        }
    }

    static
    diff_t countRunAndMakeAscending(iter_t lo, iter_t hi, compare_t compare) {
        assert( lo < hi );

        iter_t runHi = lo + 1;
        if( runHi == hi ) {
            return 1;
        }

        if(compare.lt(*(runHi++), *lo)) { // descending
            while(runHi < hi && compare.lt(*runHi, *(runHi - 1))) {
                ++runHi;
            }
            std::reverse(lo, runHi);
        }
        else { // ascending
            while(runHi < hi && compare.ge(*runHi, *(runHi - 1))) {
                ++runHi;
            }
        }

        return runHi - lo;
    }

    static
    diff_t minRunLength(diff_t n) {
        assert( n >= 0 );

        diff_t r = 0;
        while(n >= MIN_MERGE) {
            r |= (n & 1);
            n >>= 1;
        }
        return n + r;
    }

    TimSort(compare_t c)
        : comp_(c), minGallop_(MIN_GALLOP) {
    }

    void pushRun(iter_t const runBase, diff_t const runLen) {
        pending_.push_back(run(runBase, runLen));
    }

    void mergeCollapse() {
        while( pending_.size() > 1 ) {
            diff_t n = pending_.size() - 2;

            if(n > 0 && pending_[n - 1].len <= pending_[n].len + pending_[n + 1].len) {
                if(pending_[n - 1].len < pending_[n + 1].len) {
                    --n;
                }
                mergeAt(n);
            }
            else if(pending_[n].len <= pending_[n + 1].len) {
                mergeAt(n);
            }
            else {
                break;
            }
        }
    }

    void mergeForceCollapse() {
        while( pending_.size() > 1 ) {
            diff_t n = pending_.size() - 2;

            if(n > 0 && pending_[n - 1].len < pending_[n + 1].len) {
                --n;
            }
            mergeAt(n);
        }
    }

    void mergeAt(diff_t const i) {
        diff_t const stackSize = pending_.size();
        assert( stackSize >= 2 );
        assert( i >= 0 );
        assert( i == stackSize - 2 || i == stackSize - 3 );

        iter_t base1 = pending_[i].base;
        diff_t len1  = pending_[i].len;
        iter_t base2 = pending_[i + 1].base;
        diff_t len2  = pending_[i + 1].len;

        assert( len1 > 0 && len2 > 0 );
        assert( base1 + len1 == base2 );

        pending_[i].len = len1 + len2;

        if(i == stackSize - 3) {
            pending_[i + 1] = pending_[i + 2];
        }

        pending_.pop_back();

        diff_t const k = gallopRight(*base2, base1, len1, 0);
        assert( k >= 0 );

        base1 += k;
        len1  -= k;

        if(len1 == 0) {
            return;
        }

        len2 = gallopLeft(*(base1 + (len1 - 1)), base2, len2, len2 - 1);
        assert( len2 >= 0 );
        if(len2 == 0) {
            return;
        }

        if(len1 <= len2) {
            mergeLo(base1, len1, base2, len2);
        }
        else {
            mergeHi(base1, len1, base2, len2);
        }
    }

    template <typename Iter>
    diff_t gallopLeft(ref_t key, Iter const base, diff_t const len, diff_t const hint) {
        assert( len > 0 && hint >= 0 && hint < len );

        diff_t lastOfs = 0;
        diff_t ofs = 1;

        if(comp_.gt(key, *(base + hint))) {
            diff_t const maxOfs = len - hint;
            while(ofs < maxOfs && comp_.gt(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs     += hint;
        }
        else {
            diff_t const maxOfs = hint + 1;
            while(ofs < maxOfs && comp_.le(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs          = hint - ofs;
            ofs              = hint - tmp;
        }
        assert( -1 <= lastOfs && lastOfs < ofs && ofs <= len );

        return std::lower_bound(base+(lastOfs+1), base+ofs, key, comp_.less_function()) - base;
    }

    template <typename Iter>
    diff_t gallopRight(ref_t key, Iter const base, diff_t const len, diff_t const hint) {
        assert( len > 0 && hint >= 0 && hint < len );

        diff_t ofs = 1;
        diff_t lastOfs = 0;

        if(comp_.lt(key, *(base + hint))) {
            diff_t const maxOfs = hint + 1;
            while(ofs < maxOfs && comp_.lt(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs          = hint - ofs;
            ofs              = hint - tmp;
        }
        else {
            diff_t const maxOfs = len - hint;
            while(ofs < maxOfs && comp_.ge(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs     += hint;
        }
        assert( -1 <= lastOfs && lastOfs < ofs && ofs <= len );

        return std::upper_bound(base+(lastOfs+1), base+ofs, key, comp_.less_function()) - base;
    }

    void mergeLo(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        assert( len1 > 0 && len2 > 0 && base1 + len1 == base2 );

        copy_to_tmp(base1, len1);

        tmp_iter_t cursor1 = tmp_.begin();
        iter_t cursor2     = base2;
        iter_t dest        = base1;

        *(dest++) = *(cursor2++);
        if(--len2 == 0) {
            std::copy(cursor1, cursor1 + len1, dest);
            return;
        }
        if(len1 == 1) {
            std::copy(cursor2, cursor2 + len2, dest);
            *(dest + len2) = *cursor1;
            return;
        }

        int minGallop(minGallop_);

        // outer:
        while(true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert( len1 > 1 && len2 > 0 );

                if(comp_.lt(*cursor2, *cursor1)) {
                    *(dest++) = *(cursor2++);
                    ++count2;
                    count1 = 0;
                    if(--len2 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                else {
                    *(dest++) = *(cursor1++);
                    ++count1;
                    count2 = 0;
                    if(--len1 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while( (count1 | count2) < minGallop );
            if(break_outer) {
                break;
            }

            do {
                assert( len1 > 1 && len2 > 0 );

                count1 = gallopRight(*cursor2, cursor1, len1, 0);
                if(count1 != 0) {
                    std::copy_backward(cursor1, cursor1 + count1, dest + count1);
                    dest    += count1;
                    cursor1 += count1;
                    len1    -= count1;

                    if(len1 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = *(cursor2++);
                if(--len2 == 0) {
                    break_outer = true;
                    break;
                }

                count2 = gallopLeft(*cursor1, cursor2, len2, 0);
                if(count2 != 0) {
                    std::copy(cursor2, cursor2 + count2, dest);
                    dest    += count2;
                    cursor2 += count2;
                    len2    -= count2;
                    if(len2 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = *(cursor1++);
                if(--len1 == 1) {
                    break_outer = true;
                    break;
                }

                --minGallop;
            } while( (count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP) );
            if(break_outer) {
                break;
            }

            if(minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        minGallop_ = std::min(minGallop, 1);

        if(len1 == 1) {
            assert( len2 > 0 );
            std::copy(cursor2, cursor2 + len2, dest);
            *(dest + len2) = *cursor1;
        }
        else {
            assert( len1 != 0 && "Comparision function violates its general contract");
            assert( len2 == 0 );
            assert( len1 > 1 );
            std::copy(cursor1, cursor1 + len1, dest);
        }
    }

    void mergeHi(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        assert( len1 > 0 && len2 > 0 && base1 + len1 == base2 );

        copy_to_tmp(base2, len2);

        iter_t cursor1     = base1 + (len1 - 1);
        tmp_iter_t cursor2 = tmp_.begin() + (len2 - 1);
        iter_t dest        = base2 + (len2 - 1);

        *(dest--) = *(cursor1--);
        if(--len1 == 0) {
            std::copy(tmp_.begin(), tmp_.begin() + len2, dest - (len2 - 1));
            return;
        }
        if(len2 == 1) {
            dest    -= len1;
            cursor1 -= len1;
            std::copy_backward(cursor1 + 1, cursor1 + (1 + len1), dest + (1 + len1));
            *dest = *cursor2;
            return;
        }

        int minGallop( minGallop_ );

        // outer:
        while(true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert( len1 > 0 && len2 > 1 );

                if(comp_.lt(*cursor2, *cursor1)) {
                    *(dest--) = *(cursor1--);
                    ++count1;
                    count2 = 0;
                    if(--len1 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                else {
                    *(dest--) = *(cursor2--);
                    ++count2;
                    count1 = 0;
                    if(--len2 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while( (count1 | count2) < minGallop );
            if(break_outer) {
                break;
            }

            do {
                assert( len1 > 0 && len2 > 1 );

                count1 = len1 - gallopRight(*cursor2, base1, len1, len1 - 1);
                if(count1 != 0) {
                    dest    -= count1;
                    cursor1 -= count1;
                    len1    -= count1;
                    std::copy_backward(cursor1 + 1, cursor1 + (1 + count1), dest + (1 + count1));

                    if(len1 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = *(cursor2--);
                if(--len2 == 1) {
                    break_outer = true;
                    break;
                }

                count2 = len2 - gallopLeft(*cursor1, tmp_.begin(), len2, len2 - 1);
                if(count2 != 0) {
                    dest    -= count2;
                    cursor2 -= count2;
                    len2    -= count2;
                    std::copy(cursor2 + 1, cursor2 + (1 + count2), dest + 1);
                    if(len2 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = *(cursor1--);
                if(--len1 == 0) {
                    break_outer = true;
                    break;
                }

                minGallop--;
            } while( (count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP) );
            if(break_outer) {
                break;
            }

            if(minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        minGallop_ = std::min(minGallop, 1);

        if(len2 == 1) {
            assert( len1 > 0 );
            dest    -= len1;
            cursor1 -= len1;
            std::copy_backward(cursor1 + 1, cursor1 + (1 + len1), dest + (1 + len1));
            *dest = *cursor2;
        }
        else {
            assert( len2 != 0 && "Comparision function violates its general contract");
            assert( len1 == 0 );
            assert( len2 > 1 );
            std::copy(tmp_.begin(), tmp_.begin() + len2, dest - (len2 - 1));
        }
    }

    void copy_to_tmp(iter_t const begin, diff_t const len) {
        tmp_.clear();
        tmp_.reserve(len);
        std::copy(begin, begin + len, std::back_inserter(tmp_));
    }

    // the only interface is the friend timsort() function
    template <typename IterT, typename LessT>
    friend void timsort(IterT first, IterT last, LessT c);
};

template<typename RandomAccessIterator>
inline void timsort(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename std::iterator_traits<RandomAccessIterator>::value_type value_type;
    timsort(first, last, std::less<value_type>());
}

template<typename RandomAccessIterator, typename LessFunction>
inline void timsort(RandomAccessIterator first, RandomAccessIterator last, LessFunction compare) {
    TimSort<RandomAccessIterator, LessFunction>::sort(first, last, compare);
}

} // namespace TimSortNS

void TimSort(WSortView& A)
{
    TimSortNS::timsort(MyIterator(&A,0), MyIterator(&A,A.size()));
}

#undef GFX_TIMSORT_LOG
#undef GFX_TIMSORT_MOVE

// ****************************************************************************
