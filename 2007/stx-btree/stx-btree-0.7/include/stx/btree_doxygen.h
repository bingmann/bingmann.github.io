// $Id: btree_doxygen.h 38 2007-04-27 12:49:17Z tb $
/** \file btree_doxygen.h
 * Contains the doxygen comments. This header is not needed to compile the B+
 * tree.
 */

/*
 * STX B+ Tree Template Classes v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/** \mainpage STX B+ Tree Template Classes README

\author Timo Bingmann (Mail: tb a-with-circle idlebox dot net)
\date 2007-04-27

\section sec1 Summary

The STX B+ Tree package is a set of C++ template classes implementing a B+ tree
key/data container in main memory. The classes are designed as drop-in
replacements of the STL containers <tt>set</tt>, <tt>map</tt>,
<tt>multiset</tt> and <tt>multimap</tt> and follow their interfaces very
closely. By packing multiple value pairs into each node of the tree the B+ tree
reduces heap fragmentation and utilizes cache-line effects better than the
standard red-black binary tree. The tree algorithms are based on the
implementation in Cormen, Leiserson and Rivest's Introduction into Algorithms,
Jan Jannink's paper and other algorithm resources. The classes contain
extensive assertion and verification mechanisms to ensure the implementation's
correctness by testing the tree invariants.

\section sec2 Website / API Docs / Bugs / License

The current source package can be downloaded from
http://idlebox.net/2007/stx-btree/

The include files are extensively documented using doxygen. The compiled
doxygen html documentation can be found at 
http://idlebox.net/2007/stx-btree/stx-btree-0.7-doxygen/
(if you are not reading it right now).

If bugs should become known they will be posted on the above web page together
with patches or corrected versions.

The complete source code is released under the GNU Lesser General Public
License v2.1 (LGPL) which can be found in the file COPYING.

\section sec3 Original Application

The idea originally arose while coding a read-only database, which used a huge
map of millions of non-sequential integer keys to 8-byte file offsets. When
using the standard STL red-black tree implementation this would yield millions
of 20-byte heap allocations and very slow search times due to the tree's
height. So the original intension was to reduce memory fragmentation and
improve search times. The B+ tree solves this by packing multiple data pairs
into one node with a large number of descendant nodes.

In computer science lectures it is often stated that using consecutive bytes in
memory would be more cache-efficient, because the CPU's cache levels always
fetch larger blocks from main memory. So it would be best to store the keys of
a node in one continuous array. This way the inner scanning loop would be
accelerated by benefiting from cache effects and pipelining speed-ups. Thus the
cost of scanning for a matching key would be lower than in a red-black tree,
even though the number of key comparisons are theoretically larger. This second
aspect aroused my academic interest and resulted in the
\ref speedtest "speed test experiments".

A third inspiration was that no working C++ template implementation of a B+
tree could be found on the Internet. Now this one can be found.

\section sec4 Implementation Overview

This implementation contains five main classes within the \ref stx namespace
(blandly named Some Template eXtensions). The base class \ref stx::btree
"btree" implements the B+ tree algorithms using inner and leaf nodes in main
memory. Almost all STL-required function calls are implemented (see below for
the exceptions). The asymptotic time requirements of the STL standard are
theoretically not always fulfilled. However in practice this B+ tree performs
better than the STL's red-black tree at the cost of using more memory. See the
\ref speedtest "speed test results" for details.

The base class is then specialized into \ref stx::btree_set "btree_set", \ref
stx::btree_multiset "btree_multiset", \ref stx::btree_map "btree_map" and \ref
stx::btree_multimap "btree_multimap" using default template parameters and
facade functions. These classes are designed to be drop-in replacements for the
corresponding STL containers.

The insertion function splits the nodes on recursion unroll. Erase is largely
based on Jannink's ideas. See http://dbpubs.stanford.edu:8090/pub/1995-19 for
his paper on "Implementing Deletion in B+-trees".

The two set classes are derived from the base implementation class btree by
specifying an empty struct as data_type. All functions are adapted to provide
the base class with empty placeholder objects. Note that it is somewhat
inefficient to implement a set or multiset using a B+ tree: a plain B tree
would hold no extra copies of the keys. The main focus was on implementing the
maps.

\section sec5 Problem with Separated Key/Data Arrays

The most noteworthy difference to the default red-black tree implementation of
std::map is that the B+ tree does not hold key/data pairs together in memory.
Instead each B+ tree node has two separate arrays containing keys and data
values. This design was chosen to utilize cache-line effects while scanning the
key array.

However it also directly generates many problems in implementing the iterators'
operators. These return a (writable) reference or pointer to a value_type,
which is a std::pair composition. These data/key pairs however are not stored
together and thus a temporary copy must be constructed. This copy should not be
written to, because it is not stored back into the B+ tree. This effectively
prohibits use of many STL algorithms which writing to the B+ tree's
iterators. I would be grateful for hints on how to resolve this problem without
folding the key and data arrays.

\section sec6 Test Suite

The B+ tree distribution contains an extensive test suite using
cppunit. According to gcov 89.23% of the btree.h implementation is covered.

\section sec7 STL Incompatibilities

\subsection sec7-1 Key and Data Type Requirements

The tree algorithms currently do not use copy-construction. All key/data items
are allocated in the nodes using the default-constructor and are subsequently
only assigned new data (using <tt>operator=</tt>).

\subsection sec7-2 Iterators' Operators

The most important incompatibility are the non-writable <tt>operator*</tt> and
<tt>operator-></tt> of the \ref stx::btree::iterator "iterator". See above for
a discussion of the problem on separated key/data arrays.  Instead of
<tt>*iter</tt> and <tt>iter-></tt> use the new function <tt>iter.data()</tt>
which returns a writable reference to the data value in the tree.

\subsection sec7-3 Erase Functions

The B+ tree supports only two erase functions:

\code
size_type erase(const key_type &key); // erase all data pairs matching key
bool erase_one(const key_type &key);  // erase one data pair matching key
\endcode

The following STL-required functions are not supported:

\code
void erase(iterator iter);
void erase(iterator first, iterator last);
\endcode

\section sec8 Extensions

Beyond the usual STL interface the B+ tree classes support some extra goodies.

\code
// Output the tree in a pseudo-hierarchical text dump to std::cout. This
// function requires that BTREE_DEBUG is defined prior to including the btree
// headers. Furthermore the key and data types must be std::ostream printable.
void print() const;

// Run extensive checks of the tree invariants. If a corruption in found the
// program will abort via assert(). See below on enabling auto-verification.
void verify() const;

// Serialize and restore the B+ tree nodes and data into/from a binary image.
// This requires that the key and data types are integral and contain no
// outside pointers or references.
void dump(std::ostream &os) const;
bool restore(std::istream &is);
\endcode

\section sec9 B+ Tree Traits

All tree template classes take a template parameter structure which holds
important options of the implementation. The following structure shows which
static variables specify the options and the corresponding defaults:

\code
struct btree_default_map_traits
{
    // If true, the tree will self verify it's invariants after each insert()
    // or erase(). The header must have been compiled with BTREE_DEBUG
    // defined.
    static const bool	selfverify = false;

    // If true, the tree will print out debug information and a tree dump
    // during insert() or erase() operation. The header must have been
    // compiled with BTREE_DEBUG defined and key_type must be std::ostream
    // printable.
    static const bool	debug = false;

    // Number of slots in each leaf of the tree. Estimated so that each node
    // has a size of about 256 bytes.
    static const int 	leafslots =
                             MAX( 8, 256 / (sizeof(_Key) + sizeof(_Data)) );

    // Number of slots in each inner node of the tree. Estimated so that each
    // node has a size of about 256 bytes.
    static const int	innerslots =
                             MAX( 8, 256 / (sizeof(_Key) + sizeof(void*)) );
};
\endcode

\section sec10 Speed Tests

See the extra page \ref speedtest "Speed Test Results".

*/

