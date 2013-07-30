/***************************************************************************
 *  include/stxxl/bits/mng/block_alloc.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002-2007 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2007-2009 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_MNG__BLOCK_ALLOC_H
#define STXXL_MNG__BLOCK_ALLOC_H

#include <algorithm>
#include <stxxl/bits/parallel.h>
#include <stxxl/bits/common/rand.h>
#include <stxxl/bits/mng/config.h>


__STXXL_BEGIN_NAMESPACE

//! \ingroup mnglayer

//! \weakgroup alloc Allocation functors
//! Standard allocation strategies encapsulated in functors
//! \{

//! \brief example disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct basic_allocation_strategy
{
    basic_allocation_strategy(int disks_begin, int disks_end);
    basic_allocation_strategy();
    int operator () (int i) const;
    static const char * name();
};

//! \brief striping disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct striping
{
    int begin, diff;

public:
    striping(int b, int e) : begin(b), diff(e - b)
    { }

    striping() : begin(0)
    {
        diff = config::get_instance()->disks_number();
    }

    int operator () (int i) const
    {
        return begin + i % diff;
    }

    static const char * name()
    {
        return "striping";
    }
};

//! \brief fully randomized disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct FR : public striping
{
private:
    random_number<random_uniform_fast> rnd;

public:
    FR(int b, int e) : striping(b, e)
    { }

    FR() : striping()
    { }

    int operator () (int /*i*/) const
    {
        return begin + rnd(diff);
    }

    static const char * name()
    {
        return "fully randomized striping";
    }
};

//! \brief simple randomized disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct SR : public striping
{
private:
    int offset;

    void init()
    {
        random_number<random_uniform_fast> rnd;
        offset = rnd(diff);
    }

public:
    SR(int b, int e) : striping(b, e)
    {
        init();
    }

    SR() : striping()
    {
        init();
    }

    int operator () (int i) const
    {
        return begin + (i + offset) % diff;
    }

    static const char * name()
    {
        return "simple randomized striping";
    }
};

//! \brief randomized cycling disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct RC : public striping
{
private:
    std::vector<int> perm;

    void init()
    {
        for (int i = 0; i < diff; i++)
            perm[i] = i;

        stxxl::random_number<random_uniform_fast> rnd;
        std::random_shuffle(perm.begin(), perm.end(), rnd _STXXL_FORCE_SEQUENTIAL);
    }

public:
    RC(int b, int e) : striping(b, e), perm(diff)
    {
        init();
    }

    RC() : striping(), perm(diff)
    {
        init();
    }

    int operator () (int i) const
    {
        return begin + perm[i % diff];
    }

    static const char * name()
    {
        return "randomized cycling striping";
    }
};

struct RC_disk : public RC
{
    RC_disk(int b, int e) : RC(b, e)
    { }

    RC_disk() : RC(config::get_instance()->regular_disk_range().first, config::get_instance()->regular_disk_range().second)
    { }

    static const char * name()
    {
        return "Randomized cycling striping on regular disks";
    }
};

struct RC_flash : public RC
{
    RC_flash(int b, int e) : RC(b, e)
    { }

    RC_flash() : RC(config::get_instance()->flash_range().first, config::get_instance()->flash_range().second)
    { }

    static const char * name()
    {
        return "Randomized cycling striping on flash devices";
    }
};

//! \brief 'single disk' disk allocation scheme functor
//! \remarks model of \b allocation_strategy concept
struct single_disk
{
    const int disk;
    single_disk(int d, int = 0) : disk(d)
    { }

    single_disk() : disk(0)
    { }

    int operator () (int /*i*/) const
    {
        return disk;
    }

    static const char * name()
    {
        return "single disk";
    }
};

//! \brief Allocator functor adaptor

//! Gives offset to disk number sequence defined in constructor
template <class BaseAllocator_>
struct offset_allocator
{
    BaseAllocator_ base;
    int_type offset;

    //! \brief Creates functor based on instance of \c BaseAllocator_ functor
    //! with offset \c offset_
    //! \param offset_ offset
    //! \param base_ used to create a copy
    offset_allocator(int_type offset_, const BaseAllocator_ & base_) : base(base_), offset(offset_)
    { }

    //! \brief Creates functor based on instance of \c BaseAllocator_ functor
    //! \param base_ used to create a copy
    offset_allocator(const BaseAllocator_ & base_) : base(base_), offset(0)
    { }

    //! \brief Creates functor based on default \c BaseAllocator_ functor
    offset_allocator() : offset(0)
    { }

    int operator () (int_type i) const
    {
        return base(offset + i);
    }

    int_type get_offset() const
    {
        return offset;
    }

    void set_offset(int_type i)
    {
        offset = i;
    }
};

#ifndef STXXL_DEFAULT_ALLOC_STRATEGY
    #define STXXL_DEFAULT_ALLOC_STRATEGY stxxl::RC
#endif

//! \}

__STXXL_END_NAMESPACE

#endif // !STXXL_MNG__BLOCK_ALLOC_H
// vim: et:ts=4:sw=4
