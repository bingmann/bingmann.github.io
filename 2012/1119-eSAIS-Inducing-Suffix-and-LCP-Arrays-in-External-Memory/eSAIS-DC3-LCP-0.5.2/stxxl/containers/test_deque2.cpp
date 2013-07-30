/***************************************************************************
 *  containers/test_deque2.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2012 Timo Bingmann <bingmann@kit.edu>
 *  based on containers/test_deque.cpp
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#include <iterator>
#include <stxxl/deque2>

int main(int argc, char * argv[])
{
    if (argc != 2) {
        STXXL_MSG("Usage: " << argv[0] << " #ops");
        return -1;
    }

    srand(2397423);
    stxxl::deque2<int> XXLDeque;
    std::deque<int> STDDeque;

    stxxl::int64 ops = stxxl::atoint64(argv[1]);
    for (stxxl::int64 i = 0; i < ops; ++i)
    {
        unsigned curOP = rand() % 5;
        unsigned value = rand();
        switch (curOP)
        {
        case 0:
        case 1:
            XXLDeque.push_front(value);
            STDDeque.push_front(value);
            break;
        case 2:
            XXLDeque.push_back(value);
            STDDeque.push_back(value);
            break;
        case 3:
            if (!XXLDeque.empty())
            {
                XXLDeque.pop_front();
                STDDeque.pop_front();
            }
            break;
        case 4:
            if (!XXLDeque.empty())
            {
                XXLDeque.pop_back();
                STDDeque.pop_back();
            }
            break;
        }

        assert(XXLDeque.empty() == STDDeque.empty());
        assert(XXLDeque.size() == STDDeque.size());

        if (XXLDeque.size() > 0)
        {
            assert(XXLDeque.back() == STDDeque.back());
            assert(XXLDeque.front() == STDDeque.front());
        }

        if (!(i % 100000))
        {
            std::cout << "Complete check of deque (size " << XXLDeque.size() << ")\n";
            stxxl::deque2<int>::stream stream = XXLDeque.get_stream();
            std::deque<int>::const_iterator b = STDDeque.begin();

            while ( !stream.empty() )
            {
                assert( b != STDDeque.end() );
                assert( *stream == *b );
                ++stream; ++b;
            }

            assert( b == STDDeque.end() );
        }

        if (!(i % 100000))
        {
            std::cout << "Complete check of reverse deque (size " << XXLDeque.size() << ")\n";
            stxxl::deque2<int>::reverse_stream stream = XXLDeque.get_reverse_stream();
            std::deque<int>::const_reverse_iterator b = STDDeque.rbegin();

            while ( !stream.empty() )
            {
                assert( b != STDDeque.rend() );
                assert( *stream == *b );
                ++stream; ++b;
            }

            assert( b == STDDeque.rend() );
        }
    }

    return 0;
}
