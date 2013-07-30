/***************************************************************************
 *  containers/test_stack.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2003 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2010 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

//! \example containers/test_stack.cpp
//! This is an example of how to use \c stxxl::STACK_GENERATOR class
//! to generate an \b external stack type
//! with \c stxxl::grow_shrink_stack implementation, \b four blocks per page,
//! block size \b 4096 bytes

#include <stxxl/stack>


template <typename stack_type>
void test_lvalue_correctness(stack_type & stack, int a, int b)
{
    int i;
    assert(stack.empty());
    for (i = 0; i < a; ++i)
        stack.push(i);
    for (i = 0; i < b; ++i)
        stack.push(i);
    for (i = 0; i < b; ++i)
        stack.pop();
    stack.top() = 0xbeeff00d;
    for (i = 0; i < b; ++i)
        stack.push(i);
    for (i = 0; i < b; ++i)
        stack.pop();
    if ((stack.top() != int(0xbeeff00d))) {
        STXXL_ERRMSG("STACK MISMATCH AFTER top() LVALUE MODIFICATION (0x" << std::hex << stack.top() << " != 0xbeeff00d)");
        assert(stack.top() == int(0xbeeff00d));
    }
    for (i = 0; i < a; ++i)
        stack.pop();
}


template <typename stack_type>
void simple_test(stack_type & my_stack, int test_size)
{
    int i;

    for (i = 0; i < test_size; i++)
    {
        my_stack.push(i);
        assert(my_stack.top() == i);
        assert(my_stack.size() == i + 1);
    }

    for (i = test_size - 1; i >= 0; i--)
    {
        assert(my_stack.top() == i);
        my_stack.pop();
        assert(my_stack.size() == i);
    }

    for (i = 0; i < test_size; i++)
    {
        my_stack.push(i);
        assert(my_stack.top() == i);
        assert(my_stack.size() == i + 1);
    }

    // test swap
    stack_type s2;
    std::swap(s2, my_stack);
    std::swap(s2, my_stack);

    for (i = test_size - 1; i >= 0; i--)
    {
        assert(my_stack.top() == i);
        my_stack.pop();
        assert(my_stack.size() == i);
    }

    std::stack<int> int_stack;

    for (i = 0; i < test_size; i++)
    {
        int_stack.push(i);
        assert(int_stack.top() == i);
        assert(int(int_stack.size()) == i + 1);
    }

    stack_type my_stack1(int_stack);

    for (i = test_size - 1; i >= 0; i--)
    {
        assert(my_stack1.top() == i);
        my_stack1.pop();
        assert(my_stack1.size() == i);
    }

    STXXL_MSG("Test 1 passed.");

    test_lvalue_correctness(my_stack, 4 * 4096 / 4 * 2, 4 * 4096 / 4 * 2 * 20);
}

int main(int argc, char * argv[])
{
    typedef stxxl::STACK_GENERATOR<int, stxxl::external, stxxl::normal, 4, 4096>::result ext_normal_stack_type;
    typedef stxxl::STACK_GENERATOR<int, stxxl::migrating, stxxl::normal, 4, 4096>::result ext_migrating_stack_type;
    typedef stxxl::STACK_GENERATOR<int, stxxl::external, stxxl::grow_shrink, 4, 4096>::result ext_stack_type;
    typedef stxxl::STACK_GENERATOR<int, stxxl::external, stxxl::grow_shrink2, 1, 4096>::result ext_stack_type2;

    if (argc < 2)
    {
        STXXL_MSG("Usage: " << argv[0] << " test_size_in_pages");
        return -1;
    }
    {
        ext_normal_stack_type my_stack;
        simple_test(my_stack, atoi(argv[1]) * 4 * 4096 / sizeof(int));
    }
    {
        ext_migrating_stack_type my_stack;
        //simple_test(my_stack, atoi(argv[1]) * 4 * 4096 / sizeof(int));
    }
    {
        ext_stack_type my_stack;
        simple_test(my_stack, atoi(argv[1]) * 4 * 4096 / sizeof(int));
    }
    {
        // prefetch/write pool with 10 blocks prefetching and 10 block write cache (> D is recommended)
        stxxl::read_write_pool<ext_stack_type2::block_type> pool(10, 10);
        // create a stack that does not prefetch (level of prefetch aggressiveness 0)
        ext_stack_type2 my_stack(pool, 0);
        int test_size = atoi(argv[1]) * 4 * 4096 / sizeof(int), i;

        for (i = 0; i < test_size; i++)
        {
            my_stack.push(i);
            assert(my_stack.top() == i);
            assert(my_stack.size() == i + 1);
        }
        my_stack.set_prefetch_aggr(10);
        for (i = test_size - 1; i >= 0; i--)
        {
            assert(my_stack.top() == i);
            my_stack.pop();
            assert(my_stack.size() == i);
        }

        for (i = 0; i < test_size; i++)
        {
            my_stack.push(i);
            assert(my_stack.top() == i);
            assert(my_stack.size() == i + 1);
        }

        // test swap
        ext_stack_type2 s2(pool, 0);
        std::swap(s2, my_stack);
        std::swap(s2, my_stack);

        for (i = test_size - 1; i >= 0; i--)
        {
            assert(my_stack.top() == i);
            my_stack.pop();
            assert(my_stack.size() == i);
        }

        STXXL_MSG("Test 2 passed.");

        test_lvalue_correctness(my_stack, 4 * 4096 / 4 * 2, 4 * 4096 / 4 * 2 * 20);
    }

    return 0;
}

// vim: et:ts=4:sw=4
