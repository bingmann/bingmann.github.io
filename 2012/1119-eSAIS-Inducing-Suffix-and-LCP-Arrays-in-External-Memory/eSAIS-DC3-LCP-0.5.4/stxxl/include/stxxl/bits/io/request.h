/***************************************************************************
 *  include/stxxl/bits/io/request.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2008 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *  Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_IO_REQUEST_HEADER
#define STXXL_IO_REQUEST_HEADER

#include <cassert>

#include <stxxl/bits/namespace.h>
#include <stxxl/bits/io/request_interface.h>
#include <stxxl/bits/common/counting_ptr.h>
#include <stxxl/bits/common/exceptions.h>
#include <stxxl/bits/io/completion_handler.h>
#include <stxxl/bits/compat/unique_ptr.h>
#include <stxxl/bits/verbose.h>


STXXL_BEGIN_NAMESPACE

//! \addtogroup iolayer
//! \{

#define BLOCK_ALIGN 4096

class file;

//! Request with basic properties like file and offset.
class request : virtual public request_interface, public atomic_counted_object
{
protected:
    completion_handler on_complete;
    compat_unique_ptr<stxxl::io_error>::result error;

protected:
    file* file_;
    void* buffer;
    offset_type offset;
    size_type bytes;
    request_type type;

    void completed();

public:
    request(const completion_handler& on_compl,
            file* file__,
            void* buffer_,
            offset_type offset_,
            size_type bytes_,
            request_type type_);

    virtual ~request();

    file * get_file() const { return file_; }
    void * get_buffer() const { return buffer; }
    offset_type get_offset() const { return offset; }
    size_type get_size() const { return bytes; }
    request_type get_type() const { return type; }

    void check_alignment() const;

    std::ostream & print(std::ostream& out) const;

    //! Inform the request object that an error occurred during the I/O
    //! execution.
    void error_occured(const char* msg)
    {
        error.reset(new stxxl::io_error(msg));
    }

    //! Inform the request object that an error occurred during the I/O
    //! execution.
    void error_occured(const std::string& msg)
    {
        error.reset(new stxxl::io_error(msg));
    }

    //! Rises an exception if there were error with the I/O.
    void check_errors() throw (stxxl::io_error)
    {
        if (error.get())
            throw *(error.get());
    }

protected:
    void check_nref(bool after = false)
    {
        if (get_reference_count() < 2)
            check_nref_failed(after);
    }

private:
    void check_nref_failed(bool after);
};

inline std::ostream& operator << (std::ostream& out, const request& req)
{
    return req.print(out);
}

//! A reference counting pointer for \c request.
typedef counting_ptr<request> request_ptr;

//! \}

STXXL_END_NAMESPACE

#endif // !STXXL_IO_REQUEST_HEADER
// vim: et:ts=4:sw=4
