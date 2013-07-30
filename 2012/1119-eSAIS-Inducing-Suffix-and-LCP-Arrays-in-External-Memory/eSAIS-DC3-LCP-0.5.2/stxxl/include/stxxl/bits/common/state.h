/***************************************************************************
 *  include/stxxl/bits/common/state.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2008 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_STATE_HEADER
#define STXXL_STATE_HEADER

#ifdef STXXL_BOOST_THREADS
 #include <boost/thread/mutex.hpp>
 #include <boost/thread/condition.hpp>
#else
 #include <pthread.h>
#endif

#include <stxxl/bits/noncopyable.h>
#include <stxxl/bits/common/error_handling.h>


__STXXL_BEGIN_NAMESPACE

template <typename Tp = int>
class state : private noncopyable
{
    typedef Tp value_type;

#ifdef STXXL_BOOST_THREADS
    boost::mutex mutex;
    boost::condition cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
    value_type _state;

public:
    state(value_type s) : _state(s)
    {
#ifndef STXXL_BOOST_THREADS
        check_pthread_call(pthread_mutex_init(&mutex, NULL));
        check_pthread_call(pthread_cond_init(&cond, NULL));
#endif
    }

    ~state()
    {
#ifndef STXXL_BOOST_THREADS
        int res = pthread_mutex_trylock(&mutex);

        if (res == 0 || res == EBUSY) {
            check_pthread_call(pthread_mutex_unlock(&mutex));
        } else
            stxxl_function_error(resource_error);
        check_pthread_call(pthread_mutex_destroy(&mutex));
        check_pthread_call(pthread_cond_destroy(&cond));
#endif
    }

    void set_to(value_type new_state)
    {
#ifdef STXXL_BOOST_THREADS
        boost::mutex::scoped_lock Lock(mutex);
        _state = new_state;
        Lock.unlock();
        cond.notify_all();
#else
        check_pthread_call(pthread_mutex_lock(&mutex));
        _state = new_state;
        check_pthread_call(pthread_mutex_unlock(&mutex));
        check_pthread_call(pthread_cond_broadcast(&cond));
#endif
    }

    void wait_for(value_type needed_state)
    {
#ifdef STXXL_BOOST_THREADS
        boost::mutex::scoped_lock Lock(mutex);
        while (needed_state != _state)
            cond.wait(Lock);

#else
        check_pthread_call(pthread_mutex_lock(&mutex));
        while (needed_state != _state)
            check_pthread_call(pthread_cond_wait(&cond, &mutex));

        check_pthread_call(pthread_mutex_unlock(&mutex));
#endif
    }

    value_type operator () ()
    {
#ifdef STXXL_BOOST_THREADS
        boost::mutex::scoped_lock Lock(mutex);
        return _state;
#else
        value_type res;
        check_pthread_call(pthread_mutex_lock(&mutex));
        res = _state;
        check_pthread_call(pthread_mutex_unlock(&mutex));
        return res;
#endif
    }
};

__STXXL_END_NAMESPACE

#endif // !STXXL_STATE_HEADER
