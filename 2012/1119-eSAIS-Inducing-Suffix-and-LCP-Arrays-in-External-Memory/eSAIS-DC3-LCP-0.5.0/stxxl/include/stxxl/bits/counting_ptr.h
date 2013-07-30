/***************************************************************************
 *  include/stxxl/bits/counting_ptr.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2012 Timo Bingmann
 *  
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_COUNTING_PTR_H
#define STXXL_COUNTING_PTR_H

#include <stxxl/types>

__STXXL_BEGIN_NAMESPACE

/**
 * \brief High-performance smart pointer used as a wrapping reference counting
 * pointer.
 *
 * This smart pointer class requires two functions in the template type: void
 * inc_ref() and void dec_ref(). These must increment and decrement a reference
 * counter inside the template object. When initialized, the type must have
 * reference count zero. It is _not_ immediately called with add_ref(). Each new
 * object referencing the data calls add_ref() and each destroying holder calls
 * del_ref(). When the data object determines that it's internal counter is
 * zero, then it must destroy itself.
 */
template<class Type>
class counting_ptr
{
public:
    /// contained type
    typedef Type element_type;

private:
    /// the pointer to the currently referenced object
    Type* m_ptr;

public:

    /// default constructor: contains a NULL pointer.
    counting_ptr() : m_ptr(NULL)
    {
    }
    
    /// constructor with pointer: initializes reference to ptr.
    counting_ptr(Type* ptr) : m_ptr(ptr)
    {
        if (m_ptr) m_ptr->inc_ref();
    }

    /// destructor: decrements reference counter in ptr.
    ~counting_ptr()
    {
        if (m_ptr) m_ptr->dec_ref();
    }

    /// assignment operator: dereference current object and acquire reference on new one.
    counting_ptr& operator=(const counting_ptr& b)
    {
        if (m_ptr != b.m_ptr)
        {
            if (m_ptr) m_ptr->dec_ref();
            m_ptr = b.m_ptr;
            if (m_ptr) m_ptr->inc_ref();
        }
        return *this;
    }

    /// assignment to pointer: dereference current and acquire reference to new ptr.
    counting_ptr& operator=(Type* ptr)
    {
        if (m_ptr != ptr)
        {
            if (m_ptr) m_ptr->dec_ref();
            m_ptr = ptr;
            if (m_ptr) m_ptr->inc_ref();
        }
        return *this;
    }

    /// return the enclosed pointer.
    Type* get() const
    {
        return m_ptr;
    }

    /// return the enclosed object as reference.
    Type& operator*() const
    {
        assert( m_ptr != NULL );
        return *m_ptr;
    }

    /// return the enclosed pointer.
    Type* operator->() const
    {
        assert( m_ptr != NULL );
        return m_ptr;
    }

    /// return true if the pointer is the NULL pointer
    bool operator!() const
    {
        return (m_ptr == NULL);
    }

    /// swap enclosed object with another counting pointer (no reference counts need change)
    void swap(counting_ptr& b)
    {
        std::swap(m_ptr, b.m_ptr);
    }
};

template<class A, class B> inline bool operator==(counting_ptr<A> & a, counting_ptr<B> & b)
{
    return (a.get() == b.get());
}

template<class A, class B> inline bool operator!=(counting_ptr<A> & a, counting_ptr<B> & b)
{
    return (a.get() != b.get());
}

template<class A> void swap(counting_ptr<A> & a1, counting_ptr<A> & a2)
{
    a1.swap(a2);
}

__STXXL_END_NAMESPACE

#endif // STXXL_COUNTING_PTR_H
