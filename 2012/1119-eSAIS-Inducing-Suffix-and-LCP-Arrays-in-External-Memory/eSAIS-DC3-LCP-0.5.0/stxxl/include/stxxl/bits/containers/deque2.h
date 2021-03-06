/***************************************************************************
 *  include/stxxl/bits/containers/deque2.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2012 Timo Bingmann <bingmann@kit.edu>
 *  based on include/stxxl/bits/containers/queue.h
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_DEQUE2_HEADER
#define STXXL_DEQUE2_HEADER

#include <deque>

#include <stxxl/bits/deprecated.h>
#include <stxxl/bits/mng/mng.h>
#include <stxxl/bits/mng/typed_block.h>
#include <stxxl/bits/common/tmeta.h>
#include <stxxl/bits/mng/read_write_pool.h>
#include <stxxl/bits/mng/write_pool.h>
#include <stxxl/bits/mng/prefetch_pool.h>


__STXXL_BEGIN_NAMESPACE

#ifndef STXXL_VERBOSE_DEQUE2
#define STXXL_VERBOSE_DEQUE2 STXXL_VERBOSE2
#endif

//! \addtogroup stlcont
//! \{

//! \brief External deque container without random access

//! \tparam ValueType type of the contained objects (POD with no references to internal memory)
//! \tparam BlockSize size of the external memory block in bytes, default is \c STXXL_DEFAULT_BLOCK_SIZE(ValTp)
//! \tparam AllocStrategy parallel disk allocation strategy, default is \c STXXL_DEFAULT_ALLOC_STRATEGY
//! \tparam SizeType size data type, default is \c stxxl::uint64
template <class ValueType,
          unsigned BlockSize = STXXL_DEFAULT_BLOCK_SIZE(ValueType),
          class AllocStrategy = STXXL_DEFAULT_ALLOC_STRATEGY,
          class SizeType = stxxl::uint64>
class deque2 : private noncopyable
{
public:
    typedef ValueType value_type;
    typedef AllocStrategy alloc_strategy_type;
    typedef SizeType size_type;
    enum {
        block_size = BlockSize
    };

    typedef typed_block<block_size, value_type> block_type;
    typedef BID<block_size> bid_type;

    typedef std::deque<bid_type> bid_deque_type;

private:
    typedef read_write_pool<block_type> pool_type;

    /// current number of items in the deque
    size_type m_size;

    /// whether the m_pool object is own and should be deleted.
    bool m_owns_pool;

    /// read_write_pool of blocks
    pool_type* m_pool;

    /// current front block of deque
    block_type* m_front_block;

    /// current back block of deque
    block_type* m_back_block;

    /// pointer to current front element in m_front_block
    value_type * m_front_element;

    /// pointer to current back element in m_back_block
    value_type * m_back_element;

    /// block allocation strategy
    alloc_strategy_type m_alloc_strategy;

    /// block allocation counter
    unsigned_type m_alloc_count;

    /// allocated block identifiers
    bid_deque_type m_bids;

    /// block manager used
    block_manager* m_bm;

    /// number of blocks to prefetch
    unsigned_type m_blocks2prefetch;

public:
    //! \brief Constructs empty deque with own write and prefetch block pool

    //! \param D  number of parallel disks, defaulting to the configured number of scratch disks,
    //!           memory consumption will be 2 * D + 2 blocks 
    //!           (first and last block, D blocks as write cache, D block for prefetching)
    explicit deque2(int D = -1) :
        m_size(0),
        m_owns_pool(true),
        m_alloc_count(0),
        m_bm(block_manager::get_instance())
    {
        if (D < 1) D = config::get_instance()->disks_number();
        STXXL_VERBOSE_DEQUE2("deque2[" << this << "]::deque2(D)");
        m_pool = new pool_type(D, D + 2);
        init();
    }

    //! \brief Constructs empty deque2 with own write and prefetch block pool

    //! \param w_pool_size  number of blocks in the write pool, must be at least 2, recommended at least 3
    //! \param p_pool_size  number of blocks in the prefetch pool, recommended at least 1
    //! \param blocks2prefetch  defines the number of blocks to prefetch (\c front side),
    //!                          default is number of block in the prefetch pool
    explicit deque2(unsigned_type w_pool_size, unsigned_type p_pool_size, int blocks2prefetch = -1) :
        m_size(0),
        m_owns_pool(true),
        m_alloc_count(0),
        m_bm(block_manager::get_instance())
    {
        STXXL_VERBOSE_DEQUE2("deque2[" << this << "]::deque2(sizes)");
        m_pool = new pool_type(p_pool_size, w_pool_size);
        init(blocks2prefetch);
    }

    //! \brief Constructs empty deque

    //! \param pool_ block write/prefetch pool
    //! \param blocks2prefetch  defines the number of blocks to prefetch (\c front side),
    //!                          default is number of blocks in the prefetch pool
    //!  \warning Number of blocks in the write pool must be at least 2, recommended at least 3
    //!  \warning Number of blocks in the prefetch pool recommended at least 1
    deque2(pool_type & pool, int blocks2prefetch = -1) :
        m_size(0),
        m_owns_pool(false),
        m_pool(&pool),
        m_alloc_count(0),
        m_bm(block_manager::get_instance())
    {
        STXXL_VERBOSE_DEQUE2("deque2[" << this << "]::deque2(pool)");
        init(blocks2prefetch);
    }

    void swap(deque2& obj)
    {
        std::swap(m_size, obj.m_size);
        std::swap(m_owns_pool, obj.m_owns_pool);
        std::swap(m_pool, obj.m_pool);
        std::swap(m_front_block, obj.m_front_block);
        std::swap(m_back_block, obj.m_back_block);
        std::swap(m_front_element, obj.m_front_element);
        std::swap(m_back_element, obj.m_back_element);
        std::swap(m_alloc_strategy, obj.m_alloc_strategy);
        std::swap(m_alloc_count, obj.m_alloc_count);
        std::swap(m_bids, obj.m_bids);
        std::swap(m_bm, obj.m_bm);
        std::swap(m_blocks2prefetch, obj.m_blocks2prefetch);
    }

private:
    void init(int blocks2prefetch = -1)
    {
        if (m_pool->size_write() < 2) {
            STXXL_ERRMSG("deque2: invalid configuration, not enough blocks (" << m_pool->size_write() <<
                         ") in write pool, at least 2 are needed, resizing to 3");
            m_pool->resize_write(3);
        }

        if (m_pool->size_write() < 3) {
            STXXL_MSG("deque2: inefficient configuration, no blocks for buffered writing available");
        }

        if (m_pool->size_prefetch() < 1) {
            STXXL_MSG("deque2: inefficient configuration, no blocks for prefetching available");
        }

        /// initialize empty deque
        m_front_block = m_back_block = m_pool->steal();
        m_back_element = m_back_block->begin() - 1;
        m_front_element = m_back_block->begin();
        set_prefetch_aggr(blocks2prefetch);
    }

public:
    //! \brief Defines the number of blocks to prefetch (\c front side)
    //!        This method should be called whenever the prefetch pool is resized
    //! \param blocks2prefetch  defines the number of blocks to prefetch (\c front side),
    //!                         a negative value means to use the number of blocks in the prefetch pool
    void set_prefetch_aggr(int_type blocks2prefetch)
    {
        if (blocks2prefetch < 0)
            m_blocks2prefetch = m_pool->size_prefetch();
        else
            m_blocks2prefetch = blocks2prefetch;
    }

    //! \brief Returns the number of blocks prefetched from the \c front side
    unsigned_type get_prefetch_aggr() const
    {
        return m_blocks2prefetch;
    }

    //! \brief Adds an element to the end of the deque
    void push_back(const value_type & val)
    {
        if (UNLIKELY(m_back_element == m_back_block->begin() + (block_type::size - 1)))
        {
            // back block is filled
            if (m_front_block == m_back_block)
            {
                // can not write the back block because it
                // is the same as the front block, must keep it memory
                STXXL_VERBOSE1("deque2::push Case 1");
            }
            else if (size() < 2 * block_type::size)
            {
                STXXL_VERBOSE1("deque2::push Case 1.5");
                // only two blocks with a gap in the beginning, move elements within memory
                assert(m_bids.empty());
                size_t gap = m_front_element - m_front_block->begin();
                assert(gap > 0);
                std::copy(m_front_element, m_front_block->end(), m_front_block->begin());
                std::copy(m_back_block->begin(), m_back_block->begin() + gap, m_front_block->begin() + (block_type::size - gap));
                std::copy(m_back_block->begin() + gap, m_back_block->end(), m_back_block->begin());
                m_front_element -= gap;
                m_back_element -= gap;

                ++m_back_element;
                *m_back_element = val;
                ++m_size;
                return;
            }
            else
            {
                STXXL_VERBOSE1("deque2::push Case 2");
                // write the back block
                // need to allocate new block
                bid_type newbid;

                m_bm->new_block(m_alloc_strategy, newbid, m_alloc_count++);

                STXXL_VERBOSE_DEQUE2("deque2[" << this << "]: push block " << m_back_block << " @ " << FMT_BID(newbid));
                m_bids.push_back(newbid);
                m_pool->write(m_back_block, newbid);
                if (m_bids.size() <= m_blocks2prefetch) {
                    STXXL_VERBOSE1("deque2::push Case Hints");
                    m_pool->hint(newbid);
                }
            }
            m_back_block = m_pool->steal();

            m_back_element = m_back_block->begin();
            *m_back_element = val;
            ++m_size;
            return;
        }
        ++m_back_element;
        *m_back_element = val;
        ++m_size;
    }

    //! \brief Removes element from the front of the deque
    void pop_front()
    {
        assert(!empty());

        if (UNLIKELY(m_front_element == m_front_block->begin() + (block_type::size - 1)))
        {
            // if there is only one block, it implies ...
            if (m_back_block == m_front_block)
            {
                STXXL_VERBOSE1("deque2::pop Case 3");
                assert(size() == 1);
                assert(m_back_element == m_front_element);
                assert(m_bids.empty());
                // reset everything
                m_back_element = m_back_block->begin() - 1;
                m_front_element = m_back_block->begin();
                m_size = 0;
                return;
            }

            --m_size;
            if (m_size <= block_type::size)
            {
                STXXL_VERBOSE1("deque2::pop Case 4");
                assert(m_bids.empty());
                // the m_back_block is the next block
                m_pool->add(m_front_block);
                m_front_block = m_back_block;
                m_front_element = m_back_block->begin();
                return;
            }
            STXXL_VERBOSE1("deque2::pop Case 5");

            assert(!m_bids.empty());
            request_ptr req = m_pool->read(m_front_block, m_bids.front());
            STXXL_VERBOSE_DEQUE2("deque2[" << this << "]: pop block  " << m_front_block << " @ " << FMT_BID(m_bids.front()));

            // give prefetching hints
            for (unsigned_type i = 0; i < m_blocks2prefetch && i < m_bids.size() - 1; ++i)
            {
                STXXL_VERBOSE1("deque2::pop Case Hints");
                m_pool->hint(m_bids[i + 1]);
            }

            m_front_element = m_front_block->begin();
            req->wait();

            m_bm->delete_block(m_bids.front());
            m_bids.pop_front();
            return;
        }

        ++m_front_element;
        --m_size;
    }

    //! \brief Returns the size of the deque
    size_type size() const
    {
        return m_size;
    }

    //! \brief Returns \c true if deque is empty
    bool empty() const
    {
        return (m_size == 0);
    }

    //! \brief Returns a mutable reference at the back of the deque
    value_type & back()
    {
        assert(!empty());
        return *m_back_element;
    }

    //! \brief Returns a const reference at the back of the deque
    const value_type & back() const
    {
        assert(!empty());
        return *m_back_element;
    }

    //! \brief Returns a mutable reference at the front of the deque
    value_type & front()
    {
        assert(!empty());
        return *m_front_element;
    }

    //! \brief Returns a const reference at the front of the deque
    const value_type & front() const
    {
        assert(!empty());
        return *m_front_element;
    }

    ~deque2()
    {
        if (m_front_block != m_back_block)
            m_pool->add(m_back_block);

        m_pool->add(m_front_block);

        if (m_owns_pool)
            delete m_pool;

        if (!m_bids.empty())
            m_bm->delete_blocks(m_bids.begin(), m_bids.end());
    }

    /********************************************************************************/

    class stream
    {
    public:

        typedef typename deque2::value_type value_type;

        typedef typename bid_deque_type::const_iterator bid_iter_type;

    protected:

        const deque2&       m_deque;

        size_type           m_size;

        value_type*         m_current_element;

        block_type*         m_current_block;

        bid_iter_type       m_next_bid;

    public:

        stream(const deque2& deque2)
            : m_deque(deque2),
              m_size(deque2.size())
        {
            m_current_block = deque2.m_front_block;
            m_current_element = deque2.m_front_element;
            m_next_bid = deque2.m_bids.begin();
        }

        ~stream()
        {
            if ( m_current_block != m_deque.m_front_block &&
                 m_current_block != m_deque.m_back_block ) // give m_current_block back to pool
                m_deque.m_pool->add(m_current_block);
        }

        size_type size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return (m_size == 0);
        }

        const value_type& operator*() const
        {
            assert(!empty());
            return *m_current_element;
        }

        stream& operator++()
        {
            assert(!empty());

            if (UNLIKELY(m_current_element == m_current_block->begin() + (block_type::size - 1)))
            {
                // next item position is beyond end of current block, find next block
                --m_size;

                if (m_size == 0)
                {
                    STXXL_VERBOSE1("deque2::stream::operator++ last block finished clean at block end");
                    assert( m_next_bid == m_deque.m_bids.end() );
                    assert( m_current_block == m_deque.m_back_block );
                    // nothing to give back to deque pool
                    m_current_element = NULL;
                    return *this;
                }
                else if (m_size <= block_type::size)    // still items left in last partial block
                {
                    STXXL_VERBOSE1("deque2::stream::operator++ reached last block");
                    assert( m_next_bid == m_deque.m_bids.end() );
                    // the m_back_block is the next block
                    if (m_current_block != m_deque.m_front_block) // give current_block back to pool
                        m_deque.m_pool->add(m_current_block);
                    m_current_block = m_deque.m_back_block;
                    m_current_element = m_current_block->begin();
                    return *this;
                }
                else if (m_current_block == m_deque.m_front_block)
                {
                    STXXL_VERBOSE1("deque2::stream::operator++ first own-block case: steal block from deque's pool");
                    m_current_block = m_deque.m_pool->steal();
                }

                STXXL_VERBOSE1("deque2::stream::operator++ default case: fetch next block");

                assert( m_next_bid != m_deque.m_bids.end() );
                request_ptr req = m_deque.m_pool->read(m_current_block, *m_next_bid);
                STXXL_VERBOSE_DEQUE2("deque2[" << this << "]::stream::operator++ read block " << m_current_block << " @ " << FMT_BID(*m_next_bid));

                // give prefetching hints
                bid_iter_type bid = m_next_bid+1;
                for (unsigned_type i = 0; i < m_deque.m_blocks2prefetch && bid != m_deque.m_bids.end(); ++i, ++bid)
                {
                    STXXL_VERBOSE1("deque2::stream::operator++ giving prefetch hints");
                    m_deque.m_pool->hint(*bid);
                }

                m_current_element = m_current_block->begin();
                req->wait();

                ++m_next_bid;
            }
            else
            {
                --m_size;
                ++m_current_element;
            }
            return *this;
        }
    };

    stream get_stream()
    {
        return stream(*this);
    }

    /********************************************************************************/

    class reverse_stream
    {
    public:

        typedef typename deque2::value_type value_type;

        typedef typename bid_deque_type::const_reverse_iterator bid_iter_type;

    protected:

        const deque2&       m_deque;

        size_type           m_size;

        value_type*         m_current_element;

        block_type*         m_current_block;

        bid_iter_type       m_next_bid;

    public:

        reverse_stream(const deque2& deque2)
            : m_deque(deque2),
              m_size(deque2.size())
        {
            m_current_block = deque2.m_back_block;
            m_current_element = deque2.m_back_element;
            m_next_bid = deque2.m_bids.rbegin();
        }

        ~reverse_stream()
        {
            if ( m_current_block != m_deque.m_front_block &&
                 m_current_block != m_deque.m_back_block ) // give m_current_block back to pool
                m_deque.m_pool->add(m_current_block);
        }

        size_type size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return (m_size == 0);
        }

        const value_type& operator*() const
        {
            assert(!empty());
            return *m_current_element;
        }

        reverse_stream& operator++()
        {
            assert(!empty());

            if (UNLIKELY(m_current_element == m_current_block->begin()))
            {
                // next item position is beyond begin of current block, find next block
                --m_size;

                if (m_size == 0)
                {
                    STXXL_VERBOSE1("deque2::reverse_stream::operator++ last block finished clean at block begin");
                    assert( m_next_bid == m_deque.m_bids.rend() );
                    assert( m_current_block == m_deque.m_front_block );
                    // nothing to give back to deque pool
                    m_current_element = NULL;
                    return *this;
                }
                else if (m_size <= block_type::size)
                {
                    STXXL_VERBOSE1("deque2::reverse_stream::operator++ reached first block");
                    assert( m_next_bid == m_deque.m_bids.rend() );
                    // the m_back_block is the next block
                    if (m_current_block != m_deque.m_back_block) // give current_block back to pool
                        m_deque.m_pool->add(m_current_block);
                    m_current_block = m_deque.m_front_block;
                    m_current_element = m_current_block->begin() + (block_type::size - 1);
                    return *this;
                }
                else if (m_current_block == m_deque.m_back_block)
                {
                    STXXL_VERBOSE1("deque2::reverse_stream::operator++ first own-block case: steal block from deque's pool");
                    m_current_block = m_deque.m_pool->steal();
                }

                STXXL_VERBOSE1("deque2::reverse_stream::operator++ default case: fetch previous block");

                assert( m_next_bid != m_deque.m_bids.rend() );
                request_ptr req = m_deque.m_pool->read(m_current_block, *m_next_bid);
                STXXL_VERBOSE_DEQUE2("deque2[" << this << "]::reverse_stream::operator++ read block " << m_current_block << " @ " << FMT_BID(*m_next_bid));

                // give prefetching hints
                bid_iter_type bid = m_next_bid+1;
                for (unsigned_type i = 0; i < m_deque.m_blocks2prefetch && bid != m_deque.m_bids.rend(); ++i, ++bid)
                {
                    STXXL_VERBOSE1("deque2::reverse_stream::operator++ giving prefetch hints");
                    m_deque.m_pool->hint(*bid);
                }

                m_current_element = m_current_block->begin() + (block_type::size - 1);
                req->wait();

                ++m_next_bid;
            }
            else
            {
                --m_size;
                --m_current_element;
            }
            return *this;
        }
    };

    reverse_stream get_reverse_stream()
    {
        return reverse_stream(*this);
    }
};

//! \}

__STXXL_END_NAMESPACE

#endif // !STXXL_DEQUE2_HEADER
// vim: et:ts=4:sw=4
