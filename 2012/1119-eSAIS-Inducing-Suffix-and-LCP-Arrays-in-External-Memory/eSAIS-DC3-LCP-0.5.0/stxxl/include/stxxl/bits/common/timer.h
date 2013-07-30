/***************************************************************************
 *  include/stxxl/bits/common/timer.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002, 2005 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2007-2009 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *  Copyright (C) 2008 Johannes Singler <singler@ira.uka.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_TIMER_HEADER
#define STXXL_TIMER_HEADER

#ifdef STXXL_BOOST_TIMESTAMP
 #include <boost/date_time/posix_time/posix_time.hpp>
 #include <cmath>
#else
 #include <ctime>
 #include <sys/time.h>
#endif

#include <stxxl/bits/namespace.h>


__STXXL_BEGIN_NAMESPACE

//! \brief Returns number of seconds since the epoch, high resolution.
inline double
timestamp()
{
#ifdef STXXL_BOOST_TIMESTAMP
    boost::posix_time::ptime MyTime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration Duration =
        MyTime - boost::posix_time::time_from_string("1970-01-01 00:00:00.000");
    double sec = double(Duration.hours()) * 3600. +
                 double(Duration.minutes()) * 60. +
                 double(Duration.seconds()) +
                 double(Duration.fractional_seconds()) / (pow(10., Duration.num_fractional_digits()));
    return sec;
#else
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return double(tp.tv_sec) + tp.tv_usec / 1000000.;
#endif
}

class timer
{
    bool running;
    double accumulated;
    double last_clock;
    inline double timestamp();

public:
    inline timer(bool start_immediately = false);
    inline void start();
    inline void stop();
    inline void reset();
    inline double seconds();
    inline double mseconds();
    inline double useconds();
};

timer::timer(bool start_immediately) : running(false), accumulated(0.)
{
    if (start_immediately)
        start();
}

double timer::timestamp()
{
    return stxxl::timestamp();
}

void timer::start()
{
    running = true;
    last_clock = timestamp();
}

void timer::stop()
{
    running = false;
    accumulated += timestamp() - last_clock;
}

void timer::reset()
{
    accumulated = 0.;
    last_clock = timestamp();
}

double timer::mseconds()
{
    if (running)
        return (accumulated + timestamp() - last_clock) * 1000.;

    return (accumulated * 1000.);
}

double timer::useconds()
{
    if (running)
        return (accumulated + timestamp() - last_clock) * 1000000.;

    return (accumulated * 1000000.);
}

double timer::seconds()
{
    if (running)
        return (accumulated + timestamp() - last_clock);

    return (accumulated);
}

__STXXL_END_NAMESPACE

#endif // !STXXL_TIMER_HEADER
// vim: et:ts=4:sw=4