/** \page speedtest Speed Test Results

\section Experiment

The speed test compares the libstdc++ STL red-black tree with the implemented
B+ tree with many different parameters. To keep focus on the algorithms and
reduce data copying the multiset specializations were chosen. Two set of test
procedures are used: the first only inserts \a n random integers into the
tree. The second test first inserts \a n random integers, then performs \a n
lookups for those integers and finally erases all \a n integers.

These two test sequences are preformed for \a n from 125 to 4,096,000 where \a
n is doubled after each test run. For each \a n the test cycles are run until
in total 8,192,000 items were inserted. This way the measured speed for small
\a n is averaged over up to 65,536 sample runs.

Lastly it is purpose of the test to determine a good node size for the B+
tree. Therefore the test runs are performed on different slot sizes; both inner
and leaf nodes hold the same number of items. The number of slots tested ranges
from 4 to 256 slots and therefore yields node sizes from about 50 to 2,048
bytes. This requires that the B+ tree template is instantiated for each of the
probed node sizes!

\attention Compilation of the speed test with -O3 can take very long and
required much RAM.

The results are be displayed below using gnuplot. All tests were run on a
Pentium4 3.2 GHz with 1 GB RAM. A high-resolution PDF plot of the following
images can be found in the package at speedtest/speedtest.pdf

\image html speedtest-plot-000001.png
\image html speedtest-plot-000002.png

The first two plots above show the absolute time measured for inserting \a n
items into six different tree variants. For small \a n (the first plot) the
speed of red-black tree and B+ tree are very similar. For large \a n the
red-black tree slows down, and for \a n > 1,024,000 items the red-black tree
requires almost twice as much time as a B+ tree with 32 slots.

The next plot shows the insertion time per item, which is calculated by
dividing the absolute time by the number of inserted items. Notice that
insertion time is now in microseconds. The plot shows that the red-black tree
reaches some limitation at about \a n = 16,000 items. Beyond this item count
the B+ tree (with 32 slots) performs much better than the STL multiset.

\image html speedtest-plot-000003.png

\image html speedtest-plot-000004.png

The last plots goal is to find the best node size for the B+ tree. It displays
the total measured time of the insertion test depending on the number of slots
in inner and leaf nodes. The first data point on the left is the running time
of the red-black tree. Only runs with more than 1 million inserted items are
plotted. One can see that the minimum is around 65 slots for each of the
curves. However to reduce unused memory in the nodes the most practical slot
size is around 35. This amounts to total node sizes of about 280 bytes. Thus in
the implementation a target size of 256 bytes was chosen.

The following four plots show the same aspects as above, except that not only
insertion time was measured. Instead a whole insert/find/delete cycle was
performed and measured. The results are in general accordance to those of only
insertion.

\image html speedtest-plot-000005.png
\image html speedtest-plot-000006.png
\image html speedtest-plot-000007.png
\image html speedtest-plot-000008.png

*/
