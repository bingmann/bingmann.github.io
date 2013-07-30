/***************************************************************************
 *  include/stxxl/bits/stream/sorted_runs.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002-2005 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2009, 2010 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_STREAM__SORTED_RUNS_H
#define STXXL_STREAM__SORTED_RUNS_H

#include <vector>
#include <stxxl/bits/mng/mng.h>
#include <stxxl/bits/mng/typed_block.h>
#include <stxxl/bits/algo/adaptor.h>


__STXXL_BEGIN_NAMESPACE

namespace stream
{
    //! \addtogroup streampack Stream package
    //! \{


    ////////////////////////////////////////////////////////////////////////
    //     SORTED RUNS                                                    //
    ////////////////////////////////////////////////////////////////////////

    //! \brief All sorted runs of a sort operation.
    template <typename TriggerEntryType, typename CompareType_>
    struct sorted_runs : private noncopyable
    {
        typedef TriggerEntryType trigger_entry_type;
        typedef typename trigger_entry_type::block_type block_type;
        typedef typename block_type::value_type value_type;  // may differ from trigger_entry_type::value_type
        typedef std::vector<trigger_entry_type> run_type;
        typedef std::vector<value_type> small_run_type;
        typedef stxxl::external_size_type size_type;
        typedef typename std::vector<run_type>::size_type run_index_type;

        typedef CompareType_    cmp_type;

        /// total number of elements in all runs
        size_type elements;

        /// vector of runs (containing vectors of block ids)
        std::vector<run_type> runs;

        /// vector of the number of elements in each individual run
        std::vector<size_type> runs_sizes;

        //! \brief Small sort optimization:
        // if the input is small such that its total size is at most B
        // (block_type::size) then input is sorted internally and kept in the
        // array "small_run"
        small_run_type  small_run;

        /// reference counter used by counting_ptr<>
        size_type       refs;

    public:
        sorted_runs()
            : elements(0), refs(0)
        { }

        ~sorted_runs()
        {
            deallocate_blocks();
        }
        
        /// Increment internal reference counter
        void inc_ref()
        {
            ++refs;
        }

        /// Decrement internal reference counter. If it hits zero, the object is freed.
        void dec_ref()
        {
            if (--refs == 0) delete this;
        }

        /// Clear the internal state of the object: release all runs and reset.
        void clear()
        {
            deallocate_blocks();

            elements = 0;
            runs.clear();
            runs_sizes.clear();
            small_run.clear();
        }

        /// Add a new run with given number of elements
        void add_run(const run_type& run, size_type run_size)
        {
            runs.push_back(run);
            runs_sizes.push_back(run_size);
            elements += run_size;
        }

        /// Swap contents with another object. This is used by the recursive
        /// merger to swap in a sorted_runs object with fewer runs.
        void swap(sorted_runs& b)
        {
            std::swap(elements, b.elements);
            std::swap(runs, b.runs);
            std::swap(runs_sizes, b.runs_sizes);
            std::swap(small_run, b.small_run);
        }

    private:
        //! \brief Deallocates the blocks which the runs occupy
        //!
        //! \remark There is no need in calling this method, the blocks are
        //! deallocated by the destructor. However, if you wish to reuse the
        //! object, then this function can be used to clear its state.
        void deallocate_blocks()
        {
            block_manager * bm = block_manager::get_instance();
            for (unsigned_type i = 0; i < runs.size(); ++i)
            {
                bm->delete_blocks(make_bid_iterator(runs[i].begin()),
                                  make_bid_iterator(runs[i].end()));
            }
        }
    };


//! \}
}

__STXXL_END_NAMESPACE

#endif // !STXXL_STREAM__SORTED_RUNS_H
// vim: et:ts=4:sw=4
