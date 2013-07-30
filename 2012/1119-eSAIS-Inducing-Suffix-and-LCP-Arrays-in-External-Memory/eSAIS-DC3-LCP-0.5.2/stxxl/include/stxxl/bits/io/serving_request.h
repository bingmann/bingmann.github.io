/***************************************************************************
 *  include/stxxl/bits/io/serving_request.h
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

#ifndef STXXL_IO__SERVING_REQUEST_H_
#define STXXL_IO__SERVING_REQUEST_H_

#include <stxxl/bits/io/request_with_state.h>


__STXXL_BEGIN_NAMESPACE

//! \addtogroup iolayer
//! \{

//! \brief Request which serves an I/O by calling the synchronous routine of the file.
class serving_request : public request_with_state
{
    template <class base_file_type>
    friend class fileperblock_file;

public:
    serving_request(
        const completion_handler & on_cmpl,
        file * f,
        void * buf,
        offset_type off,
        size_type b,
        request_type t);

protected:
    void serve();
    void completed();

public:
    const char * io_type() const;
};

//! \}

__STXXL_END_NAMESPACE

#endif // !STXXL_IO__SERVING_REQUEST_H_
// vim: et:ts=4:sw=4
