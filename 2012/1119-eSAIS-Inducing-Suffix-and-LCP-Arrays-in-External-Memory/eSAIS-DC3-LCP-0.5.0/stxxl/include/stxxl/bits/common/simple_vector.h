/***************************************************************************
 *  include/stxxl/bits/common/simple_vector.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2008, 2011 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_SIMPLE_VECTOR_HEADER
#define STXXL_SIMPLE_VECTOR_HEADER

#include <algorithm>
#include <stxxl/bits/noncopyable.h>
#include <stxxl/bits/common/types.h>


__STXXL_BEGIN_NAMESPACE

template <class _Tp /*, class _Alloc=__STL_DEFAULT_ALLOCATOR(_Tp) */>
class simple_vector : private noncopyable
{
public:
    typedef unsigned_type size_type;
    typedef _Tp value_type;
//  typedef simple_alloc<_Tp, _Alloc> _data_allocator;

protected:
    size_type _size;
    value_type * _array;

public:
    typedef value_type * iterator;
    typedef const value_type * const_iterator;
    typedef value_type & reference;
    typedef const value_type & const_reference;

    simple_vector(size_type sz) : _size(sz), _array(NULL)
    {
        //    _array = _data_allocator.allocate(sz);
        if (size() > 0)
            _array = new _Tp[size()];
    }
    void swap(simple_vector & obj)
    {
        std::swap(_size, obj._size);
        std::swap(_array, obj._array);
    }
    ~simple_vector()
    {
        //    _data_allocator.deallocate(_array,_size);
        delete[] _array;
    }
    iterator begin()
    {
        return _array;
    }
    const_iterator begin() const
    {
        return _array;
    }
    const_iterator cbegin() const
    {
        return begin();
    }
    iterator end()
    {
        return _array + _size;
    }
    const_iterator end() const
    {
        return _array + _size;
    }
    const_iterator cend() const
    {
        return end();
    }
    size_type size() const
    {
        return _size;
    }
    reference operator [] (size_type i)
    {
        return *(begin() + i);
    }
    const_reference operator [] (size_type i) const
    {
        return *(begin() + i);
    }
};
__STXXL_END_NAMESPACE

namespace std
{
    template <class Tp_>
    void swap(stxxl::simple_vector<Tp_> & a,
              stxxl::simple_vector<Tp_> & b)
    {
        a.swap(b);
    }
}

#endif // !STXXL_SIMPLE_VECTOR_HEADER
